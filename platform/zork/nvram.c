/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <valstr.h>

#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/output.h"
#include "mosys/platform.h"

#include "intf/io.h"

#include "lib/eeprom.h"
#include "lib/math.h"

enum cmos_device {
	CMOS_DEVICE_FCH,
};

struct cmos_var_map {
	uint8_t offset;
	uint8_t length;
	char *desc;
};

struct cmos_map {
	enum cmos_device type;
	const char *device;
	int bank;
	int length;
	int clear_start; /* first bytes are usually reserved for RTC */
	struct cmos_var_map *var_list;
};

static struct cmos_var_map coreboot_cmos_bank0_vars[] = {
	{ 0x70, 1, "Post Code Bank" },	  { 0x71, 1, "Post Code Bank 0" },
	{ 0x72, 1, "Post Code Bank 1" },  { 0x73, 4, "Post Extra Bank 0" },
	{ 0x77, 4, "Post Extra Bank 1" }, { 0 }
};

static struct cmos_var_map coreboot_cmos_bank1_vars[] = {
	{ 0x12, 4, "Boot Count" }, /* 0x92 */
	{ 0 }
};

struct cmos_map zork_cmos_map[] = {
	{ CMOS_DEVICE_FCH, "CMOS", 0, 128, 0x29, coreboot_cmos_bank0_vars },
	{ CMOS_DEVICE_FCH, "CMOS_RAM", 1, 128, 0x00, coreboot_cmos_bank1_vars },
};

static const uint16_t zork_cmos_port[] = { 0x70, 0x72 };

static uint8_t zork_read_cmos(struct platform_intf *intf, int addr, int reg)
{
	uint8_t data;

	if (addr >= ARRAY_SIZE(zork_cmos_port)) {
		lperror(LOG_ERR, "Error: Invalid address (%d) in %s\n", addr,
			__func__);
		return 0;
	}

	io_write8(intf, zork_cmos_port[addr], reg);
	io_read8(intf, zork_cmos_port[addr] + 1, &data);
	return data;
}

static void zork_write_cmos(struct platform_intf *intf, int addr, int reg,
			    uint8_t val)
{
	if (addr >= ARRAY_SIZE(zork_cmos_port)) {
		lperror(LOG_ERR, "Error: Invalid address (%d) in %s\n", addr,
			__func__);
		return;
	}
	io_write8(intf, zork_cmos_port[addr], reg);
	io_write8(intf, zork_cmos_port[addr] + 1, val);
}

static int zork_nvram_list_bank(struct platform_intf *intf,
				struct cmos_map *map)
{
	struct cmos_var_map *var;
	int i;

	/* handle each cmos bank */
	for (var = map->var_list; var && var->desc; var++) {
		struct kv_pair *kv = kv_pair_new();
		uint32_t val = 0;

		switch (map->type) {
		case CMOS_DEVICE_FCH:
			for (i = 0; i < var->length; i++)
				val |= zork_read_cmos(intf, map->bank,
						      var->offset + i)
				       << (i * 8);
			break;
		default:
			lperror(LOG_ERR, "Error: Invalid type (%d) in %s\n",
				map->type, __func__);
			return -1;
		}

		kv_pair_add(kv, "device", map->device);
		kv_pair_add(kv, "name", var->desc);
		kv_pair_fmt(kv, "value", "0x%x", val);
		kv_pair_print(kv);
		kv_pair_free(kv);
	}

	return 0;
}

static int zork_nvram_list(struct platform_intf *intf)
{
	int dev, rc = 0;

	/* handle each cmos bank */
	for (dev = 0; dev < ARRAY_SIZE(zork_cmos_map); dev++)
		rc |= zork_nvram_list_bank(intf, &zork_cmos_map[dev]);

	return rc;
}

static int zork_nvram_dump(struct platform_intf *intf)
{
	struct cmos_map *map;
	int off, dev;
	uint8_t cmos_data[128];

	/* handle each cmos bank */
	for (dev = 0; dev < ARRAY_SIZE(zork_cmos_map); dev++) {
		map = &zork_cmos_map[dev];

		if (map->length > sizeof(cmos_data))
			return -1;
		memset(cmos_data, 0, sizeof(cmos_data));

		mosys_printf("%s CMOS Bank %d (%d bytes)\n", map->device,
			     map->bank, map->length);

		switch (map->type) {
		case CMOS_DEVICE_FCH:
			for (off = 0; off < map->length; off++)
				cmos_data[off] =
					zork_read_cmos(intf, map->bank, off);
			break;
		default:
			lperror(LOG_ERR, "Error: Invalid type (%d) in %s\n",
				map->type, __func__);
			return -1;
		}

		print_buffer(cmos_data, map->length);
		mosys_printf("\n");
	}

	return 0;
}

static int zork_nvram_clear(struct platform_intf *intf)
{
	struct cmos_map *map;
	int off, dev;

	/* handle each cmos bank */
	for (dev = 0;
	     dev < ARRAY_SIZE(zork_cmos_map);
	     dev++) {
		map = &zork_cmos_map[dev];

		switch (map->type) {
		case CMOS_DEVICE_FCH:
			for (off = map->clear_start; off < map->length; off++)
				zork_write_cmos(intf, map->bank, off, 0x00);
			break;
		default:
			lperror(LOG_ERR, "Error: Invalid type (%d) in %s\n",
				map->type, __func__);
			return -1;
		}
	}

	return 0;
}

struct nvram_cb zork_nvram_cb = {
	.list = zork_nvram_list,
	.dump = zork_nvram_dump,
	.clear = zork_nvram_clear,
	.vboot_read = vbnv_flash_vboot_read,
	.vboot_write = vbnv_flash_vboot_write,
};
