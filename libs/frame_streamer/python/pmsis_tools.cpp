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
#include <signal.h>
#include <stdexcept>


void Transport::register_service(int channel, Transport_service *transport_service)
{
  this->services[channel] = transport_service;
}



void Transport::register_channel(int channel, Transport_channel *transport_channel)
{
  this->channels[channel - TRANSPORT_USER_FIRST_CHANNEL] = transport_channel;
}



void Transport::handle_req(int channel, Transport_req_t *req)
{
  if (this->channels[channel] == NULL)
  {
    printf("Received invalid request (channel: %d)\n", channel);
    abort();
  }
  else
  {
    this->channels[channel]->handle_req(req);
  }
}



void Transport::start()
{
  this->reader_thread = new std::thread(&Transport::reader, this);
}

void Transport::reader()
{
  while(1) {

    Transport_req_t req;

    if (this->recv((void *)&req, sizeof(req)))
    {
      for (auto x: this->channels)
      {
        x.second->close();
        delete x.second;
      }
      this->channels.clear();
      return;
    }

    if (req.channel < TRANSPORT_USER_FIRST_CHANNEL)
    {
      Transport_service *service = this->services[req.channel];

      if (service != NULL)
      {
        service->handle_connect(&req);
      }
    }
    else
    {
      this->handle_req(req.channel - TRANSPORT_USER_FIRST_CHANNEL, &req);
    }
  }
}


static void init_sigint_handler(int s) {
  raise(SIGTERM);
  exit(-1);
}

extern "C" void pmsis_tools_init()
{
  // This should be the first C method called by python.
  // As python is not catching SIGINT where we are in C world, we have to
  // setup a  sigint handler to exit in case control+C is hit.
  signal (SIGINT, init_sigint_handler);
}