/*
 * Copyright 2019, Google Inc.
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

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/cros_ec.h"

#include "lib/cros_config.h"
#include "lib/smbios.h"
#include "lib/elog.h"
#include "lib/sku.h"

#include "hatch.h"

struct platform_cmd *hatch_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_pd,
	&cmd_platform,
	&cmd_smbios,
	&cmd_eventlog,
	NULL
};

static const char *platform_names[] = {
	"Google_Hatch", "Google_Kohaku", "Google_Helios", "Google_Kindred",
	NULL
};

int hatch_probe(struct platform_intf *intf)
{
	/* Perform a shallow probe based solely on smbios name. */
	if (!cros_config_firmware_name_match(intf, platform_names))
		return 1;
	return 0;
}

/* late setup routine; not critical to core functionality */
static int hatch_setup_post(struct platform_intf *intf)
{
	static struct sku_info sku_info;

	/* If there was no error fill in the sku info. */
	if (!cros_config_read_sku_info(intf, platform_names, &sku_info))
		intf->sku_info = &sku_info;

	if (cros_ec_setup(intf) < 0)
		return -1;

	return 0;
}

static int hatch_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb hatch_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb hatch_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &hatch_eeprom_cb,
	.memory		= &hatch_memory_cb,
	.nvram		= &hatch_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &hatch_sys_cb,
	.eventlog	= &hatch_eventlog_cb,
};

struct platform_intf platform_hatch = {
	.type		= PLATFORM_X86_64,
	.name		= "Hatch",
	.sub		= hatch_sub,
	.cb		= &hatch_cb,
	.probe		= &hatch_probe,
	.setup_post	= &hatch_setup_post,
	.destroy	= &hatch_destroy,
};
