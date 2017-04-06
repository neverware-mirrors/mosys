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

#include "pinky.h"

const struct nonspd_mem_info samsung_2gbit_lpddr3_k4e8e304ee_egce = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'E', '8', 'E', '3', '0', '4', 'E', 'E', '-',
		  'E', 'G', 'C', 'E' },
};

const struct nonspd_mem_info hynix_2gbit_lpddr3_h9ccnnn8gtmlar_nud = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 8192,
        .num_ranks              = 1,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '9', 'C', 'C', 'N', 'N', 'N', '8', 'G', 'T', 'M', 'L',
                  'A', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info elpida_8gbit_lpddr3_f8132a3ma_gd_f = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_UNDEFINED,

        .module_size_mbits      = 8192,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0x2c, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0x2c, .lsb = 0x80 },

        .part_num               =
                { 'F', '8', '1', '3', '2', 'A', '3', 'M', 'A', '-', 'G', 'D',
                  '-', 'F',},
};

const struct nonspd_mem_info samsung_4gbit_lpddr3_k4e6e304ee_egce = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'E', '6', 'E', '3', '0', '4', 'E', 'E', '-',
		  'E', 'G', 'C', 'E' },
};

const struct nonspd_mem_info samsung_lpddr3_k4e6e304eb_egcf = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'E', '6', 'E', '3', '0', '4', 'E', 'B', '-',
		  'E', 'G', 'C', 'F' },
};

const struct nonspd_mem_info hynix_4gbit_lpddr3_h9ccnnnbjtmlar_nud = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 16384,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'J', 'T', 'M', 'L',
                  'A', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info elpida_16gbit_lpddr3_fa232a2ma_gc_f = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_UNDEFINED,

        .module_size_mbits      = 16384,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0x2c, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0x2c, .lsb = 0x80 },

        .part_num               =
                { 'F', 'A', '2', '3', '2', 'A', '2', 'M', 'A', '-', 'G', 'C',
                  '-', 'F',},
};

const struct nonspd_mem_info samsung_2gbit_lpddr3_k3qf2f20em_agce = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 8192,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xce, .lsb = 0x00 },
        .dram_mfg_id            = { .msb = 0xce, .lsb = 0x00 },

        .part_num               =
                { 'K', '3', 'Q', 'F', '2', 'F', '2', '0', 'E', 'M', '-',
                  'A', 'G', 'C', 'E' },
};

const struct nonspd_mem_info elpida_8gbit_lpddr3_edfa164a2ma_jd_f = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_UNDEFINED,

        .module_size_mbits      = 8192,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

        .module_mfg_id          = { .msb = 0x2c, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0x2c, .lsb = 0x80 },

        .part_num               =
                { 'E', 'D', 'F', 'A', '1', '6', '4', 'A', '2', 'M', 'A', '-',
                  'J', 'D', '-', 'F',},
};

const struct nonspd_mem_info micron_lpddr3_mt52l256m64d2pp_107wtb = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
	        { 'M', 'T', '5', '2', 'L', '2', '5', '6', 'M', '6', '4', 'D',
		  '2', 'P', 'P', '-', '1', '0', '7', 'W', 'T', ':', 'B' },
};

const struct nonspd_mem_info samsung_8gbit_lpddr3_k4e8e304ed_egcc = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_533 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.serial_num 		= { 0, 0, 0, 0 },
	.part_num		=
		{ 'K', '4', 'E', '8', 'E', '3', '0', '4', 'E', 'D', '-',
		  'E', 'G', 'C', 'C' },
};

const struct nonspd_mem_info samsung_4gbit_ddr3l_k4b4g1646d_byk0 = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.serial_num 		= { 0, 0, 0, 0 },
	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'D', '-',
		  'B', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_4gbit_ddr3l_k4b4g1646q_hyk0 = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.serial_num 		= { 0, 0, 0, 0 },
	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'Q', '-',
		  'H', 'Y', 'K', '0' },
};

const struct nonspd_mem_info hynix_4gbit_ddr3l_h5tc4g63cfr_pba = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 4096,
        .num_ranks              = 1,
        .device_width           = 16,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .serial_num             = { 0, 0, 0, 0 },
        .part_num               =
                { 'H', '5', 'T', 'C', '4', 'G', '6', '3', 'C', 'F', 'R', '-',
                  'P', 'B', 'A'},
};

const struct nonspd_mem_info hynix_4gbit_ddr3l_h5tc4g63afr_pba = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 4096,
        .num_ranks              = 1,
        .device_width           = 16,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .serial_num             = { 0, 0, 0, 0 },
        .part_num               =
                { 'H', '5', 'T', 'C', '4', 'G', '6', '3', 'A', 'F', 'R', '-',
                  'P', 'B', 'A'},
};

