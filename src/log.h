//! @author     "Wei-Te Wong" <wongwt@wongwt.net>
//! @copyright  Copyright 2019 Wei-Te Wong. MIT Licensed.
#ifndef FLEXLOG_LOG_H_
#define FLEXLOG_LOG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

enum {
    LOG_TYPE_NONE,

    LOG_TYPE_FATAL,
    LOG_TYPE_ERROR,
    LOG_TYPE_WARN,
    LOG_TYPE_INFO,
    LOG_TYPE_DEBUG,
    LOG_TYPE_TRACE,

    MAX_LOG_TYPE
};

typedef struct log_header_s {
    const char *time;
    const char *name;
    uint32_t type;
    const char *file;
    const char *func;
    uint32_t line;
} log_header_s;

typedef struct logger_s {
    uint32_t level;
    void (*print)(log_header_s *hdr, const char *log);
} logger_s;

extern void FlexLog(log_header_s *hdr, const char *log_format, ...);
extern void FlexLogLock(bool lock);
extern void FlexLogSetLevel(uint32_t index, uint32_t level);

extern bool IsPrintableLogType(uint32_t type);
extern const char *LogTimeToStr(void);

#define FLEX_LOG(log_type, ...)                                             \
do {                                                                        \
    log_header_s log_header = {                                             \
        .time = LogTimeToStr(),                                             \
        .name = "",                                                         \
        .type = log_type,                                                   \
        .file = __FILE__,                                                   \
        .func = __func__,                                                   \
        .line = __LINE__,                                                   \
    };                                                                      \
    FlexLog(&log_header, __VA_ARGS__);                                      \
} while (0)

#define FATAL(...)                                                          \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_FATAL)) {                               \
        FLEX_LOG(LOG_TYPE_FATAL, __VA_ARGS__);                              \
        exit(EXIT_FAILURE);                                                 \
    }                                                                       \
} while (0)

#define ERROR(...)                                                          \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_ERROR))                                 \
        FLEX_LOG(LOG_TYPE_ERROR, __VA_ARGS__);                              \
} while (0)

#define WARN(...)                                                           \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_WARN))                                  \
        FLEX_LOG(LOG_TYPE_WARN, __VA_ARGS__);                               \
} while (0)

#define INFO(...)                                                           \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_INFO))                                  \
        FLEX_LOG(LOG_TYPE_INFO, __VA_ARGS__);                               \
} while (0)

#define DEBUG(...)                                                          \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_DEBUG))                                 \
        FLEX_LOG(LOG_TYPE_DEBUG, __VA_ARGS__);                              \
} while (0)

#define TRACE(...)                                                          \
do {                                                                        \
    if (IsPrintableLogType(LOG_TYPE_TRACE))                                 \
        FLEX_LOG(LOG_TYPE_TRACE, __VA_ARGS__);                              \
} while (0)

#endif  // FLEXLOG_LOG_H_
