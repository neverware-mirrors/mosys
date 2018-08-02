// Copyright 2018 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <unistd.h>

#include <fmap.h>

#include "mosys/alloc.h"
#include "mosys/platform.h"
#include "mosys/log.h"

#include "lib/eeprom.h"
#include "lib/flashrom.h"
#include "lib/smbios.h"

#include "dummy.h"

static int dummy_host_firmware_size(struct platform_intf *intf)
{
	/*
	 * FIXME: use libflashrom for this. SMBIOS won't work because it
	 * reports the actual BIOS rather than the ROM size which includes
	 * ME/GbE/etc.
	 */
	return DUMMY_HOST_FIRMWARE_ROM_SIZE;
}

static int dummy_host_firmware_read(struct platform_intf *intf,
                                  struct eeprom *eeprom,
                                  unsigned int offset,
                                  unsigned int len,
                                  void *data)
{
	return 0;
}

static struct eeprom_dev dummy_host_firmware = {
	.size		= dummy_host_firmware_size,
	.read		= dummy_host_firmware_read,
	.get_map	= eeprom_get_fmap,
};

static struct eeprom dummy_eeproms[] = {
	{
		.name		= "host_firmware",
		.type		= EEPROM_TYPE_FW,
		.flags		= EEPROM_FLAG_RDWR,
		.device		= &dummy_host_firmware,
	},
	{ 0 },
};

int dummy_eeprom_setup(struct platform_intf *intf)
{
	int rc = 0;

	return rc;
}

struct eeprom_cb dummy_eeprom_cb = {
	.eeprom_list	= dummy_eeproms,
};
