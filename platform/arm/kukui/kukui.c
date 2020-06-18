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

#include "kukui.h"

static struct platform_cmd *kukui_sub[] = {
	&cmd_ec,
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int kukui_probe(struct platform_intf *intf)
{
	/**
	 * 'kukui' interface is shared by multiple overlays (with different
	 * platform-name value in cros_config model.yaml 'platform-name')
	 * so we have to pass explicit platform (overlay) names.
	 */
	static const char* platform_names[] = {
		"Flapjack",
		"Jacuzzi",
		"Kukui",
		NULL,
	};
	return cros_config_probe(intf, platform_names);
}

static struct eventlog_cb kukui_eventlog_cb = {
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

static struct platform_cb kukui_cb = {
	.ec		= &cros_ec_cb,
	.eeprom 	= &kukui_eeprom_cb,
	.memory		= &kukui_memory_cb,
	.psu		= &generic_psu_battery_cb,
	.sys		= &kukui_sys_cb,
	.eventlog	= &kukui_eventlog_cb,
};

/* TODO(crbug.com/1070692): make static */
extern struct platform_intf platform_kukui;

struct platform_intf platform_kukui = {
	.type		= PLATFORM_ARMV8,
	.name		= "Kukui",
	.sub		= kukui_sub,
	.cb		= &kukui_cb,
	.probe		= &kukui_probe,
};
