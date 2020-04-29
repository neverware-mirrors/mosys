/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>

#include "mosys/platform.h"

extern struct platform_intf platform_cheza;
extern struct platform_intf platform_gru;
extern struct platform_intf platform_kukui;
extern struct platform_intf platform_trogdor;

struct platform_intf *platform_intf_list[] = {
#ifdef CONFIG_CROS_CONFIG
	&platform_cheza,
	&platform_kukui,
	&platform_trogdor,
#endif
	&platform_gru,
	NULL,
};
