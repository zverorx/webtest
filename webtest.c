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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <netinet/in.h>

#include "webtest.h"

#define REQUEST_SIZE	1025 

#define CHECK(FUNCNAME, FUNCRES, ERROR_HANDLER)	\
	do {										\
		if (FUNCRES == -1) {					\
			perror(FUNCNAME);					\
			goto ERROR_HANDLER;					\
		}										\
	} while(0)

typedef struct request_start_line {
	char *method;
	char *path;
	char *version;
} stline_t;

/**
 * @see create_listen_socket
 */
enum error_t { socket_err, bind_err, listen_err };

/**
 * @brief Creating a listening socket.
 *
 * Backlog argument is 16.
 * 
 * @param port Port for the socket address (host order).
 * @param[out] err Reason for the error.
 * 
 * @return fd if success, -1 otherwise.
 */
static int create_listen_socket(unsigned int port, enum error_t *err);

static int http_parse(const char *request, stline_t *stline);
static char *httpget(void);
static char *not_implemented_stat(void);
static void free_all(int count, void *ptr1, ...);
static void close_all(int count, int fd1, ...);

int start(unsigned int port)
{
	int lsock, sfd = -1, res;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	stline_t start_line;
	enum error_t err;

	enum { read_i, write_i };
	char *buff[2];
	memset(buff, 0, sizeof(buff));
	memset(&start_line, 0, sizeof(start_line));

	lsock = create_listen_socket(port, &err);
	if (lsock == -1) {
		if (err == socket_err) { CHECK("socket", -1, handle_error); }
		if (err == bind_err)   { CHECK("bind",   -1, handle_error); }
		if (err == listen_err) { CHECK("listen", -1, handle_error); }
	}

	for (;;) {
		sfd = accept(lsock, (struct sockaddr *) &client_addr, &client_len);
		CHECK("accept", sfd, handle_error);

		buff[read_i] = calloc(REQUEST_SIZE, sizeof(char));
		if (!buff[read_i]) { CHECK("calloc", -1, handle_error); }

		res = read(sfd, buff[read_i], REQUEST_SIZE - 1);
		CHECK("read", res, handle_error);
		buff[read_i][res] = '\0';

		http_parse(buff[read_i], &start_line);
		if (!strcmp(start_line.method, "GET")) { buff[write_i] = httpget(); }
		else { buff[write_i] = not_implemented_stat(); }

		res = write(sfd, buff[write_i], 64);
		CHECK("write", res, handle_error);

		close_all(1, sfd);
		free_all(5, buff[read_i], buff[write_i], start_line.method, 
		     	 start_line.path, start_line.version);
	}

	free_all(5, buff[read_i], buff[write_i], start_line.method, 
		     start_line.path, start_line.version);
	close_all(2, lsock, sfd);
	return EXIT_SUCCESS;

	handle_error:
		free_all(5, buff[read_i], buff[write_i], start_line.method,
			     start_line.path, start_line.version);
		close_all(2, lsock, sfd);
		return EXIT_FAILURE;
}

static int create_listen_socket(unsigned int port, enum error_t *err)
{
	int lsock;
	int res, optval = 1;
	struct sockaddr_in lsn_addr;

	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock == -1) {
		if (err) { *err = socket_err; }
		goto handle_error;
	}

	lsn_addr.sin_family = AF_INET;
	lsn_addr.sin_port = htons(port);
	lsn_addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	res = bind(lsock, (struct sockaddr *) &lsn_addr, sizeof(lsn_addr));
	if (res == -1) {
		if (err) { *err = bind_err; }
		goto handle_error;
	}

	res = listen(lsock, 16);
	if (res == -1) {
		if (err) { *err = listen_err; }
		goto handle_error;
	}

	return lsock;

	handle_error:
		if (lsock != -1) { close(lsock); }
		return -1;
}

static int http_parse(const char *request, stline_t *stline)
{
	enum { buff_size = 256 };
	enum part { method, path, version};

	char buff[buff_size];
	int part = 0;

	if (!stline || !request) { return -1; }

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

				return 0;
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
		return -1;
}

static char *httpget(void)
{
	size_t msg_size = sizeof("HTTP/1.1 200 OK\n\0");
	char *msg = calloc(1, msg_size);
	memcpy(msg, "HTTP/1.1 200 OK\n\0", msg_size);

    return msg;
}

static char *not_implemented_stat(void)
{
	size_t msg_size = sizeof("HTTP/1.1 501 Not Implemented\n\0");
	char *msg = calloc(1, msg_size);
	memcpy(msg, "HTTP/1.1 501 Not Implemented\n\0", msg_size);

    return msg;
}

static void free_all(int count, void *ptr1, ...)
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

static void close_all(int count, int fd1, ...)
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
