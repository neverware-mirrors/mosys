/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <chromeos/libminijail.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/securebits.h>
#include <sys/capability.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "mosys/cli.h"

#define MOSYS_CAPS CAP_TO_MASK(CAP_SYS_RAWIO) | CAP_TO_MASK(CAP_SYS_ADMIN)
#define MOSYS_SECCOMP_POLICY_PATH "/usr/share/policy/mosys-seccomp.policy"

static void setup_jail(void)
{
	int fd;
	struct minijail *j;

	/* Don't setup the minijail if we aren't running as root. */
	if (geteuid() > 0)
		return;

	j = minijail_new();

	/* TODO(crbug.com/1082531): see if we can remove this ifdef. */
#ifndef CONFIG_PLATFORM_ARCH_ARMEL
	minijail_use_caps(j, MOSYS_CAPS);
	minijail_set_ambient_caps(j);

	/* Don't set securebits because this may be inside another minijail.
	   See b/112030238. */
	minijail_skip_setting_securebits(j, SECURE_ALL_BITS | SECURE_ALL_LOCKS);
	minijail_remount_proc_readonly(j);
	minijail_namespace_net(j);
#endif

	minijail_no_new_privs(j);
	minijail_set_seccomp_filter_tsync(j);

	/* This file is expected to not be present in initramfs.  Only
	   set the seccomp policy when it exists. */
	fd = open(MOSYS_SECCOMP_POLICY_PATH, O_RDONLY);
	if (fd >= 0) {
		minijail_parse_seccomp_filters_from_fd(j, fd);
		minijail_use_seccomp_filter(j);
		close(fd);
	}

	minijail_enter(j);
	minijail_destroy(j);
}

int main(int argc, char *argv[])
{
	setup_jail();
	return mosys_main(argc, argv);
}
