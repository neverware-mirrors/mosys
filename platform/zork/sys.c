/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "mosys/alloc.h"
#include "mosys/platform.h"

#include "lib/smbios.h"
#include "lib/string.h"

static char *zork_get_name(struct platform_intf *intf)
{
	return mosys_strdup(intf->name);
}

struct sys_cb zork_sys_cb = {
	.version = &smbios_sysinfo_get_version,
	.vendor = &smbios_sysinfo_get_vendor,
	.name = &zork_get_name,
	.firmware_vendor = &smbios_bios_get_vendor,
	.sku_number = &smbios_sysinfo_get_sku_number,
};
