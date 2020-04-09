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
 * Authors: Antoine Faravelon Greenwaves Technologies 
 * (antoine.faravelon@greenwaves-technologies.com)
 */

#include "dpi/models.hpp"
#include <stdint.h>
#include <vector>


typedef enum {
  STATE_GET_CMD,
  STATE_GET_ADDRESS,
  STATE_WAIT_CYCLES,
  STATE_GET_DATA,
  STATE_WAIT_CS_EDGE,
  STATE_BUSY
} Spiflash_state_e;

typedef enum {
  REG_NONE,
  STATUS_REG,
  READ_PARAM_REG,
  JEDEC_REG
} Spiflash_reg_e;

typedef enum {
  BUSY_TIME_WRITE = 30000000000,
  BUSY_TIME_ERASE = 70000000000
} Spiflash_busy_time_e;

#define JEDEC_REG_VALUE 0xABCDEF

class Spiflash;

class Spiflash_qspi_itf : public Qspi_itf
{
public:
  Spiflash_qspi_itf(Spiflash *top) : top(top) {}
  void sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  void edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask);
  void cs_edge(int64_t timestamp, int cs);

private:
    Spiflash *top;
};



class Spiflash : public Dpi_model
{
  friend class Spiflash_qspi_itf;

public:
  Spiflash(js::config *config, void *handle);

protected:

  void sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void cs_edge(int64_t timestamp, int cs);
  void handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
  void handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask);


private:

  void handle_command(uint8_t cmd);
  void handle_reg_access(uint8_t reg, int64_t timestamp);
  void start(void);

  Spiflash_qspi_itf *qspi0;

  Spiflash_state_e state;

  int cmd_count;
  int prev_sck = 0;
  // TODO: configure default according to flash defaults
  int dummy_cycles = 6;
  int page_size = 256;
  int page_mask = (this->page_size-1);
  int command_addr;
  int current_write_page;
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
  bool reset_enable = 0;
  bool reset_n_hold = 0;
  uint8_t reg = REG_NONE;
  int jedec_byte = -1;
  bool quad_command;
  bool quad_address;
  uint32_t current_addr;
  uint8_t current_data;
  bool is_write = false;
  bool is_erase = false;
  bool is_read = false;
  bool status_protected = 0;
  int block_protection = 0;
  int burst_length = 0;
  int erase_size;
  uint8_t wren = 0;
  int wait_cycles;

  int64_t timestamp_busy = 0;
  int64_t busy_time = 0;
  
  void *trace;
};


Spiflash::Spiflash(js::config *config, void *handle) : Dpi_model(config, handle)
{
  this->mem_size = config->get("mem_size")->get_int();
  verbose = true; //config->get("verbose")->get_bool();
  print("Creating SPIFLASH model (mem_size: 0x%x)", this->mem_size);
  data = new unsigned char[mem_size];
  memset(data, 0xFF, mem_size);
  qspi0 = new Spiflash_qspi_itf(this);
  create_itf("input", static_cast<Dpi_itf *>(qspi0));

  this->current_cs = 1;
  this->tx_file = NULL;

  this->state = STATE_GET_CMD;
  this->cmd_count = 0;
  this->qpi = false;

  this->trace = this->trace_new(config->get_child_str("name").c_str());
}

