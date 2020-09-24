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

#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vp/itf/hyper.hpp"
#include "vp/itf/wire.hpp"
#include "archi/utils.h"

#define REGS_AREA_SIZE 1024

#define FLASH_STATE_IDLE 0
#define FLASH_STATE_WRITE_BUFFER_WAIT_SIZE 1
#define FLASH_STATE_WRITE_BUFFER 2
#define FLASH_STATE_WRITE_BUFFER_WAIT_CONFIRM 3
#define FLASH_STATE_CMD_0 4
#define FLASH_STATE_CMD_1 5
#define FLASH_STATE_LOAD_VCR 6

#define FLASH_SECTOR_SIZE (1<<18)


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


typedef enum
{
  HYPERBUS_STATE_CA,
  HYPERBUS_STATE_DATA
} hyperbus_state_e;


class Hyperflash : public vp::component
{

public:
  int build();

  Hyperflash(js::config *config);

  void handle_access(int reg_access, int address, int read, uint8_t data);
  int preload_file(char *path);
  void erase_sector(unsigned int addr);
  void erase_chip();
  int setup_writeback_file(const char *path);

  static void sync_cycle(void *_this, int data);
  static void cs_sync(void *__this, bool value);

protected:
  vp::trace     trace;
  vp::hyper_slave   in_itf;
  vp::wire_slave<bool> cs_itf;

  int size;
  uint8_t *data;
  bool data_is_mmapped;
  uint8_t *reg_data;

  hyperflash_state_e state;
  hyperbus_state_e hyper_state;
  int pending_bytes;
  uint16_t pending_cmd;

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
  int reg_access;
};







void Hyperflash::erase_sector(unsigned int addr)
{
  addr &= ~(FLASH_SECTOR_SIZE - 1);

  this->trace.msg(vp::trace::LEVEL_INFO, "Erasing sector (address: 0x%x)\n", addr);

  if (addr >= this->size)
  {
    this->warning.force_warning("Received out-of-bound erase request (addr: 0x%x, flash_size: 0x%x)\n", addr, this->size);
    return;
  }

  memset(&this->data[addr], 0xff, FLASH_SECTOR_SIZE);
}



void Hyperflash::erase_chip()
{
  this->trace.msg(vp::trace::LEVEL_INFO, "Erasing chip\n");
  for (unsigned int addr=0; addr<addr + this->size; addr+= FLASH_SECTOR_SIZE)
  {
    this->erase_sector(addr);
  }
}



