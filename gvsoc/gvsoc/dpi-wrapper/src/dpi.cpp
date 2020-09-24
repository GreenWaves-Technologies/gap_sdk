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


#include <stdint.h>
#include <stdio.h>
#include <gv/gvsoc.h>
#include <gv/dpi_chip_wrapper.hpp>

using namespace std;

extern "C" void *dpi_open(char *config_path)
{
  struct gv_conf gv_conf;
  gv_init(&gv_conf);
  void *handle = gv_create(config_path, &gv_conf);
  return handle;
}

extern "C" void dpi_start(void *instance)
{

  gv_start(instance);
}

extern "C" void dpi_start_task(void *arg0, void *arg1)
{
    void (*callback)(void *) = (void (*)(void *))arg0;
    callback(arg1);
}

extern "C" void *dpi_bind(void *handle, char *name, int sv_handle)
{
  void *result = gv_chip_pad_bind(handle, name, sv_handle);
  return result;
}

extern "C" void dpi_edge(void *handle, int64_t timestamp, int data)
{
  Dpi_chip_wrapper_callback *callback = (Dpi_chip_wrapper_callback *)handle;
  callback->function(callback->_this, timestamp, data);
}