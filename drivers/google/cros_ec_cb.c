/*
 * Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
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
 *
 * cros_ec_cb.c: EC accessor functions / callbacks for use in platform_intf
 */

#include <sys/types.h>
#include <unistd.h>

#include "lib/string.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"
#include "drivers/google/cros_ec_commands.h"

static int cros_ec_setup(struct ec_cb *ec)
{
	struct cros_ec_priv *priv;

	if (!ec || !(priv = ec->priv) || !priv->devfs) {
		lprintf(LOG_ERR, "%s: EC interface is not valid.\n",
			__func__);
		return -1;
	}

	if (priv->devfs->fd < 0) {
		/* Device has not been set up yet, do the setup now. */
		return cros_ec_probe_dev(ec);
	}

	/* Device is already setup, do nothing. */
	return 0;
}

static int cros_ec_destroy(struct ec_cb *ec)
{
	int rv = 0;
	struct cros_ec_priv *priv;

	if (!ec || !(priv = ec->priv) || !priv->devfs) {
		lprintf(LOG_ERR, "%s: EC interface is not valid.\n",
			__func__);
		return -1;
	}

	if (priv->devfs->fd >= 0) {
		rv = close(priv->devfs->fd);
		priv->devfs->fd = -1;
	}

	return rv;
}

static ssize_t cros_ec_name(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	struct ec_response_get_chip_info chip_info;

	if (cros_ec_chip_info(ec, &chip_info))
		return -1;

	return strlcpy(buf, chip_info.name, buf_sz);
}

static ssize_t cros_ec_vendor(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	struct ec_response_get_chip_info chip_info;

	if (cros_ec_chip_info(ec, &chip_info))
		return -1;

	return strlcpy(buf, chip_info.vendor, buf_sz);
}

static struct cros_ec_dev cros_ec_devfs = {
	.name = CROS_EC_DEV_NAME,
	.fd = -1,
};

static struct cros_ec_priv cros_ec_priv = {
	.devfs = &cros_ec_devfs,
};

struct ec_cb cros_ec_cb = {
	.setup = cros_ec_setup,
	.destroy = cros_ec_destroy,
	.vendor = cros_ec_vendor,
	.name = cros_ec_name,
	.fw_version = cros_ec_version,
	.pd_chip_info = cros_ec_pd_chip_info,
	.priv = &cros_ec_priv,
};

static struct cros_ec_dev cros_pd_devfs = {
	.name = CROS_PD_DEV_NAME,
	.fd = -1,
};

static struct cros_ec_priv cros_pd_priv = {
	.devfs = &cros_pd_devfs,
};

struct ec_cb cros_pd_cb = {
	.setup = cros_ec_setup,
	.destroy = cros_ec_destroy,
	.vendor = cros_ec_vendor,
	.name = cros_ec_name,
	.fw_version = cros_ec_version,
	.priv = &cros_pd_priv,
};

static struct cros_ec_dev cros_fp_devfs = {
	.name = CROS_FP_DEV_NAME,
	.fd = -1,
};

static struct cros_ec_priv cros_fp_priv = {
	.devfs = &cros_fp_devfs,
};

struct ec_cb cros_fp_cb = {
	.setup = cros_ec_setup,
	.destroy = cros_ec_destroy,
	.vendor = cros_ec_vendor,
	.name = cros_ec_name,
	.fw_version = cros_ec_version,
	.priv = &cros_fp_priv,
};
