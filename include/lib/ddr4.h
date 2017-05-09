/*
 * Copyright 2017, Google Inc.
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
 *
 * DDR4 register map.
 */

#ifndef LIB_DDR4_H_
#define LIB_DDR4_H_

enum ddr4_reg_map {
	/* Base Configuration and DRAM Parameters */
	DDR4_SPD_REG_SIZE_CRC,
	DDR4_SPD_REG_REVISION,
	DDR4_SPD_REG_DEVICE_TYPE,
	DDR4_SPD_REG_MODULE_TYPE,
	DDR4_SPD_REG_DENSITY_BANKS,
	DDR4_SPD_REG_ADDRESSING,
	DDR4_SPD_REG_VOLTAGE = 11,
	DDR4_SPD_REG_MODULE_ORG,
	DDR4_SPD_REG_MODULE_BUS_WIDTH,
	DDR4_SPD_REG_FTB_MTB_TIMEBASE = 17,
	DDR4_SPD_REG_TCK_MIN = 18,
	DDR4_SPD_REG_FINE_OFFSET_TCK_MIN = 125,
	DDR4_SPD_REG_CRC_0,
	DDR4_SPD_REG_CRC_1,
	/* Manufacturing Information */
	DDR4_SPD_REG_MODULE_MANUF_JEDEC_ID_LSB = 320,
	DDR4_SPD_REG_MODULE_MANUF_JEDEC_ID_MSB,
	DDR4_SPD_REG_MODULE_MANUF_LOC,
	DDR4_SPD_REG_MODULE_MANUF_DATE_YEAR,
	DDR4_SPD_REG_MODULE_MANUF_DATE_WEEK,
	DDR4_SPD_REG_MODULE_MANUF_SERIAL_0,
	DDR4_SPD_REG_MODULE_MANUF_SERIAL_1,
	DDR4_SPD_REG_MODULE_MANUF_SERIAL_2,
	DDR4_SPD_REG_MODULE_MANUF_SERIAL_3,
	DDR4_SPD_REG_MODULE_PART_NUM_0,
	DDR4_SPD_REG_MODULE_PART_NUM_1,
	DDR4_SPD_REG_MODULE_PART_NUM_2,
	DDR4_SPD_REG_MODULE_PART_NUM_3,
	DDR4_SPD_REG_MODULE_PART_NUM_4,
	DDR4_SPD_REG_MODULE_PART_NUM_5,
	DDR4_SPD_REG_MODULE_PART_NUM_6,
	DDR4_SPD_REG_MODULE_PART_NUM_7,
	DDR4_SPD_REG_MODULE_PART_NUM_8,
	DDR4_SPD_REG_MODULE_PART_NUM_9,
	DDR4_SPD_REG_MODULE_PART_NUM_10,
	DDR4_SPD_REG_MODULE_PART_NUM_11,
	DDR4_SPD_REG_MODULE_PART_NUM_12,
	DDR4_SPD_REG_MODULE_PART_NUM_13,
	DDR4_SPD_REG_MODULE_PART_NUM_14,
	DDR4_SPD_REG_MODULE_PART_NUM_15,
	DDR4_SPD_REG_MODULE_PART_NUM_16,
	DDR4_SPD_REG_MODULE_PART_NUM_17,
	DDR4_SPD_REG_MODULE_REVISION_0,
	DDR4_SPD_REG_DRAM_MANUF_JEDEC_ID_LSB,
	DDR4_SPD_REG_DRAM_MANUF_JEDEC_ID_MSB,
};

#endif /* LIB_DDR4_H_ */
