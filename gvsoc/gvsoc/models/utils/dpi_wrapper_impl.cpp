/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <stdio.h>
#include <string.h>
#include "dpi/tb_driver.h"
#include "vp/itf/qspim.hpp"
#include "vp/itf/jtag.hpp"
#include "vp/itf/uart.hpp"
#include "vp/itf/i2c.hpp"
#include "vp/itf/cpi.hpp"
#include "vp/itf/wire.hpp"
#include "vp/itf/i2s.hpp"
#include <vector>
#include <thread>
#include <unistd.h>
#include <ucontext.h>


typedef struct {
  void *handle;
  vp::trace tx_trace;
} uart_handle_t;

typedef struct {
  void *handle;
  vp::trace tx_trace;
} i2s_handle_t;

typedef struct {
  void *handle;
  vp::i2c_slave itf;
  vp::trace scl_trace;
  vp::trace sda_trace;
} i2c_handle_t;

typedef struct {
  void *handle;
  vp::wire_slave<bool> itf;
  vp::trace trace;
} gpio_handle_t;

typedef struct {
  void *handle;
  vp::qspim_slave *itf;
  vp::wire_slave<bool> *cs_itf;
} qspim_handle_t;

typedef struct {
  void *handle;
  vp::trace trace;
} cpi_handle_t;

typedef struct {
  void *handle;
  vp::wire_master<bool> *itf;
  vp::wire_master<uint32_t> *config_itf;
} ctrl_handle_t;

class dpi_wrapper;
class dpi_task;

static vector<vp::jtag_master *> jtag_masters;

static vector<ctrl_handle_t *> ctrl_handles;

static vector<vp::uart_slave *> uart_slaves;
static vector<vp::i2s_master *> i2s_slaves;
static vector<uart_handle_t *> uart_handles;
static vector<i2s_handle_t *> i2s_handles;
static vector<qspim_handle_t *> qspim_handles;
static vector<i2c_handle_t *> i2c_handles;

static vector<vp::cpi_master *> cpi_masters;
static vector<cpi_handle_t *> cpi_handles;
static vector<gpio_handle_t *> gpio_handles;

static ucontext_t main_context;

static dpi_task *active_task;

class dpi_task
{
  friend class dpi_wrapper;

public:
  dpi_task(dpi_wrapper *top, int id) : top(top), id(id), is_started(false), event_raised(false) {}
  void start();
  void wait_event();
  void wait_ps(int64_t t);

protected:

private:
  static void wait_handler(void *__this, vp::clock_event *event);
  static void entry_stub(int id);

  dpi_wrapper *top;
  ucontext_t context;
  int id;
  vp::clock_event *wait_evt;
  bool is_started;
  bool event_raised;
  dpi_task *next;
};

class dpi_periodic_handler
{
  friend class dpi_wrapper;

public:
  dpi_periodic_handler(dpi_wrapper *top, int id, int64_t period) : top(top), id(id), period(period) {}
  void start();

protected:

private:
  static void entry_stub(void *__this, vp::clock_event *event);

  dpi_wrapper *top;
  int id;
  int64_t period;
  vp::clock_event *wait_evt;
};

class dpi_wrapper : public vp::component
{

public:

  dpi_wrapper(js::config *config);

  int build();
  void start();
  void stop();
  void create_task(int id);
  void create_periodic_handler(int id, int64_t period);
  int wait(int64_t t);
  int wait_ps(int64_t t);
  void wait_event();
  void raise_event();
  void raise_event_from_ext();
  vp::trace *get_trace() { return &trace; }
  void enqueue_waiting_for_event(dpi_task *task);

  vp::clock_event *delayed_evt;

private:

  static void delayed_handler(void *__this, vp::clock_event *event);

