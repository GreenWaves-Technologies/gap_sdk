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
