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

#ifndef MOSYS_PLATFORM_H__
#define MOSYS_PLATFORM_H__

#include <sys/types.h>
#include <stdint.h>

#include "lib/elog.h"

struct kv_pair;
struct nonspd_mem_info;

/* defined platform types */
enum platform_type {
	PLATFORM_DEFAULT,
	PLATFORM_X86_64,
	PLATFORM_ARMV7,
	PLATFORM_ARMV8,
};

/*
 *  Command types. These are exposed via "mosys -tv" to help with automated
 *  testing.
 */
enum arg_type {
	ARG_TYPE_GETTER,	/* "getter" functions */
	ARG_TYPE_SETTER,	/* "setter" functions */
	ARG_TYPE_SUB,		/* branch deeper into command hierachy */
};

/* nested command lists */
struct platform_intf;
struct platform_cmd {
	const char *name;		/* command name */
	const char *desc;		/* command help text */
	const char *usage;		/* command usage text */
	enum arg_type type;		/* argument type */
	union {				/* sub-commands or function */
		struct platform_cmd *sub;
		int (*func)(struct platform_intf *intf,
			    struct platform_cmd *cmd,
			    int argc, char **argv);
	} arg;
};

/* platform operations handlers */
struct pci_intf;
struct mmio_intf;
struct i2c_intf;

struct platform_op {
	struct pci_intf *pci;		/* pci interface */
	struct mmio_intf *mmio;		/* mmio interface */
	struct i2c_intf *i2c;		/* i2c interface */
};

/*
 * mappings of logical DIMM to physical
 * not all map types apply to all platforms
 */
enum dimm_map_type {
	DIMM_TO_BUS,			/* SPD bus */
	DIMM_TO_ADDRESS,		/* SPD address */
};

/* memory related callbacks */
struct smbios_table;
struct memory_spd_cb {
	int (*read)(struct platform_intf *intf,
	            int dimm, int reg, int len, unsigned char *buf);
};

struct memory_cb {
	int (*dimm_count)(struct platform_intf *intf);
	int (*dimm_map)(struct platform_intf *intf,
	                enum dimm_map_type type, int dimm);
	int (*dimm_speed)(struct platform_intf *intf,
	                  int dimm, struct kv_pair *kv);

	struct memory_spd_cb *spd;

	/* for systems where SPD-formatted memory info is not available */
	int (*nonspd_mem_info)(struct platform_intf *intf, int dimm,
				const struct nonspd_mem_info **info);
};

/* eventlog related callbacks */
enum smbios_log_entry_type;
struct smbios_log_entry;
struct eventlog_cb {
	int (*print_type)(struct platform_intf *intf,
	                  struct smbios_log_entry *entry,
	                  struct kv_pair *kv);
	int (*print_data)(struct platform_intf *intf,
	                  struct smbios_log_entry *entry,
	                  struct kv_pair *kv);
	int (*print_multi)(struct platform_intf *intf,
			   struct smbios_log_entry *entry,
			   int start_id);
	int (*verify)(struct platform_intf *intf,
	              struct smbios_log_entry *entry);
	int (*verify_header)(struct elog_header *elog_header);
	int (*add)(struct platform_intf *intf, enum smbios_log_entry_type type,
		   size_t data_size, uint8_t *data);
	int (*clear)(struct platform_intf *intf);
	int (*fetch)(struct platform_intf *intf, uint8_t **data,
		     size_t *length, off_t *header_offset, off_t *data_offset);
	int (*write)(struct platform_intf *intf, uint8_t *data, size_t length);
};

/* NVRAM callbacks */
struct nvram_cb {
	int (*vboot_read)(struct platform_intf *intf);
	int (*vboot_write)(struct platform_intf *intf,
			   const char *hexstring);
};

/* EEPROM and EEPROM-related callbacks */
struct eeprom_cb {
	struct eeprom *eeprom_list;
};

/* system information callbacks */
struct sys_cb {
	/* methods useful for probing */
	char *(*vendor)(struct platform_intf *intf);
	char *(*version)(struct platform_intf *intf);

	/*
	 * Query identifiers for chassis id, brand code, customization id, and
	 * SKU number.
	 */
	char *(*chassis)(struct platform_intf *intf);
	char *(*brand)(struct platform_intf *intf);
	char *(*customization)(struct platform_intf *intf);
	int (*sku_number)(struct platform_intf *intf);

	/* Obtain the model name of this device. With unified builds a board
	 * can support multiple models. This allows the model to be obtained,
	 * which can be used to adjust how various packages work. */
	char *(*model)(struct platform_intf *intf);
};

struct ec_cb {
	ssize_t (*vendor)(struct ec_cb *ec, char *buf, size_t buf_sz);
	ssize_t (*name)(struct ec_cb *ec, char *buf, size_t buf_sz);
	ssize_t (*fw_version)(struct ec_cb *ec, char *buf, size_t buf_sz);
	int (*pd_chip_info)(struct ec_cb *ec, int port);

	int (*setup)(struct ec_cb *ec);
	int (*destroy)(struct ec_cb *ec);

	void *priv;	/* private data for EC */
};

enum psu_types {
	PSU_TYPE_UNKNOWN,
	PSU_TYPE_BATTERY,	/* AC + rechargeable battery */
	PSU_TYPE_AC_ONLY,	/* No battery in system */
	PSU_TYPE_AC_PRIMARY,	/* AC as primary source + backup battery */
};

