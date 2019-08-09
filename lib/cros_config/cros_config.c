/*
 * Copyright 2017, Google Inc.
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

#define _GNU_SOURCE

#include <errno.h>
#include <libfdt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mosys/alloc.h"
#include "mosys/callbacks.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/acpi.h"
#include "lib/cros_config.h"
#include "lib/cros_config_struct.h"
#include "lib/fdt.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/string.h"

static int get_sku_id(struct platform_intf *intf)
{
#ifdef CONFIG_PLATFORM_ARCH_X86
	return smbios_sysinfo_get_sku_number(intf);
#else
	return fdt_get_sku_id();
#endif
}

static const char *get_firmware_name(struct platform_intf *intf)
{
	static char *fw_name;
	int len;

	if (fw_name)
		return fw_name;

#ifdef CONFIG_PLATFORM_ARCH_X86
	len = acpi_get_frid(&fw_name);
#else
	len = fdt_get_frid(&fw_name);
#endif
	if (len < 0) {
		free(fw_name);
		fw_name = NULL;
	} else {
		/* Discard the version and time stamp. */
		char *dot = strchr(fw_name, '.');
		if (dot)
			*dot = '\0';
		add_destroy_callback(free, fw_name);
	}
	return fw_name;
}

int cros_config_read_sku_info_fdt(struct platform_intf *intf,
				  const char *compat_platform_names[],
				  struct sku_info *sku_info)
{
	return cros_config_read_default_sku_info_fdt(
			intf, compat_platform_names, -1, sku_info);
}

#ifdef CONFIG_PLATFORM_ARCH_ARMEL
static bool match_fdt(const char *name, void *arg)
{
	return probe_fdt_compatible(&name, 1, 1) == 0;
}
#endif

int cros_config_read_default_sku_info_fdt(struct platform_intf *intf,
					  const char *compat_platform_names[],
					  const int default_sku_id,
					  struct sku_info *sku_info)
{
#ifdef CONFIG_PLATFORM_ARCH_ARMEL
	static const int MAX_NAME_LEN = 256;
	static const int NO_PARTIAL_MATCHES = 0;
	int sku_id;

	sku_id = fdt_get_sku_id();
	if (sku_id < 0) {
		lprintf(LOG_DEBUG, "%s: Unknown SKU ID\n", __func__);
		if (default_sku_id >= 0) {
			sku_id = default_sku_id;
			lprintf(LOG_DEBUG, "%s: Force SKU ID to %u\n",
				__func__, default_sku_id);
		} else
			return -1;
	}

	// probe_fdt_compatible() returns an index of the matching name
	// found in the find_platform_names list. Success is an index
	// that is greater than or equal zero.
	for (int i = 0; compat_platform_names[i]; ++i) {
		char platform_name[MAX_NAME_LEN + 1];
		platform_name[MAX_NAME_LEN] = '\0';
		int ret = snprintf(platform_name, MAX_NAME_LEN, "google,%s",
				   compat_platform_names[i]);
		if (ret >= MAX_NAME_LEN) {
			lprintf(LOG_ERR, "Platform name exceeded %d chars\n",
				MAX_NAME_LEN);
			return -1;
		}
		const char *compat_names = &platform_name[0];
		// probe_fdt_compatible() takes a const char* const []
		// as the first parameter, we are only passing one name
		// to check at a time requiring full name matches.
		int found = probe_fdt_compatible(&compat_names, 1,
						 NO_PARTIAL_MATCHES);
		if (found >= 0) {
			// Platform is compatible, now read the sku info to see
			// if we can find a device match.
			return cros_config_read_sku_info_struct(
			    intf, sku_id, sku_info, match_fdt, NULL);
		}
	}
	return -1;
#else // CONFIG_PLATFORM_ARCH_ARMEL
	lprintf(LOG_ERR, "Only ARM platforms should call %s\n", __func__);
	return -1;
#endif
}

static bool match_firmware(const char *expected, void *arg)
{
	const char *fw_name = (const char *)arg;

	/* Firmware name starts with `Google_`, but currently the data in
	 * cros_config may come without `Google_`, or in FDT compatible string
	 * format `google,`.
	 * TODO(hungte) Remove the conversion when we have migrated all
	 * model.yaml records to firmware name.
	 */
	const char *google = "google";
	const int len = strlen(google);
	if (strncasecmp(fw_name, google, len) == 0 && fw_name[len])
		fw_name += len + 1;
	if (strncasecmp(expected, google, len) == 0 && expected[len])
		expected += len + 1;

	return !*expected || !strcasecmp(expected, fw_name);
}

int cros_config_read_default_sku_info(
		struct platform_intf *intf, const char *find_platform_names[],
		struct sku_info *sku_info, int default_sku_id)
{
	const char *firmware_name;
	int sku_id;

	firmware_name = get_firmware_name(intf);
	if (!firmware_name) {
		lprintf(LOG_DEBUG, "%s: Unknown firmware name\n", __func__);
		return -1;
	}
	if (find_platform_names &&
	    !strlfind(firmware_name, find_platform_names, 1))
		return -ENOENT;

	sku_id = get_sku_id(intf);
	if (sku_id == -1) {
		lprintf(LOG_DEBUG, "%s: Unknown SKU ID\n", __func__);
		if (default_sku_id == -1)
			return -1;
		sku_id = default_sku_id;
	}

	return cros_config_read_sku_info_struct(
			intf, sku_id, sku_info, match_firmware,
			(void *)firmware_name);
}

int cros_config_read_sku_info(struct platform_intf *intf,
			      const char *find_platform_names[],
			      struct sku_info *sku_info)
{
	return cros_config_read_default_sku_info(
			intf, find_platform_names, sku_info, -1);
}

int cros_config_probe_default_sku(struct platform_intf *intf,
				  const char *platform_names[],
				  const int default_sku_id)
{
	/* intf->name should match first config name. */
	int config_map_size = 0;
	const struct config_map *configs =
			cros_config_get_config_map(&config_map_size);

	if (!config_map_size)
		return 0;

	if (platform_names) {
		/**
		 * If multiple platforms are supported by same interface,
		 * we have to scan whole config map.
		 */
		int i, found = 0;
		for (i = 0; !found && i < config_map_size; i++) {
			if (strlfind(configs[i].platform_name,
				     platform_names, 1))
				found = 1;
		}
		if (!found)
			return 0;
	} else {
		/* The interface supports single platform. */
		if (strcasecmp(intf->name, configs->platform_name))
			return 0;
	}

	struct sku_info *sku_info = mosys_malloc(sizeof(*sku_info));
	assert(sku_info);

	int r = cros_config_read_default_sku_info(
			intf, NULL, sku_info, default_sku_id);
	/**
	 * cros_config_read_sku_info returns 0 on success while probe functions
	 * should return 1 when found, 0 not found, and <0 for error.
	 */
	if (r == 0) {
		intf->sku_info = sku_info;
		return 1;
	}
	free(sku_info);
	return r < 0 ? r : 0;
}

int cros_config_probe(struct platform_intf *intf, const char *platform_names[])
{
	return cros_config_probe_default_sku(intf, platform_names, -1);
}
