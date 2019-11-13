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

#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/callbacks.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/acpi.h"
#include "lib/fdt.h"
#include "lib/file.h"
#include "lib/probe.h"
#include "lib/smbios.h"
#include "lib/string.h"

static int plat_get_frid(char **buf)
{
#ifdef CONFIG_PLATFORM_ARCH_X86
	return acpi_get_frid(buf);
#else
	return fdt_get_frid(buf);
#endif
}

int probe_frid(const char *frids[])
{
	int ret = 0;
	off_t len;
	static char *id = NULL;

	if (!id) {
		char *raw_frid = NULL, *tmp;

		if (plat_get_frid(&raw_frid) < 0)
			goto probe_frid_done;

		/* FRID begins with platform name, followed by a dot, followed
		 * by revision ID */
		tmp = strchr(raw_frid, '.');
		if (!tmp) {
			lprintf(LOG_DEBUG, "%s: Invalid FRID: \"%s\"\n",
			                   __func__, raw_frid);
			free(raw_frid);
			goto probe_frid_done;
		}

		len = tmp - raw_frid + 1;
		id = mosys_malloc(len + 1);
		snprintf(id, len, "%s", raw_frid);
		lprintf(LOG_DEBUG, "%s: Platform name: \"%s\"\n", __func__, id);
		free(raw_frid);
		add_destroy_callback(free, id);
	}

	if (strlfind(id, frids, 0)) {
		lprintf(LOG_DEBUG, "%s: matched id \"%s\"\n", __func__, id);
		ret = 1;
	}

probe_frid_done:
	return ret;
}

int probe_smbios(struct platform_intf *intf, const char *ids[])
{
	static char *id = NULL;
	int ret = 0;

	if (id)
		goto probe_smbios_cmp;

	/* Attempt platform-specific SMBIOS handler if one exists, else use the
	 * default approach. */
	if (intf->cb->smbios && intf->cb->smbios->system_name) {
		id = intf->cb->smbios->system_name(intf);
	} else {
		struct smbios_table table;
		if (smbios_find_table(intf, SMBIOS_TYPE_SYSTEM, 0, &table,
				      SMBIOS_LEGACY_ENTRY_BASE,
				      SMBIOS_LEGACY_ENTRY_LEN) == 0)
			id = table.string[table.data.system.name];
	}

probe_smbios_cmp:
	if (!id) {
		ret = 0;
		lprintf(LOG_SPEW, "%s: cannot find product name\n", __func__);
	} else if (strlfind(id, ids, 0)) {
		ret = 1;
		lprintf(LOG_DEBUG, "%s: matched id \"%s\"\n", __func__, id);
	}
	return ret;
}

#define FDT_COMPATIBLE "/proc/device-tree/compatible"
int probe_fdt_compatible(const char * const id_list[], int num_ids,
			 int allow_partial)
{
	int ret = -1, i, fd;
	char path[PATH_MAX];
	char compat[64];	/* arbitrarily chosen max size */
	char *p;

	lprintf(LOG_DEBUG, "Probing platform with FDT compatible node\n");

	snprintf(path, PATH_MAX, "%s/%s",
			mosys_get_root_prefix(), FDT_COMPATIBLE);
	fd = file_open(path, FILE_READ);
	if (fd < 0) {
		lprintf(LOG_DEBUG, "Cannot open %s\n", path);
		return -1;
	}

	/*
	 * Device tree "compatible" data consists of a list of comma-separated
	 * pairs with a NULL after each pair. For example, "foo,bar\0bam,baz\0"
	 * is foo,bar and bam,baz.
	 */
	p = &compat[0];
	while (read(fd, p, 1) == 1) {
		if (*p != 0) {
			p++;
			if (p - &compat[0] > sizeof(compat)) {
				lprintf(LOG_ERR,
					"FDT compatible identifier too long\n");
				goto probe_fdt_compatible_exit;
			}
			continue;
		}

		for (i = 0; (i < num_ids) && id_list[i]; i++) {
			int cmp = 0;

			lprintf(LOG_DEBUG, "\t\"%s\" == \"%s\" ? ",
					&compat[0], id_list[i]);

			if (allow_partial) {
				cmp = strncmp(&compat[0],
					id_list[i], strlen(id_list[i]));
			} else {
				cmp = strcmp(&compat[0], id_list[i]);
			}

			if (!cmp) {
				lprintf(LOG_DEBUG, "yes\n");
				ret = i;
				goto probe_fdt_compatible_exit;
			} else {
				lprintf(LOG_DEBUG, "no\n");
			}
		}

		p = &compat[0];
	}

probe_fdt_compatible_exit:
	close(fd);
	return ret;
}
