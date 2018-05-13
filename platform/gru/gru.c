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
#endif

struct gru_probe_id {
	const char *name;
	const char *fdt_compat;
	const struct sku_info single_sku;
} gru_id_list[] = {
	[BOB]		= { "Bob", "google,bob-rev", { .brand = "ASUO" } },
	[GRU]		= { "Gru", "google,gru-rev", { .brand = "LOGA" } },
	[KEVIN]		= { "Kevin", "google,kevin-rev", { .brand = "SMAJ" } },
	[RAINIER]	= { "Rainier", "google,rainier-rev" },
	[SCARLET]	= { "Scarlet", "google,scarlet-rev", { .brand = "DXZT" } },
};

#define GRU_CMD_EC_NUM	0
struct platform_cmd *gru_sub[] = {
	/* Keep this as the first entry. intf->sub will be set to point to
         * the next entry if it turns out that we don't have an EC. */
	[GRU_CMD_EC_NUM] = &cmd_ec,

	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int gru_probe(struct platform_intf *intf)
{
#ifdef CONFIG_CROS_CONFIG
	static struct sku_info sku_info;
	int ret;

	ret = cros_config_read_sku_info(intf, "", &sku_info);

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

static int gru_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	return 0;
}

static int gru_destroy(struct platform_intf *intf)
{
	if (intf->cb->ec && intf->cb->ec->destroy)
		intf->cb->ec->destroy(intf, intf->cb->ec);
	return 0;
}

struct eventlog_cb gru_eventlog_cb = {
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

struct platform_cb gru_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &gru_eeprom_cb,
	.memory		= &gru_memory_cb,
	.nvram	= &cros_spi_flash_nvram_cb,
	.psu 		= &generic_psu_battery_cb,
	.sys 		= &gru_sys_cb,
	.eventlog	= &gru_eventlog_cb,
};

struct platform_intf platform_gru = {
	.type		= PLATFORM_ARMV8,
	.name		= "Gru",
	.sub		= gru_sub,
	.cb		= &gru_cb,
	.probe		= &gru_probe,
	.setup_post	= &gru_setup_post,
	.destroy	= &gru_destroy,
};
