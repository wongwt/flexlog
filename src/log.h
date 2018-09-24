//! @file       log.h
//! @brief      FlexLog logging utility
//!
//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2018 Wei-Te Wong. MIT Licensed.
#ifndef FLEXLOG_LOG_H_
#define FLEXLOG_LOG_H_

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//! @brief      Log type enumeration
enum {
    LOG_TYPE_NONE,      //!< Disable logging
    LOG_TYPE_FATAL,     //!< Fatal
    LOG_TYPE_ERROR,     //!< Error
    LOG_TYPE_WARN,      //!< Warning
    LOG_TYPE_INFO,      //!< Informative
    LOG_TYPE_DEBUG,     //!< Debug
    LOG_TYPE_TRACE,     //!< Trace
    LOG_TYPE_MAX
};

//! @brief      Logger enumeration
enum {
    LOGGER_PRIMARY,     //!< Primary logger
    LOGGER_SECONDARY,   //!< Secondary logger
    LOGGER_MAX
};

//! @brief      Log structure
typedef struct log_s {
    //! @brief      Current time string
    const char *time_str;

    //! @brief      Process ID
    pid_t pid;

    //! @brief      Log type
    int type;

    //! @brief      Log type string
    const char *type_str;

    //! @brief      Function name
    const char *func_str;

    //! @brief      Line number
    int line;

    //! @brief      Log message
    char *message;
} log_s;

//! @brief      Logger structure
typedef struct logger_s {
    //! @brief      Maximum level of log types to print
    int level;

    //! @brief      Print handler (mandatory)
    //!
    //! @param      log         Log instance
    void (*print)(log_s *log);

    //! @brief      Setup handler (optional)
    //!
    //! @param      arg         Optional arguments
    //! @retval     Success:    0
    //! @retval     Failure:    Negative Linux errno
    int (*setup)(void *arg);

    //! @brief      Unset handler (optional)
    //!
    //! @retval     Success:    0
    //! @retval     Failure:    Negative Linux errno
    int (*unset)(void);
} logger_s;

//! @brief      Print log
//!
//! @param      type        Log type
//! @param      func        Function name
//! @param      line        Line number
//! @param      format      Log message format
//! @param      ...         Optional arguments specified by format
extern void PrintLog(int type, const char *func, int line, char *format, ...);

//! @brief      Set logging clock type
//!
//! @param      clock       Logging clock type
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetLogClock(clockid_t clock);

//! @brief      Set maximum level of log type to print
//!
//! @param      index       Logger index
//! @param      level       Maximum level of log type to print
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetLogLevel(int index, int level);

//! @brief      Setup logger
//!
//! @param      index       Logger index
//! @param      logger      New logger instance
//! @param      arg         Optional arguments
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int SetupLogger(int index, logger_s logger, void *arg);

//! @brief      Unset logger
//!
//! @param      index       Logger index
//! @retval     Success:    0
//! @retval     Failure:    Negative Linux errno
extern int UnsetLogger(int index);

//! @brief      Print fatal log message
//!
//! @param      ...         Log message
#define FATAL(...)                                                          \
do {                                                                        \
    PrintLog(LOG_TYPE_FATAL, __func__, __LINE__, __VA_ARGS__);              \
    exit(EXIT_FAILURE);                                                     \
} while (0)

//! @brief      Print error log message
//!
//! @param      ...         Log message
#define ERROR(...)                                                          \
do {                                                                        \
    PrintLog(LOG_TYPE_ERROR, __func__, __LINE__, __VA_ARGS__);              \
} while (0)

//! @brief      Print warning log message
//!
//! @param      ...         Log message
#define WARN(...)                                                           \
do {                                                                        \
    PrintLog(LOG_TYPE_WARN, __func__, __LINE__, __VA_ARGS__);               \
} while (0)

//! @brief      Print informative log message
//!
//! @param      ...         Log message
#define INFO(...)                                                           \
do {                                                                        \
    PrintLog(LOG_TYPE_INFO, __func__, __LINE__, __VA_ARGS__);               \
} while (0)

//! @brief      Print debug log message
//!
//! @param      ...         Log message
#define DEBUG(...)                                                          \
do {                                                                        \
    PrintLog(LOG_TYPE_DEBUG, __func__, __LINE__, __VA_ARGS__);              \
} while (0)

//! @brief      Print trace log message
//!
//! @param      ...         Log message
#define TRACE(...)                                                          \
do {                                                                        \
    PrintLog(LOG_TYPE_TRACE, __func__, __LINE__, __VA_ARGS__);              \
} while (0)

#endif  // FLEXLOG_LOG_H_
