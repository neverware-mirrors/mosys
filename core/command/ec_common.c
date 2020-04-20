/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <errno.h>
#include <stdlib.h>

#include "lib/math.h"
#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/platform.h"

static int ec_info_common(struct ec_cb *ec)
{
	char vendor[32], name[32], fw_version[32];
	struct kv_pair *kv;
	int rc;

	if (!ec) {
		errno = ENOSYS;
		return -1;
	}

	if (ec->setup && ec->setup(ec) < 0) {
		lprintf(LOG_ERR, "%s: EC setup failed!\n", __func__);
		return -1;
	}

	kv = kv_pair_new();

	if (ec->vendor && ec->vendor(ec, vendor, ARRAY_SIZE(vendor)) >= 0)
		kv_pair_add(kv, "vendor", vendor);
	if (ec->name && ec->name(ec, name, ARRAY_SIZE(name)) >= 0)
		kv_pair_add(kv, "name", name);
	if (ec->fw_version &&
	    ec->fw_version(ec, fw_version, ARRAY_SIZE(fw_version)) >= 0)
		kv_pair_add(kv, "fw_version", fw_version);

	rc = kv_pair_print(kv);
	kv_pair_free(kv);

	if (ec->destroy && ec->destroy(ec) < 0)
		lprintf(LOG_ERR, "%s: EC destroy failed!\n", __func__);

	return rc;
}

static int ec_info(struct platform_intf *intf, struct platform_cmd *cmd,
		   int argc, char **argv)
{
	return ec_info_common(intf->cb->ec);
}

static int pd_info(struct platform_intf *intf, struct platform_cmd *cmd,
		   int argc, char **argv)
{
	return ec_info_common(intf->cb->pd);
}

static int fp_info(struct platform_intf *intf, struct platform_cmd *cmd,
		   int argc, char **argv)
{
	return ec_info_common(intf->cb->fp);
}

static int pd_chip_info(struct platform_intf *intf, struct platform_cmd *cmd,
			int argc, char **argv)
{
	int rv;
	unsigned port;
	struct ec_cb *ec;
	char *endptr = NULL;

	if (argc < 1)
		goto error_usage;

	errno = 0;
	port = strtoul(argv[0], &endptr, 0);
	if (errno || (endptr && *endptr))
		goto error_usage;

	if (!intf || !intf->cb || !intf->cb->ec ||
	    !intf->cb->ec->pd_chip_info) {
		errno = ENOSYS;
		return -1;
	}

	ec = intf->cb->ec;
	if (ec->setup && ec->setup(ec) < 0) {
		lprintf(LOG_ERR, "%s: EC setup failed!\n", __func__);
		return -1;
	}

	rv = ec->pd_chip_info(ec, port);

	if (ec->destroy && ec->destroy(ec) < 0)
		lprintf(LOG_ERR, "%s: EC destroy failed!\n", __func__);

	return rv;

error_usage:
	platform_cmd_usage(cmd);
	errno = EINVAL;
	return -1;
}

struct platform_cmd ec_cmds[] = {
	{
		.name	= "info",
		.desc	= "Print basic EC information",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = ec_info }
	},
	{ NULL }
};

struct platform_cmd pd_cmds[] = {
	{
		.name	= "info",
		.desc	= "Print basic PD information",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = pd_info }
	},
	{
		.name	= "chip",
		.desc	= "Print PD chip information",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = pd_chip_info }
	},
	{ NULL }
};

struct platform_cmd fp_cmds[] = {
	{
		.name	= "info",
		.desc	= "Print basic FP MCU information",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = fp_info }
	},
	{ NULL }
};

struct platform_cmd cmd_ec = {
	.name	= "ec",
	.desc	= "EC information",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = ec_cmds }
};

struct platform_cmd cmd_pd = {
	.name	= "pd",
	.desc	= "PD information",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = pd_cmds }
};

struct platform_cmd cmd_fp = {
	.name	= "fp",
	.desc	= "Fingerprint MCU information",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = fp_cmds }
};
