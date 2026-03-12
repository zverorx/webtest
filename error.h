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

#ifndef ERROR_H_SENTRY
#define ERROR_H_SENTRY

#include <errno.h>
#include <string.h>
#include <stdio.h>

/**
 * @struct error
 * @brief Error status container.
 */
typedef struct error {
	int has_error;      /**< Error flag (1 = error, 0 = no error) */
	int errnum;         /**< System error number (errno) */
	char funcname[16];  /**< Function name where error occurred */
} err_t;

/**
 * @brief Sets error information in the error structure.
 * 
 * @param err Error status container.
 * @param errnum System error number (errno).
 * @param funcname Function name where error occurred.
 */
static inline void err_set(err_t *err, int errnum, const char *funcname)
{
    if (!err) { return; }
    
    memset(err, 0, sizeof(err_t));

    err->has_error = 1;
    err->errnum = errnum;
    strncpy(err->funcname, funcname, sizeof(err->funcname) - 1);
}

/**
 * @brief Reports error to stderr.
 * 
 * @param err Error status container.
 * @warning Modifies errno.
 */
static inline void err_report(const err_t *err)
{
    if (!err) { return; }

    errno = err->errnum;
	perror(err->funcname);
}

#endif /* ERROR_H_SENTRY */
