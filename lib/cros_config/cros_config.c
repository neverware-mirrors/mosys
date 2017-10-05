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

/**
 * check_sku_map() - Check a single sku-map node for a match
 *
 * This searches the given sku-map node to see if it is a match for the given
 * SKU ID.
 *
 * @fdt: Device tree blob
 * @node: 'sku-map' node to examine
 * @find_sku_id: SKU ID to search for
 * @return phandle found (> 0), if any, 0 if not found, negative on error
 */
static int check_sku_map(const char *fdt, int node, int find_sku_id)
{
	const fdt32_t *data, *end, *ptr;
	int found_phandle = 0;
	int len;

	lperror(LOG_DEBUG, "%s: Trying %s\n", __func__,
			fdt_get_name(fdt, node, NULL));

	/* If we have a single SKU, deal with that first */
	data = fdt_getprop(fdt, node, "single-sku", &len);
	if (data) {
		if (len != sizeof(fdt32_t)) {
			lperror(LOG_ERR, "%s: single-sku: Invalid length %d\n",
				__func__, len);
			return -1;
		}
		found_phandle = fdt32_to_cpu(*data);
		lperror(LOG_DEBUG, "%s: Single SKU match\n", __func__);
		return found_phandle;
	}

	/*
	 * Locate the map and make sure it is a multiple of 2 cells (first is
	 * SKU ID, second is phandle).
	 */
	data = fdt_getprop(fdt, node, "simple-sku-map", &len);
	if (!data)
		return cros_config_fdt_err("find simple-sku-map", len);
	if (len % (sizeof(fdt32_t) * 2)) {
		/*
		 * Validation of configuration should catch this, so this
		 * should never happen. But we don't want to crash.
		 */
		lperror(LOG_ERR, "%s: %s: simple-sku-map: Invalid length %d\n",
			__func__, fdt_get_name(fdt, node, NULL), len);
		return -1;
	}

	/* Search for the SKU ID in the list */
	for (end = (fdt32_t *)((char *)data + len), ptr = data;
	     ptr < end; ptr += 2) {
		int sku_id = fdt32_to_cpu(ptr[0]);
		int phandle = fdt32_to_cpu(ptr[1]);

		if (sku_id == find_sku_id) {
			found_phandle = phandle;
			break;
		}
	}
	if (!found_phandle) {
		lperror(LOG_DEBUG, "%s: SKU ID %d not found in mapping\n",
			__func__, find_sku_id);
		return 0;
	}
	lperror(LOG_DEBUG, "%s: Simple SKU map match\n", __func__);

	return found_phandle;
}

/**
 * check_sku_map() - Check all sku-map nodes for a match
 *
 * This searches all the sku-map subnodes to see if it is a match for the given
 * SKU ID.
 *
 * @fdt: Device tree blob
 * @mapping_node: 'mapping' node to examine
 * @find_sku_id: SKU ID to search for
 * @return phandle found (> 0), if any, 0 if not found, negative on error
 */
static int check_sku_maps(const char *fdt, int mapping_node, int find_sku_id)
{
	int subnode, phandle;

	fdt_for_each_subnode(subnode, fdt, mapping_node) {
		phandle = check_sku_map(fdt, subnode, find_sku_id);
		if (phandle < 0)
			return -1;
		else if (phandle > 0)
			break;
	}

	return phandle;
}

/**
 * follow_phandle() - Find the model node pointed to by a phandle
 *
 * @fdt: Device tree blob
 * @phandle: Phandle to look up
 * @targetp: Returns the target node of the phandle
 * @return model node for this phandle, or negative on error
 */
static int follow_phandle(const char *fdt, int phandle, int *targetp)
{
	const char *parent_name;
	int node, parent;
	int model_node;

	/* Follow the phandle to the target */
	node = fdt_node_offset_by_phandle(fdt, phandle);
	if (node < 0)
		return cros_config_fdt_err("find phandle for sku ID", node);

	/* Figure out whether the target is a model or a sub-model */
	parent = fdt_parent_offset(fdt, node);
	if (parent < 0) {
		return cros_config_fdt_err("find parent of phandle target",
					   node);
	}
	parent_name = fdt_get_name(fdt, parent, NULL);
	if (!strcmp(parent_name, "submodels")) {
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
	*targetp = node;

	return model_node;
}

int cros_config_setup_sku(const char *fdt, struct sku_info *sku_info,
			  int find_sku_id)
{
	int mapping_node, model_node;
	int phandle;
	int target;

	lprintf(LOG_DEBUG, "%s: Looking up SKU ID %d\n", __func__, find_sku_id);
	mapping_node = fdt_path_offset(fdt, "/chromeos/family/mapping");
	if (mapping_node < 0)
		return cros_config_fdt_err("find mapping", mapping_node);

	phandle = check_sku_maps(fdt, mapping_node, find_sku_id);
	if (phandle <= 0)
		goto err;
	model_node = follow_phandle(fdt, phandle, &target);
	if (model_node < 0)
		goto err;

	/* We found the model node, so pull out the data */
	memset(sku_info, '\0', sizeof(*sku_info));
	sku_info->model = fdt_get_name(fdt, model_node, NULL);
	sku_info->brand = fdt_getprop(fdt, target, "brand-code", NULL);
	/* we don't report the sub-model in mosys */
	lprintf(LOG_DEBUG, "%s: Found model '%s'\n", __func__, sku_info->model);

	return 0;
err:
	lperror(LOG_ERR, "%s: Could not locate SKU in mapping\n", __func__);
	return -1;
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
