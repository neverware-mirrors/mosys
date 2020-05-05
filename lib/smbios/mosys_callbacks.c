/* Copyright 2012, Google Inc.
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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "mosys/alloc.h"
#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/file.h"
#include "lib/smbios.h"
#include "lib/smbios_tables.h"

#define	SYSFS_SMBIOS_DIR	"/sys/class/dmi/id"

/*
 * smbios_scan_sysfs - scan for a SMBIOS entry by filename and return its value
 *
 * @filename:	filename to scan for
 *
 * Note: User must remember to free allocated memory
 *
 * returns pointer to allocated memory containing SMBIOS info
 * returns NULL if not found
 */
static char *smbios_scan_sysfs(const char *filename)
{
	ssize_t bytes_read;
	char path[PATH_MAX];
	char contents[SMBIOS_MAX_STRING_LENGTH + 1];

	snprintf(path, sizeof(path), SYSFS_SMBIOS_DIR "/%s", filename);
	bytes_read = read_file(path, contents, sizeof(contents), LOG_DEBUG);
	if (bytes_read <= 0)
		return NULL;

	/* Trim off trailing newline, if there is one. */
	if (contents[bytes_read - 1] == '\n')
		contents[bytes_read - 1] = '\0';

	return mosys_strdup(contents);
}

/*
 * smbios_sysinfo_get_vendor  -  return platform vendor
 *
 * @intf:       platform interface
 *
 * returns pointer to allocated platform vendor string
 * returns NULL if not found
 */
char *smbios_sysinfo_get_vendor(struct platform_intf *intf)
{
	char *str = NULL;
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_SYSTEM, 0, &table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0) {
		lprintf(LOG_DEBUG, "%s: normal method failed, "
		                   "trying sysfs\n", __func__);
		str = smbios_scan_sysfs("sys_vendor");
	} else {
		str = mosys_strdup(table.string
				   [table.data.system.manufacturer]);
	}

	return str;
}

/*
 * smbios_sysinfo_get_name  -  return platform product name
 *
 * @intf:       platform interface
 *
 * returns pointer to allocated platform name string
 * returns NULL if not found
 */
char *smbios_sysinfo_get_name(struct platform_intf *intf)
{
	char *str = NULL;
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_SYSTEM, 0, &table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0) {
		lprintf(LOG_DEBUG, "%s: attempting to use sysfs\n", __func__);
		str = smbios_scan_sysfs("product_name");
	} else {
		str = mosys_strdup(table.string[table.data.system.name]);
	}

	return str;
}

/*
 * smbios_sysinfo_get_version  -  return platform version
 *
 * @intf:       platform interface
 *
 * returns pointer to allocated platform version string
 * returns NULL if not found
 */
char *smbios_sysinfo_get_version(struct platform_intf *intf)
{
	char *str = NULL;
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_SYSTEM, 0, &table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0) {
		lprintf(LOG_INFO, "%s: normal approach failed, trying sysfs\n",
		                  __func__);
		str = smbios_scan_sysfs("product_version");
	} else {
		str = mosys_strdup(table.string[table.data.system.version]);
	}

	return str;
}

/*
 * smbios_sysinfo_get_sku  -  return platform SKU
 *
 * @intf:       platform interface
 *
 * returns pointer to allocated platform SKU string
 * returns NULL if not found
 */
char *smbios_sysinfo_get_sku(struct platform_intf *intf)
{
	struct smbios_table table;

	if (smbios_find_table(intf, SMBIOS_TYPE_SYSTEM, 0, &table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0)
		return NULL;

	return mosys_strdup(table.string[table.data.system.sku_number]);
}

/*
 * smbios_sysinfo_get_sku_number  -  return platform SKU number
 *
 * @intf:       platform interface
 *
 * returns SKU number in integer
 * returns -1 if not found
 */
int smbios_sysinfo_get_sku_number(struct platform_intf *intf)
{
	char *sku_var = smbios_sysinfo_get_sku(intf);
	int result = -1;

	lprintf(LOG_DEBUG, "got sku_var='%s'\n", sku_var ? sku_var : "(null)");
	if (sku_var) {
		/* Coreboot writes 'sku%d'. */
		sscanf(sku_var, "sku%d", &result);
		free(sku_var);
	}
	return result;
}

struct smbios_cb smbios_sysinfo_cb = {
	.system_vendor		= smbios_sysinfo_get_vendor,
	.system_name		= smbios_sysinfo_get_name,
	.system_version		= smbios_sysinfo_get_version,
	.system_sku		= smbios_sysinfo_get_sku,
};

/*
 * Memory callbacks
 */

/*
 * smbios_dimm_count  -  return total number of dimm slots
 *
 * @intf:       platform interface
 *
 * returns dimm slot count
 */
int smbios_dimm_count(struct platform_intf *intf)
{
	int status = 0, dimm_cnt = 0;
	struct smbios_table table;

	while (status == 0) {
		status = smbios_find_table(intf, SMBIOS_TYPE_MEMORY, dimm_cnt,
					   &table,
					   SMBIOS_LEGACY_ENTRY_BASE,
					   SMBIOS_LEGACY_ENTRY_LEN);
		if (status == 0)
			dimm_cnt++;
	}

	return dimm_cnt;
}

int smbios_dimm_speed(struct platform_intf *intf,
		     int dimm, struct kv_pair *kv)
{
	struct smbios_table table;
	if (smbios_find_table(intf, SMBIOS_TYPE_MEMORY, dimm, &table,
			      SMBIOS_LEGACY_ENTRY_BASE,
			      SMBIOS_LEGACY_ENTRY_LEN) < 0) {
		return -1;
	}

	kv_pair_fmt(kv, "speed", "%d MHz", table.data.mem_device.speed);

	return 0;
}
