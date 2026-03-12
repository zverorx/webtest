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

#ifndef WEBTEST_H_SENTRY
#define WEBTEST_H_SENTRY

/**
 * @brief Starts the server.
 * 
 * @param port Port for the listening socket (host order).
 * @note It calls perror.
 */
void start(unsigned short port);

#endif /* WEBTEST_H_SENTRY */
