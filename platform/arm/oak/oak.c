/*
 * Copyright 2014, Google Inc.
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

#include <stdlib.h>
#include <unistd.h>

#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/cros_ec.h"

#include "lib/cros_config.h"
#include "lib/file.h"
#include "lib/generic_callbacks.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "lib/sku.h"

#include "oak.h"

#ifndef CONFIG_CROS_CONFIG
static struct oak_probe_id {
	const char *name;
	const char *fdt_compat;
	const struct sku_info single_sku;
} oak_id_list[] = {
	{ "Elm", "google,elm", { .brand = "ACAZ" } },
	{ "Hana", "google,hana", { .brand = "LEAO" } },
	{ "Oak", "google,oak", { .brand = NULL } },
};
#endif

static struct platform_cmd *oak_sub[] = {
	&cmd_ec,
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int oak_probe(struct platform_intf *intf)
{
#ifdef CONFIG_CROS_CONFIG
	return cros_config_probe(intf, NULL);
#else
	int i;

	for (i = 0; i < ARRAY_SIZE(oak_id_list); i++) {
		const char **compat = &oak_id_list[i].fdt_compat;

		if (probe_fdt_compatible(compat, 1, 1) == 0) {
			lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT "
				"compatible node.\n", oak_id_list[i].name);
			intf->name = oak_id_list[i].name;
			intf->sku_info = &oak_id_list[i].single_sku;
			return 1;
		}
	}

	return 0;
#endif
}

static struct eventlog_cb oak_eventlog_cb = {
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

static struct platform_cb oak_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &oak_eeprom_cb,
	.memory		= &oak_memory_cb,
	.pd		= &cros_pd_cb,
	.psu		= &generic_psu_battery_cb,
	.sys		= &oak_sys_cb,
	.eventlog	= &oak_eventlog_cb,
};

static struct platform_intf platform_oak = {
	.type		= PLATFORM_ARMV8,
	.sub		= oak_sub,
	.cb		= &oak_cb,
	.probe		= &oak_probe,
};
REGISTER_PLATFORM(platform_oak, "Oak");
