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

#include <stdlib.h>
#include <string.h>
#include <libfdt.h>

#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/sku.h"
#include "lib/smbios.h"

static int cros_config_fdt_err(const char *where, int err)
{
	lperror(LOG_ERR, "%s: %s: %s\n", __func__, where, fdt_strerror(err));

	return err;
}

int cros_config_setup_sku(const char *fdt, struct sku_info *sku_info,
			  int find_sku_id)
{
	const fdt32_t *data, *end, *ptr;
	int mapping_node, model_node;
	const char *parent_name;
	int found_phandle = 0;
	int parent, node;
	int len;

	lprintf(LOG_DEBUG, "%s: Looking up SKU ID %d\n", __func__, find_sku_id);
	/*
	 * TODO(sjg@chromium.org): Support multiple SKU maps, as needed for
	 * reef-uni
	 */
	mapping_node = fdt_path_offset(fdt, "/chromeos/family/mapping/sku-map");
	if (mapping_node < 0) {
		return cros_config_fdt_err("find mapping", mapping_node);
	}

	/*
	 * Locate the map and make sure it is a multiple of 2 cells (first is
	 * SKU ID, second is phandle).
	 */
	data = fdt_getprop(fdt, mapping_node, "simple-sku-map", &len);
	if (!data) {
		return cros_config_fdt_err("find sku-map", len);
	}
	if (len % (sizeof(fdt32_t) * 2)) {
		/*
		 * Validation of configuration should catch this, so this
		 * should never happen. But we don't want to crash.
		 */
		lperror(LOG_ERR, "%s: Invalid length %d\n", __func__);
		return -1;
	}

	/* Search for the SKU ID in the list */
	for (end = (void *)data + len, ptr = data; ptr < end; ptr += 2) {
		int sku_id = fdt32_to_cpu(ptr[0]);
		int phandle = fdt32_to_cpu(ptr[1]);

		if (sku_id == find_sku_id) {
			found_phandle = phandle;
			break;
		}
	}
	if (!found_phandle) {
		lperror(LOG_ERR, "%s: SKU ID %d not found in mapping\n",
			__func__, find_sku_id);
		return -1;
	}

	/* Follow the phandle to the target */
	node = fdt_node_offset_by_phandle(fdt, found_phandle);
	if (node < 0) {
		return cros_config_fdt_err("find phandle for sku ID", node);
	}

	/* Figure out whether the target is a model or a sub-model */
	parent = fdt_parent_offset(fdt, node);
	if (parent < 0) {
		return cros_config_fdt_err("find parent of phandle target",
					   node);
	}
	parent_name = fdt_get_name(fdt, parent, NULL);
	if (!strcmp(parent_name, "sub-models")) {
		model_node = fdt_parent_offset(fdt, parent);
		if (model_node < 0) {
			return cros_config_fdt_err("find sub-model parent",
						   node);
		}
	} else if (!strcmp(parent_name, "models")) {
		model_node = node;
	} else {
		lperror(LOG_ERR, "%s: phandle target parent '%s' invalid\n",
			__func__, parent_name);
		return -1;
	}

	/* We found the model node, so pull out the data */
	memset(sku_info, '\0', sizeof(*sku_info));
	sku_info->model = fdt_get_name(fdt, model_node, NULL);
	sku_info->brand = fdt_getprop(fdt, node, "brand-code", NULL);
	/* we don't report the sub-model in mosys */
	lprintf(LOG_DEBUG, "%s: Found model '%s'\n", __func__, sku_info->model);

	return 0;
}

int cros_config_read_sku_info(struct platform_intf *intf,
			      struct sku_info *sku_info)
{
	extern char __dtb_config_begin[];
	char *fdt = __dtb_config_begin;
	int sku_id;
	int ret;

	sku_id = smbios_sysinfo_get_sku_number(intf);
	if (sku_id == -1)
		lprintf(LOG_DEBUG, "%s: Unknown SKU ID\n", __func__);
	ret = cros_config_setup_sku(fdt, sku_info, sku_id);
	if (ret) {
		lperror(LOG_ERR, "%s: Failed to read master configuration",
			__func__);
		return -1;
	}

	return 0;
}
