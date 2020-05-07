/*
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/cros_ec.h"

#include "lib/cros_config.h"
#include "lib/eeprom.h"
#include "lib/memory.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "reef.h"

#ifndef CONFIG_CROS_CONFIG

/* sku_info: brand, model, chassis, customization */
static struct sku_info
	/* Reef SKUs */
	/* TODO(hungte) Reef firmware updater was using 'reef' as model name
	 * for Electro and Basking. We should remove the 'reef' below when
	 * updater configuration is corrected.
	 */
	SKU_ELECTRO = { .brand = "ACBB", .model = "reef", .chassis = "ELECTRO"},
	SKU_BASKING = { .brand = "ASUN", .model = "reef", .chassis = "BASKING"},
	/* Snappy SKUs */
	SKU_ALAN = { .brand = "CLSF", .model = "snappy", .chassis = "ALAN"},
	SKU_BIGDADDY = { .brand = "FQZI", .model = "snappy", .chassis = "BIGDADDY"},
	SKU_SNAPPY = { .brand = "HPZO", .model = "snappy", .chassis = "SNAPPY"};
/* Reference: b/35583395 */
static struct sku_mapping reef_sku_table[] = {
	{0, &SKU_BASKING},
	{8, &SKU_ELECTRO},
	{SKU_NUMBER_ANY, NULL},
};

/* Reference: b/65339688 */
static struct sku_mapping snappy_sku_table[] = {
	{2, &SKU_BIGDADDY},
	{5, &SKU_BIGDADDY},
	{7, &SKU_ALAN},
	{8, &SKU_SNAPPY},
	{SKU_NUMBER_ANY, NULL},
};

struct probe_ids {
	const char *names[2];
	/**
	 * Devices with SKU-based mapping should define sku_table,
	 * otherwise use single_sku.
	 */
	struct sku_mapping *sku_table;
	const struct sku_info single_sku;
};

static const struct probe_ids probe_id_list[] = {
	/* For coral SKU map, please see cs/f:dtsi f:coral */
	{ { "Reef", }, .sku_table = reef_sku_table },
	{ { "Pyro", }, .single_sku = { .brand = "LEAN", .model = "pyro" } },
	{ { "Sand", }, },
	{ { "Snappy", }, .sku_table = snappy_sku_table },
	{ { "Coral", }, },
	{ { NULL }, },
};

#endif /* CONFIG_CROS_CONFIG */

struct platform_cmd *reef_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_pd,
	&cmd_platform,
	&cmd_eventlog,
	NULL
};

int reef_probe(struct platform_intf *intf)
{

#ifdef CONFIG_CROS_CONFIG
	/**
	 * 'reef' interface is shared by multiple overlays (with different
	 * platform-name value in cros_config model.yaml 'platform-name')
	 * so we have to pass explicit platform (overlay) names.
	 */
	const char *platform_names[] = {
		"Reef", "Pyro", "Sand", "Snappy",
		NULL
	};

	return cros_config_probe(intf, platform_names);
#else
	static int status, probed;
	const struct probe_ids *pid;

	if (probed)
		return status;

	for (pid = probe_id_list; pid && pid->names[0]; pid++) {
		/* FRID */
		if ((status = probe_frid(pid->names)))
			goto exit;

		/* SMBIOS */
		if ((status = probe_smbios(intf, pid->names)))
			goto exit;
	}
	return 0;

exit:
	probed = 1;

	/* Update canonical platform name */
	intf->name = pid->names[0];
	if (pid->sku_table) {
		intf->sku_info = sku_find_info(intf, pid->sku_table);
	} else {
		intf->sku_info = &pid->single_sku;
	}
	return status;
#endif /* CONFIG_CROS_CONFIG */
}

int coral_probe(struct platform_intf *intf)
{
#ifdef CONFIG_CROS_CONFIG
	/* cros_config model.yaml 'platform-name' should match intf.name. */
	return cros_config_probe(intf, NULL);
#else
	return -1;
#endif
}

struct eventlog_cb reef_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb reef_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &reef_eeprom_cb,
	.memory		= &smbios_memory_cb,
	.nvram		= &cros_spi_flash_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &reef_sys_cb,
	.eventlog	= &reef_eventlog_cb,
};

struct platform_cb coral_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &reef_eeprom_cb,
	.memory		= &smbios_memory_cb,
	.nvram		= &cros_spi_flash_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &coral_sys_cb,
	.eventlog	= &reef_eventlog_cb,
};

struct platform_intf platform_reef = {
	.type		= PLATFORM_X86_64,
	.name		= "Reef",
	.sub		= reef_sub,
	.cb		= &reef_cb,
	.probe		= &reef_probe,
};

struct platform_intf platform_coral = {
	.type		= PLATFORM_X86_64,
	.name		= "Coral",
	.sub		= reef_sub,
	.cb		= &coral_cb,
	.probe		= &coral_probe,
};