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

#ifndef HTTP_H_SENTRY
#define HTTP_H_SENTRY

/**
 * 
 */
typedef struct request_start_line {
	char *method;
	char *path;
	char *version;
} stline_t;

/**
 *
 */
int http_parse(const char *request, stline_t *stline);

/**
 * 
 */
char *httpget(void);

/**
 * 
 */
char *not_implemented_stat(void);

#endif /* HTTP_H_SENTRY */
