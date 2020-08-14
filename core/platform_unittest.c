/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* These unit tests are designed to test probing from multiple
   platforms.  CONFIG_SINGLE_PLATFORM should not be enabled. */
#undef CONFIG_SINGLE_PLATFORM

#include <limits.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// cmocka doesn't include some headers it uses, e.g. setjmp. Prevent
// clang-format from putting the headers in order, so it gets the above
// includes.
// clang-format off
#include <cmocka.h>
// clang-format on

#include "mosys/globals.h"
#include "mosys/intf_list.h"
#include "mosys/platform.h"

/* Subcommands list used for testing which defines no commands */
static struct platform_cmd *no_subcommands[] = {
	NULL
};

/* Test probe functions, which records what they are called on */
static struct platform_intf *probe_called;

static int probe_never_match(struct platform_intf *intf)
{
	probe_called = intf;
	return 0;
}

static bool probe_mocked_match_enable;
static int probe_mocked_match(struct platform_intf *intf)
{
	probe_called = intf;
	return probe_mocked_match_enable;
}

static int probe_always_error(struct platform_intf *intf)
{
	probe_called = intf;
	return -1;
}

/* Test setup functions, which records what they are called on */
static struct platform_intf *setup_called;

static int setup_always_success(struct platform_intf *intf)
{
	setup_called = intf;
	return 0;
}

static int setup_always_error(struct platform_intf *intf)
{
	setup_called = intf;
	return -1;
}

/* Test destroy function, which records what it was called on */
static struct platform_intf *destroy_called;

static int destroy(struct platform_intf *intf)
{
	destroy_called = intf;
	return 0;
}

/* Test setup-post functions, which records what they are called on */
static struct platform_intf *setup_post_called;

static int setup_post_always_success(struct platform_intf *intf)
{
	setup_post_called = intf;
	return 0;
}

static int setup_post_always_error(struct platform_intf *intf)
{
	setup_post_called = intf;
	return -1;
}

/*
 * A minimal platform intf definition that should match by name, but
 * not call any probe, setup, destroy, or setup-post functions.
 */
static struct platform_intf minimal_intf;
REGISTER_PLATFORM(minimal_intf, "minimal");

/*
 * A more complete definition which should never match, but may probe,
 * setup, destroy, or setup-post without error.
 */
static struct platform_intf basic_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_never_match,
	.setup = &setup_always_success,
	.destroy = &destroy,
	.setup_post = &setup_post_always_success,
};
REGISTER_PLATFORM(basic_intf, "basic");

/*
 * Same as basic, but always errors during probing
 */
static struct platform_intf error_probe_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_always_error,
	.setup = &setup_always_success,
	.destroy = &destroy,
	.setup_post = &setup_post_always_success,
};
REGISTER_PLATFORM(error_probe_intf, "error_probe");

/*
 * Same as basic, but always errors during setup
 */
static struct platform_intf error_setup_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_never_match,
	.setup = &setup_always_error,
	.destroy = &destroy,
	.setup_post = &setup_post_always_success,
};
REGISTER_PLATFORM(error_setup_intf, "error_setup");

/*
 * Same as basic, but always errors during setup-post
 */
static struct platform_intf error_setup_post_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_never_match,
	.setup = &setup_always_success,
	.destroy = &destroy,
	.setup_post = &setup_post_always_error,
};
REGISTER_PLATFORM(error_setup_post_intf, "error_setup_post");

/*
 * Same as error_setup_post, but always errors during setup-post
 */
static struct platform_intf error_setup_post_no_destroy_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_never_match,
	.setup = &setup_always_success,
	.setup_post = &setup_post_always_error,
};
REGISTER_PLATFORM(error_setup_post_no_destroy_intf,
		  "error_setup_post_no_destroy");

/*
 * Same as basic, but matches conditionally during probing based on
 * the probe_mocked_match_enable variable.
 */
