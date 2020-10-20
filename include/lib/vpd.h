/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**
 * An arbitrary maximum value length which is more than sufficient for
 * what mosys needs (whitelabel_tag, customization_id, and
 * rlz_brand_code).
 */
#define VPD_MAX_VALUE_SIZE 256

/**
 * vpd_get_value() - Get a RO Vital Product Data (VPD) value.
 *
 * @value:	The name of the value to read (e.g., "whitelabel_tag").
 *
 * Returns: a newly allocated and null terminated buffer containing
 * the value upon success, or NULL upon failure.  The caller is
 * expected to free() the returned buffer.
 */
extern char *vpd_get_value(const char *value);
