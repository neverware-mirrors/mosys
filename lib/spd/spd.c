/* Copyright 2012, Google Inc.
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
 * DIMM Serial Presence Detect interface normal access is via I2C but some
 * platforms hide SPD behind memory controller and other access method must be
 * used.
 */

#define _LARGEFILE64_SOURCE
#include <arpa/inet.h>	/* ntohl() */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "mosys/alloc.h"
#include "mosys/list.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "intf/i2c.h"
#include "lib/cbfs_core.h"
#include "lib/math.h"
#include "lib/smbios.h"
#include "lib/smbios_tables.h"
#include "lib/spd.h"

static spd_raw_override spd_raw_access_override;

const char *ddr_freq_prettyprint[] = {
	[DDR_FREQ_UNKNOWN] = "Unknown",
	[DDR_333] = "667",
	[DDR_400] = "800",
	[DDR_533] = "1066",
	[DDR_667] = "1333",
	[DDR_800] = "1600",
	[DDR_933] = "1866",
	[DDR_1067] = "2133",
	[DDR_1355] = "2710",
	[DDR_1400] = "2400",
	[DDR_1600] = "3200",
};

/*
 * spd_raw_i2c  -  Read/write to/from SPD via I2C
 *
 * @intf:	platform interface
 * @bus:        bus to read from
 * @address:    address on bus
 * @reg:	configuration register
 * @length:	number of bytes to read
 * @data:       data buffer
 * @rw:         1=write, 0=read
 *
 * returns number of bytes written or read
 * returns <0 to indicate error
 */
static int spd_raw_i2c(struct platform_intf *intf, int bus,
		       int address, int reg, int length, void *data, int rw)
{
	if (!intf->op->i2c) {
		return -ENOSYS;
	}

	switch (rw) {
	case SPD_READ:
		return intf->op->i2c->smbus_read_reg(intf, bus, address,
					             reg, length, data);
	case SPD_WRITE:
		return intf->op->i2c->smbus_write_reg(intf, bus, address,
						      reg, length, data);
	}

	return -1;
}

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
int spd_raw_access(struct platform_intf *intf, int bus, int address,
                   int reg, int length, void *data, int rw)
{
	if (spd_raw_access_override != NULL) {
		return spd_raw_access_override(intf, bus, address,
		                               reg, length, data, rw);
	}

	return spd_raw_i2c(intf, bus, address, reg, length, data, rw);
}

int spd_read_i2c(struct platform_intf *intf, int bus,
                 int address, int reg, int length, void *data)
{
	const char module[] = "eeprom";

	if (!intf->cb->memory || !intf->cb->memory->dimm_map)
		return -1;

	/* Read info from /sys */
	if (intf->op->i2c->find_driver(intf, module)) {
		FILE *fp;
		int ret;
		char path[80];

		/* Get the data */
		snprintf(path, sizeof(path), "%s/%u-%04x/%s",
			 intf->op->i2c->sys_root, bus, address, module);
		if (!(fp = fopen(path, "r"))) {
			lprintf(LOG_DEBUG,
				"Failed to open %s\n", path);
			return -1;
		}

		fseek(fp, reg, SEEK_SET);
		ret = fread(data, 1, length, fp);
		fclose(fp);

		return ret;
	} else 	{
		return spd_raw_access(intf, bus, address, reg,
		                      length, data, SPD_READ);
	}

	return -1;
}

#if 0
int spd_write_i2c(struct platform_intf *intf,
                  int dimm, int reg, int length, const void *data)
{
	int bus, address;

	if (!intf->cb->memory->dimm_map)
		return -1;

	/* convert logical dimm map */
	bus = intf->cb->memory->dimm_map(intf, DIMM_TO_BUS, dimm);
	if (bus < 0)
		return -1;

	address = intf->cb->memory->dimm_map(intf, DIMM_TO_ADDRESS, dimm);
	if (address < 0)
		return -1;

	//FIXME: we cast data to remove the const - can we do better?
	return intf->cb->memory->spd->write(intf, bus, address, reg,
	                                    length, (void *)data);
}
#endif

int override_spd_raw_access(spd_raw_override override)
{
	spd_raw_access_override = override;
	return 0;
}

/* new_spd_device() - create a new instance of spd_device
 *
 * @intf:  platform_intf for access
 * @dimm:  Google logical dimm number to represent
 *
 * returns allocated and filled in spd_devices on success, NULL if error
 */
struct spd_device *new_spd_device(struct platform_intf *intf, int dimm)
{
	struct spd_device *spd;

	if (intf == NULL || dimm < 0) {
		return NULL;
	}

	spd = mosys_malloc(sizeof(*spd));
	spd->dimm_num = dimm;
	memset(&spd->eeprom.data[0], 0xff, SPD_MAX_LENGTH);

	if (intf->cb->memory->spd->read(intf, dimm, 0, 3,
	                                &spd->eeprom.data[0]) != 3) {
		free(spd);
		return NULL;
	}

	spd->dram_type = (enum spd_dram_type)spd->eeprom.data[2];
	spd->eeprom.length = spd_total_size(&spd->eeprom.data[0]);

	/* Invalid length. */
	if (spd->eeprom.length <= 0) {
		lperror(LOG_DEBUG, "Invalid DIMM(%d) SPD length(%d).\n",
		        dimm, spd->eeprom.length);
		free(spd);
		return NULL;
	}

