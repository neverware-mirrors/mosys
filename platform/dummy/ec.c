// Copyright 2018 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mosys/log.h"
#include "mosys/platform.h"

#include "drivers/google/cros_ec.h"
#include "drivers/google/cros_ec_commands.h"

#define DUMMY_EC_BUS           1
#define DUMMY_EC_ADDRESS       0x1e

static int dummy_cros_ec_cmd(struct platform_intf *intf,
                             struct ec_cb *ec,
                             int command, int command_version,
                             const void *indata, int insize,
                             const void *outdata, int outsize) {
        uint32_t* in;
        uint32_t* out;
        switch (command) {
                case EC_CMD_GET_VERSION:
                        if (indata) {
                                in = (uint32_t*) indata;
                                char roversion[32] = "roversion";
                                char rwversion[32] = "rwversion";
                                uint32_t type = 1;
                                memcpy(in, roversion, sizeof(roversion));
                                memcpy(in+8, rwversion, sizeof(rwversion));
                                // RO
                                memcpy(in+24, &type, sizeof(type));
                        }
                        if (outdata) {
                                out = (uint32_t*) outdata;
                                char roversion[32] = "roversion";
                                char rwversion[32] = "rwversion";
                                uint32_t type = 1;
                                memcpy(out, roversion, sizeof(roversion));
                                memcpy(out+8, rwversion, sizeof(rwversion));
                                // RO
                                memcpy(out+24, &type, sizeof(type));
                        }
                        break;
                case EC_CMD_GET_CHIP_INFO:
                        if (indata) {
                                in = (uint32_t*) indata;
                                char vendor[32] = "vendor";
                                char name[32] = "name";
                                char revision[32] = "revision";
                                memcpy(in, vendor, sizeof(vendor));
                                memcpy(in+8, name, sizeof(name));
                                memcpy(in+16, revision, sizeof(revision));
                        }
                        if (outdata) {
                                out = (uint32_t*) outdata;
                                char vendor[32] = "vendor";
                                char name[32] = "name";
                                char revision[32] = "revision";
                                memcpy(out, vendor, sizeof(vendor));
                                memcpy(out+8, name, sizeof(name));
                                memcpy(out+16, revision, sizeof(revision));
                        }
        }
        return 0;
};


static struct cros_ec_dev dev = {
        .name   = CROS_EC_DEV_NAME,
};

static struct i2c_addr i2c_addr = {
        .bus    = DUMMY_EC_BUS,                /* may be overridden */
        .addr   = DUMMY_EC_ADDRESS,
};

static struct cros_ec_priv dummy_ec_priv = {
        .devfs  = &dev,
        .i2c = &i2c_addr,
        .cmd = &dummy_cros_ec_cmd,
};

int dummy_ec_setup(struct platform_intf *intf)
{
        MOSYS_CHECK(intf->cb && intf->cb->ec);
        intf->cb->ec->priv = &dummy_ec_priv;

        return 1;
}
