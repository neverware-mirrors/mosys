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
 * i2c.c: I2C bus access via Linux I2C IOCTL interface.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/log.h"
#include "mosys/platform.h"

#include "intf/i2c.h"

#include "lib/ddr4.h"
#include "lib/math.h"
#include "lib/string.h"
#include "lib/spd.h"

/* these are exposed only to make testing easier */
#define I2C_DEV_ROOT	"/dev"
#define I2C_SYS_ROOT	"/sys/bus/i2c/devices"

/*
 * Block writes need recovery delays for internal processing,
 * according to e.g. the datasheets for Atmel EEPROMs.
 * These are mildly pessimistic defaults.
 */
#define BLOCK_WRITE_DELAY	1000
#define BLOCK_WRITE_RETRIES	32

static struct i2c_handle {
	struct i2c_addr addr;
	int fd;
} i2c_handles[I2C_HANDLE_MAX];

static int i2c_handle_num = 0;

/*
 * i2c_open_dev  -  Open connection to I2C slave address
 *
 * @intf:       platform interface
 * @bus:        I2C bus/adapter
 * @address:    I2C slave address
 *
 * returns handle for open I2C device
 * returns <0 to indicate error
 */
static int i2c_open_dev(struct platform_intf *intf, int bus, int address)
{
	char devf[512];
	int handle, fd;

	if (i2c_handle_num >= I2C_HANDLE_MAX) {
		lprintf(LOG_NOTICE, "Out of I2C handles\n");
		return -1;
	}

	for (handle = 0; handle < i2c_handle_num; handle++) {
		if (i2c_handles[handle].addr.bus == bus &&
		    i2c_handles[handle].addr.addr == address &&
		    i2c_handles[handle].fd >= 0)
			return handle;
	}

	snprintf(devf, sizeof(devf), "%s/i2c-%d",
	         intf->op->i2c->dev_root, bus);

	fd = open(devf, O_RDWR);
	if (fd < 0) {
		lperror(LOG_DEBUG, "Unable to open I2C device %s", devf);
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, address) < 0) {
		lperror(LOG_NOTICE, "Unable to set I2C slave address to 0x%02x",
		        address);
		close(fd);
		return -1;
	}

	i2c_handles[i2c_handle_num].addr.bus = bus;
	i2c_handles[i2c_handle_num].addr.addr = address;
	i2c_handles[i2c_handle_num].fd = fd;

	lprintf(LOG_DEBUG, "Opened I2C handle %d to %d-%02x (fd %d)\n",
	        i2c_handle_num, bus, address, fd);

	return i2c_handle_num++;
}

/*
 * i2c_close_dev  -  Close all open I2C handles
 *
 * @intf:       platform interface
 * @handle:     I2C handle to close, -1 for all
 */
static void i2c_close_dev(struct platform_intf *intf)
{
	int i;

	// close all handles
	for (i = 0; i < i2c_handle_num; i++) {
		close(i2c_handles[i].fd);
		i2c_handles[i].fd = -1;
		i2c_handles[i].addr.bus = -1;
		i2c_handles[i].addr.addr = -1;
	}
	i2c_handle_num = 0;
}

static int smbus_read_reg(struct platform_intf *intf, int bus,
			  int address, int reg, int length, void *data)
{
	int handle, fd, i;
	int ddr4_handle, ddr4_fd;
	int32_t result;
	static int read_words = 1;
	int on_page_1 = 0;

	if (length < 1 || length > SPD_MAX_LENGTH) {
		lprintf(LOG_NOTICE, "Invalid I2C read length: %d\n", length);
		return -1;
	}

	lprintf(LOG_DEBUG,
	        "i2c_read_dev: Reading %d bytes from %d-%02x at %02x\n", length,
	        bus, address, reg);

	/* open connection to i2c slave */
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;
	fd = i2c_handles[handle].fd;

	memset(data, 0, length);
	i = 0;
	while (i < length) {
		/*
		 * For ddr4, for offsets 256+ bytes, need to switch to
		 * page 1 to read that data.
		 */
		if (i == 256 &&
		    (((char*)data)[DDR4_SPD_REG_DEVICE_TYPE] ==
		     SPD_DRAM_TYPE_DDR4)) {
			ddr4_handle = i2c_open_dev(intf, bus, SPD_PAGE_1);
			ddr4_fd = i2c_handles[ddr4_handle].fd;
			i2c_smbus_write_byte_data(ddr4_fd, 0, 0);
			on_page_1 = 1;
		}
		if (read_words && (i < length - 1)) {
			/* Do 2-byte reads whenever possible */
			result = i2c_smbus_read_word_data(fd, reg + i);

                        if (result < 0) {
				if (read_words) {
					/* try again with byte read */
					read_words = 0;
					continue;
				}
				// COV_NF_START
				lperror(LOG_NOTICE,
				        "Failed to read I2C register 0x%02x "
				        "from i2c-%d-%02x", reg + i, bus,
				        address);
				break;
				// COV_NF_END
			}
			memcpy(data + i, &result, 2);
			i = i + 2;
                } else {
			/* Do an 1-byte read otherwise*/
			result = i2c_smbus_read_byte_data(fd, reg + i);

			if (result < 0) {
				lperror(LOG_NOTICE,
				        "Failed to read I2C register 0x%02x "
				        "from i2c-%d-%02x", reg + i, bus,
				        address);
				break;
			}
			memcpy(data + i, &result, 1);
			i++;
		}
	}
	if (on_page_1 == 1) {
		ddr4_handle = i2c_open_dev(intf, bus, SPD_PAGE_0);
		ddr4_fd = i2c_handles[ddr4_handle].fd;
		i2c_smbus_write_byte_data(ddr4_fd, 0, 0);
		on_page_1 = 0;
	}

	return i;
}

