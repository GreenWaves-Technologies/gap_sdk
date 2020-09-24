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

#include "sa_iss.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define MEMORY_SIZE (16*1024*1024)



int main(int argc, char **argv)
{
  iss_t *iss;
  iss_reg_t bootaddr;

  iss = new iss_t;

  iss->fast_mode = 0;
  iss->mem_size = MEMORY_SIZE;
  iss->mem_array = (unsigned char *)malloc(MEMORY_SIZE);
  if (iss->mem_array == NULL) return -1;

  if (load_binary(iss, argv[1], argc, argv, &bootaddr))
    return -1;

  iss->cpu.config.isa = strdup("rv32imcXpulpv2");

  if (iss_open(iss)) return -1;

  iss_start(iss);
 
  iss_pc_set(iss, bootaddr);

#ifdef FAST_LOOP

  // Experimental, not yet fully stable

  do
  {
    iss->fast_mode = iss_exec_switch_to_fast(iss);

    if (iss->fast_mode)
    {
      // The fast mode is only executing instructions
      // and can be used when there is no check to do
      // like interupts, HW loops or performance
      // counters
      do
      {
        iss_exec_step(iss);
      } while(iss->fast_mode);
    }
    else
    {
      // The full mode is checking everything. This could
      // also always be called to simplify
      iss_exec_step_check_all(iss);
    }
  } while (iss->hit_exit == 0);

#else
  
  while (iss->hit_exit == 0)
  {
    iss_exec_step_check_all(iss);
  }

#endif

  return iss->exit_status;
}
