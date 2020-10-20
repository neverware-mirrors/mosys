/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * VPD implementations for legacy platforms which don't support the
 * sysfs VPD driver (VPD not cached in CBMEM by firmware).
 *
 * Note: as of October 2020, the list of non-AUE platforms which
 * requires this is:
 * - Baytrail devices (Rambi)
 * - Braswell devices (Strago)
 * - Broadwell devices (Auron, Jecht)
 * - Veyron devices
 *
 * By today's published AUE dates, that means this code can be deleted
 * in late 2022.
 */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lib/vpd.h"
#include "mosys/alloc.h"
#include "mosys/log.h"

char *vpd_get_value(const char *name)
{
	char result[VPD_MAX_VALUE_SIZE] = { 0 };
	size_t result_len;
	size_t result_offset = 0;
	size_t bytes_to_read = sizeof(result) - 1;
	int wstatus;
	int pipefd[2];
	pid_t pid;
	const char *const argv[] = { "/usr/sbin/vpd_get_value", name, NULL };
	char *const env[] = { NULL };

	if (pipe(pipefd) < 0) {
		lprintf(LOG_ERR, "%s: pipe() failed: %s\n", __func__,
			strerror(errno));
		return NULL;
	}

	pid = fork();
	if (pid < 0) {
		lprintf(LOG_ERR, "%s: fork() failed: %s\n", __func__,
			strerror(errno));
		close(pipefd[1]);
		goto exit;
	}

	if (pid == 0) {
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
			lprintf(LOG_ERR, "%s: dup2() failed: %s\n", __func__,
				strerror(errno));
			exit(1);
		}

		execve(argv[0], (char * const *)argv, env);
		lprintf(LOG_ERR, "%s: cannot exec %s: %s\n", __func__, argv[0],
			strerror(errno));
		exit(1);
	}

	close(pipefd[1]);
	if (waitpid(pid, &wstatus, 0) < 0) {
		lprintf(LOG_ERR, "%s: waitpid() failed: %s\n", __func__,
			strerror(errno));
		goto exit;
	}

	if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) {
		lprintf(LOG_ERR, "%s: %s %s returned non-zero (wstatus=%d)\n",
			__func__, argv[0], argv[1], wstatus);
		goto exit;
	}

	while (bytes_to_read) {
		ssize_t read_rv = read(pipefd[0], result + result_offset,
				       bytes_to_read);

		if (read_rv < 0) {
			lprintf(LOG_ERR, "%s: read() failed: %s\n",
				__func__, strerror(errno));
			goto exit;
		} else if (read_rv == 0) {
			break;
		} else {
			result_offset += read_rv;
			bytes_to_read -= read_rv;
		}
	}

exit:
	close(pipefd[0]);
	result_len = strlen(result);

	if (result_len) {
		if (result[result_len - 1] == '\n')
			result[result_len - 1] = '\0';
		return mosys_strdup(result);
	}

	return NULL;
}
