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
#include <vector>

#define SPIM_VERIF_CMD_BIT        56
#define SPIM_VERIF_CMD_WIDTH      8

#define SPIM_VERIF_CMD_INFO_BIT   32
#define SPIM_VERIF_CMD_INFO_WIDTH 24

#define SPIM_VERIF_CMD_ADDR_BIT   0
#define SPIM_VERIF_CMD_ADDR_WIDTH 32


typedef union {
  struct {
    unsigned int delay:32;
    unsigned int value:8;
    unsigned int id:16;
    unsigned int cmd:8;
  };
  uint64_t raw;
} cmd_gpio_set_t;


#define SPIM_VERIF_FIELD_GET(value,bit,width) (((value) >> (bit)) & ((1UL<<(width))-1))

typedef enum {
  STATE_GET_CMD,
  STATE_WRITE_CMD,
  STATE_FULL_DUPLEX_CMD,
  STATE_READ_CMD
} Spim_verif_state_e;

typedef enum {
  SPIM_VERIF_CMD_WRITE = 1,
  SPIM_VERIF_CMD_READ = 2,
  SPIM_VERIF_CMD_FULL_DUPLEX = 3,
  SPIM_VERIF_CMD_GPIO_SET = 4
} spim_cmd_e;




class Spim_verif;

class Spim_verif_qspi_itf : public Qspi_itf
{
public:
  Spim_verif_qspi_itf(Spim_verif *top) : top(top) {}
  void sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  void edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask);
  void cs_edge(int64_t timestamp, int cs);

private:
    Spim_verif *top;
};



class Spim_verif_gpio_itf : public Gpio_itf
{
public:
  Spim_verif_gpio_itf(Spim_verif *top) : top(top) {}
  void edge(int64_t timestamp, int data);

private:
    Spim_verif *top;
};


class Spim_verif_gpio_handler
{
public:
  Spim_verif_gpio_handler(Spim_verif *top, int gpio, int value);

  static void gpio_handler_stub(Spim_verif_gpio_handler *);

private:

  Spim_verif *top;
  int gpio;
  int value;
};


class Spim_verif : public Dpi_model
{
  friend class Spim_verif_qspi_itf;

public:
  Spim_verif(js::config *config, void *handle);

  void gpio_handler(int gpio, int value);

protected:

  void sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void cs_edge(int64_t timestamp, int cs);
  void handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);


private:

  Spim_verif_qspi_itf *qspi0;
  std::vector<Spim_verif_gpio_itf *> gpios;

  void handle_read(uint64_t cmd);
  void handle_write(uint64_t cmd);
  void handle_full_duplex(uint64_t cmd);
  void handle_gpio_set(uint64_t cmd);

  void exec_write(int data);
  void exec_read();
  void exec_dump_single(int sdio0);
  void exec_dump_qpi(int sdio0, int sdio1, int sdio2, int sdio3);

  void handle_command(uint64_t cmd);

  Spim_verif_state_e state = STATE_GET_CMD;
  uint64_t current_cmd = 0;
  int prev_sck = 0;
  int cmd_count = 0;
  int dummy_cycles = 0;
  bool wait_cs;
  int command_addr;
  int current_addr;
  int current_write_addr;
  int current_size;
  int current_write_size;
  unsigned char *data;
  int nb_bits;
  int nb_write_bits;
  uint32_t byte;
  bool verbose;
  unsigned int pending_write;
  int current_cs;
  FILE *tx_file;
  int tx_dump_bits;
  bool tx_dump_qpi;
  int tx_dump_byte;
  int mem_size;

  void *trace;
};


Spim_verif_gpio_handler::Spim_verif_gpio_handler(Spim_verif *top, int gpio, int value)
: top(top), gpio(gpio), value(value)
{

}

void Spim_verif_gpio_handler::gpio_handler_stub(Spim_verif_gpio_handler *_this)
{
  _this->top->gpio_handler(_this->gpio, _this->value);
}

Spim_verif::Spim_verif(js::config *config, void *handle) : Dpi_model(config, handle)
{
  this->mem_size = config->get("mem_size")->get_int();
  verbose = true; //config->get("verbose")->get_bool();
  print("Creating SPIM VERIF model (mem_size: 0x%x)", this->mem_size);
  data = new unsigned char[mem_size];
  qspi0 = new Spim_verif_qspi_itf(this);
  create_itf("input", static_cast<Dpi_itf *>(qspi0));

  js::config *gpios_conf = config->get("gpio");
  if (gpios_conf != NULL)
  {
    for (auto x: gpios_conf->get_childs())
    {
      Spim_verif_gpio_itf *gpio = new Spim_verif_gpio_itf(this);
      gpios.push_back(gpio);
      this->create_itf(x.second->get_str(), static_cast<Dpi_itf *>(gpio));
    }
  }

  wait_cs = false;
  this->current_cs = 1;
  this->tx_file = NULL;

  js::config *tx_file_config = config->get("tx_file");
  if (tx_file_config != NULL)
  {
    js::config *path_config = tx_file_config->get("path");
    js::config *qpi_config = tx_file_config->get("qpi");
    this->tx_dump_bits = 0;
    this->tx_file = fopen(path_config->get_str().c_str(), "wb");
    this->tx_dump_qpi = qpi_config != NULL && qpi_config->get_bool();
  }

  this->trace = this->trace_new(config->get_child_str("name").c_str());
}

