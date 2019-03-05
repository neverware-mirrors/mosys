/*
 * Copyright 2014, Google Inc.
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

#include "oak.h"

#define OAK_DIMM_COUNT 2
#define KUKUI_DIMM_COUNT 1

enum memory_config {
	HYNIX_DDR3_H9CCNNN8GTMLAR_NUD_1G,
	HYNIX_DDR3_H9CCNNNBJTALAR_NUD_2G,
	HYNIX_DDR3_H9CCNNNBLTBLAR_NUD_2G,
	HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G,
	MICRON_DDR3_MT52L256M32D1PF_107WTB_1G,
	MICRON_DDR3_MT52L512M32D2PF_107WTB_2G,
	MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G,
	SAMSUNG_DDR3_K4E6E304EB_EGCF_2G,
	SAMSUNG_DDR3_K4E6E304EE_EGCE_2G,
	SAMSUNG_DDR3_K4E8E304EE_EGCE_1G,
	SAMSUNG_DDR3_K4E8E324EB_EGCF_1G,
	MEM_UNKNOWN,
};

static int oak_get_memory_config(struct platform_intf *intf)
{
	uint32_t ram_code;

	if (fdt_get_ram_code(&ram_code) < 0) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}

	switch (ram_code) {
	case 0:
		return HYNIX_DDR3_H9CCNNN8GTMLAR_NUD_1G;
	case 1:
		return SAMSUNG_DDR3_K4E8E304EE_EGCE_1G;
	case 2:
		return SAMSUNG_DDR3_K4E6E304EE_EGCE_2G;
	case 3:
		return HYNIX_DDR3_H9CCNNNBLTBLAR_NUD_2G;
	case 4:
		return SAMSUNG_DDR3_K4E6E304EB_EGCF_2G;
	case 5:
		return SAMSUNG_DDR3_K4E8E324EB_EGCF_1G;
	case 6:
		return MICRON_DDR3_MT52L512M32D2PF_107WTB_2G;
	case 7:
		return HYNIX_DDR3_H9CCNNNBJTALAR_NUD_2G;
	case 8:
		return MICRON_DDR3_MT52L256M32D1PF_107WTB_1G;

	default:
		lprintf(LOG_ERR, "Unable to determine memory configuration\n");
	}

	return MEM_UNKNOWN;
}

static int kukui_get_memory_config(struct platform_intf *intf)
{
	uint32_t ram_code;

	if (fdt_get_ram_code(&ram_code) < 0) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}

	switch (ram_code) {
	case 1:
		return HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G;
	case 2:
		return MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G;
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
static int oak_dimm_count(struct platform_intf *intf)
{
	return OAK_DIMM_COUNT;

}

static int kukui_dimm_count(struct platform_intf *intf)
{
	return KUKUI_DIMM_COUNT;
}

static int oak_get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	switch (oak_get_memory_config(intf)) {
	case HYNIX_DDR3_H9CCNNN8GTMLAR_NUD_1G:
		*info = &hynix_lpddr3_h9ccnnn8gtmlar_nud;
		break;
	case HYNIX_DDR3_H9CCNNNBJTALAR_NUD_2G:
		*info = &hynix_lpddr3_h9ccnnnbjtalar_nud;
		break;
	case HYNIX_DDR3_H9CCNNNBLTBLAR_NUD_2G:
		*info = &hynix_lpddr3_h9ccnnnbltblar_nud;
		break;
	case MICRON_DDR3_MT52L256M32D1PF_107WTB_1G:
		*info = &micron_lpddr3_mt52l256m32d1pf_107wtb;
		break;
	case MICRON_DDR3_MT52L512M32D2PF_107WTB_2G:
		*info = &micron_lpddr3_mt52l512m32d2pf_107wtb;
		break;
	case SAMSUNG_DDR3_K4E6E304EB_EGCF_2G:
		*info = &samsung_lpddr3_k4e6e304eb_egcf;
		break;
	case SAMSUNG_DDR3_K4E6E304EE_EGCE_2G:
		*info = &samsung_lpddr3_k4e6e304ee_egce;
		break;
	case SAMSUNG_DDR3_K4E8E304EE_EGCE_1G:
		*info = &samsung_lpddr3_k4e8e304ee_egce;
		break;
	case SAMSUNG_DDR3_K4E8E324EB_EGCF_1G:
		*info = &samsung_lpddr3_k4e8e324eb_egcf;
		break;
	default:
		return -1;
	}
	return 0;
}

static int kukui_get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	switch (kukui_get_memory_config(intf)) {
	case HYNIX_LPDDR4X_H9HCNNNCPMALHR_NEE_4G:
		*info = &hynix_lpddr4x_h9hcnnncpmalhr_nee;
		break;
	case MICRON_LPDDR4X_MT53E1G32D4NQ_046WTE_4G:
		*info = &micron_lpddr4x_mt53e1g32d4nq_046wte;
		break;
	default:
		return -1;
	}
	return 0;
}

struct memory_cb oak_memory_cb = {
	.dimm_count		= oak_dimm_count,
	.nonspd_mem_info	= &oak_get_mem_info,
};

struct memory_cb kukui_memory_cb = {
	.dimm_count		= kukui_dimm_count,
	.nonspd_mem_info	= &kukui_get_mem_info,
};