/* Copyright 2012, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
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
 */

#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>

// cmocka doesn't include some headers it uses, e.g. setjmp. Prevent
// clang-format from putting the headers in order, so it gets the above
// includes.
// clang-format off
#include <cmocka.h>
// clang-format on

#include "mosys/globals.h"
#include "mosys/list.h"
#include "mosys/platform.h"

#include "lib/file.h"

// Reads the root of the testdata directory into <buf>. Note that it is assumed
// <buf> is of size at least PATH_MAX.
static void get_testdata_root(char *buf)
{
	char *src = getenv("SRC");
	assert_non_null(src);

	snprintf(buf, PATH_MAX, "%s/unittests/testdata", src);
}

static void scanft_test(void **state)
{
	// The relevant directories for this test are structured as:
	//
	// ├── dir1
	// │   ├── dir2_symlink -> ../dir2
	// │   ├── needle0
	// │   └── subdir
	// │       ├── needle1
	// │       └── self_symlink -> ../subdir
	// └── dir2
	//     └── needle2
	//
	// The scan will start in "dir1", which contains symlinks to dir2.
	struct ll_node *list = NULL;
	char testdata_root[PATH_MAX];
	char root[PATH_MAX];

	get_testdata_root(testdata_root);

	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "scanft_test/dir1/");

	/* The first needle should be in given root. */
	assert_non_null(scanft(&list, root, /*filename=*/"needle0",
			       /*str=*/NULL,
			       /*maxdepth=*/0, /*symdepth=*/0));
	list_cleanup(&list);

	/* The second needle requires some basic recursion to find. */
	assert_null(scanft(&list, root, /*filename=*/"needle1", /*str=*/NULL,
			   /*maxdepth=*/0, /*symdepth=*/0));
	assert_non_null(scanft(&list, root, /*filename=*/"needle1",
			       /*str=*/NULL,
			       /*maxdepth=*/-1, /*symdepth=*/0));
	list_cleanup(&list);

	/* The third needle requires recursion and symlink
	 * handling to find. */
	assert_null(scanft(&list, root, /*filename=*/"needle2", /*str=*/NULL,
			   /*maxdepth=*/-1, /*symdepth=*/0));
	assert_non_null(scanft(&list, root, /*filename=*/"needle2",
			       /*str=*/NULL,
			       /*maxdepth=*/-1, /*symdepth=*/1));
	list_cleanup(&list);

	/*
	 * The fourth needle doesn't exist so scanft() should return NULL. A
	 * self-referencing directory symlink has been added to the hierarchy to
	 * test symlink depth handling.
	 */
	assert_null(scanft(&list, root, /*filename=*/"needle3", /*str=*/NULL,
			   /*maxdepth=*/-1, /*symdepth=*/100));
	list_cleanup(&list);
}

static void sysfs_lowest_smbus_test(void **state)
{
	char testdata_root[PATH_MAX];
	char root[PATH_MAX];

	get_testdata_root(testdata_root);

	/*
	 * Find "needle" in haystack of sysfs-style i2c-<bus> directories.
	 *
	 * test0/ : no needles, just hay
	 * test1/ : needle is i2c-0, hay is in i2c-1 and i2c-2
	 * test2/ : needle is i2c-1, hay is in i2c-0 and i2c-2
	 * test3/ : needle is i2c-2, hay is in i2c-0 and i2c-1
	 * test4/ : needles are in i2c-0 and i2c-2, hay is in i2c-1
	 */
	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "sysfs_lowest_smbus_test/test0/");
	assert_int_equal(-1, sysfs_lowest_smbus(root, "needle"));

	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "sysfs_lowest_smbus_test/test1/");
	assert_int_equal(0, sysfs_lowest_smbus(root, "needle"));

	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "sysfs_lowest_smbus_test/test2");
	assert_int_equal(1, sysfs_lowest_smbus(root, "needle"));

	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "sysfs_lowest_smbus_test/test3");
	assert_int_equal(2, sysfs_lowest_smbus(root, "needle"));

	/* test invalid path */
	snprintf(root, sizeof(root), "%s/%s", testdata_root,
		 "sysfs_lowest_smbus_test/nothing");
	assert_int_equal(-1, sysfs_lowest_smbus(root, "needle"));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
	    cmocka_unit_test(scanft_test),
	    cmocka_unit_test(sysfs_lowest_smbus_test),
	};

	return cmocka_run_group_tests(tests, /*group_setup=*/NULL,
				      /*group_teardown=*/NULL);
}
