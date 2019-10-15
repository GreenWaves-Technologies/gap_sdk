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

#ifndef __CABLES_CABLE_HPP__
#define __CABLES_CABLE_HPP__

#include "json.hpp"
#include "log/log.hpp"

typedef enum {
  CABLE_CONNECTED=0,
  CABLE_DISCONNECTED=1
} cable_state_t;

typedef void (*cable_cb_t)(cable_state_t state);

class Cable_jtag_itf
{
public:

    virtual bool bit_inout(char*, char, bool) { printf ("i am bit_inout virtual fct in cable class\n"); return false; }

    virtual bool stream_inout(char*, char*, unsigned int, bool) { printf ("i am stream_inout virtual fct in cable class\n"); return false; }

    virtual int flush() { return -1; }
    virtual bool purge() { return false; }
    virtual bool jtag_reset(bool) { printf("JTAG\n"); return false; }

    virtual void device_select(unsigned int) {}

    bool jtag_soft_reset();
    bool jtag_write_tms(int val);
    bool jtag_shift_ir();
    bool jtag_shift_dr();
    bool jtag_idle();
    bool jtag_shift(int width, char *bits);
    bool jtag_shift_ir(unsigned int ir);
    bool jtag_set_reg(unsigned int reg, int width, unsigned int value);
    bool jtag_get_reg(unsigned int reg, int width, unsigned int *out_value, unsigned int value);
    virtual ~Cable_jtag_itf() {};
};



class Cable_io_itf
{
public:
  virtual bool access(bool, unsigned int, int, char*) { return false; }
  virtual ~Cable_io_itf() {};
};



class Cable_ctrl_itf
{
public:
  virtual bool chip_reset(bool) { return false; }
  virtual ~Cable_ctrl_itf() {};
};



class Cable : public Cable_io_itf, public Cable_jtag_itf, public Cable_ctrl_itf
{
public:
  Cable(js::config *config) : config(config) {}
  virtual bool connect(js::config *) { return true; }

  virtual void lock() { }

  virtual void unlock() { }
  virtual ~Cable() {};
  js::config *get_config() { return this->config; }

protected:  
  js::config *config;
};

#endif
