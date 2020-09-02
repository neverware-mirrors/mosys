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

#include "pinky.h"
#include "lib/fdt.h"
#include "lib/file.h"
#include "lib/generic_callbacks.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

static struct veyron_probe_id {
	const char *name;
	const char *fdt_compat;
} veyron_id_list[] = {
	{ "Fievel", "google,veyron-fievel" },
	{ "Mickey", "google,veyron-mickey" },
	{ "Tiger", "google,veyron-tiger" },
};

static struct platform_cmd *pinky_sub[] = {
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int pinky_probe(struct platform_intf *intf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(veyron_id_list); i++) {
		const char **compat = &veyron_id_list[i].fdt_compat;

		if (probe_fdt_compatible(compat, 1, 1) == 0) {
			lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT "
				"compatible node.\n", veyron_id_list[i].name);
			intf->name = veyron_id_list[i].name;
			return 1;
		}
	}

	return 0;
}

static struct eventlog_cb pinky_eventlog_cb = {
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

static struct platform_cb pinky_cb = {
	.eeprom 	= &pinky_eeprom_cb,
	.memory		= &pinky_memory_cb,
	.psu		= &generic_psu_ac_only_cb,
	.sys		= &fdt_sysinfo_cb,
	.eventlog	= &pinky_eventlog_cb,
};

static struct platform_intf platform_pinky = {
	.type		= PLATFORM_ARMV7,
	.sub		= pinky_sub,
	.cb		= &pinky_cb,
	.probe		= &pinky_probe,
};
REGISTER_PLATFORM(platform_pinky, "Pinky");
