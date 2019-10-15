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

#ifndef __VP_LAUNCHER_HPP_
#define __VP_LAUNCHER_HPP_

#include <stddef.h>
#include <stdint.h>

typedef enum {
  GV_CONF_NO_TIMING = 0
} gv_conf_timing_e;

typedef struct {
  gv_conf_timing_e timing;
} gv_conf_t;

#ifdef __cplusplus
extern "C" {
#endif

void gv_init(gv_conf_t *gv_conf);

void *gv_create(gv_conf_t *gv_conf, char *config_file);

int gv_launch(void *handle);

void gv_destroy(void *handle);

#ifdef __cplusplus
}
#endif





typedef enum {
  GV_IOREQ_GRANTED,
  GV_IOREQ_DONE,
  GV_IOREQ_DONE_ERROR
} gv_ioreq_state_e;

typedef struct {
  gv_ioreq_state_e state;
  int latency;
} gv_ioreq_t;

typedef void (*gv_ioreq_response_t)(void *context, gv_ioreq_t *req);
typedef void (*gv_ioreq_request_t)(void *context, void *data, void *addr, size_t size, int is_write, gv_ioreq_response_t callback, void *cb_context);

#ifdef __cplusplus
extern "C" {
#endif

void *gv_ioreq_binding(void *handle, char *path, void *base, size_t size, gv_ioreq_request_t callback, void *context);

int gv_ioreq(void *binding, int64_t timestamp, void *data, void *addr, size_t size, int is_write, gv_ioreq_response_t callback, void *context);

#ifdef __cplusplus
}
#endif



#endif