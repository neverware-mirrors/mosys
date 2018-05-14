/*
 * Copyright 2018, Google Inc.
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
#include <unistd.h>

#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"

#include "lib/elog.h"
#include "lib/smbios.h"

#include "dummy.h"

const char *dummy_id_list[] = {"Dummy", NULL};

struct platform_cmd *dummy_sub[] = {
	// No commands
	NULL};

int dummy_probe(struct platform_intf *intf)
{
	// Always fail probe so we don't ever run on real hardware. We can
	// force this platform in tests.
	return 0;
}

struct eventlog_cb dummy_eventlog_cb = {
	.print_type = &elog_print_type,
	.print_data = &elog_print_data,
	.print_multi = &elog_print_multi,
	.verify = &elog_verify,
	.verify_header = &elog_verify_header,
	.fetch = &elog_fetch_from_smbios,
};

struct platform_cb dummy_cb = {
	.ec = &cros_ec_cb,
	.smbios = &smbios_sysinfo_cb,
	.eventlog = &dummy_eventlog_cb,
};

struct platform_intf platform_dummy = {
	.type = PLATFORM_X86_64,
	.name = "Dummy",
	.id_list = dummy_id_list,
	.sub = dummy_sub,
	.cb = &dummy_cb,
	.probe = &dummy_probe,
};