void Spim_verif::handle_read(uint64_t cmd)
{
  int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
  current_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
  command_addr = current_addr;

  this->trace_msg(this->trace, 2, "Handling read command (size: 0x%x, addr: 0x%x)", size, current_addr);

  state = STATE_READ_CMD;
  current_size = size;
  nb_bits = 0;
  wait_cs = true;
  //dummy_cycles = 1;
}

void Spim_verif::handle_write(uint64_t cmd)
{
  int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
  current_write_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
  command_addr = current_write_addr;

  this->trace_msg(this->trace, 2, "Handling write command (size: 0x%x, addr: 0x%x)", size, current_write_addr);

  state = STATE_WRITE_CMD;
  current_write_size = size;
  nb_write_bits = 0;
}

void Spim_verif::gpio_handler(int gpio, int value)
{
  this->trace_msg(this->trace, 2, "Setting GPIO value (gpio: %d, value: %d)", gpio, value);
  this->gpios[gpio]->set_data(value);
}

void Spim_verif::handle_gpio_set(uint64_t cmd_value)
{
  cmd_gpio_set_t cmd = { .raw=cmd_value };

  this->trace_msg(this->trace, 2, "Handling gpio set command (id: %d, value: %d, delay: %d)", cmd.id, cmd.value, cmd.delay, cmd.delay);

  if (cmd.id >= this->gpios.size())
  {
    print("WARNING: trying to set invalid gpio (id: 0x%d)", cmd.id);
    return;
  }

  Spim_verif_gpio_handler *handler = new Spim_verif_gpio_handler(this, cmd.id, cmd.value);
  this->create_delayed_handler(((int64_t)cmd.delay)*1000, (void *)&Spim_verif_gpio_handler::gpio_handler_stub, handler);
}

void Spim_verif::handle_full_duplex(uint64_t cmd)
{
  int size = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_INFO_BIT, SPIM_VERIF_CMD_INFO_WIDTH);
  current_addr = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_ADDR_BIT, SPIM_VERIF_CMD_ADDR_WIDTH);
  command_addr = current_addr;

  this->trace_msg(this->trace, 2, "Handling full duplex command (size: 0x%x, addr: 0x%x)", size, current_addr);

  state = STATE_FULL_DUPLEX_CMD;
  current_write_addr = current_addr;
  current_size = size;
  current_write_size = size;
  nb_bits = 0;
  nb_write_bits = 0;
  wait_cs = true;
}

void Spim_verif::exec_read()
{
  if (dummy_cycles)
  {
    dummy_cycles--;
    return;
  }

  if (nb_bits == 0)
  {
    if (this->current_addr >= this->mem_size)
    {
      this->fatal("Trying to read outside memory range (addr: 0x%x, mem size: 0x%x)\n", this->current_addr, this->mem_size);
    }
    else
    {
      byte = data[current_addr];
      this->trace_msg(this->trace, 4, "Read byte from memory (value: 0x%x, rem_size: 0x%x)", byte, current_size);
    }
    nb_bits = 8;
    current_addr++;
  }

  int bit = (byte >> 7) & 1;
  byte <<= 1;

  qspi0->set_data(bit);

  nb_bits--;
  current_size--;
  if (current_size == 0)
  {
    if (state == STATE_READ_CMD)
    {
      wait_cs = true;
      state = STATE_GET_CMD;
    }
  }
}

void Spim_verif::exec_write(int val)
{
  pending_write = (pending_write << 1)| val;
  nb_write_bits++;
  if (nb_write_bits == 8)
  {
    if (this->current_write_addr >= this->mem_size)
    {
      this->fatal("Trying to write outside memory range (addr: 0x%x, mem size: 0x%x)\n", this->current_write_addr, this->mem_size);
    }
    else
    {
      data[current_write_addr] = pending_write;

      this->trace_msg(this->trace, 4, "Wrote byte to memory (addr: 0x%x, value: 0x%x, rem_size: 0x%x)", current_write_addr, data[current_write_addr], current_write_size-1);
    }

    nb_write_bits = 0;
    current_write_addr++;
  }
  
  current_write_size--;
  if (current_write_size == 0)
  {
    if (nb_write_bits != 0)
    {
      int shift = 8 - nb_write_bits;
      pending_write = (data[current_write_addr] & ~((1<<shift) - 1)) | (pending_write << shift);
      this->trace_msg(this->trace, 4, "Wrote byte to memory (value: 0x%x)", data[current_write_addr]);
    }
    wait_cs = true;
    state = STATE_GET_CMD;
  }
}

