/* Copyright 2010, Google Inc.
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
 * kv_pair.h: key-value pair defines and prototypes
 */

#ifndef KV_PAIR_H__
#define KV_PAIR_H__

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

/* key=value string pair list */
#define KV_PAIR_MAX_VALUE_LEN      1024

enum kv_pair_style {
	KV_STYLE_PAIR,		/* key1="value1" key2="value2" */
	KV_STYLE_VALUE,		/* | value1 | value2 | */
	KV_STYLE_LONG,		/* key1         | value1 */
				/* key2         | value2 */
	KV_STYLE_SINGLE,	/* prints raw value for specified key */
};

struct kv_pair {
	char *key;
	char *value;
	struct kv_pair *next;
};

extern enum kv_pair_style mosys_get_kv_pair_style(void);

extern void mosys_set_kv_pair_style(enum kv_pair_style style);

/*
 * kv_get_single_key  -  get single key to match against if -s option used
 *
 * returns single value specified by user
 * returns NULL if no single value has been specified by the user
 */
extern const char *kv_get_single_key(void);

/*
 * kv_set_single_key  -  set single key to match when using -s option
 *
 * @value:	value to match against
 */
extern void kv_set_single_key(const char *value);

/*
 * kv_pair_new  -  create new key=value pair
 *
 * returns pointer to new key=value pair
 * returns NULL to indicate error
 */
extern struct kv_pair *kv_pair_new(void);

/*
 * kv_pair_add  -  add new key=value pair to list
 *
 * @kv_list:    key=value pair list
 * @key:        key string
 * @value:      value string
 *
 * returns pointer to new key=value pair
 * returns NULL to indicate error
 */
extern struct kv_pair *kv_pair_add(struct kv_pair *kv_list,
                                   const char *key, const char *value);

/*
 * kv_pair_add_bool  -  add new boolean kvpair to list
 *
 * @kv_list:    key=value pair list
 * @key:        key string
 * @value:      value
 *
 * returns pointer to new key=value pair
 * returns NULL to indicate error
 */
extern struct kv_pair *kv_pair_add_bool(struct kv_pair *kv_list,
                                        const char *key, int value);

/*
 * kv_pair_fmt  -  add key=value pair based on printf format
 *                 NOTE: uses variable argument list
 *
 * @kv_list:    list of key=value pairs
 * @kv_key:     key string
 * @format:     printf-style format for value input
 * @...:        arguments to format
 *
 * returns pointer to new key=value pair
 * returns NULL to indicate error
 */
extern struct kv_pair *kv_pair_fmt(struct kv_pair *kv_list,
                                   const char *kv_key, const char *format, ...);

/*
 * kv_pair_free  -  clean a key=value pair list
 *
 * @kv_list:    pointer to key=value list
 */
extern void kv_pair_free(struct kv_pair *kv_list);

/*
 * kv_pair_print  -  print a key=value pair list
 *
 * @kv_list:    pointer to key=value list
 * @style:      print style
 *
 * returns 0 to indicate success
 * returns -1 to indicate error
 */
extern int kv_pair_print_to_file(FILE* fp, struct kv_pair *kv_list,
                                 enum kv_pair_style style);

/*
 * kv_pair_print  -  print a key=value pair list to gsys output
 *
 * @kv_list:    pointer to key=value list
 *
 * returns 0 to indicate success
 * returns -1 to indicate error
 */
extern int kv_pair_print(struct kv_pair *kv_list);

#endif /* KV_PAIR_H__ */
