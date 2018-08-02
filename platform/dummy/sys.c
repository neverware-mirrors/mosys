// Copyright 2018 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef CONFIG_CROS_CONFIG

#include "mosys/alloc.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"

#include "lib/sku.h"
#include "lib/smbios.h"

static char *dummy_get_name(struct platform_intf *intf)
{
	return mosys_strdup(intf->name);
}

struct sys_cb dummy_sys_cb = {
	.version		= &cros_ec_board_version_str,
	.vendor			= &smbios_sysinfo_get_vendor,
	.name			= &dummy_get_name,
	.family			= &smbios_sysinfo_get_family,
	.firmware_vendor	= &smbios_bios_get_vendor,
	.sku_number		= &smbios_sysinfo_get_sku_number,
	.signature_id           = sku_get_signature_id,
};
#endif /* CONFIG_CROS_CONFIG */
