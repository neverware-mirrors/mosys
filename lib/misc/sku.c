/**
 * Copyright 2017 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/file.h"
#include "lib/sku.h"
#include "lib/string.h"

/*
 * Strips the "end of line" character (\n) in string.
 */
static void _strip_eol(char *str)
{
	char *newline = strchr(str, '\n');
	if (newline)
		*newline = '\0';
}

/*
 * Reads one stripped line from fp and close file.
 *
 * This is a helper utility for functions reading identifier files.
 */
static char *_read_close_stripped_line(FILE *fp)
{
	char buffer[256];

	if (!fp)
		return NULL;

	if (!fgets(buffer, sizeof(buffer), fp)) {
		buffer[0] = '\0';
	} else {
		_strip_eol(buffer);
	}
	fclose(fp);

	if (!*buffer)
		return NULL;
	return mosys_strdup(buffer);
}

/*
 * Reads and returns a VPD value.
 */
static char *_get_vpd_value(const char *key_name)
{
	char command[PATH_MAX];
	FILE *fp = NULL;
	char *value;

	snprintf(command, sizeof(command), "vpd_get_value %s", key_name);
	command[sizeof(command) - 1] = '\0';

	fp = popen(command, "r");
	value = _read_close_stripped_line(fp);
	return value;
}

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

	customization_id = _get_vpd_value("customization_id");
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

const struct sku_info *sku_find_info(struct platform_intf *intf,
                                     const struct sku_mapping *mappings)
{
	int sku_number = 0;

	if (!mappings)
		return NULL;

	sku_number = sku_get_number(intf);
	for (; mappings->info; mappings++) {
		if (sku_number == mappings->number ||
		    mappings->number == SKU_NUMBER_ANY)
			return mappings->info;
	}
	return NULL;
}

char *sku_get_brand(struct platform_intf *intf)
{
	const struct sku_info *info = intf->sku_info;
	const char *legacy_path = "/opt/oem/etc/BRAND_CODE";
	FILE *fp = NULL;

	if (info && info->brand)
		return mosys_strdup(info->brand);

	fp = fopen(legacy_path, "r");
	if (fp)
		return _read_close_stripped_line(fp);

	return _get_vpd_value("rlz_brand_code");
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
	customization_id = _get_vpd_value("customization_id");
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

	customization_id = _get_vpd_value("customization_id");
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

	value = _get_vpd_value("whitelabel_tag");
	if (!value)
		value = "";
	return mosys_strdup(value);
}
