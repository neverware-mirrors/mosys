/* Copyright 2010, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *    * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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
 * binary_blob.c: helper functions for handling binary blobs
 */

#include <stdlib.h>
#include <uuid/uuid.h>

#include "mosys/alloc.h"
#include "mosys/log.h"

#include "lib/string.h"
#include "lib/vpd_tables.h"
#include "lib/vpd_binary_blob.h"

int print_agz_blob_v3(uint8_t *data, uint32_t size, struct kv_pair *kv)
{
	struct agz_blob_0_3 *agz_blob = data;
	char s[37];
	char *s2;

	if (size != sizeof(*agz_blob)) {
		lprintf(LOG_DEBUG, "AGZ binary blob expected size: %lu, "
		                   "got: %lu\n", sizeof(*agz_blob), size);
		return -1;
	}

	kv_pair_add(kv, "blob_type", "agz_blob");

	snprintf(s, sizeof(agz_blob->product_name) + 1, "%s", agz_blob->product_name);
	kv_pair_add(kv, "product_name", s);

	snprintf(s, sizeof(agz_blob->product_manufacturer) + 1,
	         "%s", agz_blob->product_manufacturer);
	kv_pair_add(kv, "product_manufacturer", s);

	uuid_unparse(agz_blob->uuid, s);
	kv_pair_add(kv, "product_uuid", s);

	s2 = buf2str(agz_blob->motherboard_serial_number,
	                 sizeof(agz_blob->motherboard_serial_number));
	kv_pair_add(kv, "motherboard_serial_number", s2);
	free(s2);

	s2 = buf2nicid(agz_blob->esn_3g, NIC_ID_IMEI);
	kv_pair_add(kv, "3g_esn", s2);
	free(s2);

	s2 = buf2nicid(agz_blob->wlan_mac_id, NIC_ID_IEEE802);
	kv_pair_add(kv, "wlan_macid", s2);
	free(s2);

	snprintf(s, sizeof(agz_blob->country_code) + 1,
	         "%s", agz_blob->country_code);
	kv_pair_add(kv, "country_code", s);

	snprintf(s, sizeof( agz_blob->product_serial_number) + 1,
	         "%s", agz_blob->product_serial_number);
	kv_pair_fmt(kv, "product_serial_number", s);

	return 0;
}

int print_agz_blob_v5(uint8_t *data, uint32_t size, struct kv_pair *kv)
{
	struct agz_blob_0_5 *agz_blob = data;
	char s[37];
	char *s2;

	if (size != sizeof(*agz_blob)) {
		lprintf(LOG_DEBUG, "AGZ binary blob expected size: %lu, "
		                   "got: %lu\n", sizeof(*agz_blob), size);
		return -1;
	}

	kv_pair_add(kv, "blob_type", "agz_blob");

	snprintf(s, sizeof(agz_blob->product_name) + 1, "%s", agz_blob->product_name);
	kv_pair_add(kv, "product_name", s);

	snprintf(s, sizeof(agz_blob->product_manufacturer) + 1,
	         "%s", agz_blob->product_manufacturer);
	kv_pair_add(kv, "product_manufacturer", s);

	uuid_unparse(agz_blob->uuid, s);
	kv_pair_add(kv, "product_uuid", s);

	snprintf(s, sizeof(agz_blob->motherboard_serial_number) + 1,
	         "%s", agz_blob->motherboard_serial_number);
	kv_pair_add(kv, "motherboard_serial_number", s);

	snprintf(s, sizeof(agz_blob->esn_3g) + 1, "%s", agz_blob->esn_3g);
	kv_pair_add(kv, "3g_esn", s);

	s2 = buf2nicid(agz_blob->wlan_mac_id, NIC_ID_IEEE802);
	kv_pair_add(kv, "wlan_macid", s2);
	free(s2);

	snprintf(s, sizeof(agz_blob->country_code) + 1,
	         "%s", agz_blob->country_code);
	kv_pair_add(kv, "country_code", s);

	snprintf(s, sizeof( agz_blob->product_serial_number) + 1,
	         "%s", agz_blob->product_serial_number);
	kv_pair_fmt(kv, "product_serial_number", s);

	return 0;
}

