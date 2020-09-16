/*
 * Copyright 2015, Google Inc.
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

#include "lib/math.h"
#include "lib/nonspd.h"
#include "mosys/log.h"

const struct nonspd_mem_info elpida_lpddr3_edfa164a2ma_jd_f = {
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

const struct nonspd_mem_info elpida_lpddr3_f8132a3ma_gd_f = {
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

const struct nonspd_mem_info elpida_lpddr3_fa232a2ma_gc_f = {
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

const struct nonspd_mem_info hynix_ddr3l_h5tc4g63afr_pba = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 4096,
        .num_ranks              = 1,
        .device_width           = 16,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '5', 'T', 'C', '4', 'G', '6', '3', 'A', 'F', 'R', '-',
                  'P', 'B', 'A'},
};

const struct nonspd_mem_info hynix_ddr3l_h5tc4g63cfr_pba = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 4096,
        .num_ranks              = 1,
        .device_width           = 16,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '5', 'T', 'C', '4', 'G', '6', '3', 'C', 'F', 'R', '-',
                  'P', 'B', 'A'},
};

const struct nonspd_mem_info hynix_ddr3l_h5tc4g63efr_rda = {
        .dram_type              = SPD_DRAM_TYPE_DDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_UNDEFINED,

        .module_size_mbits      = 4096,
        .num_ranks              = 1,
        .device_width           = 16,
        .ddr_freq               = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '5', 'T', 'C', '4', 'G', '6', '3', 'E', 'F', 'R', '-',
                  'R', 'D', 'A'},
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnn8gtmlar_nud = {
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

const struct nonspd_mem_info hynix_lpddr3_h9ccnnn8jtblar_nud = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 2048 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id = { .msb = 0xad, .lsb = 0x80 },

	.part_num = { 'H', '9', 'C', 'C', 'N', 'N', 'N', '8', 'J', 'T', 'B',
		      'L', 'A', 'R', '-', 'N', 'U', 'D' },
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnnbjtalar_nud = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 16384,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'J', 'T', 'A', 'L',
                  'A', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnnbjtmlar_nud = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR3,
        .module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

        .module_size_mbits      = 16384,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num               =
                { 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'J', 'T', 'M', 'L',
                  'A', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info hynix_ddr3l_h5tc8g63amr_pba = {
	.dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,
	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 16,
	.ddr_freq 		= { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id		= { .msb = 0xad, .lsb = 0x80 },

	.part_num		=
		{ 'H', '5', 'T', 'C', '8', 'G', '6', '3', 'A', 'M', 'R', '-',
		  'P', 'B', 'A' },
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnnbptblbr_nud = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits      = 16384,
	.num_ranks              = 2,
	.device_width           = 32,
	.ddr_freq               = { DDR_667, DDR_800, DDR_933 },

	.module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

	.part_num =
		{ 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'P', 'T', 'B', 'L',
		  'B', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnnbltblar_nud = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits      = 16384,
	.num_ranks              = 2,
	.device_width           = 32,
	.ddr_freq               = { DDR_667, DDR_800, DDR_933 },

	.module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

	.part_num =
		{ 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'L', 'T', 'B', 'L',
		  'A', 'R', '-', 'N', 'U', 'D',},
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnnbktmlbr_ntd = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits      = 16384,
	.num_ranks              = 2,
	.device_width           = 32,
        .ddr_freq               = { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

	.part_num =
		{ 'H', '9', 'C', 'C', 'N', 'N', 'N', 'B', 'K', 'T', 'M', 'L',
		  'B', 'R', '-', 'N', 'T', 'D',},
};

const struct nonspd_mem_info hynix_lpddr3_h9ccnnncltmlar_nud = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 8192 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id = { .msb = 0xad, .lsb = 0x80 },

	.part_num = { 'H', '9', 'C', 'C', 'N', 'N', 'N', 'C', 'L', 'T', 'M',
		      'L', 'A', 'R', '-', 'N', 'U', 'D' },
};

static const struct nonspd_mem_info hynix_lpddr4_h9hcnnn8kumlhr = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR4,

        .module_size_mbits      = 8192,
        .num_ranks              = 1,
        .device_width           = 32,
        .ddr_freq               = { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num =
                { 'H', '9', 'H', 'C', 'N', 'N', 'N', '8', 'K', 'U', 'M', 'L',
                  'H', 'R',},
};

static const struct nonspd_mem_info hynix_lpddr4_h9hcnnnbpumlhr = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR4,

        .module_size_mbits      = 16384,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num =
                { 'H', '9', 'H', 'C', 'N', 'N', 'N', 'B', 'P', 'U', 'M', 'L',
                  'H', 'R',},
};

const struct nonspd_mem_info hynix_lpddr4x_h9hcnnncpmalhr_nee = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR4X,

        .module_size_mbits      = 32768,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_800, DDR_1200, DDR_1600 },

        .module_mfg_id          = { .msb = 0xad, .lsb = 0x80 },
        .dram_mfg_id            = { .msb = 0xad, .lsb = 0x80 },

        .part_num =
                { 'H', '9', 'H', 'C', 'N', 'N', 'N', 'C', 'P', 'M', 'A', 'L',
                  'H', 'R', '-', 'N', 'E', 'E'},
};

const struct nonspd_mem_info hynix_lpddr4x_h9hcnnncpmmlxr_nee = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq		= { DDR_800, DDR_1200, DDR_1600 },

	.module_mfg_id		= { .msb = 0xad, .lsb = 0x80 },
	.dram_mfg_id		= { .msb = 0xad, .lsb = 0x80 },

	.part_num =
                { 'H', '9', 'H', 'C', 'N', 'N', 'N', 'C', 'P', 'M', 'M', 'L',
                  'X', 'R', '-', 'N', 'E', 'E'},
};

const struct nonspd_mem_info micron_mt41k256m16ha = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		= { 'M', 'T', '4', '1', 'K', '2', '5', '6', 'M',
				    '1', '6', 'H', 'A', '-', '1', '2', '5' },
};

const struct nonspd_mem_info micron_mt52l256m32d1pf = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_800, DDR_933, DDR_1067 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		= { 'M', 'T', '5', '2', 'L', '2', '5', '6', 'M',
				    '3', '2', 'D', '1', 'P', 'F', '-', '0', '9',
				    '3', 'W', 'T', ':', 'B' },
};

const struct nonspd_mem_info micron_mt52l512m32d2pf = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_800, DDR_933, DDR_1067 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		= { 'M', 'T', '5', '2', 'L', '5', '1', '2', 'M',
				    '3', '2', 'D', '2', 'P', 'F', '-', '0', '9',
				    '3', 'W', 'T', ':', 'B' },
};

const struct nonspd_mem_info nanya_ddr3l_nt5cc256m16dp_di = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	/* CL = 11, CWL = 8, min = 1.25ns, max <1.5ns */
	.ddr_freq 		= { DDR_667, DDR_800 },
	.module_mfg_id		= { .msb = 0x0b, .lsb = 0x03 },
	.dram_mfg_id		= { .msb = 0x0b, .lsb = 0x03 },

	.part_num		= { 'N', 'T', '5', 'C', 'C', '2', '5', '6',
				    'M', '1', '6', 'D', 'P', '-', 'D', 'I' },
};

