/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_DEDEDE_H__
#define PLATFORM_DEDEDE_H__

#include <inttypes.h>
#include "mosys/platform.h"

#define DEDEDE_HOST_FIRMWARE_ROM_SIZE	(32768 * 1024)
/* platform callbacks */
extern struct eeprom_cb dedede_eeprom_cb;	/* eeprom.c */
extern struct sys_cb dedede_sys_cb;		/* sys.c */

#endif
