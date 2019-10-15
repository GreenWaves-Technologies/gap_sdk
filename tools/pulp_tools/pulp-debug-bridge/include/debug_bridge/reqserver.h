/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies SAS
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
 * Authors: Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#ifndef __REQSERVER_H__
#define __REQSERVER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
  REQSERVER_READMEM_REQ  = 0,
  REQSERVER_WRITEMEM_REQ  = 1,
  REQSERVER_MAX_REQ_NUM = REQSERVER_WRITEMEM_REQ 
} reqserver_req_type_e;

typedef enum {
  REQSERVER_READMEM_RSP  = 0,
  REQSERVER_WRITEMEM_RSP  = 1,
  REQSERVER_ALERT_RSP  = 2,
  REQSERVER_RESET_RSP  = 3,
  REQSERVER_ERROR_RSP  = 4
} reqserver_rsp_type_e;

typedef struct {
  int32_t type;
  int trans_id;
  unsigned int addr;
  int len;
} reqserver_req_t;

typedef struct {
  reqserver_req_t req;
  char data[];
} reqserver_write_req_t;

typedef struct {
  int32_t type;
  int trans_id;
} reqserver_rsp_t;

typedef struct {
  reqserver_rsp_t rsp;
  int len;
} reqserver_rsp_payload_t;

typedef struct {
  reqserver_rsp_payload_t rsp_payload;
  char data[];
} reqserver_read_rsp_t;

#ifdef __cplusplus
}
#endif

#endif