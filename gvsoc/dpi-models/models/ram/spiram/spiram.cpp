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


typedef enum {
  STATE_GET_CMD,
  STATE_GET_ADDRESS,
  STATE_WAIT_CYCLES,
  STATE_GET_DATA
} Spiram_state_e;



class Spiram;

class Spiram_qspi_itf : public Qspi_itf
{
public:
  Spiram_qspi_itf(Spiram *top) : top(top) {}
  void sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  void edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask);
  void cs_edge(int64_t timestamp, int cs);

private:
    Spiram *top;
};



class Spiram : public Dpi_model
{
  friend class Spiram_qspi_itf;

public:
  Spiram(js::config *config, void *handle);

protected:

  void sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void cs_edge(int64_t timestamp, int cs);
  void handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);


private:

  void handle_command(uint8_t cmd);
  bool check_refresh(int64_t timestamp);

  Spiram_qspi_itf *qspi0;

  Spiram_state_e state;

  int cmd_count;
  int prev_sck = 0;
  int dummy_cycles = 0;
  int command_addr;
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
  uint8_t current_cmd;
  bool qpi;
  bool quad_command;
  uint32_t current_addr;
  uint8_t current_data;
  bool is_write;
  int wait_cycles;
  int64_t refresh_timestamp;
  bool refresh_failure;
  int cs_pulse_width;
  void *trace;
};


Spiram::Spiram(js::config *config, void *handle) : Dpi_model(config, handle)
{
  this->mem_size = config->get("mem_size")->get_int();
  verbose = true; //config->get("verbose")->get_bool();
  print("Creating SPIRAM model (mem_size: 0x%x)", this->mem_size);
  data = new unsigned char[mem_size];
  qspi0 = new Spiram_qspi_itf(this);
  create_itf("input", static_cast<Dpi_itf *>(qspi0));

  this->current_cs = 1;
  this->tx_file = NULL;

  this->state = STATE_GET_CMD;
  this->cmd_count = 0;
  this->qpi = false;
  this->refresh_timestamp = -1;
  this->refresh_failure = false;
  this->cs_pulse_width = 8*1000*1000;

  this->trace = this->trace_new(config->get_child_str("name").c_str());
}


void Spiram_qspi_itf::cs_edge(int64_t timestamp, int cs)
{
  top->cs_edge(timestamp, cs);
}

void Spiram_qspi_itf::sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->sck_edge(timestamp, sck, data_0, data_1, data_2, data_3, mask);
}

void Spiram_qspi_itf::edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->edge(timestamp, data_0, data_1, data_2, data_3, mask);
}

bool Spiram::check_refresh(int64_t timestamp)
{
  if (this->refresh_failure)
    return true;

  if (this->refresh_timestamp != -1 && this->refresh_timestamp < timestamp)
  {
    this->fatal("CS has not been released in time to let the RAM being refreshed");
    memset(this->data, 0, this->mem_size);
    this->refresh_timestamp = -1;
    this->refresh_failure = true;
    return true;
  }

  return false;
}

void Spiram::cs_edge(int64_t timestamp, int cs)
{
  if (cs == 0)
  {
    if (this->check_refresh(timestamp))
      return;
  }

  if (this->current_cs == cs) return;

  if (this->current_cs == 1 && cs == 0)
  {
    this->refresh_timestamp = timestamp + this->cs_pulse_width;
  }
  else if (cs == 1)
  {
    this->refresh_timestamp = -1;
    this->refresh_failure = false;
  }

  this->current_cs = cs;

  this->trace_msg(this->trace, 3, "CS edge (timestamp: %ld, cs: %d)", timestamp, cs);
  if (cs == 1) {
    qspi0->set_data(3);
    this->state = STATE_GET_CMD;
    this->cmd_count = 0;
  }

  if (cs == 0)
  {
  }
}