static struct platform_intf mocked_match_intf = {
	.type = PLATFORM_X86_64,
	.sub = no_subcommands,
	.probe = &probe_mocked_match,
	.setup = &setup_always_success,
	.destroy = &destroy,
	.setup_post = &setup_post_always_success,
};
REGISTER_PLATFORM(mocked_match_intf, "mocked_match");

static void find_minimal_by_name(void **state)
{
	assert_ptr_equal(mosys_platform_setup("minimal"), &minimal_intf);
	assert_null(probe_called);
	assert_null(setup_called);
	assert_null(destroy_called);
	assert_null(setup_post_called);
}

static void find_basic_by_name(void **state)
{
	assert_ptr_equal(mosys_platform_setup("basic"), &basic_intf);
	assert_null(probe_called);
	assert_ptr_equal(setup_called, &basic_intf);
	assert_null(destroy_called);
	assert_ptr_equal(setup_post_called, &basic_intf);

	setup_called = NULL;
	setup_post_called = NULL;
}

static void find_by_name_is_case_insensitive(void **state)
{
	assert_ptr_equal(mosys_platform_setup("BaSiC"), &basic_intf);
	assert_null(probe_called);
	assert_ptr_equal(setup_called, &basic_intf);
	assert_null(destroy_called);
	assert_ptr_equal(setup_post_called, &basic_intf);

	setup_called = NULL;
	setup_post_called = NULL;
}

static void find_bad_name_fails(void **state)
{
	assert_null(mosys_platform_setup("BAD~NAME"));
	assert_null(probe_called);
	assert_null(setup_called);
	assert_null(destroy_called);
	assert_null(setup_post_called);
}

static void setup_error_fails(void **state)
{
	assert_null(mosys_platform_setup("error_setup"));
	assert_null(probe_called);
	assert_ptr_equal(setup_called, &error_setup_intf);
	assert_null(destroy_called);
	assert_null(setup_post_called);

	setup_called = NULL;
	destroy_called = NULL;
}

static void setup_post_error_fails(void **state)
{
	assert_null(mosys_platform_setup("error_setup_post"));
	assert_null(probe_called);
	assert_ptr_equal(setup_called, &error_setup_post_intf);
	assert_ptr_equal(destroy_called, &error_setup_post_intf);
	assert_ptr_equal(setup_post_called, &error_setup_post_intf);

	setup_called = NULL;
	destroy_called = NULL;
	setup_post_called = NULL;
}

static void setup_post_error_fails_no_destroy(void **state)
{
	assert_null(mosys_platform_setup("error_setup_post_no_destroy"));
	assert_null(probe_called);
	assert_ptr_equal(setup_called, &error_setup_post_no_destroy_intf);
	assert_null(destroy_called);
	assert_ptr_equal(setup_post_called, &error_setup_post_no_destroy_intf);

	setup_called = NULL;
	setup_post_called = NULL;
}

static void probe_matches(void **state)
{
	probe_mocked_match_enable = true;
	assert_ptr_equal(mosys_platform_setup(NULL), &mocked_match_intf);
	assert_ptr_equal(probe_called, &mocked_match_intf);
	assert_ptr_equal(setup_called, &mocked_match_intf);
	assert_null(destroy_called);
	assert_ptr_equal(setup_post_called, &mocked_match_intf);

	probe_called = NULL;
	setup_called = NULL;
	setup_post_called = NULL;
	probe_mocked_match_enable = false;
}

static void probe_fails(void **state)
{
	assert_null(mosys_platform_setup(NULL));
	assert_non_null(probe_called);
	assert_null(setup_called);
	assert_null(destroy_called);
	assert_null(setup_post_called);

	probe_called = NULL;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(find_minimal_by_name),
		cmocka_unit_test(find_basic_by_name),
		cmocka_unit_test(find_by_name_is_case_insensitive),
		cmocka_unit_test(find_bad_name_fails),
		cmocka_unit_test(setup_error_fails),
		cmocka_unit_test(setup_post_error_fails),
		cmocka_unit_test(setup_post_error_fails_no_destroy),
		cmocka_unit_test(probe_matches),
		cmocka_unit_test(probe_fails),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
