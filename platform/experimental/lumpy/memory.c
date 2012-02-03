/*
 * Copyright (C) 2011 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <limits.h>	/* for INT_MAX */

#include <arpa/inet.h>	/* ntohl() */

#include "mosys/alloc.h"
#include "mosys/callbacks.h"
#include "mosys/globals.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "drivers/intel/series6.h"

#include "lib/cbfs_core.h"
#include "lib/file.h"
#include "lib/flashrom.h"
#include "lib/math.h"
#include "lib/spd.h"

#include "lumpy.h"

#define LUMPY_DIMM_COUNT	2

/*
 * lumpy_dimm_count  -  return total number of dimm slots
 *
 * @intf:       platform interface
 *
 * returns dimm slot count
 */
static int lumpy_dimm_count(struct platform_intf *intf)
{
	return LUMPY_DIMM_COUNT;
}

/*
 * dimm_map  -  Convert logical dimm number to useful values
 *
 * @intf:       platform interface
 * @dimm:       logical dimm number
 * @type:       conversion type
 *
 * returns specified converted value
 * returns <0 to indicate error
 */
static int lumpy_i2c_dimm_map(struct platform_intf *intf,
                              enum dimm_map_type type, int dimm)
{
	int ret = -1;
	/* Lumpy can have two DIMMs, but only one will be on I2C */
	static struct dimm_map {
		int node;
		int channel;
		int slot;
		int bus;
		int address;
	} lumpy_dimm_map[] = {
		/* Node 0 */
		{ 0, 0, 0, 15, 0x50 }
	};
	static unsigned int first_run = 1;
	static int bus_offset = 0;

	if (dimm < 0 || dimm >= intf->cb->memory->dimm_count(intf)) {
		lprintf(LOG_ERR, "Invalid DIMM: %d\n", dimm);
		return -1;
	}

	/*
	 * Determine offset for smbus numbering:
	 * 1. Scan known bus numbers for lowest value.
	 * 2. Scan /sys for SMBus entries that match the adapter name.
	 * 3. Calculate the difference between the lowest expected bus number
	 *    and the lowest bus number seen in sysfs matching the criteria.
	 */
	if (first_run) {
		char path[PATH_MAX];
		int lowest_known_bus = INT_MAX, x;

		if (lumpy_dimm_map[0].bus < lowest_known_bus)
			lowest_known_bus = lumpy_dimm_map[0].bus;

		snprintf(path, sizeof(path), "%s/%s",
		         mosys_get_root_prefix(), "/sys/bus/i2c/devices");
		x = sysfs_lowest_smbus(path, SERIES6_SMBUS_ADAPTER);
		if (x >= 0) {
			bus_offset = x - lowest_known_bus;
			lprintf(LOG_DEBUG, "%s: bus_offset: %d\n",
			        __func__, bus_offset);
		} else {
			lprintf(LOG_DEBUG, "%s: unable to determine "
			                   "bus offset\n", __func__);
			bus_offset = 0;
		}

		first_run = 0;
	}

	switch (type) {
	case DIMM_TO_BUS:
		ret = lumpy_dimm_map[0].bus + bus_offset;
		break;
	case DIMM_TO_ADDRESS:
		ret = lumpy_dimm_map[0].address;
		break;
	default:
		break;
	}

	return ret;
}

static int lumpy_spd_read_cbfs(struct platform_intf *intf,
                               int dimm, int reg, int len, uint8_t *buf)
{
	int rc = -1;
	static int first_run = 1;
	static uint8_t *bootblock = NULL;
	size_t size = LUMPY_HOST_FIRMWARE_ROM_SIZE;
	struct cbfs_file *file;

	if (first_run == 1) {
		bootblock = mosys_malloc(size);	/* FIXME: overkill */
		add_destroy_callback(free, bootblock);
		first_run = 0;

		/* read SPD from CBFS entry located within bootblock region */
		if (flashrom_read(bootblock, size,
		                  INTERNAL_BUS_SPI, "BOOT_STUB") < 0)
			goto lumpy_spd_read_cbfs_exit;
	}

	if ((file = cbfs_find("spd.bin", bootblock, size)) == NULL)
		goto lumpy_spd_read_cbfs_exit;

	memcpy(buf, (void *)file + ntohl(file->offset) + reg, len);
	rc = len;
lumpy_spd_read_cbfs_exit:
	return rc;
}

static int lumpy_spd_read_i2c(struct platform_intf *intf,
                              int dimm, int reg, int len, uint8_t *buf)
{
	int bus;
	int address;

	/* read SPD from slotted DIMM if available */
	bus = intf->cb->memory->dimm_map(intf, DIMM_TO_BUS, dimm);
	address = intf->cb->memory->dimm_map(intf, DIMM_TO_ADDRESS, dimm);
	return spd_read_i2c(intf, bus, address, reg, len, buf);
}

static int lumpy_spd_read(struct platform_intf *intf,
                          int dimm, int reg, int len, uint8_t *buf)
{
	int ret = -1;

	/*
	 * Lumpy has two memory modules: One on-board, and an optional slotted
	 * DIMM. We'll call the on-board module "dimm 0" and the optional DIMM
	 * "dimm 1".
	 */
	if (dimm == 0) {
		if (intf->cb->sys && intf->cb->sys->firmware_vendor) {
			const char *bios = intf->cb->sys->firmware_vendor(intf);
			if (bios && !strcasecmp(bios, "coreboot"))
				ret = lumpy_spd_read_cbfs(intf, dimm,
				                          reg, len, buf);
			free((void *)bios);
		}
	} else if (dimm == 1)
		ret = lumpy_spd_read_i2c(intf, dimm, reg, len, buf);
	else
		lprintf(LOG_DEBUG, "%s: Invalid DIMM: %d\n", __func__, dimm);

	return ret;
}

static struct memory_spd_cb lumpy_spd_cb = {
	.read		= lumpy_spd_read,
};

struct memory_cb lumpy_memory_cb = {
	.dimm_count	= lumpy_dimm_count,
	.dimm_map	= lumpy_i2c_dimm_map,
	.spd		= &lumpy_spd_cb,
};