int print_google_blob_v1_1(uint8_t *data, uint32_t size, struct kv_pair *kv)
{
	struct google_blob_1_1 *google_blob = data;
	char s[37];
	char *s2;

	if (size != sizeof(*google_blob)) {
		lprintf(LOG_DEBUG, "Google binary blob expected size: %lu, "
		                   "got: %lu\n", sizeof(*google_blob), size);
		return -1;
	}

	kv_pair_add(kv, "blob_type", "google_blob");

	snprintf(s, sizeof(google_blob->product_serial_number) + 1,
	         "%s", google_blob->product_serial_number);
	kv_pair_add(kv, "product_serial_number", s);

	snprintf(s, sizeof(google_blob->product_sku) + 1,
	         "%s", google_blob->product_sku);
	kv_pair_add(kv, "product_sku", s);

	uuid_unparse(google_blob->uuid, s);
	kv_pair_add(kv, "product_uuid", s);

	snprintf(s, sizeof(google_blob->motherboard_serial_number) + 1,
	         "%s", google_blob->motherboard_serial_number);
	kv_pair_add(kv, "motherboard_serial_number", s);

	snprintf(s, sizeof(google_blob->imei_3g) + 1, "%s", google_blob->imei_3g);
	kv_pair_add(kv, "imei_esn", s);

	snprintf(s, sizeof(google_blob->ssd_serial_number) + 1,
	         "%s", google_blob->ssd_serial_number);
	kv_pair_fmt(kv, "ssd_serial_number", s);

	snprintf(s, sizeof(google_blob->memory_serial_number) + 1,
	         "%s", google_blob->memory_serial_number);
	kv_pair_fmt(kv, "memory_serial_number", s);

	s2 = buf2nicid(google_blob->wlan_mac_id, NIC_ID_IEEE802);
	kv_pair_add(kv, "wlan_macid", s2);
	free(s2);

	return 0;
}

/*
 * print_google_blob_v2_0 - print google blob version 2.0
 *
 * @data: pointer to the beginning of blob data
 * @size: this argument is ignored -- it exists only to match function
 *        signatures for this type of callback.
 * @kv_pair: key=value pair
 *
 * The version 2.0 Google blob is different; it does not use a fixed format.
 * Instead, it is to be interpreted from a key-value format. The format is:
 * |--------------------------------------------------------------|
 * | type | key length | key string | value length | value string |
 * |--------------------------------------------------------------|
 *  1 byte     1 byte      varies        1 byte       varies
 *
 * Note: Strings are not NULL-terminated
 *
 */
int print_google_blob_v2_0(uint8_t *data, uint32_t size, struct kv_pair *kv)
{
	uint8_t *entry = data;
	enum google_blob_v2_0_field_type {
		GOOGLE_BLOB_V2_0_TERMINATOR = 0x00,
		GOOGLE_BLOB_V2_0_STRING = 0x01,
	} type;

	kv_pair_add(kv, "blob_type", "google_blob");

	type = entry[0];
	while (type != GOOGLE_BLOB_V2_0_TERMINATOR) {
		unsigned int key_length = 0, value_length;
		char *key_string, *value_string;
		int pos = 1;	/* start counting at key length */

		key_length = entry[pos] & ~0x80;
		while (entry[pos] & 0x80) {
			/* if highest bit is set, then continue to interpret
			   the next byte as the lower 7 significant bits */
			key_length = (key_length << 7) | (entry[pos] & ~0x80);
			pos++;
		}

		lprintf(LOG_DEBUG, "type: %02x, length: %02x\n", type, key_length);
		key_string = mosys_malloc(key_length + 1);
		memcpy(key_string, &entry[2], key_length);
		key_string[key_length + 1] = '\0';

		value_length = entry[2 + key_length];
		value_string = mosys_malloc(value_length + 1);
		memcpy(value_string, &entry[2 + key_length + 1], value_length);
		value_string[value_length + 1] = '\0';

		kv_pair_add(kv, key_string, value_string);
		free(key_string);
		free(value_string);

		/* advance the pointer and determine the next type */
		entry += 2 + key_length + 1 + value_length;
		type = entry[0];
	}

	return 0;
}