const struct nonspd_mem_info hynix_8gbit_ddr3l_h5tc8g63amr_pba = {
	.dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,
	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 16,
	.ddr_freq 		= { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id		= { .msb = 0xad, .lsb = 0x80 },

	.serial_num 		= { 0, 0, 0, 0 },
	.part_num		=
		{ 'H', '5', 'T', 'C', '8', 'G', '6', '3', 'A', 'M', 'R', '-',
		  'P', 'B', 'A' },
};

const struct nonspd_mem_info samsung_8gbit_ddr3l_k4b8g1646q_myk0 = {
	.dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,
	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 16,
	.ddr_freq 		= { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.serial_num 		= { 0, 0, 0, 0 },
	.part_num		=
		{ 'K', '4', 'B', '8', 'G', '1', '6', '4', '6', 'Q', '-',
		  'M', 'Y', 'K', '0' },
};
const struct nonspd_mem_info nanya_ddr3l_nt5cc256m16dp_di = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_UNDEFINED,
        .module_size_mbits      = 4096,
	.num_ranks              = 1,
	.device_width           = 16,
	/* CL = 11, CWL = 8, min = 1.25ns, max <1.5ns */
	.ddr_freq               = { DDR_667, DDR_800 },

	.module_mfg_id          = { .msb = 0x0b, .lsb = 0x03 },
	.dram_mfg_id            = { .msb = 0x0b, .lsb = 0x03 },

	.serial_num             = { 0, 0, 0, 0 },
	.part_num               =
		{ 'N', 'T', '5', 'C', 'C', '2', '5', '6',
		  'M', '1', '6', 'D', 'P', '-', 'D', 'I' },
};

const struct nonspd_mem_info samsung_k4b4g1646e = {
	.dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,
	.module_size_mbits      = 4096,
	.num_ranks              = 1,
	.device_width           = 16,
	.ddr_freq               = { DDR_400, DDR_533, DDR_667, DDR_800 },
	.module_mfg_id          = { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id            = { .msb = 0xce, .lsb = 0x00 },
	.serial_num             = { 0, 0, 0, 0 },
	.part_num               =
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'E', '-',
		  'B', 'Y', 'K', '0' },
};

static int pinky_dimm_count;
static const struct nonspd_mem_info *pinky_mem_info;
static int supports_tristate(struct platform_intf *intf)
{
	if(!strncmp(intf->name, "Jerry", 5) ||
	   !strncmp(intf->name, "Jaq", 3) ||
	   !strncmp(intf->name, "Mighty", 6))
		return 0;
	return 1;
}
/* read RAM code and fill in values needed by memory commands */
static int read_ram_code(struct platform_intf *intf)
{
	static int done = 0, ret = 0;
	uint32_t ram_code;

	if (done)
		return ret;

	ram_code = fdt_get_ram_code();
	if (ram_code == ~(0)) {
		lprintf(LOG_ERR, "Unable to obtain RAM code.\n");
		return -1;
	}

	if(!strncmp(intf->name, "Speedy", 6) ||
	   !strncmp(intf->name, "Minnie", 6))
	{
		switch (ram_code) {
		case 0:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_2gbit_lpddr3_k4e8e304ee_egce;
			break;
		case 1:
			pinky_dimm_count = 2;
			pinky_mem_info = &hynix_2gbit_lpddr3_h9ccnnn8gtmlar_nud;
			break;
		case 2:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_lpddr3_k4e6e304eb_egcf;
			break;
		case 7:
			pinky_dimm_count = 2;
			pinky_mem_info = &elpida_8gbit_lpddr3_f8132a3ma_gd_f;
			break;
		case 8:
			pinky_dimm_count = 2;
			pinky_mem_info = &samsung_4gbit_lpddr3_k4e6e304ee_egce;
			break;
		case 9:
			pinky_dimm_count = 2;
			pinky_mem_info = &hynix_4gbit_lpddr3_h9ccnnnbjtmlar_nud;
			break;
		case 0x0b:
			pinky_dimm_count = 2;
			pinky_mem_info = &elpida_16gbit_lpddr3_fa232a2ma_gc_f;
			break;
		default:
			ret = -1;
			break;
		}
	}else{
                if(!strncmp(intf->name, "Mickey", 6)) {
			switch (ram_code) {
			case 0:
				pinky_dimm_count = 2;
				pinky_mem_info = &samsung_2gbit_lpddr3_k3qf2f20em_agce;
				break;
			case 7:
				pinky_dimm_count = 2;
				pinky_mem_info = &elpida_8gbit_lpddr3_edfa164a2ma_jd_f;
				break;
			case 17:
				pinky_dimm_count = 2;
				pinky_mem_info = &micron_lpddr3_mt52l256m64d2pp_107wtb;
				break;
			default:
				ret = -1;
				break;
			}
		} else {
			switch (ram_code) {
			case 0:
				if(supports_tristate(intf)) {
					pinky_dimm_count = 2;
					pinky_mem_info = &samsung_8gbit_lpddr3_k4e8e304ed_egcc;
				} else {
					pinky_dimm_count = 4;
					pinky_mem_info = &samsung_k4b4g1646e;
					}
				break;
			case 4:
				pinky_dimm_count = 4;
				pinky_mem_info = &samsung_4gbit_ddr3l_k4b4g1646d_byk0;
				break;
			case 5:
				pinky_dimm_count = 4;
				pinky_mem_info = &hynix_4gbit_ddr3l_h5tc4g63cfr_pba;
				break;
			case 6:
				pinky_dimm_count = 4;
				pinky_mem_info = &samsung_4gbit_ddr3l_k4b4g1646q_hyk0;
				break;
		        case 0x0a:
				pinky_dimm_count = 4;
				pinky_mem_info = &nanya_ddr3l_nt5cc256m16dp_di;
				break;
			case 0x0d:
				pinky_dimm_count = 4;
				pinky_mem_info = &hynix_4gbit_ddr3l_h5tc4g63afr_pba;
				break;
			case 0x0e:
				pinky_dimm_count = 4;
				pinky_mem_info = &samsung_8gbit_ddr3l_k4b8g1646q_myk0;
				break;
			case 0x0f:
				pinky_dimm_count = 4;
				pinky_mem_info = &hynix_8gbit_ddr3l_h5tc8g63amr_pba;
				break;
			default:
				ret = -1;
				break;
			}
		}
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
	if (read_ram_code(intf) < 0)
		return -1;

	return pinky_dimm_count;
}

static int get_mem_info(struct platform_intf *intf,
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
