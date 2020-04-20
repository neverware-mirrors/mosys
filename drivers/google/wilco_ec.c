/*
 * Copyright 2019 Google LLC
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

#include <string.h>
#include <errno.h>

#include "lib/file.h"
#include "lib/string.h"
#include "mosys/alloc.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#define WILCO_EC_NAME "wilco"
#define WILCO_EC_VENDOR "google"
#define WILCO_EC_SYSFS_VERSION                                                 \
	"/sys/bus/platform/devices/GOOG000C:00/version"

static ssize_t wilco_ec_name(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	return strlcpy(buf, WILCO_EC_NAME, buf_sz);
}

static ssize_t wilco_ec_vendor(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	return strlcpy(buf, WILCO_EC_VENDOR, buf_sz);
}

static ssize_t wilco_ec_fw_version(struct ec_cb *ec, char *buf, size_t buf_sz)
{
	ssize_t bytes_read;

	bytes_read = read_file(WILCO_EC_SYSFS_VERSION, buf, buf_sz, LOG_ERR);

	if (bytes_read <= 0)
		return -1;

	if (buf[bytes_read - 1] != '\n')
		lprintf(LOG_WARNING,
			"%s: Expected version file to end with newline, but "
			"it didn't!",
			__func__);

	buf[bytes_read - 1] = '\0';
	return bytes_read - 1;
}

struct ec_cb wilco_ec_cb = {
	.vendor		= wilco_ec_vendor,
	.name		= wilco_ec_name,
	.fw_version	= wilco_ec_fw_version,
};
