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
#include "lib/fdt.h"
#include "lib/file.h"
#include "lib/generic_callbacks.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "lib/sku.h"

#include "oak.h"

static int oak_probed_board = -1;
static int kukui_probed_board = -1;

struct probe_id {
	const char *name;
	const char *fdt_compat;
	const struct sku_info single_sku;
	int has_pd;
} oak_probe_id_list[] = {
	{ "Elm", "google,elm", { .brand = "ACAZ" }, 0 },
	{ "Hana", "google,hana", { .brand = "LEAO" }, 0 },
	{ "Oak", "google,oak", { .brand = NULL }, 1 },
	{ "Rowan", "google,rowan", { .brand = NULL }, 0 },
}, kukui_probe_id_list[] = {
	{ "Kukui", "google,kukui", { .brand = "ZZCR" }, 0 },
};

#define OAK_CMD_PD_NUM	0

struct platform_cmd *oak_sub[] = {
	/* Keep this as the first entry. intf->sub will be set to point to
         * the next entry if it turns out that we don't have a PD. */
	[OAK_CMD_PD_NUM] = &cmd_pd,

	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int oak_probe(struct platform_intf *intf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(oak_probe_id_list); i++) {
		const char **compat = &oak_probe_id_list[i].fdt_compat;

		if (probe_fdt_compatible(compat, 1, 1) == 0) {
			lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT "
				"compatible node.\n", oak_probe_id_list[i].name);
			intf->name = oak_probe_id_list[i].name;
			intf->sku_info = &oak_probe_id_list[i].single_sku;
			oak_probed_board = i;
			break;
		}
	}

	lprintf(LOG_DEBUG, "%s: probed_board: %d\n", __func__, oak_probed_board);
	return oak_probed_board > -1 ? 1 : 0;
}

static int kukui_probe(struct platform_intf *intf)
{
#ifdef CONFIG_CROS_CONFIG
	static const char* platform_arr[] = {"kukui"};
	static struct sku_info sku_info;

	int ret = cros_config_read_sku_info_fdt(intf, platform_arr, 1,
						&sku_info);
	/* If there was no error, indicate that we found a match */
	if (!ret) {
		intf->sku_info = &sku_info;
		return 1;
	}

	return ret;
#else
	int i;

	for (i = 0; i < ARRAY_SIZE(kukui_probe_id_list); i++) {
		const char **compat = &kukui_probe_id_list[i].fdt_compat;

		if (probe_fdt_compatible(compat, 1, 1) == 0) {
			lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT "
				"compatible node.\n", oak_probe_id_list[i].name);
			intf->name = kukui_probe_id_list[i].name;
			intf->sku_info = &kukui_probe_id_list[i].single_sku;
			kukui_probed_board = i;
			break;
		}
	}

	lprintf(LOG_DEBUG, "%s: probed_board: %d\n", __func__, kukui_probed_board);
	return kukui_probed_board > -1 ? 1 : 0;
#endif /* CONFIG_CROS_CONFIG */
}

static int oak_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	if (oak_probe_id_list[oak_probed_board].has_pd) {
		if (cros_pd_setup(intf) < 0)
			return -1;
	} else {
		intf->cb->pd = NULL;
		intf->sub = &oak_sub[OAK_CMD_PD_NUM + 1];
	}

	if (fdt_set_nvram_cb(intf) < 0)
		return -1;

	return 0;
}

static int kukui_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	if (kukui_probe_id_list[kukui_probed_board].has_pd) {
		if (cros_pd_setup(intf) < 0)
			return -1;
	} else {
		intf->cb->pd = NULL;
		intf->sub = &oak_sub[OAK_CMD_PD_NUM + 1];
	}

	if (fdt_set_nvram_cb(intf) < 0)
		return -1;

	return 0;
}


static int oak_destroy(struct platform_intf *intf)
{
	if (intf->cb->ec && intf->cb->ec->destroy)
		intf->cb->ec->destroy(intf, intf->cb->ec);
	return 0;
}

struct eventlog_cb oak_eventlog_cb = {
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

struct platform_cb oak_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &oak_eeprom_cb,
	.memory		= &oak_memory_cb,
	.pd		= &cros_pd_cb,
	.psu		= &generic_psu_battery_cb,
	.sys		= &oak_sys_cb,
	.eventlog	= &oak_eventlog_cb,
};

struct platform_cb kukui_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &kukui_eeprom_cb,
	.memory		= &kukui_memory_cb,
	.pd		= &cros_pd_cb,
	.psu		= &generic_psu_battery_cb,
	.sys		= &oak_sys_cb,
	.eventlog	= &oak_eventlog_cb,
};

struct platform_intf platform_oak = {
	.type		= PLATFORM_ARMV8,
	.name		= "Oak",
	.sub		= oak_sub,
	.cb		= &oak_cb,
	.probe		= &oak_probe,
	.setup_post	= &oak_setup_post,
	.destroy	= &oak_destroy,
};

struct platform_intf platform_kukui = {
	.type		= PLATFORM_ARMV8,
	.name		= "Kukui",
	.sub		= oak_sub,
	.cb		= &kukui_cb,
	.probe		= &kukui_probe,
	.setup_post	= &kukui_setup_post,
	.destroy	= &oak_destroy,
};
