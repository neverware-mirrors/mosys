/* Copyright 2012, Google Inc.
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
 * platform.c: platform interface routines
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mosys/alloc.h"
#include "mosys/globals.h"
#include "mosys/intf_list.h"
#include "mosys/kv_pair.h"
#include "mosys/log.h"
#include "mosys/platform.h"
#include "mosys/output.h"

#include "lib/string.h"

#ifndef LINE_MAX
#define LINE_MAX 64
#endif

static struct platform_list *platform_list;

void push_platform_intf(struct platform_intf *intf, struct platform_list *dest)
{
	dest->entry = intf;
	dest->next = platform_list;
	platform_list = dest;
}

/*
 * Internal function used by mosys_platform_setup to search for a
 * supported platform by name
 */
static struct platform_intf *find_platform_by_name(const char *platform_name)
{
	for (struct platform_list *p = platform_list; p; p = p->next) {
		struct platform_intf *intf = p->entry;
		if (!strcasecmp(intf->name, platform_name))
			return intf;
		continue;
	}

	return NULL;
}

/*
 * Internal function used by mosys_platform_setup to probe each
 * platform until one matches
 */
static struct platform_intf *probe_platform(void)
{
	for (struct platform_list *p = platform_list; p; p = p->next) {
		struct platform_intf *intf = p->entry;

		if (intf->probe) {
			int rc = intf->probe(intf);

			if (rc < 0) {
				lprintf(LOG_DEBUG, "Error encountered when "
					"probing %s\n", intf->name);
				continue;
			} else if (rc > 0) {
				lprintf(LOG_DEBUG, "Platform %s found (via "
					"probing)\n", intf->name);
				return intf;
			}
		}
	}

	return NULL;
}

/*
 * mosys_platform_setup  -  identify platform, setup interfaces and commands
 *
 * @platform_name: Optional platform name given to bypass
 *     auto-detection
 *
 * returns pointer to identified platform interface
 * returns NULL if platform not identified or other error
 */
struct platform_intf *mosys_platform_setup(const char *platform_name)
{
	struct platform_intf *intf;
	int platform_count = 0;
	bool probe_called = false;

	/* setup defaults for each platform */
	for (struct platform_list *p = platform_list; p; p = p->next) {
		intf = p->entry;
		if (!intf->name)
			intf->name = "";
		if (!intf->op)
			intf->op = &platform_common_op;

		platform_count++;
	}

	if (platform_name) {
		intf = find_platform_by_name(platform_name);
	} else if (platform_count == 1) {
		intf = platform_list->entry;
	} else {
		intf = probe_platform();
		probe_called = true;
	}

	if (!intf)
		return NULL;

	/*
	 * If we did not call the platform's probe function earlier
	 * (e.g., selection by string or there is only a single
	 * platform compiled in mosys), we need to call it now as this
	 * may provide model-specific information.
	 *
	 * In the case the probe fails, most commands will still work.
	 * We setup the platform_intf anyway, but provide a warning.
	 */
	if (!probe_called && intf->probe) {
		if (intf->probe(intf) <= 0) {
			lprintf(LOG_ERR,
				"Platform probe function failed. "
				"Model-specific commands may be incorrect.\n");
		}
	}

	/* call platform-specific setup if found */
	if (intf->setup && intf->setup(intf) < 0)
		return NULL;

	/* prepare interface operations */
	if (intf_op_setup(intf) < 0) {
		mosys_platform_destroy(intf);
		return NULL;
	}

	/* call platform-specific post-setup if found */
	if (intf->setup_post &&
	    intf->setup_post(intf) < 0) {
		mosys_platform_destroy(intf);
		return NULL;
	}

	return intf;
}

/*
 * mosys_platform_destroy  -  clean up platform interface
 *
 * @intf:	platform interface
 */
