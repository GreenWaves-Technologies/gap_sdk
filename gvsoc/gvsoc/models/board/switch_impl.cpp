/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include <vp/vp.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <string.h>

class Switch : public vp::component
{

public:

  Switch(js::config *config);

  static void sync(void *__this, uint32_t value);

  int build();
  void start();

private:


  vp::wire_master<int>  out_itf;
  vp::wire_slave<uint32_t>   in_itf;
  int value;
};



void Switch::sync(void *__this, uint32_t value)
{
  Switch *_this = (Switch *)__this;
  _this->out_itf.sync(value);
}


Switch::Switch(js::config *config)
: vp::component(config)
{
}

int Switch::build()
{
  this->new_master_port("out", &this->out_itf);

  in_itf.set_sync_meth(&Switch::sync);
  this->new_slave_port("input", &this->in_itf);

  this->value = this->get_config_int("value");

  return 0;
}

void Switch::start()
{
  this->out_itf.sync(this->value);
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Switch(config);
}
