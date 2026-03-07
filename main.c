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
#include <errno.h>
#include <stdlib.h>

#include "webtest.h"

/** 
 * @def PORT_I
 * @brief Index in argv by which the port is stored.
 */
#define PORT_I	1

/**
 * @brief Converting a string to an unsigned integer.
 * 
 * @param str String representation of a number.
 * @param[out] num Conversion result.
 * 
 * @return 1 if success, 0 otherwise (num = 0).
 */
static int str_to_uint(const char *str, unsigned int *num);

int main(int argc, char **argv)
{
	unsigned int port;

	if (argc != 2) {
		fputs("Usage: webtest <listen_port>\n", stderr);
		return EXIT_FAILURE;
	}

	if (!str_to_uint(argv[PORT_I], &port)) {
		fputs("Invalid port\n", stderr);
		return EXIT_FAILURE;
	}

	return start(port);
}

static int str_to_uint(const char *str, unsigned int *num)
{
	char *endptr = NULL;

	if (!str || !num) { return 0; }

	errno = 0;

	*num = strtol(str, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0' || num < 0)
	{
		return 0;
	}

	return 1;
}
