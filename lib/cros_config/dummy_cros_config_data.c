#include "lib/cros_config_struct.h"

static struct config_map all_configs[] = {
    {.platform_name = "Dummy",
     .smbios_match_name = "Dummy",
     .sku_id = 8,
     .customization_id = "",
     .whitelabel_tag = "dummy_whitelabel",
     .info = {.brand = "DUMM",
              .model = "dummy",
              .customization = "dummy",
              .signature_id = "dummy"}},

};

const struct config_map *cros_config_get_config_map(int *num_entries) {
  *num_entries = 1;
  return &all_configs[0];
}