const struct nonspd_mem_info nanya_ddr3l_nt5cc256m16er_ek = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },
	.module_mfg_id		= { .msb = 0x0b, .lsb = 0x03 },
	.dram_mfg_id		= { .msb = 0x0b, .lsb = 0x03 },

	.part_num		= { 'N', 'T', '5', 'C', 'C', '2', '5', '6',
				    'M', '1', '6', 'E', 'R', '-', 'E', 'K' },
};

const struct nonspd_mem_info samsung_k4b4g1646d = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'D',
		  '-', 'B', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_k4b4g1646e = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'E',
		  '-', 'B', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_k4b4g1646e_byma = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_UNDEFINED,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'E',
		  '-', 'B', 'Y', 'M', 'A' },
};

const struct nonspd_mem_info samsung_ddr3l_k4b4g1646d_byk0 = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'D', '-',
		  'B', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_ddr3l_k4b4g1646q_hyk0 = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 4096,
	.num_ranks		= 1,
	.device_width		= 16,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '4', 'G', '1', '6', '4', '6', 'Q', '-',
		  'H', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_ddr3l_k4b8g1646q_myk0 = {
	.dram_type              = SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type  = DDR3_MODULE_TYPE_SO_DIMM,
	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 16,
	.ddr_freq 		= { DDR_333, DDR_400, DDR_533, DDR_667, DDR_800 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'B', '8', 'G', '1', '6', '4', '6', 'Q', '-',
		  'M', 'Y', 'K', '0' },
};

