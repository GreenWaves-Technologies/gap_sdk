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



void dpi_uart_edge(void *handle, int64_t timestamp, int data)
{
  Uart_itf *itf = static_cast<Uart_itf *>((Dpi_itf *)handle);
  itf->tx_edge(timestamp, data);
}

void *dpi_uart_bind(void *comp_handle, const char *name, int handle)
{
  Dpi_model *model = (Dpi_model *)comp_handle;
  return model->bind_itf(name, (void *)(long)handle);
}

void Uart_itf::rx_edge(int data)
{
  dpi_uart_rx_edge((int)(long)sv_handle, data);
}