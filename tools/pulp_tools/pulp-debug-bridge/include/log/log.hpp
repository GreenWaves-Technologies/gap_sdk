/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <mutex>

#define MAX_LOG_LINE 1024

typedef enum
{
  LOG_ERROR = 0,
  LOG_WARNING = 1,
  LOG_INFO = 2,
  LOG_DEBUG = 3,
  LOG_DETAIL = 4,
  LOG_PROTOCOL = 5,
  LOG_LEVEL_MAX = LOG_PROTOCOL
} log_level_e;

class Log {
  public:
    Log(const char * module) : module(module) {}
    Log() {}
    void print(log_level_e, const char *str, ...);
    void print(log_level_e, const char *str, va_list va);
    void error(const char *str, ...) ;
    void warning(const char *str, ...) ;
    void user(const char *str, ...) ;
    void debug(const char *str, ...) ;
    void detail(const char *str, ...) ;
    void protocol(const char *str, ...) ;
    bool is_lvl(int level);
    bool is_error_lvl();
    bool is_warning_lvl();
    bool is_user_lvl();
    bool is_debug_lvl();
    bool is_detail_lvl();
    bool is_protocol_lvl();
    static int log_level;
    static char last_error[MAX_LOG_LINE];
    const char * get_module() { return module; }
  private:
    static std::mutex m_last_error;
    const char * module;
};

#endif
