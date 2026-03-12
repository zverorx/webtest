/* 
 * This file is part of webtest.
 * webtest - web server for speed testing.
 *
 * Copyright (C) 2026 Egorov Konstantin
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
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>

#include "webtest_core.h"
#include "../http/http.h"
#include "../error.h"

#define MAX_REQUEST_SIZE	1024
#define MAX_CLIENT_COUNT	10

static int client_count; /**< Active client counter */

/**
 * @brief Creating a listening socket.
 *
 * Backlog argument is 16.
 * 
 * @param port Port for the socket address (host order).
 * @param[out] err Error status container.
 * 
 * @return fd if success, -1 otherwise.
 */
static int create_listen_socket(unsigned short port, err_t *err);

/**
 * @brief Handling HTTP client request.
 * 
 * @param sockfd Client socket descriptor.
 * @param[out] err Error status container.
 */
static void client_handle(int sockfd, err_t *err);

/**
 * @brief Hanling SIGCHLD.
 * 
 * @param signal Signal number.
 * @note After the child process is completed, the client_count is decremented.
 */
static void signal_handle(int signal);

void start(unsigned short port)
{
	int lsock, sfd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct sigaction sa;
	err_t err;
	pid_t pid;

	lsock = create_listen_socket(port, &err);
	if (lsock == -1) { err_report(&err); return; }

    sa.sa_handler = &signal_handle;
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

	for (;;) {
		sfd = accept(lsock, (struct sockaddr *) &client_addr, &client_len);
		if (sfd == -1) { perror("accept"); continue; }

		if (client_count >= MAX_CLIENT_COUNT) { continue; }

		pid = fork();
		if (pid != -1) { client_count++; }

		if (pid == 0) {
			close(lsock);

			client_handle(sfd, &err);
			if (err.has_error) { err_report(&err); }

			close(sfd);
			return;
		}
		
		close(sfd);
	}
}

static int create_listen_socket(unsigned short port, err_t *err)
{
	int lsock;
	int res, optval = 1;
	struct sockaddr_in lsn_addr;

	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock == -1) {
		err_set(err, errno, "socket");
		goto handle_error;
	}

	lsn_addr.sin_family = AF_INET;
	lsn_addr.sin_port = htons(port);
	lsn_addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	res = bind(lsock, (struct sockaddr *) &lsn_addr, sizeof(lsn_addr));
	if (res == -1) {
		err_set(err, errno, "bind");
		goto handle_error;
	}

	res = listen(lsock, 16);
	if (res == -1) {
		err_set(err, errno, "listen");
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

	start_line.method = NULL;
	start_line.path = NULL;
	start_line.version = NULL;

	read_buff = calloc(MAX_REQUEST_SIZE + 1, sizeof(char));
	if (!read_buff) {
		err_set(err, errno, "malloc");
		goto cleanup;
	}

	res = read(sockfd, read_buff, MAX_REQUEST_SIZE);
	if (res == -1) {
		err_set(err, errno, "read");
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
		free(read_buff);
		free(start_line.method);
		free(start_line.path);
		free(start_line.version);
}

static void signal_handle(int signal)
{
	int stat;
	while (waitpid(-1, &stat, WNOHANG) > 0) { client_count--; }
}
