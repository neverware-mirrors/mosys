/*
 * Copyright 2012, Google Inc.
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
 * DDR3 register map.
 */

#ifndef LIB_DDR3_H_
#define LIB_DDR3_H_

enum ddr3_reg_map {
	/* 0 */
	DDR3_SPD_REG_SIZE_CRC,
	DDR3_SPD_REG_REVISION,
	DDR3_SPD_REG_DEVICE_TYPE,
	DDR3_SPD_REG_MODULE_TYPE,
	DDR3_SPD_REG_DENSITY_BANKS,
	DDR3_SPD_REG_ADDRESSING,
	DDR3_SPD_REG_VOLTAGE,
	DDR3_SPD_REG_MODULE_ORG,
	DDR3_SPD_REG_MODULE_BUS_WIDTH,
	DDR3_SPD_REG_FTB_DIVIDEND_DIVSOR,
	/* 10 */
	DDR3_SPD_REG_MTB_DIVIDEND,
	DDR3_SPD_REG_MTB_DIVISOR,
	DDR3_SPD_REG_TCK_MIN,
	DDR3_SPD_REG_RESERVED,
	DDR3_SPD_REG_CAS_LAT_LSB,
	DDR3_SPD_REG_CAS_LAT_MSB,
	DDR3_SPD_REG_TAA_MIN,
	DDR3_SPD_REG_TWR_MIN,
	DDR3_SPD_REG_TRCD_MIN,
	DDR3_SPD_REG_TRRD_MIN,
	/* 20 */
	DDR3_SPD_REG_TRP_MIN,
	DDR3_SPD_REG_TRAS_TRC_UP_NIBBLE,
	DDR3_SPD_REG_TRAS_MIN_LSB,
	DDR3_SPD_REG_TRC_MIN_LSB,
	DDR3_SPD_REG_TRFC_MIN_LSB,
	DDR3_SPD_REG_TRFC_MIN_MSB,
	DDR3_SPD_REG_TWTR_MIN,
	DDR3_SPD_REG_TRTP_MIN,
	DDR3_SPD_REG_TFAW_UP_NIBBLE,
	DDR3_SPD_REG_TFAW_MIN,
	/* 30 */
	DDR3_SPD_REG_OPT_FEATURES,
	DDR3_SPD_REG_THERM_REFRESH_OPT,
	DDR3_SPD_REG_MODULE_THERM_SENSOR,
	DDR3_SPD_REG_SDRAM_DEVICE_TYPE,
	DDR3_SPD_REG_GENERAL_SECTION_START,
	DDR3_SPD_REG_GENERAL_SECTION_END = 59,
	/* 60 */
	DDR3_SPD_REG_MODULE_SPECIFIC_SECTION_START,
	DDR3_SPD_REG_MODULE_SPECIFIC_SECTION_END = 116,
	/* 117 */
	DDR3_SPD_REG_MODULE_MANUF_JEDEC_ID_LSB,
	DDR3_SPD_REG_MODULE_MANUF_JEDEC_ID_MSB,
	DDR3_SPD_REG_MODULE_MANUF_LOC,
	/* 120 */
	DDR3_SPD_REG_MODULE_MANUF_DATE_YEAR,
	DDR3_SPD_REG_MODULE_MANUF_DATE_WEEK,
	DDR3_SPD_REG_MODULE_MANUF_SERIAL_0,
	DDR3_SPD_REG_MODULE_MANUF_SERIAL_1,
	DDR3_SPD_REG_MODULE_MANUF_SERIAL_2,
	DDR3_SPD_REG_MODULE_MANUF_SERIAL_3,
	DDR3_SPD_REG_CRC_0,
	DDR3_SPD_REG_CRC_1,
	DDR3_SPD_REG_MODULE_PART_NUM_0,
	DDR3_SPD_REG_MODULE_PART_NUM_START = DDR3_SPD_REG_MODULE_PART_NUM_0,
	DDR3_SPD_REG_MODULE_PART_NUM_1,
	/* 130 */
	DDR3_SPD_REG_MODULE_PART_NUM_2,
	DDR3_SPD_REG_MODULE_PART_NUM_3,
	DDR3_SPD_REG_MODULE_PART_NUM_4,
	DDR3_SPD_REG_MODULE_PART_NUM_5,
	DDR3_SPD_REG_MODULE_PART_NUM_6,
	DDR3_SPD_REG_MODULE_PART_NUM_7,
	DDR3_SPD_REG_MODULE_PART_NUM_8,
	DDR3_SPD_REG_MODULE_PART_NUM_9,
	DDR3_SPD_REG_MODULE_PART_NUM_10,
	DDR3_SPD_REG_MODULE_PART_NUM_11,
	/* 140 */
	DDR3_SPD_REG_MODULE_PART_NUM_12,
	DDR3_SPD_REG_MODULE_PART_NUM_13,
	DDR3_SPD_REG_MODULE_PART_NUM_14,
	DDR3_SPD_REG_MODULE_PART_NUM_15,
	DDR3_SPD_REG_MODULE_PART_NUM_16,
	DDR3_SPD_REG_MODULE_PART_NUM_17,
	DDR3_SPD_REG_MODULE_PART_NUM_END = DDR3_SPD_REG_MODULE_PART_NUM_17,
	DDR3_SPD_REG_MODULE_REVISION_0,
	DDR3_SPD_REG_MODULE_REVISION_1,
	DDR3_SPD_REG_DRAM_MANUF_JEDEC_ID_LSB,
	DDR3_SPD_REG_DRAM_MANUF_JEDEC_ID_MSB,
	DDR3_SPD_REG_MANUF_SPECIFIC_DATA_START,
	DDR3_SPD_REG_MANUF_SPECIFIC_DATA_END = 175,
	/* 176 */
	DDR3_SPD_REG_CUSTOMER_USE_START,
	DDR3_SPD_REG_CUSTOMER_USE_END = 255,
};

#endif /* LIB_DDR3_H_ */
