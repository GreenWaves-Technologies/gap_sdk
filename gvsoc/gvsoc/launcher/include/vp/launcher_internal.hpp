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

#ifndef __VP_LAUNCHER_INTERNAL_HPP_
#define __VP_LAUNCHER_INTERNAL_HPP_

#include "vp/launcher.h"
#include <stdint.h>

#define GV_IOREQ_DESC_TYPE_REQUEST  0
#define GV_IOREQ_DESC_TYPE_RESPONSE 1

typedef struct {
  int64_t              type;
  uint64_t             addr;
  uint64_t             size;
  int64_t              is_write;
  int64_t              timestamp;
  gv_ioreq_response_t  response_cb;
  void                *response_context;
  void                *binding;
  void                *req;
  gv_ioreq_t           user_req;
  int64_t              latency;
  void                *data;
} gv_ioreq_desc_t;

#endif