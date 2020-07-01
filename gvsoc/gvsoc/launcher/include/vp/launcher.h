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

//void gv_init(gv_conf_t *gv_conf);

//void *gv_create(gv_conf_t *gv_conf, char *config_file);

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