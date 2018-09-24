//! @brief      FlexLog console logger extension
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#include "console_logger.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int console_log_color_mode = CONSOLE_LOG_COLOR_ON;

static const char *ConsoleColorStr(int type);
static FILE *ConsoleHandle(int type);
static bool ConsoleLogColorIsValid(int mode);
static const char *ConsoleResetStr(int type);

static const char *ConsoleColorStr(int type) {
    if (console_log_color_mode == CONSOLE_LOG_COLOR_OFF) {
        return "";
    }

    switch (type) {
        case LOG_TYPE_FATAL:
            return "\033[35m";  // Magenta
        case LOG_TYPE_ERROR:
            return "\033[31m";  // Red
        case LOG_TYPE_WARN:
            return "\033[33m";  // Yellow
        case LOG_TYPE_INFO:
            return "\033[32m";  // Green
        case LOG_TYPE_DEBUG:
            return "\033[36m";  // Cyan
        case LOG_TYPE_TRACE:
            return "\033[37m";  // White
        default:
            return "";
    }
}

static FILE *ConsoleHandle(int type) {
    switch (type) {
        case LOG_TYPE_FATAL:
        case LOG_TYPE_ERROR:
        case LOG_TYPE_WARN:
            return stderr;
        default:
            return stdout;
    }
}

static bool ConsoleLogColorIsValid(int mode) {
    return mode == CONSOLE_LOG_COLOR_OFF || mode == CONSOLE_LOG_COLOR_ON;
}

static const char *ConsoleResetStr(int type) {
    if (console_log_color_mode == CONSOLE_LOG_COLOR_OFF) {
        return "";
    }

    switch (type) {
        case LOG_TYPE_FATAL:
        case LOG_TYPE_ERROR:
        case LOG_TYPE_WARN:
        case LOG_TYPE_INFO:
        case LOG_TYPE_DEBUG:
        case LOG_TYPE_TRACE:
            return "\033[m";
        default:
            return "";
    }
}

void PrintConsoleLog(log_s *log) {
    FILE *handle = ConsoleHandle(log->type);

    const char *color = ConsoleColorStr(log->type);
    const char *reset = ConsoleResetStr(log->type);

    if (log->type == LOG_TYPE_INFO) {
        fprintf(handle, "[%s] %d %s<%s>%s %s\n",
                log->time_str, log->pid, color, log->type_str, reset,
                log->message);
    } else {
        fprintf(handle, "[%s] %d %s<%s>%s %s():%d %s\n",
                log->time_str, log->pid, color, log->type_str, reset,
                log->func_str, log->line, log->message);
    }

    fflush(handle);
}

int SetConsoleLogColor(int mode) {
    if (!ConsoleLogColorIsValid(mode)) {
        fprintf(stderr, "Invalid console log color mode\n");
        return -EINVAL;
    }

    console_log_color_mode = mode;

    return 0;
}

int SetupConsoleLogger(void *arg) {
    (void) arg;

    return SetConsoleLogColor(CONSOLE_LOG_COLOR_ON);
}
