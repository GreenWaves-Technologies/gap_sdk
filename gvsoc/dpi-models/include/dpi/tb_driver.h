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

#ifndef __DPI_TB_DRIVER_H__
#define __DPI_TB_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif


/* JSON PARSING 
 * A json file can be parsed and then browse using the following functions. */


// Load a json file, parse it and return a descriptor which can be used
// with other functions to get JSON information
void *dpi_config_get_from_file(const char *path);


// Return the json descriptor of the sub-element of the given descriptor
// which has the given name
void *dpi_config_get_config(void *config, const char *name);


// Convert the given JSON descriptor into an integer
int dpi_config_get_int(void *config);


// Convert the given JSON descriptor into a string
const char *dpi_config_get_str(void *config);



/* TESTBENCH DRIVER utility functions 
 * The following functions can be called to help the testbench driver what must
 * be instantiated and what must be connected and how.  */


// Initialize  the testbench driver with the given JSON configuration
void *dpi_driver_set_config(void *handle);


// Get number of components that the testbench should instantiate
int dpi_driver_get_nb_comp(void *handle);


// Return the name of the given component identified by his index
const char *dpi_driver_get_comp_name(void *handle, int index);


// Return the JSON configuration of the given component identified by his index
void *dpi_driver_get_comp_config(void *handle, int index);


// Return the number of interfaces of the given component JSON descriptor
int dpi_driver_get_comp_nb_itf(void *comp_handle, int index);


// Return interface information about the given component JSON descriptor
void dpi_driver_get_comp_itf_info(void *comp_handle, int index, int itf_index,
  const char **itf_name, const char **itf_type, int *itf_id, int *itf_sub_id);


// Load the DPI model for the specified component JSON descriptor
void *dpi_model_load(void *config, void *handle);


int dpi_model_start(void *handle);

int dpi_model_stop(void *handle);


int dpi_start_task(int id);

void dpi_exec_periodic_handler(int id);




void *dpi_qspim_bind(void *comp_handle, const char *name, int handle);

void *dpi_jtag_bind(void *comp_handle, const char *name, int handle);

void *dpi_cpi_bind(void *comp_handle, const char *name, int handle);

void *dpi_ctrl_bind(void *comp_handle, const char *name, int handle);

void *dpi_gpio_bind(void *comp_handle, const char *name, int handle);

void *dpi_uart_bind(void *comp_handle, const char *name, int handle);

void *dpi_i2s_bind(void *comp_handle, const char *name, int handle);

void *dpi_i2c_bind(void *comp_handle, const char *name, int handle);

void dpi_uart_edge(void *handle, int64_t timestamp, int data);

void dpi_i2c_edge(void *handle, int64_t timestamp, int scl, int sda);

void dpi_i2s_edge(void *handle, int64_t timestamp, int sck, int ws, int sd);

int dpi_qspim_cs_edge(void *handle, int64_t timestamp, int active);

void dpi_qspim_edge(void *handle, int64_t timestamp, int data_0, int data_1, int data_2, int data_3, int mask);

void dpi_gpio_edge(void *handle, int64_t timestamp, int data);


#ifdef __cplusplus
}
#endif



#endif