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
 * gec.c: Generic CrOS EC functions and structures.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mosys/alloc.h"
#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"
#include "drivers/google/cros_ec_commands.h"
#include "drivers/google/cros_ec_dev.h"

#include "lib/math.h"
#include "lib/string.h"

#define BOARD_VERSION_LEN	8	/* a 16-bit value or "Unknown" */
#define ANX74XX_VENDOR_ID	0xAAAA
#define PS8751_VENDOR_ID	0x1DA0

ssize_t cros_ec_version(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	static const char *const fw_copies[] = { "unknown", "RO", "RW" };
	struct ec_response_get_version r;
	struct cros_ec_priv *priv;

	MOSYS_CHECK(ec && ec->priv);
	priv = ec->priv;

	MOSYS_CHECK(priv->cmd);
	if (priv->cmd(ec, EC_CMD_GET_VERSION, 0, &r, sizeof(r), NULL, 0))
		return -1;

	/* Ensure versions are null-terminated before we print them */
	r.version_string_ro[sizeof(r.version_string_ro) - 1] = '\0';
	r.version_string_rw[sizeof(r.version_string_rw) - 1] = '\0';

	/* Print versions */
	lprintf(LOG_DEBUG, "RO version:    %s\n", r.version_string_ro);
	lprintf(LOG_DEBUG, "RW version:    %s\n", r.version_string_rw);
	lprintf(LOG_DEBUG, "Firmware copy: %s\n",
		(r.current_image < ARRAY_SIZE(fw_copies) ?
		 fw_copies[r.current_image] : "?"));

	switch (r.current_image) {
	case EC_IMAGE_RO:
		return strlcpy(buf, r.version_string_ro, buf_sz);
	case EC_IMAGE_RW:
		return strlcpy(buf, r.version_string_rw, buf_sz);
	}

	lprintf(LOG_DEBUG, "%s: cannot determine version\n", __func__);
	return -1;
}

int cros_ec_board_version(struct ec_cb *ec)
{
	struct cros_ec_priv *priv;
	struct ec_response_board_version r;
	int rc = 0;

	if (ec->setup && ec->setup(ec) < 0) {
		lprintf(LOG_ERR, "%s: EC setup failed!\n", __func__);
		return -1;
	}

	MOSYS_CHECK(ec && ec->priv);
	priv = ec->priv;

	MOSYS_CHECK(priv->cmd);
	rc = priv->cmd(ec, EC_CMD_GET_BOARD_VERSION, 0, &r, sizeof(r), NULL, 0);

	if (ec->destroy && ec->destroy(ec) < 0) {
		lprintf(LOG_ERR, "%s: EC destroy failed!\n", __func__);
		return -1;
	}

	if (rc)
		return -1;

	lprintf(LOG_DEBUG, "CrOS EC Board Version: %d\n", r.board_version);

	return r.board_version;
}

char *cros_ec_board_version_str(struct platform_intf *intf)
{
	int version;
	char *str = mosys_zalloc(BOARD_VERSION_LEN);

	/* Baseboard may have variants without EC in which case this
	   function needs to be used more selectively. */
	MOSYS_CHECK(intf && intf->cb->ec);

	version = cros_ec_board_version(intf->cb->ec);
	if (version < 0) {
		snprintf(str, BOARD_VERSION_LEN, "Unknown");
	} else {
		/*
		 * Prepend "rev" to match format currently used for CrOS
		 * platforms with devicetree and newer (beginning with Reef)
		 * SMBIOS system (type-1) version format.
		 */
		snprintf(str, BOARD_VERSION_LEN, "rev%d", version);
	}

	return str;
}

int cros_ec_chip_info(struct ec_cb *ec, struct ec_response_get_chip_info *info)
{
	int rc = 0;
	struct cros_ec_priv *priv;

	MOSYS_CHECK(ec && ec->priv);
	priv = ec->priv;

	MOSYS_CHECK(priv->cmd);
	rc = priv->cmd(ec, EC_CMD_GET_CHIP_INFO, 0, info, sizeof(*info), NULL,
		       0);
	if (rc)
		return rc;

	lprintf(LOG_DEBUG, "CrOS EC vendor: %s\n", info->vendor);
	lprintf(LOG_DEBUG, "CrOS EC name: %s\n", info->name);
	lprintf(LOG_DEBUG, "CrOS EC revision: %s\n", info->revision);

	return rc;
}

int cros_ec_pd_chip_info(struct ec_cb *ec, int port)
{
	struct cros_ec_priv *priv;
	struct ec_params_pd_chip_info p;
	struct ec_response_pd_chip_info info;
	struct kv_pair *kv;
	int rc;

	MOSYS_CHECK(ec && ec->priv);
	priv = ec->priv;

	p.port = port;
	p.renew = 0;

	MOSYS_CHECK(priv->cmd);
	rc = priv->cmd(ec, EC_CMD_PD_CHIP_INFO, 0,
		       &info, sizeof(info), &p, sizeof(p));
	if (rc)
		return rc;

	kv = kv_pair_new();
	kv_pair_fmt(kv, "vendor_id", "0x%x", info.vendor_id);
	kv_pair_fmt(kv, "product_id", "0x%x", info.product_id);
	kv_pair_fmt(kv, "device_id", "0x%x", info.device_id);
	switch (info.vendor_id) {
	case ANX74XX_VENDOR_ID:
	case PS8751_VENDOR_ID:
		kv_pair_fmt(kv, "fw_version", "0x%" PRIx64,
			    info.fw_version_number);
		break;
	default:
		kv_pair_fmt(kv, "fw_version", "UNSUPPORTED");
	}
	kv_pair_print(kv);
	kv_pair_free(kv);

	return rc;
}
