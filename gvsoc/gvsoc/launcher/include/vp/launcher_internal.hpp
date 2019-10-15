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