const struct nonspd_mem_info samsung_lpddr3_k3qf2f20em_agce = {
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

const struct nonspd_mem_info samsung_lpddr3_k4e6e304eb_egce = {
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
		  'E', 'G', 'C', 'E' },
};

const struct nonspd_mem_info samsung_lpddr3_k4e6e304ec_egcf = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 4096 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0xce, .lsb = 0x00 },

	.part_num = { 'K', '4', 'E', '6', 'E', '3', '0', '4', 'E', 'C', '-',
		      'E', 'G', 'C', 'F' },
};

const struct nonspd_mem_info samsung_lpddr3_k4e6e304ee_egce = {
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

const struct nonspd_mem_info samsung_lpddr3_k4e6e304ee_egcf = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 4096 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0xce, .lsb = 0x00 },

	.part_num = { 'K', '4', 'E', '6', 'E', '3', '0', '4', 'E', 'E', '-',
		      'E', 'G', 'C', 'F' },
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

const struct nonspd_mem_info samsung_lpddr3_k4e8e304ed_egcc = {
	.dram_type		= SPD_DRAM_TYPE_DDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_533 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'E', '8', 'E', '3', '0', '4', 'E', 'D', '-',
		  'E', 'G', 'C', 'C' },
};

const struct nonspd_mem_info samsung_lpddr3_k4e8e304ee_egce = {
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

const struct nonspd_mem_info samsung_lpddr3_k4e8e324eb_egcf = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'E', '8', 'E', '3', '2', '4', 'E', 'B', '-',
		  'E', 'G', 'C', 'F' },
};

const struct nonspd_mem_info samsung_lpddr3_k4ebe304eb_egcf = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 8192 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0xce, .lsb = 0x00 },

	.part_num = { 'K', '4', 'E', 'B', 'E', '3', '0', '4', 'E', 'B', '-',
		      'E', 'G', 'C', 'F' },
};

const struct nonspd_mem_info micron_lpddr3_mt52l256m32d1pf_107wtb = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
	        { 'M', 'T', '5', '2', 'L', '2', '5', '6', 'M', '3', '2', 'D',
		  '1', 'P', 'F', '-', '1', '0', '7', 'W', 'T', ':', 'B' },
};

const struct nonspd_mem_info micron_lpddr3_mt52l256m32d1pf107 = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 2048 * 8,
	.num_ranks = 1,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0x2c, .lsb = 0x00 },

	.part_num = { 'M', 'T', '5', '2', 'L', '2', '5', '6', 'M', '3', '2',
		      'D', '1', 'P', 'F', '1', '0', '7' },
};

const struct nonspd_mem_info micron_lpddr3_mt52l256m32d1pf_10 = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 2048 * 8,
	.num_ranks = 1,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0x2c, .lsb = 0x00 },

	.part_num = { 'M', 'T', '5', '2', 'L', '2', '5', '6', 'M', '3', '2',
		      'D', '1', 'P', 'F', '-', '1', '0' },
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

const struct nonspd_mem_info micron_lpddr3_mt52l512m32d2pf_107wtb = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type	= DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
	        { 'M', 'T', '5', '2', 'L', '5', '1', '2', 'M', '3', '2', 'D',
		  '2', 'P', 'F', '-', '1', '0', '7', 'W', 'T', ':', 'B' },
};

const struct nonspd_mem_info micron_lpddr3_mt52l512m32d2pf_10 = {
	.dram_type = SPD_DRAM_TYPE_LPDDR3,
	.module_type.ddr3_type = DDR3_MODULE_TYPE_SO_DIMM,

	.module_size_mbits = 4096 * 8,
	.num_ranks = 2,
	.device_width = 64,
	.ddr_freq = { DDR_400, DDR_533, DDR_667, DDR_800, DDR_933 },

	.module_mfg_id = { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id = { .msb = 0x2c, .lsb = 0x00 },

	.part_num = { 'M', 'T', '5', '2', 'L', '5', '1', '2', 'M', '3', '2',
		      'D', '2', 'P', 'F', '-', '1', '0' },
};

static const struct nonspd_mem_info micron_lpddr4_mt53b256m32d1np = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'B', '2', '5', '6', 'M', '3', '2', 'D',
		  '1', 'N', 'P'},
};

