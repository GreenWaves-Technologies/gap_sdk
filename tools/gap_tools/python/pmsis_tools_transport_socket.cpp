/*
 * Copyright (C) 2019 GreenWaves Technologies
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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "pmsis_tools.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <mutex>
#include <queue>
#include <map>
#include <condition_variable>


int sock_recv(int sock, char *ptr, int size)
{
  while(size > 0)
  {
    int ret = ::recv(sock, (void *)ptr, size, 0);
    if (ret <= 0)
    {
      return -1;
    }

    ptr += ret;
    size -= ret;
  }

  return 0;
}



int Transport_socket::recv(void *buffer, int size)
{
  return sock_recv(this->client, (char *)buffer, size);
}




Transport_socket::Transport_socket(int port)
: port(port)
{
}



void Transport_socket::listener()
{
  int client;

  fprintf(stderr, "Proxy listening on port %d\n", this->port);

  while(1) {
    if((this->client = accept(this->proxy_socket_in, NULL, NULL)) == -1) {
      if(errno == EAGAIN) continue;

      fprintf(stderr, "Unable to accept connection: %s\n", strerror(errno));
      return;
    }
    printf("Proxy: Client connected!\n");
    this->start();
  }

  return;
}



int Transport_socket::open()
{
  struct sockaddr_in addr;
  int yes = 1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  this->proxy_socket_in = socket(PF_INET, SOCK_STREAM, 0);
  if(this->proxy_socket_in < 0)
  {
    fprintf(stderr, "Unable to create comm socket: %s\n", strerror(errno));
    return -1;
  }

  if(setsockopt(proxy_socket_in, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    fprintf(stderr, "Unable to setsockopt on the socket: %s\n", strerror(errno));
    return -1;
  }

  if(bind(proxy_socket_in, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    fprintf(stderr, "Unable to bind the socket: %s\n", strerror(errno));
    return -1;
  }

  if(listen(proxy_socket_in, 1) == -1) {
    fprintf(stderr, "Unable to listen: %s\n", strerror(errno));
    return -1;
  }

  this->listener_thread = new std::thread(&Transport_socket::listener, this);

  return 0;
}


extern "C" void *transport_socket_open(int port)
{
  Transport_socket *transport = new Transport_socket(port);
  if (transport->open())
    return NULL;

  return (void *)static_cast<Transport *>(transport);
}
