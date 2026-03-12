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
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "http.h"
#include "index.html.h"

int http_parse(const char *request, stline_t *stline)
{
	enum { buff_size = 256 };
	enum part { method, path, version};

	char buff[buff_size];
	int part = 0;

	if (!stline || !request) { return 0; }

	stline->method = NULL;
	stline->path = NULL;
	stline->version = NULL;

	for (int buff_i = 0, req_i = 0; 
		 buff_i < buff_size - 1 && request[req_i] != '\0';
		 buff_i++, req_i++) {
		if (request[req_i] == ' '  ||
			request[req_i] == '\n' ||
			request[req_i] == '\r') {

			if (buff_i == 0) { goto handle_error; }
			buff[buff_i] = '\0';

			switch (part) {
				case method:
					stline->method = strdup(buff);
					if (!stline->method) { goto handle_error; }
					break;
				case path:
					stline->path = strdup(buff);
					if (!stline->path) { goto handle_error; }
					break;
				case version:
					stline->version = strdup(buff);
					if (!stline->version) { goto handle_error; }

				return 1;
			}

			while (request[req_i + 1] == ' ') { req_i++; }

			part++;
			buff_i = -1;
			continue;
		}

		buff[buff_i] = request[req_i];
	}

	handle_error:
		free(stline->method);
		free(stline->path);
		free(stline->version);
		stline->method = NULL;
		stline->path = NULL;
		stline->version = NULL;
		return 0;
}

int send_code_stat(int sockfd, int code)
{
	int res = -2;

	switch (code) {
		case 200:
			res = write(sockfd, "HTTP/1.1 200 OK\r\n", 
						strlen("HTTP/1.1 200 OK\r\n"));
			break;
		case 400:
			res = write(sockfd, "HTTP/1.1 400 Bad Request\r\n", 
						strlen("HTTP/1.1 400 Bad Request\r\n"));
			break;
		case 404:
			res = write(sockfd, "HTTP/1.1 404 Not Found\r\n", 
						strlen("HTTP/1.1 404 Not Found\r\n"));
			break;
		case 422:
			res = write(sockfd, "HTTP/1.1 422 Unprocessable Content\r\n", 
						strlen("HTTP/1.1 422 Unprocessable Content\r\n"));
			break;
		case 501:
			res = write(sockfd, "HTTP/1.1 501 Not Implemented\r\n", 
						strlen("HTTP/1.1 501 Not Implemented\r\n"));
			break;
	}

	return res;
}

int send_headers(int sockfd, const char *type, size_t len)
{
	char buff[256];
	size_t headers_len;

	if (!type || len < 0) { return -2; }

	headers_len = snprintf(buff, sizeof(buff), "Content-Type: %s\r\n"
        				   "Content-Length: %ld\r\n\r\n", type, len);
	buff[headers_len] = '\0';

	return write(sockfd, buff, headers_len);
}

int httpget(int sockfd, const char *path)
{
	enum { buff_size = 1024, upper_limit = 10737418240 };
	int res = -1, code_stat;
	size_t junk_size = 0;
	char buff[buff_size];

	if (!path) { return -1; }

	if (!strcmp(path, "/")) {
		res = send_code_stat(sockfd, 200);
		if (res == -1) { return res; }

		res = send_headers(sockfd, "text/html", strlen(INDEX_HTML));
		if (res == -1) { return res; }

		return write(sockfd, INDEX_HTML, strlen(INDEX_HTML));
	}

	junk_size = path_parse(path, &code_stat);
	if (code_stat != 200) { return send_code_stat(sockfd, code_stat); }
	if (junk_size > upper_limit) { return send_code_stat(sockfd, 422); }

	res = send_code_stat(sockfd, 200);
	if (res == -1) { return res; }

	res = send_headers(sockfd, "application/octet-stream", junk_size);
	if (res == -1) { return res; }

	memset(buff, '?', sizeof(buff));

	for (;;) {
		if (junk_size <= buff_size) {
			return write(sockfd, buff, junk_size);
		}

		res = write(sockfd, buff, buff_size);
		if (res == -1) { return res; }
		junk_size -= buff_size;
	}
}

size_t path_parse(const char *path, int *code_stat)
{
	size_t junk_size = 0;
	char *endptr = NULL;
	char unit;
	char buff[32];

	if (strlen(path) < 3) {
		if (code_stat) { *code_stat = 404; }
		return 0;
	}

	memset(buff, 0, sizeof(buff));
	strncpy(buff, path + 1, sizeof(buff) - 1); /* /[0-9]+[BKMG]->[0-9]+[BKMG] */
	buff[strlen(buff) - 1] = '\0'; /* [0-9]+[BKMG]->[0-9]+ */

	errno = 0;
	junk_size = strtoull(buff, &endptr, 10);
	if (errno == ERANGE || *endptr != '\0' || buff[0] == '-') {
		if (code_stat) { *code_stat = 404; }
		return 0;
	}

	unit = path[strlen(path) - 1];
	switch (unit) {
		case 'B':
			if (junk_size > SIZE_MAX) {
				if (code_stat) { *code_stat = 422; }
				return 0;
			}
		break;
		case 'K':
			if (junk_size > SIZE_MAX / 1024) {
				if (code_stat) { *code_stat = 422; }
				return 0;
			}
			junk_size *= 1024;
			break;
			
		case 'M':
			if (junk_size > SIZE_MAX / (1024 * 1024)) {
				if (code_stat) { *code_stat = 422; }
				return 0;
			}
			junk_size *= 1024 * 1024;
			break;
			
		case 'G':
			if (junk_size > SIZE_MAX / (1024 * 1024 * 1024)) {
				if (code_stat) { *code_stat = 422; }
				return 0;
			}
			junk_size *= 1024 * 1024 * 1024;
			break;
			
		default:
			if (code_stat) { *code_stat = 404; }
			return 0;
	}

	if (code_stat) { *code_stat = 200; }
	return junk_size;
}
