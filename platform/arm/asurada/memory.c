/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/fdt.h"
#include "lib/nonspd.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "asurada.h"

#define DIMM_COUNT 1

/*
 * dimm_count  -  return total number of dimm slots
 *
 * @intf:       platform interface
 *
 * returns dimm slot count
 */
static int dimm_count(struct platform_intf *intf)
{
	return DIMM_COUNT;
}

static const struct nonspd_mem_info *asurada_dram[] = {
};

static int get_mem_info(struct platform_intf *intf, int dimm,
			const struct nonspd_mem_info **info)
{
	uint32_t ram_code;

	if (fdt_get_ram_code(&ram_code) < 0) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}
	if (ram_code >= ARRAY_SIZE(asurada_dram) || !asurada_dram[ram_code]) {
		lprintf(LOG_ERR, "Invalid RAM code: %d.\n", ram_code);
		return -1;
	}
	*info = asurada_dram[ram_code];
	return 0;
}

struct memory_cb asurada_memory_cb = {
	.dimm_count		= dimm_count,
	.nonspd_mem_info	= get_mem_info,
};