  static void qspim_sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  static void qspim_sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id);
  static void qspim_cs_sync(void *__this, bool active, int id);
  static void jtag_sync(void *__this, int tdo, int id);
  static void i2c_sync(void *__this, int scl, int sda, int id);
  static void i2c_sync_cycle(void *__this, int sda, int id);
  static void uart_sync(void *__this, int data, int id);
  static void i2s_sync(void *__this, int sck, int ws, int sd, int id);
  static void gpio_sync(void *__this, bool data, int id);
  static void cpi_sync(void *__this, int pclk, int href, int vsync, int data, int id);
  static void wakeup_handler(void *__this, vp::clock_event *event);

  vp::trace     trace;

  vector<std::thread *>threads;
  vector<void *>models;
  vector<dpi_task *> tasks;
  vector<dpi_periodic_handler *> handlers;
  dpi_task * first_waiting_task = NULL;
  vp::wire_master<bool> chip_reset_itf;
  vp::wire_master<uint32_t> chip_config_itf;

  bool event_raised = false;
  vp::clock_event *wakeup_evt;
};

void dpi_task::wait_ps(int64_t t)
{
  int64_t period = this->top->get_period();
  int64_t cycles = (t + period - 1) / period;
  this->top->event_enqueue(this->wait_evt, cycles);
  swapcontext(&this->context, &main_context);
}


void dpi_task::wait_event()
{
  top->enqueue_waiting_for_event(this);
  swapcontext(&this->context, &main_context);
}

void dpi_task::wait_handler(void *__this, vp::clock_event *event)
{
  dpi_task *_this = (dpi_task *)__this;
  active_task = _this;
  swapcontext(&main_context, &_this->context);
}

void dpi_task::entry_stub(int id)
{
  dpi_start_task(id);
}

void dpi_task::start()
{
  this->wait_evt = top->event_new(this, dpi_task::wait_handler);
  top->event_enqueue(this->wait_evt, 0);

  getcontext(&this->context);

  this->context.uc_stack.ss_sp = malloc(65536);
  this->context.uc_stack.ss_size = 65536;
  this->context.uc_link = &main_context;

  makecontext(&this->context, (void (*)())dpi_task::entry_stub, 1, this->id);
}


void dpi_periodic_handler::start()
{
  this->wait_evt = top->event_new(this, dpi_periodic_handler::entry_stub);
  this->top->event_enqueue(this->wait_evt, 1);
}

void dpi_periodic_handler::entry_stub(void *__this, vp::clock_event *event)
{
  dpi_periodic_handler *_this = (dpi_periodic_handler *)__this;
  _this->top->event_enqueue(_this->wait_evt, 1);
  dpi_exec_periodic_handler(_this->id);
}


dpi_wrapper::dpi_wrapper(js::config *config)
: vp::component(config)
{
  this->wakeup_evt = this->event_new(this, dpi_wrapper::wakeup_handler);
  this->delayed_evt = this->event_new(this, dpi_wrapper::delayed_handler);
}


void dpi_wrapper::delayed_handler(void *__this, vp::clock_event *event)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  _this->raise_event();
}

void dpi_wrapper::wakeup_handler(void *__this, vp::clock_event *event)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  _this->raise_event();
}

void dpi_wrapper::enqueue_waiting_for_event(dpi_task *task)
{
  task->next = this->first_waiting_task;
  this->first_waiting_task = task;
}

int dpi_wrapper::wait(int64_t t)
{
  active_task->wait_ps(t*1000);
  return 0;
}

int dpi_wrapper::wait_ps(int64_t t)
{
  active_task->wait_ps(t);
  return 0;
}

void dpi_wrapper::wait_event()
{
  active_task->wait_event();
}

void dpi_wrapper::raise_event()
{
  dpi_task *current = this->first_waiting_task;
  this->first_waiting_task = NULL;
  while (current)
  {
    dpi_task *next = current->next;

    active_task = current;
    swapcontext(&main_context, &current->context);

    current = next;
  }
}

void dpi_wrapper::raise_event_from_ext()
{
  while(this->get_clock() == NULL)
    pthread_yield();

  this->get_clock()->get_engine()->lock();
  if (!this->wakeup_evt->is_enqueued())
    this->event_enqueue(this->wakeup_evt, 0);
  this->get_clock()->get_engine()->unlock();
}

