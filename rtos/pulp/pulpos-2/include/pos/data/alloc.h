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


#ifndef __POS_DATA_ALLOC_H__
#define __POS_DATA_ALLOC_H__

#ifndef LANGUAGE_ASSEMBLY

typedef struct pos_alloc_block_s
{
  	int                      size;
  	struct pos_alloc_block_s *next;
} pos_alloc_chunk_t;

typedef struct 
{
  	pos_alloc_chunk_t *first_free;
#ifdef ARCHI_MEMORY_POWER
  	uint32_t track_pwd;
  	uint32_t *pwd_count;
  	uint32_t *ret_count;
  	uint32_t bank_size_log2;
  	uint32_t first_bank_addr;
#endif
} pos_alloc_t;


struct pi_cl_alloc_req_s
{
  void *result;
  int size;
  pi_task_t task;
  char done;
  char cid;
  char is_l2;
};


struct pi_cl_free_req_s
{
  void *result;
  int size;
  void *chunk;
  pi_task_t task;
  char done;
  char cid;
  char is_l2;
};


#endif

#endif