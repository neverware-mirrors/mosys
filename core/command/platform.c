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
#include "mosys/platform.h"

#include "lib/probe.h"
#include "lib/sku.h"
#include "mosys/log.h"
#include "mosys/kv_pair.h"

/* Identifiers for platform_generic_identifier_cmd. */
enum {
	PLATFORM_ID_VENDOR,
	PLATFORM_ID_NAME,
	PLATFORM_ID_VERSION,
	PLATFORM_ID_FAMILY,
	PLATFORM_ID_MODEL,
	PLATFORM_ID_CHASSIS,
	PLATFORM_ID_BRAND,
	PLATFORM_ID_CUSTOMIZATION,
	PLATFORM_ID_SIGNATURE,
};

static int print_platforminfo(const char *key, const char *value);
struct platform_cmd platform_cmds[];

static int platform_generic_identifier_cmd(struct platform_intf *intf,
					   struct platform_cmd *cmd,
					   int id_enum)
{
	char *id = NULL;
	char* (*fallback)(struct platform_intf *intf) = NULL;
	char* (*getter)(struct platform_intf *intf) = NULL;

	if (!intf->cb || !intf->cb->sys) {
		errno = ENOSYS;
		return -1;
	}

	switch (id_enum) {
		case PLATFORM_ID_VENDOR:
			getter = intf->cb->sys->vendor;
			break;

		case PLATFORM_ID_NAME:
			getter = intf->cb->sys->name;
			break;

		case PLATFORM_ID_VERSION:
			getter = intf->cb->sys->version;
			break;

		case PLATFORM_ID_FAMILY:
			getter = intf->cb->sys->family;
			break;

		case PLATFORM_ID_MODEL:
			getter = intf->cb->sys->model;
			fallback = sku_get_model;
			break;

		case PLATFORM_ID_CHASSIS:
			getter = intf->cb->sys->chassis;
			fallback = sku_get_chassis;
			break;

		case PLATFORM_ID_BRAND:
			getter = intf->cb->sys->brand;
			fallback = sku_get_brand;
			break;

		case PLATFORM_ID_CUSTOMIZATION:
			getter = intf->cb->sys->customization;
			fallback = sku_get_customization;
			break;

		case PLATFORM_ID_SIGNATURE:
			/*
			 * We don't have a fallback to sku_get_signature_id
			 * here. This means that any platform using unified
			 * builds must have a * signature_id field in its
			 * callbacks in sys.c
			 */
			getter = intf->cb->sys->signature_id;
			break;
	}

	if (getter) {
		id = getter(intf);
	} else if (fallback) {
		id = fallback(intf);
	} else {
		errno = ENOSYS;
	}

	return id ? print_platforminfo(cmd->name, id) : -1;
}

static int platform_vendor_cmd(struct platform_intf *intf,
			       struct platform_cmd *cmd,
			       int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_VENDOR);
}

static int platform_name_cmd(struct platform_intf *intf,
			     struct platform_cmd *cmd,
			     int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_NAME);
}

static int platform_version_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_VERSION);
}

static int platform_family_cmd(struct platform_intf *intf,
			       struct platform_cmd *cmd,
			       int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_FAMILY);
}

static int platform_model_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_MODEL);
}

static int platform_chassis_cmd(struct platform_intf *intf,
				struct platform_cmd *cmd,
				int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_CHASSIS);
}

static int platform_brand_cmd(struct platform_intf *intf,
			      struct platform_cmd *cmd,
			      int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd, PLATFORM_ID_BRAND);
}

static int platform_customization_cmd(struct platform_intf *intf,
				      struct platform_cmd *cmd,
				      int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd,
					       PLATFORM_ID_CUSTOMIZATION);
}

static int platform_signature_cmd(struct platform_intf *intf,
				  struct platform_cmd *cmd,
				  int argc, char **argv)
{
	return platform_generic_identifier_cmd(intf, cmd,
					       PLATFORM_ID_SIGNATURE);
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

static int print_platforminfo(const char *key, const char *value)
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
	free((char*)value);
	return rc;
}

static int platform_id_cmd(struct platform_intf *intf,
                           struct platform_cmd *in_cmd, int argc, char **argv)
{
	struct platform_cmd *cmd;

        /* Run every command we can find, avoiding recursion */
        for (cmd = platform_cmds; cmd->name; cmd++) {
		if (cmd->arg.func && cmd->arg.func != platform_id_cmd &&
		    cmd->type == ARG_TYPE_GETTER)
			cmd->arg.func(intf, cmd, argc, argv);
        }

	return 0;
}

struct platform_cmd platform_cmds[] = {
	{
		.name	= "id",
		.desc	= "Run all platform commands (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_id_cmd }
	},
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
		.name   = "signature",
		.desc   = "Display Signature ID (deprecated)",
		.type   = ARG_TYPE_GETTER,
		.arg    = { .func = platform_signature_cmd }
	},
	{
		.name	= "version",
		.desc	= "Display Platform Version (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_version_cmd }
	},
	{
		.name	= "family",
		.desc	= "Display Platform Family (deprecated)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = platform_family_cmd }
	},
	{NULL}
};

struct platform_cmd cmd_platform = {
	.name	= "platform",
	.desc	= "Platform Information (deprecated)",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = platform_cmds }
};