void dpi_wrapper::create_task(int id)
{
  this->tasks.push_back(new dpi_task(this, id));
}

void dpi_wrapper::create_periodic_handler(int id, int64_t period)
{
  this->handlers.push_back(new dpi_periodic_handler(this, id, period));
}

void dpi_wrapper::jtag_sync(void *__this, int tdo, int id)
{
  vp::jtag_master *itf = jtag_masters[id];
  itf->tdo = tdo;
}

void dpi_wrapper::qspim_sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{
  printf("%s %d\n", __FILE__, __LINE__);
}

void dpi_wrapper::qspim_sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  dpi_qspim_edge(qspim_handles[id]->handle, _this->get_clock()->get_time(), data_0, data_1, data_2, data_3, mask);
}

void dpi_wrapper::qspim_cs_sync(void *__this, bool active, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  dpi_qspim_cs_edge(qspim_handles[id]->handle, _this->get_clock()->get_time(), active);
}

void dpi_wrapper::uart_sync(void *__this, int data, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  uart_handles[id]->tx_trace.event((uint8_t *)&data);
  dpi_uart_edge(uart_handles[id]->handle, _this->get_clock()->get_time(), data);
}

void dpi_wrapper::i2s_sync(void *__this, int sck, int ws, int sd, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  //i2s_handles[id]->tx_trace.event((uint8_t *)&data);
  dpi_i2s_edge(i2s_handles[id]->handle, _this->get_clock()->get_time(), sck, ws, sd);
}

void dpi_wrapper::gpio_sync(void *__this, bool data, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  gpio_handles[id]->trace.event((uint8_t *)&data);
  dpi_gpio_edge(gpio_handles[id]->handle, _this->get_clock()->get_time(), data);
}

void dpi_wrapper::i2c_sync(void *__this, int scl, int sda, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  i2c_handles[id]->sda_trace.event((uint8_t *)&sda);
  dpi_i2c_edge(i2c_handles[id]->handle, _this->get_clock()->get_time(), scl, sda);
}

void dpi_wrapper::i2c_sync_cycle(void *__this, int sda, int id)
{
  dpi_wrapper *_this = (dpi_wrapper *)__this;
  i2c_handles[id]->sda_trace.event((uint8_t *)&sda);
  dpi_i2c_edge(i2c_handles[id]->handle, _this->get_clock()->get_time(), 0, sda);
}

