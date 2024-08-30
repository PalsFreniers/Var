#include "logger.h"
#include <stdio.h>

void log_debug(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        log_write(LOG_DEBUG, fmt, ap);
        va_end(ap);
}

void log_info(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        log_write(LOG_INFO, fmt, ap);
        va_end(ap);
}

void log_warning(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        log_write(LOG_WARNING, fmt, ap);
        va_end(ap);
}

void log_error(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        log_write(LOG_ERROR, fmt, ap);
        va_end(ap);
}

const char *LogLevel_toString(const enum LogLevel lvl) {
        switch (lvl) {
                case LOG_DEBUG:
                        return "\033[34m[DEBUG]\033[0m";
                case LOG_INFO:
                        return "\033[32m[INFO]\033[0m";
                case LOG_WARNING:
                        return "\033[33m[WARNING]\033[0m";
                case LOG_ERROR:
                        return "\033[31m[ERROR]\033[0m";
                default:
                        return "\033[35m[LOG]\033[0m";
        }
}

void log_write(const enum LogLevel level, const char *fmt, va_list lst) {
        fprintf(stderr, "%s => ", LogLevel_toString(level));
        vfprintf(stderr, fmt, lst);
        fprintf(stderr, "\n");
}
