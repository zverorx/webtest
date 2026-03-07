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
#include <netinet/in.h>

#include "webtest.h"

#define CHECK(FUNCNAME, FUNCRES, ERROR_HANDLER)	\
	do {										\
		if (FUNCRES == -1) {					\
			perror(FUNCNAME);					\
			goto ERROR_HANDLER;					\
		}										\
	} while(0)

/**
 * @enum error_t
 * @see create_listen_socket
 */
enum error_t { socket_err, bind_err, listen_err };

/**
 * @brief Creating a listening socket.
 *
 * Backlog argument is 16.
 * 
 * @param port Port for the socket address.
 * @param[out] err Reason for the error.
 * 
 * @return fd if success, -1 otherwise.
 */
static int create_listen_socket(unsigned int port, enum error_t *err);

int start(unsigned int port)
{
	int lsock, sfd = -1;
	struct sockaddr_in sender_addr;
	socklen_t len;
	enum error_t err;

	lsock = create_listen_socket(port, &err);
	if (lsock == -1) {
		if (err == socket_err) { CHECK("socket", -1, handle_error); }
		if (err == bind_err)   { CHECK("bind",   -1, handle_error); }
		if (err == listen_err) { CHECK("listen", -1, handle_error); }
	}

	len = sizeof(sender_addr);
	sfd = accept(lsock, (struct sockaddr *) &sender_addr, &len);
	CHECK("accept", sfd, handle_error);

	/* TODO: request processing */

	close(lsock);
	close(sfd);

	return EXIT_SUCCESS;

	handle_error:
		if (lsock != -1) { close(lsock); }
		if (sfd != -1)   { close(sfd);   }

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
