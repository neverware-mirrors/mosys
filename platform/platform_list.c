/*
 * Copyright 2012, Google Inc.
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

#include <stdlib.h>

#include "mosys/platform.h"

/* experimental */
extern struct platform_intf platform_dummy;

/* production platforms */
extern struct platform_intf platform_auron;
extern struct platform_intf platform_beltino;
extern struct platform_intf platform_cheza;
extern struct platform_intf platform_coral;
extern struct platform_intf platform_cyan;
extern struct platform_intf platform_fizz;
extern struct platform_intf platform_flapjack;
extern struct platform_intf platform_glados;
extern struct platform_intf platform_gru;
extern struct platform_intf platform_hatch;
extern struct platform_intf platform_kahlee;
extern struct platform_intf platform_kukui;
extern struct platform_intf platform_mistral;
extern struct platform_intf platform_nami;
extern struct platform_intf platform_nyan;
extern struct platform_intf platform_oak;
extern struct platform_intf platform_octopus;
extern struct platform_intf platform_pinky;
extern struct platform_intf platform_poppy;
extern struct platform_intf platform_rambi;
extern struct platform_intf platform_reef;
extern struct platform_intf platform_samus;
extern struct platform_intf platform_sarien;
extern struct platform_intf platform_slippy;
extern struct platform_intf platform_storm;
extern struct platform_intf platform_strago;

struct platform_intf *platform_intf_list[] = {
#ifdef CONFIG_PLATFORM_ARCH_X86
	&platform_auron,
	&platform_beltino,
	&platform_coral,
	&platform_cyan,
	&platform_glados,
	&platform_rambi,
	&platform_reef,
	&platform_samus,
	&platform_slippy,
	&platform_strago,
#ifdef CONFIG_CROS_CONFIG
	&platform_fizz,
	&platform_kahlee,
	&platform_hatch,
	&platform_nami,
	&platform_poppy,
	&platform_octopus,
	&platform_sarien,
#endif // CONFIG_PLATFORM_ARCH_X86
/* experimental platforms */
	&platform_dummy,
#endif // CONFIG_PLATFORM_ARCH_X86

#ifdef CONFIG_PLATFORM_ARCH_ARMEL
	&platform_gru,
	&platform_mistral,
	&platform_nyan,
	&platform_oak,
	&platform_pinky,
	&platform_storm,
#ifdef CONFIG_CROS_CONFIG
	&platform_cheza,
	&platform_kukui,
#endif // CONFIG_CROS_CONFIG
#endif // CONFIG_PLATFORM_ARCH_ARMEL
	NULL
};
