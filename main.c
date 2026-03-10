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

#include "core/webtest_core.h"
#include "common/common.h"

int main(int argc, char **argv)
{
	enum { port_i = 1 }; /* index of port in argv */
	unsigned int port;

	if (argc != 2) {
		fputs("Usage: webtest <listen_port>\n", stderr);
		return EXIT_FAILURE;
	}

	if (!str_to_uint(argv[port_i], &port)) {
		fputs("Invalid port\n", stderr);
		return EXIT_FAILURE;
	}

	start(port);
}
