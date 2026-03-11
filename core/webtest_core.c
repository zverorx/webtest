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
#include <netinet/in.h>
#include <errno.h>

#include "webtest_core.h"
#include "../http/http.h"
#include "../common/common.h"

#define REQUEST_SIZE	1025 

typedef struct error {
	int has_error;
	int errnum;
	char funcname[16];
} err_t;

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
static int create_listen_socket(unsigned int port, err_t *err);
static void client_handle(int sockfd, err_t *err);

void start(unsigned int port)
{
	int lsock, sfd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	err_t err;
	pid_t pid;

	lsock = create_listen_socket(port, &err);
	if (lsock == -1) {
		errno = err.errnum;
		perror(err.funcname);
		return;
	}

	for (;;) {
		sfd = accept(lsock, (struct sockaddr *) &client_addr, &client_len);
		if (sfd == -1) { perror("accept"); continue; }

		pid = fork();
		if (pid == 0) {
			close(lsock);
			sleep(5);
			client_handle(sfd, &err);
			if (err.has_error) {
				errno = err.errnum;
				perror(err.funcname);
			}

			return;
		}
		
		close(sfd);
	}
}

static int create_listen_socket(unsigned int port, err_t *err)
{
	int lsock;
	int res, optval = 1;
	struct sockaddr_in lsn_addr;

	if (err) { memset(err, 0, sizeof(err_t)); }

	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock == -1) {
		if (err) {
			err->has_error = 1;
			err->errnum = errno;
			strncpy(err->funcname, "socket", sizeof(err->funcname) - 1);
		}
		goto handle_error;
	}

	lsn_addr.sin_family = AF_INET;
	lsn_addr.sin_port = htons(port);
	lsn_addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	res = bind(lsock, (struct sockaddr *) &lsn_addr, sizeof(lsn_addr));
	if (res == -1) {
		if (err) {
			err->has_error = 1;
			err->errnum = errno;
			strncpy(err->funcname, "bind", sizeof(err->funcname) - 1);
		}
		goto handle_error;
	}

	res = listen(lsock, 16);
	if (res == -1) {
		if (err) {
			err->has_error = 1;
			err->errnum = errno;
			strncpy(err->funcname, "listen", sizeof(err->funcname) - 1);
		}
		goto handle_error;
	}

	return lsock;

	handle_error:
		if (lsock != -1) { close(lsock); }
		return -1;
}

static void client_handle(int sockfd, err_t *err)
{
	int res;
	char *read_buff = NULL;
	stline_t start_line;

	if (err) { memset(err, 0, sizeof(err_t)); }

	read_buff = calloc(REQUEST_SIZE, sizeof(char));
	if (!read_buff) {
		if (err) {
			err->has_error = 1;
			err->errnum = errno;
			strncpy(err->funcname, "malloc", sizeof(err->funcname) - 1);
		}
		goto cleanup;
	}

	res = read(sockfd, read_buff, REQUEST_SIZE - 1);
	if (res == -1) {
		if (err) {
			err->has_error = 1;
			err->errnum = errno;
			strncpy(err->funcname, "read", sizeof(err->funcname) - 1);
		}
		goto cleanup;
	}
	read_buff[res] = '\0';

	if (http_parse(read_buff, &start_line)) {
		if (!strcmp(start_line.method, "GET")) {
			httpget(sockfd, start_line.path);
		}
		else { send_code_stat(sockfd, 501); }
	}
	else { send_code_stat(sockfd, 400); }

cleanup:
	close_all(1, sockfd);
	free_all(4, read_buff, start_line.method, 
				start_line.path, start_line.version);
}