	/* Fill in copy of SPD eeprom area. */
	if (intf->cb->memory->spd->read(intf, dimm, 0,
	                                spd->eeprom.length,
	                                &spd->eeprom.data[0])
			!= spd->eeprom.length) {
		lperror(LOG_DEBUG,
		        "Unable to read full contents of SPD from DIMM %d.\n",
		        dimm);
		free(spd);
		return NULL;
	}

	return spd;
}

enum {
	SPD_INFO_DDR4,
	SPD_INFO_DEFAULT,
};

static const struct spd_info {
	size_t spd_len;
	uint32_t spd_part_off;
	size_t spd_part_len;
} spd_mem_info[] = {
	[SPD_INFO_DDR4] = {
		.spd_len = SPD_DDR4_LENGTH,
		.spd_part_off = SPD_DDR4_PART_OFF,
		.spd_part_len = SPD_DDR4_PART_LEN,
	},
	[SPD_INFO_DEFAULT] = {
		.spd_len = SPD_DEFAULT_LENGTH,
		.spd_part_off = SPD_DEFAULT_PART_OFF,
		.spd_part_len = SPD_DEFAULT_PART_LEN,
	},
};

static ssize_t find_spd_by_part_number(struct platform_intf *intf, int dimm,
				       uint8_t *spd, size_t spd_file_len)
{
	const char *smbios_part_num;
	size_t smbios_part_num_len, spd_part_num_len;
	ssize_t ret = -1, offset;
	uint8_t i, num_spd;
	uint8_t *ptr, *spd_part_num;
	struct smbios_table *table = mosys_malloc(sizeof(*table));
	const struct spd_info *info = &spd_mem_info[SPD_INFO_DEFAULT];

	lprintf(LOG_DEBUG, "Use SMBIOS type 17 to get memory information\n");
	if (smbios_find_table(intf, SMBIOS_TYPE_MEMORY, dimm, table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0) {
		lprintf(LOG_DEBUG, "Can't find smbios type17\n");
		goto out;
	}

	if (table->data.mem_device.type == SMBIOS_MEMORY_TYPE_DDR4)
		info = &spd_mem_info[SPD_INFO_DDR4];

	smbios_part_num = table->string[table->data.mem_device.part_number];
	smbios_part_num_len = strnlen(smbios_part_num, info->spd_part_len);

	if (smbios_part_num_len == 0) {
		lprintf(LOG_DEBUG, "SMBIOS type 17 is missing part number\n");
		goto out;
	}

	// Legacy firmware doesn't remove trailing whitespaces from SPD part
	// number so needs to check again; otherwise the part number might never
	// matches to spd_part_num[] below which did remove trailing whitespaces.
	while (smbios_part_num_len > 0 &&
	       smbios_part_num[smbios_part_num_len - 1] == ' ')
		smbios_part_num_len--;

	num_spd = spd_file_len / info->spd_len;

	for (i = 0; i < num_spd; i++) {
		offset = i * info->spd_len;
		ptr = spd + offset;
		spd_part_num = ptr + info->spd_part_off;
		spd_part_num_len = info->spd_part_len;

		// Strip off trailing whitespace from SPD part number.
		while (spd_part_num_len > 0 &&
		       spd_part_num[spd_part_num_len - 1] == ' ')
			spd_part_num_len--;

		if (spd_part_num_len != smbios_part_num_len)
			continue;

		if (!memcmp(smbios_part_num, spd_part_num,
			    smbios_part_num_len)) {
			lprintf(LOG_DEBUG, "Using memory config %u\n", i);
			ret = offset;
			break;
		}
	}

out:
	free(table);
	return ret;
}

static int _spd_read_from_cbfs(const char *spd_cbfs_filename,
				struct platform_intf *intf,
				int module, int reg, int num_bytes_to_read,
				uint8_t *spd, size_t fw_size, uint8_t *fw)
{
	struct cbfs_file *file;
	size_t file_len;
	ssize_t spd_offset;
	uint8_t *ptr;

	lprintf(LOG_DEBUG, "Read SPD %s from cbfs\n", spd_cbfs_filename);

	if ((file = cbfs_find(spd_cbfs_filename, fw, fw_size)) == NULL)
		return -1;

	ptr = (uint8_t *)file + ntohl(file->offset);
	file_len = ntohl(file->len);

	spd_offset = find_spd_by_part_number(intf, module, ptr, file_len);
	if (spd_offset < 0)
		return -1;

	MOSYS_CHECK(spd_offset + reg + num_bytes_to_read <= file_len);

	memcpy(spd, ptr + spd_offset + reg, num_bytes_to_read);

	return num_bytes_to_read;
}

int spd_read_from_cbfs(struct platform_intf *intf,
			int module, int reg, int num_bytes_to_read,
			uint8_t *spd, size_t fw_size, uint8_t *fw)
{
	const char *spd_cbfs_files[] = {
		"spd.bin",
		"sec-spd.bin",
	};
	int bytes_to_read;
	int i;

	for (i = 0; i < ARRAY_SIZE(spd_cbfs_files); i++) {
		bytes_to_read = _spd_read_from_cbfs(spd_cbfs_files[i], intf,
						    module, reg,
						    num_bytes_to_read, spd,
						    fw_size, fw);
		if (bytes_to_read >= 0)
			return bytes_to_read;
	}
	return -1;
}
