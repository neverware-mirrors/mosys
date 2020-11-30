/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stddef.h>

#include "lib/generic_callbacks.h"
#include "lib/math.h"
#include "lib/memory.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"

static struct platform_cmd *jecht_sub[] = {
	&cmd_eventlog,
	&cmd_memory,
	&cmd_platform,
	&cmd_psu,
	NULL,
};

static int jecht_probe(struct platform_intf *intf)
{
	static struct {
		const char *smbios_name;
		struct sku_info info;
	} models[] = {
		{ "Buddy", { .model = "buddy", .brand = "ACAU" } },
		{ "Guado", { .model = "guado", .brand = "ASUG" } },
		{ "Rikku", { .model = "rikku", .brand = "ACAQ" } },
		{ "Tidus", { .model = "tidus", .brand = "LEAF" } },
	};
	int rv;

	for (size_t i = 0; i < ARRAY_SIZE(models); i++) {
		const char *probe_list[] = { models[i].smbios_name, NULL };

		rv = probe_smbios(intf, probe_list);
		if (rv < 0) {
			return -1;
		} else if (rv > 0) {
			intf->sku_info = &models[i].info;
			return 1;
		}
	}

	return 0;
}

static struct eventlog_cb jecht_eventlog_cb = {
	.print_type = &elog_print_type,
	.print_data = &elog_print_data,
	.print_multi = &elog_print_multi,
	.verify = &elog_verify,
	.verify_header = &elog_verify_header,
	.fetch = &elog_fetch_from_smbios,
};

static struct platform_cb jecht_cb = {
	.memory = &smbios_memory_cb,
	.psu = &generic_psu_ac_only_cb,
	.eventlog = &jecht_eventlog_cb,
};

static struct platform_intf platform_jecht = {
	.type = PLATFORM_X86_64,
	.sub = jecht_sub,
	.cb = &jecht_cb,
	.probe = jecht_probe,
};
REGISTER_PLATFORM(platform_jecht, "Jecht");
