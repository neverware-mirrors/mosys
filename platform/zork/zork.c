/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "lib/memory.h"

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/intf_list.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"

#include "lib/cros_config.h"
#include "lib/eeprom.h"
#include "lib/elog.h"
#include "lib/smbios.h"

#include "zork.h"

static struct platform_cmd *zork_sub[] = { &cmd_ec,	  &cmd_eeprom,
					   &cmd_memory,	  &cmd_nvram,
					   &cmd_pd,	  &cmd_platform,
					   &cmd_eventlog, NULL };

static int zork_probe(struct platform_intf *intf)
{
	/* cros_config model.yaml 'platform-name' should match intf.name. */
	return cros_config_probe(intf, NULL);
}

/* late setup routine; not critical to core functionality */
static int zork_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	return 0;
}

static struct eventlog_cb zork_eventlog_cb = {
	.print_type = &elog_print_type,
	.print_data = &elog_print_data,
	.print_multi = &elog_print_multi,
	.verify = &elog_verify,
	.verify_header = &elog_verify_header,
	.fetch = &elog_fetch_from_smbios,
};

static struct platform_cb zork_cb = {
	.ec = &cros_ec_cb,
	.eeprom = &zork_eeprom_cb,
	.memory = &smbios_memory_cb,
	.nvram = &cros_spi_flash_nvram_cb,
	.smbios = &smbios_sysinfo_cb,
	.sys = &zork_sys_cb,
	.eventlog = &zork_eventlog_cb,
};

struct platform_intf platform_zork = {
	.type = PLATFORM_X86_64,
	.name = "Zork",
	.sub = zork_sub,
	.cb = &zork_cb,
	.probe = &zork_probe,
	.setup_post = &zork_setup_post,
};