void Spiflash::start(void)
{
  //this->trace_msg("Building spiFlash (size: 0x%x)\n", this->size);

  // Preload the memory
  js::config *stim_file_conf = this->get_config()->get("stim_file");
  if (stim_file_conf == NULL)
  {
    stim_file_conf = this->get_config()->get("content/image");
  }
  if (stim_file_conf == NULL)
  {
    stim_file_conf = this->get_config()->get("preload_file");
  }

  if (stim_file_conf != NULL)
  {
    std::string path = stim_file_conf->get_str();
    //this->get_trace()->msg("Preloading memory with stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "rb");
    if (file == NULL)
    {
      //this->get_trace()->fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
    if (fread(this->data, 1, mem_size, file) == 0)
    {
      //this->get_trace()->fatal("Failed to read stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
  }

  js::config *slm_stim_file_conf = this->get_config()->get("slm_stim_file");
  if (slm_stim_file_conf != NULL)
  {
    std::string path = stim_file_conf->get_str();
    //this->get_trace()->msg("Preloading memory with slm stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL)
    {
      //this->get_trace()->fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }

    while(1)
    {
      unsigned int addr, value;
      int err;
      if ((err = fscanf(file, "@%x %x\n", &addr, &value)) != 2) {
        if (err == EOF) break;
        //this->get_trace()->fatal("Incorrect stimuli file (path: %s)\n", path.c_str());
        return;
      }
      if (addr < mem_size) this->data[addr] = value;
    }
  }
}

void Spiflash_qspi_itf::cs_edge(int64_t timestamp, int cs)
{
  top->cs_edge(timestamp, cs);
}

void Spiflash_qspi_itf::sck_edge(int64_t timestamp, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->sck_edge(timestamp, sck, data_0, data_1, data_2, data_3, mask);
}

void Spiflash_qspi_itf::edge(int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask)
{
  top->edge(timestamp, data_0, data_1, data_2, data_3, mask);
}

void Spiflash::cs_edge(int64_t timestamp, int cs)
{
  if (this->current_cs == cs) return;

  this->current_cs = cs;

  this->trace_msg(this->trace, 3, "CS edge (timestamp: %ld, cs: %d)", timestamp, cs);
  if (cs == 1) {
    qspi0->set_data(3);
    this->state = STATE_GET_CMD;
    this->cmd_count = 0;
    if(this->wren && this->is_write && this->reg)
    {
      this->wren = 0;
      this->is_write = 0;
    }
    if(this->wren && this->is_write && !this->reg)
    {
      this->timestamp_busy = timestamp;
      this->busy_time = BUSY_TIME_WRITE;
      this->wren = 0;
      this->is_write = 0;
    }
    if(this->wren && this->is_erase)
    {
      this->timestamp_busy = timestamp;
      this->busy_time = BUSY_TIME_ERASE;
      this->wren = 0;
      this->is_erase = 0;
    }
    this->is_read = false;
    this->reg = REG_NONE;
  }

  if (cs == 0)
  {
  }
}

void Spiflash::edge(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  this->trace_msg(this->trace, 4, "Edge (timestamp: %ld, data_0: %d, data_1: %d, data_2: %d, data_3: %d, mask: 0x%x)", timestamp, sdio0, sdio1, sdio2, sdio3, mask);

  handle_clk_high(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
  handle_clk_low(timestamp, sdio0, sdio1, sdio2, sdio3, mask);
}

void Spiflash::handle_command(uint8_t cmd)
{
  this->trace_msg(this->trace, 2, "Handling command 0x%2.2x", this->current_cmd);

  this->quad_command = false;
  this->quad_address = false;

  if (this->current_cmd == 0x35)
  {
    if(this->reset_enable)
    {
      this->trace_msg(this->trace, 1, "ERROR: enabling QPI while reset enable");
    }
    else
    {
      this->trace_msg(this->trace, 1, "Enabling QPI mode");
      this->qpi = true;
    }
  }
  else if (this->current_cmd == 0xF5)
  {
    this->trace_msg(this->trace, 1, "Disabling QPI mode");
    this->qpi = false;
  }
  else if (this->current_cmd == 0x06)
  {
    this->trace_msg(this->trace, 1, "Set write enable latch");
    this->wren = 1;
  }
  else if (this->current_cmd == 0x04)
  {
    this->trace_msg(this->trace, 1, "Unset write enable latch");
    this->wren = 0;
  }
  else if (this->current_cmd == 0x02)
  {
    this->trace_msg(this->trace, 1, "flash page program");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = true;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x05)
  {
    this->trace_msg(this->trace, 1, "Read status register");
    this->quad_command = false;
    this->state = STATE_GET_DATA;
    this->reg = STATUS_REG;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x9f)
  {
    this->trace_msg(this->trace, 1, "Read jedec id");
    this->quad_command = false;
    this->state = STATE_GET_DATA;
    this->reg = JEDEC_REG;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 0;
    this->jedec_byte = 2;
  }
  else if (this->current_cmd == 0xaf)
  {
    if(!this->qpi)
    {
      this->fatal("trying to use qpi jedec read in spi mode");
    }
    this->trace_msg(this->trace, 1, "Read jedec id");
    this->quad_command = false;
    this->state = STATE_GET_DATA;
    this->reg = JEDEC_REG;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 0;
    this->jedec_byte = 2;
  }
  else if ((this->current_cmd == 0x65) || this->current_cmd == 0x63)
  {
    this->trace_msg(this->trace, 1, "Set read parameters");
    this->quad_command = false;
    this->state = STATE_GET_DATA;
    this->reg = READ_PARAM_REG;
    this->is_write = true;
    this->wait_cycles = 0;
  }
  else if ((this->current_cmd == 0x38) || (this->current_cmd == 0x32))
  {
    this->trace_msg(this->trace, 1, "SPI quad IO write");
    this->quad_command = true;
    this->state = STATE_GET_ADDRESS;
    this->is_write = true;
    this->wait_cycles = 0;
  }
  else if ((this->current_cmd == 0xD7) || (this->current_cmd == 0x20))
  {
    this->trace_msg(this->trace, 1, "SPI flash sector erase");
    this->state = STATE_GET_ADDRESS;
    this->is_erase = true;
    this->erase_size = (1<<12);
    this->wait_cycles = 0;
  }
  else if ((this->current_cmd == 0x52))
  {
    this->trace_msg(this->trace, 1, "SPI flash sector erase");
    this->state = STATE_GET_ADDRESS;
    this->is_erase = true;
    this->erase_size = (1<<15);
    this->wait_cycles = 0;
  }
  else if ((this->current_cmd == 0xD8))
  {
    this->trace_msg(this->trace, 1, "SPI flash sector erase");
    this->state = STATE_GET_ADDRESS;
    this->is_erase = true;
    this->erase_size = (1<<16);
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x03)
  {
    this->trace_msg(this->trace, 1, "SPI read");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 0;
  }
  else if (this->current_cmd == 0x0B)
  {
    this->trace_msg(this->trace, 1, "SPI fast read");
    this->quad_command = false;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = this->dummy_cycles;
  }
  else if (this->current_cmd == 0x6B)
  {
    this->trace_msg(this->trace, 1, "SPI fast quad read");
    this->quad_command = true;
    this->quad_address = true;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 6;
  }
  else if (this->current_cmd == 0xEB)
  {
    this->trace_msg(this->trace, 1, "SPI fast quad read");
    this->quad_command = true;
    this->quad_address = true;
    this->state = STATE_GET_ADDRESS;
    this->is_write = false;
    this->is_read = true;
    this->wait_cycles = 3;
  }
  else if (this->current_cmd == 0x66)
  {
    this->trace_msg(this->trace, 1, "Reset enable");
    if(this->qpi)
    {
      this->fatal("ERROR: Reset enable with QPI enabled");
    }
    else
    {
      this->reset_enable = true;
    }
  }
  else if (this->current_cmd == 0x99)
  {
    this->trace_msg(this->trace, 1, "Reset");
  }
}


void Spiflash::handle_reg_access(uint8_t reg, int64_t timestamp)
{
  if(this->is_write)
  {
    if (this->reg == STATUS_REG && !this->status_protected)
    { // set block protection, quad enable and status reg protection
      this->block_protection = ((this->current_data >> 0x2) & 0xF);
      this->qpi              = ((this->current_data >> 0x6) & 0x1);
      this->status_protected = ((this->current_data >> 0x7) & 0x1);
      if (this->qpi && this->reset_enable)
      {
        this->fatal("Trying to set QPIE while reset enable is set\n");
      }
    }
    else if (this->reg == READ_PARAM_REG)
    {// set dummy cycles, hold#/reset# and read burst
      this->burst_length = (this->current_data & 0x3);
      this->dummy_cycles = ((this->current_data >> 0x3) & 0xF);
      this->reset_n_hold = ((this->current_data >> 0x7) & 0x1);
      
    }
    else
    {
      this->trace_msg(this->trace, 2, "Trying to wite unimplemented reg (reg: 0x%x, value: 0x%2.2x)", this->reg, this->current_data);
    }
  }
  else
  {
    if (this->reg == STATUS_REG)
    { // read block protection, quad enable and status reg protection
      this->current_data = ((this->timestamp_busy + this->busy_time) > timestamp)
        | ((this->block_protection << 0x2))
        | ((this->qpi << 0x6)) | ((this->status_protected << 0x7));
    }
    else if (this->reg == READ_PARAM_REG)
    {// set dummy cycles, hold#/reset# and read burst
      this->current_data = this->burst_length
        | ((this->dummy_cycles << 0x3)) | ((this->reset_n_hold << 0x7));
      
    }
    else if (this->reg == JEDEC_REG)
    {      
      if(jedec_byte<0)
      {// if no new command and already done, wrap around
        jedec_byte = 2;
      }
      this->current_data = JEDEC_REG_VALUE >> this->jedec_byte * 8;
      this->jedec_byte --;
    }
    else
    {
      this->fatal("Trying to wite unimplemented reg (reg: 0x%x, value: 0x%2.2x)", this->reg, this->current_data);
    }
  }
  this->trace_msg(this->trace, 2, "Read register (reg: 0x%6.6x, value: 0x%2.2x)", this->reg, this->current_data);
}

void Spiflash::handle_clk_high(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
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
    if (this->qpi || this->quad_address)
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
      if (this->is_write)
      {
        this->current_write_page = this->current_addr & ~(this->page_mask);
      }
      else if (this->is_erase && this->wren)
      {
        this->trace_msg(this->trace, 2, "Erasing data (addr: 0x%x, size: 0x%2.2x)", this->current_addr, this->erase_size);
        if (this->current_addr >= this->mem_size)
        {
          this->trace_msg(this->trace, 2, "Writing data (addr: 0x%x, value: 0x%2.2x)", this->current_addr, this->current_data);
          this->fatal("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", this->current_addr, this->mem_size);
        }
        memset(&(this->data[this->current_addr]), 0xFF, this->erase_size);
        this->state = STATE_BUSY;
      }
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
      if (this->qpi | this->quad_command)
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
        if(this->wren)
        {
          if (this->reg)
          {
            this->handle_reg_access(this->reg, timestamp);
            this->state = STATE_WAIT_CS_EDGE;
          }
          else if (this->current_addr >= this->mem_size)
          {
            this->trace_msg(this->trace, 2, "Writing data (addr: 0x%x, value: 0x%2.2x)", this->current_addr, this->current_data);
            this->fatal("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", this->current_addr, this->mem_size);
          }
          else
          {
            this->trace_msg(this->trace, 2, "Writing data (addr: 0x%x, value: 0x%2.2x)", this->current_addr, this->current_data);
            if ((this->timestamp_busy + this->busy_time) > timestamp)
            {
              this->fatal("Trying to write while flash is busy");
            }
            else
            {
              if (!((~this->data[this->current_addr] & ~this->current_data) == ~this->data[this->current_addr]))
              {
                this->fatal("Trying to write non erased data at 0x%x 0x%2.2x original data:0x%2.2x\n", this->current_addr ,this->current_data,this->data[this->current_addr]);
              }
              this->data[this->current_addr++] = this->current_data;
              if(this->current_addr >= (this->current_write_page + this->page_size))
              {
                this->current_addr = this->current_addr = this->current_write_page;
              }
            }
          }
        }
      }
    }
  }
}

void Spiflash::handle_clk_low(int64_t timestamp, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
{
  if (this->state == STATE_GET_DATA && this->is_read)
  {
    if (this->cmd_count == 0)
    {
      if (this->reg)
      {
        this->handle_reg_access(this->reg, timestamp);
      }
      else if (this->current_addr >= this->mem_size)
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

void Spiflash::sck_edge(int64_t timestamp, int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask)
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
  return new Spiflash(config, handle);
}