void Spiram::edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (this->check_refresh(timestamp))
    return;

  this->trace_msg(this->trace, 4, "Edge (timestamp: %ld, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sdio0, sdio1, sdio2, sdio3, mask);

  handle_clk_high(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
  handle_clk_low(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
}

void Spiram::handle_command(uint8_t cmd)
{
  this->trace_msg(this->trace, 2, "Handling command 0x%2.2x", this->current_cmd);

  if (this->current_cmd == 0x35)
  {
    this->trace_msg(this->trace, 1, "Enabling QPI mode");
    this->qpi = true;
  }
  else if (this->current_cmd == 0xF5)
  {
    this->trace_msg(this->trace, 1, "Disabling QPI mode");
    this->qpi = false;
  }
  else if (this->current_cmd == 0x02)
  {
    this->trace_msg(this->trace, 1, "SPI write");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = true;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x38)
  {
    this->trace_msg(this->trace, 1, "SPI quad write");
    this->quad_command = true;
    this->state = STATE_GET_ADDRESS;
    this->is_write = true;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x03)
  {
    this->trace_msg(this->trace, 1, "SPI read");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x0B)
  {
    this->trace_msg(this->trace, 1, "SPI fast read");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->wait_cycles = 8;
  }
  else if (this->current_cmd == 0xEB)
  {
    this->trace_msg(this->trace, 1, "SPI fast quad read");
    this->quad_command = true;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->wait_cycles = 6;
  }
  else if (this->current_cmd == 0x66)
  {
    this->trace_msg(this->trace, 1, "Reset enable");
  }
  else if (this->current_cmd == 0x99)
  {
    this->trace_msg(this->trace, 1, "Reset");
  }
}

void Spiram::handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (this->state == STATE_GET_CMD)
  {
    if (this->qpi)
    {
      this->current_cmd = (this->current_cmd << 4) | (sdio0 << 0) | (sdio1 << 1) | (sdio2 << 2) | (sdio3 << 3);
      this->trace_msg(this->trace, 4, "Received command bits (count: %d, pending: %x, bit0: %d, bit1: %d, bit2: %d, bit3: %d)", this->cmd_count, this->current_cmd, sdio0, sdio1, sdio2, sdio3);
      this->cmd_count += 4;
    }
    else
    {
      this->current_cmd = (this->current_cmd << 1) | sdio0;
      this->trace_msg(this->trace, 4, "Received command bit (count: %d, pending: %x, bit: %d)", this->cmd_count, this->current_cmd, sdio0);
      this->cmd_count++;
    }
    if (this->cmd_count == 8)
    {
      this->cmd_count = 0;
      this->handle_command(this->current_cmd);
    }
  }
  else if (this->state == STATE_GET_ADDRESS)
  {
    if (this->qpi)
    {
      this->current_addr = (this->current_addr << 4) | (sdio0 << 0) | (sdio1 << 1) | (sdio2 << 2) | (sdio3 << 3);
      this->trace_msg(this->trace, 4, "Received address bits (count: %d, pending: %x, bit0: %d, bit1: %d, bit2: %d, bit3: %d)", this->cmd_count, this->current_addr, sdio0, sdio1, sdio2, sdio3);
      this->cmd_count += 4;
    }
    else
    {
      this->current_addr = (this->current_addr << 1) | sdio0;
      this->trace_msg(this->trace, 4, "Received address bit (count: %d, pending: %x, bit: %d)", this->cmd_count, this->current_addr, sdio0);
      this->cmd_count++;
    }
    if (this->cmd_count == 24)
    {
      this->cmd_count = 0;
      this->current_addr = this->current_addr & 0xffffff;
      this->trace_msg(this->trace, 2, "Received address (addr: 0x%6.6x)", this->current_addr);
      if (this->wait_cycles)
        this->state = STATE_WAIT_CYCLES;
      else
        this->state = STATE_GET_DATA;
    }
  }
  else if (this->state == STATE_WAIT_CYCLES)
  {
    this->trace_msg(this->trace, 3, "Wait cycle (cycles: %d)", this->wait_cycles);
    this->wait_cycles--;
    if (this->wait_cycles == 0)
    {
      this->state = STATE_GET_DATA;
    }
  }
  else if (this->state == STATE_GET_DATA)
  {
    if (this->is_write)
    {
      if (this->qpi)
      {
        this->current_data = (this->current_data << 4) | (sdio0 << 0) | (sdio1 << 1) | (sdio2 << 2) | (sdio3 << 3);
        this->trace_msg(this->trace, 4, "Received data bits (count: %d, pending: %x, bit0: %d, bit1: %d, bit2: %d, bit3: %d)", this->cmd_count, this->current_data, sdio0, sdio1, sdio2, sdio3);
        this->cmd_count += 4;
      }
      else
      {
        this->current_data = (this->current_data << 1) | sdio0;
        this->trace_msg(this->trace, 4, "Received data bit (count: %d, pending: %x, bit: %d)", this->cmd_count, this->current_data, sdio0);
        this->cmd_count++;
      }
      if (this->cmd_count == 8)
      {
        this->cmd_count = 0;
        this->trace_msg(this->trace, 2, "Writing data (addr: 0x%x, value: 0x%2.2x)", this->current_addr, this->current_data);

        if (this->current_addr >= this->mem_size)
        {
          this->fatal("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", this->current_addr, this->mem_size);
        }
        else
        {
          this->data[this->current_addr++] = this->current_data;
        }
      }
    }
  }
}

void Spiram::handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (this->state == STATE_GET_DATA && !this->is_write)
  {
    if (this->cmd_count == 0)
    {
      if (this->current_addr >= this->mem_size)
      {
        this->fatal("Trying to read outside memory range (addr: 0x%x, mem size: 0x%x)\n", this->current_addr, this->mem_size);
      }
      else
      {
        this->current_data = this->data[this->current_addr];
        this->trace_msg(this->trace, 2, "Read byte from memory (address: 0x%6.6x, value: 0x%2.2x)", this->current_addr, this->current_data);
        this->current_addr++;
      }
    }

    if (this->qpi || this->quad_command)
    {
      int bits = (this->current_data >> 4) & 0xf;
      this->current_data <<= 4;
      this->qspi0->set_qpi_data((bits >> 0) & 1, (bits >> 1) & 1, (bits >> 2) & 1, (bits >> 3) & 1, 0xf);
      this->cmd_count += 4;
    }
    else
    {
      int bit = (this->current_data >> 7) & 1;
      this->current_data <<= 1;
      this->qspi0->set_data(bit);
      this->cmd_count++;
    }

    if (this->cmd_count == 8)
    {
      this->cmd_count = 0;
    }
  }
}

void Spiram::sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (this->check_refresh(timestamp))
    return;

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
  return new Spiram(config, handle);
}
