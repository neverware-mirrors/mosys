/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/fdt.h"
#include "lib/nonspd.h"

#include "mosys/platform.h"

#include "trogdor.h"

static int trogdor_dimm_count;
static const struct nonspd_mem_info *trogdor_mem_info;

/* read RAM code and fill in values needed by memory commands */
static int read_ram_code(struct platform_intf *intf)
{
	/* TODO(b/148607682): Add memory info */
	return -1;
}

/*
 * dimm_count  -  return total number of dimm slots
 *
 * @intf:       platform interface
 *
 * returns dimm slot count
 */
static int dimm_count(struct platform_intf *intf)
{
	if (read_ram_code(intf) < 0)
		return -1;

	return trogdor_dimm_count;
}

static int get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	if (read_ram_code(intf) < 0)
		return -1;

	*info = trogdor_mem_info;
	return 0;
}

struct memory_cb trogdor_memory_cb = {
	.dimm_count		= &dimm_count,
	.nonspd_mem_info	= &get_mem_info,
};
