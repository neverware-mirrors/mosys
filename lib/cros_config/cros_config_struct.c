#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mosys/log.h"
#include "mosys/platform.h"

#include "lib/cros_config.h"
#include "lib/cros_config_struct.h"
#include "lib/math.h"
#include "lib/probe.h"
#include "lib/sku.h"
#include "lib/smbios.h"

#ifdef CONFIG_PLATFORM_ARCH_X86
static const char *str_or_null(const char *in_str)
{
	return in_str ?: "(null)";
}

int cros_config_read_sku_info_struct(struct platform_intf *intf,
				     const char *smbios_name, int sku_id,
				     struct sku_info *sku_info)
{
	const char *whitelabel_tag = sku_get_whitelabel_from_vpd();
	const char *customization_id = sku_get_vpd_customization(intf);
	if (!customization_id)
		customization_id = "";

	int config_map_size = 0;
	const struct config_map *configs =
	    cros_config_get_config_map(&config_map_size);

	lprintf(LOG_DEBUG,
		"%s: Checking config: smbios_name='%s', sku_id=%d, "
		"whitelabel_tag='%s', customization_id='%s'\n",
		__func__, str_or_null(smbios_name), sku_id,
		str_or_null(whitelabel_tag), str_or_null(customization_id));
	for (int i = 0; i < config_map_size; i++) {
		const struct config_map *config = &configs[i];

		lprintf(LOG_DEBUG,
			"   item %d: smbios_match_name='%s', sku_id=%d, "
			"whitelabel_tag='%s', customization_id='%s'\n",
			i, config->smbios_match_name, config->sku_id,
			config->whitelabel_tag, config->customization_id);
		bool smbios_match =
		    !*config->smbios_match_name ||
		    !strcmp(smbios_name, config->smbios_match_name);
		bool sku_match =
		    (-1 == config->sku_id) || (sku_id == config->sku_id);
		bool whitelabel_match =
		    !*config->whitelabel_tag ||
		    !strcmp(whitelabel_tag, config->whitelabel_tag);
		bool customization_match =
		    !*config->customization_id ||
		    !strcmp(customization_id, config->customization_id);
		if (smbios_match && sku_match && customization_match &&
		    whitelabel_match) {
			intf->name = config->platform_name;
			memcpy(sku_info, &config->info, sizeof(*sku_info));
			lprintf(
			    LOG_DEBUG,
			    "   - Found, name='%s', model='%s', brand='%s'\n",
			    str_or_null(intf->name),
			    str_or_null(sku_info->model),
			    str_or_null(sku_info->brand));

			return 0;
		}
	}
	lprintf(LOG_DEBUG, "   - Not found\n");

	return -1;
}
#endif // CONFIG_PLATFORM_ARCH_X86

#ifdef CONFIG_PLATFORM_ARCH_ARMEL
int cros_config_read_sku_info_struct(struct platform_intf *intf,
				     struct sku_info *sku_info)
{
	const char *whitelabel_tag = sku_get_whitelabel_from_vpd();
	const char *customization_id = sku_get_vpd_customization(intf);
	if (!customization_id)
		customization_id = "";

	int config_map_size = 0;
	const struct config_map *configs =
	    cros_config_get_config_map(&config_map_size);
	for (int i = 0; i < config_map_size; i++) {
		const struct config_map *config = &configs[i];
		bool device_match = probe_fdt_compatible(
			&config->device_tree_compatible_match, 1, 1) == 0;
		bool whitelabel_match =
		    !strcmp("", config->whitelabel_tag) ||
		    !strcmp(whitelabel_tag, config->whitelabel_tag);
		bool customization_match =
		    !strcmp("", config->customization_id) ||
		    !strcmp(customization_id, config->customization_id);
		if (device_match && customization_match && whitelabel_match) {
			intf->name = config->platform_name;
			memcpy(sku_info, &config->info, sizeof(*sku_info));
			return 0;
		}
	}

	return -1;
}
#endif // CONFIG_PLATFORM_ARCH_ARMEL
