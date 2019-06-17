/*
 * Copyright 2018, Google Inc.
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
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "octopus.h"

struct platform_cmd *octopus_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_smbios,
	&cmd_eventlog,
	NULL
};

static const char *platform_names = "Ampton,Bip,Bloog,Bobba,Casta,Fleex,Garg,"
				    "Meep,Phaser,Yorp";

int octopus_probe(struct platform_intf *intf)
{
	/* Perform a shallow probe based solely on smbios name. */
	if (!cros_config_smbios_platform_name_match(intf, platform_names))
		return 1;

	return 0;
}

/* late setup routine; not critical to core functionality */
static int octopus_setup_post(struct platform_intf *intf)
{
	static struct sku_info sku_info;

	/* If there was no error fill in the sku info. */
	if (!cros_config_read_sku_info(intf, platform_names, &sku_info))
		intf->sku_info = &sku_info;

	if (cros_ec_setup(intf) < 0)
		return -1;

	return 0;
}

static int octopus_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb octopus_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb octopus_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &octopus_eeprom_cb,
	.memory		= &octopus_memory_cb,
	.nvram		= &octopus_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &octopus_sys_cb,
	.eventlog	= &octopus_eventlog_cb,
};

struct platform_intf platform_octopus = {
	.type		= PLATFORM_X86_64,
	.name		= "Octopus",
	.sub		= octopus_sub,
	.cb		= &octopus_cb,
	.probe		= &octopus_probe,
	.setup_post	= &octopus_setup_post,
	.destroy	= &octopus_destroy,
};
