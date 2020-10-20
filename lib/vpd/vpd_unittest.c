/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cmocka.h>

#include "lib/file.h"
#include "lib/math.h"
#include "lib/vpd.h"
#include "mosys/log.h"

typeof(execve) __real_execve;
typeof(execve) __wrap_execve;
typeof(read_file) __real_read_file;
typeof(read_file) __wrap_read_file;

static struct {
	const char *name;
	const char *value;
} fake_vpd_values[] = {
	{ "customization_id", "fake_cid" },
	{ "whitelabel_tag", "fake_wl" },
	{ "rlz_brand_code", "ZZCR" },
};

int __wrap_execve(const char *prog, char *const argv[], char *const envp[])
{
	if (strcmp(prog, "/usr/sbin/vpd_get_value")) {
		return __real_execve(prog, argv, envp);
	}

	if (!argv[1])
		exit(1);

	for (size_t i = 0; i < ARRAY_SIZE(fake_vpd_values); i++) {
		if (!strcmp(fake_vpd_values[i].name, argv[1])) {
			printf("%s\n", fake_vpd_values[i].value);
			exit(0);
		}
	}

	/* vpd_get_value exits zero even when the value does not exist */
	exit(0);
	__builtin_unreachable();
}

#define SYSFS_PREFIX "/sys/firmware/vpd/ro/"
ssize_t __wrap_read_file(const char *path, char *buf, size_t buf_sz,
			 enum log_levels lvl)
{
	if (strncmp(path, SYSFS_PREFIX, strlen(SYSFS_PREFIX))) {
		return __real_read_file(path, buf, buf_sz, lvl);
	}

	const char *name = path + strlen(SYSFS_PREFIX);

	for (size_t i = 0; i < ARRAY_SIZE(fake_vpd_values); i++) {
		if (!strcmp(fake_vpd_values[i].name, name)) {
			size_t out_size = strlen(fake_vpd_values[i].value) + 1;

			if (out_size > buf_sz)
				return -1;

			memcpy(buf, fake_vpd_values[i].value, out_size);
			return out_size;
		}
	}

	return -1;
}

static void vpd_value_exists_test(void **state)
{
	assert_string_equal(vpd_get_value("customization_id"), "fake_cid");
	assert_string_equal(vpd_get_value("whitelabel_tag"), "fake_wl");
	assert_string_equal(vpd_get_value("rlz_brand_code"), "ZZCR");
}

static void vpd_value_not_exists_test(void **state)
{
	assert_null(vpd_get_value("does_not_exist"));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(vpd_value_exists_test),
		cmocka_unit_test(vpd_value_not_exists_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
