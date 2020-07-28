/*
 * Copyright 2014, Google Inc.
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

#ifndef MOSYS_LIB_FDT_H__
#define MOSYS_LIB_FDT_H__

#include <inttypes.h>

#include <mosys/platform.h>

/*
 * fdt_get_ram_code - Obtain RAM code from FDT ram-code node
 *
 * returns 0 to indicate success, <0 to indicate failure.
 */
extern int fdt_get_ram_code(uint32_t *ram_code);

/*
 * fdt_get_board_id - Obtain board ID code from FDT board-id node
 *
 * returns 0 to indicate success, <0 to indicate failure.
 */
extern int fdt_get_board_id(uint32_t *board_id);

/*
 * fdt_get_sku_id  -  return platform SKU ID from FDT sku-id node
 *
 * returns SKU ID in integer
 * returns -1 if not found, other <0 values indicate invalid SKU ID
 */
extern int fdt_get_sku_id(void);

/*
 * fdt_get_frid - retrieve FRID
 *
 * @buf:	buffer to store firmware ID
 * @buf_sz:	the size of the buffer
 *
 * returns length of read only firmware id to indicate success
 * returns <0 to indicate error
 */
extern ssize_t fdt_get_frid(char *buf, size_t buf_sz);

/**
 * fdt_sysinfo_cb - common sysinfo callbacks for ARM
 */
extern struct sys_cb fdt_sysinfo_cb;

#endif	/* MOSYS_LIB_FDT_H__ */
