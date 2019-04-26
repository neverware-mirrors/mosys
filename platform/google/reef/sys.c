/*
 * Copyright 2016, Google Inc.
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

#include <ctype.h>
#include "mosys/alloc.h"
#include "mosys/platform.h"

#include "lib/acpi.h"
#include "lib/smbios.h"
#include "lib/string.h"

static char *reef_get_name(struct platform_intf *intf)
{
	return mosys_strdup(intf->name);
}

static char *reef_get_model(struct platform_intf *intf)
{
	/* TODO(sjg@chromium.org): Consider obtaining this from ACPI data */
        char *model = strlower(mosys_strdup(intf->name));

	if (strcmp(model, "reef") == 0) {
		char *sku_str = smbios_sysinfo_get_sku(intf);
		char *sku_var = sku_str;
		int sku_id;
		/* The sku_str should be in 'skuN' format. */
		while (sku_var && isascii(*sku_var) && !isdigit(*sku_var))
			sku_var++;
		if (sku_var && !*sku_var)
			sku_var = NULL;

		sku_id = atoi(sku_var ? sku_var : "-1");
		free(sku_str);

		switch (sku_id) {
		case 0:
			free(model);
			model = mosys_strdup("basking");
			break;
		case 8:
			free(model);
			model = mosys_strdup("electro");
			break;
		}
	}
	return model;
}

struct sys_cb reef_sys_cb = {
	.version		= &smbios_sysinfo_get_version,
	.vendor			= &smbios_sysinfo_get_vendor,
	.name			= &reef_get_name,
	.family			= &smbios_sysinfo_get_family,
	.firmware_vendor	= &smbios_bios_get_vendor,
	.model			= &reef_get_model,
};
