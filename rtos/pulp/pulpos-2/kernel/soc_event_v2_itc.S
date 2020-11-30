#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# 
# Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
#

#include <archi/pulp.h>


    .section .text_l2

    #
    # SOC event handler entry
    #

.global pos_soc_event_handler_asm
    pos_soc_event_handler_asm:

    #
    # Context saving
    #

    # The stack is first adjusted to have stack-based load/store compressed
    add sp, sp, -128
    sw  x8, 0(sp)
    sw  x9, 4(sp)
    sw  x10, 8(sp)
    sw  x11, 12(sp)
    sw  x12, 16(sp)



    #
    # SOC event fifo handing
    #

    # Pop one element from the FIFO
    li   x8, ARCHI_FC_ITC_ADDR
    lw   x10, ITC_FIFO_OFFSET(x8)


    #
    # UDMA channel events
    #

    # We have the event ID in x10, get pointer to the corresponding event callback
    slli x11, x10, 2

#ifdef __USE_ASM_OPTIM__

    lw   x9, %tiny(pos_udma_channel_callback)(x11)
    lw   x11, %tiny(pos_udma_channel_callback_arg)(x11)
    jr   x9

#else

    lw   x12, %tiny(pos_soc_event_callback)(x11)
    lw   x11, %tiny(pos_soc_event_callback_arg)(x11)
    la   x9, pos_soc_event_handler_end_asm
    j    pos_irq_call_external_c_function

#endif




pos_soc_event_no_udma_asm:


    #
    # SOC event status register
    #

pos_soc_event_store_asm:
    # If the event is not handled, store it in the soc event status mask
    srli    x11, x10, 5
    slli    x11, x11, 2
    lw      x12, %tiny(pos_soc_event_status)(x11)
    andi    x10, x10, 0x1f

    p.bsetr x12, x12, x10

    sw      x12, %tiny(pos_soc_event_status)(x11)




    # SOC event handler exit
    #
    .global pos_soc_event_handler_end_asm
pos_soc_event_handler_end_asm:
    lw  x8, 0(sp)
    lw  x9, 4(sp)
    lw  x10, 8(sp)
    lw  x11, 12(sp)
    lw  x12, 16(sp)
    add sp, sp, 128
    mret
