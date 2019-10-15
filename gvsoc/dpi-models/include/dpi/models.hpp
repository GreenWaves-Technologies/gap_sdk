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

#ifndef __DPI_MODELS_HPP__
#define __DPI_MODELS_HPP__

#include <json.hpp>
#include <map>

#ifdef USE_DPI
#include "questa/dpiheader.h"
#endif



class Dpi_itf
{
  public:
    void bind(void *handle);

  protected:
    void *sv_handle;
};



class Qspi_itf : public Dpi_itf
{
  public:
    virtual void sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask) {};
    virtual void edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask) {};
    virtual void cs_edge(int64_t timestamp, int cs) {};
    void set_data(int data_0);
    void set_qpi_data(int data_0, int data_1, int data_2, int data_3, int mask);
};



class Gpio_itf : public Dpi_itf
{
  public:
    virtual void edge(int64_t timestamp, int data) {}
    void set_data(int data);
};



class Jtag_itf : public Dpi_itf
{
  public:
    void tck_edge(int tck, int tdi, int tms, int trst, int *tdo);
};



class Uart_itf : public Dpi_itf
{
  public:
    virtual void tx_edge(int64_t timestamp, int data) {}
    void rx_edge(int data);
};



class I2c_itf : public Dpi_itf
{
  public:
    virtual void tx_edge(int64_t timestamp, int scl, int sda) {}
    void rx_edge(int sda);
};



class I2s_itf : public Dpi_itf
{
  public:
    virtual void edge(int64_t timestamp, int sck, int ws, int sd) {}
    void rx_edge(int sck, int ws, int sd);
};



class Cpi_itf : public Dpi_itf
{
  public:
    virtual void edge(int64_t timestamp, int pclk, int hsync, int vref, int data) {}
    void edge(int pclk, int hsync, int vref, int data);
};




class Ctrl_itf : public Dpi_itf
{
  public:
    void reset_edge(int reset);
    void config_edge(uint32_t config);
};



class Dpi_handler {
public:
  Dpi_handler(void *arg0, void *arg1, int64_t time) : arg0(arg0), arg1(arg1), time(time) {}
  void *arg0;
  void *arg1;
  int64_t time;
  Dpi_handler *next;
};


class Dpi_model
{
public:
  Dpi_model(js::config *config, void *handle);
  void *bind_itf(std::string name, void *handle);
  void create_itf(std::string name, Dpi_itf *itf);
  void create_task(void *arg1, void *arg2);
  void create_periodic_handler(int64_t period, void *arg1, void *arg2);
  void create_delayed_handler(int64_t period, void *arg1, void *arg2);
  static void callback_task_stub(void *__this);
  void callback_task();
  void wait(int64_t ns);
  void wait_ps(int64_t ps);
  void wait_event();
  void raise_event();
  void wait_task_event();
  void wait_task_event_timeout(int64_t timeout);
  void raise_task_event();
  void raise_event_from_ext();
  virtual void start() {};
  void start_all();

protected:
  void *trace_new(const char *name);
  void trace_msg(void *trace, int level, const char *format, ...);
  void print(const char *format, ...);
  void fatal(const char *format, ...);
  js::config *get_config();

private:
  js::config *config;
  std::map<std::string, Dpi_itf *> itfs;
  void *handle;
  Dpi_handler *first_handler;
};

typedef enum
{
  I2C_SLAVE_STATE_WAIT_START,
  I2C_SLAVE_STATE_WAIT_ADDRESS,
  I2C_SLAVE_STATE_GET_DATA,
  I2C_SLAVE_STATE_ACK
} I2c_slave_state_e;

class I2c_slave
{
public:
  I2c_slave(unsigned int address);
  virtual void start(unsigned int address, bool is_read) {}
  virtual void handle_byte(uint8_t byte) {}
  virtual void stop() {}
  virtual void ack() {}

  void send_byte(uint8_t byte);
  void send_ack();

  void handle_edge(int scl, int sda_int, int *sda_out);

private:
  unsigned int address;
  uint8_t pending_data;
  bool is_read;
  I2c_slave_state_e state;
  int pending_bits;
  int prev_sda;
  unsigned int pending_send_byte;
  bool pending_send_ack;
};



#endif