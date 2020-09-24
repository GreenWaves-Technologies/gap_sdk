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

#ifndef __GV__GVSOC_H__
#define __GV__GVSOC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


struct gv_conf {
    bool open_proxy;
    int *proxy_socket;
    int req_pipe;
    int reply_pipe;
};


void gv_init(struct gv_conf *gv_conf);


void *gv_open(const char *config_path, bool open_proxy, int *proxy_socket, int req_pipe, int reply_pipe);

void *gv_create(const char *config_path, struct gv_conf *conf);

void gv_start(void *instance);

void gv_step(void *instance, int64_t timestamp);

int64_t gv_time(void *instance);

int gv_run(void *_instance);

void gv_stop(void *_instance);

void gv_destroy(void *handle);

void *gv_chip_pad_bind(void *handle, char *name, int ext_handle);

#ifdef __cplusplus
}
#endif

#endif