void Spim_verif::handle_command(uint64_t cmd)
{
  this->trace_msg(this->trace, 2, "Handling command %x", current_cmd);

  int cmd_id = SPIM_VERIF_FIELD_GET(cmd, SPIM_VERIF_CMD_BIT, SPIM_VERIF_CMD_WIDTH);

  switch (cmd_id) {
    case SPIM_VERIF_CMD_WRITE: handle_write(cmd); break;
    case SPIM_VERIF_CMD_READ: handle_read(cmd); break;
    case SPIM_VERIF_CMD_FULL_DUPLEX: handle_full_duplex(cmd); break;
    case SPIM_VERIF_CMD_GPIO_SET: handle_gpio_set(cmd); break;
    default: print("WARNING: received unknown command (raw: 0x%llx, id: 0x%x)", cmd, cmd_id);
  }

}


void Spim_verif_gpio_itf::edge(int64_t timestamp, int data)
{
}


void Spim_verif_qspi_itf::cs_edge(int64_t timestamp, int cs)
{
  top->cs_edge(timestamp, cs);
}

void Spim_verif_qspi_itf::sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->sck_edge(timestamp, sck, data_0, data_1, data_2, data_3, mask);
}

void Spim_verif_qspi_itf::edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->edge(timestamp, data_0, data_1, data_2, data_3, mask);
}

void Spim_verif::cs_edge(int64_t timestamp, int cs)
{
  if (this->current_cs == cs) return;

  this->current_cs = cs;

  this->trace_msg(this->trace, 3, "CS edge (timestamp: %ld, cs: %d)", timestamp, cs);
  if (cs == 1) {
    // Reset pending addresses to detect CS edge during command
    current_write_addr = command_addr;
    current_addr = command_addr;

    this->wait_cs = false;
    qspi0->set_data(3);
  }

  if (cs == 0)
  {
    if (state == STATE_READ_CMD || state == STATE_FULL_DUPLEX_CMD)
    {
      exec_read();
    }
  }
}

void Spim_verif::edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  this->trace_msg(this->trace, 4, "Edge (timestamp: %ld, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sdio0, sdio1, sdio2, sdio3, mask);

  handle_clk_high(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
  handle_clk_low(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
}

void Spim_verif::exec_dump_single(int sdio0)
{
  this->tx_dump_byte = (this->tx_dump_byte << 1)| sdio0;
  this->tx_dump_bits++;
  if (this->tx_dump_bits == 8)
  {
    this->tx_dump_bits = 0;
    fwrite((void *)&this->tx_dump_byte, 1, 1, this->tx_file);
  }
}

void Spim_verif::exec_dump_qpi(int sdio0, int sdio1, int sdio2, int sdio3)
{
  this->tx_dump_byte = (this->tx_dump_byte << 4) | (sdio3 << 3) | (sdio2 << 2) | (sdio1 << 1) | sdio0;
  this->tx_dump_bits += 4;
  if (this->tx_dump_bits == 8)
  {
    this->tx_dump_bits = 0;
    fwrite((void *)&this->tx_dump_byte, 1, 1, this->tx_file);
  }
}

void Spim_verif::handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (wait_cs)
    return;

  if (this->tx_file != NULL)
  {
    if (this->tx_dump_qpi)
      exec_dump_qpi(sdio0, sdio1, sdio2, sdio3);
    else
      exec_dump_single(sdio0);
    // TODO properly destroy the model and close the logfile instead of flushing
    fflush(NULL);
  }

  if (state == STATE_GET_CMD)
  {
    current_cmd = (current_cmd << 1) | sdio0;
    this->trace_msg(this->trace, 4, "Received command bit (count: %d, pending: %x, bit: %d)", cmd_count, current_cmd, sdio0);
    cmd_count++;
    if (cmd_count == 64)
    {
      cmd_count = 0;
      handle_command(current_cmd);
    }
  }
  else if (state == STATE_WRITE_CMD || state == STATE_FULL_DUPLEX_CMD)
  {
    exec_write(sdio0);
  }
}

void Spim_verif::handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (wait_cs)
    return;

  if (state == STATE_READ_CMD || state == STATE_FULL_DUPLEX_CMD)
  {
    exec_read();
  }
}

void Spim_verif::sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  this->trace_msg(this->trace, 4, "SCK edge (timestamp: %ld, sck: %d, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sck, sdio0, sdio1, sdio2, sdio3, mask);

  if (prev_sck == 1 && !sck)
  {
    handle_clk_low(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
  }
  else if (prev_sck == 0 && sck)
  {
    handle_clk_high(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
  }
  prev_sck = sck;
}

extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new Spim_verif(config, handle);
}
