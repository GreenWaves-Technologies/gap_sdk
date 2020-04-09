/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 1/15/2020.
 */

#ifndef PI_LOG_H
#define PI_LOG_H

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"

#include "pmsis.h"

/**
 * @brief Log level
 *
 */

#define PI_LOG_NONE 0 /*!< No log output */
#define PI_LOG_ERROR 1 /*!< Report API errors. */
#define PI_LOG_WARNING 2 /*!< Alert user: losing a dev when transfer is on going... */
#define PI_LOG_INFO 3 /*!<  System info : frequency change, voltage, ... */
#define PI_LOG_DEBUG 4 /*!< Common information for users. App level. */
#define PI_LOG_TRACE 5 /*!< Information low level for driver dev  (driver transfers). */

typedef uint8_t pi_log_level_t;

#define PI_LOG_ERROR_TEXT E
#define PI_LOG_WARNING_TEXT W
#define PI_LOG_INFO_TEXT I
#define PI_LOG_DEBUG_TEXT D
#define PI_LOG_TRACE_TEXT T

#ifndef PI_LOG_DEFAULT_LEVEL
#define PI_LOG_DEFAULT_LEVEL PI_LOG_WARNING
#endif

#ifndef PI_LOG_DEFAULT_DYNAMIC_LEVEL
#define PI_LOG_DEFAULT_DYNAMIC_LEVEL PI_LOG_TRACE
#endif

#ifndef PI_LOG_LOCAL_LEVEL
#define PI_LOG_LOCAL_LEVEL PI_LOG_DEFAULT_LEVEL
#endif

// Dynamic log level. This global variable must be implemented by PMSIS implementation.
extern pi_log_level_t dynamic_log_level; // Must be initialized at PI_LOG_DEFAULT_DYNAMIC_LEVEL By default.

typedef int (*vprintf_like_t)(const char *, va_list);
// Function pointer used to write log output. This global variable must be implemented by PMSIS implementation.
extern vprintf_like_t pi_log_vprint_func; // By default: &pi_log_default_vprintf;

/**
 * @brief Set function used to output log entries
 *
 * By default, log output goes to printf function.
 * This function can be used to redirect log
 * output to some other destination, such as file or network.
 * Returns the original log handler,
 * which may be necessary to return output to the previous destination.
 *
 * @param func new Function used for output. Must have same signature as vprintf.
 *
 * @return func old Function used for output.
 */
static inline vprintf_like_t pi_log_set_vprintf(vprintf_like_t func)
{
    vprintf_like_t old = pi_log_vprint_func;
    pi_log_vprint_func = func;
    return old;
}

/**
 * @brief Set the log level.
 *
 * By default, the dynamical log level is set to PI_LOG_DEFAULT_DYNAMIC_LEVEL.
 * This function can be used to set this level.
 * Returns the original dynamic log level.
 *
 * @param level new level used for log level.
 *
 * @return level old log level.
 */
static inline pi_log_level_t pi_log_set_dynamic_level(pi_log_level_t level)
{
    pi_log_level_t old = dynamic_log_level;
    dynamic_log_level = level;
    return old;
}

/**
 * @brief Get the current dynamic log level.
 *
 * @return the dynamic log level, it may be different than the global log level (Max(dynamic_level, static_level))
 */
static inline pi_log_level_t pi_log_get_dynamic_level(void)
{
    return dynamic_log_level;
}

/**
 * @brief Get the local log level.
 *
 * @return the static local log level, it may be different than global log level (Max(dynamic_level, static_level))
 */
#define pi_log_get_static_level() PI_LOG_LOCAL_LEVEL;

/**
 * @brief Get the current log level.
 *
 * @return Return the current log level: Max(dynamic_level, static_level).
 */
static inline pi_log_level_t pi_log_get_level(void)
{
    pi_log_level_t dynamic = pi_log_get_dynamic_level();
    return (dynamic <= PI_LOG_LOCAL_LEVEL) ? dynamic : PI_LOG_LOCAL_LEVEL;
}

/**
 * Log format
 */

// Colors
#ifndef NO_LOG_COLORS
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_T
#else //CONFIG_LOG_COLORS
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_T
#define LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS

#define LETTER2COLOR(letter) LOG_COLOR_ ## letter

// Core and cluster id
#ifndef PI_LOG_NO_CORE_ID
#define CORE_FORMAT "[%lu:%lu] "
#define CORE_VARS pi_cluster_id(), pi_core_id(),
#else
#define CORE_FORMAT
#define CORE_VARS
#endif

// Log format
#define STRINGIFY_LETTER(letter) #letter
#define PI_LOG_FORMAT(letter, format) LETTER2COLOR(letter) STRINGIFY_LETTER(letter) " " CORE_FORMAT "%s: " format LOG_RESET_COLOR "\n"


