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

#include "gru.h"
#include "drivers/google/cros_ec.h"
#include "lib/cros_config.h"
#include "lib/file.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/generic_callbacks.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

enum gru_boards {
	UNKNOWN = -1,
	BOB,
	GRU,
	KEVIN,
	RAINIER,
	SCARLET
};

#ifndef CONFIG_CROS_CONFIG
static enum gru_boards probed_board = UNKNOWN;

static struct gru_probe_id {
	const char *name;
	const char *fdt_compat;
	const struct sku_info single_sku;
} gru_id_list[] = {
	[BOB]		= { "Bob", "google,bob-rev", { .brand = "ASUO" } },
	[GRU]		= { "Gru", "google,gru-rev", { .brand = "LOGA" } },
	[KEVIN]		= { "Kevin", "google,kevin-rev", { .brand = "SMAJ" } },
	[RAINIER]	= { "Rainier", "google,rainier-rev" },
};
#endif

static struct platform_cmd *gru_sub[] = {
	&cmd_ec,
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int gru_probe(struct platform_intf *intf)
{
#ifdef CONFIG_CROS_CONFIG
	static const char* platform_arr[] = {"gru", NULL};
	static struct sku_info sku_info;

	int ret = cros_config_read_default_sku_info_fdt(intf, platform_arr,
						        SCARLET_DEFAULT_SKU_ID,
						        &sku_info);
	/* If there was no error, indicate that we found a match */
	if (!ret) {
		intf->sku_info = &sku_info;
		return 1;
	}

	return ret;
#else
	int i;

	for (i = 0; i < ARRAY_SIZE(gru_id_list); i++) {
		const char **compat = &gru_id_list[i].fdt_compat;

		if (probe_fdt_compatible(compat, 1, 1) == 0) {
			lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT "
				"compatible node.\n", gru_id_list[i].name);
			intf->name = gru_id_list[i].name;
			intf->sku_info = &gru_id_list[i].single_sku;
			probed_board = i;
			break;
		}
	}

	lprintf(LOG_DEBUG, "%s: probed_board: %d\n", __func__, probed_board);
	return probed_board > UNKNOWN ? 1 : 0;
#endif /* CONFIG_CROS_CONFIG */
}

static struct eventlog_cb gru_eventlog_cb = {
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

static struct platform_cb gru_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &gru_eeprom_cb,
	.memory		= &gru_memory_cb,
	.psu 		= &generic_psu_battery_cb,
	.sys 		= &gru_sys_cb,
	.eventlog	= &gru_eventlog_cb,
};

/* TODO(crbug.com/1070692): make static */
extern struct platform_intf platform_gru;

struct platform_intf platform_gru = {
	.type		= PLATFORM_ARMV8,
	.name		= "Gru",
	.sub		= gru_sub,
	.cb		= &gru_cb,
	.probe		= &gru_probe,
};
