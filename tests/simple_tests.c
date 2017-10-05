/*
 * Copyright 2017, Google Inc.
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
#include <string.h>

#include "mosys/log.h"

#include "lib/cros_config.h"
#include "lib/sku.h"

static int do_test(const char *fdt, const char * smbios_name, int sku_id,
		   const char *expected_model, const char *expected_brand)
{
	struct sku_info sku_info;
	int ret;

	ret = cros_config_setup_sku(fdt, &sku_info, smbios_name, sku_id);
	if (ret) {
		if (!expected_model)
			return 0;
		fprintf(stdout, "SKU ID %d, expected a config\n", sku_id);
		return 1;
	}
	if (!expected_model) {
		fprintf(stdout, "SKU ID %d, expected no config\n", sku_id);
		return 1;
	}
	if (strcmp(sku_info.model, expected_model)) {
		fprintf(stdout, "SKU ID %d, model is %s\n", sku_id,
			sku_info.model);
		return 1;
	}
	if (!expected_brand && sku_info.brand) {
		fprintf(stdout, "SKU ID %d, expected no brand, got %s\n",
			sku_id, sku_info.brand);
		return 1;
	}
	if (expected_brand && !sku_info.brand) {
		fprintf(stdout, "SKU ID %d, expected brand, got none\n",
			sku_id);
		return 1;
	}
	if (expected_brand && strcmp(sku_info.brand, expected_brand)) {
		fprintf(stdout, "SKU ID %d, brand is %s\n", sku_id,
			sku_info.brand);
		return 1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	extern char __dtb_test_config_begin[];
	char *fdt = __dtb_test_config_begin;
	int ret;

	mosys_log_init("stderr", LOG_SPEW, stderr);

	/* Single SKU */
	ret = do_test(fdt, "pyro", 0, "pyro", "ABCE");
	ret |= do_test(fdt, "snappy", 0, "snappy", "ABCF");
	ret |= do_test(fdt, "sand", 0, "sand", "ABCH");

	/* SMBIOS plus SKU ID lookup */
	ret = do_test(fdt, "coral", 0, "astronaut", NULL);
	ret |= do_test(fdt, "coral", 61, "astronaut", "WXYZ");
	ret |= do_test(fdt, "coral", 62, "astronaut", "ABCD");
	ret |= do_test(fdt, "coral", 160, "nasher", "CPPT");
	ret |= do_test(fdt, "coral", 163, "nasher360", "INUT");

	/* Different SMBIOS name */
	ret |= do_test(fdt, "reef", 0, "basking", "ABCG");

	ret |= do_test(fdt, "reef", 4, "reef", "ABCA");
	ret |= do_test(fdt, "reef", 5, "reef", "ABCA");
	ret |= do_test(fdt, "reef", 8, "electro", "ABCI");

	/* Without an SMBIOS name we should fail */
	ret |= do_test(fdt, NULL, 61, NULL, NULL);

	/* Invalid SKU ID */
	ret |= do_test(fdt, "coral", 255, NULL, NULL);

	if (ret) {
		fprintf(stdout, "Simple tests failed\n");
		return 1;
	}

	fprintf(stdout, "Simple tests passed\n");

	return 0;
}
