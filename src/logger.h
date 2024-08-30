#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

enum LogLevel {
        LOG_ERROR,
        LOG_WARNING,
        LOG_INFO,
        LOG_DEBUG,
};

void log_debug(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void log_info(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void log_warning(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void log_error(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));

void log_write(const enum LogLevel level, const char *fmt, va_list lst);

#endif // LOGGER_H
