/*
 * Copyright 2019, Google LLC.
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

#include <stdio.h>
#include <errno.h>

#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/platform.h"

static int ish_info(struct platform_intf *intf, struct platform_cmd *cmd,
		   int argc, char **argv)
{
	struct kv_pair *kv;
	int rc;

	if (!intf->cb->ish) {
		errno = ENOSYS;
		return -1;
	}

	kv = kv_pair_new();
	if (!kv) {
		errno = ENOMEM;
		return -1;
	}

	if (!intf->cb->ish->vendor || !intf->cb->ish->name ||
	    !intf->cb->ish->fw_version) {
		errno = ENOSYS;
		rc = -1;
		goto ish_info_done;
	}

	kv_pair_add(kv, "vendor",
			intf->cb->ish->vendor(intf, intf->cb->ish));
	kv_pair_add(kv, "name", intf->cb->ish->name(intf, intf->cb->ish));
	kv_pair_add(kv, "fw_version",
			intf->cb->ish->fw_version(intf, intf->cb->ish));

	rc = kv_pair_print(kv);

ish_info_done:
	kv_pair_free(kv);
	return rc;
}

struct platform_cmd ish_cmds[] = {
	{
		.name	= "info",
		.desc	= "Print basic EC information",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = ish_info}
	},
	{ NULL }
};

struct platform_cmd cmd_ish = {
	.name	= "ish",
	.desc	= "ISH information",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = ish_cmds }
};
