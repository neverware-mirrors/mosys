/* Copyright 2012, Google Inc.
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
 *
 * intf_list.c: hardware component interfaces
 */

#include "mosys/intf_list.h"
#include "mosys/platform.h"

#include "intf/i2c.h"
#include "intf/pci.h"
#include "intf/mmio.h"

/* common operations */
struct platform_op platform_common_op = {
	.pci		= &pci_file_intf,
	.i2c		= &i2c_dev_intf,
	.mmio		= &mmio_mmap_intf,
};

/*
 * intf_op_setup  -  prepare interface operations
 *
 * @intf:       platform interface
 *
 * returns aggregate return code from setup functions
 */
int intf_op_setup(struct platform_intf *intf)
{
	int rc = 0;

	if (intf->op->pci && intf->op->pci->setup)
		rc |= intf->op->pci->setup(intf);
	if (intf->op->i2c && intf->op->i2c->setup)
		rc |= intf->op->i2c->setup(intf);
	if (intf->op->mmio && intf->op->mmio->setup)
		rc |= intf->op->mmio->setup(intf);

	return rc;
}

/*
 * intf_op_destroy  -  cleanup interface operations
 *
 * @intf:       platform interface
 */
void intf_op_destroy(struct platform_intf *intf)
{
	if (intf->op->pci && intf->op->pci->destroy)
		intf->op->pci->destroy(intf);
	if (intf->op->i2c && intf->op->i2c->destroy)
		intf->op->i2c->destroy(intf);
	if (intf->op->mmio && intf->op->mmio->destroy)
		intf->op->mmio->destroy(intf);
}
