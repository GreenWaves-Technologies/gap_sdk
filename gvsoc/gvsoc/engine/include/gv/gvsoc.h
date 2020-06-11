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

#ifndef __GV__GVSOC_H__
#define __GV__GVSOC_H__

#ifdef __cplusplus
extern "C" {
#endif

void *gv_open(const char *config_path, bool open_proxy, int *proxy_socket, int req_pipe, int reply_pipe);

void *gv_create(const char *config_path);

void gv_start(void *instance, bool open_proxy, int *proxy_socket, int req_pipe, int reply_pipe);

int gv_run(void *_instance);

void gv_stop(void *_instance);

void *gv_chip_pad_bind(void *handle, char *name, int ext_handle);

#ifdef __cplusplus
}
#endif

#endif
