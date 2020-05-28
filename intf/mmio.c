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
 *
 * mmio.c: Memory Map IO
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mosys/alloc.h"
#include "mosys/file_backed_range.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "intf/mmio.h"

#include "lib/string.h"

static inline struct file_backed_range *
mmio_ranges(struct platform_intf *intf)
{
	return intf->op->mmio->ranges;
}

/*
 * mmio_setup
 *
 * @intf:     platform interface
 *
 * returns 0 if mmio_ranges() is set to non-null
 * returns -1 if mmio_ranges() is NULL
 */
static int mmio_setup(struct platform_intf *intf)
{
	if (mmio_ranges(intf)) {
		return 0;
	}
	return -1;
}

/*
 * mmio_destroy
 *
 * @intf:     platform interface
 *
 */
static void mmio_destroy(struct platform_intf *intf)
{
	(void)intf;
	return;
}

/*
 * mmio_mmap -  map a piece of physical memory into the address space
 *
 * @intf:     platform interface
 * @flags:    permsissions (O_RDONLY, O_WRONLY, etc)
 * @address:  address of memory to map
 * @length:   length of data to map
 *
 * return pointer to mapping on success, NULL otherwise.
 */
static void *mmio_mmap(struct platform_intf *intf, int flags,
                       uint64_t address, unsigned int length)
{
	size_t moffset;
	void *mptr;
	int fd;
	struct file_backed_range *file_range;
	const char *file_name;

	/* FIXME: hack to get thru SMBIOS parsing in early platform setup */
	if (mmio_setup(intf) < 0) {
		return NULL;
	}

	lprintf(LOG_DEBUG, "mmio_mmap(0x%016jx, %d)\n", (uintmax_t)address,
		length);

	if (length == 0) {
		lprintf(LOG_DEBUG, "mmap_map: Nothing to do!\n");
		return NULL;
	}

	file_range = find_file_backed_range(address, length, mmio_ranges(intf));

	if (file_range == NULL) {
		lprintf(LOG_DEBUG, "Unable to find backing file for range.\n");
		return NULL;
	}

	file_name = file_range->file_name;
	fd = open(file_name, flags);
	if (fd < 0) {
		lprintf(LOG_ERR, "Failed to open file %s\n", file_name);
		return NULL;
	}

	/* adjust the address to reflect the base address that the start of
	 * the file represents. */
	address -= get_address_range_begin(&file_range->range);

	/* get alignment */
	moffset = address % getpagesize();

	mptr = mmap(0,	                /* result address (don't care) */
	            moffset + length,   /* total length (aligned) */
		    PROT_READ,          /* memory protection */
	            MAP_SHARED,	        /* flags */
	            fd,	                /* source file descriptor */
	            address - moffset);	/* offset into file */

	if (mptr == MAP_FAILED) {
		/* Add back the base address for a clearer error message. */
		address += get_address_range_begin(&file_range->range);
		lperror(LOG_ERR, "Failed to mmap range 0x%016jx-0x%016jx",
		        (uintmax_t)address, (uintmax_t)address + length);
		close(fd);
		return NULL;
	}
	close(fd);

	return mptr + moffset;
}

/*
 * mmio_munmap -  unmap a piece of physical memory into the address space
 *
 * @intf:     platform interface
 * @mptr:     pointer address is mapped into
 * @address:  address of memory to unmap
 * @length:   length of data to unmap
 *
 * returns <0 if failure, 0 on success
 */
static int mmio_munmap(struct platform_intf *intf, void *mptr,
                       uint64_t address, unsigned int length)
{
	size_t moffset;
	struct file_backed_range *file_range;

	if (mptr == NULL) {
		return -1;
	}

	file_range = find_file_backed_range(address, length, mmio_ranges(intf));

	if (file_range == NULL) {
		lprintf(LOG_DEBUG, "Unable to find backing file for range.\n");
		return -1;
	}

	/* adjust the address to reflect the base address that the start of
	 * the file represents. */
	address -= get_address_range_begin(&file_range->range);

	moffset = address % getpagesize();

	/* clean up */
	if (munmap(mptr - moffset, moffset + length) < 0) {
		address += get_address_range_begin(&file_range->range);
		lperror(LOG_ERR, "Failed to munmap range0x%016jx-0x%016jx",
		        (uintmax_t)address, (uintmax_t)address + length);
		return -1;
	}

	return 0;
}

/*
 * mmio_mmap_read  -  read from memory mapped address into buffer
 *
 * @intf:       platform interface
 * @address:    physical address to read from
 * @length:     length of data to read
 * @data:       buffer to save data into
 *
 * returns length of data read
 * returns <0 if failed to read
 */
static int mmio_mmap_read(struct platform_intf *intf,
                          uint64_t address, int length, void *data)
{
	char *mptr;

	/* Map in the requested address. */
	mptr = mmio_map(intf, O_RDONLY, address, length);

	if (mptr == NULL) {
		return -1;
	}

	/* copy into supplied buffer */
	memcpy(data, mptr, length);

	/* clean up */
	mmio_unmap(intf, mptr, address, length);

	return length;
}

#define MEM_DEV		"/dev/mem"

static struct file_backed_range mmio_file_ranges[] = {
	/* A size of 0 with a base address of 0 represents the entire physical
	 * address range. */
	FILE_BACKED_RANGE_INIT(0, 0, MEM_DEV),
	FILE_BACKED_RANGE_END,
};

/* Memory-mapped interface */
struct mmio_intf mmio_mmap_intf = {
	.ranges		= mmio_file_ranges,
	.setup		= mmio_setup,
	.destroy	= mmio_destroy,
	.read   	= mmio_mmap_read,
	.map    	= mmio_mmap,
	.unmap		= mmio_munmap,
};
