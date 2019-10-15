/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SA
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
 * Authors: Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H
#ifdef _WIN32
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
  #include <Windef.h>
  #include <windows.h>
  typedef int port_t;
  typedef SOCKET socket_t;
  typedef int func_ret_t;
  typedef int socklen_t;
  #define LST_SHUT_WR SD_SEND
  #define SOCKERR_WOULDBLOCK WSAEWOULDBLOCK
  #define SOCKERR_INPROGRESS WSAEINPROGRESS
  #define CSOCKERRNO (WSAGetLastError())
  #define SOCKERRNO (::WSAGetLastError())
#else
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <errno.h>
  typedef int port_t;
  typedef int socket_t;
  #define INVALID_SOCKET -1
  typedef int func_ret_t;
  #define SOCKET_ERROR -1
  #define LST_SHUT_WR SHUT_WR
  #define SOCKERR_WOULDBLOCK EWOULDBLOCK
  #define SOCKERR_INPROGRESS EINPROGRESS
  #define CSOCKERRNO errno
  #define SOCKERRNO errno
#endif
#endif