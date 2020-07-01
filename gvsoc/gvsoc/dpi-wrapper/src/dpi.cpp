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