/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __PULP_UDMA_UDMA_HYPER_V3_IMPL_HPP__
#define __PULP_UDMA_UDMA_HYPER_V3_IMPL_HPP__

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/qspim.hpp>
#include <vp/itf/uart.hpp>
#include <vp/itf/cpi.hpp>
#include <vp/itf/wire.hpp>
#include <vp/itf/hyper.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "archi/udma/udma_v4.h"

class Hyper_periph : public Udma_periph
{
public:
  Hyper_periph(udma *top, int id, int itf_id);
  static void rx_sync(void *__this, int data);

protected:
  vp::hyper_master hyper_itf;

private:
  vp::trace     trace;
};

#endif
