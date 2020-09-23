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
#include <stdio.h>
#include <string.h>
#include <vp/itf/qspim.hpp>

#define CMD_READ_ID       0x9f
#define CMD_RDCR          0x35
#define CMD_WREN          0x06
#define CMD_WRR           0x48
#define CMD_WRAR          0x71
#define CMD_RDSR1         0x05
#define CMD_P4E           0x20
#define CMD_PP            0x02
#define CMD_QIOR          0xEB
#define CMD_QIOR_4B       0xEC
#define CMD_READ          0x0C
#define CMD_READ_SIMPLE   0x03
#define CMD_SND_MODE_BYTE 0x0A
#define CMD_RESET_ENABLE  0x66
#define CMD_RESET         0x99
#define CMD_SECTOR_ERASE  0xD8
#define CMD_READ_SR2V     0x07

class spiflash;

typedef struct {
  unsigned char id;
  string desc;
  void (*handler)(void*,int,int,int,int);
  void (*start_handler)(void*);
} command_t;

typedef union {
  struct {
    int freeze:1;
    int quad:1;
    int tbparm:1;
    int bpnv:1;
    int rfu:1;
    int tbprot:1;
    int lc0:1;
    int lc1:1;
  };
  uint8_t raw;
} reg_cr1_t;

typedef union {
  struct {
    int ps:1;
    int es:1;
    int estat:1;
  };
  uint8_t raw;
} reg_sr2v_t;

#define REGS_AREA_SIZE 1024

#define FLASH_STATE_IDLE 0
#define FLASH_STATE_WRITE_BUFFER_WAIT_SIZE 1
#define FLASH_STATE_WRITE_BUFFER 2
#define FLASH_STATE_WRITE_BUFFER_WAIT_CONFIRM 3
#define FLASH_STATE_CMD_0 4
#define FLASH_STATE_CMD_1 5
#define FLASH_STATE_LOAD_VCR 6


class spiflash;



class spiflash : public vp::component
{
public:

  spiflash(js::config *config);

  int build();
  void start();

  static void sector_erase(void *__this, int data_0, int data_1, int data_2, int data_3);
  static void sector_erase_done(void *__this, vp::clock_event *event);
  static void quad_read(void *__this, int data_0, int data_1, int data_2, int data_3);
  static void single_read(void *__this, int data_0, int data_1, int data_2, int data_3);
  static void write_any_register(void *__this, int data_0, int data_1, int data_2, int data_3);
  static void read_sr2v(void *__this, int data_0, int data_1, int data_2, int data_3);
  static void read_sr2v_start(void *__this);
  static void page_program(void *__this, int data_0, int data_1, int data_2, int data_3);

protected:
  vp::qspim_slave   in_itf;
  vp::wire_slave<bool> cs_itf;

private:

  static void sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask);
  static void sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask);
  static void cs_sync(void *__this, bool active);

  void handle_data(int data_0, int data_1, int data_2, int data_3);
  void start_command();
  void enqueue_bits(int data_0, int data_1, int data_2, int data_3);
  void send_bits();

  
  vp::trace     trace;

  int size;

  command_t *commands[256];
  uint8_t *mem_data;
  unsigned int pending_word;
  unsigned int pending_addr;
  int pending_bits;
  unsigned char pending_command_id;
  command_t *pending_command;
  int pending_post_addr_cmd;
  int pending_bytes;

  reg_cr1_t cr1;
  reg_sr2v_t sr2v;

  bool quad;
  bool read;
  bool waiting_command;

  unsigned int current_addr;

  vp::clock_event *sector_erase_event;

};



