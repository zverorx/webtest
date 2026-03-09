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

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

void free_all(int count, void *ptr1, ...)
{
	void *tmp_ptr;
	va_list args;

	va_start(args, ptr1);

	free(ptr1);

	for (int i = 1; i < count; i++) {
		tmp_ptr = va_arg(args, void *);
		free(tmp_ptr);
	}

	va_end(args);
}

void close_all(int count, int fd1, ...)
{
	int tmp_fd;
	va_list args;

	va_start(args, fd1);

	if (fd1 != -1) { close(fd1); }

	for (int i = 1; i < count; i++) {
		tmp_fd = va_arg(args, int);
		if (tmp_fd != -1) { close(tmp_fd); }
	}

	va_end(args);
}

int str_to_uint(const char *str, unsigned int *num)
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
