//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2019 Wei-Te Wong. MIT Licensed.
#ifndef FLEXLOG_FILE_LOGGER_H_
#define FLEXLOG_FILE_LOGGER_H_

#include "log.h"

extern logger_s file_logger;

extern void SetupFileLogger(const char *path);
extern void UnsetFileLogger(void);

#endif  // FLEXLOG_FILE_LOGGER_H_
