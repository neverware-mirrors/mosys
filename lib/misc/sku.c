/**
 * Copyright 2017 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>
#include <string.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/file.h"
#include "lib/sku.h"
#include "lib/string.h"
#include "lib/vpd.h"

/*
 * Extracts the SERIES part from VPD "customization_id".
 *
 * customization_id should be in LOEMID-SERIES format.
 * If - is not found, return nothing.
 */
static char *_extract_customization_id_series_part(void)
{
	char *customization_id;
	char *series = NULL, *dash;

	customization_id = vpd_get_value("customization_id");
	if (!customization_id)
		return NULL;

	dash = strchr(customization_id, '-');
	if (dash) {
		series = mosys_strdup(dash + 1);
	}
	free(customization_id);

	return series;
}

int sku_get_number(struct platform_intf *intf)
{
	if (!intf || !intf->cb || !intf->cb->sys) {
		return -1;
	}

	if (intf->cb->sys->sku_number) {
		return intf->cb->sys->sku_number(intf);
	}

	return -1;
}

char *sku_get_brand(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;

	if (info && info->brand)
		return mosys_strdup(info->brand);

	return vpd_get_value("rlz_brand_code");
}

char *sku_get_chassis(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;
	char *result = NULL;

	if (info && info->chassis)
		return mosys_strdup(info->chassis);

	/* Use ${model%%.*} as default value. */
	result = sku_get_model(intf);
	if (!result && intf->name)
		result = mosys_strdup(intf->name);
	if (result) {
		char *dot = strchr(result, '.');
		if (dot)
			*dot = '\0';
		strupper(result);
	}
	return result;
}

char *sku_get_model(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;
	char *result = NULL;

	if (info && info->model)
		return mosys_strdup(info->model);

	/* First try customization_id (go/cros-chassis-id). */
	result = _extract_customization_id_series_part();
	if (!result && intf->name)
		result = mosys_strdup(intf->name);
	if (result)
		strlower(result);
	return result;
}

char *sku_get_vpd_customization(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;
	char *customization_id;

	/* Look for VPD first before looking into model */
	customization_id = vpd_get_value("customization_id");
	if (customization_id)
		return customization_id;

	if (info && info->customization)
		return mosys_strdup(info->customization);

	return NULL;
}

char *sku_get_customization(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;
	char *customization_id;

	/* Look for model first before looking into VPD */
	if (info && info->customization)
		return mosys_strdup(info->customization);

	customization_id = vpd_get_value("customization_id");
	if (customization_id)
		return customization_id;

	return NULL;
}

/*
 * Do not use this outside of unibuild since it is not meaningful to have a
 * signature ID in a legacy build.
 */
char *sku_get_whitelabel_from_vpd(void)
{
	const char *value;

	value = vpd_get_value("whitelabel_tag");
	if (!value)
		value = "";
	return mosys_strdup(value);
}
