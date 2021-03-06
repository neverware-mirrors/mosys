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

#include <string.h>

#include "lib/fdt.h"
#include "lib/nonspd.h"

#include "mosys/log.h"
#include "mosys/platform.h"

#include "pinky.h"

static int pinky_dimm_count;
static const struct nonspd_mem_info *pinky_mem_info;

/* read RAM code and fill in values needed by memory commands */
static int read_ram_code(struct platform_intf *intf)
{
	static int done = 0, ret = 0;
	uint32_t ram_code;

	if (done)
		return ret;

	if (fdt_get_ram_code(&ram_code) < 0) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}

	if (!strcmp(intf->name, "Mickey")) {
		switch (ram_code) {
		case 0:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_lpddr3_k3qf2f20em_agce;
			break;
		case 3:
			pinky_dimm_count = 1;
			pinky_mem_info = &hynix_lpddr3_h9ccnnnbptblbr_nud;
			break;
		case 7:
			pinky_dimm_count = 2;
			pinky_mem_info = &elpida_lpddr3_edfa164a2ma_jd_f;
			break;
		case 17:
			pinky_dimm_count = 2;
			pinky_mem_info = &micron_lpddr3_mt52l256m64d2pp_107wtb;
			break;
		case 19:
			pinky_dimm_count = 1;
			pinky_mem_info = &hynix_lpddr3_h9ccnnnbktmlbr_ntd;
			break;
		default:
			ret = -1;
			break;
		}
	} else {
		switch (ram_code) {
		case 0:
			pinky_dimm_count = 4;
			pinky_mem_info = &samsung_k4b4g1646e;
			break;
		case 2:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_lpddr3_k4e6e304eb_egce;
			break;
		case 4:
			pinky_dimm_count = 4;
			pinky_mem_info = &samsung_ddr3l_k4b4g1646d_byk0;
			break;
		case 5:
			pinky_dimm_count = 4;
			pinky_mem_info = &hynix_ddr3l_h5tc4g63cfr_pba;
			break;
		case 6:
			pinky_dimm_count = 4;
			pinky_mem_info = &samsung_ddr3l_k4b4g1646q_hyk0;
			break;
		case 0x0a:
			pinky_dimm_count = 4;
			pinky_mem_info = &nanya_ddr3l_nt5cc256m16dp_di;
			break;
		case 0x0c:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_lpddr3_k4e8e324eb_egcf;
			break;
		case 0x0d:
			pinky_dimm_count = 4;
			pinky_mem_info = &hynix_ddr3l_h5tc4g63afr_pba;
			break;
		case 0x0e:
			pinky_dimm_count = 4;
			pinky_mem_info = &samsung_ddr3l_k4b8g1646q_myk0;
			break;
		case 0x0f:
			pinky_dimm_count = 4;
			pinky_mem_info = &hynix_ddr3l_h5tc8g63amr_pba;
			break;
		case 0x10:
			pinky_dimm_count = 4;
			pinky_mem_info = &samsung_k4b4g1646e;
			break;
		default:
			ret = -1;
			break;
		}
	}

	done = 1;
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
	if (read_ram_code(intf) < 0)
		return -1;

	return pinky_dimm_count;
}

static int get_mem_info(struct platform_intf *intf, int dimm,
			const struct nonspd_mem_info **info)
{
	if (read_ram_code(intf) < 0)
		return -1;

	*info = pinky_mem_info;
	return 0;
}

struct memory_cb pinky_memory_cb = {
	.dimm_count		= dimm_count,
	.nonspd_mem_info	= &get_mem_info,
};
