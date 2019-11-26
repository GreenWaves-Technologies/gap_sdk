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

#include <vp/vp.hpp>
#include <stdio.h>
#include <string.h>
#include "vp/itf/hyper.hpp"
#include "vp/itf/wire.hpp"
#include "archi/utils.h"
#include "archi/udma/hyper/udma_hyper_v1.h"

#define REGS_AREA_SIZE 1024

#define FLASH_STATE_IDLE 0
#define FLASH_STATE_WRITE_BUFFER_WAIT_SIZE 1
#define FLASH_STATE_WRITE_BUFFER 2
#define FLASH_STATE_WRITE_BUFFER_WAIT_CONFIRM 3
#define FLASH_STATE_CMD_0 4
#define FLASH_STATE_CMD_1 5
#define FLASH_STATE_LOAD_VCR 6

#define FLASH_SECTOR_SIZE (1<<18)

class hyperchip;


typedef enum {
  HYPERFLASH_STATE_WAIT_CMD0,
  HYPERFLASH_STATE_WAIT_CMD1,
  HYPERFLASH_STATE_WAIT_CMD2,
  HYPERFLASH_STATE_WAIT_CMD3,
  HYPERFLASH_STATE_WAIT_CMD4,
  HYPERFLASH_STATE_WAIT_CMD5,
  HYPERFLASH_STATE_PROGRAM_START,
  HYPERFLASH_STATE_PROGRAM,
  HYPERFLASH_STATE_GET_STATUS_REG
} hyperflash_state_e;


class Hyperflash
{
  friend class hyperchip;

public:
  Hyperflash(hyperchip *top, int size);

  void handle_access(int reg_access, int address, int read, uint8_t data);
  int preload_file(char *path);
  void erase_sector(unsigned int addr);
  void erase_chip();

protected:
  hyperchip *top;
  int size;
  uint8_t *data;
  uint8_t *reg_data;

  hyperflash_state_e state;
  int pending_bytes;
  uint16_t pending_cmd;
};



class Hyperram
{
public:
  Hyperram(hyperchip *top, int size);

  void handle_access(int reg_access, int address, int read, uint8_t data);

private:
  hyperchip *top;
  int size;
  uint8_t *data;
  uint8_t *reg_data;
};



typedef enum
{
  HYPERCHIP_STATE_CA,
  HYPERCHIP_STATE_DATA
} hyperchip_state_e;

class hyperchip : public vp::component
{
  friend class Hyperram;
  friend class Hyperflash;

public:

  hyperchip(const char *config);

  int build();

  static void sync_cycle(void *_this, int data);
  static void cs_sync(void *__this, bool value);

protected:
  vp::trace     trace;
  vp::hyper_slave   in_itf;
  vp::wire_slave<bool> cs_itf;

private:
  Hyperflash *flash;
  Hyperram *ram;

  union
  {
    struct {
      unsigned int low_addr:3;
      unsigned int reserved:13;
      unsigned int high_addr:29;
      unsigned int burst_type:1;
      unsigned int address_space:1;
      unsigned int read:1;
    } __attribute__((packed));;
    uint8_t raw[6];
  } ca;

  int ca_count;
  int current_address;
  int flash_access;
  int reg_access;

  hyperchip_state_e state;
};



Hyperram::Hyperram(hyperchip *top, int size) : top(top), size(size)
{
  this->data = new uint8_t[this->size];
  memset(this->data, 0xff, this->size);

  this->reg_data = new uint8_t[REGS_AREA_SIZE];
  memset(this->reg_data, 0x57, REGS_AREA_SIZE);
  ((uint16_t *)this->reg_data)[0] = 0x8F1F;
}


void Hyperram::handle_access(int reg_access, int address, int read, uint8_t data)
{
  if (address >= this->size)
  {
    this->top->warning.force_warning("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", address, this->size);
  }
  else
  {
    if (read)
    {
      uint8_t data = this->data[address];
      this->top->trace.msg(vp::trace::LEVEL_TRACE, "Sending data byte (value: 0x%x)\n", data);
      this->top->in_itf.sync_cycle(data);

    }
    else
    {
      this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received data byte (value: 0x%x)\n", data);
      this->data[address] = data;
    }
  }
}



Hyperflash::Hyperflash(hyperchip *top, int size) : top(top), size(size)
{
  this->data = new uint8_t[this->size];
  memset(this->data, 0x57, this->size);

  this->reg_data = new uint8_t[REGS_AREA_SIZE];
  memset(this->reg_data, 0x57, REGS_AREA_SIZE);
  ((uint16_t *)this->reg_data)[0] = 0x8F1F;

  this->state = HYPERFLASH_STATE_WAIT_CMD0;
  this->pending_bytes = 0;
  this->pending_cmd = 0;
}