static command_t commands_descs[] = {
  { CMD_READ_ID      , "read ID"             , NULL                          , NULL},
  { CMD_RDCR         , "read config"         , NULL                          , NULL},
  { CMD_WREN         , "write enable"        , NULL                          , NULL}, 
  { CMD_WRR          , "write register"      , NULL                          , NULL},
  { CMD_WRAR         , "write any register"  , &spiflash::write_any_register , NULL},    
  { CMD_RDSR1        , "read status register", NULL                          , NULL},
  { CMD_P4E          , "parameter 4k erase"  , NULL                          , NULL},
  { CMD_PP           , "page program"        , &spiflash::page_program       , NULL},
  { CMD_QIOR         , "quad IO read"        , NULL                          , NULL},
  { CMD_QIOR_4B      , "quad IO read"        , &spiflash::quad_read          , NULL},
  { CMD_READ         , "IO read"             , &spiflash::single_read        , NULL},
  { CMD_READ_SIMPLE  , "IO read"             , &spiflash::single_read        , NULL},
  { CMD_RESET_ENABLE , "Reset enable"        , NULL                          , NULL},
  { CMD_RESET        , "Reset"               , NULL                          , NULL},
  { CMD_SECTOR_ERASE , "Sector erase"        , &spiflash::sector_erase       , NULL},
  { CMD_READ_SR2V    , "Read SR2V"           , &spiflash::read_sr2v          , &spiflash::read_sr2v_start}
};


void spiflash::page_program(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  _this->enqueue_bits(data_0, data_1, data_2, data_3);

  if (_this->pending_bits == 24)
  {
    _this->current_addr = _this->pending_word & 0xffffff;
    _this->read = false;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Received address (address: 0x%x)\n", _this->current_addr);
  }

  if (_this->pending_bits >= 32)
  {
    if (_this->pending_bits % 8 == 0)
    {
      if (_this->current_addr >= _this->size) {
        _this->warning.force_warning("Received out-of-bound request (address: 0x%x, memSize: 0x%x)\n", _this->current_addr, _this->size);
        return;
      }

      _this->trace.msg(vp::trace::LEVEL_DEBUG, "Writing byte (address: 0x%x, value: 0x%x)\n", _this->current_addr, (uint8_t)_this->pending_word);

      _this->mem_data[_this->current_addr++] = _this->pending_word;
    }
  }
}

void spiflash::sector_erase_done(void *__this, vp::clock_event *event)
{
  spiflash *_this = (spiflash *)__this;

  _this->trace.msg(vp::trace::LEVEL_INFO, "Sector erase done\n");

  _this->sr2v.raw |= 1<<2;
}

void spiflash::sector_erase(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  if (_this->pending_bits == 0)
    _this->sr2v.raw &= ~(1<<2);

  _this->enqueue_bits(data_0, data_1, data_2, data_3);

  if (_this->pending_bits == 24)
  {
    _this->current_addr = _this->pending_word;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Received address (address: 0x%x)\n", _this->current_addr);
    _this->event_enqueue(_this->sector_erase_event, 1000000);
  }

}

void spiflash::read_sr2v_start(void *__this)
{
  spiflash *_this = (spiflash *)__this;

  _this->pending_word = _this->sr2v.raw;
  _this->read = true;
  _this->send_bits();
}

void spiflash::read_sr2v(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  _this->send_bits();
}

void spiflash::quad_read(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  if (_this->pending_bits == 0)
  {
    _this->quad = true;
  }

  _this->enqueue_bits(data_0, data_1, data_2, data_3);

  if (_this->pending_bits == 32)
  {
    _this->current_addr = _this->pending_word;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Received address (address: 0x%x)\n", _this->current_addr);
  }

  if (_this->pending_bits == 40)
  {
    int mode = _this->pending_word & 0xff;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Received mode (value: 0x%x)\n", mode);
    _this->read = true;
  }

  if (_this->pending_bits >= 40)
  {
    if (_this->pending_bits % 8 == 0)
    {
      if (_this->current_addr >= _this->size) {
        _this->warning.force_warning("Received out-of-bound request (address: 0x%x, memSize: 0x%x)\n", _this->current_addr, _this->size);
        return;
      }

      _this->pending_word = _this->mem_data[_this->current_addr++];
    }
  }

  _this->send_bits();

}

