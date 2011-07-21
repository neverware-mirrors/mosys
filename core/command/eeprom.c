/*
 * Copyright (C) 2010 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>

#include "mosys/platform.h"
#include "mosys/alloc.h"
#include "mosys/log.h"
#include "mosys/output.h"
#include "mosys/kv_pair.h"

#include "lib/crypto.h"
#include "lib/eeprom.h"
#include "lib/file.h"
#include "lib/fmap.h"
#include "lib/string.h"
#include "lib/string_builder.h"
#include "lib/valstr.h"

static int eeprom_enet_info_cmd(struct platform_intf *intf,
                                struct platform_cmd *cmd, int argc, char **argv)
{
	if (!intf->cb->eeprom ||
	    !intf->cb->eeprom->enet ||
	    !intf->cb->eeprom->enet->read)
		return -1;
	
	return intf->cb->eeprom->enet->read(intf, argc, argv);
}

static int eeprom_list_cmd(struct platform_intf *intf,
                           struct platform_cmd *cmd, int argc, char **argv)
{
	struct eeprom *eeprom;
	const struct valstr flag_lut[] = {
		/* FIXME: this should go in a lib */
		{ 1 << EEPROM_RD, "read" },
		{ 1 << EEPROM_WR, "write" },
		{ 1 << EEPROM_VERBOSE_ONLY, "verbose" },
		{ 0, NULL },
	};
	int i;

	if (!intf->cb->eeprom || !intf->cb->eeprom->eeprom_list)
		return -1;

	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		struct kv_pair *kv = kv_pair_new();
		unsigned int flags = eeprom->flags;
		struct string_builder *str;

		kv_pair_add(kv, "name", eeprom->name);
		/* FIXME: should this go one level deeper? */
		if (eeprom->device) {
			size_t size = eeprom->device->size(intf, eeprom);

			kv_pair_fmt(kv, "size", "%u", size);
			kv_pair_add(kv, "units", "bytes");
		}

		if (!flags) {
			kv_pair_add(kv, "flags", "");

			kv_pair_print(kv);
			kv_pair_free(kv);
			continue;
		}

		str = new_string_builder();
		for (i = 0; i < sizeof(eeprom->flags) * CHAR_BIT; i++) {
			if (eeprom->flags & (1 << i)) {
				const char *tmp = val2str(1 << i, flag_lut);

				string_builder_strncat(str, tmp, strlen(tmp));
				flags &= ~(1 << i);
				if (flags)
					string_builder_add_char(str, ',');
			}
		}
		kv_pair_add(kv, "flags", string_builder_get_string(str));

		kv_pair_print(kv);
		kv_pair_free(kv);
		free_string_builder(str);
	}

	return 0;
}

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
 * returns 1 to indicate success, fmap area found
 * returns 0 to indicate success, no fmap area found
 * returns <0 to indicate failure (e.g. file error) 
 */
static int eeprom_map_cmd_file(struct platform_intf *intf, char *filename)
{
	struct stat s;
	uint8_t *blob;
	off_t offset;
	int rc = -1, fd;

	if ((fd = file_open(filename, FILE_READ)) < 0)
		return -1;

	if (fstat(fd, &s) < 0) {
		lprintf(LOG_ERR, "cannot stat \"%s\"\n", filename);
		goto eeprom_map_cmd_file_exit_1;
	}

	blob = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (blob == MAP_FAILED) {
		lprintf(LOG_ERR, "unable to mmap \"%s\"\n", filename);
		goto eeprom_map_cmd_file_exit_1;
	}

	if ((offset = fmap_find(blob, s.st_size)) < 0) {
		lprintf(LOG_ERR, "unable to find fmap\n");
		goto eeprom_map_cmd_file_exit_2;
	}

	if (print_fmap_areas(filename, (struct fmap *)(blob + offset)) >= 0)
		rc = 1;

eeprom_map_cmd_file_exit_2:
	munmap(blob, s.st_size);
eeprom_map_cmd_file_exit_1:
	close(fd);
	return rc;
}

