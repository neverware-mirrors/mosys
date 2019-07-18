/*
 * Copyright 2017, Google Inc.
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
#include "lib/generic_callbacks.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "fizz.h"

struct platform_cmd *fizz_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_fp,
	&cmd_memory,
	&cmd_nvram,
	&cmd_pd,
	&cmd_platform,
	&cmd_psu,
	&cmd_smbios,
	&cmd_eventlog,
	NULL
};

static const char *platform_names[] = {
	"Fizz", "Karma",
	NULL
};

int fizz_probe(struct platform_intf *intf)
{
	static struct sku_info sku_info;
	int ret;

	ret = cros_config_read_sku_info(intf, platform_names, &sku_info);

	/* If there was no error, indicate that we found a match */
	if (!ret) {
		intf->sku_info = &sku_info;
		return 1;
	}

	return ret;
}

/* late setup routine; not critical to core functionality */
static int fizz_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	return 0;
}

static int fizz_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb fizz_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb fizz_cb = {
	.ec		= &cros_ec_cb,
	.fp		= &cros_fp_cb,
	.eeprom		= &fizz_eeprom_cb,
	.memory		= &fizz_memory_cb,
	.nvram		= &fizz_nvram_cb,
	.psu		= &generic_psu_ac_only_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &fizz_sys_cb,
	.eventlog	= &fizz_eventlog_cb,
};

struct platform_intf platform_fizz = {
	.type		= PLATFORM_X86_64,
	.name		= "Fizz",
	.sub		= fizz_sub,
	.cb		= &fizz_cb,
	.probe		= &fizz_probe,
	.setup_post	= &fizz_setup_post,
	.destroy	= &fizz_destroy,
};
#endif /* CONFIG_CROS_CONFIG */
