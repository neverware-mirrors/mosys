/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "mosys/alloc.h"
#include "mosys/platform.h"

#include "lib/probe.h"
#include "lib/fdt.h"

static char *trogdor_get_version(struct platform_intf *intf)
{
	uint32_t board_id;
	char board_id_str[16];

	if (fdt_get_board_id(&board_id) < 0)
		return NULL;

	snprintf(board_id_str, sizeof(board_id_str), "rev%u", board_id);
	return mosys_strdup(board_id_str);
}

static int trogdor_get_sku_id(struct platform_intf *intf)
{
	return fdt_get_sku_id();
}

static char *trogdor_get_vendor(struct platform_intf *intf)
{
	return mosys_strdup("Google");
}

static char *trogdor_get_name(struct platform_intf *intf)
{
	return mosys_strdup(intf->name);
}

struct sys_cb trogdor_sys_cb = {
	.vendor			= &trogdor_get_vendor,
	.name			= &trogdor_get_name,
	.version		= &trogdor_get_version,
	.sku_number		= &trogdor_get_sku_id,
};
