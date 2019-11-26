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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>

#include <fmap.h>
#include <valstr.h>

#include "mosys/platform.h"
#include "mosys/log.h"
#include "mosys/kv_pair.h"

#include "lib/eeprom.h"
#include "lib/file.h"
#include "lib/string_builder.h"

/* helper function for printing fmap area information */
static int print_fmap_areas(const char *name, struct fmap *fmap)
{
	int i;

	for (i = 0; i < fmap->nareas; i++) {
		struct kv_pair *kv = kv_pair_new();
		const char *str = NULL;

		kv_pair_fmt(kv, "name", "%s", name);
		kv_pair_fmt(kv, "area_name", "%s", fmap->areas[i].name);
		kv_pair_fmt(kv, "area_offset", "0x%08x", fmap->areas[i].offset);
		kv_pair_fmt(kv, "area_size", "0x%08x", fmap->areas[i].size);

		str = fmap_flags_to_string(fmap->areas[i].flags);
		if (str == NULL)
			return -1;
		kv_pair_fmt(kv, "area_flags", "%s", str);

		kv_pair_print(kv);
		kv_pair_free(kv);
		free((void *)str);
	}

	return 0;
}

/*
 * eeprom_map_cmd_file - helper for printing fmap from ROM image
 *
 * @intf:	platform interface
 * @filename:	name of file (ROM image)
 *
 * returns 0 to indicate success
 * returns <0 to indicate failure (e.g. file error) 
 */
static int eeprom_map_cmd_file(struct platform_intf *intf, char *filename)
{
	struct stat s;
	uint8_t *blob;
	off_t offset;
	int rc = -1, errsv = 0, fd;

	if ((fd = file_open(filename, FILE_READ)) < 0)
		return -1;

	if (fstat(fd, &s) < 0) {
		errsv = errno;
		lprintf(LOG_ERR, "cannot stat \"%s\"\n", filename);
		goto eeprom_map_cmd_file_exit_1;
	}

	blob = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (blob == MAP_FAILED) {
		errsv = errno;
		lprintf(LOG_ERR, "unable to mmap \"%s\"\n", filename);
		goto eeprom_map_cmd_file_exit_1;
	}

	if ((offset = fmap_find(blob, s.st_size)) < 0) {
		lprintf(LOG_ERR, "unable to find fmap\n");
		goto eeprom_map_cmd_file_exit_2;
	}

	rc = print_fmap_areas(filename, (struct fmap *)(blob + offset));

eeprom_map_cmd_file_exit_2:
	munmap(blob, s.st_size);
eeprom_map_cmd_file_exit_1:
	close(fd);
	errno = errsv;
	return rc;
}

/*
 * eeprom_map_cmd_eeprom - helper for printing fmap of ROMs in the system
 *
 * @intf:	platform interface
 * @name:	name of ROM/EEPROM for which to print fmap (optional)
 *
 * returns 0 to indicate success
 * returns <0 to indicate failure (e.g. EEPROM not found)
 */
static int eeprom_map_cmd_eeprom(struct platform_intf *intf, char *name)
{
	int rc = 0, eeprom_found = 0;
	struct eeprom *eeprom;

	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		struct fmap *fmap = NULL;

		if (name) {
			if (!strcmp(eeprom->name, name))
				eeprom_found = 1;
			else
				continue;
		}

		if (!eeprom->device || !eeprom->device->get_map)
			continue;

		fmap = eeprom->device->get_map(intf, eeprom);
		if (!fmap)
			continue;

		/* let caller handle errors, allow loop to continue */
		rc |= print_fmap_areas(eeprom->name, fmap);

		free(fmap);
	}

	if (name && !eeprom_found) {
		errno = ENODEV;
		rc = -1;
	}
	return rc;
}

static int eeprom_map_cmd(struct platform_intf *intf,
                          struct platform_cmd *cmd, int argc, char **argv)
{
	int rc = -1, errsv = 0;
	char *name = argv[0];
	struct stat s;

	/* The simple case: No argument, just print FMAPs from all EEPROMS */
	if (!argc) {
		rc = eeprom_map_cmd_eeprom(intf, NULL);
		goto eeprom_map_cmd_exit;
	} else if (argc != 1) {
		platform_cmd_usage(cmd);
		errsv = EINVAL;
		goto eeprom_map_cmd_exit;
	}

	/*
	 * The argument can be either a filename or an EEPROM device name.
	 * Try using it as a filename first since files are generic. If that
	 * fails, the argument must be a EEPROM present on the machine.
	 */

	if (stat(name, &s) == 0) {
		rc = eeprom_map_cmd_file(intf, name);
		errsv = errno;
		if (rc < 0) {
			lperror(LOG_ERR, "Failed to read flashmap from file "
				"\"%s\"", name);
		}
		goto eeprom_map_cmd_exit;
	}

	if (intf->cb->eeprom) {
		rc = eeprom_map_cmd_eeprom(intf, name);
		errsv = errno;
		if (rc < 0) {
			lperror(LOG_ERR, "Failed to read flashmap from device "
				"\"%s\"", name);
		}
	} else {
		errsv = ENOSYS;
	}

eeprom_map_cmd_exit:
	if (rc < 0)
		lprintf(LOG_ERR, "could not read flash map\n");
	errno = errsv;
	return rc;
}

#define EEPROM_DEPRECATED " (deprecated, use flashrom or dump_fmap instead)"

struct platform_cmd eeprom_cmds[] = {
	{
		.name	= "map",
		.desc	= "Print EEPROM maps if present" EEPROM_DEPRECATED,
		.usage	= "mosys eeprom map <eeprom/filename>",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_map_cmd }
	},
	{ NULL }
};

struct platform_cmd cmd_eeprom = {
	.name	= "eeprom",
	.desc	= "EEPROM Information" EEPROM_DEPRECATED,
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = eeprom_cmds }
};
