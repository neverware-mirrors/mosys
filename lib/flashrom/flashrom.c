/*
 * Copyright 2012, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * flashrom.c: flashrom wrappers
 */

#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/flashrom.h"
#include "lib/math.h"

#define MAX_ARRAY_SIZE 256

/*
 * do_cmd - Execute a command
 *
 * @cmd:	Command to execute.
 * @argv:	Arguments. By convention, argv[0] is cmd.
 *
 * returns -1 to indicate error, 0 to indicate success
 */
static int do_cmd(const char *cmd, char *const *argv)
{
	int rc = 0;
	int pid = -1;
	int status = 0;
	int i;
	int null_fd;

	null_fd = open("/dev/null", O_WRONLY);
	if (null_fd < 0)
		return -1;

	if (argv != NULL) {
		for (i = 0; argv[i] != NULL; i++) {
			lprintf(LOG_DEBUG, "%s ", argv[i]);
		}
		lprintf(LOG_DEBUG, "\n");
	}

	if ((pid = fork()) < 0) {
		lprintf(LOG_ERR, "fork() error\n");
		rc = -1;
	} else if (pid == 0) { /* child */
		dup2(null_fd, fileno(stdin));
		dup2(null_fd, fileno(stdout));
		dup2(null_fd, fileno(stderr));

		execvp(cmd, argv);

		lperror(LOG_ERR, "%s: Failed to run %s", __func__, cmd);
		rc = -1;
	} else { /* parent */
		if (waitpid(pid, &status, 0) > 0) {
			if (WIFEXITED(status)) {
				if (WEXITSTATUS(status) != 0) {
					lprintf(LOG_ERR,
						"Child process returned %d.\n",
						WEXITSTATUS(status));
					rc = -1;
				}
			} else {
				lprintf(LOG_ERR, "Child process did not exit "
						"normally.\n");
				rc = -1;
			}
		} else {
			lprintf(LOG_ERR, "waitpid() returned error.\n");
			rc = -1;
		}
	}

	close(null_fd);
	return rc;
}

/* TODO: add arbitrary range support */
int flashrom_read(uint8_t *buf, size_t size, const char *region)
{
	int fd, rc = -1;
	char filename[] = "flashrom_XXXXXX";
	char full_filename[PATH_MAX];
	char *args[MAX_ARRAY_SIZE];
	struct stat s;
	int i = 0;

	args[i++] = strdup("flashrom");
	args[i++] = strdup("-p");
	args[i++] = strdup("host");

	strcpy(full_filename, "/tmp/");
	strcat(full_filename, filename);
	if (mkstemp(full_filename) == -1) {
		lperror(LOG_DEBUG,
			"Unable to make temporary file for flashrom");
		goto flashrom_read_exit_0;
	}

	if (region) {
		args[i++] = strdup("-i");
		args[i++] = strdup(region);
	}
	args[i++] = strdup("-r");
	args[i++] = strdup(full_filename);
	args[i++] = NULL;

	if (do_cmd("flashrom", args) < 0)
		goto flashrom_read_exit_1;

	fd = open(full_filename, O_RDONLY);
	if (fstat(fd, &s) < 0) {
		lprintf(LOG_DEBUG, "%s: Cannot stat %s\n", __func__, full_filename);
		goto flashrom_read_exit_1;
	}

	if (s.st_size != size) {
		lprintf(LOG_DEBUG, "%s: Size of image: %jd, expected %zu\n",
		                   __func__, (intmax_t)s.st_size, size);
		goto flashrom_read_exit_1;
	}

	if (read(fd, buf, size) != size) {
		lperror(LOG_DEBUG, "%s: Unable to read image\n", full_filename);
		goto flashrom_read_exit_1;
	}

	rc = 0;
flashrom_read_exit_1:
	for (i = 0; args[i] != NULL; i++)
		free(args[i]);
flashrom_read_exit_0:
	unlink(full_filename);
	return rc;
}