static const struct nonspd_mem_info micron_lpddr4_mt53b512m32d2np = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'B', '5', '1', '2', 'M', '3', '2', 'D',
		  '2', 'N', 'P'},
};

static const struct nonspd_mem_info micron_lpddr4_mt53e512m32d2np = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'E', '5', '1', '2', 'M', '3', '2', 'D',
		  '2', 'N', 'P'},
};

const struct nonspd_mem_info micron_lpddr4x_mt53e1g32d4nq_046wte = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq		= { DDR_800, DDR_1200, DDR_1600 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'E', '1', 'G', '3', '2', 'D', '4', 'N',
		  'Q', '-', '4', '6', 'W', 'T', ':', 'E'},
};

const struct nonspd_mem_info micron_lpddr4x_mt53e1g32d2np_046wta = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq		= { DDR_2133 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num	=
		{ 'M', 'T', '5', '3', 'E', '1', 'G', '3', '2', 'D', '2', 'N',
		  'P', '-', '4', '6', 'W', 'T', ':', 'A'},
};

const struct nonspd_mem_info micron_lpddr4x_mt53e2g32d4nq_046wta = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq		= { DDR_2133 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'E', '2', 'G', '3', '2', 'D', '4', 'N',
		  'Q', '-', '4', '6', 'W', 'T', ':', 'A'},
};

const struct nonspd_mem_info micron_lpddr4x_mt53d1g32d4dt_046wtd = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq		= { DDR_2133 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '5', '3', 'D', '1', 'G', '3', '2', 'D', '4', 'D',
		  'T', '-', '4', '6', 'W', 'T', ':', 'D'},
};

const struct nonspd_mem_info micron_lpddr4x_mt29vzzzad8dqksl = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq		= { DDR_800, DDR_1200, DDR_1600 },

	.module_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x2c, .lsb = 0x00 },

	.part_num		=
		{ 'M', 'T', '2', '9', 'V', 'Z', 'Z', 'Z', 'A', 'D', '8', 'D',
		  'Q', 'K', 'S', 'L'},
};

const struct nonspd_mem_info micron_lpddr4x_mt29vzzzad8gqfsl_046 = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits      = 32768,
	.num_ranks              = 2,
	.device_width           = 32,
	.ddr_freq               = { DDR_2133 },

	.module_mfg_id          = { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id            = { .msb = 0x2c, .lsb = 0x00 },

	.part_num               =
		{ 'M', 'T', '2', '9', 'V', 'Z', 'Z', 'Z', 'A', 'D', '8', 'G',
		  'Q', 'F', 'S', 'L', '-', '0', '4', '6'},
};

const struct nonspd_mem_info micron_lpddr4x_mt29vzzzbd9dqkpr_046 = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits      = 32768,
	.num_ranks              = 2,
	.device_width           = 32,
	.ddr_freq               = { DDR_2133 },

	.module_mfg_id          = { .msb = 0x2c, .lsb = 0x00 },
	.dram_mfg_id            = { .msb = 0x2c, .lsb = 0x00 },

	.part_num               =
		{ 'M', 'T', '2', '9', 'V', 'Z', 'Z', 'Z', 'B', 'D', '9', 'D',
		  'Q', 'K', 'P', 'R', '-', '0', '4', '6'},
};

const struct nonspd_mem_info samsung_lpddr4_k3uh5h50mm_agcj = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_1355 },

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '3', 'U', 'H', '5', 'H', '5', '0', 'M', 'M', '-',
		  'A', 'G', 'C', 'J' },
};

static const struct nonspd_mem_info samsung_lpddr4_k4f6e304hb_mgcj = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 16384,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'F', '6', 'E', '3', '0', '4', 'H', 'B', '-',
		  'M', 'G', 'C', 'J' },
};

static const struct nonspd_mem_info samsung_lpddr4_k4f6e3s4hm_mgcj = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 16384,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'F', '6', 'E', '3', 'S', '4', 'H', 'M', '-',
		  'M', 'G', 'C', 'J' },
};