int dpi_wrapper::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);


  this->new_master_port("chip_reset", &this->chip_reset_itf);
  this->new_master_port("chip_config", &this->chip_config_itf);

  void *config_handle = dpi_config_get_from_file(getenv("PULP_CONFIG_FILE"));

  if (config_handle == NULL) return 0;

  void *driver_handle = dpi_driver_set_config(config_handle);
  int nb_comp = dpi_driver_get_nb_comp(driver_handle);

  for(int i = 0; i < nb_comp; i++)
  {
    const char *comp_name = dpi_driver_get_comp_name(driver_handle, i);
    js::config *comp_config = (js::config *)dpi_driver_get_comp_config(driver_handle, i);
    std::string comp_type = comp_config->get_child_str("type");
    int nb_itf = dpi_driver_get_comp_nb_itf(comp_config, i);

    this->trace.msg("Found TB driver component (index: %d, name: %s, type: %s, nb_itf: %d)\n", i, comp_name, comp_type.c_str(), nb_itf);

    if (comp_type == "dpi")
    {
//      dpi_models::periph_wrapper i_comp = new();
//      int err;
//
//      $display("[TB] %t - Instantiating DPI component", $realtime, i);

      this->trace.msg("Instantiating DPI component\n");

      void *dpi_model = dpi_model_load(comp_config, (void *)this);
      if (dpi_model == NULL)
      {
        snprintf(vp_error, VP_ERROR_SIZE, "Failed to instantiate DPI model\n");
        return -1;
      }
//      err = i_comp.load_model(comp_config);
//      if (err != 0) $fatal(1, "[TB] %t - Failed to instantiate periph model", $realtime);
//
      models.push_back(dpi_model);

      for(int j = 0; j < nb_itf; j++)
      {
        const char *itf_type;
        const char *itf_name;
        int itf_id;
        int itf_sub_id;
        dpi_driver_get_comp_itf_info(comp_config, i, j, &itf_name, &itf_type, &itf_id, &itf_sub_id);

        this->trace.msg("Got interface information (index: %d, name: %s, type: %s, id: %d, sub_id: %d)\n", j, itf_name, itf_type, itf_id, itf_sub_id);

        if (strcmp(itf_type, "QSPIM") == 0)
        {
          qspim_handle_t *handle = new qspim_handle_t;
          int id = qspim_handles.size();
          qspim_handles.push_back(handle);

          vp::qspim_slave *itf = new vp::qspim_slave();
          itf->set_sync_meth_muxed(&dpi_wrapper::qspim_sync, id);
          itf->set_sync_cycle_meth_muxed(&dpi_wrapper::qspim_sync_cycle, id);
          new_slave_port("spim" + std::to_string(itf_id) + "_cs" + std::to_string(itf_sub_id) + "_data", itf);

          vp::wire_slave<bool> *cs_itf = new vp::wire_slave<bool>();
          cs_itf->set_sync_meth_muxed(&dpi_wrapper::qspim_cs_sync, id);
          new_slave_port("spim" + std::to_string(itf_id) + "_cs" + std::to_string(itf_sub_id), cs_itf);

          handle->itf = itf;
          handle->cs_itf = cs_itf;

          handle->handle = dpi_qspim_bind(dpi_model, itf_name, qspim_handles.size()-1);
          if (handle->handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind QSPIM interface (name: %s)\n", itf_name);
            return -1;
          }

//            i_comp.qpim_bind(itf_name, qspi_infos[itf_id].itf, qspi_infos[itf_id].cs[itf_sub_id]);
        }
        else if (strcmp(itf_type, "JTAG") == 0)
        {
          vp::jtag_master *itf = new vp::jtag_master();
          itf->set_sync_meth_muxed(&dpi_wrapper::jtag_sync, itf_id);
          new_master_port(itf_name + std::to_string(itf_id), itf);
          jtag_masters.push_back(itf);
          void *handle = dpi_jtag_bind(dpi_model, itf_name, jtag_masters.size()-1);
          if (handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind JTAG interface (name: %s)\n", itf_name);
            return -1;
          }

        }
        else if (strcmp(itf_type, "UART") == 0)
        {
          vp::uart_slave *itf = new vp::uart_slave();
          itf->set_sync_meth_muxed(&dpi_wrapper::uart_sync, itf_id);
          new_slave_port(itf_name + std::to_string(itf_id), itf);
          uart_slaves.push_back(itf);
          uart_handle_t *handle = new uart_handle_t;
          handle->handle = dpi_uart_bind(dpi_model, itf_name, uart_handles.size());
          if (handle->handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind UART interface (name: %s)\n", itf_name);
            return -1;
          }
          uart_handles.push_back(handle);
          traces.new_trace_event(itf_name + std::to_string(itf_id) + "/tx", &handle->tx_trace, 1);

        }
        else if (strcmp(itf_type, "I2S") == 0)
        {
          vp::i2s_master *itf = new vp::i2s_master();
          itf->set_sync_meth_muxed(&dpi_wrapper::i2s_sync, itf_id);
          new_master_port(itf_name + std::to_string(itf_id), itf);
          i2s_slaves.push_back(itf);
          i2s_handle_t *handle = new i2s_handle_t;
          handle->handle = dpi_i2s_bind(dpi_model, itf_name, i2s_handles.size());
          if (handle->handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind I2S interface (name: %s)\n", itf_name);
            return -1;
          }
          i2s_handles.push_back(handle);
          traces.new_trace_event(itf_name + std::to_string(itf_id) + "/tx", &handle->tx_trace, 1);

        }
        else if (strcmp(itf_type, "I2C") == 0)
        {
          i2c_handle_t *handle = new i2c_handle_t;
          handle->itf.set_sync_meth_muxed(&dpi_wrapper::i2c_sync, itf_id);
          handle->itf.set_sync_cycle_meth_muxed(&dpi_wrapper::i2c_sync_cycle, itf_id);
          new_slave_port(itf_name + std::to_string(itf_id), &handle->itf);
          i2c_handles.reserve(itf_id + 1);
          i2c_handles[itf_id] = handle;

          handle->handle = dpi_i2c_bind(dpi_model, itf_name, itf_id);
          if (handle->handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind I2C interface (name: %s)\n", itf_name);
            return -1;
          }
          traces.new_trace_event(itf_name + std::to_string(itf_id) + "/tx", &handle->sda_trace, 1);

        }
        else if (strcmp(itf_type, "GPIO") == 0)
        {
          gpio_handle_t *handle = new gpio_handle_t;
          handle->itf.set_sync_meth_muxed(&dpi_wrapper::gpio_sync, itf_id);
          new_slave_port("gpio" + std::to_string(itf_id), &handle->itf);
          gpio_handles.reserve(itf_id + 1);
          gpio_handles[itf_id] = handle;

          handle->handle = dpi_gpio_bind(dpi_model, itf_name, itf_id);
          if (handle->handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind gpio interface (name: %s)\n", itf_name);
            return -1;
          }
          traces.new_trace_event(itf_name + std::to_string(itf_id), &handle->trace, 0);
        }
        else if (strcmp(itf_type, "CPI") == 0)
        {
          vp::cpi_master *itf = new vp::cpi_master();
          new_master_port(itf_name + std::to_string(itf_id), itf);
          cpi_masters.push_back(itf);
          void *handle = dpi_cpi_bind(dpi_model, itf_name, cpi_masters.size()-1);
          if (handle == NULL)
          {
            snprintf(vp_error, VP_ERROR_SIZE, "Failed to bind CPI interface (name: %s)\n", itf_name);
            return -1;
          }
        }
        else if (strcmp(itf_type, "CTRL") == 0)
        {
          ctrl_handle_t *handle = new ctrl_handle_t;
          handle->handle = dpi_ctrl_bind(dpi_model, itf_name, ctrl_handles.size());
          handle->itf = &this->chip_reset_itf;
          handle->config_itf = &this->chip_config_itf;
          ctrl_handles.push_back(handle);
//            i_comp.ctrl_bind(itf_name, ctrl_infos[itf_id].itf);
        }

      }
    }
  }

  return 0;
}

