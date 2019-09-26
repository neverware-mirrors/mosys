/*
 * Copyright 2013, Google Inc.
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

#ifndef MOSYS_DRIVERS_INTEL_LYNXPOINT_LP_H__
#define MOSYS_DRIVERS_INTEL_LYNXPOINT_LP_H__

struct platform_intf;
struct gpio_map;
struct gpio_reg;

/*
 * LynxPoint-LP has one SMBus interface, so for the purpose of matching with
 * sysfs entries we don't need to worry about the exact IO address which appears
 * at the end of the string (e.g. "SMBus I801 adapter at 2000").
 */
#define LYNXPOINT_LP_SMBUS_ADAPTER "SMBus I801 adapter"

int lynxpoint_lp_read_gpio(struct platform_intf *intf, struct gpio_map *gpio);

/*
 * lynxpoint_lp_set_gpio  - set GPIO status
 *
 * @intf:	platform interface
 * @gpio:	gpio map
 * @status:	0/1
 *
 * returns 0 if successful
 * returns <0 on read failure
 */
int lynxpoint_lp_set_gpio(struct platform_intf *intf, struct gpio_map *gpio,
			  int state);

/*
 * lynxpoint_lp_gpio_list  -  list all GPIOs and their states
 *
 * @intf:	platform interface
 *
 * returns 0 if successful
 * returns <0 if failure
 */
int lynxpoint_lp_gpio_list(struct platform_intf *intf);

/*
 * lynxpoint_lp_list_gpio_attributes - list GPIO's attributes
 *
 * @reg:        GPIO's attributes
 *
 * returns 0 if successful
 * returns <0 if failure
 */
int lynxpoint_lp_list_gpio_attributes(struct gpio_map *gpio,
				      struct gpio_reg *reg);

/*
 * lynxpoint_lp_get_gpio_attributes - get GPIO's attributes
 *
 * @intf:	platform interface
 * @gen: 	ich_generation
 * @gpio:	gpio map
 * @reg: 	GPIO's attributes
 *
 * returns 0 if successful
 * returns <0 if failure
 */
int lynxpoint_lp_read_gpio_attributes(struct platform_intf *intf,
				      struct gpio_map *gpio,
				      struct gpio_reg *reg);

#endif /* MOSYS_DRIVERS_INTEL_LYNXPOINT_LP_H__ */
