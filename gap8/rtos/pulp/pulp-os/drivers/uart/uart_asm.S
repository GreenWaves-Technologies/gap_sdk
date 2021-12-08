/*
 * Copyright (C) 2020 GreenWaves Technologies
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
 *          Sivarajah Sivapiriyan, GreenWaves Technologies (sivarajah.sivapiriyan@greenwaves-technologies.com)
 */

#include "rt/rt_data.h"
#include "archi/pulp.h"

  // x8: callback arg, x9: channel, x10:event, x11,x12:temp
  // call __pi_uart_handle_copy(x10:event, x11:callback arg)
  .extern __pi_uart_handle_copy
  .global __pi_uart_handle_copy_asm
__pi_uart_handle_copy_asm:

//  mv        x11, x10
  mv        x11, x8
  la        x12, __pi_uart_handle_copy
  la        x9, udma_event_handler_end
  j         __rt_call_external_c_function
