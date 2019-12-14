/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_PUFF_H__
#define PLATFORM_PUFF_H__

#include <inttypes.h>
#include "mosys/platform.h"

#define PUFF_HOST_FIRMWARE_ROM_SIZE_32MB	(32768 * 1024)

/* platform callbacks */
extern struct eeprom_cb puff_eeprom_cb;	/* eeprom.c */
extern struct memory_cb puff_memory_cb;	/* memory.c */
extern struct sys_cb puff_sys_cb;		/* sys.c */

#endif /* PLATFORM_PUFF_H_ */
