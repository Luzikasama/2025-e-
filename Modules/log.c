#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "modules.h" // ȷ�������� m_time_ms() ������HAL��Ķ���




static char s_tx_buffer[LOG_TX_BUFFER_SIZE];
static volatile uint16_t s_tx_head = 0; // дָ�� (��log_output�޸�)
static volatile uint16_t s_tx_tail = 0; // ��ָ�� (��logger_task���жϻص��޸�)

// DMA����״̬��־��volatile�ؼ���ȷ�����жϺ���ѭ���䰲ȫ����
static volatile bool s_is_tx_dma_busy = false;


static log_level_t current_level = LOG_LEVEL_DEBUG;

// ��־�ȼ��ַ���
static const char *log_level_str[] = {
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR"
};

// �ڲ�����ԭ��
static void log_output(log_level_t level, const char *fmt, va_list args);

// --- 2. ����API������ʵ�� ---

void log_init() {
    s_tx_head = 0;
    s_tx_tail = 0;
    s_is_tx_dma_busy = false;
}

void log_set_level(log_level_t level) {
    current_level = level;
}


// --- 3. �������߼�����־��ӡ������ʵ�� ---
// log_debug, log_info�Ⱥ������ֲ��䣬���ǵ���log_output

void log_debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_output(LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_output(LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_output(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_output(LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}

static void log_output(log_level_t level, const char *fmt, va_list args) {
    // �����־�ȼ�
    if (level < current_level) return;

    // 1. ʹ��һ����ʱ��ջ�ϻ���������ʽ����������־�ַ���
    char temp_buffer[LOG_FORMAT_BUFFER_SIZE];
    int header_len = 0;
    
    // 2. ��ʽ��ʱ�������־ͷ
#ifdef LOG_ENABLE_TIMESTAMP
    m_time_t ms_total = m_time_ms();
    uint64_t total_seconds = ms_total / 1000;
    uint32_t hours = total_seconds / 3600;
    uint32_t minutes = (total_seconds % 3600) / 60;
    uint32_t seconds = total_seconds % 60;
    uint32_t milliseconds = ms_total % 1000;
    header_len += snprintf(temp_buffer, sizeof(temp_buffer), "[%02lu:%02lu:%02lu:%03lu] ",
                    (unsigned long)hours, (unsigned long)minutes, (unsigned long)seconds, (unsigned long)milliseconds);
#endif
    header_len += snprintf(temp_buffer + header_len, sizeof(temp_buffer) - header_len, "[%s] ", log_level_str[level]);

    // 3. ��ʽ����־��������
    vsnprintf(temp_buffer + header_len, sizeof(temp_buffer) - header_len, fmt, args);

    // 4. ���ؼ����������µ� uartprintf_DMA �������з���
    //    ����ʹ�� "%s\r\n" ��ʽ������ʽ���õ��ַ����ͻ��з�һ����
    uartprintf_DMA("%s\r\n", temp_buffer);
}