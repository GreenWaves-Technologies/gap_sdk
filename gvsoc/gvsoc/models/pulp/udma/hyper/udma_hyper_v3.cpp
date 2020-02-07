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


#include "../udma_impl.hpp"
#include "archi/utils.h"
#include "vp/itf/hyper.hpp"



Hyper_periph::Hyper_periph(udma *top, int id, int itf_id) : Udma_periph(top, id)
{
  std::string itf_name = "hyper" + std::to_string(itf_id);

  top->traces.new_trace(itf_name, &trace, vp::DEBUG);

  this->hyper_itf.set_sync_cycle_meth(&Hyper_periph::rx_sync);
  top->new_master_port(this, itf_name, &this->hyper_itf);
}



void Hyper_periph::rx_sync(void *__this, int data)
{
}
