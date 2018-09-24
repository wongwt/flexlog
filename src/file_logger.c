//! @brief      FlexLog file logger extension
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#include "file_logger.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static FILE *handle = NULL;

void PrintFileLog(log_s *log) {
    if (handle == NULL) {
        return;
    }

    fprintf(handle, "[%s] %d <%s> %s\n", log->time_str, log->pid, log->type_str,
            log->message);
    fflush(handle);
}

int SetupFileLogger(void *arg) {
    const char *path = arg;

    if (handle != NULL) {
        fprintf(stderr, "File logger is already setup\n");
        return -EPERM;
    }
    if (path == NULL || strlen(path) == 0) {
        fprintf(stderr, "Invalid log file path\n");
        return -EINVAL;
    }

    errno = 0;
    handle = fopen(path, "a+");
    if (handle == NULL) {
        fprintf(stderr, "%s: Failed to open log file\n", strerror(errno));
        return -errno;
    }

    return 0;
}

int UnsetFileLogger(void) {
    if (handle == NULL) {
        fprintf(stderr, "File logger is already unset\n");
        return -EPERM;
    }

    fprintf(handle, "-*-*-*-*-*-*-*-*-\n");
    fflush(handle);

    errno = 0;
    int ret = fclose(handle);
    if (ret != 0) {
        fprintf(stderr, "%s: Failed to close log file\n", strerror(errno));
        return -errno;
    }

    return 0;
}
