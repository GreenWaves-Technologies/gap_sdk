/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and
 * GreenWaves Technologies
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

 .global __rt_rtc_handler
__rt_rtc_handler:
// The stack is first adjusted to have stack-based load/store compressed
  add sp, sp, -128
  sw  x8, 0(sp)
  sw  x9, 4(sp)
  sw  x10, 8(sp)
  sw  x11, 12(sp)
  sw  x12, 16(sp)


  lw    x11, %tiny(__rtc_handler)(x0)
  jal   x9, __rt_event_enqueue


  lw  x8, 0(sp)
  lw  x9, 4(sp)
  lw  x10, 8(sp)
  lw  x11, 12(sp)
  lw  x12, 16(sp)
  add sp, sp, 128
  mret