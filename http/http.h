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
 * @note Support codes: 200, 400, 404, 422, 501.
 */
int send_code_stat(int sockfd, int code);

/**
 * @brief Sending HTTP headers to the client.
 * 
 * @param sockfd Write socket.
 * @param type Content-Type value.
 * @param len Content-Length value.
 * @return Writing result or -2 if invalid arguments.
 */
int send_headers(int sockfd, const char *type, size_t len);

/**
 * @brief Processing HTTP GET request.
 * 
 * @param sockfd Client socket.
 * @param path Request path.
 * @return Last write result, or -1 on error.
 */
int httpget(int sockfd, const char *path);

/**
 * @brief Parsing path string to extract garbage size.
 * 
 * Expected format: /[0-9]+[BKMG]:
 * 
 * @param path Path string to parse (e.g., "/42K")
 * @param[out] code_stat HTTP status code (200 if success).
 * @return Size in bytes, or 0 if parsing failed.
 */
size_t path_parse(const char *path, int *code_stat);

#endif /* HTTP_H_SENTRY */
