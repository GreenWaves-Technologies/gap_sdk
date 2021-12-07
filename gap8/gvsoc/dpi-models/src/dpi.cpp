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
#include <stdarg.h>
#include <map>
#include <vector>

#include <json.hpp>
#include <dlfcn.h>

#include "dpi/models.hpp"


#include "dpi/tb_driver.h"




// NOTE for DPI models integration with Questasim.
//
// As vsim does not export symbols to dynamically loaded modules, we need
// to group all symbols used by models into a proxy shared library that we
// dynamically load from here, so that the symbols are then available for
// the modules.

// This structure to group to entry functions of the proxy library.
typedef struct
{
  void *(*model_load)(void *config, void *handle);
} libperiph_api_t;

// Part of the JSON configuration that the testbench should use
static js::config *driver_config;

// Number of components described by the JSON file that the testbench should
// instantiate
static int driver_nb_comp = 0;
static libperiph_api_t *periph_api = NULL;




class Dpi_comp_Binding
{
public:
  Dpi_comp_Binding(std::string port, std::string chip_port) : port(port), chip_port(chip_port) {}

  std::string port;
  std::string chip_port;
};

class Dpi_comp
{
public:
  void reg_binding(Dpi_comp_Binding *binding) { bindings.push_back(binding); }
  std::vector<Dpi_comp_Binding *> bindings;
};

static Dpi_comp *dpi_comps = NULL;
static std::map<std::string, Dpi_comp *> dpi_comps_map;

/*
 * All the following functions are the entry points that the testbench can
 * call using DPI-C calls in order to understand what needs to be instantiated
 * and configured and also to ask for DPI models instantiation.
 */ 



/* JSON PARSING 
 * A json file can be parsed and then browse using the following functions. */


// Load a json file, parse it and return a descriptor which can be used
// with other functions to get JSON information
void *dpi_config_get_from_file(const char *path)
{
  return (void *)js::import_config_from_file(path);
}


// Return the json descriptor of the sub-element of the given descriptor
// which has the given name
void *dpi_config_get_config(void *config, const char *name)
{
  return (void *)(((js::config *)config)->get(name));
}


// Convert the given JSON descriptor into an integer
int dpi_config_get_int(void *config)
{
  return ((js::config *)config)->get_int();
}


// Convert the given JSON descriptor into a string
const char *dpi_config_get_str(void *config)
{
  return ((js::config *)config)->get_str().c_str();
}



/* TESTBENCH DRIVER utility functions 
 * The following functions can be called to help the testbench driver what must
 * be instantiated and what must be connected and how.  */


// Initialize  the testbench driver with the given JSON configuration
void *dpi_driver_set_config(void *handle)
{
  js::config *config = (js::config *)handle;
  driver_config = config->get("**/system_tree/board");
  js::config *tb_comps_config = driver_config->get("tb_comps");
  if (tb_comps_config != NULL)
    driver_nb_comp = tb_comps_config->get_size();
  return NULL;
}


// Get number of components that the testbench should instantiate
int dpi_driver_get_nb_comp(void *handle)
{
  if (dpi_comps == NULL && driver_nb_comp != 0) 
  {
    dpi_comps = new Dpi_comp[driver_nb_comp];
    for (int i=0; i<driver_nb_comp; i++)
    {
      js::config *tb_comps_config = driver_config->get("tb_comps");
      js::config *comp_config = tb_comps_config->get_elem(i);
      dpi_comps_map[tb_comps_config->get_elem(i)->get_str()] = &dpi_comps[i];
    }

    js::config *bindings = driver_config->get("tb_bindings");

    for (int i=0; i<bindings->get_size(); i++)
    {
      js::config *binding = bindings->get_elem(i);
      std::string master_desc = binding->get_elem(0)->get_str();
      std::string slave_desc = binding->get_elem(1)->get_str();
      std::string delimiter = "->";
      std::string master = master_desc.substr(0, master_desc.find(delimiter));
      std::string master_port = master_desc.erase(0, master_desc.find(delimiter) + delimiter.length());
      std::string slave = slave_desc.substr(0, slave_desc.find(delimiter));
      std::string slave_port = slave_desc.erase(0, slave_desc.find(delimiter) + delimiter.length());

      std::string comp;

      if (master == "chip")
      {
        dpi_comps_map[slave]->reg_binding(new Dpi_comp_Binding(slave_port, master_port));
      }
      else
      {
        dpi_comps_map[master]->reg_binding(new Dpi_comp_Binding(master_port, slave_port));
      }
    }
  }

  return driver_nb_comp;
}


