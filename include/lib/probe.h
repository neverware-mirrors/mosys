/*
 * Copyright 2012, Google Inc.
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

#ifndef MOSYS_LIB_PROBE_H__
#define MOSYS_LIB_PROBE_H__

struct platform_intf;

/*
 * probe_frid - attempt to match platform to chromeos firmware revision id
 *
 * @frids:	null-terminated list of firmware revision IDs
 *
 * returns 1 to indicate match
 * returns 0 to indicate no match
 * returns <0 to indicate error
 */
extern int probe_frid(const char *frids[]);

/*
 * probe_smbios - probe smbios for system info
 *
 * @ids:	null-terminated list of ids
 *
 * returns 1 to indicate match
 * returns 0 to indicate no match
 * returns <0 to indicate error
 */
extern int probe_smbios(struct platform_intf *intf, const char *ids[]);

/*
 * probe_fdt_compatible - Probe platform using device tree "compatible" node
 *
 * @id_list:		Known platform IDs to compare with
 * @num_ids:		Number of known platform IDs
 * @allow_partial:	Allow partial match (0=no, 1=yes)
 *
 * returns the index of the platform ID if found
 * returns <0 to indicate error
 */
extern int probe_fdt_compatible(const char * const id_list[],
				int num_ids, int allow_partial);

#endif /* MOSYS_LIB_PROBE_H__ */