void Hyperflash::handle_access(int reg_access, int address, int read, uint8_t data)
{
  if (address >= this->size)
  {
    this->warning.force_warning("Received out-of-bound request (addr: 0x%x, flash_size: 0x%x)\n", address, this->size);
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
      this->trace.msg(vp::trace::LEVEL_TRACE, "Sending data byte (value: 0x%x)\n", data);
      this->in_itf.sync_cycle(data);
    }
    else
    {
      if (this->state == HYPERFLASH_STATE_PROGRAM)
      {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Writing to flash (address: 0x%x, value: 0x%x)\n", address, data);

        uint8_t new_value = this->data[address] & data;

        if (new_value != data)
        {
          this->warning.force_warning("Failed to program specified location (addr: 0x%x, flash_val: 0x%2.2x, program_val: 0x%2.2x)\n", address, this->data[address], data);
        }

        this->data[address] &= new_value;
      }
      else
      {
        this->trace.msg(vp::trace::LEVEL_TRACE, "Received data byte (address: 0x%x, value: 0x%x)\n", address, data);

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
  this->get_trace()->msg(vp::trace::LEVEL_INFO, "Preloading memory with stimuli file (path: %s)\n", path);
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("Unable to open stimulus file (path: %s, error: %s)\n", path, strerror(errno));
    return -1;
  }

  if (fread(this->data, 1, this->size, file) == 0)
    return -1;

  return 0;
}

/*
 * Bback the data memory to a mmap file to provide access to the hyperflash content
 * at the end of the execution.
 */
int Hyperflash::setup_writeback_file(const char *path)
{
  this->get_trace()->msg("writeback memory to an output file (path: %s)\n", path);
  int fd = open(path, O_RDWR | O_CREAT, 0600);
  if (fd < 0) {
	  printf("Unable to open writeback file (path: %s, error: %s)\n", path, strerror(errno));
	  return 0;
  }

  if (ftruncate(fd, this->size) < 0) {
	  printf("Unable to truncate writeback file (path: %s, error: %s)\n", path, strerror(errno));
	  close(fd);
	  return -1;
  }
  uint8_t *mmapped_data = (uint8_t *) mmap(NULL, this->size, PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
  if (!mmapped_data) {
	  printf("Unable to mmap writeback file (path: %s, error: %s)\n", path, strerror(errno));
	  close(fd);
	  return -1;
  }

  /* copy the current data content into the mmap area and replace data pointer with the mmap pointer */
  memcpy(mmapped_data, this->data, this->size);
  free(this->data);
  this->data = mmapped_data;
  this->data_is_mmapped = true;

  /*
   * fd is even not useful anymore and can be closed.
   * Data are automatically write back to the file
   * at random time during execution (depending of the kernel cache behavior)
   * and, anyway, at the termination of the application.
   */
  close(fd);
  return 0;
}



Hyperflash::Hyperflash(js::config *config)
: vp::component(config)
{
}



void Hyperflash::sync_cycle(void *__this, int data)
{
  Hyperflash *_this = (Hyperflash *)__this;

  if (_this->hyper_state == HYPERBUS_STATE_CA)
  {
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command byte (value: 0x%x)\n", data);

    _this->ca_count--;
    _this->ca.raw[_this->ca_count] = data;
    if (_this->ca_count == 0)
    {
      _this->hyper_state = HYPERBUS_STATE_DATA;
      _this->current_address = (_this->ca.low_addr | (_this->ca.high_addr << 3)) & ~1;

      _this->reg_access = _this->ca.address_space == 1;

      _this->trace.msg(vp::trace::LEVEL_TRACE, "Received command header (reg_access: %d, addr: 0x%x, read: %d)\n", _this->ca.address_space, _this->current_address, _this->ca.read);

      if (_this->state == HYPERFLASH_STATE_PROGRAM)
        _this->trace.msg(vp::trace::LEVEL_DEBUG, "Received program command header (addr: 0x%x)\n", _this->current_address);

    }
  }
  else if (_this->hyper_state == HYPERBUS_STATE_DATA)
  {
    _this->handle_access(_this->reg_access, _this->current_address, _this->ca.read, data);
    _this->current_address++;
  }
}

void Hyperflash::cs_sync(void *__this, bool value)
{
  Hyperflash *_this = (Hyperflash *)__this;
  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received CS sync (value: %d)\n", value);

  _this->hyper_state = HYPERBUS_STATE_CA;
  _this->ca_count = 6;

  if (value == 0)
  {
    if (_this->state == HYPERFLASH_STATE_PROGRAM_START)
    {
      _this->state = HYPERFLASH_STATE_PROGRAM;
    }
    else if (_this->state == HYPERFLASH_STATE_PROGRAM)
    {
      _this->trace.msg(vp::trace::LEVEL_DEBUG, "End of program command (addr: 0x%x)\n", _this->current_address);
      _this->state = HYPERFLASH_STATE_WAIT_CMD0;
    }
  }
}

int Hyperflash::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in_itf.set_sync_cycle_meth(&Hyperflash::sync_cycle);
  new_slave_port("input", &in_itf);

  cs_itf.set_sync_meth(&Hyperflash::cs_sync);
  new_slave_port("cs", &cs_itf);

  js::config *conf = this->get_js_config();

  this->size = conf->get("size")->get_int();

  this->data = new uint8_t[this->size];
  memset(this->data, 0xff, this->size);
  this->data_is_mmapped = false;

  this->reg_data = new uint8_t[REGS_AREA_SIZE];
  memset(this->reg_data, 0x57, REGS_AREA_SIZE);
  ((uint16_t *)this->reg_data)[0] = 0x8F1F;

  this->state = HYPERFLASH_STATE_WAIT_CMD0;
  this->pending_bytes = 0;
  this->pending_cmd = 0;
  
  js::config *preload_file_conf = conf->get("preload_file");
  if (preload_file_conf == NULL)
  {
    preload_file_conf = conf->get("content/image");
  }

  if (preload_file_conf)
  {
    if (this->preload_file((char *)preload_file_conf->get_str().c_str()))
      return -1;
  }

  js::config *writeback_file_conf = conf->get("writeback_file");
  if (writeback_file_conf)
  {
    if (this->setup_writeback_file(writeback_file_conf->get_str().c_str()))
    return -1;
  }

  return 0;
}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Hyperflash(config);
}
