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

#include "lib/file.h"
#include "mosys/alloc.h"
#include "mosys/callbacks.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include <errno.h>
#include <unistd.h>

#define WILCO_EC_NAME "wilco"
#define WILCO_EC_VENDOR "google"
#define WILCO_EC_SYSFS_VERSION                                                 \
	"/sys/bus/platform/devices/GOOG000C:00/version"

static const char *wilco_ec_name(struct platform_intf *intf, struct ec_cb *ec)
{
	return WILCO_EC_NAME;
}

static const char *wilco_ec_vendor(struct platform_intf *intf, struct ec_cb *ec)
{
	return WILCO_EC_VENDOR;
}

static const char *wilco_ec_fw_version(struct platform_intf *intf,
				       struct ec_cb *ec)
{
	const size_t max_size = 64;
	int fd, len;
	char *buffer = mosys_zalloc(max_size);

	add_destroy_callback(free, (void *)buffer);

	fd = file_open(WILCO_EC_SYSFS_VERSION, FILE_READ);
	if (fd < 0) {
		strncpy(buffer, "ERROR", max_size);
		return buffer;
	}

	len = read(fd, buffer, max_size);
	if (len <= 0) {
		lprintf(LOG_ERR, "Unable to read %s: %s.\n",
			WILCO_EC_SYSFS_VERSION, strerror(errno));
		strncpy(buffer, "ERROR", max_size);
		goto close_file;
	}

	if (buffer[len - 1] != '\n') {
		lprintf(LOG_ERR, "Value was truncated: '%.*s'\n", len, buffer);
		strncpy(buffer, "ERROR TRUNCATED", max_size);
		goto close_file;
	}

	/* Replace the newline with a null-terminator */
	buffer[len - 1] = '\0';

close_file:
	close(fd);
	return buffer;
}

struct ec_cb wilco_ec_cb = {
	.vendor		= wilco_ec_vendor,
	.name		= wilco_ec_name,
	.fw_version	= wilco_ec_fw_version,
};
