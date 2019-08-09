/*
 * Copyright 2013, Google Inc.
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
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "poppy.h"

struct platform_cmd *poppy_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_fp,
	&cmd_memory,
	&cmd_nvram,
	&cmd_pd,
	&cmd_platform,
	&cmd_smbios,
	&cmd_eventlog,
	NULL
};

int poppy_probe(struct platform_intf *intf)
{
	static struct sku_info sku_info;
	int ret = 0;

	const char *platform1[] = {
		"Google_Soraka", "Google_Rammus",
		NULL
	};
	if (!cros_config_firmware_name_match(intf, platform1)) {
		/** Soraka,Rammus will always work correctly, no hacks needed */
		ret = cros_config_read_sku_info(intf, platform1, &sku_info);
		if (!ret) {
			intf->sku_info = &sku_info;
			return 1;
		}
		return 0;
	}

	const char *platform2[] = {
		"Google_Nautilus",
		NULL
	};
	if (!cros_config_firmware_name_match(intf, platform2)) {
		/** Nautilus uni-build will work correctly, no hacks needed. */
		ret = cros_config_read_sku_info(intf, platform2, &sku_info);
		if (!ret) {
			intf->sku_info = &sku_info;
			return 1;
		}
		/** If we get here we know we are a Nautilus unibuild w/o
		 * upgraded bios, need to force the read of sku info with
		 * sku_id zero. */
		lprintf(LOG_DEBUG,
			"%s: read_sku_info failed for Nautilus, force sku=0\n",
			__func__);
		ret = cros_config_read_forced_sku_info(intf, platform2, 0,
						       &sku_info);
		if (!ret) {
			intf->sku_info = &sku_info;
			return 1;
		}
	}

	return ret;
}

/* late setup routine; not critical to core functionality */
static int poppy_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;

	if (cros_pd_setup(intf) < 0)
		return -1;

	if (cros_fp_setup(intf) < 0)
		return -1;

	return 0;
}

static int poppy_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb poppy_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb poppy_cb = {
	.ec		= &cros_ec_cb,
	.pd		= &cros_pd_cb,
	.fp		= &cros_fp_cb,
	.eeprom		= &poppy_eeprom_cb,
	.memory		= &poppy_memory_cb,
	.nvram		= &poppy_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &poppy_sys_cb,
	.eventlog	= &poppy_eventlog_cb,
};

struct platform_intf platform_poppy = {
	.type		= PLATFORM_X86_64,
	.name		= "Poppy",
	.sub		= poppy_sub,
	.cb		= &poppy_cb,
	.probe		= &poppy_probe,
	.setup_post	= &poppy_setup_post,
	.destroy	= &poppy_destroy,
};