static const struct nonspd_mem_info samsung_lpddr4_k4f8e304hb_mgcj = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'F', '8', 'E', '3', '0', '4', 'H', 'B', '-',
		  'M', 'G', 'C', 'J' },
};

static const struct nonspd_mem_info samsung_lpddr4_k4f8e3s4hd_mgcl = {
	.dram_type              = SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits      = 8192,
	.num_ranks              = 1,
	.device_width           = 32,
	.ddr_freq               = { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id          = { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id            = { .msb = 0xce, .lsb = 0x00 },

	.part_num               =
		{ 'K', '4', 'F', '8', 'E', '3', 'S', '4', 'H', 'D', '-',
		  'M', 'G', 'C', 'L' },
};

const struct nonspd_mem_info samsung_lpddr4x_kmdh6001da_b422 = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', 'M', 'D', 'H', '6', '0', '0', '1', 'D', 'A', '-',
		  'B', '4', '2', '2' },
};

const struct nonspd_mem_info samsung_lpddr4x_kmdp6001da_b425 = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', 'M', 'D', 'P', '6', '0', '0', '1', 'D', 'A', '-',
		  'B', '4', '2', '5' },
};

const struct nonspd_mem_info samsung_lpddr4x_kmdv6001da_b620 = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', 'M', 'D', 'V', '6', '0', '0', '1', 'D', 'A', '-',
		  'B', '6', '2', '0' },
};

const struct nonspd_mem_info samsung_lpddr4x_k4ube3d4am_mgcj = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'U', 'B', 'E', '3', 'D', '4', 'A', 'M', '-',
		  'M', 'G', 'C', 'J' },
};

const struct nonspd_mem_info samsung_lpddr4x_k4ube3d4aa_mgcl = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'U', 'B', 'E', '3', 'D', '4', 'A', 'A', '-',
		  'M', 'G', 'C', 'L' },
};

const struct nonspd_mem_info samsung_lpddr4x_k4ube3d4aa_mgcr = {
        .dram_type              = SPD_DRAM_TYPE_LPDDR4X,

        .module_size_mbits      = 32768,
        .num_ranks              = 2,
        .device_width           = 32,
        .ddr_freq               = { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

        .module_mfg_id          = { .msb = 0xce, .lsb = 0x00 },
        .dram_mfg_id            = { .msb = 0xce, .lsb = 0x00 },

        .part_num               =
                { 'K', '4', 'U', 'B', 'E', '3', 'D', '4', 'A', 'A', '-',
                  'M', 'G', 'C', 'R' },
};

static const struct nonspd_mem_info samsung_lpddr4_k4f6e304hb_mgch = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4,

	.module_size_mbits	= 8192,
	.num_ranks		= 1,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0xce, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0xce, .lsb = 0x00 },

	.part_num		=
		{ 'K', '4', 'F', '6', 'E', '3', '0', '4', 'H', 'B', '-',
		  'M', 'G', 'C', 'H' },
};

const struct nonspd_mem_info sandisk_lpddr4x_sdada4cr_128g = {
	.dram_type		= SPD_DRAM_TYPE_LPDDR4X,

	.module_size_mbits	= 32768,
	.num_ranks		= 2,
	.device_width		= 32,
	.ddr_freq 		= { DDR_667, DDR_800, DDR_933, DDR_1067, DDR_1200},

	.module_mfg_id		= { .msb = 0x45, .lsb = 0x00 },
	.dram_mfg_id		= { .msb = 0x45, .lsb = 0x00 },

	.part_num		=
		{ 'S', 'D', 'A', 'D', 'A', '4', 'C', 'R', '-', '1', '2',
		  '8', 'G' },
};


// This one is reserved for storing mem info from SMBIOS if no explicit entry
// was added above.
static struct nonspd_mem_info part_extracted_from_smbios = {
	.part_num		=
		{ 'U', 'N', 'P', 'R', 'O', 'V', 'I', 'S', 'I', 'O', 'N', 'E', 'D'},
};

