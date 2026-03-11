/* 
 * This file is part of webtest.
 * webtest - web server for speed testing.
 *
 * Copyright (C) 2025 Egorov Konstantin
 *
 * webtest is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * webtest is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with webtest. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "core/webtest_core.h"

/**
 * @brief Converting a string to an unsigned short.
 * 
 * @param str String representation of a number.
 * @param[out] num Conversion result.
 * 
 * @return 1 if success, 0 otherwise.
 */
static int str_to_ushrt(const char *str, unsigned short *num);

int main(int argc, char **argv)
{
	enum { port_i = 1 }; /* index of port in argv */
	unsigned short port;

	if (argc != 2) {
		fputs("Usage: webtest <listen_port>\n", stderr);
		return EXIT_FAILURE;
	}

	if (!str_to_ushrt(argv[port_i], &port)) {
		fputs("Invalid port\n", stderr);
		return EXIT_FAILURE;
	}

	start(port);
}

static int str_to_ushrt(const char *str, unsigned short *num)
{
	char *endptr = NULL;
	long tmp_num;

	if (!str || !num)  { return 0; }
	if (str[0] == '-') { return 0; }

	errno = 0;

	tmp_num = strtol(str, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0') { return 0; }

	if (tmp_num > USHRT_MAX) { return 0; }

	*num = tmp_num;
	return 1;
}
