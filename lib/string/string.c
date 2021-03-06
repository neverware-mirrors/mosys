/*
 * Copyright 2010, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistribution of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * Redistribution in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of Sun Microsystems, Inc. or the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * 
 * This software is provided "AS IS," without a warranty of any kind.
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN MICROSYSTEMS, INC. ("SUN") AND ITS LICENSORS SHALL NOT BE LIABLE
 * FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.  IN NO EVENT WILL
 * SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA,
 * OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR
 * PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY OF
 * LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 * string.c: string utilities
 */

#include <ctype.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "mosys/alloc.h"
#include "mosys/log.h"

#include "lib/string.h"

/*
 * buf2str  -  represent an array of bytes as a hex string (no prefix)
 *
 * @inbuf:      input array[]
 * @length:     buffer length
 *
 * returns allocated pointer to outbuf
 * returns NULL on failure
 */
char *buf2str(uint8_t *inbuf, size_t length)
{
	size_t i;
	char *outbuf;
	char *outp;

	if (inbuf == NULL)
		return NULL;

	outbuf = mosys_malloc(2*length + 1);
	outp = outbuf;

	for (i = 0; i < length; i++) {
		/* sprintf includes the NUL terminator */
		sprintf(outp, "%02x", inbuf[i]);
		outp += 2;
	}

	outbuf[2*length] = '\0';
	return outbuf;
}

char *format_string(const char *fmt, ...)
{
	int n;
	int size = 128;
	char *p;
	va_list ap;

	p = mosys_malloc(size);
	while (1) {
		/* try to print in the allocated space */
		va_start(ap, fmt);
		n = vsnprintf(p, size, fmt, ap);
		va_end(ap);

		/* else try again with more space */
		if (n < 0) {
			/* older libc says overflow = error, double the size */
			size *= 2;  // COV_NF_LINE
		} else if (n >= size) {
			/* newer libs returns precisely what is needed */
			size = n+1;
		} else {
			/* success */
			return p;
		}
		p = mosys_realloc(p, size);
	}
	return NULL;
}

/*
 * find_pattern - find pattern in a buffer
 *
 * @haystack:		buffer to search in
 * @haystack_length:	number of bytes in haystack
 * @needle:		pattern to search for
 * @needle_length:	number of bytes in needle
 * @align:		alignment required
 * @offset:		location of needle, if found
 * 
 * returns 0 to indicate success
 * returns <0 to indicate failure
 */
int find_pattern(void *haystack, size_t haystack_length,
                 void *needle, size_t needle_length,
                 size_t align, size_t *offset)
{
	size_t i;

	if (!haystack || !needle || !offset) {
		return -1;
	}

	for (i = 0; i < haystack_length; i += align) {
		if (i + needle_length >= haystack_length) {
			break;
		}
		if (!memcmp((uint8_t *)haystack + i, needle, needle_length)) {
			*offset = i;
			return 0;
		}
	}

	return -1;
}

const char *strlfind(const char *str, const char *const arr[], int cs)
{
	int i;
	const char *ret = NULL;

	if (cs < 0 || cs > 1) {
		lprintf(LOG_SPEW, "Invalid CS\n");
		return NULL;
	}

	if (arr == NULL) {
		lprintf(LOG_SPEW, "Empty static id list.\n");
		return NULL;
	}

	for (i = 0; arr[i] != NULL; i++) {
		lprintf(LOG_SPEW, "%s: \"%s\" ?= \"%s\": ",
		                  __func__, str, arr[i]);
		if (cs) {
			if (!strcmp(str, arr[i])) {
				ret = arr[i];
				lprintf(LOG_SPEW, "Yes.\n");
				break;
			}
		} else {
			if (!strcasecmp(str, arr[i])) {
				ret = arr[i];
				lprintf(LOG_SPEW, "Yes.\n");
				break;
			}
		}

		lprintf(LOG_SPEW, "No.\n");
	}

	return ret;
}

char *strlower(char *str)
{
	char *p;

	for (p = str; *p; p++)
		*p = tolower(*p);

	return str;
}

char *strupper(char *str)
{
	char *p;

	for (p = str; *p; p++)
		*p = toupper(*p);

	return str;
}

size_t strlcpy(char *dest, const char *src, size_t size) {
	size_t copied = 0;

	if (size == 0)
		return 0;

	for (size_t i = 0; i < size - 1; i++) {
		if (*src == '\0')
			break;

		*dest = *src;
		dest++;
		src++;
		copied++;
	}

	*dest = '\0';
	return copied;
}