void spiflash::single_read(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  _this->enqueue_bits(data_0, data_1, data_2, data_3);

  if (_this->pending_bits == 24)
  {
    _this->current_addr = _this->pending_word & 0xffffff;
    _this->read = true;
    _this->trace.msg(vp::trace::LEVEL_INFO, "Received address (address: 0x%x)\n", _this->current_addr);
  }

  if (_this->pending_bits >= 24)
  {
    if (_this->pending_bits % 8 == 0)
    {
      if (_this->current_addr >= _this->size) {
        _this->warning.force_warning("Received out-of-bound request (address: 0x%x, memSize: 0x%x)\n", _this->current_addr, _this->size);
        return;
      }

      _this->pending_word = _this->mem_data[_this->current_addr++];
    }
  }

  _this->send_bits();

}

void spiflash::enqueue_bits(int data_0, int data_1, int data_2, int data_3)
{
  if (!this->quad)
  {
    if (!this->read)
    {
      this->trace.msg(vp::trace::LEVEL_TRACE, "Handling single data (data_0: %d)\n", data_0);
      this->pending_word = (this->pending_word << 1) | data_0;
    }
    this->pending_bits++;
  }
  else
  {
    if (!this->read)
    {
      this->trace.msg(vp::trace::LEVEL_TRACE, "Handling quad data (data_0: %d, data_1: %d, data_2: %d, data_3: %d)\n", data_0, data_1, data_2, data_3);
      this->pending_word = (this->pending_word << 4) | (data_3 << 3) | (data_2 << 2) | (data_1 << 1) | (data_0 << 0);
    }
    this->pending_bits += 4;
  }
}

void spiflash::send_bits()
{
  if (this->read)
  {
    if (!this->quad)
    {
      unsigned int value = (this->pending_word >> 7) & 0x1;
      this->pending_word <<= 1;
      this->trace.msg(vp::trace::LEVEL_TRACE, "Sending single data (data_0: %d)\n", value);
      this->in_itf.sync(0, value, 0, 0, 2);
    }
    else
    {
      unsigned int value = (this->pending_word >> 4) & 0xf;
      this->pending_word <<= 4;
      this->trace.msg(vp::trace::LEVEL_TRACE, "Sending quad data (data_0: %d, data_1: %d, data_2: %d, data_3: %d)\n", (value >> 0) & 1, (value >> 1) & 1, (value >> 2) & 1, (value >> 3) & 1);
      this->in_itf.sync((value >> 0) & 1, (value >> 1) & 1, (value >> 2) & 1, (value >> 3) & 1, 0xf);
    }
  }
}

void spiflash::write_any_register(void *__this, int data_0, int data_1, int data_2, int data_3)
{
  spiflash *_this = (spiflash *)__this;

  _this->enqueue_bits(data_0, data_1, data_2, data_3);

  if (_this->pending_bits == 32)
  {
    unsigned int addr = _this->pending_word >> 8;
    unsigned int value = _this->pending_word & 0xff;

    if (addr == 0x800002)
    {
      _this->trace.msg(vp::trace::LEVEL_INFO, "Writing cr1 register (value: %d)\n", value);
      _this->cr1.raw = value;
    }

    _this->start_command();
  }
}

void spiflash::start_command()
{
  this->waiting_command = true;
  this->pending_bits = 8;
  this->quad = false;
  this->read = false;
  this->pending_word = 0;
}

void spiflash::handle_data(int data_0, int data_1, int data_2, int data_3)
{
  if (this->waiting_command)
  {
    this->trace.msg(vp::trace::LEVEL_TRACE, "Handling single data (data_0: %d)\n", data_0);

    this->pending_word = (this->pending_word << 1) | data_0;
    this->pending_bits--;

    if (this->pending_bits == 0)
    {
      this->waiting_command = false;
      this->pending_command_id = this->pending_word & 0xff;
      this->pending_command = this->commands[this->pending_command_id];
      if (this->pending_command == NULL)
      {
        this->warning.force_warning("Received unknown command (cmd: 0x%x)\n", this->pending_command_id);
        this->start_command();
        return;
      }
      this->trace.msg(vp::trace::LEVEL_TRACE, "Received command (ID: 0x%x, name: %s)\n", this->pending_command_id, this->pending_command->desc.c_str());
 
      if (this->pending_command->start_handler)
        this->pending_command->start_handler(this);
    }
  }
  else
  {
    if (this->pending_command->handler)
      this->pending_command->handler(this, data_0, data_1, data_2, data_3);
  }
}


