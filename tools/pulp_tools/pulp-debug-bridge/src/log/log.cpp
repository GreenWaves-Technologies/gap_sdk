
#include "log.hpp"

#include <stdio.h>
#include <string.h>

char Log::last_error[MAX_LOG_LINE] = "unknown error";
int Log::log_level = LOG_DEBUG;
std::mutex Log::m_last_error;

bool Log::is_lvl(int level)
{
  return (this->log_level >= level);
}

bool Log::is_error_lvl() { return is_lvl(LOG_ERROR); }
bool Log::is_warning_lvl() { return is_lvl(LOG_WARNING); }
bool Log::is_user_lvl() { return is_lvl(LOG_INFO); }
bool Log::is_debug_lvl() { return is_lvl(LOG_DEBUG); }
bool Log::is_detail_lvl() { return is_lvl(LOG_DETAIL); }
bool Log::is_protocol_lvl() { return is_lvl(LOG_PROTOCOL); }

void Log::print(log_level_e level, const char *str, va_list va)
{
  if (is_lvl(level)) {
    vprintf(str, va);
    fflush(stdout);
  }
}

void Log::print(log_level_e level, const char *str, ...)
{
  va_list va;
  va_start(va, str);
  this->print(level, str, va);
  va_end(va);
}

// void Log::DumpHex(const void* data, size_t size) {
// 	char ascii[17];
// 	size_t i, j;
// 	ascii[16] = '\0';
// 	for (i = 0; i < size; ++i) {
// 		printf("%02X ", ((unsigned char*)data)[i]);
// 		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
// 			ascii[i % 16] = ((unsigned char*)data)[i];
// 		} else {
// 			ascii[i % 16] = '.';
// 		}
// 		if ((i+1) % 8 == 0 || i+1 == size) {
// 			printf(" ");
// 			if ((i+1) % 16 == 0) {
// 				printf("|  %s \n", ascii);
// 			} else if (i+1 == size) {
// 				ascii[(i+1) % 16] = '\0';
// 				if ((i+1) % 16 <= 8) {
// 					printf(" ");
// 				}
// 				for (j = (i+1) % 16; j < 16; ++j) {
// 					printf("   ");
// 				}
// 				printf("|  %s \n", ascii);
// 			}
// 		}
// 	}
// }

void Log::user(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  this->print(LOG_INFO, str, args);
  va_end(args);
}

void Log::detail(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  this->print(LOG_DETAIL, str, args);
  va_end(args);
}

void Log::protocol(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  this->print(LOG_PROTOCOL, str, args);
  va_end(args);
}

void Log::debug(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  this->print(LOG_DEBUG, str, args);
  va_end(args);
}

void Log::warning(const char *str, ...)
{
  va_list args;
  va_start(args, str);
  this->print(LOG_WARNING, str, args);
  va_end(args);
}

void Log::error(const char *str, ...)
{
  char buf[MAX_LOG_LINE];
  va_list va;
  va_start(va, str);
  vsnprintf(buf, MAX_LOG_LINE, str, va);
  va_end(va);
  {
    std::unique_lock<std::mutex> lck(m_last_error);
    strncpy(last_error, buf, MAX_LOG_LINE);
  }
  if (this->log_level <= LOG_ERROR) return;
  va_start(va, str);
  vfprintf(stderr, str, va);
  va_end(va);
}

extern "C" int get_max_log_level()
{
  return LOG_LEVEL_MAX;
}
