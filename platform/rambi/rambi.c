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

#include "drivers/google/cros_ec.h"

#include "lib/probe.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "rambi.h"

struct probe_ids {
	const char *names[2];
	const char *frids[2];
};

static const struct probe_ids probe_id_list[] = {
	{ { "Banjo", NULL },
	  { "Google_Banjo", NULL },
	},
	{ { "Candy", NULL },
	  { "Google_Candy", NULL },
	},
	{ { "Clapper", NULL },
	  { "Google_Clapper", NULL },
	},
	{ { "Cranky", NULL },
	  { "Google_Cranky", NULL },
	},
	{ { "Enguarde", NULL },
	  { "Google_Enguarde", NULL },
	},
	{ { "Expresso", NULL },
	  { "Google_Expresso", NULL },
	},
	{ { "Glimmer", NULL },
	  { "Google_Glimmer", NULL },
	},
	{ { "Gnawty", NULL },
	  { "Google_Gnawty", NULL },
	},
	{ { "Heli", NULL },
	  { "Google_Heli", NULL },
	},
	{ { "Kip", NULL },
	  { "Google_Kip", NULL },
	},
	{ { "Ninja", NULL },
	  { "Google_Ninja", NULL },
	},
	{ { "Orco", NULL },
	  { "Google_Orco", NULL },
	},
	{ { "Quawks", NULL },
	  { "Google_Quawks", NULL },
	},
	{ { "Rambi", NULL },
	  { "Google_Rambi", NULL },
	},
	{ { "Squawks", NULL },
	  { "Google_Squawks", NULL },
	},
	{ { "Sumo", NULL },
	  { "Google_Sumo", NULL },
	},
	{ { "Swanky", NULL },
	  { "Google_Swanky", NULL },
	},
	{ { "Winky", NULL },
	  { "Google_Winky", NULL },
	},
	{ { NULL } }
};

struct platform_cmd *rambi_sub[] = {
	&cmd_ec,
	&cmd_eeprom,
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int rambi_probe(struct platform_intf *intf)
{
	static int status = 0, probed = 0;
	const struct probe_ids *pid;

	if (probed)
		return status;

	for (pid = probe_id_list; pid && pid->names[0]; pid++) {
		/* FRID */
		if ((status = probe_frid(pid->frids)))
			goto exit;

		/* SMBIOS */
		if ((status = probe_smbios(intf, pid->names)))
			goto exit;
	}
	return 0;

exit:
	probed = 1;
	/* Update canonical platform name */
	intf->name = pid->names[0];
	return status;
}

/* late setup routine; not critical to core functionality */
static int rambi_setup_post(struct platform_intf *intf)
{
	if (cros_ec_setup(intf) < 0)
		return -1;
	return 0;
}

struct eventlog_cb rambi_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb rambi_cb = {
	.ec		= &cros_ec_cb,
	.eeprom		= &rambi_eeprom_cb,
	.memory		= &rambi_memory_cb,
	.smbios		= &smbios_sysinfo_cb,
	.psu		= &rambi_psu_cb,
	.sys 		= &rambi_sys_cb,
	.eventlog	= &rambi_eventlog_cb,
};

struct platform_intf platform_rambi = {
	.type		= PLATFORM_X86_64,
	.name		= "Rambi",
	.sub		= rambi_sub,
	.cb		= &rambi_cb,
	.probe		= &rambi_probe,
	.setup_post	= &rambi_setup_post,
};
