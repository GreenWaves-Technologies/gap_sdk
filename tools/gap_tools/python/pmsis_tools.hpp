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

#ifndef __PMSIS_TOOLS_HPP__
#define __PMSIS_TOOLS_HPP__

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
#include <tools/services.h>



typedef struct 
{
  uint32_t channel;
  uint32_t packet_size;
  uint32_t info;
} Transport_req_t;



class Transport_channel
{
public:
  virtual void handle_req(Transport_req_t *req) = 0;
  virtual void close() = 0;
};



class Transport_service
{
public:
  virtual void handle_connect(Transport_req_t *req) = 0;
};



class Transport
{
public:
  void start();
  virtual int recv(void *buffer, int size) = 0;
  void register_service(int channel, Transport_service *transport_service);
  void register_channel(int channel, Transport_channel *transport_channel);

private:
  void handle_req(int channel, Transport_req_t *req);
  void reader();
  std::map<int, Transport_channel *> channels;
  std::map<int, Transport_service *> services;
  std::thread *reader_thread;
};



class Transport_socket : public Transport
{
public:
  int open();
  Transport_socket(int port);
  int recv(void *buffer, int size);

private:
  void listener();
  int port;
  std::thread *listener_thread;
  int proxy_socket_in;
  int client;
};



class Frame_stream;

class Frame_streamer : public Transport_service
{
public:
  Frame_streamer(Transport *transport, int show);
  void *get_frame();
  void handle_connect(Transport_req_t *req);
  void *get_stream(char *name, int *width, int *height, int *depth);

private:
  Transport *transport;
  std::map<std::string, Frame_stream *> streams;
  int show;
  std::mutex mutex;
  std::condition_variable cond;
};


#endif