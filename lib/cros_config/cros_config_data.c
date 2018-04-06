#include "lib/cros_config_struct.h"

const struct config_map all_configs[] = {
    {.platform_name = "NameOnly",
     .smbios_match_name = "NameOnly",
     .sku_id = -1,
     .customization_id = "",
     .info = {.brand = "AAAA",
	      .model = "NameOnly",
	      .customization = "NameOnly"}},
    {.platform_name = "NameSku",
     .smbios_match_name = "NameSku",
     .sku_id = 1,
     .customization_id = "",
     .info = {.brand = "BBBB", .model = "NameSku", .customization = "NameSku"}},
    {.platform_name = "NameSkuCustomization",
     .smbios_match_name = "NameSkuCustomization",
     .sku_id = 1,
     .customization_id = "NameSkuCustomization",
     .info = {.brand = "CCCC",
	      .model = "NameSkuCustomization",
	      .customization = "NameSkuCustomization"}},
};

struct config_map **cros_config_get_config_map() { return &all_configs; }
