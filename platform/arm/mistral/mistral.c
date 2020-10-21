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

#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/intf_list.h"
#include "mosys/log.h"

#include "lib/file.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "lib/sku.h"

#include "mistral.h"

enum MISTRAL_boards {
	MISTRAL,
};

static const char *id_list[] = {
	[MISTRAL] = "google,mistral",
	NULL,
};

static struct platform_cmd *mistral_sub[] = {
	&cmd_memory,
	&cmd_platform,
	&cmd_eventlog,
	NULL
};

static int mistral_probe(struct platform_intf *intf)
{
	int index;

	index = probe_fdt_compatible(&id_list[0], ARRAY_SIZE(id_list), 0);
	if (index >= 0) {
		lprintf(LOG_DEBUG, "Found platform \"%s\" via FDT compatible "
				"node.\n", id_list[index]);
	}

	return index >= 0 ? 1 : 0;
}

static struct eventlog_cb mistral_eventlog_cb = {
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

static struct platform_cb mistral_cb = {
	.memory		= &mistral_memory_cb,
	.eventlog	= &mistral_eventlog_cb,
};

static struct sku_info mistral_sku_info = {
	.model		= "mistral",
	.brand		= "BAMH",
};

static struct platform_intf platform_mistral = {
	.type		= PLATFORM_ARMV8,
	.sub		= mistral_sub,
	.cb		= &mistral_cb,
	.probe		= &mistral_probe,
	.sku_info	= &mistral_sku_info,
};
REGISTER_PLATFORM(platform_mistral, "Mistral");
