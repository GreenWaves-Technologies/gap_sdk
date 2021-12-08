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

int dpi_qspim_cs_edge(void *handle, int64_t timestamp, int cs)
{
  Qspi_itf *itf = static_cast<Qspi_itf *>((Dpi_itf *)handle);
  itf->cs_edge(timestamp, cs);
  return 0;
}

int dpi_qspim_sck_edge(void *handle, int64_t timestamp, svLogic sck, svLogic data_0, svLogic data_1, svLogic data_2, svLogic data_3, int mask)
{
  Qspi_itf *itf = static_cast<Qspi_itf *>((Dpi_itf *)handle);
  itf->sck_edge(timestamp, sck, data_0, data_1, data_2, data_3, mask);
  return 0;
}



void dpi_qspim_edge(void *handle, int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask)
{
  Qspi_itf *itf = static_cast<Qspi_itf *>((Dpi_itf *)handle);
  itf->edge(timestamp, data_0, data_1, data_2, data_3, mask);
}

void *dpi_qspim_bind(void *comp_handle, const char *name, int handle)
{
  Dpi_model *model = (Dpi_model *)comp_handle;
  return model->bind_itf(name, (void *)(long)handle);
}

void Qspi_itf::set_data(int data_0)
{
  dpi_qspim_set_data((int)(long)sv_handle, data_0);
}

void Qspi_itf::set_qpi_data(int data_0, int data_1, int data_2, int data_3, int mask)
{
  dpi_qspim_set_qpi_data((int)(long)sv_handle, data_0, data_1, data_2, data_3, 0xf);
}
