/*
 * Copyright 2012, Google Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "mosys/alloc.h"
#include "mosys/command_list.h"
#include "mosys/platform.h"

#include "lib/probe.h"
#include "lib/sku.h"
#include "mosys/log.h"
#include "mosys/kv_pair.h"

static int print_platforminfo(const char *key, char *value)
{
	struct kv_pair *kv = kv_pair_new();
	int rc;

	if (!value) {
		lprintf(LOG_ERR, "Unable to determine platform %s\n", key);
		return -1;
	}

	kv_pair_add(kv, key, value);
	rc = kv_pair_print(kv);
	kv_pair_free(kv);
	free(value);
	return rc;
}

static int platform_vendor_cmd(struct platform_intf *intf,
			       struct platform_cmd *cmd,
			       int argc, char **argv)
{
	if (intf->cb && intf->cb->sys && intf->cb->sys->vendor) {
		return print_platforminfo("vendor",
					  intf->cb->sys->vendor(intf));
	}

	errno = ENOSYS;
	return -1;
}

static int platform_name_cmd(struct platform_intf *intf,
			     struct platform_cmd *cmd,
			     int argc, char **argv)
{
	int rc;
	struct kv_pair *kv = kv_pair_new();

	kv_pair_add(kv, "name", intf->name);
	rc = kv_pair_print(kv);
	kv_pair_free(kv);

	return rc;
}

static int platform_version_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	if (intf->cb && intf->cb->sys && intf->cb->sys->version) {
		return print_platforminfo("version",
					  intf->cb->sys->version(intf));
	}

	errno = ENOSYS;
	return -1;
}

static int platform_model_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	return print_platforminfo("model", sku_get_model(intf));
}

static int platform_chassis_cmd(struct platform_intf *intf,
				struct platform_cmd *cmd,
				int argc, char **argv)
{
	return print_platforminfo("chassis", sku_get_chassis(intf));
}

static int platform_brand_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	return print_platforminfo("brand", sku_get_brand(intf));
}

static int platform_customization_cmd(struct platform_intf *intf,
				      struct platform_cmd *cmd,
				      int argc, char **argv)
{
	return print_platforminfo("customization", sku_get_customization(intf));
}

static int platform_sku_cmd(struct platform_intf *intf,
			    struct platform_cmd *cmd,
			    int argc, char **argv)
{
	char buffer[16];
	int sku_number = sku_get_number(intf);

	if (sku_number < 0) {
		errno = ENOSYS;
		return -1;
	}

	snprintf(buffer, sizeof(buffer), "%d", sku_number);
	return print_platforminfo(cmd->name, mosys_strdup(buffer));
}

static struct platform_cmd platform_cmds[] = {
	{
		.name	= "vendor",
		.desc	= "Display Platform Vendor (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_vendor_cmd }
	},
	{
		.name	= "name",
		.desc	= "Display Platform Product Name (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_name_cmd }
	},
	{
		.name	= "model",
		.desc	= "Display Model (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_model_cmd }
	},
	{
		.name	= "chassis",
		.desc	= "Display Chassis ID (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_chassis_cmd }
	},
	{
		.name	= "sku",
		.desc	= "Display SKU Number (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_sku_cmd }
	},
	{
		.name	= "brand",
		.desc	= "Display Brand Code (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_brand_cmd }
	},
	{
		.name	= "customization",
		.desc	= "Display Customization ID (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_customization_cmd }
	},
	{
		.name	= "version",
		.desc	= "Display Platform Version (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_version_cmd }
	},
	{NULL}
};

struct platform_cmd cmd_platform = {
	.name	= "platform",
	.desc	= "Platform Information (deprecated)",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = platform_cmds }
};
