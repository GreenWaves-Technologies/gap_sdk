/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and
 * GreenWaves Technologies
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

using namespace std;

class Eeprom;


class Eeprom_i2c_itf : public I2c_itf
{
public:
  Eeprom_i2c_itf(Eeprom *top) : top(top) {}
  void tx_edge(int64_t timestamp, int scl, int sda);

private:
  Eeprom *top;
};



class Eeprom_i2c_slave : public I2c_slave
{
public:
  Eeprom_i2c_slave(Eeprom *top, unsigned int address);

  void start(unsigned int address, bool is_write);
  void handle_byte(uint8_t byte);
  void stop();
  void ack();

private:
  Eeprom *top;
  int pending_bytes;
  int pending_addr;
  int wait_address;
};



class Eeprom : public Dpi_model
{
  friend class Eeprom_i2c_slave;

public:
  Eeprom(js::config *config, void *handle);

  void start();

  void i2c_tx_edge(int64_t timestamp, int scl, int sda);

  bool i2c_is_read;

private:

  I2c_itf *i2c;
  Eeprom_i2c_slave *i2c_slave;
  int size;
  uint8_t *mem_array;
};


Eeprom::Eeprom(js::config *config, void *handle) : Dpi_model(config, handle)
{
  i2c = new Eeprom_i2c_itf(this);
  create_itf("i2c", static_cast<I2c_itf *>(i2c));

  this->i2c_slave = new Eeprom_i2c_slave(this, 0xA0);
  this->size = 65536;

  this->mem_array = new uint8_t[this->size];
}

void Eeprom::start()
{
}


void Eeprom::i2c_tx_edge(int64_t timestamp, int scl, int sda)
{
  int sda_out = 0;
  this->i2c_slave->handle_edge(scl, sda, &sda_out);
  if (scl && this->i2c_is_read)
  {
    this->i2c->rx_edge(sda_out);
  }
}



void Eeprom_i2c_itf::tx_edge(int64_t timestamp, int scl, int sda)
{
  top->i2c_tx_edge(timestamp, scl, sda);
}

Eeprom_i2c_slave::Eeprom_i2c_slave(Eeprom *top, unsigned int address) : I2c_slave(address), top(top)
{
  this->pending_addr = 0;
  this->pending_bytes = 0;
  this->wait_address = 2;
}

void Eeprom_i2c_slave::start(unsigned int address, bool is_read)
{
  this->top->print("Received start (address: 0x%x, is_read: %d)\n", address, is_read);
  this->top->i2c_is_read = is_read;
}

void Eeprom_i2c_slave::handle_byte(uint8_t byte)
{
  if (this->wait_address)
  {
    if (this->wait_address == 2)
      this->pending_addr = this->pending_addr | (byte << 8);
    else
      this->pending_addr = this->pending_addr | byte;

    this->top->print("Received address byte (address: 0x%x)\n", this->pending_addr);

    this->wait_address--;
  }
  else
  {
    if (this->pending_addr >= this->top->size)
    {
      this->top->fatal("Received out-of-bound request (addr: 0x%x, memSize: 0x%x)\n", this->pending_addr, this->top->size);
    }
    else
    {
      if (!this->top->i2c_is_read)
      {
        if (this->pending_addr >= this->top->size)
        {
          this->top->fatal("Received out-of-bound request (addr: 0x%x, memSize: 0x%x)\n", this->pending_addr, this->top->size);
        }
        else
        {
          this->top->print("Writing memory (address: 0x%x, value: 0x%x)\n", this->pending_addr, byte);
          this->top->mem_array[this->pending_addr] = byte;
        }
      }
      this->pending_addr++;
    }
  }
}

void Eeprom_i2c_slave::ack()
{
  if (this->top->i2c_is_read && this->wait_address == 0)
  {
    uint8_t byte = this->top->mem_array[this->pending_addr];
    this->top->print("Reading memory (address: 0x%x, value: 0x%x)\n", this->pending_addr, byte);
    this->send_byte(byte);
  }
}

void Eeprom_i2c_slave::stop()
{
  this->top->print("Received stop\n");
  this->wait_address = 2;
  this->pending_addr = 0;
}


extern "C" Dpi_model *dpi_model_new(js::config *config, void *handle)
{
  return new Eeprom(config, handle);
}
