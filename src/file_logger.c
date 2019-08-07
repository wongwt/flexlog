//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2019 Wei-Te Wong. MIT Licensed.
#include "file_logger.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *stream = NULL;

static const char *FileLogTypeToStr(uint32_t type) {
    switch (type) {
        case LOG_TYPE_FATAL:
            return "-F-";
        case LOG_TYPE_ERROR:
            return "-E-";
        case LOG_TYPE_WARN:
            return "-W-";
        case LOG_TYPE_INFO:
            return "-I-";
        case LOG_TYPE_DEBUG:
            return "-D-";
        case LOG_TYPE_TRACE:
            return "-T-";
        default:
            return "";
    }
}

static void PrintFileLog(log_header_s *hdr, const char *log) {
    if (!stream)
        return;

    fprintf(stream, "[%s] %s %s():%u %s\n",
            hdr->time, FileLogTypeToStr(hdr->type), hdr->func, hdr->line, log);
    fflush(stream);
}

logger_s file_logger = { 0 };

void SetupFileLogger(const char *path) {
    if (stream) {
        fprintf(stderr, "%s():%d %s; stream: %p\n",
                __func__, __LINE__, strerror(EPERM), stream);
        exit(EXIT_FAILURE);
    }

    FlexLogLock(true);

    errno = 0;
    stream = fopen(path, "a+");
    if (!stream) {
        fprintf(stderr, "%s():%d fopen(): %s; path: %s\n",
                __func__, __LINE__, strerror(errno), path);
        exit(EXIT_FAILURE);
    }

    file_logger.level = LOG_TYPE_TRACE;
    file_logger.print = PrintFileLog;

    FlexLogLock(false);
}

void UnsetFileLogger(void) {
    if (!stream)
        return;

    FlexLogLock(true);

    file_logger.level = LOG_TYPE_NONE;
    file_logger.print = NULL;

    fprintf(stream, "-*-*-*-*-*-*-*-*-\n");
    fflush(stream);
    fclose(stream);

    FlexLogLock(false);
}
