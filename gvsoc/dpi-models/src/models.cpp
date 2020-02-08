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

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>

#include <json.hpp>
#include <dlfcn.h>

#include "dpi/models.hpp"

static std::vector<void *> tasks_cb;
static std::vector<void *> tasks_arg;
static std::vector<void *> handlers_cb;
static std::vector<void *> handlers_arg;

static void dpi_fatal_stub(void *handle, const char *format, ...)
{
  int size = 1024;
  while(1)
  {
    char str[size];
    va_list ap;
    va_start(ap, format);
    int iter_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (iter_size <= size)
    {
      dpi_fatal(handle, str);
      break;
    }
    size = iter_size;
  }
}

static void dpi_print_stub(void *handle, const char *format, ...)
{
  int size = 1024;
  while(1)
  {
    char str[size];
    va_list ap;
    va_start(ap, format);
    int iter_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (iter_size <= size)
    {
      dpi_print(handle, str);
      break;
    }
    size = iter_size;
  }
}

void Dpi_model::print(const char *format, ...)
{
  int size = 1024;
  while(1)
  {
    char str[size];
    va_list ap;
    va_start(ap, format);
    int iter_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (iter_size <= size)
    {
      dpi_print(handle, str);
      break;
    }
    size = iter_size;
  }
}

void *Dpi_model::trace_new(const char *name)
{
  return dpi_trace_new(this->handle, name);
}

void Dpi_model::trace_msg(void *trace, int level, const char *format, ...)
{
  int size = 1024;
  while(1)
  {
    char str[size];
    va_list ap;
    va_start(ap, format);
    int iter_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (iter_size <= size)
    {
      dpi_trace_msg(trace, level, str);
      break;
    }
    size = iter_size;
  }
}

void Dpi_model::fatal(const char *format, ...)
{
  int size = 1024;
  while(1)
  {
    char str[size];
    va_list ap;
    va_start(ap, format);
    int iter_size = vsnprintf(str, size, format, ap);
    va_end(ap);
    if (iter_size <= size)
    {
      dpi_fatal(handle, str);
      break;
    }
    size = iter_size;
  }
}

void Dpi_model::create_task(void *arg1, void *arg2)
{
  int task_id = tasks_cb.size();
  tasks_cb.push_back(arg1);
  tasks_arg.push_back(arg2);
  dpi_create_task(handle, task_id);
}

void Dpi_model::create_periodic_handler(int64_t period, void *arg1, void *arg2)
{
  int handler_id = handlers_cb.size();
  handlers_cb.push_back(arg1);
  handlers_arg.push_back(arg2);
  dpi_create_periodic_handler(handle, handler_id, period);
}

void Dpi_model::create_delayed_handler(int64_t period, void *arg1, void *arg2)
{

  Dpi_handler *handler = new Dpi_handler(arg1, arg2, period + dpi_time(this->handle));

  int64_t full_time = period + dpi_time(this->handle);

  Dpi_handler *current = this->first_handler;
  Dpi_handler *prev = NULL;

  while (current && current->time < full_time)
  {
    prev = current;
    current = current->next;
  }

  if (prev)
    prev->next = handler;
  else
    this->first_handler = handler;

  handler->next = current;

  this->raise_task_event();
}

int dpi_start_task(int id)
{
  ((void (*)(void *))tasks_cb[id])(tasks_arg[id]);
  return 0;
}

void dpi_exec_periodic_handler(int id)
{
  ((void (*)(void *))handlers_cb[id])(handlers_arg[id]);
}

void Dpi_model::callback_task_stub(void *__this)
{
  Dpi_model *_this = (Dpi_model *)__this;
  _this->callback_task();
}

void Dpi_model::callback_task()
{
  while(1)
  {
    Dpi_handler *current = this->first_handler;

    if (!current)
    {
      this->wait_task_event();
    }
    else
    {
      int64_t time = dpi_time(this->handle);

      while (current && time >= current->time)
      {
        print("Executing delayed handler");
        this->first_handler = current->next;

        ((void (*)(void *))current->arg0)(current->arg1);

        delete current;
        current = this->first_handler;
      }


      if (this->first_handler)
      {
        print("Waiting for next delayed handler (time: %ld)", this->first_handler->time);
        this->wait_task_event_timeout(this->first_handler->time - time);
      }
    }
  }
}

Dpi_model::Dpi_model(js::config *config, void *handle)
 : config(config), handle(handle), first_handler(NULL)
{
}

void Dpi_model::start_all()
{
  this->start();
  create_task((void *)&Dpi_model::callback_task_stub, this);
}

void Dpi_model::stop_all()
{
  this->stop();
}

void Dpi_model::wait(int64_t ns)
{
  dpi_wait(handle, ns);
}

