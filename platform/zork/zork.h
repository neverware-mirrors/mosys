/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_ZORK_H__
#define PLATFORM_ZORK_H__

#include "mosys/platform.h"

#define ZORK_HOST_FIRMWARE_ROM_SIZE (16384 * 1024)

/* platform callbacks */
extern struct eeprom_cb zork_eeprom_cb; /* eeprom.c */
extern struct memory_cb zork_memory_cb; /* memory.c */
extern struct sys_cb zork_sys_cb; /* sys.c */

#endif /* PLATFORM_ZORK_H_ */
