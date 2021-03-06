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
 * Serial Presence Detect (SPD) code for access of SPDs on DIMMs.
 */

#ifndef LIB_SPD_H__
#define LIB_SPD_H__

#include <inttypes.h>

#include "intf/i2c.h"
#include "lib/ddr4.h"
#include "lib/val2str.h"

#define SPD_READ          0
#define SPD_WRITE         1

#define SPD_DEFAULT_LENGTH 256
#define SPD_DDR4_LENGTH    512
#define SPD_MAX_LENGTH     SPD_DDR4_LENGTH

#define SPD_DEFAULT_PART_OFF 128
#define SPD_DDR4_PART_OFF    DDR4_SPD_REG_MODULE_PART_NUM_0

#define SPD_DEFAULT_PART_LEN 18
#define SPD_DDR4_PART_LEN    SPD_DEFAULT_PART_LEN

#define SPD_PAGE_0        (0x6C >> 1)
#define SPD_PAGE_1        (0x6E >> 1)

/* forward declarations */
struct kv_pair;
struct platform_intf;
struct spd_reg;

/* different types of DRAM (fundamental memory type) for SPD */
enum spd_dram_type {
	SPD_DRAM_TYPE_DDR3	= 0x0b,
	SPD_DRAM_TYPE_LPDDR3	= 0xf1,
	SPD_DRAM_TYPE_JEDEC_LPDDR3 = 0x0f,
	SPD_DRAM_TYPE_DDR4      = 0x0c,
	SPD_DRAM_TYPE_LPDDR4	= 0x10,
	SPD_DRAM_TYPE_LPDDR4X	= 0x11,
};

enum ddr3_module_type {
	DDR3_MODULE_TYPE_UNDEFINED = 0,
	DDR3_MODULE_TYPE_RDIMM,
	DDR3_MODULE_TYPE_UDIMM,
	DDR3_MODULE_TYPE_SO_DIMM,
	DDR3_MODULE_TYPE_MICRO_DIMM,
	DDR3_MODULE_TYPE_MINI_RDIMM,
	DDR3_MODULE_TYPE_MINI_UDIMM,
	DDR3_MODULE_TYPE_MINI_CDIMM,
	DDR3_MODULE_TYPE_72b_SO_UDIMM,
	DDR3_MODULE_TYPE_72b_SO_RDIMM,
	DDR3_MODULE_TYPE_72b_SO_CDIMM,
	DDR3_MODULE_TYPE_LRDIMM,
};

static const struct valstr ddr3_module_type_lut[] = {
	{ DDR3_MODULE_TYPE_UNDEFINED, "Undefined" },
	{ DDR3_MODULE_TYPE_RDIMM, "RDIMM" },
	{ DDR3_MODULE_TYPE_UDIMM, "UDIMM" },
	{ DDR3_MODULE_TYPE_SO_DIMM, "SO-DIMM" },
	{ DDR3_MODULE_TYPE_MICRO_DIMM, "MICRO-DIMM" },
	{ DDR3_MODULE_TYPE_MINI_RDIMM, "MINI-RDIMM" },
	{ DDR3_MODULE_TYPE_MINI_UDIMM, "MINI-UDIMM" },
	{ DDR3_MODULE_TYPE_MINI_CDIMM, "MINI-CDIMM" },
	{ DDR3_MODULE_TYPE_72b_SO_UDIMM, "72b-SO-UDIMM" },
	{ DDR3_MODULE_TYPE_72b_SO_RDIMM, "72b-SO-RDIMM" },
	{ DDR3_MODULE_TYPE_72b_SO_CDIMM, "72b-SO-CDIMM" },
	{ DDR3_MODULE_TYPE_LRDIMM, "LRDIMM" },
};

enum ddr_freq {
	DDR_FREQ_UNKNOWN = 0,	/* uninitialized value */
	DDR_333,
	DDR_400,
	DDR_533,
	DDR_667,
	DDR_800,
	DDR_933,
	DDR_1067,
	DDR_1200,
	DDR_1333,
	DDR_1355,
	DDR_1466,
	DDR_1600,
	DDR_1867,
	DDR_2133,

	DDR_FREQ_MAX
};

extern const char *ddr_freq_prettyprint[];

/* spd register handlers */
struct spd_reg {
	const char *name;
	const char *units;
	const char *(*func)(struct spd_reg *reg,
			    const uint8_t *eeprom,
			    uint8_t byte);
	const char *table[256];
};

struct spd_eeprom {
	int length;
	uint8_t data[SPD_MAX_LENGTH];
};

struct spd_device {
	int dimm_num; /* DIMM number in system. */
	enum spd_dram_type dram_type; /* Fundamental DRAM type. */
	struct i2c_addr smbus; /* Address of DIMM in system. */
	struct spd_eeprom eeprom;
};

/*
 * various SPD fields that can be retrieved
 * these are found in different locations on DDR/DDR2 vs. FBDIMM
 */
enum spd_field_type {
	SPD_GET_DRAM_TYPE,      /* DRAM type */
	SPD_GET_MODULE_TYPE,    /* DIMM type */
	SPD_GET_MFG_ID,		/* Module Manufacturer ID */
	SPD_GET_MFG_ID_DRAM,	/* DRAM Manufacturer ID */
	SPD_GET_MFG_LOC,	/* Module Manufacturing Location */
	SPD_GET_MFG_DATE,	/* Module Manufacturing Date */
	SPD_GET_PART_NUMBER,	/* Module Part Number */
	SPD_GET_REVISION_CODE,	/* Module Revision Code */
	SPD_GET_SIZE,		/* Module Size (in MB) */
	SPD_GET_ECC,		/* ECC capable: boolean */
	SPD_GET_RANKS,		/* Number of ranks */
	SPD_GET_WIDTH,		/* SDRAM device width */
	SPD_GET_CHECKSUM,	/* SPD checksum */
	SPD_GET_SPEEDS,		/* module frequency capabilities */
};

