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

#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "drivers/google/cros_ec.h"

#include "lib/file.h"
#include "lib/generic_callbacks.h"
#include "lib/math.h"
#include "lib/probe.h"

#include "nyan.h"

static const char *nyan_kitty_id_list[] = {
	"google,nyan-kitty-rev5",
	"google,nyan-kitty-rev4",
	"google,nyan-kitty-rev3",
	"google,nyan-kitty-rev2",
	"google,nyan-kitty-rev1",
	"google,nyan-kitty-rev0",
	NULL,
};

static struct platform_cmd *nyan_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_nvram,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int nyan_probe(struct platform_intf *intf)
{
	int index;

	index = probe_fdt_compatible(&nyan_kitty_id_list[0],
				     ARRAY_SIZE(nyan_kitty_id_list), 0);
	if (index >= 0) {
		lprintf(LOG_DEBUG,
			"Found platform \"%s\" via FDT compatible node.\n",
			nyan_kitty_id_list[index]);
		return 1;
	}

	return 0;
}

static struct eventlog_cb nyan_eventlog_cb = {
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

static struct platform_cb nyan_cb = {
	.ec 		= &cros_ec_cb,
	.eeprom 	= &nyan_eeprom_cb,
	.memory		= &nyan_memory_cb,
	.nvram		= &cros_ec_nvram_cb,
	.psu		= &generic_psu_ac_only_cb,
	.sys 		= &nyan_sys_cb,
	.eventlog	= &nyan_eventlog_cb,
};

/* TODO(crbug.com/1070692): make static */
extern struct platform_intf platform_nyan;

struct platform_intf platform_nyan = {
	.type		= PLATFORM_ARMV7,
	.name		= "Kitty",
	.sub		= nyan_sub,
	.cb		= &nyan_cb,
	.probe		= &nyan_probe,
};
