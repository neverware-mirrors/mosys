/*
 * Copyright (C) 2010 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdlib.h>

/* 
 * FIXME: This is too x86-centric right now. We'll need to be smarter
 * about making sane choices for interfaces and commands on other
 * architectures.
 */

#include "mosys/command_list.h"
#include "mosys/platform.h"
#include "mosys/log.h"

#include "lib/smbios.h"

#include "x86.h"

/* FIXME: command/command_list.h is included magically via Makefile */
const char *default_x86_id_list[] = {
	NULL
};

struct platform_cmd *platform_default_x86_sub[] = {
	&cmd_eeprom,
	&cmd_smbios,
	&cmd_platform,
	NULL
};

struct sys_cb default_x86_sys_cb;

struct platform_cb default_x86_cb = {
	.eeprom = &default_x86_eeprom_cb,
	.smbios = &smbios_sysinfo_cb,
	.sys	= &default_x86_sys_cb,
};

int default_x86_probe(struct platform_intf *intf)
{
	return 1;
}

static int default_x86_setup_post(struct platform_intf *intf)
{
	default_x86_eeprom_setup(intf);

	return 0;
}

struct platform_intf platform_default_x86 = {
	.type		= PLATFORM_X86,
	.name		= "Default",
	.probe		= default_x86_probe,
	.id_list	= default_x86_id_list,
	.sub		= platform_default_x86_sub,
	.cb		= &default_x86_cb,
	.setup_post	= &default_x86_setup_post,
};
