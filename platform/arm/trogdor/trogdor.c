/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "trogdor.h"
#include "drivers/google/cros_ec.h"
#include "lib/cros_config.h"
#include "lib/eeprom.h"
#include "lib/sku.h"
#include "lib/generic_callbacks.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"

static struct platform_cmd *trogdor_sub[] = {
	&cmd_ec,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int trogdor_probe(struct platform_intf *intf)
{
	return cros_config_probe(intf, NULL);
}

static struct eventlog_cb trogdor_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.add		= &elog_add_event_manually,
	.clear		= &elog_clear_manually,
	.fetch		= &elog_fetch_from_flash,
	.write		= &elog_write_to_flash,
};

static struct platform_cb trogdor_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &trogdor_eeprom_cb,
	.memory		= &trogdor_memory_cb,
	.nvram		= &cros_spi_flash_nvram_cb,
	.psu 		= &generic_psu_battery_cb,
	.sys 		= &trogdor_sys_cb,
	.eventlog	= &trogdor_eventlog_cb,
};

/* TODO(crbug.com/1070692): make static */
extern struct platform_intf platform_trogdor;

struct platform_intf platform_trogdor = {
	.type		= PLATFORM_ARMV8,
	.name		= "Trogdor",
	.sub		= trogdor_sub,
	.cb		= &trogdor_cb,
	.probe		= &trogdor_probe,
};
