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

#include "rt/rt_data.h"
#include "archi/pulp.h"

    .global __rt_pe_start
__rt_pe_start:

  la      t1, __rt_stacks_base
  la      t2, __rt_stacks_size

  lw      sp, 0(t1)
  lw      t2, 0(t2)
  csrr    t3, CSR_MHARTID
  andi    t3, t3, 0x1f

  addi    t3, t3, 1
  mul     t2, t2, t3
  add     sp, sp, t2

  j       __rt_pe_entry