/*
 * returns 1 to indicate success, fmap area found
 * returns 0 to indicate success, no fmap area found
 * returns <0 to indicate failure (e.g. eeprom not found)
 */
static int eeprom_map_cmd_eeprom(struct platform_intf *intf, char *name)
{
	int rc = -1;
	struct eeprom *eeprom;

	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		struct fmap *fmap = NULL;

		if (name && strcmp(eeprom->name, name))
			continue;

		if (!eeprom->device || !eeprom->device->get_map)
			continue;

		rc = 0;		/* eeprom found; update again if fmap found */
		fmap = eeprom->device->get_map(intf, eeprom);
		if (!fmap)
			continue;
		else
			if (print_fmap_areas(eeprom->name, fmap) >= 0)
				rc = 1;

		free(fmap);
	}

	return rc;
}

static int eeprom_map_cmd(struct platform_intf *intf,
                          struct platform_cmd *cmd, int argc, char **argv)
{
	int rc = 0;

	if (argc) {
		if (argc != 1) {
			platform_cmd_usage(cmd);
			rc = -1;
			goto eeprom_map_cmd_exit;
		}

		lprintf(LOG_DEBUG, "%s: attempting to read fmap from file\n",
		                   __func__);
		if (eeprom_map_cmd_file(intf, argv[0]) == 1)
			goto eeprom_map_cmd_exit;
	}

	lprintf(LOG_DEBUG, "%s: attempting to read fmap from eeprom\n",
	                   __func__);
	if (!intf->cb->eeprom)
		return -1;
	rc = eeprom_map_cmd_eeprom(intf, argv[0]);

eeprom_map_cmd_exit:
	if (rc < 0)
		lprintf(LOG_ERR, "could not read flash map\n", __func__);
	return rc;
}

static int eeprom_csum_cmd(struct platform_intf *intf,
                           struct platform_cmd *cmd, int argc, char **argv)
{
	struct eeprom *eeprom;
	struct crypto_algo *crypto = &sha1_algo;
	uint8_t *digest = NULL;
	char *name;
	int fd = 0;
	int rc = 0;

	if (!intf->cb->eeprom || !intf->cb->eeprom->eeprom_list)
		return -1;

	if (argc)
		name = argv[0];

	if ((fd = file_open(name, FILE_READ)) >= 0) {
		struct stat s;
		uint8_t *blob;
		char *digest_str;
		struct kv_pair *kv;

		if (fstat(fd, &s) < 0) {
			lprintf(LOG_ERR, "cannot stat \"%s\"\n", name);
			rc = -1;
			goto eeprom_csum_cmd_exit;
		}

		blob = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (blob == MAP_FAILED) {
			lprintf(LOG_ERR, "unable to mmap \"%s\"\n", name);
			rc = -1;
			close(fd);
			goto eeprom_csum_cmd_exit;
		}

		if (fmap_get_csum(blob, s.st_size, &digest, crypto) < 0) {
			lprintf(LOG_DEBUG, "fmap_get_csum failed, checksumming "
			                   "entire image\n");

			crypto->init(crypto->ctx);
			crypto->update(crypto->ctx, blob, s.st_size);
			crypto->final(crypto->ctx);
			digest = crypto->get_digest(crypto);
		}

		digest_str = buf2str(digest, crypto->digest_len);

		kv = kv_pair_new();
		kv_pair_fmt(kv, "name", name);
		kv_pair_fmt(kv, "checksum", digest_str);
		kv_pair_print(kv);

		kv_pair_free(kv);
		free(digest_str);
		munmap(blob, s.st_size);
	}

	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		uint8_t *image = NULL;
		int len;
		char *digest_str;
		struct kv_pair *kv;

		if (name && strcmp(eeprom->name, name))
			continue;

		if (!eeprom->device || !eeprom->device->size ||
		    !eeprom->device->read)
			continue;

