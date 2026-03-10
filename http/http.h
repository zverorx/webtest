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
 * @struct request_start_line
 * @brief Represents the starting line of an HTTP request.
 */
typedef struct request_start_line {
	char *method;
	char *path;
	char *version;
} stline_t;

/**
 * @brief Parsing the start line of an HTTP request.
 * 
 * @param request Request from client.
 * @param[out] stline Start line store.
 * @return 1 if success, 0 otherwise.
 */
int http_parse(const char *request, stline_t *stline);

/**
 * @brief Sending status code to the client.
 * 
 * @param sockfd Write socket.
 * @param code Status code.
 * @return Writing result or -2 if the message for the code is not implemented.
 * 
 * @note Support codes: 200, 404, 501.
 */
int send_code_stat(int sockfd, int code);

/**
 * 
 */
char *httpget(void);

#endif /* HTTP_H_SENTRY */
