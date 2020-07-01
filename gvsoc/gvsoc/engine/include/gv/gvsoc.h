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
