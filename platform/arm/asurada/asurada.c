/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include <unistd.h>

#include "drivers/google/cros_ec.h"
#include "lib/cros_config.h"
#include "lib/elog.h"
#include "lib/file.h"
#include "lib/generic_callbacks.h"
#include "lib/sku.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"

#include "asurada.h"

static struct platform_cmd *asurada_sub[] = {
	&cmd_ec,
	&cmd_memory,
	&cmd_pd,
	&cmd_platform,
	&cmd_psu,
	&cmd_eventlog,
	NULL
};

static int asurada_probe(struct platform_intf *intf)
{
	return cros_config_probe(intf, NULL);
}

static struct eventlog_cb asurada_eventlog_cb = {
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

static struct platform_cb asurada_cb = {
	.ec		= &cros_ec_cb,
	.memory		= &asurada_memory_cb,
	.psu		= &generic_psu_battery_cb,
	.sys		= &asurada_sys_cb,
	.eventlog	= &asurada_eventlog_cb,
};

static struct platform_intf platform_asurada = {
	.type		= PLATFORM_ARMV8,
	.sub		= asurada_sub,
	.cb		= &asurada_cb,
	.probe		= &asurada_probe,
};
REGISTER_PLATFORM(platform_asurada, "Asurada");