static const struct nonspd_mem_info *nospdmemory[] = {
	&elpida_lpddr3_edfa164a2ma_jd_f,
	&elpida_lpddr3_f8132a3ma_gd_f,
	&elpida_lpddr3_fa232a2ma_gc_f,
	&hynix_ddr3l_h5tc4g63afr_pba,
	&hynix_ddr3l_h5tc4g63cfr_pba,
	&hynix_ddr3l_h5tc4g63efr_rda,
	&hynix_lpddr3_h9ccnnn8gtmlar_nud,
	&hynix_lpddr3_h9ccnnn8jtblar_nud,
	&hynix_lpddr3_h9ccnnnbjtalar_nud,
	&hynix_lpddr3_h9ccnnnbjtmlar_nud,
	&hynix_lpddr3_h9ccnnncltmlar_nud,
	&hynix_ddr3l_h5tc8g63amr_pba,
	&hynix_lpddr3_h9ccnnnbptblbr_nud,
	&hynix_lpddr3_h9ccnnnbltblar_nud,
	&hynix_lpddr3_h9ccnnnbktmlbr_ntd,
	&hynix_lpddr4_h9hcnnn8kumlhr,
	&hynix_lpddr4_h9hcnnnbpumlhr,
	&hynix_lpddr4x_h9hcnnncpmalhr_nee,
	&hynix_lpddr4x_h9hcnnncpmmlxr_nee,
	&micron_lpddr3_mt52l256m32d1pf107,
	&micron_lpddr3_mt52l256m32d1pf_10,
	&micron_lpddr3_mt52l256m32d1pf_107wtb,
	&micron_lpddr3_mt52l256m64d2pp_107wtb,
	&micron_lpddr3_mt52l512m32d2pf_107wtb,
	&micron_lpddr3_mt52l512m32d2pf_10,
	&micron_lpddr4_mt53b256m32d1np,
	&micron_lpddr4_mt53b512m32d2np,
	&micron_lpddr4_mt53e512m32d2np,
	&micron_lpddr4x_mt29vzzzad8dqksl,
	&micron_lpddr4x_mt29vzzzad8gqfsl_046,
	&micron_lpddr4x_mt29vzzzbd9dqkpr_046,
	&micron_lpddr4x_mt53e1g32d4nq_046wte,
	&micron_lpddr4x_mt53e1g32d2np_046wta,
	&micron_lpddr4x_mt53e2g32d4nq_046wta,
	&micron_lpddr4x_mt53d1g32d4dt_046wtd,
	&micron_mt41k256m16ha,
	&micron_mt52l256m32d1pf,
	&micron_mt52l512m32d2pf,
	&nanya_ddr3l_nt5cc256m16dp_di,
	&nanya_ddr3l_nt5cc256m16er_ek,
	&samsung_k4b4g1646d,
	&samsung_k4b4g1646e,
	&samsung_k4b4g1646e_byma,
	&samsung_ddr3l_k4b4g1646d_byk0,
	&samsung_ddr3l_k4b4g1646q_hyk0,
	&samsung_ddr3l_k4b8g1646q_myk0,
	&samsung_lpddr3_k3qf2f20em_agce,
	&samsung_lpddr3_k4e6e304eb_egce,
	&samsung_lpddr3_k4e6e304ec_egcf,
	&samsung_lpddr3_k4e6e304ee_egce,
	&samsung_lpddr3_k4e6e304ee_egcf,
	&samsung_lpddr3_k4e6e304eb_egcf,
	&samsung_lpddr3_k4e8e304ed_egcc,
	&samsung_lpddr3_k4e8e304ee_egce,
	&samsung_lpddr3_k4e8e324eb_egcf,
	&samsung_lpddr3_k4ebe304eb_egcf,
	&samsung_lpddr4_k3uh5h50mm_agcj,
	&samsung_lpddr4_k4f6e304hb_mgch,
	&samsung_lpddr4_k4f6e304hb_mgcj,
	&samsung_lpddr4_k4f6e3s4hm_mgcj,
	&samsung_lpddr4_k4f8e304hb_mgcj,
	&samsung_lpddr4_k4f8e3s4hd_mgcl,
	&samsung_lpddr4x_kmdh6001da_b422,
	&samsung_lpddr4x_kmdp6001da_b425,
	&samsung_lpddr4x_kmdv6001da_b620,
	&samsung_lpddr4x_k4ube3d4am_mgcj,
	&samsung_lpddr4x_k4ube3d4aa_mgcl,
	&sandisk_lpddr4x_sdada4cr_128g
};

static int transfer_speed_from_smbios_to_nonspd_mem_info(
	struct smbios_table *table,
	struct nonspd_mem_info *info)
{
	uint32_t expected_speed;