/**
 * @brief Write message into the log
 *
 * This function is not intended to be used directly. Instead, use one of
 * PI_LOG_ERR, PI_LOG_WNG, PI_LOG_INF, PI_LOG_DBG, PI_LOG_TRC macros.
 */
static inline void pi_log_write(pi_log_level_t level, const char *tag, const char *format, ...)
{
    if(level > dynamic_log_level)
        return;
    
    va_list list;
    va_start(list, format);
    (*pi_log_vprint_func)(format, list);
    va_end(list);
}

#define PI_LOG(level, tag, fmt, ...) \
    pi_log_write(level, tag, PI_LOG_FORMAT(level ## _TEXT, fmt), CORE_VARS tag, ##__VA_ARGS__)


#if PI_LOG_LOCAL_LEVEL >= PI_LOG_ERROR
#define PI_LOG_ERR(tag, fmt, ...) PI_LOG(PI_LOG_ERROR, tag, fmt, ##__VA_ARGS__)
#else // Error
#define PI_LOG_ERR(tag, fmt, ...)
#endif

#if PI_LOG_LOCAL_LEVEL >= PI_LOG_WARNING
#define PI_LOG_WNG(tag, fmt, ...) PI_LOG(PI_LOG_WARNING, tag, fmt, ##__VA_ARGS__)
#else // Warning
#define PI_LOG_WNG(tag, fmt, ...)
#endif

#if PI_LOG_LOCAL_LEVEL >= PI_LOG_INFO
#define PI_LOG_INF(tag, fmt, ...) PI_LOG(PI_LOG_INFO, tag, fmt, ##__VA_ARGS__)
#else // Info
#define PI_LOG_INF(tag, fmt, ...)
#endif

#if PI_LOG_LOCAL_LEVEL >= PI_LOG_DEBUG
#define PI_LOG_DBG(tag, fmt, ...) PI_LOG(PI_LOG_DEBUG, tag, fmt, ##__VA_ARGS__)
#else // Info
#define PI_LOG_DBG(tag, fmt, ...)
#endif

#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
#define PI_LOG_TRC(tag, fmt, ...) PI_LOG(PI_LOG_TRACE, tag, fmt, ##__VA_ARGS__)
#else // Trace
#define PI_LOG_TRC(tag, fmt, ...)
#endif

/**
 * @brief Function used by default to write log throught vprintf.
 * @param format
 * @param list
 * @return
 */
static inline int pi_log_default_vprintf(const char *format, va_list list)
{
    
    int rc;

//    va_list list;
//    va_start(list, format);
    
    rc = vprintf(format, list);
//    va_end(list);
    
    return rc;
}

;




/**
 * Module macros
 */
#define SSBL_TAG "ssbl"
#define SSBL_ERR(fmt, ...) PI_LOG_ERR(SSBL_TAG, fmt, ##__VA_ARGS__)
#define SSBL_WNG(fmt, ...) PI_LOG_WNG(SSBL_TAG, fmt, ##__VA_ARGS__)
#define SSBL_INF(fmt, ...) PI_LOG_INF(SSBL_TAG, fmt, ##__VA_ARGS__)
#define SSBL_DBG(fmt, ...) PI_LOG_DBG(SSBL_TAG, fmt, ##__VA_ARGS__)
#define SSBL_TRC(fmt, ...) PI_LOG_TRC(SSBL_TAG, fmt, ##__VA_ARGS__)

#define I2S_TAG "i2s"
#define I2S_ERR(fmt, ...) PI_LOG_ERR(I2S_TAG, fmt, ##__VA_ARGS__)
#define I2S_WNG(fmt, ...) PI_LOG_WNG(I2S_TAG, fmt, ##__VA_ARGS__)
#define I2S_INF(fmt, ...) PI_LOG_INF(I2S_TAG, fmt, ##__VA_ARGS__)
#define I2S_DBG(fmt, ...) PI_LOG_DBG(I2S_TAG, fmt, ##__VA_ARGS__)
#define I2S_TRC(fmt, ...) PI_LOG_TRC(I2S_TAG, fmt, ##__VA_ARGS__)

#define INIT_TAG "init"
#define INIT_ERR(fmt, ...) PI_LOG_ERR(INIT_TAG, fmt, ##__VA_ARGS__)
#define INIT_WNG(fmt, ...) PI_LOG_WNG(INIT_TAG, fmt, ##__VA_ARGS__)
#define INIT_INF(fmt, ...) PI_LOG_INF(INIT_TAG, fmt, ##__VA_ARGS__)
#define INIT_DBG(fmt, ...) PI_LOG_DBG(INIT_TAG, fmt, ##__VA_ARGS__)
#define INIT_TRC(fmt, ...) PI_LOG_TRC(INIT_TAG, fmt, ##__VA_ARGS__)

#endif //PI_LOG_H
