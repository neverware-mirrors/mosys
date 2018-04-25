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

#include "mosys/alloc.h"
#include "mosys/platform.h"

#include "lib/probe.h"
#include "lib/fdt.h"

/* Gru uses device-tree compatible typle: google,<family>-<name>-rev<N>,
 * ie "google,gru-gru-rev0"
 * Scarlet uses an additional sku<X> suffix:
 * google,<family>-<name>-rev<N>-sku<X> -- e.g., "google,gru-scarlet-rev5-sku6"
 */

static char *gru_get_version(struct platform_intf *intf)
{
	uint32_t board_id;
	char board_id_str[16];

	if (fdt_get_board_id(&board_id) < 0)
		return NULL;

	snprintf(board_id_str, sizeof(board_id_str), "rev%u", board_id);
	return mosys_strdup(board_id_str);
}

static char *gru_get_sku_id(struct platform_intf *intf)
{
	uint32_t sku_id;

	if (fdt_get_sku_id(&sku_id) < 0)
		return -1;

	return sku_id;
}

static char *gru_get_vendor(struct platform_intf *intf)
{
	/* FIXME: implement this */
	return mosys_strdup("Unknown");
}

static char *gru_get_name(struct platform_intf *intf)
{
	return mosys_strdup(intf->name);
}

struct sys_cb gru_sys_cb = {
	.vendor			= &gru_get_vendor,
	.name			= &gru_get_name,
	.version		= &gru_get_version,
	.sku_number		= &gru_get_sku_id,
};