void dpi_wrapper::start()
{
  for (int i=0; i<models.size(); i++)
  {
    dpi_model_start(models[i]);
  }

  for (auto x: tasks)
  {
    x->start();
  }

  for (auto x: handlers)
  {
    x->start();
  }
}

void dpi_wrapper::stop()
{
  for (int i=0; i<models.size(); i++)
  {
    dpi_model_stop(models[i]);
  }
}

extern "C" void dpi_ctrl_reset_edge(void *_handle, int reset)
{
  ctrl_handle_t *handle = ctrl_handles[(int)(long)_handle];
  if (handle->itf->is_bound())
  {
    handle->itf->sync(reset);
  }
}

extern "C" void dpi_ctrl_config_edge(void *_handle, uint32_t config)
{
  ctrl_handle_t *handle = ctrl_handles[(int)(long)_handle];
  if (handle->config_itf->is_bound())
  {
    handle->config_itf->sync(config);
  }
}

extern "C" void dpi_jtag_tck_edge(void *handle, int tck, int tdi, int tms, int trst, int *tdo)
{
  vp::jtag_master *itf = jtag_masters[(int)(long)handle];
  if (itf->is_bound())
  {
    itf->sync(tck, tdi, tms, trst);
    *tdo = itf->tdo;
  }
}

extern "C" void dpi_uart_rx_edge(void *handle, int data)
{
  vp::uart_slave *itf = uart_slaves[(int)(long)handle];
  itf->sync(data);
}

