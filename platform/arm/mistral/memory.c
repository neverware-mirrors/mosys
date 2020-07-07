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

#include <stdbool.h>
#include <stddef.h>

#include "lib/file.h"
#include "lib/math.h"
#include "lib/nonspd.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "mistral.h"

/* Treat each module as a logical "DIMM" */
#define MISTRAL_DIMM_COUNT	2

#define MISTRAL_VPD_ENTRY_VALUE_SIZE	64

/*
 * dimm_count  -  return total number of dimm slots
 *
 * @intf:       platform interface
 *
 * returns dimm slot count
 */
static int dimm_count(struct platform_intf *intf)
{
	return MISTRAL_DIMM_COUNT;
}

const static struct {
	const char *part_num;
	const struct nonspd_mem_info *info;
} memory_configurations[] = {
	{ "SAMSUNG_K4B4G1646E-BYMA", &samsung_k4b4g1646e_byma },
	{ "NANYA_NT5CC256M16ER-EK", &nanya_ddr3l_nt5cc256m16er_ek },
	{ "HYNIX_H5TC4G63EFR-RDA", &hynix_ddr3l_h5tc4g63efr_rda },
};

static int get_mem_info(struct platform_intf *intf, int dimm,
			const struct nonspd_mem_info **info_out)
{
	static bool first_run = true;
	static const struct nonspd_mem_info *info;
	static char part_num[MISTRAL_VPD_ENTRY_VALUE_SIZE];

	if (!first_run)
		goto exit;
	first_run = false;

	if (read_file("/sys/firmware/vpd/ro/dram_id", part_num,
		      sizeof(part_num), LOG_ERR) < 0)
		return -1;

	for (size_t i = 0; i < ARRAY_SIZE(memory_configurations); i++) {
		if (!strcmp(part_num, memory_configurations[i].part_num)) {
			info = memory_configurations[i].info;
			break;
		}
	}

exit:
	*info_out = info;

	if (!info) {
		lprintf(LOG_ERR, "Unknown memory part number: %s", part_num);
		return -1;
	}

	return 0;
}

struct memory_cb mistral_memory_cb = {
	.dimm_count		= dimm_count,
	.nonspd_mem_info	= &get_mem_info,
};
