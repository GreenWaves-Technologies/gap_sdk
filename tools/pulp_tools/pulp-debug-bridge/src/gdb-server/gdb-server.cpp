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
 * Authors: Andreas Traber
 */

#include "cable.hpp"
#include "gdb-server/gdb-server.hpp"
#include <stdarg.h>

Gdb_server::Gdb_server(const EventLoop::SpEventLoop &event_loop, const std::shared_ptr<Cable> &cable, js::config *config, int socket_port,
    const char * capabilities)
: log("GDBS"), m_event_loop(std::move(event_loop)), cable(std::move(cable)), config(config),
  m_socket_port(socket_port), capabilities(capabilities)
{
}

void Gdb_server::start() {
  target = std::make_shared<Target>(this);

  bkp = std::make_shared<Breakpoints>(this);

  rsp = std::make_shared<Rsp>(this, m_socket_port, m_event_loop);

  rsp->start();
}
 
void Gdb_server::target_update_power() {
  target->update_power();
}

void Gdb_server::signal_exit(int status) {
  rsp->signal_exit(status);
}

void Gdb_server::refresh_target()
{
  target->reinitialize();
  target->update_power();
  bkp->enable_all();
}

int Gdb_server::stop()
{
  if (rsp != NULL)
  {
    rsp->stop();
    rsp = NULL;
  }
  return 1;
}

void Gdb_server::abort()
{
  if (rsp != NULL)
  {
    rsp->abort();
    rsp = NULL;
  }
}

void Gdb_server::print(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);
}
