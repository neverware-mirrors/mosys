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

#ifdef CONFIG_CROS_CONFIG

#include <stdlib.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/cros_ec.h"
#include "drivers/google/wilco_ec.h"

#include "lib/cros_config.h"
#include "lib/smbios.h"
#include "lib/elog.h"
#include "lib/sku.h"

#include "sarien.h"

struct platform_cmd *sarien_sub[] = {
	&cmd_ec,
	&cmd_ish,
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
	"Google_Sarien", "Google_Arcada",
	NULL
};

int sarien_probe(struct platform_intf *intf)
{
	static struct sku_info sku_info;
	int ret;

	ret = cros_config_read_sku_info(intf, platform_names, &sku_info);

	if(!ret) {
		intf->sku_info = &sku_info;
		return 1;
	}

	return ret;
}

/* late setup routine; not critical to core functionality */
static int sarien_setup_post(struct platform_intf *intf)
{
	if (cros_ish_setup(intf) < 0)
		return -1;
	return 0;
}

static int sarien_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb sarien_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb sarien_cb = {
	.ec		= &wilco_ec_cb,
	.ish		= &cros_ish_cb,
	.eeprom		= &sarien_eeprom_cb,
	.memory		= &sarien_memory_cb,
	.nvram		= &sarien_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &sarien_sys_cb,
	.eventlog	= &sarien_eventlog_cb,
};

struct platform_intf platform_sarien = {
	.type		= PLATFORM_X86_64,
	.name		= "Sarien",
	.sub		= sarien_sub,
	.cb		= &sarien_cb,
	.probe		= &sarien_probe,
	.setup_post	= &sarien_setup_post,
	.destroy	= &sarien_destroy,
};
#endif /* CONFIG_CROS_CONFIG */
