//! @brief      FlexLog logging utility
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#include "log.h"

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "console_logger.h"

static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

static clockid_t log_clock = CLOCK_MONOTONIC;

static logger_s logger_pool[LOGGER_MAX] = {
    [LOGGER_PRIMARY] = CONSOLE_LOGGER(LOG_TYPE_INFO),
};

static log_s Log(int type, const char *func, int line, char *message);
static bool LogClockIsValid(clockid_t clock);
static bool LoggerIndexIsValid(int index);
static bool LoggerInitialized(int index);
static bool LoggerIsValid(logger_s logger);
static char *LogTimeStr(void);
static bool LogTypeIsValid(int type);
static const char *LogTypeStr(int type);
static bool PrintLogRequired(int type);

static log_s Log(int type, const char *func, int line, char *message) {
    log_s log = {
        .time_str   = LogTimeStr(),
        .pid        = getpid(),
        .type       = type,
        .type_str   = LogTypeStr(type),
        .func_str   = func,
        .line       = line,
        .message    = message,
    };

    return log;
}

static bool LogClockIsValid(clockid_t clock) {
    return clock == CLOCK_MONOTONIC || clock == CLOCK_REALTIME;
}

static bool LoggerIndexIsValid(int index) {
    return index >= LOGGER_PRIMARY && index < LOGGER_MAX;
}

static bool LoggerInitialized(int index) {
    // Print handler is a mandatory field and should not be NULL
    return logger_pool[index].print != NULL;
}

static bool LoggerIsValid(logger_s logger) {
    // Print handler is a mandatory field and should not be NULL
    return logger.print != NULL;
}

static char *LogTimeStr(void) {
    struct timespec log_time = {0};
    errno = 0;
    if (clock_gettime(log_clock, &log_time) != 0) {
        fprintf(stderr, "Unable to get time: %s\n", strerror(errno));
        return "";
    }

    static char log_time_str[25] = {0};

    if (log_clock == CLOCK_MONOTONIC) {
        snprintf(log_time_str, sizeof log_time_str, "%8ld.%06ld",
                 log_time.tv_sec, log_time.tv_nsec / 1000);
    } else {
        struct tm tmp_time = {0};
        strftime(log_time_str, sizeof log_time_str, "%F %T%z",
                 localtime_r(&log_time.tv_sec, &tmp_time));
    }

    return log_time_str;
}

static bool LogTypeIsValid(int type) {
    return type >= LOG_TYPE_NONE && type < LOG_TYPE_MAX;
}

static const char *LogTypeStr(int type) {
    switch (type) {
        case LOG_TYPE_FATAL:
            return "FTL";
        case LOG_TYPE_ERROR:
            return "ERR";
        case LOG_TYPE_WARN:
            return "WRN";
        case LOG_TYPE_INFO:
            return "INF";
        case LOG_TYPE_DEBUG:
            return "DBG";
        case LOG_TYPE_TRACE:
            return "TRC";
        default:
            return "";
    }
}

static bool PrintLogRequired(int type) {
    if (type == LOG_TYPE_NONE || !LogTypeIsValid(type)) {
        return false;
    }

    for (int logger = 0; logger < LOGGER_MAX; logger++) {
        if (type <= logger_pool[logger].level) {
            return true;
        }
    }

    return false;
}

void PrintLog(int type, const char *func, int line, char *format, ...) {
    if (!PrintLogRequired(type)) {
        return;
    }

    va_list arg;
    va_start(arg, format);
    char *message = NULL;
    vasprintf(&message, format, arg);
    va_end(arg);

    log_s log = Log(type, func, line, message);

    int ret = pthread_mutex_lock(&log_lock);
    if (ret != 0) {
        fprintf(stderr, "Unable to lock log lock: %s\n", strerror(ret));
        goto free_memory;
    }

    for (int logger = 0; logger < LOGGER_MAX; logger++) {
        logger_s l = logger_pool[logger];

        if (l.print != NULL && type <= l.level) {
            l.print(&log);
        }
    }

    ret = pthread_mutex_unlock(&log_lock);
    if (ret != 0) {
        fprintf(stderr, "Unable to unlock log lock: %s\n", strerror(ret));
    }

free_memory:
    free(message);
    message = NULL;
}

int SetLogClock(clockid_t clock) {
    if (!LogClockIsValid(clock)) {
        fprintf(stderr, "Invalid log clock\n");
        return -EINVAL;
    }

    log_clock = clock;

    return 0;
}

int SetLogLevel(int index, int level) {
    if (!LoggerIndexIsValid(index) || !LoggerInitialized(index)) {
        fprintf(stderr, "Invalid logger index or logger not initialized\n");
        return -EPERM;
    }

    if (!LogTypeIsValid(level)) {
        fprintf(stderr, "Invalid log type\n");
        return -EINVAL;
    }

    int ret = pthread_mutex_lock(&log_lock);
    if (ret != 0) {
        fprintf(stderr, "Unable to lock log lock: %s\n", strerror(ret));
        return -ret;
    }

    logger_pool[index].level = level;

    if (pthread_mutex_unlock(&log_lock) != 0) {
        fprintf(stderr, "Unable to unlock log lock\n");
    }

    return ret;
}

int SetupLogger(int index, logger_s logger, void *arg) {
    if (!LoggerIndexIsValid(index) || LoggerInitialized(index)) {
        fprintf(stderr, "Invalid logger index or logger already initialized\n");
        return -EPERM;
    }

    if (!LoggerIsValid(logger)) {
        fprintf(stderr, "Invalid logger\n");
        return -EINVAL;
    }

    int ret = pthread_mutex_lock(&log_lock);
    if (ret != 0) {
        fprintf(stderr, "Unable to lock log lock: %s\n", strerror(ret));
        return -ret;
    }

    if (logger.setup != NULL) {
        ret = logger.setup(logger, arg);
        if (ret != 0) {
            fprintf(stderr, "Unable to setup logger: %s\n", strerror(-ret));
            goto mutex_unlock;
        }
    }

    logger_pool[index] = logger;

mutex_unlock:
    if (pthread_mutex_unlock(&log_lock) != 0) {
        fprintf(stderr, "Unable to unlock log lock\n");
    }

    return ret;
}

int UnsetLogger(int index) {
    if (!LoggerIndexIsValid(index) || !LoggerInitialized(index)) {
        fprintf(stderr, "Invalid logger index or logger not initialized\n");
        return -EPERM;
    }

    int ret = pthread_mutex_lock(&log_lock);
    if (ret != 0) {
        fprintf(stderr, "Unable to lock log lock: %s\n", strerror(ret));
        return -ret;
    }

    if (logger_pool[index].unset != NULL) {
        ret = logger_pool[index].unset();
        if (ret != 0) {
            fprintf(stderr, "Unable to unset logger: %s\n", strerror(-ret));
            goto mutex_unlock;
        }
    }

    memset(&logger_pool[index], 0, sizeof logger_pool[index]);

mutex_unlock:
    if (pthread_mutex_unlock(&log_lock) != 0) {
        fprintf(stderr, "Unable to unlock log lock\n");
    }

    return ret;
}
