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

/**
 * @see create_listen_socket
 */
enum error_t { socket_err, bind_err, listen_err };

/**
 * @see http_method
 */
enum method { get_m, post_m, put_m, delete_m, other_m };

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

static enum method http_method(const char *request);
static char *httpget(void);
static char *not_implemented_stat(void);
static void free_all(int fd1, int fd2, void *ptr1, void *ptr2);

int start(unsigned int port)
{
	int lsock, sfd = -1, res;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	enum error_t err;

	enum { read_i, write_i };
	char *buff[2];
	memset(buff, 0, sizeof(buff));

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

		res = http_method(buff[read_i]);
		switch (res) {
			case get_m: buff[write_i] = httpget(); break;
			default: buff[write_i] = not_implemented_stat(); break;
		}

		res = write(sfd, buff[write_i], 64);
		CHECK("write", res, handle_error);

		free_all(sfd, -1, buff[read_i], buff[write_i]);
	}

	free_all(lsock, sfd, buff[read_i], buff[write_i]);
	return EXIT_SUCCESS;

	handle_error:
		free_all(lsock, sfd, buff[read_i], buff[write_i]);
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

static enum method http_method(const char *request)
{
	if (!request) { return other_m; }

	if (request[0] == 'G' && 
		request[1] == 'E' && 
		request[2] == 'T') { return get_m; }

	if (request[0] == 'P' && 
		request[1] == 'O' && 
		request[2] == 'S' && 
		request[3] == 'T') { return post_m; }

	if (request[0] == 'P' && 
		request[1] == 'U' && 
		request[2] == 'T') { return put_m; }

	if (request[0] == 'D' && 
		request[1] == 'E' && 
		request[2] == 'L' && 
		request[3] == 'E' && 
		request[4] == 'T' && 
		request[5] == 'E') { return delete_m; }

	return other_m;
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

static void free_all(int fd1, int fd2, void *ptr1, void *ptr2)
{
	if (fd1 != -1) { close(fd1); }
	if (fd2 != -1) { close(fd2); }
	
	if (ptr1) { free(ptr1); ptr1 = NULL; }
	if (ptr2) { free(ptr2); ptr2 = NULL; }
}
