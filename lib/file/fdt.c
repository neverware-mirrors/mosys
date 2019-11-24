/* Copyright 2014, Google Inc.
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
 *
 * fdt.c: Helper functions for getting data out of the device tree.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include "lib/chromeos.h"
#include "lib/fdt.h"
#include "lib/file.h"
#include "lib/string_builder.h"

#include "mosys/globals.h"
#include "mosys/log.h"

/* for fdt_set_nvram_cb() */
extern struct nvram_cb cros_ec_nvram_cb;
extern struct nvram_cb cros_spi_flash_nvram_cb;

#define FDT_ROOT		"/proc/device-tree/"
#define FDT_VBNV_STORAGE_PATH	"firmware/chromeos/nonvolatile-context-storage"
#define FDT_FRID_PATH		"firmware/chromeos/readonly-firmware-version"
/* FIXME: assume coreboot for now */
#define FDT_RAM_CODE_PATH	"firmware/coreboot/ram-code"
#define FDT_BOARD_ID_PATH	"firmware/coreboot/board-id"
#define FDT_SKU_ID_PATH		"firmware/coreboot/sku-id"
#define FDT_MODEL_NODE		"model"


/* returns number of bytes read or -1 to indicate error */
static ssize_t fdt_read_node(const char *path, char *buf, size_t buf_sz)
{
	ssize_t ret;
	struct string_builder *sb;

	sb = new_string_builder();
	string_builder_strcat(sb, mosys_get_root_prefix());
	string_builder_strcat(sb, FDT_ROOT);
	string_builder_strcat(sb, path);

	ret = read_file(string_builder_get_string(sb), buf, buf_sz, LOG_ERR);

	free_string_builder(sb);
	return ret;
}

static int fdt_get_uint32_val(const char *path, uint32_t *val)
{
	int len = sizeof(*val);

	if (fdt_read_node(path, (char *)val, len) != len)
		return -1;

	*val = ntohl(*val);
	return 0;
}

int fdt_get_ram_code(uint32_t *ram_code)
{
	if (fdt_get_uint32_val(FDT_RAM_CODE_PATH, ram_code) < 0) {
		lprintf(LOG_ERR, "%s: Error when reading RAM code\n", __func__);
		return -1;
	}

	if (*ram_code == 0xffffffff) {
		lprintf(LOG_ERR, "%s: ram_code is invalid.\n", __func__);
		return -1;
	}

	lprintf(LOG_DEBUG, "%s: ram_code: %u\n", __func__, *ram_code);
	return 0;
}

int fdt_get_board_id(uint32_t *board_id)
{
	if (fdt_get_uint32_val(FDT_BOARD_ID_PATH, board_id) < 0) {
		lprintf(LOG_ERR, "%s: Error when reading board ID\n", __func__);
		return -1;
	}

	if (*board_id == 0xffffffff) {
		lprintf(LOG_ERR, "%s: board_id is invalid.\n", __func__);
		return -1;
	}

	lprintf(LOG_DEBUG, "%s: board_id: %u\n", __func__, *board_id);
	return 0;
}

int fdt_get_sku_id(void)
{
	uint32_t sku_id = 0xffffffff;

	if (fdt_get_uint32_val(FDT_SKU_ID_PATH, &sku_id) < 0) {
		lprintf(LOG_ERR, "%s: Error when reading board ID\n", __func__);
		return -1;
	}

	if (sku_id == 0xffffffff) {
		lprintf(LOG_ERR, "%s: sku_id is invalid.\n", __func__);
		return -1;
	}

	lprintf(LOG_DEBUG, "%s: sku_id: %u\n", __func__, sku_id);
	return sku_id;
}

ssize_t fdt_get_frid(char *buf, size_t buf_sz)
{
	ssize_t len;

	len = fdt_read_node(FDT_FRID_PATH, buf, buf_sz);
	if (len < 0) {
		lprintf(LOG_ERR, "%s: failed to read frid from %s\n",
			__func__, FDT_FRID_PATH);
		return -1;
	}

	return len + 1;
}

static enum vbnv_storage_media fdt_get_vbnv_storage(void)
{
	char buf[8];
	int len = sizeof(buf);
	enum vbnv_storage_media ret;

	if (fdt_read_node(FDT_VBNV_STORAGE_PATH, buf, len) < 0) {
		lprintf(LOG_ERR, "%s: Error when reading VBNV storage"
				"type\n", __func__);
		return VBNV_STORAGE_UNKNOWN;
	}

	if (!strncmp(buf, "cros-ec", len) || !strncmp(buf, "mkbp", len))
		ret = VBNV_STORAGE_CROS_EC;
	else if (!strncmp(buf, "disk", len))
		ret = VBNV_STORAGE_DISK;
	else if (!strncmp(buf, "flash", len))
		ret = VBNV_STORAGE_FLASH;
	else if (!strncmp(buf, "nvram", len))
		ret = VBNV_STORAGE_NVRAM;
	else
		ret = VBNV_STORAGE_UNKNOWN;

	lprintf(LOG_DEBUG, "%s: VBNV storage type: %d\n", __func__, ret);
	return ret;
}

int fdt_set_nvram_cb(struct platform_intf *intf)
{
	switch (fdt_get_vbnv_storage()) {
	case VBNV_STORAGE_CROS_EC:
		intf->cb->nvram = &cros_ec_nvram_cb;
		break;
	case VBNV_STORAGE_DISK:
		lprintf(LOG_ERR, "VBNV from disk is not supported yet.\n");
		return -ENOSYS;
	case VBNV_STORAGE_FLASH:
		intf->cb->nvram = &cros_spi_flash_nvram_cb;
		break;
	case VBNV_STORAGE_NVRAM:
		return -ENOSYS;
	default:
		return -1;
	}

	return 0;
}

/* TODO(hungte) Replace fdt_model by probe_frid. */
const char *fdt_model(void)
{
	static char model[33];
	int len;

	len = fdt_read_node(FDT_MODEL_NODE, model, sizeof(model) - 1);
	if (len < 0)
		return NULL;

	return model;
}
