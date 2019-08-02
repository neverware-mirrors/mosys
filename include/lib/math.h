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

#ifndef MOSYS_LIB_MATH_H__
#define MOSYS_LIB_MATH_H__

#include <inttypes.h>
#include <sys/types.h>

#include "mosys/mosys.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * rolling8_csum  -  Bytewise rolling summation "checksum" of a buffer
 *
 * @buf:	buffer to sum
 * @len:	length of buffer
 */
extern uint8_t rolling8_csum(uint8_t *buf, size_t len);

#ifndef __mask
# define __mask(high, low) ((1ULL << (high)) + \
                            (((1ULL << (high)) - 1) - ((1ULL << (low)) - 1)))
#endif

#ifndef __min
# define __min(a, b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef __max
# define __max(a, b)  ((a) > (b) ? (a) : (b))
#endif

#ifndef __abs
# define __abs(x) ((x) < 0 ? -(x) : (x))
#endif

static inline uint8_t bin2bcd(uint8_t bin)
{
	MOSYS_DCHECK(bin <= 99);
	return (bin % 10) | ((bin / 10) * 0x10);
}

/* unittest stuff */
extern int math_unittest(void);

#endif /* MOSYS_LIB_MATH_H__ */
