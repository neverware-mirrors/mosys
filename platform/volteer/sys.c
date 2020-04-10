/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/smbios.h"
#include "mosys/platform.h"

struct sys_cb volteer_sys_cb = {
	.version		= &smbios_sysinfo_get_version,
	.vendor			= &smbios_sysinfo_get_vendor,
	.firmware_vendor	= &smbios_bios_get_vendor,
	.sku_number		= &smbios_sysinfo_get_sku_number,
};