/*
 * We can't actually use i2c_smbus_read_block_data() because the driver
 * doesn't know how to do the 2-byte address write. So we do the best we can
 * by performing the address write once, then calling i2c_smbus_read_byte()
 * repeatedly to keep the overhead to a minimum.
 */
static int smbus_read16_dev(struct platform_intf *intf, int bus,
			    int address, int reg, int length, void *data)
{
	uint8_t hi, lo;
	int fd, handle, i;
	int32_t result;
	uint8_t *dp = data;

	memset(data, 0, length);

	lprintf(LOG_DEBUG,
	        "i2c_read16_dev: Reading %d bytes from %d-%02x at %02x\n",
	        length, bus, address, reg);

	// open connection to i2c slave
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;
	fd = i2c_handles[handle].fd;

	// Write eeprom offset
	hi = reg >> 8;
	lo = reg & 0xff;
	result = i2c_smbus_write_byte_data(fd, hi, lo);
	if (result < 0) {
		lperror(LOG_NOTICE,
		        "Failed to write I2C register 0x%04x from i2c-%d-%02x",
		        reg, bus, address);
		return -1;
	}

	// Read the block
	for (i = 0; i < length; ++i) {
		result = i2c_smbus_read_byte(fd);
		if (result < 0) {
			lperror(LOG_NOTICE,
			        "Failed to read I2C register 0x%04x "
			        "from i2c-%d-%02x", reg + i, bus, address);
			return -1;
		}
		dp[i] = result;
	}

	return i;
}

static int smbus_read_raw(struct platform_intf *intf,
			  int bus, int address, int length, void *data)
{
	int result, count;
	int fd, handle;
	uint8_t *dp = data;

	lprintf(LOG_DEBUG,
	        "%s: Reading byte from %d-%02x\n",
	        __func__, bus, address);

	/* open connection to i2c slave */
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;
	fd = i2c_handles[handle].fd;

	for (count = 0; count < length; count++) {
		/* read byte */
		result = i2c_smbus_read_byte(fd);
		if (result < 0) {
			lperror(LOG_NOTICE,
			        "%s: Failed to read from from i2c-%d-%02x",
			        __func__, bus, address);
			break;
		}

		dp[count] = result;
	}

	return count;
}

static int smbus_write_reg(struct platform_intf *intf, int bus,
			   int address, int reg, int length, const void *data)
{
	int handle, fd, i;
	int32_t result;
	const uint8_t *data_ptr = data;

	if (length < 1 || length > 256) {
		lprintf(LOG_NOTICE, "Invalid I2C write length: %d\n", length);
		return -1;
	}

	lprintf(LOG_DEBUG,
	        "i2c_write_dev: Writing %d bytes to %d-%02x at %02x\n",
	        length, bus, address, reg);

	/* open connection to i2c slave */
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;
	fd = i2c_handles[handle].fd;

	for (i = 0; i < length; i++) {
		/* write one byte at a time */
		result = i2c_smbus_write_byte_data(fd, reg + i, data_ptr[i]);
		if (result < 0) {
			lperror(LOG_NOTICE,
			        "Failed to write I2C register 0x%02x"
			        " from i2c-%d-%02x", reg, bus, address);
			i = -1;
			break;
		}
	}

	return i;
}

