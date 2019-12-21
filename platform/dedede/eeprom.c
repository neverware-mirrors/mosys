/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <unistd.h>

#include <fmap.h>

#include "mosys/alloc.h"
#include "mosys/platform.h"
#include "mosys/log.h"

#include "lib/eeprom.h"
#include "lib/flashrom.h"
#include "lib/smbios.h"

#include "dedede.h"


static int host_firmware_size(struct platform_intf *intf)
{
	return DEDEDE_HOST_FIRMWARE_ROM_SIZE;
}

static int host_firmware_read(struct platform_intf *intf,
			      struct eeprom *eeprom,
			      unsigned int offset,
			      unsigned int len,
			      void *data)
{
	uint8_t *buf;
	size_t rom_size;

	rom_size = eeprom->device->size(intf);
	buf = mosys_malloc(rom_size);

	if (flashrom_read(buf, rom_size, HOST_FIRMWARE, NULL) < 0)
		return -1;

	memcpy(data, &buf[offset], len);
	free(buf);
	return 0;
}

static int host_firmware_read_by_name(struct platform_intf *intf,
				      struct eeprom *eeprom,
				      const char *name,
				      uint8_t **data)
{
	return flashrom_read_by_name(data, HOST_FIRMWARE, name);
}

static int host_firmware_write_by_name(struct platform_intf *intf,
				       struct eeprom *eeprom,
				       const char *name,
				       unsigned int len,
				       uint8_t *data)
{
	return flashrom_write_by_name(len, data, HOST_FIRMWARE, name);
}

static struct eeprom_dev host_firmware = {
	.size		= host_firmware_size,
	.read		= host_firmware_read,
	.read_by_name	= host_firmware_read_by_name,
	.write_by_name	= host_firmware_write_by_name,
	.get_map	= eeprom_get_fmap,
};

static struct eeprom_region host_firmware_regions[] = {
	{
		.name	= "RW_NVRAM",
		.flag	= EEPROM_FLAG_VBNV,
	},
	{ 0 },
};

static struct eeprom eeproms[] = {
	{
		.name		= "host_firmware",
		.type		= EEPROM_TYPE_FW,
		.flags		= EEPROM_FLAG_RDWR | EEPROM_FLAG_FMAP,
		.device		= &host_firmware,
		.regions	= &host_firmware_regions[0],
	},
	{ 0 },
};

struct eeprom_cb dedede_eeprom_cb = {
	.eeprom_list	= eeproms,
};
