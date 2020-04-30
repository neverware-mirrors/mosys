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

#include <stdlib.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/wilco_ec.h"

#include "lib/cros_config.h"
#include "lib/eeprom.h"
#include "lib/memory.h"
#include "lib/smbios.h"
#include "lib/elog.h"
#include "lib/sku.h"

#include "sarien.h"

static struct platform_cmd *sarien_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_eventlog,
	NULL
};

static int sarien_probe(struct platform_intf *intf)
{
	/* support single platform from configs->platform_name */
	return cros_config_probe(intf, NULL);
}

static struct eventlog_cb sarien_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

static struct platform_cb sarien_cb = {
	.ec		= &wilco_ec_cb,
	.eeprom		= &sarien_eeprom_cb,
	.memory		= &smbios_memory_cb,
	.nvram		= &cros_spi_flash_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys		= &sarien_sys_cb,
	.eventlog	= &sarien_eventlog_cb,
};

struct platform_intf platform_sarien = {
	.type		= PLATFORM_X86_64,
	.name		= "Sarien",
	.sub		= sarien_sub,
	.cb		= &sarien_cb,
	.probe		= &sarien_probe,
};