void Hyperflash::erase_sector(unsigned int addr)
{
  addr &= ~(FLASH_SECTOR_SIZE - 1);

  this->top->trace.msg(vp::trace::LEVEL_INFO, "Erasing sector (address: 0x%x)\n", addr);

  if (addr >= this->size)
  {
    this->top->warning.force_warning("Received out-of-bound erase request (addr: 0x%x, ram_size: 0x%x)\n", addr, this->size);
    return;
  }


  for (unsigned int current; current < addr + FLASH_SECTOR_SIZE; current++)
  {
    this->data[current] = 0xff;
  }
}



void Hyperflash::erase_chip()
{
  this->top->trace.msg(vp::trace::LEVEL_INFO, "Erasing chip\n");
  for (unsigned int addr=0; addr<addr + this->size; addr+= FLASH_SECTOR_SIZE)
  {
    this->erase_sector(addr);
  }
}



void Hyperflash::handle_access(int reg_access, int address, int read, uint8_t data)
{
  if (address >= this->size)
  {
    this->top->warning.force_warning("Received out-of-bound request (addr: 0x%x, ram_size: 0x%x)\n", address, this->size);
  }
  else
  {
    if (read)
    {
      uint8_t data;
      if (this->state == HYPERFLASH_STATE_GET_STATUS_REG)
      {
        data = this->pending_cmd;
        this->pending_bytes--;
        this->pending_cmd >>= 8;
        if (this->pending_bytes == 0)
          this->state = HYPERFLASH_STATE_WAIT_CMD0;
      }
      else
      {
        data = this->data[address];
      }
      this->top->trace.msg(vp::trace::LEVEL_TRACE, "Sending data byte (value: 0x%x)\n", data);
      this->top->in_itf.sync_cycle(data);
    }
    else
    {
      if (this->state == HYPERFLASH_STATE_PROGRAM)
      {
        this->top->trace.msg(vp::trace::LEVEL_TRACE, "Writing to flash (address: 0x%x, value: 0x%x)\n", address, data);

        if (this->data[address] != 0xff)
        {
          this->top->warning.force_warning("Trying to program flash without erasing sector (addr: 0x%x)\n", address);
        }

        this->data[address] = data;
      }
      else
      {
        this->top->trace.msg(vp::trace::LEVEL_TRACE, "Received data byte (address: 0x%x, value: 0x%x)\n", address, data);

        if (this->pending_bytes == 0)
        {
          this->pending_cmd = data;
          this->pending_bytes = 1;
        }
        else
        {
          uint16_t cmd = this->pending_cmd | data << 8;

          this->pending_cmd = 0;
          this->pending_bytes = 0;

          switch (this->state)
          {
            case HYPERFLASH_STATE_WAIT_CMD0:
              if ((address >> 1) == 0x555 && cmd == 0xAA)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD1;
              }
              else if ((address >> 1) == 0x555 && cmd == 0x70)
              {
                this->state = HYPERFLASH_STATE_GET_STATUS_REG;
                this->pending_bytes = 2;
                this->pending_cmd = 0x80;
              }
            break;

            case HYPERFLASH_STATE_WAIT_CMD1:
              if ((address >> 1) == 0x2AA && cmd == 0x55)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD2;
              }
            break;

            case HYPERFLASH_STATE_WAIT_CMD2:
              if ((address >> 1) == 0x555 && cmd == 0xA0)
              {
                this->state = HYPERFLASH_STATE_PROGRAM_START;
              }
              else if ((address >> 1) == 0x555 && cmd == 0x80)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD3;
              }
              else if ((address >> 1) == 0x555 && cmd == 0x38)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD0;
              }
            break;

            case HYPERFLASH_STATE_WAIT_CMD3:
              if ((address >> 1) == 0x555 && cmd == 0xAA)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD4;
              }
            break;

            case HYPERFLASH_STATE_WAIT_CMD4:
              if ((address >> 1) == 0x2AA && cmd == 0x55)
              {
                this->state = HYPERFLASH_STATE_WAIT_CMD5;
              }
            break;

            case HYPERFLASH_STATE_WAIT_CMD5:
              if ((address >> 1) == 0x555 && cmd == 0x10)
              {
                this->erase_chip();
                this->state = HYPERFLASH_STATE_WAIT_CMD0;
              }
              else if (cmd == 0x30)
              {
                this->erase_sector(address);
                this->state = HYPERFLASH_STATE_WAIT_CMD0;
              }
            break;
          }
        }
      }
    }
  }
}

