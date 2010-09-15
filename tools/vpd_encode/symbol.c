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
 * symbol.c: symbol handling
 */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/string_builder.h"

#include "mosys/alloc.h"
#include "mosys/list.h"
#include "mosys/log.h"
#include "mosys/globals.h"

#include "symbol.h"

static struct ll_node *symbols;

/* for debugging */
static void print_symbol(struct ll_node *node)
{
	struct vpd_symbol *symbol;

	if (node->data)
		symbol = node->data;
	else
		return;

	lprintf(LOG_DEBUG, "symbol->name: \"%s\", symbol->value: "
	       "\"%s\"\n", symbol->name, symbol->value);
}

void free_vpd_symbol(struct vpd_symbol *symbol)
{
	free(symbol->name);
	free(symbol->value);
	free(symbol);
	symbol = NULL;
}

void free_vpd_symbol_node(struct ll_node *node)
{
	struct vpd_symbol *symbol;

	if (!node->data)
		return;

	symbol = (struct vpd_symbol *)node->data;
	free_vpd_symbol(symbol);
}

void cleanup_symtab()
{
	list_foreach(symbols, free_vpd_symbol_node);
	list_cleanup(&symbols);
}

/*
 * parse_symbol - parse a buffer and generate a symbol, if possible
 *
 * @str:	string to parse
 * @len:	length
 *
 * Parse two substrings delimited by equal ('=') sign:
 * First string: The symbol name, beginning with CONFIG_. No spaces,
 *               quotes, or special symbols allowed.
 * Second string: The symbol value, interpreted as follows:
 *     - If enclosed in quotes, value is a string.
 *     - If no quotes and the character is 'y' or 'n', value is
 *       assumed to be booloean.
 *     - If no quotes and character is a number, value is assumed
 *       to be numerical.
 *
 * Example:
 * CONFIG_SYMBOL_STRING="symbol value"
 * CONFIG_SYMBOL_BOOLEAN=y
 * CONFIG_SYMBOL_NUMERICAL=0xdeadbeef
 *
 * returns a newly allocated vpd_symbol if successful
 * returns NULL otherwise
 */
static struct vpd_symbol *parse_symbol(const char *str, size_t len)
{
	struct vpd_symbol *symbol = NULL;
	char *p;
	struct string_builder *sb;
	int index = 0;

	if (!str) {
		lprintf(LOG_DEBUG, "%s: string is NULL\n", __func__);
	}

	if (len < strlen("CONFIG_")) {
		lprintf(LOG_DEBUG, "%s: too few bytes: %lu\n", __func__, len);
		return NULL;
	}

	if (strncmp(str , "CONFIG_", 7)) {
		lprintf(LOG_DEBUG, "%s: string does not begin with "
		        "CONFIG_\n", __func__);
		return NULL;
	}

	p = strchr(str, '=');
	if (!p) {
		lprintf(LOG_DEBUG, "%s: string \"%s\" is not a symbol=value "
		        "pair\n", __func__, str);
		return NULL;
	}

	index = p - str;
	symbol = mosys_zalloc(sizeof(*symbol));
	symbol->name = mosys_malloc(index + 1);
	snprintf(symbol->name, index + 1, "%s", str);

	/* iterate past the '=' and copy the symbol's
	   value one character at a time */
	p++;
	index += sizeof(char);
	sb = new_string_builder();

	if (*p == '"') {
		/* symbol is a string, so we will stop parsing when
		   we encounter the end quote */
		p++;	/* iterate past open quote */
		while (*p != '"' && index < len) {
			string_builder_add_char(sb, *p);
			p++;
			index += sizeof(char);
		}

		if (*p != '"') {
			lprintf(LOG_ERR, "Error parsing \"%s\": "
			        "no end quote found\n", symbol->name);
			free_vpd_symbol(symbol);
			free_string_builder(sb);
			symbol = NULL;
			goto parse_symbol_exit;
		}

		symbol->type = STRING;
	} else if (isdigit(*p)) {
		char *endptr;
		unsigned long long u;

		u = strtoull(p, &endptr, 0);

		/* finish parsing the string for error checking */
		while (!isspace(*p) && index < len) {
			p++;
			index += sizeof(char);
		}

		if (endptr != p) {
			lprintf(LOG_ERR, "Error parsing \"%s\": "
			        "value is non-numeric\n", symbol->name);
			free_vpd_symbol(symbol);
			free_string_builder(sb);
			symbol = NULL;
			goto parse_symbol_exit;
		}

		string_builder_sprintf(sb, "%llu", u);

		symbol->type = NUMERIC;
	} else if (*p == 'y' || *p == 'n') {
		/* symbol is boolean, so we'll stop parsing after
		   this character is read */
		string_builder_add_char(sb, *p);

		/* FIXME: Add sanity checking */
		symbol->type = BOOLEAN;
	}

	symbol->value = mosys_strdup(string_builder_get_string(sb));
	free_string_builder(sb);

	lprintf(LOG_DEBUG, "%s: symbol->name: %s, symbol->value: %s\n",
	        __func__, symbol->name, symbol->value);

parse_symbol_exit:
	return symbol;
}

/*
 * gen_symtab - Parse config file and insert symbols into a table (linked list)
 *
 * Assumes file is in the format:
 * CONFIG_SYMBOL_STRING=value
 *
 * where the value may be a numeric value, a boolean 'y' or 'n' (for yes / no),
 * or a string enclosed in quotation marks ("")
 *
 * returns 0 to indicate success
 * returns <0 to indicate failure
 */
int gen_symtab(const char *file)
{
	FILE *fp;
	char *line = NULL;
	size_t read;
	size_t len = 0;

	fp = fopen(file, "r");
	if (fp == NULL) {
		fprintf(stderr, "unable to open fp file \"%s\": %s\n",
				file, strerror(errno));
		return -1;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		struct vpd_symbol *symbol = NULL;

		/* line is commented */
		if (line[0] == '#')
			continue;

		symbol = parse_symbol(line, read);
		if (symbol)
			symbols = list_insert_before(symbols, symbol);
	}

	if (line)
		free(line);
	fclose(fp);
	return 0;
}

struct vpd_symbol *lookup_symbol(const char *symbol)
{
	struct ll_node *node;
	struct vpd_symbol *ret = NULL;

	if (symbols)
		node = list_head(symbols);
	else
		return NULL;

	while (node) {
		struct vpd_symbol *sym = node->data;

		if (!sym) {
			lprintf(LOG_DEBUG, "%s: no data for this node???\n");
			continue;
		}

		if (!strcmp(symbol, sym->name)) {
			ret = sym;
			break;
		}

		node = node->next;
	}

	return ret;
}

char *sym2str(const char *symbol)
{
	struct vpd_symbol *s;

	s = lookup_symbol(symbol);
	if (!s)
		return NULL;

	return s->value;
}

int sym2bool(const char *symbol)
{
	struct vpd_symbol *s;
	unsigned long int val = 0;

	s = lookup_symbol(symbol);
	if (!s)
		return 0;

	if (s->type != BOOLEAN)
		return 0;

	if (s->value[0] == 'y') {
		val = 1;
	} else if (s->value[0] == 'n') {
		val = 0;
	} else {
		lprintf(LOG_DEBUG, "%s: Non-boolean value, s->value[0]: %c\n",
			__func__, s->value[0]);
	}

	return val;
}
