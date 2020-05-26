/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_ASURADA_H
#define PLATFORM_ASURADA_H

#include "mosys/platform.h"

/* platform callbacks */
extern struct eeprom_cb asurada_eeprom_cb;
extern struct memory_cb asurada_memory_cb;
extern struct sys_cb asurada_sys_cb;

#endif /* PLATFORM_ASURADA_H */
