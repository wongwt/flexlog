//! @file       console_logger.h
//! @brief      FlexLog console logger extension
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#ifndef FLEXLOG_CONSOLE_LOGGER_H_
#define FLEXLOG_CONSOLE_LOGGER_H_

#include "log.h"

//! @brief      Console log color mode
enum {
    CONSOLE_LOG_COLOR_OFF,
    CONSOLE_LOG_COLOR_ON
};

//! @brief      Print log to console
//!
//! @param      log         Log instance
extern void PrintConsoleLog(log_s *log);

//! @brief      Set console log color mode
//!
//! @param      mode        Console log color mode
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetConsoleLogColor(int mode);

//! @brief      Setup console logger
//!
//! @param      logger      Logger instance
//! @param      arg         Optional arguments
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetupConsoleLogger(struct logger_s logger, void *arg);

//! @brief      Console logger constructor
//!
//! @param      type        Maximum level of log types to print
#define CONSOLE_LOGGER(type)                                                \
{                                                                           \
    .level = type,                                                          \
    .print = PrintConsoleLog,                                               \
    .setup = SetupConsoleLogger,                                            \
}

#endif  // FLEXLOG_CONSOLE_LOGGER_H_