void mosys_platform_destroy(struct platform_intf *intf)
{
	/* Call the destroy callbacks and cleanup afterwards. */
//	invoke_destroy_callbacks();
//	cleanup_destroy_callbacks();

	if (intf) {
		/* cleanup interface */
		if (intf->destroy)
			intf->destroy(intf);

		/* cleanup interface operations */
		if (intf->op)
			intf_op_destroy(intf);
	}
}

/*
 * platform_cmd_usage  -  print usage text for command
 *
 * @cmd:	command pointer
 */
void platform_cmd_usage(struct platform_cmd *cmd)
{
	printf("usage: %s %s\n\n", cmd->name, cmd->usage ? : "");
}

/*
 * tree_sub -	subcommand handler for print_tree
 *
 * @intf:	platform interface
 * @cmd:	command to iterate thru
 * @depth:	depth of recursion (number of leading tabs to prepend)
 * @str:	string with full command name
 *
 * returns 0 to indicate success
 * returns <0 to indicate failure (or end of command hierarchy)
 */
static int tree_subcommand(struct platform_intf *intf,
			   struct platform_cmd *cmd,
			   int depth, char *str)
{
	struct platform_cmd *sub;
	char *tabs;
	int index;

	tabs = mosys_malloc(depth + 1);
	memset(tabs, '\t', depth);
	tabs[depth] = '\0';

	for (sub = cmd->arg.sub, index = 1; sub->name != NULL; sub++, index++) {
		printf("%s", tabs);

		if (sub->type == ARG_TYPE_SUB) {
			int pos = 0, len = 0;

			if (mosys_get_verbosity() >= LOG_NOTICE) {
				printf("[branch] %s ", str);

				/* add full command info */
				pos = strlen(str);
				len = LINE_MAX - strlen(str);
				snprintf(str + pos, len, " %s", sub->name);
			}

			printf("%s\n", sub->name);

			/* continue descending into the hierarchy */
			tree_subcommand(intf, sub, depth + 1, str);

			if (mosys_get_verbosity() >= LOG_NOTICE) {
				memset(str + pos, 0, len);
			}
		} else if (sub->type == ARG_TYPE_GETTER) {
			if (mosys_get_verbosity() >= LOG_NOTICE)
				printf("[leaf] %s ", str);
			printf("%s\n", sub->name);
		} else if (sub->type == ARG_TYPE_SETTER) {
			if (mosys_get_verbosity() >= LOG_NOTICE)
				printf("[flur] %s ", str);
			printf("%s\n", sub->name);
		} else {
			// not a subcommand or function?!?
			return -1;
		}

	}

	free(tabs);
	return 0;
}

/*
 * print_tree - print command tree for this platform
 *
 * @intf:	platform interface
 */
void print_tree(struct platform_intf *intf)
{
	int root;
	char str[LINE_MAX];

	for (root = 0; intf->sub[root] != NULL; root++) {
		if (mosys_get_verbosity() >= LOG_NOTICE) {
			printf("[root] ");
			snprintf(str, sizeof(str), "mosys %s",
						   intf->sub[root]->name);
		} else {
			snprintf(str, sizeof(str), "%s", intf->sub[root]->name);
		}
		printf("%s\n", str);

		if (tree_subcommand(intf, intf->sub[root], 1, str) < 0)
			lprintf(LOG_DEBUG, "tree walking failed: %s", str);
	}
}

/*
 * print_platforms - print platform ids
 *
 * returns 0 to indicate success
 * returns <0 to indicate failure
 */
int print_platforms() {
	struct kv_pair *kv;
	int rc;

	/* go through all supported interfaces */
	for (struct platform_list *p = platform_list; p; p = p->next) {
		struct platform_intf *intf = p->entry;

		if (intf->type == PLATFORM_DEFAULT)
			continue;

		kv = kv_pair_new();
		kv_pair_add(kv, "id", intf->name);
		rc = kv_pair_print(kv);
		kv_pair_free(kv);
		if (rc)
			return rc;
	}

	return rc;
}
