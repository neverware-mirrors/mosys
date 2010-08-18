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
 *
 * string_builder.c: String builder helper functions. See comments in
 * string_builder.h for more details.
 *
 * eventlog.c: SMBIOS event log access.
 */

#include <inttypes.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/platform.h"
#include "mosys/output.h"

#include "intf/mmio.h"

#include "lib/common.h"
#include "lib/smbios.h"
#include "lib/valstr.h"

struct smbios_eventlog_iterator {
	int verbose;
	int log_area_length;  /* length of log */
	int header_offset;    /* offset into log area to read the header. */
	int data_offset;      /* offset into log area of first data element */
	int current_offset;   /* current offset into log_area */
	uint8_t log_area[0];  /* log area */
};

/*
 * smbios_new_eventlog_iterator - obtain a new smbios_eventlog_iterator
 *                                for iterating through SMBIOS event log.
 *
 * @intf:        platform_intf used for filling initializing
 *               smbios_eventlog_iterator
 * @elog_table:  pointer to SMBIOS event log used to initialize a
 *               smbios_eventlog_iterator
 *
 * Return new smbios_eventlog_iterator based on smbios_table_log pointed to
 * by elog_table.
 */
struct smbios_eventlog_iterator *smbios_new_eventlog_iterator(
    struct platform_intf *intf, struct smbios_table_log *elog_table)
{
	struct smbios_eventlog_iterator *elog_iter;

	if (intf == NULL || elog_table == NULL)
		return NULL;

	/* Only support memory mapped I/O access */
	if (elog_table->method != SMBIOS_LOG_METHOD_TYPE_MEM)
		return NULL;

	/* Allocate and fill in iterator. */
	elog_iter = mosys_malloc(sizeof(*elog_iter) + elog_table->length);
	elog_iter->verbose = mosys_get_verbosity();
	elog_iter->log_area_length = elog_table->length;
	elog_iter->header_offset = elog_table->header_start;
	elog_iter->data_offset = elog_table->data_start;
	smbios_eventlog_iterator_reset(elog_iter);

	/* Copy the current log inforamtion in. */
	if (mmio_read(intf, elog_table->address.mem,
	              elog_table->length, &elog_iter->log_area[0]) < 0) {
		smbios_free_eventlog_iterator(elog_iter);
		return NULL;
	}

	if (mosys_get_verbosity() > 4)
		print_buffer(elog_iter->log_area, elog_iter->log_area_length);

	return elog_iter;
}

/*
 * smbios_free_eventlog_iterator - free the smbios_eventlog_iterator obtained
 *                                 from smbios_new_eventlog_iterator().
 *
 * @elog_iter:  smbios_eventlog_iterator to free
 */
void smbios_free_eventlog_iterator(struct smbios_eventlog_iterator *elog_iter)
{
	free(elog_iter);
}

/*
 * smbios_eventlog_iterator_reset - reset the iterator to point to "before" the
 *                                  first event.
 *
 * @elog_iter:   eventlog iterator to reset
 *
 * returns < 0 on error, 0 on success.
 */
int smbios_eventlog_iterator_reset(struct smbios_eventlog_iterator *elog_iter)
{
	if (elog_iter == NULL)
		return -1;

	elog_iter->current_offset = -1;

	return 0;
}

/*
 * smbios_eventlog_get_next_entry - retrieve next event log entry
 *
 * @elog_iter:   eventlog iterator used to obtain next eventlog entry
 *
 * returns NULL on error, else next event log entry pointed to by
 * smbios_eventlog_iterator. The smbios_log_entry's lifetime is dependent on
 * the life of elog_iter, that is if elog_iter is free()'d the smbios_log_entry
 * returned by this function is no longer valid.
 */
struct smbios_log_entry *smbios_eventlog_get_next_entry(
    struct smbios_eventlog_iterator *elog_iter)
{
	struct smbios_log_entry *entry;
	int next_offset;

	if (elog_iter == NULL)
		return NULL;

	if (elog_iter->current_offset < 0) {
		/* If current_offset is pointing "before" the first entry move
		 * to first entry. */
		next_offset = elog_iter->data_offset;
	} else {
		/* Point next_offset to the next entry. */
		entry =(void *)&elog_iter->log_area[elog_iter->current_offset];
		next_offset = elog_iter->current_offset + entry->length;
	}

