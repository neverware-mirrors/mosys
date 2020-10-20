/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <limits.h>
#include <stdlib.h>

#include "lib/file.h"
#include "lib/vpd.h"
#include "mosys/alloc.h"
#include "mosys/log.h"

#define VPD_SYSFS_BASE "/sys/firmware/vpd/ro"

char *vpd_get_value(const char *name)
{
	char path[PATH_MAX];
	char result[VPD_MAX_VALUE_SIZE];

	snprintf(path, sizeof(path), VPD_SYSFS_BASE "/%s", name);
	if (read_file(path, result, sizeof(result), LOG_DEBUG) < 0)
		return NULL;

	return mosys_strdup(result);
}
