/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/nonspd.h"
#include "lib/smbios.h"
#include "puff.h"

struct memory_cb puff_memory_cb = {
	.dimm_count		= smbios_dimm_count,
	.dimm_speed		= smbios_dimm_speed,
	.nonspd_mem_info	= &spd_set_nonspd_info,
};