void Dpi_model::wait_ps(int64_t ps)
{
  dpi_wait_ps(handle, ps);
}

void Dpi_model::wait_event()
{
  dpi_wait_event(handle);
}

void Dpi_model::wait_task_event()
{
  dpi_wait_task_event(handle);
}

void Dpi_model::wait_task_event_timeout(int64_t timeout)
{
  dpi_wait_task_event_timeout(handle, timeout);
}

void Dpi_model::raise_task_event()
{
  dpi_raise_task_event(handle);
}

// This function is only useful on virtual platform to avoid active polling
// between the a task and a thread
void Dpi_model::raise_event()
{
  // TODO
  // Dirty hack to not call system verilog task from pthread on RTL platform
  if (handle)
    dpi_raise_event(handle);
}

void Dpi_model::raise_event_from_ext()
{
  // TODO
  // Dirty hack to not call system verilog task from pthread on RTL platform
  if (handle)
    dpi_raise_event_from_ext(handle);
}


void Dpi_itf::bind(void *handle)
{
  sv_handle = handle;
}

void *Dpi_model::bind_itf(std::string name, void *handle)
{
  if (itfs.find(name) == itfs.end())
    return NULL;
  itfs[name]->bind(handle);
  return (void *)itfs[name];
}

void Dpi_model::create_itf(std::string name, Dpi_itf *itf)
{
  itfs[name] = itf;
}

js::config *Dpi_model::get_config()
{
  return config;
}

extern "C" void *model_load(void *_config, void *handle)
{
  js::config *config = (js::config *)_config;

  js::config *module_config = config->get("module");

  if (module_config == NULL)
  {
    dpi_fatal_stub(handle, "ERROR, Failed to open periph model, didn't find configuration item 'model'");
    return NULL; 
  }

  std::string module_name = module_config->get_str();

  void *module = dlopen(module_name.c_str(), RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
  if (module == NULL)
  {
    dpi_fatal_stub(handle, "ERROR, Failed to open periph model (%s) with error: %s", module_name.c_str(), dlerror());
    return NULL;
  }

  Dpi_model *(*model_new)(js::config *, void *) = (Dpi_model *(*)(js::config *, void *))dlsym(module, "dpi_model_new");
  if (model_new == NULL)
  {
    dpi_fatal_stub(handle, "ERROR, invalid DPI model being loaded (%s)", module_name.c_str());
    return NULL;
  }

  void *result =  model_new(config, handle);
  return result;
}

I2c_slave::I2c_slave(unsigned int address)
 : address(address)
{
  this->state = I2C_SLAVE_STATE_WAIT_START;
  this->prev_sda = 1;
  this->pending_send_ack = false;
}

void I2c_slave::send_byte(uint8_t byte)
{
  this->pending_send_byte = byte;
}

void I2c_slave::send_ack()
{
  this->pending_send_ack = true;
}

void I2c_slave::handle_edge(int scl, int sda_in, int *sda_out)
{
  if (scl == 1 && this->prev_sda != sda_in)
  {
    if (this->prev_sda == 1)
    {
      this->state = I2C_SLAVE_STATE_WAIT_ADDRESS;
      this->address = 0;
      this->pending_bits = 8;
    }
    else
    {
      this->state = I2C_SLAVE_STATE_WAIT_START;
      this->stop();
    }
    goto end;
  }

  if (scl == 0)
  {
    if (this->pending_send_ack)
    {
      this->pending_send_ack = false;
      *sda_out = 1;
    }
    goto end;
  }

  switch (this->state)
  {
    case I2C_SLAVE_STATE_WAIT_ADDRESS: {
      if (this->pending_bits > 1)
        this->address = (this->address << 1) | sda_in;
      else
        this->is_read = sda_in;
      this->pending_bits--;
      if (this->pending_bits == 0)
      {
        this->start(this->address, this->is_read);
        this->state = I2C_SLAVE_STATE_ACK;
        this->pending_bits = 8;
      }
      break;
    }
    case I2C_SLAVE_STATE_GET_DATA: {

      if (sda_out)
      {
        *sda_out = (this->pending_send_byte >> 7) & 1;
        this->pending_send_byte <<= 1;
      }

      this->pending_data = (this->pending_data << 1) | sda_in;
      this->pending_bits--;
      if (this->pending_bits == 0)
      {
        this->pending_bits = 8;
        this->handle_byte(this->pending_data);
        this->state = I2C_SLAVE_STATE_ACK;
      }
      break;
    }
    case I2C_SLAVE_STATE_ACK: {
      this->ack();
      this->state = I2C_SLAVE_STATE_GET_DATA;
      break;
    }

  }

end:
  this->prev_sda = sda_in;
}
