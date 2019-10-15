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

#include "dpi/models.hpp"
#include <stdint.h>

class Uart_tb;

class Uart_tb_uart_itf : public Uart_itf
{
public:
  Uart_tb_uart_itf(Uart_tb *top) : top(top) {}
  void tx_edge(int64_t timestamp, int data);

private:
    Uart_tb *top;
};

class Uart_tb : public Dpi_model
{
public:
  Uart_tb(js::config *config, void *handle);

  void tx_edge(int64_t timestamp, int tx);
  void tx_sampling();

  void start();

private:

  void dpi_task();
  static void dpi_uart_task_stub(Uart_tb *);
  void start_tx_sampling(int baudrate);
  void stop_tx_sampling();

  int period;
  bool wait_start = true;
  bool wait_stop = false;
  int current_tx;
  int baudrate;
  int nb_bits;
  bool loopback;
  bool stdout;
  uint8_t byte;
  FILE *tx_file = NULL;

  Uart_itf *uart;
};

void Uart_tb::start()
{
  period = 0;
  create_task((void *)&Uart_tb::dpi_uart_task_stub, this);
}

void Uart_tb::dpi_uart_task_stub(Uart_tb *_this)
{
  _this->dpi_task();
}

Uart_tb::Uart_tb(js::config *config, void *handle) : Dpi_model(config, handle)
{
  baudrate = config->get("baudrate")->get_int();
  loopback = config->get("loopback")->get_bool();
  stdout = config->get("stdout")->get_bool();
  std::string tx_filename = config->get("tx_file")->get_str();
  print("Instantiated uart model (baudrate: %d, loopback: %d, stdout: %d, tx_file: %s)", baudrate, loopback, stdout, tx_filename.c_str());
  if (tx_filename != "")
  {
    tx_file = fopen(tx_filename.c_str(), (char *)"w");
    if (tx_file == NULL)
    {
      print("Unable to open TX log file: %s", strerror(errno));
    }
  }
  uart = new Uart_tb_uart_itf(this);
  create_itf("uart", static_cast<Uart_itf *>(uart));
}

void Uart_tb::tx_sampling()
{
  print("Sampling bit (value: %d)", current_tx);

  if (wait_stop)
  {
    if (current_tx == 1)
    {
      print("Received stop bit");
      wait_start = true;
      wait_stop = false;
      this->stop_tx_sampling();
    }
  }
  else
  {
    print("Received data bit (data: %d)", current_tx);
    byte = (byte >> 1) | (current_tx << 7);
    nb_bits++;
    if (nb_bits == 8) {
      print("Sampled TX byte (value: 0x%x)", byte);
      if (stdout) printf("%c", byte);
      if (tx_file) {
        fwrite((void *)&byte, 1, 1, tx_file);
      }
      print("Waiting for stop bit");
      wait_stop = true;
    }
  }
}

void Uart_tb::tx_edge(int64_t timestamp, int tx)
{
  if (loopback) uart->rx_edge(tx);

  current_tx = tx;
  
  if (wait_start && tx == 0)
  {
    print("Received start bit");
    this->start_tx_sampling(baudrate);
    wait_start = false;
    nb_bits = 0;
  }
}

void Uart_tb::start_tx_sampling(int baudrate)
{
  period = 1000000000000/baudrate;
  this->raise_event();
}

void Uart_tb::stop_tx_sampling()
{
  period = 0;
}

void Uart_tb::dpi_task()
{
  while(1)
  {
    while(period == 0)
    { 
      this->wait_event();
    }

    wait_ps(period/2);

    while(period != 0)
    {
      this->wait_ps(period);
      this->tx_sampling();
    }
  }
}


void Uart_tb_uart_itf::tx_edge(int64_t timestamp, int data)
{
  top->tx_edge(timestamp, data);
}

extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new Uart_tb(config, handle);
}