	/* Cannot proceed past end of log area. */
	if (next_offset >= elog_iter->log_area_length)
		return NULL;

	/* Point to next potential entry */
	entry =(void *)&elog_iter->log_area[next_offset];

	/* Ensure the fields read do not exceed the log length. */
	if (next_offset + offsetof(typeof(*entry), length) +
	    sizeof(entry->length) >= elog_iter->log_area_length)
		return NULL;

	/* Can't go past terminating entry. */
	if (entry->type == SMBIOS_EVENT_TYPE_ENDLOG)
		return NULL;

	/* Check if the entry exceeds the length of the log. */
	if (entry->length + next_offset >= elog_iter->log_area_length)
		return NULL;

	/* Advance the offset. */
	elog_iter->current_offset = next_offset;

	if (elog_iter->verbose)
		print_buffer(entry, entry->length);

	return entry;
}

/*
 * smbios_eventlog_get_current_entry - retrieve current event log entry
 *
 * @elog_iter:   eventlog iterator used to obtain current eventlog entry
 *
 * returns NULL on error, else current event log entry pointed to by
 * smbios_eventlog_iterator. The smbios_log_entry's lifetime is dependent on
 * the life of elog_iter, that is if elog_iter is free()'d the smbios_log_entry
 * returned by this function is no longer valid.
 */
struct smbios_log_entry *smbios_eventlog_get_current_entry(
    struct smbios_eventlog_iterator *elog_iter)
{
	struct smbios_log_entry *entry;

	if (elog_iter == NULL)
		return NULL;

	/* If we are pointing "before" the first entry move to next entry. */
	if (elog_iter->current_offset < 0) {
		entry = smbios_eventlog_get_next_entry(elog_iter);
	} else {
		entry =(void *)&elog_iter->log_area[elog_iter->current_offset];
	}

	return entry;
}

void *smbios_eventlog_get_header(struct smbios_eventlog_iterator *elog_iter)
{
	if (elog_iter == NULL)
		return NULL;

	/* There is no header if the header_offset equals the data offset. */
	if (elog_iter->header_offset == elog_iter->data_offset)
		return NULL;

	return &elog_iter->log_area[elog_iter->header_offset];
}

/* SMBIOS Event Log types, SMBIOSv2.4 section 3.3.16.1 */
static const struct valstr smbios_eventlog_types[] = {
	{ 0x00, "Reserved" },
	{ 0x01, "Single-bit ECC memory error" },
	{ 0x02, "Multi-bit ECC memory error" },
	{ 0x03, "Parity memory error" },
	{ 0x04, "Bus timeout" },
	{ 0x05, "I/O channel check" },
	{ 0x06, "Software NMI" },
	{ 0x07, "POST memory resize" },
	{ 0x08, "POST error" },
	{ 0x09, "PCI parity error" },
	{ 0x0a, "PCI system error" },
	{ 0x0b, "CPU failure" },
	{ 0x0c, "EISA failsafe timer timeout" },
	{ 0x0d, "Correctable memory log disabled" },
	{ 0x0e, "Logging disabled, too many errors" },
	{ 0x0f, "Reserved" },
	{ 0x10, "System limit exceeded" },
	{ 0x11, "Hardware watchdog reset" },
	{ 0x12, "System configuration information" },
	{ 0x13, "Hard-disk information" },
	{ 0x14, "System reconfigured" },
	{ 0x15, "Uncorrectable CPU-complex error" },
	{ 0x16, "Log area cleared" },
	{ 0x17, "System boot" },
	{ 0xff, "End of log" },
	{ 0x00, NULL }
};

/*
 * smbios_get_event_type_string - returns a string identifying the entry type
 *
 * @entry:   SMBIOS log entry to obtain string info for.
 *
 * returns the string describing the event type, else NULL if type is unknown
 * to SMBIOS.
 */
const char *smbios_get_event_type_string(struct smbios_log_entry *entry)
{
	return val2str_default(entry->type, smbios_eventlog_types, NULL);
}