		if ((len = eeprom->device->size(intf, eeprom)) < 0) {
			lprintf(LOG_DEBUG, "failed to obtain size of %s\n",
			                   eeprom->name);
			continue;
		}
		image = mosys_malloc(len);
		if (eeprom->device->read(intf, eeprom, 0, len, image) < 0) {
			lprintf(LOG_DEBUG, "failed to read %s\n", eeprom->name);
			continue;
		}

		if (fmap_get_csum(image, len, &digest, crypto) < 0) {
			lprintf(LOG_DEBUG, "fmap_get_csum failed, checksumming "
			                   "entire image\n");
			                   
			crypto->init(crypto->ctx);
			crypto->update(crypto->ctx, image, len);
			crypto->final(crypto->ctx);
			digest = crypto->get_digest(crypto);
		}

		digest_str = buf2str(digest, crypto->digest_len);

		kv = kv_pair_new();
		kv_pair_fmt(kv, "name", eeprom->name);
		kv_pair_fmt(kv, "checksum", digest_str);
		kv_pair_print(kv);

		kv_pair_free(kv);
		free(digest_str);
		free(image);
	}

eeprom_csum_cmd_exit:
	return rc;
}

static int eeprom_dump_cmd(struct platform_intf *intf,
                           struct platform_cmd *cmd, int argc, char **argv)
{
	struct eeprom *eeprom;
	int rc = 0, fd = -1;
	struct stat st;
	uint8_t *buf = NULL;
	const char *devname, *filename;
	size_t eeprom_size;

	if ((argc < 1) || (argc > 2)) {
		platform_cmd_usage(cmd);
		return -1;
	}
	devname = argv[0];
	if (argc == 2)
		filename = argv[1];
	else
		filename = NULL;

	if (!intf->cb->eeprom || !intf->cb->eeprom->eeprom_list)
		return -1;

	/* find the eeprom to do work on */
	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		if (!strcmp(eeprom->name, devname))
			break;
	}
	if (!eeprom->name) {
		lprintf(LOG_ERR, "eeprom %s not found\n", devname);
		return -1;
	}
	if (!eeprom->device->read)
		return -ENOSYS;

	if (filename != NULL) {
		unsigned int filemode = S_IRUSR | S_IWUSR | S_IRGRP;

		/* Do not overwrite an existing file */
		if (lstat(filename, &st) == 0) {
			lprintf(LOG_ERR, "File %s already exists\n", filename);
			return -1;
		}
		if ((fd = open(filename, O_CREAT | O_WRONLY, filemode)) < 0) {
			lperror(LOG_ERR, "Could not open file %s", filename);
			return -1;
		}
	}

	/* do the actual work - read from eeprom, print to screen or
	 * write to file */
	eeprom_size = eeprom->device->size(intf, eeprom);
	buf = mosys_malloc(eeprom_size);
	if (eeprom->device->read(intf, eeprom, 0, eeprom_size, buf) < 0) {
		lprintf(LOG_ERR, "Unable to read %d bytes from %s\n",
				  eeprom_size, eeprom->name);
		rc = -1;
		goto eeprom_dump_done;
	}

	if (filename != NULL) {
		int count;

		count = write(fd, buf, eeprom_size);
		if (count != eeprom_size) {
			lprintf(LOG_ERR, "Unable to write %d bytes to %s\n",
					 eeprom_size, filename);
			rc = -1;
		}
	} else {
		print_buffer(buf, eeprom_size);
	}

eeprom_dump_done:
	close(fd);
	free(buf);
	return rc;
}

static int eeprom_write_cmd(struct platform_intf *intf,
                            struct platform_cmd *cmd, int argc, char **argv)
{
	struct eeprom *eeprom;
	int rc = 0, fd;
	struct stat st;
	uint8_t *buf = NULL;
	const char *devname, *filename;
	size_t eeprom_size;

