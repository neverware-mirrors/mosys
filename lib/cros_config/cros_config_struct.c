#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/cros_config_struct.h"
#include "lib/math.h"
#include "lib/sku.h"
#include "lib/smbios.h"

int cros_config_read_sku_info_struct(struct platform_intf *intf,
				     const char *find_platform_names,
				     struct sku_info *sku_info)
{
	const char *smbios_name;
	int sku_id;

	smbios_name = smbios_sysinfo_get_name(intf);
	if (!smbios_name)
		lprintf(LOG_DEBUG, "%s: Unknown SMBIOS name\n", __func__);
	sku_id = smbios_sysinfo_get_sku_number(intf);
	if (sku_id == -1)
		lprintf(LOG_DEBUG, "%s: Unknown SKU ID\n", __func__);

	const char *customization_id = sku_get_vpd_customization(intf);
	if (customization_id == NULL) {
		customization_id = sku_get_whitelabel_from_vpd();
	}

	int config_map_size = 0;
	const struct config_map *configs =
	    cros_config_get_config_map(&config_map_size);
	for (int i = 0; i < config_map_size; i++) {
		const struct config_map *config = &configs[i];
		bool smbios_match =
		    strcmp("", config->smbios_match_name) == 0 ||
		    strcmp(smbios_name, config->smbios_match_name) == 0;
		bool sku_match =
		    (-1 == config->sku_id) || (sku_id == config->sku_id);
		bool customization_match =
		    strcmp("", config->customization_id) == 0 ||
		    strcmp(customization_id, config->customization_id) == 0;
		if (smbios_match && sku_match && customization_match) {
			intf->name = config->platform_name;
			memcpy(sku_info, &config->info, sizeof(*sku_info));
			return 0;
		}
	}

	return -1;
}
