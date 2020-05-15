/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"

#include "drivers/google/cros_ec.h"

#include "lib/cros_config.h"
#include "lib/eeprom.h"
#include "lib/elog.h"
#include "lib/memory.h"

#include "volteer.h"

static struct platform_cmd *volteer_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_pd,
	&cmd_platform,
	&cmd_eventlog,
	NULL
};

static int volteer_probe(struct platform_intf *intf)
{
	/* cros_config model.yaml 'platform-name' should match intf.name. */
	return cros_config_probe(intf, NULL);
}

static struct eventlog_cb volteer_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

static struct platform_cb volteer_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &volteer_eeprom_cb,
	.memory		= &smbios_memory_cb,
	.nvram		= &cros_spi_flash_nvram_cb,
	.sys 		= &volteer_sys_cb,
	.eventlog	= &volteer_eventlog_cb,
};

/* TODO(crbug.com/1070692): make static */
extern struct platform_intf platform_volteer;

struct platform_intf platform_volteer = {
	.type		= PLATFORM_X86_64,
	.name		= "Volteer",
	.sub		= volteer_sub,
	.cb		= &volteer_cb,
	.probe		= &volteer_probe,
};
