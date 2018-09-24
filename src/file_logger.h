//! @file       file_logger.h
//! @brief      FlexLog file logger extension
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#ifndef FLEXLOG_FILE_LOGGER_H_
#define FLEXLOG_FILE_LOGGER_H_

#include "log.h"

//! @brief      Print log to file
//!
//! @param      log         Log instance
extern void PrintFileLog(log_s *log);

//! @brief      Setup file logger
//!
//! @param      arg         Path to log file
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetupFileLogger(void *arg);

//! @brief      Unset file logger
//!
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int UnsetFileLogger(void);

//! @brief      File logger constructor
//!
//! @param      type        Maximum level of log types to print
#define FILE_LOGGER(type)                                                   \
{                                                                           \
    .level = type,                                                          \
    .print = PrintFileLog,                                                  \
    .setup = SetupFileLogger,                                               \
    .unset = UnsetFileLogger,                                               \
}

#endif  // FLEXLOG_FILE_LOGGER_H_
