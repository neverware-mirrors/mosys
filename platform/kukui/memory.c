/*
 * Copyright 2019, Google Inc.
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

#include "lib/fdt.h"
#include "lib/nonspd.h"

#include "mosys/log.h"
#include "mosys/platform.h"

#include "kukui.h"

#define DIMM_COUNT 1

enum memory_config {
	HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G,
	MICRON_LPDDR4X_MT29VZZZAD8DQKSL_4G,
	MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G,
	SAMSUNG_LPDDR4X_KMDH6001DA_B422_4G,
	SAMSUNG_LPDDR4X_KMDP6001DA_B425_4G,
	SAMSUNG_LPDDR4X_KMDV6001DA_B620_4G,
	SAMSUNG_LPDDR4X_K4UBE3D4AM_MGCJ_4G,
	SANDISK_LPDDR4X_SDADA4CR_128G_4G,
	MEM_UNKNOWN,
};

static int get_memory_config(struct platform_intf *intf)
{
	uint32_t ram_code;

	if (fdt_get_ram_code(&ram_code) < 0) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}

	switch (ram_code) {
	case 0:
		return SAMSUNG_LPDDR4X_K4UBE3D4AM_MGCJ_4G;
	case 1:
		return HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G;
	case 2:
		return MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G;
	case 3:
		return SAMSUNG_LPDDR4X_KMDH6001DA_B422_4G;
	case 4:
		return SAMSUNG_LPDDR4X_KMDP6001DA_B425_4G;
	case 5:
		return MICRON_LPDDR4X_MT29VZZZAD8DQKSL_4G;
	case 6:
		return SAMSUNG_LPDDR4X_KMDV6001DA_B620_4G;
	case 7:
		return SANDISK_LPDDR4X_SDADA4CR_128G_4G;
	default:
		lprintf(LOG_ERR, "Not implemented\n");
	}

	return MEM_UNKNOWN;
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
	return DIMM_COUNT;
}

static int get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	switch (get_memory_config(intf)) {
	case SAMSUNG_LPDDR4X_K4UBE3D4AM_MGCJ_4G:
		*info = &samsung_lpddr4x_k4ube3d4am_mgcj;
		break;
	case HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G:
		*info = &hynix_lpddr4x_h9hcnnncpmalhr_nee;
		break;
	case MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G:
		*info = &micron_lpddr4x_mt53e1g32d4nq_046wte;
		break;
	case SAMSUNG_LPDDR4X_KMDH6001DA_B422_4G:
		*info = &samsung_lpddr4x_kmdh6001da_b422;
		break;
	case SAMSUNG_LPDDR4X_KMDP6001DA_B425_4G:
		*info = &samsung_lpddr4x_kmdp6001da_b425;
		break;
	case MICRON_LPDDR4X_MT29VZZZAD8DQKSL_4G:
		*info = &micron_lpddr4x_mt29vzzzad8dqksl;
		break;
	case SAMSUNG_LPDDR4X_KMDV6001DA_B620_4G:
		*info = &samsung_lpddr4x_kmdv6001da_b620;
		break;
	case SANDISK_LPDDR4X_SDADA4CR_128G_4G:
		*info = &sandisk_lpddr4x_sdada4cr_128g;
		break;
	default:
		return -1;
	}
	return 0;
}

struct memory_cb kukui_memory_cb = {
	.dimm_count		= dimm_count,
	.nonspd_mem_info	= get_mem_info,
};