void spiflash::sync(void *__this, int sck, int data_0, int data_1, int data_2, int data_3, int mask)
{
  spiflash *_this = (spiflash *)__this;
  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received edge (sck: %d, data_0: %d, data_1: %d, data_2: %d, data_3: %d)\n", sck, data_0, data_1, data_2, data_3);

  if (sck)
    _this->handle_data(data_0, data_1, data_2, data_3);
}

void spiflash::sync_cycle(void *__this, int data_0, int data_1, int data_2, int data_3, int mask)
{
  spiflash *_this = (spiflash *)__this;
  _this->handle_data(data_0, data_1, data_2, data_3);
}

void spiflash::cs_sync(void *__this, bool active)
{
  spiflash *_this = (spiflash *)__this;  
  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received CS sync (value: %d)\n", active);

  if (active == false)
  {
    _this->start_command();
  }


}
  
int spiflash::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  this->in_itf.set_sync_meth(&spiflash::sync);
  this->in_itf.set_sync_cycle_meth(&spiflash::sync_cycle);
  this->new_slave_port("input", &this->in_itf);

  this->cs_itf.set_sync_meth(&spiflash::cs_sync);
  this->new_slave_port("cs", &this->cs_itf);

  memset((void *)this->commands, 0, sizeof(commands));

  for (unsigned int i=0; i<sizeof(commands_descs)/sizeof(command_t) ; i++)
  {
    command_t *command = &commands_descs[i];
    this->commands[command->id] = command;
  }

  this->size = this->get_config_int("size");

  this->mem_data = new uint8_t[this->size];

  memset(this->mem_data, 0x57, this->size);

  this->cr1.raw = 0;
  this->quad = false;

  this->sector_erase_event = event_new(spiflash::sector_erase_done);

  this->sr2v.raw = 0;

  return 0;
}


void spiflash::start()
{
  this->trace.msg(vp::trace::LEVEL_INFO, "Building spiFlash (size: 0x%x)\n", this->size);

  // Preload the memory
  js::config *stim_file_conf = this->get_js_config()->get("stim_file");
  if (stim_file_conf == NULL)
  {
    stim_file_conf = this->get_js_config()->get("content/image");
  }
  if (stim_file_conf == NULL)
  {
    stim_file_conf = this->get_js_config()->get("preload_file");
  }

  if (stim_file_conf != NULL)
  {
    string path = stim_file_conf->get_str();
    this->get_trace()->msg(vp::trace::LEVEL_INFO, "Preloading memory with stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "rb");
    if (file == NULL)
    {
      this->get_trace()->fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
    if (fread(this->mem_data, 1, size, file) == 0)
    {
      this->get_trace()->fatal("Failed to read stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }
  }

  js::config *slm_stim_file_conf = this->get_js_config()->get("slm_stim_file");
  if (slm_stim_file_conf != NULL)
  {
    string path = stim_file_conf->get_str();
    this->get_trace()->msg(vp::trace::LEVEL_INFO, "Preloading memory with slm stimuli file (path: %s)\n", path.c_str());

    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL)
    {
      this->get_trace()->fatal("Unable to open stim file: %s, %s\n", path.c_str(), strerror(errno));
      return;
    }

    while(1)
    {
      unsigned int addr, value;
      int err;
      if ((err = fscanf(file, "@%x %x\n", &addr, &value)) != 2) {
        if (err == EOF) break;
        this->get_trace()->fatal("Incorrect stimuli file (path: %s)\n", path.c_str());
        return;
      }
      if (addr < size) this->mem_data[addr] = value;
    }
  }
}

spiflash::spiflash(js::config *config)
: vp::component(config)
{
}


extern "C" vp::component *vp_constructor(js::config *config)
{
  return new spiflash(config);
}
