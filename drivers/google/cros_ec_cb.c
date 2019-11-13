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

#include "lib/string.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"
#include "drivers/google/cros_ec_commands.h"

static ssize_t cros_ec_name(struct platform_intf *intf, struct ec_cb *ec,
			    char *buf, size_t buf_sz)
{
	struct ec_response_get_chip_info chip_info;

	if (cros_ec_chip_info(intf, ec, &chip_info))
		return -1;

	return strlcpy(buf, chip_info.name, buf_sz);
}

static ssize_t cros_ec_vendor(struct platform_intf *intf, struct ec_cb *ec,
			      char *buf, size_t buf_sz)
{
	struct ec_response_get_chip_info chip_info;

	if (cros_ec_chip_info(intf, ec, &chip_info))
		return -1;

	return strlcpy(buf, chip_info.vendor, buf_sz);
}

struct ec_cb cros_ec_cb = {
	.vendor		= cros_ec_vendor,
	.name		= cros_ec_name,
	.fw_version	= cros_ec_version,
	.pd_chip_info	= cros_ec_pd_chip_info,
};

struct ec_cb cros_pd_cb = {
	.vendor		= cros_ec_vendor,
	.name		= cros_ec_name,
	.fw_version	= cros_ec_version,
};

struct ec_cb cros_fp_cb = {
	.vendor		= cros_ec_vendor,
	.name		= cros_ec_name,
	.fw_version	= cros_ec_version,
};
