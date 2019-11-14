/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_VOLTEER_H__
#define PLATFORM_VOLTEER_H__

#include <inttypes.h>
#include "mosys/platform.h"

#define VOLTEER_HOST_FIRMWARE_ROM_SIZE_32MB	(32768 * 1024)

/* platform callbacks */
extern struct eeprom_cb volteer_eeprom_cb;	/* eeprom.c */
extern struct memory_cb volteer_memory_cb;	/* memory.c */
extern struct nvram_cb volteer_nvram_cb;		/* nvram.c */
extern struct sys_cb volteer_sys_cb;		/* sys.c */

#endif /* PLATFORM_VOLTEER_H_ */