int flashrom_read_by_name(uint8_t **buf, const char *region)
{
	int fd, rc = -1;
	struct stat s;
	char filename[] = "flashrom_XXXXXX";
	char full_filename[PATH_MAX];
	char *args[MAX_ARRAY_SIZE];
	char region_file[MAX_ARRAY_SIZE];
	int i = 0;

	if (!region)
		goto flashrom_read_exit_0;

	args[i++] = strdup("flashrom");
	args[i++] = strdup("-p");
	args[i++] = strdup("host");

	strcpy(full_filename, "/tmp/");
	strcat(full_filename, filename);
	if (mkstemp(full_filename) == -1) {
		lperror(LOG_DEBUG,
			"Unable to make temporary file for flashrom");
		goto flashrom_read_exit_1;
	}

	args[i++] = strdup("-i");
	strcpy(region_file, region);
	strcat(region_file, ":");
	strcat(region_file, full_filename);
	args[i++] = strdup(region_file);
	args[i++] = strdup("-r");
	args[i++] = NULL;

	if (do_cmd("flashrom", args) < 0) {
		lprintf(LOG_DEBUG, "Unable to read region \"%s\"\n", region);
		goto flashrom_read_exit_2;
	}

	fd = open(full_filename, O_RDONLY);
	if (fstat(fd, &s) < 0) {
		lprintf(LOG_DEBUG, "%s: Cannot stat %s\n",__func__, full_filename);
		goto flashrom_read_exit_2;
	}

	*buf = mosys_malloc(s.st_size);
	if (read(fd, *buf, s.st_size) < 0) {
		lperror(LOG_DEBUG, "%s: Unable to read image", full_filename);
		free(*buf);
		goto flashrom_read_exit_2;
	}

	rc = s.st_size;
flashrom_read_exit_2:
	for (i = 0; args[i] != NULL; i++)
	  free(args[i]);
flashrom_read_exit_1:
	unlink(full_filename);
flashrom_read_exit_0:
	return rc;
}

int flashrom_write_by_name(size_t size, uint8_t *buf, const char *region)
{
	int fd, written, rc = -1;
	char filename[] = "flashrom_XXXXXX";
	char full_filename[PATH_MAX];
	char region_file[MAX_ARRAY_SIZE];
	char *args[MAX_ARRAY_SIZE];
	int i = 0;

	if (!region)
		goto flashrom_write_exit_0;

	args[i++] = strdup("flashrom");
	args[i++] = strdup("-p");
	args[i++] = strdup("host");

	strcpy(full_filename, "/tmp/");
	strcat(full_filename, filename);
	if (mkstemp(full_filename) == -1) {
		lperror(LOG_DEBUG,
			"Unable to make temporary file for flashrom");
		goto flashrom_write_exit_0;
	}

	fd = open(full_filename, O_WRONLY);
	if (fd < 0) {
		lprintf(LOG_DEBUG, "%s: Couldn't open %s: %s\n", __func__,
			full_filename, strerror(errno));
		goto flashrom_write_exit_0;
	}
	written = write(fd, buf, size);
	if (written < 0) {
		lprintf(LOG_DEBUG, "%s: Couldn't write to %s\n", __func__,
			full_filename);
		goto flashrom_write_exit_0;
	}
	if (written != size) {
		lprintf(LOG_DEBUG, "%s: Incomplete write to %s\n", __func__,
			full_filename);
		goto flashrom_write_exit_0;
	}

	args[i++] = strdup("-i");
	strcpy(region_file, region);
	strcat(region_file, ":");
	strcat(region_file, full_filename);
	args[i++] = strdup(region_file);
	args[i++] = strdup("-w");
	args[i++] = strdup("--fast-verify");
	args[i++] = NULL;

	if (do_cmd("flashrom", args) < 0) {
		lprintf(LOG_DEBUG, "Unable to write region \"%s\"\n", region);
		goto flashrom_write_exit_1;
	}

	rc = written;

flashrom_write_exit_1:
	for (i = 0; args[i] != NULL; i++)
		free(args[i]);
flashrom_write_exit_0:
	unlink(full_filename);
	return rc;
}

int flashrom_read_host_firmware_region(struct platform_intf *intf,
							uint8_t **buf)
{
	int rc = -1;
	const char *regions[] = { "COREBOOT", "BOOT_STUB" };

	for (int i = 0; i < ARRAY_SIZE(regions); i++) {
		rc = flashrom_read_by_name(buf, regions[i]);
		if (rc > 0)
			break;
	}

	return rc;
}
