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
 * @sku_info: Returns SKU information on success
 * @return: 0 if OK, other value on error
 */
int cros_config_read_sku_info(struct platform_intf *intf,
			      struct sku_info *sku_info);

/**
 * cros_config_setup_sku() - read SKU information (internal functions)
 *
 * Read information about a particular SKU ID. Only use this directly for
 * testing.
 *
 * @fdt: Device tree file
 * @sku_info: Returns SKU information on success
 * @find_smbios_name: SMBIOS name to look up
 * @find_sku_id: SKU ID to look up
 * @return: 0 if OK, other value on error
 */
int cros_config_setup_sku(const char *fdt, struct sku_info *sku_info,
			  const char *find_smbios_name, int find_sku_id);

#endif
