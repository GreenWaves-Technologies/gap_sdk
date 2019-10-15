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


#include <stdint.h>
#include <stdio.h>

#include "dpi/models.hpp"

void *dpi_ctrl_bind(void *comp_handle, const char *name, int handle)
{
  Dpi_model *model = (Dpi_model *)comp_handle;
  return model->bind_itf(name, (void *)(long)handle);
}


void Ctrl_itf::reset_edge(int reset)
{
  dpi_ctrl_reset_edge((int)(long)sv_handle, reset);
}


void Ctrl_itf::config_edge(uint32_t config)
{
  dpi_ctrl_config_edge((int)(long)sv_handle, config);
}