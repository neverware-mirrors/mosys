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

#ifndef _CROS_CONFIG_H_
#define _CROS_CONFIG_H_

struct platform_intf;
struct sku_info;

/**
 * cros_config_read_sku_info() - read SKU information for current model
 *
 * Read information about the current model.
 *
 * @intf: Platform information, used to access SMBIOS name and SKU ID
 * @find_platform_names: Array of permitted platform names, end with NULL
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_sku_info(struct platform_intf *intf,
			      const char *find_platform_names[],
			      struct sku_info *sku_info);

/**
 * cros_config_read_sku_info_fdt() - read SKU information for current ARM
 * model using the ARM FDT information.
 *
 * Read information about the current ARM model.
 *
 * @intf: Platform information, used to access ARM FDT name and SKU ID
 * @compat_platform_names: Array of platform names that are permitted,
 *    use the platform id not including the leading 'google,'. Platform
 *    id 'google,gru' should pass in only 'gru'. Platform ids are currently
 *    limited to 256 chars, and the array must end with NULL.
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_sku_info_fdt(struct platform_intf *intf,
			          const char *compat_platform_names[],
			      	  struct sku_info *sku_info);
/**
 * cros_config_read_default_sku_info_fdt() - read SKU information for current
 * ARM model using the ARM FDT information with a default SKU ID to fall back
 * to when no valid SKU ID is found through FDT.
 *
 * Read information about the current ARM model.
 *
 * @intf: Platform information, used to access ARM FDT name and SKU ID
 * @compat_platform_names: Array of platform names that are permitted,
 *    use the platform id not including the leading 'google,'. Platform
 *    id 'google,gru' should pass in only 'gru'. Platform ids are currently
 *    limited to 256 chars, and the array must end with NULL.
 * @default_sku_id: The fallback sku id if fdt_get_sku_id() returns
 *    invalid (<0) sku id. Pass negative value here if we don't want to
 *    fall back.
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_default_sku_info_fdt(struct platform_intf *intf,
					  const char *compat_platform_names[],
					  const int default_sku_id,
					  struct sku_info *sku_info);

/**
 * cros_config_read_forced_sku_info() - read SKU information for current model
 * forcing the sku to a passed in value.
 *
 * Read information about the current model.
 *
 * @intf: Platform information, used to access SMBIOS name and SKU ID
 * @find_platform_names: Array of permitted platform names, end with NULL
 * @forced_sku_number: forced sku number
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_forced_sku_info(struct platform_intf *intf,
			             const char *find_platform_names[],
			             const int forced_sku_number,
			             struct sku_info *sku_info);

/**
 * cros_config_smbios_platform_name_match() - shallow match on sbmios name
 *
 * @intf: Platform information, used to access SMBIOS name and SKU ID
 * @find_platform_names: Array of permitted platform names, end with NULL
 * @return: 0 if OK, other value on error
 */
int cros_config_smbios_platform_name_match(struct platform_intf *intf,
					   const char *find_platform_names[]);

#endif
