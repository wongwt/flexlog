//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2019 Wei-Te Wong. MIT Licensed.
#include "log.h"

#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static FILE *ConsoleLogStream(uint32_t type) {
    switch (type) {
        case LOG_TYPE_FATAL:
        case LOG_TYPE_ERROR:
        case LOG_TYPE_WARN:
            return stderr;
        default:
            return stdout;
    }
}

static const char *ConsoleLogTypeToStr(uint32_t type) {
    static bool log_color = true;

    switch (type) {
        case LOG_TYPE_FATAL:
            return log_color ? "\033[35m<FTL>\033[m" : "<FTL>";  // Magenta
        case LOG_TYPE_ERROR:
            return log_color ? "\033[31m<ERR>\033[m" : "<ERR>";  // Red
        case LOG_TYPE_WARN:
            return log_color ? "\033[33m<WRN>\033[m" : "<WRN>";  // Yellow
        case LOG_TYPE_INFO:
            return log_color ? "\033[32m<INF>\033[m" : "<INF>";  // Green
        case LOG_TYPE_DEBUG:
            return log_color ? "\033[36m<DBG>\033[m" : "<DBG>";  // Cyan
        case LOG_TYPE_TRACE:
            return log_color ? "\033[37m<TRC>\033[m" : "<TRC>";  // White
        default:
            return "";
    }
}

static void PrintConsoleLog(log_header_s *hdr, const char *log) {
    FILE *stream = ConsoleLogStream(hdr->type);

    switch (hdr->type) {
        case LOG_TYPE_INFO:
            fprintf(stream, "[%13s] %s %s %s\n", hdr->time, hdr->name,
                    ConsoleLogTypeToStr(hdr->type), log);
            break;
        default:
            fprintf(stream, "[%13s] %s %s %s:%u %s\n", hdr->time, hdr->name,
                    ConsoleLogTypeToStr(hdr->type), hdr->file, hdr->line, log);
            break;
    }

    fflush(stream);
}

static logger_s console_logger = {
    .level = LOG_TYPE_INFO,
    .print = PrintConsoleLog,
};


#define ERROR_EXIT(f, ...)                                                  \
do {                                                                        \
    fprintf(stderr, "%s:%d " #f "\n", __FILE__, __LINE__, __VA_ARGS__);     \
    exit(EXIT_FAILURE);                                                     \
} while (0)

enum {
    LOGGER_CONSOLE,

    MAX_LOGGER
};

static logger_s *loggers[MAX_LOGGER] = {
    [LOGGER_CONSOLE] = &console_logger,
};

void FlexLog(log_header_s *hdr, const char *log_format, ...) {
    if (!hdr)
        ERROR_EXIT("%s; hdr: <null>", strerror(EINVAL));

    if (!log_format)
        ERROR_EXIT("%s; log_format: <null>", strerror(EINVAL));

    FlexLogLock(true);

    va_list arg;
    va_start(arg, log_format);
    char *log = NULL;
    vasprintf(&log, log_format, arg);
    va_end(arg);

    for (uint32_t index = 0; index < MAX_LOGGER; index++) {
        if (!loggers[index] || !loggers[index]->print)
            continue;

        if (hdr->type <= loggers[index]->level)
            loggers[index]->print(hdr, log);
    }

    free(log);

    FlexLogLock(false);
}

void FlexLogLock(bool lock) {
    static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

    if (lock)
        pthread_mutex_lock(&log_lock);
    else
        pthread_mutex_unlock(&log_lock);
}

void FlexLogSetLevel(uint32_t index, uint32_t level) {
    if (index >= MAX_LOGGER)
        ERROR_EXIT("%s; index: %u", strerror(EINVAL), index);

    if (level >= MAX_LOG_TYPE)
        ERROR_EXIT("%s; level: %u", strerror(EINVAL), level);

    FlexLogLock(true);

    loggers[index]->level = level;

    FlexLogLock(false);
}


bool IsPrintableLogType(uint32_t type) {
    if (type == LOG_TYPE_NONE || type >= MAX_LOG_TYPE)
        ERROR_EXIT("%s; type: %u", strerror(EINVAL), type);

    for (uint32_t index = 0; index < MAX_LOGGER; index++)
        if (loggers[index] && type <= loggers[index]->level)
            return true;

    return false;
}

const char *LogTimeToStr(void) {
    static clockid_t log_clock = CLOCK_MONOTONIC;

    struct timespec log_time = { 0 };

    errno = 0;
    if (clock_gettime(log_clock, &log_time) < 0)
        ERROR_EXIT("clock_gettime(): %s", strerror(EINVAL));

    static char log_time_str[25] = { 0 };

    switch (log_clock) {
        case CLOCK_MONOTONIC:
            snprintf(log_time_str, sizeof log_time_str, "%ld.%06ld",
                     log_time.tv_sec, log_time.tv_nsec / 1000);
            return log_time_str;
        case CLOCK_REALTIME: {
            struct tm tmp_time = { 0 };
            strftime(log_time_str, sizeof log_time_str, "%F %T%z",
                     localtime_r(&log_time.tv_sec, &tmp_time));
            return log_time_str;
        }
        default:
            ERROR_EXIT("%s; log_clock: %u", strerror(EINVAL), log_clock);
    }
}
