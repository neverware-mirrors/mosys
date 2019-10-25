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
 * I2C (Inter-Integrated Circuit) is a multi-master serial computer bus
 * invented by Philips that is used to attach low-speed peripherals to 
 * a motherboard, embedded system, or cellphone.
 */

#ifndef INTF_I2C_H__
#define INTF_I2C_H__

#include <inttypes.h>
#include <errno.h>

#define I2C_HANDLE_MAX		64

struct platform_intf;
struct i2c_intf {
	const char *sys_root;
	const char *dev_root;

	/*
	 * setup - prepare interface
	 *
	 * @intf:       platform interface
	 *
	 * returns 0 to indicate success
	 * returns <0 to indicate failure
	 */
	int (*setup)(struct platform_intf *intf);

	/*
	 * destroy - teardown interface
	 *
	 * @intf:       platform interface
	 */
	void (*destroy)(struct platform_intf *intf);

	/*
	 * smbus_read_reg - Read from a register addressable SMBus device
	 *
	 * @intf:       platform interface
	 * @bus:        I2C bus/adapter
	 * @address:    I2C slave address
	 * @reg:        I2C register offset
	 * @length:     number of bytes to read (1-255)
	 * @data:       data buffer
	 *
	 * returns number of bytes read
	 * returns <0 to indicate failure
	 */
	int (*smbus_read_reg)(struct platform_intf *intf,
			      int bus, int address, int reg,
			      int length, void *data);

	/*
	 * smbus_write_reg - Write bytes to SMBus slave address
	 *
	 * @intf:       platform interface
	 * @bus:        I2C bus/adapter
	 * @address:    I2C slave address
	 * @reg:        I2C register offset
	 * @length:     number of bytes to read (1-255)
	 * @data:       data buffer
	 *
	 * returns number of bytes written
	 * returns <0 to indicate error
	 */
	int (*smbus_write_reg)(struct platform_intf *intf,
			      int bus, int address, int reg,
			      int length, const void *data);

	/*
	 * smbus_read16_dev - Read SMBus device using 16-bit register offset
	 *
	 * @intf:       platform interface
	 * @bus:        I2C bus/adapter
	 * @address:    I2C slave address
	 * @reg:        I2C register offset (16-bit)
	 * @length:     number of bytes to read (1-255)
	 * @data:       data buffer
	 *
	 * returns number of bytes read
	 * returns <0 to indicate failure
	 */
	int (*smbus_read16)(struct platform_intf *intf,
			    int bus, int address, int reg,
			    int length, void *data);


	/*
	 * smbus_write16_buf - Write to SMBus device using 16-bit offset
	 *
	 * @handle:     I2C device handle
	 * @reg:        I2C register offset (16-bit)
	 * @dp:         data buffer
	 * @len:        number of bytes to write (1-32)
	 *
	 * returns number of bytes written
	 * returns <0 to indicate error
	 */
	int (*smbus_write16)(struct platform_intf *intf,
			     int bus, int address, int reg,
			     int length, const void *data);

	/*
	 * smbus_read_raw  -  bytewise SMBus read without register addressing
	 *
	 * @intf:       platform interface
	 * @bus:        I2C bus/adapter
	 * @address:    I2C slave address
	 * @length:	number of bytes to read
	 * @data:       data buffer
	 *
	 * returns number of bytes read
	 * returns <0 to indicate failure
	 */
	int (*smbus_read_raw)(struct platform_intf *intf,
			      int bus, int address, int length, void *data);

	/*
	 * smbus_write_raw  -  write to device without register addressing
	 *
	 * @intf:       platform interface
	 * @bus:        I2C bus/adapter
	 * @address:    I2C slave address
	 * @length:	number of bytes to write (1-256)
	 * @data:       data buffer
	 *
	 * returns number of bytes written
	 * returns <0 to indicate failure
	 */
	int (*smbus_write_raw)(struct platform_intf *intf,
			       int bus, int address, int length, void *data);

	/*
	 * find_driver - Determine if driver is loaded
	 *
	 * @intf:	Platform interface
	 * @module: 	The name of the module to search for
	 *
	 * returns 0 if the module is not found or if /proc/modules does not exist
	 * returns > 0 if the module is found
	 */
	int (*find_driver)(struct platform_intf *intf,
			   const char *name);
};

/* I2C operations for Linux /dev interface */
extern struct i2c_intf i2c_dev_intf;

struct i2c_addr {
	int bus;
	int addr;
};

#include <stddef.h>
#include "intf/linux-i2c-dev.h"

#endif /* INTF_I2C_H__ */
