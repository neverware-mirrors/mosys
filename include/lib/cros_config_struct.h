/*
 * Copyright 2018, Google Inc.
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

#ifndef _CROS_CONFIG_STRUCT_H_
#define _CROS_CONFIG_STRUCT_H_

#include "lib/sku.h"
#include "mosys/platform.h"

/**
 * config_map - a struct to hold a mapping of config identity to config.
 *
 * TODO(chromium:925078): Update description of platform name.
 * `platform_name` Platform name from identity.
 *         Ignored now since mosys conditionally calls cros_config already.
 * `smbios_match_name` SMBIOS name to match.
 * `firmware_name_match` String used to match against firmware output, e.g.
           SMBIOS name on x86 or device-tree compatible string on ARM.
 * `sku_id` SKU id to match.
 * `customization_id` 'customization-id' to match from the VPD.
 * `whitelabel_tag` 'whitelabel-tag' to match from the VPD.
 * 'sku_info' Corresonding sku_info for these attributes.
 */
struct config_map {
	// Set to "" if match is not required.
	const char *firmware_name_match;
	// Set to -1 if match is not required.
	int sku_id;
	const char *platform_name;
	// Set to "" if match is not required.
	const char *customization_id;
	// Set to "" if match is not required.
	const char *whitelabel_tag;
	struct sku_info info;
};

/**
 * cros_config_read_sku_info_struct() - read SKU information for current device
 *
 * Read information about the current device using generated struct data.
 *
 * @intf: Platform information, used to access SMBIOS name and SKU ID
 * @smbios_name: SMBIOS name read from the platform, pass NULL for ARM
 * @sku_id: SKU ID read from the platform.
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_sku_info_struct(struct platform_intf *intf,
				     const char *smbios_name, int sku_id,
				     struct sku_info *sku_info);

/**
 * cros_config_get_config_map() - Returns a handle to the config map.
 *
 * This function exists so the config map can be auto-generated in a
 * separate code file at build time.
 *
 * @num_entries: Value is set to the array size.
 *
 * @return: Pointer to the array of config_map entries.
 */
const struct config_map *cros_config_get_config_map(int *num_entries);

#endif