// Return the name of the given component identified by his index
const char *dpi_driver_get_comp_name(void *handle, int index)
{
  js::config *tb_comps_config = driver_config->get("tb_comps");
  return strdup(tb_comps_config->get_elem(index)->get_str().c_str());
}


// Return the JSON configuration of the given component identified by his index
void *dpi_driver_get_comp_config(void *handle, int index)
{
  js::config *tb_comps_config = driver_config->get("tb_comps");
  const char *comp_name = tb_comps_config->get_elem(index)->get_str().c_str();
  return driver_config->get(comp_name);
}


// Return the number of interfaces of the given component JSON descriptor
int dpi_driver_get_comp_nb_itf(void *comp_handle, int index)
{
  js::config *comp_config = (js::config *)comp_handle;
  Dpi_comp *comp = &dpi_comps[index];
  return comp->bindings.size();
}


// Return interface information about the given component JSON descriptor
void dpi_driver_get_comp_itf_info(void *comp_handle, int index, int itf_index,
  const char **itf_name, const char **itf_type, int *itf_id, int *itf_sub_id)
{
  *itf_type = (const char *)"UNKNOWN";
  *itf_name = (const char *)"UNKNOWN";

  Dpi_comp *comp = &dpi_comps[index];
  Dpi_comp_Binding *binding = comp->bindings[itf_index];
  const char *chip_port_name = binding->chip_port.c_str();
  if (strncmp(chip_port_name, "spim", 4) == 0)
  {
    *itf_type = (const char *)"QSPIM";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 4)
    {
      *itf_id = atoi(&chip_port_name[4]);
      *itf_sub_id = atoi(&chip_port_name[8]);
    }
  }
  else if (strncmp(chip_port_name, "jtag", 4) == 0)
  {
    *itf_type = (const char *)"JTAG";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 4)
    {
      *itf_id = atoi(&chip_port_name[4]);
      *itf_sub_id = 0;
    }
    else
    {
      *itf_id = 0;
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "uart", 4) == 0)
  {
    *itf_type = (const char *)"UART";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 4)
    {
      *itf_id = atoi(&chip_port_name[4]);
      *itf_sub_id = 0;
    }
    else
    {
      *itf_id = 0;
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "cpi", 3) == 0)
  {
    *itf_type = (const char *)"CPI";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 3)
    {
      *itf_id = atoi(&chip_port_name[3]);
      *itf_sub_id = 0;
    }
    else
    {
      *itf_id = 0;
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "i2c", 3) == 0)
  {
    *itf_type = (const char *)"I2C";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 3)
    {
      *itf_id = atoi(&chip_port_name[3]);
      *itf_sub_id = 0;
    }
    else
    {
      *itf_id = 0;
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "i2s", 3) == 0)
  {
    *itf_type = (const char *)"I2S";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 3)
    {
      *itf_id = atoi(&chip_port_name[3]);
      *itf_sub_id = 0;
    }
    else
    {
      *itf_id = 0;
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "ctrl", 4) == 0)
  {
    *itf_type = (const char *)"CTRL";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 4)
    {
      *itf_id = atoi(&chip_port_name[4]);
      *itf_sub_id = 0;
    }
  }
  else if (strncmp(chip_port_name, "gpio", 4) == 0)
  {
    *itf_type = (const char *)"GPIO";
    *itf_name = strdup(binding->port.c_str());
    if (strlen(chip_port_name) > 4)
    {
      *itf_id = atoi(&chip_port_name[4]);
      *itf_sub_id = 0;
    }
  }
}



// Load the DPI model for the specified component JSON descriptor
void *dpi_model_load(void *config, void *handle)
{
  // Due to the limitations of questasim on dynamic symbols (see note at the
  // top), we have to dynamically load the library managing DPI models).
  
  if (periph_api == NULL)
  {
    void *libperiph = dlopen("libpulpperiph.so", RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
    if (libperiph == NULL)
    {
      dpi_print(NULL, "ERROR, Failed to open Pulp periph models library with error:");
      dpi_print(NULL, dlerror());
      return NULL;
    }

    libperiph_api_t *api = new libperiph_api_t;

    api->model_load = (void * (*)(void *config, void *handle))dlsym(libperiph, "model_load");
    if (api->model_load == NULL)
    {
      dpi_print(NULL, "ERROR, didn't find symbol model_load in Pulp periph models library");
      return NULL;
    }

    periph_api = api;
  }

  void *result = periph_api->model_load(config, handle);
  return result;
}

int dpi_model_start(void *handle)
{
  Dpi_model *model = (Dpi_model *)handle;
  model->start_all();
  return 0;
}


int dpi_model_stop(void *handle)
{
  Dpi_model *model = (Dpi_model *)handle;
  model->stop_all();
  return 0;
}