/* power supply related callbacks */
struct psu_cb {
	enum psu_types (*type)(struct platform_intf *intf);
};

/* platform-specific callbacks */
struct platform_cb {
	struct memory_cb *memory;	/* memory callbacks */
	struct eventlog_cb *eventlog;	/* eventlog callbacks */
	struct sys_cb *sys;		/* system callbacks */
	struct nvram_cb *nvram;		/* nvram related callbacks */
	struct eeprom_cb *eeprom;	/* eeprom callbacks */
	struct ec_cb *ec;		/* ec callbacks */
	struct ec_cb *pd;		/* pd callbacks */
	struct ec_cb *fp;		/* fp callbacks */
	struct psu_cb *psu;		/* power supply callbacks */
};

/* SKU based platform information, provided by lib/sku.h */
struct sku_info;

/*
 * Top-level interface handler.
 * One of these should be defined for each supported platform.
 */
struct platform_intf {
	enum platform_type type;	/* numeric platform type */
	const char *name;		/* canonical platform name */
	const struct sku_info *sku_info;	/* SKU information */
	struct platform_cmd **sub;	/* list of commands */
	struct platform_op *op;		/* operations */
	struct platform_cb *cb;		/* callbacks */

	/*
	 * returns 1 to indicate platform identified
	 * returns 0 otherwise
	 */
	int (*probe)(struct platform_intf *intf);
	int (*setup)(struct platform_intf *intf);
	int (*setup_post)(struct platform_intf *intf);
	int (*destroy)(struct platform_intf *intf);
};

/* Used for the global linked-list of all platforms. */
struct platform_list {
	struct platform_intf *entry;
	struct platform_list *next;
};

/**
 * REGISTER_PLATFORM - Macro to register a platform_intf in the global
 * platforms list.
 *
 * @intf:       The platform interface to register.
 * @name:       The name of the platform.  For unibuild devices, this
 *              platform_intf will be matched based on a comparison to
 *              /identity:platform-name in cros_config.
 *
 * This should be placed after the definition of a platform_intf, like so:
 *    static struct platform_intf platform_foo {
 *         ...
 *    };
 *    REGISTER_PLATFORM(platform_foo, "Foo");
 */
#define REGISTER_PLATFORM(intf, name) \
	_REGISTER_PLATFORM(intf, name, _register_platform_##intf)

/*
 * _REGISTER_PLATFORM Internal helper macro for REGISTER_PLATFORM.
 *
 * @_intf:        Platform interface.
 * @_name:        The name of the platform.
 * @function_id:  Identifier to be used for the generated function.
 *
 * This works by creating a constructor function [1] which
 * conditionally places the platform interface into the global list
 * before main runs.  In the case that _PLATFORM_IS_ENABLED(_name) can
 * be determined to be false at compile-time, the entire function,
 * including the references to the platform interface, will be
 * optimized away by the compiler.
 *
 * [1]: https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html
 */
#define _REGISTER_PLATFORM(_intf, _name, function_id)                       \
	static void __attribute__((constructor)) function_id(void)          \
	{                                                                   \
		static struct platform_list ent;                            \
		_Static_assert(                                             \
			__builtin_constant_p(_name),                        \
			"Platform names must be a compile-time constant."); \
		if (_PLATFORM_IS_ENABLED(_name)) {                          \
			struct platform_intf *intf = &(_intf);              \
			intf->name = _name;                                 \
			push_platform_intf(intf, &ent);                     \
		}                                                           \
	}

/*
 * _PLATFORM_IS_ENABLED - Determine if a platform_intf (by-name) is
 * enabled in this build.
 *
 * @name:       The name of the platform.
 *
 * For binary size optimizations to happen, this should result in an
 * expression which can be evaluated at compile-time under the current
 * optimization settings.
 */
#ifdef CONFIG_SINGLE_PLATFORM
#define _PLATFORM_IS_ENABLED(name) \
	!__builtin_strcmp(CONFIG_SINGLE_PLATFORM, name)
#else
#define _PLATFORM_IS_ENABLED(name) 1
#endif

/**
 * push_platform_intf - push an entry to the global platform list
 *
 * @intf:       Platform interface to register.
 * @dest:       Statically-allocated platform_list node.
 *
 * This is an internal helper used by the REGISTER_PLAFTORM macro.
 */
extern void push_platform_intf(struct platform_intf *intf,
			       struct platform_list *dest);

/*
 * mosys_platform_setup  -  determine current platform and return handler
 *
 * returns pointer to pre-defined interface for detected platform
 * returns NULL if platform not identified or other error
 *
 */
extern struct platform_intf *mosys_platform_setup(const char *platform_name);

/*
 * mosys_platform_destroy  -  clean up platform interface
 *
 * @intf:	platform interface
 *
 */
extern void mosys_platform_destroy(struct platform_intf *intf);

/*
 * platform_cmd_usage  -  print usage help text for command
 *
 * @cmd:	command pointer
 *
 */
extern void platform_cmd_usage(struct platform_cmd *cmd);

/*
 * print_tree - print command tree for this platform
 *
 * @intf:	platform interface
 */
extern void print_tree(struct platform_intf *intf);

extern int print_platforms(void);

#endif /* MOSYS_PLATFORM_H__ */
