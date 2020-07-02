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