extern "C" void dpi_i2c_rx_edge(void *handle, int sda)
{
  vp::i2c_slave *itf = &i2c_handles[(int)(long)handle]->itf;
  itf->sync(sda);
}

extern "C" void dpi_i2s_rx_edge(void *handle, int sck, int ws, int sd)
{
  vp::i2s_master *itf = i2s_slaves[(int)(long)handle];
  itf->sync(sck, ws, sd);
}

extern "C" void dpi_cpi_edge(void *handle, int pclk, int href, int vsync, int data)
{
  vp::cpi_master *itf = cpi_masters[(int)(long)handle];
  itf->sync(pclk, href, vsync, data);
}

extern "C" void dpi_print(void *data, const char *msg)
{
  dpi_wrapper *_this = (dpi_wrapper *)data;
  int len = strlen(msg);
  char buff[len + 2];
  strcpy(buff, msg);
  buff[len] = '\n';
  buff[len + 1] = 0;
  _this->get_trace()->msg(buff);
}

extern "C" void *dpi_trace_new(void *data, const char *name)
{
  dpi_wrapper *_this = (dpi_wrapper *)data;
  vp::trace *trace = new(vp::trace);
  _this->traces.new_trace(name, trace, vp::DEBUG);
  return (void *)trace;
}

extern "C" void dpi_trace_msg(void *data, int level, const char *msg)
{
  vp::trace *trace = (vp::trace *)data;
  int len = strlen(msg);
  char buff[len + 2];
  strcpy(buff, msg);
  buff[len] = '\n';
  buff[len + 1] = 0;
  trace->msg(level, buff);
}

extern "C" void dpi_fatal(void *data, const char *msg)
{
  dpi_wrapper *_this = (dpi_wrapper *)data;
  int len = strlen(msg);
  char buff[len + 2];
  strcpy(buff, msg);
  buff[len] = '\n';
  buff[len + 1] = 0;
  _this->get_trace()->fatal(buff);
}

extern "C" void dpi_wait_event(void *handle)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->wait_event();
}

extern "C" void dpi_raise_event(void *handle)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->raise_event();
}

extern "C" void dpi_raise_event_from_ext(void *handle)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->raise_event_from_ext();
}

extern "C" int dpi_wait(void *handle, int64_t t)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->wait(t);
}

extern "C" int dpi_wait_ps(void *handle, int64_t t)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->wait_ps(t);
}

extern "C" int64_t dpi_time(void *handle, int64_t t)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  return dpi->get_clock()->get_time();
}

extern "C" int dpi_wait_task_event(void *handle)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  dpi->wait_event();
  return 0;
}

extern "C" void dpi_raise_task_event(void *handle)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  dpi->raise_event();
}

extern "C" int dpi_wait_task_event_timeout(void *handle, int64_t t)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  int64_t period = dpi->get_period();
  int64_t cycles = (t + period - 1) / period;
  dpi->event_enqueue(dpi->delayed_evt, cycles);
  dpi->wait_event();
  return 0;
}

extern "C" void dpi_gpio_set_data(void *handle, int data)
{
  vp::wire_slave<bool> *itf = &gpio_handles[(int)(long)handle]->itf;
  itf->sync(data);
}

extern "C" void dpi_qspim_set_data(void *handle, int data)
{
  vp::qspim_slave *itf = qspim_handles[(int)(long)handle]->itf;
  itf->sync(0, data, 0, 0, 0x2);
}

extern "C" void dpi_qspim_set_qpi_data(void *handle, int data_0, int data_1, int data_2, int data_3, int mask)
{
  vp::qspim_slave *itf = qspim_handles[(int)(long)handle]->itf;
  itf->sync(data_0, data_1, data_2, data_3, mask);
}

extern "C" void dpi_create_task(void *handle, int id)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  dpi->create_task(id);
}

extern "C" void dpi_create_periodic_handler(void *handle, int id, int64_t period)
{
  dpi_wrapper *dpi = (dpi_wrapper *)handle;
  dpi->create_periodic_handler(id, period);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new dpi_wrapper(config);
}
