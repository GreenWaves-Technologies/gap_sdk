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
#include <gv/gvsoc.h>
#include <gv/dpi_chip_wrapper.hpp>

using namespace std;

extern "C" void dpi_external_edge(int handle, int value);

extern "C" void *dpi_open(char *config_path)
{
  void *handle = gv_open(config_path, 0, NULL, 0, 0);
  return handle;
}

extern "C" void *dpi_bind(void *handle, char *name, int sv_handle)
{
  void *result = gv_chip_pad_bind(handle, name);
  return result;
}

extern "C" void dpi_edge(void *handle, int64_t timestamp, int data)
{
  Dpi_chip_wrapper_callback *callback = (Dpi_chip_wrapper_callback *)handle;
  callback->function(callback->_this, timestamp, data);
}