/*
 * new_spd_device() - create a new instance of spd_device
 *
 * @intf:  platform_intf for access
 * @dimm:  Google logical dimm number to represent
 *
 * returns allocated and filled in spd_devices on success, NULL if error
 */
extern struct spd_device *new_spd_device(struct platform_intf *intf, int dimm);

/* add register to key=value pair */
extern int spd_print_reg(struct platform_intf *intf,
			 struct kv_pair *kv, const void *data, uint8_t reg);

/* add field to key=value pair */
extern int spd_print_field(struct platform_intf *intf,
			   struct kv_pair *kv,
			   const void *data, enum spd_field_type type);

/* print raw spd */
extern int spd_print_raw(struct kv_pair *kv, int len, uint8_t *data);

/*
 * spd_read_i2c  -  Read from SPD configuration space
 *
 * @intf:	platform interface
 * @bus:	i2c bus
 * @address:	i2c address
 * @reg:	register offset
 * @length:	number of bytes to read
 * @data:       data buffer
 *
 * returns number of bytes read
 * returns <0 to indicate error
 */
extern int spd_read_i2c(struct platform_intf *intf, int bus,
                        int addr, int reg, int length, void *data);

/* spd_raw_access - read/write access method to SPDs
 *
 * @intf:  platform interface
 * @bus:  SMBus number of requested SPD
 * @address: SMBus address of requested SPD
 * @reg:  register in SPD to perform access on
 * @length:  length of access to perform
 * @data: pointer to buffer that is either filled or read from to do the access
 * @rw:  specify SPD operation (SPD_READ or SPD_WRITE)
 *
 * returns 0 on success, < 0 on error.
 */
extern int spd_raw_access(struct platform_intf *intf, int bus, int address,
                          int reg, int length, void *data, int rw);

/*
 * spd_total_size  -  determine total bytes in spd from first few bytes
 *
 * @data:	spd data
 *
 * returns total size of SPD, may be less than max depending on type of module
 * returns <0 to indicate failure
 *
 */
extern int spd_total_size(uint8_t *data);

/*
 * SPD register and field callbacks.
 */

/* Common register printing callbacks. */
extern const char *spd_revision_code(struct spd_reg *reg,
                                     const uint8_t * eeprom, uint8_t byte);
extern const char *spd_shift_by(struct spd_reg *reg,
                                const uint8_t * eeprom, uint8_t byte);
extern const char *spd_module_bank_density(struct spd_reg *reg,
                                           const uint8_t * eeprom,
                                           uint8_t byte);
extern const char *spd_shift_access_time(struct spd_reg *reg,
                                         const uint8_t * eeprom, uint8_t byte);
extern const char *spd_decimal_access_time(struct spd_reg *reg,
                                           const uint8_t * eeprom,
                                           uint8_t byte);
extern const char *spd_burst(struct spd_reg *reg, const uint8_t * eeprom,
                             uint8_t byte);
extern const char *spd_readhex(struct spd_reg *reg,
                               const uint8_t * eeprom, uint8_t byte);
extern const char *spd_readbyte(struct spd_reg *reg,
                                const uint8_t * eeprom, uint8_t byte);
extern const char *spd_table_lookup(struct spd_reg *reg,
                                    const uint8_t * eeprom, uint8_t byte);

/*
 * spd_print_field_ddr3  -  add common DDR SPD fields into key=value pair
 *
 * @intf:       platform interface
 * @kv:         key=value pair
 * @data:       raw spd data
 * @type:       type of field to retrieve
 *
 * returns 1 to indicate data added to key=value pair
 * returns 0 to indicate no data added
 * returns <0 to indicate error
 *
 */
extern int spd_print_field_ddr3(struct platform_intf *intf, struct kv_pair *kv,
                                const void *data, enum spd_field_type type);

/*
 * spd_print_field_ddr4  -  add common DDR SPD fields into key=value pair
 *
 * @intf:       platform interface
 * @kv:         key=value pair
 * @data:       raw spd data
 * @type:       type of field to retrieve
 *
 * returns 1 to indicate data added to key=value pair
 * returns 0 to indicate no data added
 * returns <0 to indicate error
 *
 */
extern int spd_print_field_ddr4(struct platform_intf *intf, struct kv_pair *kv,
                                const void *data, enum spd_field_type type);

/*
 * spd_read_from_cbfs  -  retrieve SPD info from CBFS
 *
 * @intf:	platform interface
 * @module:	module number
 * @reg:	SPD register offset
 * @spd_len:	length of SPD data
 * @spd:	raw SPD data
 * @fw_size:	size of firmware image
 * @fw:		firmware image
 *
 * returns 0 to indicate success
 * returns <0 to indicate error
 */
extern int spd_read_from_cbfs(struct platform_intf *intf,
			      int module, int reg, int spd_len, uint8_t *spd,
			      size_t fw_size, uint8_t *fw);

/*
 * spd_read_cbfs_flashrom  -  retrieve SPD info from flashrom
 *
 * @intf:	platform interface
 * @dimm:	dimm number
 * @reg:	SPD register offset
 * @spd_len:	length of SPD data
 * @spd_buf:	raw SPD data buffer
 *
 * returns 0 to indicate success
 * returns <0 to indicate error
 */
extern int spd_read_cbfs_flashrom(struct platform_intf *intf, int dimm,
			    	  int reg, int spd_len, uint8_t *spd_buf);

extern struct memory_spd_cb memory_spd_cbfs;
#endif /* LIB_SPD_H__ */