	for (int index = DDR_333; index < DDR_FREQ_MAX; index++) {
		expected_speed = strtoul(ddr_freq_prettyprint[index], NULL, 10);
		if (table->data.mem_device.speed >= expected_speed - 1 &&
		    table->data.mem_device.speed <= expected_speed + 1) {
			info->ddr_freq[0] = index;
			return 0;
		}
	}

	lprintf(LOG_ERR, "%s: mem speed %hu in SMBIOS is out of range.",
			__func__, table->data.mem_device.speed);
	return -1;
}

static enum spd_dram_type map_smbios_mem_type_to_spd(struct smbios_table *table)
{
	switch (table->data.mem_device.type) {
	case SMBIOS_MEMORY_TYPE_DDR3:
		return SPD_DRAM_TYPE_DDR3;
	case SMBIOS_MEMORY_TYPE_DDR4:
		return SPD_DRAM_TYPE_DDR4;
	case SMBIOS_MEMORY_TYPE_LPDDR3:
		return SPD_DRAM_TYPE_LPDDR3;
	case SMBIOS_MEMORY_TYPE_LPDDR4:
		return SPD_DRAM_TYPE_LPDDR4;
	default:
		lprintf(LOG_ERR, "%s: Unknown SMBIOS memory type: %d\n",
			__func__, table->data.mem_device.type);
		return 0;
	}
}

static int extract_mem_info_from_smbios(
	struct smbios_table *table,
	struct nonspd_mem_info *info)
{
	const char *smbios_part_num;
	size_t smbios_part_num_len, max_part_num_len;
	uint32_t size;

	max_part_num_len = sizeof(info->part_num) - 1;
	smbios_part_num = table->string[table->data.mem_device.part_number];
	smbios_part_num_len = strlen(smbios_part_num);

	if (!smbios_part_num_len ||
		smbios_part_num_len > max_part_num_len) {
		lprintf(LOG_ERR, "%s: SMBIOS Memory info table: part num is missing. "
				"Or len of part number %lu is larger then buffer %lu."
				, __func__, (unsigned long)smbios_part_num_len,
				(unsigned long)max_part_num_len);
		return -1;
	}

	size = (table->data.mem_device.size & 0x7fff) * 8;
	info->module_size_mbits =
		(table->data.mem_device.size & 0x8000 ? size * 1024 : size);

	strncpy((char *)info->part_num, smbios_part_num, max_part_num_len);

	info->dram_type = map_smbios_mem_type_to_spd(table);
	info->num_ranks = table->data.mem_device.attributes & 0xf;
	info->device_width = table->data.mem_device.data_width;

	return transfer_speed_from_smbios_to_nonspd_mem_info(table, info);
}

int spd_set_nonspd_info_from_smbios(struct platform_intf *intf, int dimm,
				    const struct nonspd_mem_info **info)
{
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_MEMORY, dimm, &table) < 0) {
		lprintf(LOG_ERR, "%s: SMBIOS Memory info table missing\n",
			__func__);
		return -1;
	}

	/* memory device from SMBIOS is mapped into a nonspd_mem_info */
	if (extract_mem_info_from_smbios(&table, &part_extracted_from_smbios))
		return -1;

	*info = &part_extracted_from_smbios;

	return 0;
}

int spd_set_nonspd_info(struct platform_intf *intf, int dimm,
                        const struct nonspd_mem_info **info)
{
	int index;
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_MEMORY, dimm, &table) < 0) {
		lprintf(LOG_ERR, "%s: SMBIOS Memory info table missing\n",
			__func__);
		return -1;
	}

	for (index = 0; index < ARRAY_SIZE(nospdmemory); index++) {
		if (!strncmp(table.string[table.data.mem_device.part_number],
			(const char *)nospdmemory[index]->part_num,
			sizeof(nospdmemory[index]->part_num))) {
			*info = nospdmemory[index];
			break;
		}
	}

	if (index < ARRAY_SIZE(nospdmemory)) {
		return 0;
	}

	/* memory device from SMBIOS is mapped into a nonspd_mem_info */
	if (extract_mem_info_from_smbios(&table, &part_extracted_from_smbios)) {
		return -1;
	}

	*info = &part_extracted_from_smbios;

	return 0;
}
