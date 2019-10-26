/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/memory.h"
#include "lib/spd.h"
#include "lib/smbios.h"

struct memory_cb cbfs_memory_cb = {
	.dimm_count	= smbios_dimm_count,
	.spd		= &memory_spd_cbfs,
	.dimm_speed	= smbios_dimm_speed,
};