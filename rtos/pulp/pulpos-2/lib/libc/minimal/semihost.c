/*
 * Copyright (C) 2018 Greenwaves Technologies
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

#include "semihost.h"
#include "string.h"

// roughly this is the last stage of printf:
// print a string until '\0'
void pos_semihost_write0(const char *print_string)
{
    pos_semihost(SEMIHOSTING_SYS_WRITE0, (long) print_string);
}

int pos_semihost_open(const char *name, int mode)
{
    uint32_t len = strlen(name);
    uint32_t args[3] = {(uint32_t)name,mode,len};
    return pos_semihost(SEMIHOSTING_SYS_OPEN, (long) args);
}

int pos_semihost_close(int fd)
{
    //uint32_t args[3] = {name,mode,len};
    return pos_semihost(SEMIHOSTING_SYS_CLOSE, (long) fd);
}

int pos_semihost_read(int fd, uint8_t *buffer, int len)
{
    uint32_t args[3] = {(uint32_t)fd,(uint32_t)buffer,(uint32_t)len};
    return pos_semihost(SEMIHOSTING_SYS_READ, (long) args);
}

int pos_semihost_write(int fd, uint8_t *buffer, int len)
{
    uint32_t args[3] = {(uint32_t)fd,(uint32_t)buffer,(uint32_t)len};
    return pos_semihost(SEMIHOSTING_SYS_WRITE, (long) args);
}

int pos_semihost_seek(int fd, uint32_t pos)
{
    uint32_t args[2] = {(uint32_t)fd,pos};
    return pos_semihost(SEMIHOSTING_SYS_SEEK, (long) args);
}

int pos_semihost_flen(int fd)
{
    return pos_semihost(SEMIHOSTING_SYS_FLEN, (long) fd);
}

int pos_semihost_exit(int code)
{
    return pos_semihost(SEMIHOSTING_SYS_EXIT, (long) code);
}
