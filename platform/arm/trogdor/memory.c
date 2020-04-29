/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/fdt.h"
#include "lib/nonspd.h"

#include "mosys/platform.h"

#include "trogdor.h"

/* read RAM code and fill in values needed by memory commands */
static int read_ram_code(struct platform_intf *intf, int *dimm_count,
			 const struct nonspd_mem_info **mem_info)
{
	static int done;
	static int ret;
	static int s_dimm_count;
	static const struct nonspd_mem_info *s_mem_info;

	uint32_t ram_code;

	if (!done) {
		if (fdt_get_ram_code(&ram_code) < 0) {
			lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
			return -1;
		}

		switch (ram_code) {
		case 2:
			s_dimm_count = 1;
			s_mem_info = &micron_lpddr4x_mt53e1g32d2np_046wta;
			break;

		case 3:
			s_dimm_count = 1;
			s_mem_info = &micron_lpddr4x_mt53e2g32d4nq_046wta;
			break;

		default:
			lprintf(LOG_ERR, "Unknown ram code.\n");
			ret = -1;
			break;
		}

		done = 1;
	}

	return ret;
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
	int dimm_count;
	const struct nonspd_mem_info *mem_info;

	if (read_ram_code(intf, &dimm_count, &mem_info) < 0)
		return -1;

	return dimm_count;
}

static int get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	int dimm_count;
	const struct nonspd_mem_info *mem_info;

	if (read_ram_code(intf, &dimm_count, &mem_info) < 0)
		return -1;

	*info = mem_info;
	return 0;
}

struct memory_cb trogdor_memory_cb = {
	.dimm_count = &dimm_count,
	.nonspd_mem_info = &get_mem_info,
};
