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

#include <ctype.h>

#include "lib/fdt.h"
#include "lib/flashrom.h"
#include "lib/nonspd.h"
#include "lib/string.h"

#include "mosys/alloc.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "mistral.h"

/* Treat each module as a logical "DIMM" */
#define MISTRAL_DIMM_COUNT	2

#define MISTRAL_RO_VPD_SIZE	0x20000
#define MISTRAL_VPD_ENTRY_VALUE_SIZE	64

enum mistral_memory_config {
	SAMSUNG_DDR3L_1866_512M,
	NANYA_DDR3L_1866_512M,
	HYNIX_DDR3L_1866_512M,
	MEM_UNKNOWN,
};

enum mistral_board_id {
	BOARD_ID_MISTRAL_PROTO = 0, /* 000 */
};

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

/* Parse vpd partition and get value of key */
int parse_vpd(uint8_t *buf, size_t size, char *key,
		char value[MISTRAL_VPD_ENTRY_VALUE_SIZE])
{
	size_t offset = 0;
	int i = 0;

	/* find vpd entry */
	if (find_pattern(buf, size, key, strlen(key), 1, &offset) < 0) {
		lprintf(LOG_ERR, "Unable to find key %s in vpd\n", key);
		return -1;
	}

	/* skip key */
	offset += strlen(key) + 1;

	for (i = 0; i < MISTRAL_VPD_ENTRY_VALUE_SIZE; ++i) {
		if (isprint(buf[offset + i])) {
			value[i] = buf[offset + i];
		} else {
			break;
		}
	}

  return 0;
}

static enum mistral_memory_config get_memory_config(struct platform_intf *intf)
{
	uint8_t *buf = NULL;
	int ret = 0;
	char memory[MISTRAL_VPD_ENTRY_VALUE_SIZE] = {0};
	enum mistral_memory_config memory_config = MEM_UNKNOWN;

	ret = flashrom_read_by_name(&buf, HOST_FIRMWARE, "RO_VPD");
	if (ret != MISTRAL_RO_VPD_SIZE) {
		lprintf(LOG_ERR, "Read RO_VPD error, ret = %d\n", ret);
		goto exit;
	}

	if (parse_vpd(buf, MISTRAL_RO_VPD_SIZE, "dram_id", memory) < 0) {
		lprintf(LOG_ERR, "Unable to determine memory configuration\n");
		goto exit;
	}

	if (!strcmp(memory, "SAMSUNG_K4B4G1646E-BYMA")) {
		memory_config = SAMSUNG_DDR3L_1866_512M;
	} else if (!strcmp(memory, "NANYA_NT5CC256M16ER-EK")) {
		memory_config = NANYA_DDR3L_1866_512M;
	} else if (!strcmp(memory, "HYNIX_H5TC4G63EFR-RDA")) {
		memory_config = HYNIX_DDR3L_1866_512M;
	} else {
		lprintf(LOG_ERR, "Invalid memory configuration %s\n", memory);
		goto exit;
      }
exit:
	if (buf)
		free(buf);
	return memory_config;
}

static int get_mem_info(struct platform_intf *intf,
			const struct nonspd_mem_info **info)
{
	switch (get_memory_config(intf)) {
	case SAMSUNG_DDR3L_1866_512M:
		*info = &samsung_k4b4g1646e_byma;
		break;
	case NANYA_DDR3L_1866_512M:
		*info = &nanya_ddr3l_nt5cc256m16er_ek;
		break;
	case HYNIX_DDR3L_1866_512M:
		*info = &hynix_ddr3l_h5tc4g63efr_rda;
		break;
	default:
		return -1;
	}

	return 0;
}

struct memory_cb mistral_memory_cb = {
	.dimm_count		= dimm_count,
	.nonspd_mem_info	= &get_mem_info,
};
