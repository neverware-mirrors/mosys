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

#include "lib/probe.h"
#include "lib/smbios.h"
#include "lib/elog.h"

#include "glados.h"

struct probe_ids {
	const char *names[2];
	const char *hwids[2];
	const char *frids[2];
};

static const struct probe_ids probe_id_list[] = {
	{ { "Asuka", NULL },
	  { "ASUKA", NULL },
	  { "Asuka", NULL },
	},
	{ { "Glados", NULL },
	  { "GLADOS", NULL },
	  { "Glados", NULL },
	},
        { { "Skylake", NULL },
          { "SKYLAKE", NULL },
          { "Skylake", NULL },
        },
        { { "Kunimitsu", NULL },
          { "KUNIMITSU", NULL },
          { "Kunimitsu", NULL },
        },
	{ { "Chell", NULL },
	  { "CHELL", NULL },
	  { "Chell", NULL },
	},
	{ { "Lars", NULL },
	  { "LARS", NULL },
	  { "Lars", NULL },
	},
	{ { "Sentry", NULL },
	  { "SENTRY", NULL },
	  { "Sentry", NULL },
	},
	{ { NULL } }
};

struct platform_cmd *glados_sub[] = {
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

int glados_probe(struct platform_intf *intf)
{
	static int status, probed;
	const struct probe_ids *pid;

	if (probed)
		return status;

	for (pid = probe_id_list; pid && pid->names[0]; pid++) {
		/* HWID */
		if (probe_hwid((const char **)pid->hwids)) {
			status = 1;
			goto glados_probe_exit;
		}

		/* FRID */
		if (probe_frid((const char **)pid->frids)) {
			status = 1;
			goto glados_probe_exit;
		}

		/* SMBIOS */
		if (probe_smbios(intf, (const char **)pid->names)) {
			status = 1;
			goto glados_probe_exit;
		}
	}
	return 0;

glados_probe_exit:
	probed = 1;
	/* Update canonical platform name */
	intf->name = pid->names[0];
	return status;
}

/* late setup routine; not critical to core functionality */
static int glados_setup_post(struct platform_intf *intf)
{
	if (glados_ec_setup(intf) <= 0)
		return -1;

	if (glados_pd_setup(intf) <= 0)
		return -1;

	return 0;
}

static int glados_destroy(struct platform_intf *intf)
{
	return 0;
}

struct eventlog_cb glados_eventlog_cb = {
	.print_type	= &elog_print_type,
	.print_data	= &elog_print_data,
	.print_multi	= &elog_print_multi,
	.verify		= &elog_verify,
	.verify_header	= &elog_verify_header,
	.fetch		= &elog_fetch_from_smbios,
};

struct platform_cb glados_cb = {
	.ec		= &cros_ec_cb,
	.pd		= &cros_pd_cb,
	.eeprom		= &glados_eeprom_cb,
	.memory		= &glados_memory_cb,
	.nvram		= &glados_nvram_cb,
	.smbios		= &smbios_sysinfo_cb,
	.sys 		= &glados_sys_cb,
	.eventlog	= &glados_eventlog_cb,
};

struct platform_intf platform_glados = {
	.type		= PLATFORM_X86_64,
	.name		= "Glados",
	.sub		= &glados_sub,
	.cb		= &glados_cb,
	.probe		= &glados_probe,
	.setup_post	= &glados_setup_post,
	.destroy	= &glados_destroy,
};
