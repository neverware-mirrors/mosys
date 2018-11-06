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
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/cros_config.h"
#include "lib/cros_config_struct.h"
#include "lib/fdt.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"
#include "lib/string.h"


#ifdef CONFIG_PLATFORM_ARCH_X86
/**
 * string_in_list() - Check if a name is in a comma-separated list
 *
 * @name: Name to search for
 * @list: List of names to search in, separated by a comma
 * @return true if found, false if not found
 */
static bool string_in_list(const char *name, const char *list)
{
	const char *p, *end;

	for (p = list; *p; p = end + (*end == ',')) {
		end = strchrnul(p, ',');
		if (!strncmp(name, p, end - p))
			return true;
	}

	return false;
}
#endif // CONFIG_PLATFORM_ARCH_X86


#ifdef CONFIG_PLATFORM_ARCH_X86
/** internal function with common code to read sku info */
int internal_cros_config_read_sku_info(struct platform_intf *intf,
				       const int sku_number,
				       const char *smbios_name,
				       struct sku_info *sku_info)
{
	lprintf(LOG_DEBUG, "%s: JSON lookup SMBIOS name '%s', SKU ID %d\n",
		__func__, smbios_name ? smbios_name : "(null)", sku_number);

	return cros_config_read_sku_info_struct(intf, smbios_name, sku_number,
					       sku_info);
}

#endif // CONFIG_PLATFORM_ARCH_X86

int cros_config_read_sku_info_fdt(struct platform_intf *intf,
				  const char *compat_platform_names[],
				  int compat_platform_names_size,
				  struct sku_info *sku_info)
{
	return cros_config_read_default_sku_info_fdt(intf,
		compat_platform_names, compat_platform_names_size,
		-1, sku_info);
}

int cros_config_read_default_sku_info_fdt(struct platform_intf *intf,
					  const char *compat_platform_names[],
					  int compat_platform_names_size,
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
	for (int i = 0; i < compat_platform_names_size; ++i) {
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
			return cros_config_read_sku_info_struct(intf, NULL,
								sku_id,
								sku_info);
		}
	}
	return -1;
#else // CONFIG_PLATFORM_ARCH_ARMEL
	lprintf(LOG_ERR, "Only ARM platforms should call %s\n", __func__);
	return -1;
#endif
}

int cros_config_read_sku_info(struct platform_intf *intf,
			      const char *find_platform_names,
			      struct sku_info *sku_info)
{
#ifdef CONFIG_PLATFORM_ARCH_X86
	const char *smbios_name;
	int sku_id;

	smbios_name = smbios_sysinfo_get_name(intf);
	if (!smbios_name)
		lprintf(LOG_DEBUG, "%s: Unknown SMBIOS name\n", __func__);
	sku_id = smbios_sysinfo_get_sku_number(intf);
	if (sku_id == -1)
		lprintf(LOG_DEBUG, "%s: Unknown SKU ID\n", __func__);

	if (smbios_name && !string_in_list(smbios_name, find_platform_names)) {
		lprintf(LOG_DEBUG, "%s: Could not locate name '%s' in '%s'\n",
			__func__, smbios_name, find_platform_names);
		return -ENOENT;
	}
	return internal_cros_config_read_sku_info(intf, sku_id, smbios_name,
						  sku_info);
#else // CONFIG_PLATFORM_ARCH_X86
	lprintf(LOG_ERR, "Only X86 platforms should call %s\n", __func__);
	return -1;
#endif
}

// TODO(gmeinke): combine read forced sku with existing read function.
int cros_config_read_forced_sku_info(struct platform_intf *intf,
				     const char *find_platform_names,
				     const int forced_sku_number,
				     struct sku_info *sku_info)
{
#ifdef CONFIG_PLATFORM_ARCH_X86
	const char *smbios_name;

	smbios_name = smbios_sysinfo_get_name(intf);
	if (!smbios_name)
		lprintf(LOG_DEBUG, "%s: Unknown SMBIOS name\n", __func__);

	if (smbios_name && !string_in_list(smbios_name, find_platform_names)) {
		lprintf(LOG_DEBUG, "%s: Could not locate name '%s' in '%s'\n",
			__func__, smbios_name, find_platform_names);
		return -ENOENT;
	}
	return internal_cros_config_read_sku_info(intf, forced_sku_number,
						  smbios_name, sku_info);
#else // CONFIG_PLATFORM_ARCH_X86
	lprintf(LOG_ERR, "Only X86 platforms should call %s\n", __func__);
	return -1;
#endif
}

int cros_config_smbios_platform_name_match(struct platform_intf *intf,
					   const char *find_platform_names)
{
#ifdef CONFIG_PLATFORM_ARCH_X86
	const char *smbios_name;

	smbios_name = smbios_sysinfo_get_name(intf);
	if (!smbios_name) {
		lprintf(LOG_DEBUG, "%s: Unknown SMBIOS name\n", __func__);
		return -ENOENT;
	}

	if (!string_in_list(smbios_name, find_platform_names)) {
		lprintf(LOG_DEBUG, "%s: Could not locate name '%s' in '%s'\n",
			__func__, smbios_name, find_platform_names);
		return -ENOENT;
	}

	return 0;
#else // CONFIG_PLATFORM_ARCH_X86
	return -ENOENT;
#endif
}
