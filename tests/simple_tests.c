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

static int do_test(const char *fdt, const char *platform_names,
		   const char *smbios_name, int sku_id,
		   const char *wl_name, const char *expected_model,
		   const char *expected_brand, const char *expected_platform,
		   const char *expected_sig_id)
{
	const char *platform_name;
	struct sku_info sku_info;
	int ret;

	ret = cros_config_setup_sku(fdt, &sku_info, platform_names,
				    smbios_name, sku_id, wl_name,
				    &platform_name);
	if (ret) {
		if (!expected_model)
			goto pass;
		fprintf(stdout, "SKU ID %d, expected a config\n", sku_id);
		goto err;
	}
	if (!expected_model) {
		fprintf(stdout, "SKU ID %d, expected no config\n", sku_id);
		goto err;
	}
	if (strcmp(sku_info.model, expected_model)) {
		fprintf(stdout, "SKU ID %d, model is %s, expected %s\n", sku_id,
			sku_info.model, expected_model);
		goto err;
	}
	if (!expected_brand && sku_info.brand) {
		fprintf(stdout, "SKU ID %d, expected no brand, got %s\n",
			sku_id, sku_info.brand);
		goto err;
	}
	if (expected_brand && !sku_info.brand) {
		fprintf(stdout, "SKU ID %d, expected brand, got none\n",
			sku_id);
		goto err;
	}
	if (expected_brand && strcmp(sku_info.brand, expected_brand)) {
		fprintf(stdout, "SKU ID %d, brand is %s\n", sku_id,
			sku_info.brand);
		goto err;
	}
	if (expected_platform && !platform_name) {
		fprintf(stdout, "SKU ID %d, expected platform, got none\n",
			sku_id);
		goto err;
	}
	if (expected_platform && strcmp(platform_name, expected_platform)) {
		fprintf(stdout, "SKU ID %d, platform is %s\n", sku_id,
			platform_name);
		goto err;
	}
	if (!expected_sig_id)
		expected_sig_id = expected_model;
	if (strcmp(sku_info.signature_id, expected_sig_id)) {
		fprintf(stdout, "SKU ID %d, signature_id is %s, expected %s\n",
			sku_id, sku_info.signature_id, expected_sig_id);
		goto err;
	}
pass:
	fprintf(stdout, "-- passed\n\n");

	return 0;
err:
	fprintf(stdout, "\n");
	return -1;
}

int main(int argc, char **argv)
{
	extern char __dtb_test_config_begin[];
	char *fdt = __dtb_test_config_begin;
	const char *names = "Reef,Pyro,Snappy,Sand,Coral,Bad";
	int ret;

	mosys_log_init("stderr", LOG_SPEW, stderr);

	/* Single SKU */
	ret = do_test(fdt, names, "Pyro", 0, "", "pyro", "ABCE", "Pyro", NULL);
	ret |= do_test(fdt, names, "Snappy", 0, "", "snappy", "ABCF", "Snappy",
		       NULL);
	ret |= do_test(fdt, names, "Sand", 0, "", "sand", "ABCH", "Sand", NULL);

	/* SMBIOS plus SKU ID lookup */
	ret = do_test(fdt, names, "Coral", 0, "", "astronaut", NULL, "Coral",
		      NULL);
	ret |= do_test(fdt, names, "Coral", 61, "", "astronaut", "WXYZ",
		       "Coral", NULL);
	ret |= do_test(fdt, names, "Coral", 62, "", "astronaut", "ABCD",
		       "Coral", NULL);
	ret |= do_test(fdt, names, "Coral", 160, "", "nasher", "CPPT", "Coral",
		       NULL);
	ret |= do_test(fdt, names, "Coral", 163, "", "nasher360", "INUT",
		       "Coral", NULL);

	/* Different SMBIOS name */
	ret |= do_test(fdt, names, "Reef", 0, "", "basking", "ABCG", "Reef",
		       NULL);

	ret |= do_test(fdt, names, "Reef", 4, "", "reef", "ABCA", "Reef", NULL);
	ret |= do_test(fdt, names, "Reef", 5, "", "reef", "ABCA", "Reef", NULL);
	ret |= do_test(fdt, names, "Reef", 8, "", "electro", "ABCI", "Reef",
		       NULL);

	/* white label should provide correct brand code and signature ID */
	ret |= do_test(fdt, names, "Coral", 82, "", "whitetip2", "SHAQ",
		       "Coral", NULL);
	ret |= do_test(fdt, names, "Coral", 78, "", "whitetip", NULL, "Coral",
		       NULL);
	ret |= do_test(fdt, names, "Coral", 78, "whitetip1", "whitetip1",
		       "SHAR", "Coral", NULL);
	ret |= do_test(fdt, names, "Coral", 78, "whitetip2", "whitetip2",
		       "SHAQ", "Coral", NULL);

	/*
	 * For blacktip (alternate schema) if we don't provide the whitelabel
	 * name then we get no brand code or signature ID.
	 */
	ret |= do_test(fdt, names, "Coral", 83, "", "blacktip", NULL, "Coral",
		       NULL);
	ret |= do_test(fdt, names, "Coral", 84, "", "blacktip", NULL, "Coral",
		       NULL);
	ret |= do_test(fdt, names, "Coral", 85, "", "blacktip", NULL, "Coral",
		       NULL);

	/*
	 * But when we do provide the whitelabel name (blacktip1, 2) we do get
	 * a brand code and signature ID. Of course the signature ID matches
	 * the whitelabel name, since they came from the same place. But this
	 * provides some sort of validation, since the whitelabel name comes
	 * from VPD.
	 */
	ret |= do_test(fdt, names, "Coral", 83, "", "blacktip", NULL, "Coral",
		       NULL);
	ret |= do_test(fdt, names, "Coral", 84, "blacktip1", "blacktip", "HBBN",
		       "Coral", "blacktip1");
	ret |= do_test(fdt, names, "Coral", 85, "blacktip2", "blacktip", "HBBO",
		       "Coral", "blacktip2");

	/* Without an SMBIOS name we should fail */
	ret |= do_test(fdt, names, NULL, 61, "", NULL, NULL, NULL, NULL);

	/* Invalid SKU ID */
	ret |= do_test(fdt, names, "Coral", 255, "", NULL, NULL, NULL, NULL);

	/* Invalid platform name */
	ret |= do_test(fdt, names, "Bad", 0, "", "sand", "ABCH", "unknown",
		       NULL);

	/* Platform name that we don't support in the config */
	ret |= do_test(fdt, "wibble", "Pyro", 0, "", NULL, NULL, NULL, NULL);

	/* Empty platform name supports anything */
	ret |= do_test(fdt, "Reef,Snappy,Sand,Bad,,", "Pyro", 0, "", "pyro",
		       "ABCE", "Pyro", NULL);

	if (ret) {
		fprintf(stdout, "Simple tests failed\n");
		return 1;
	}

	fprintf(stdout, "Simple tests passed\n");

	return 0;
}
