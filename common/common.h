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

#ifndef COMMON_H_SENTRY
#define COMMON_H_SENTRY

#include <stdio.h>

/**
 * 
 */
#define CHECK(FUNCNAME, FUNCRES, ERROR_HANDLER)	\
	do {										\
		if (FUNCRES == -1) {					\
			perror(FUNCNAME);					\
			goto ERROR_HANDLER;					\
		}										\
	} while(0)

/**
 * 
 */
void free_all(int count, void *ptr1, ...);

/**
 * 
 */
void close_all(int count, int fd1, ...);

/**
 * @brief Converting a string to an unsigned integer.
 * 
 * @param str String representation of a number.
 * @param[out] num Conversion result.
 * 
 * @return 1 if success, 0 otherwise (num = 0).
 */
int str_to_uint(const char *str, unsigned int *num);

#endif /* COMMON_H_SENTRY */