/*
 * The LSByte of the 16-bit register offset is sent as the first byte of
 * the transfer buffer. Note the driver won't let us transfer more than
 * 32 bytes, and since we're using one data byte as the low byte of the
 * offset address, we have to perform the write in 31-byte chunks. Ugh.
 */
static int smbus_write16_buf(int handle, int reg, const uint8_t *dp, int len)
{
	uint8_t buf[32];
	int32_t result;
	int fd = i2c_handles[handle].fd;
	int bus = i2c_handles[handle].addr.bus;
	int address = i2c_handles[handle].addr.addr;
	int i;

	memset(buf, 0, 32);
	buf[0] = reg & 0xff;	// LSByte of register offset
	len = __min(len, 31);
	memcpy(&buf[1], dp, len);

	result = i2c_smbus_write_i2c_block_data(fd, reg >> 8, len+1, buf);

	if (result < 0) {
		lperror(LOG_NOTICE,
		        "Failed to write %d bytes "
		        "to I2C register 0x%04x from i2c-%d-%02x",
		        len, reg, bus, address);
		return result;
	}

	// Spin waiting for device to recover.
	for (i = 0; i < BLOCK_WRITE_RETRIES; ++i) {
		usleep(BLOCK_WRITE_DELAY);
		if (ioctl(fd, I2C_SLAVE, address) == 0)
			return result;
	}
	lperror(LOG_NOTICE,
	        "i2c-%d-%02x didn't recover in %d usec\n",
	        bus, address,
	        BLOCK_WRITE_RETRIES * BLOCK_WRITE_DELAY);
	return result;
}

static int smbus_write16_dev(struct platform_intf *intf,
			     int bus, int address, int reg,
			     int length, const void *data)
{
	int handle, written = 0;
	const uint8_t *data_ptr = (uint8_t *)data;
	int buf_len, count;

	// Open connection to this address
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;

	if (length == 0) {
		/* useful for setting internal address counters in devices */
		written = smbus_write16_buf(handle, reg, NULL, 0);
	}

	while (length) {
		buf_len = __min(length, 31);
		count = smbus_write16_buf(handle, reg, data_ptr, buf_len);
		if (count != buf_len)
			return written;
		length -= count;
		written += count;
		data_ptr += count;
		reg += count;

		if (length == 0)
			return written;

		buf_len = 1;
		count = smbus_write16_buf(handle, reg, data_ptr, buf_len);
		if (count != buf_len)
			return written;
		length -= buf_len;
		written += buf_len;
		data_ptr += buf_len;
		reg += buf_len;
	}
	return written;
}

static int smbus_write_raw(struct platform_intf *intf,
			   int bus, int address, int length, void *data)
{
	int handle, fd, count;
	int32_t result;
	const uint8_t *data_ptr = data;

	lprintf(LOG_DEBUG,
	        "%s: Writing byte to %d-%02x\n",
	        __func__, bus, address);

	/* open connection to i2c slave */
	handle = i2c_open_dev(intf, bus, address);
	if (handle < 0)
		return -1;
	fd = i2c_handles[handle].fd;

	for (count = 0; count < length; count++) {
		/* write byte */
		result = i2c_smbus_write_byte(fd, data_ptr[count]);
		if (result < 0) {
			lperror(LOG_NOTICE,
			        "Failed to write byte to i2c-%d-%02x",
			        bus, address);
			break;
		}
  	}

	return count;
}

static int i2c_find_driver(struct platform_intf *intf, const char *module)
{
	char s[80];
	FILE *fp;
	int len = strlen(module);
	int ret = 0;

	fp = fopen("/proc/modules", "r");
	if (fp == NULL)
		return 0;
	len = __min(len, 80);
	while (fgets(s, 80, fp) != NULL) {
		if (!strncmp(s, module, len) && (len == strcspn(s, " "))) {
			ret = 1;
			break;
		}
	}

	fclose(fp);
	return ret;
}

/* I2C operations based on Linux /dev interface */
struct i2c_intf i2c_dev_intf = {
	.sys_root		= I2C_SYS_ROOT,
	.dev_root		= I2C_DEV_ROOT,
	.destroy		= i2c_close_dev,
	.smbus_read_reg		= smbus_read_reg,
	.smbus_write_reg	= smbus_write_reg,
	.smbus_read16		= smbus_read16_dev,
	.smbus_write16		= smbus_write16_dev,
	.smbus_read_raw		= smbus_read_raw,
	.smbus_write_raw	= smbus_write_raw,
	.find_driver		= i2c_find_driver,
};