int Hyperflash::preload_file(char *path)
{
  this->top->get_trace()->msg(vp::trace::LEVEL_INFO, "Preloading memory with stimuli file (path: %s)\n", path);
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("Unable to open stimulus file (path: %s, error: %s)\n", path, strerror(errno));
    return -1;
  }

  if (fread(this->data, 1, this->size, file) == 0)
    return -1;

  return 0;
}

hyperchip::hyperchip(const char *config)
: vp::component(config)
{
}

void hyperchip::sync_cycle(void *__this, int data)
{
  hyperchip *_this = (hyperchip *)__this;

  if (_this->state == HYPERCHIP_STATE_CA)
  {
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command byte (value: 0x%x)\n", data);

    _this->ca_count--;
    _this->ca.raw[_this->ca_count] = data;
    if (_this->ca_count == 0)
    {
      _this->state = HYPERCHIP_STATE_DATA;
      _this->current_address = (_this->ca.low_addr | (_this->ca.high_addr << 3)) & ~1;

      _this->flash_access = ARCHI_REG_FIELD_GET(_this->current_address, REG_MBR_WIDTH, 1);
      uint32_t top_addr = ARCHI_REG_FIELD_GET(_this->current_address, REG_MBR_WIDTH+1, 32-REG_MBR_WIDTH-1);
      if (top_addr != 0)
        _this->trace.force_warning("Truncated hyper address (address: 0x%lx, mask: 0x%lx)\n", _this->current_address, (1<<REG_MBR_WIDTH)-1);

      _this->current_address = ARCHI_REG_FIELD_GET(_this->current_address, 0, REG_MBR_WIDTH);
      _this->reg_access = _this->ca.address_space == 1;

      _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command header (flash_access: %d, reg_access: %d, addr: 0x%x, read: %d)\n", _this->flash_access, _this->ca.address_space, _this->current_address, _this->ca.read);

      if (_this->flash->state == HYPERFLASH_STATE_PROGRAM)
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received program command header (addr: 0x%x)\n", _this->current_address);

    }
  }
  else if (_this->state == HYPERCHIP_STATE_DATA)
  {
    if (_this->flash_access)
    {
      _this->flash->handle_access(_this->reg_access, _this->current_address, _this->ca.read, data);
    }
    else
    {
      _this->ram->handle_access(_this->reg_access, _this->current_address, _this->ca.read, data);
    }
    _this->current_address++;
  }
}

void hyperchip::cs_sync(void *__this, bool value)
{
  hyperchip *_this = (hyperchip *)__this;
  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received CS sync (value: %d)\n", value);

  _this->state = HYPERCHIP_STATE_CA;
  _this->ca_count = 6;

  if (value == 0)
  {
    if (_this->flash->state == HYPERFLASH_STATE_PROGRAM_START)
    {
      _this->flash->state = HYPERFLASH_STATE_PROGRAM;
    }
    else if (_this->flash->state == HYPERFLASH_STATE_PROGRAM)
    {
      _this->trace.msg(vp::trace::LEVEL_DEBUG, "End of program command (addr: 0x%x)\n", _this->current_address);
      _this->flash->state = HYPERFLASH_STATE_WAIT_CMD0;
    }
  }
}

int hyperchip::build()
{

  traces.new_trace("trace", &trace, vp::DEBUG);

  in_itf.set_sync_cycle_meth(&hyperchip::sync_cycle);
  new_slave_port("input", &in_itf);

  cs_itf.set_sync_meth(&hyperchip::cs_sync);
  new_slave_port("cs", &cs_itf);

  int ram_size = 0;
  int flash_size = 0;

  js::config *conf = this->get_js_config();

  js::config *ram_conf = conf->get("ram");
  if (ram_conf)
    ram_size = ram_conf->get("size")->get_int();
  this->ram = new Hyperram(this, ram_size);

  js::config *flash_conf = conf->get("flash");
  if (flash_conf)
    flash_size = flash_conf->get("size")->get_int();
  this->flash = new Hyperflash(this, flash_size);

  if (flash_conf)
  {
    js::config *preload_file_conf = flash_conf->get("preload_file");
    if (preload_file_conf)
    {
      if (this->flash->preload_file((char *)preload_file_conf->get_str().c_str()))
        return -1;
    }
  }

  return 0;
}



extern "C" void *vp_constructor(const char *config)
{
  return (void *)new hyperchip(config);
}
