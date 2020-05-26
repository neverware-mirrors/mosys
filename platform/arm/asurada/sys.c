/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "drivers/google/cros_ec.h"
#include "lib/fdt.h"
#include "mosys/platform.h"

#include "asurada.h"

struct sys_cb asurada_sys_cb = {
	.version		= cros_ec_board_version_str,
};
