#ifndef __LOG_H__
#define __LOG_H__

#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// �����˺�������ʱ���
#define LOG_ENABLE_TIMESTAMP 1
#define LOG_FORMAT_BUFFER_SIZE 256
#define LOG_TX_BUFFER_SIZE 256 
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

void log_init();
void log_set_level(log_level_t level);

void log_debug(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);


#endif // __LOG_H__
