/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <stdlib.h>

#include "mosys/platform.h"

extern struct platform_intf platform_auron;
extern struct platform_intf platform_beltino;
extern struct platform_intf platform_coral;
extern struct platform_intf platform_cyan;
extern struct platform_intf platform_dedede;
extern struct platform_intf platform_fizz;
extern struct platform_intf platform_glados;
extern struct platform_intf platform_hatch;
extern struct platform_intf platform_kahlee;
extern struct platform_intf platform_octopus;
extern struct platform_intf platform_poppy;
extern struct platform_intf platform_puff;
extern struct platform_intf platform_rambi;
extern struct platform_intf platform_reef;
extern struct platform_intf platform_sarien;
extern struct platform_intf platform_strago;
extern struct platform_intf platform_volteer;
extern struct platform_intf platform_zork;

struct platform_intf *platform_intf_list[] = {
#ifdef CONFIG_CROS_CONFIG
	&platform_coral,
        &platform_dedede,
	&platform_fizz,
	&platform_kahlee,
	&platform_hatch,
	&platform_poppy,
	&platform_puff,
	&platform_octopus,
	&platform_sarien,
        &platform_volteer,
	&platform_zork,
#else
	&platform_auron,
	&platform_beltino,
	&platform_cyan,
	&platform_glados,
	&platform_rambi,
	&platform_strago,
#endif
	/* Reef is a special-snowflake since the unibuild migration
	   was never finished. */
	&platform_reef,
	NULL,
};