	if (argc != 2) {
		platform_cmd_usage(cmd);
		return -1;
	}
	devname = argv[0];
	filename = argv[1];

	if (!intf->cb->eeprom || !intf->cb->eeprom->eeprom_list)
		return -1;

	/* find the eeprom to do work on */
	for (eeprom = intf->cb->eeprom->eeprom_list;
	     eeprom && eeprom->name;
	     eeprom++) {
		if (!strcmp(eeprom->name, devname))
			break;
	}
	if (!eeprom->name) {
		lprintf(LOG_ERR, "eeprom %s not found\n", devname);
		return -1;
	}
	if (!eeprom->device->write)
		return -ENOSYS;

	/* size sanity checks */
	eeprom_size = eeprom->device->size(intf, eeprom);
	if (lstat(filename, &st) < 0) {
		lperror(LOG_ERR, "lstat failure on file %s", filename);
		return -1;
	}
	if (st.st_size > eeprom_size) {
		lprintf(LOG_ERR, "cannot write %u bytes to %s eeprom "
			         "(%u bytes)\n", st.st_size, eeprom->name,
		                  eeprom_size);
		return -1;
	}

	if ((fd = open(filename, O_RDONLY)) < 0) {
		lperror(LOG_ERR, "Could not open file %s", filename);
		return -1;
	}

	/* do the actual work - read from file, write to eeprom */
	buf = mosys_malloc(st.st_size);
	if (read(fd, buf, st.st_size) != st.st_size) {
		lperror(LOG_ERR, "Could not read file %s", filename);
		rc = -1;
		goto eeprom_write_done;
	}

	if (eeprom->device->write(intf, eeprom, 0,
				  st.st_size, buf) != st.st_size) {
		lprintf(LOG_ERR, "Unable to write %u bytes to %s\n",
				  st.st_size, eeprom->name);
		rc = -1;
	} else {
		mosys_printf("Wrote image to %s\n", eeprom->name);
	}

eeprom_write_done:
	close(fd);
	free(buf);
	return rc;
}

struct platform_cmd eeprom_enet_cmds[] = {
	{
		.name	= "info",
		.desc	= "Print information from ethernet controller EEPROM(s)",
		.usage	= "mosys eeprom enet read [ethN]",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_enet_info_cmd }
	},
#if 0
	{
		.name	= "write",
		.desc	= "Write to ethernet controller EEPROM(s)",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_enet_write_cmd }
	},
#endif
	{ NULL }
};

struct platform_cmd eeprom_cmds[] = {
	{
		.name	= "list",
		.desc	= "List EEPROMs present in system and some basic info",
		.usage	= "mosys eeprom list",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_list_cmd }
	},
	{
		.name	= "map",
		.desc	= "Print EEPROM maps if present",
		.usage	= "mosys eeprom map <eeprom/filename>",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_map_cmd }
	},
	{
		.name	= "csum",
		.desc	= "Print sha1 checksum",
		.usage	= "mosys eeprom csum <eeprom/filename>",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_csum_cmd }
	},
	{
		.name	= "dump",
		.desc	= "Dump entire contents of EEPROM to file",
		.usage	= "mosys eeprom dump <device> <file>",
		.type	= ARG_TYPE_GETTER,
		.arg	= { .func = eeprom_dump_cmd }
	},
	{
		.name	= "write",
		.desc	= "Write contents of file to EEPROM",
		.usage	= "mosys eeprom write <device> <file>",
		.type	= ARG_TYPE_SETTER,
		.arg	= { .func = eeprom_write_cmd }
	},
	{
		.name	= "enet",
		.desc	= "Ethernet Commands",
		.type	= ARG_TYPE_SUB,
		.arg	= { .sub = eeprom_enet_cmds }
	},
	{ NULL }
};

struct platform_cmd cmd_eeprom = {
	.name	= "eeprom",
	.desc	= "EEPROM Information",
	.type	= ARG_TYPE_SUB,
	.arg	= { .sub = eeprom_cmds }
};
