#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
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
    # Soc event extraction
    #

    # Pop one element from the FIFO
    li  x8, ARCHI_EU_ADDR + EU_SOC_EVENTS_AREA_OFFSET + EU_SOC_EVENTS_CURRENT_EVENT
    lw  x8, 0(x8)

    # Now that we popped the element, we can clear the soc event FIFO event as the FIFO is generating
    # an event as soon as the FIFO is not empty
    li  x9, 1<<PULP_SOC_EVENTS_EVENT
    li  x10, ARCHI_EU_DEMUX_ADDR + EU_CORE_BUFFER_CLEAR
    sw  x9, 0(x10)

    # Extract ID part
    p.extractu x10, x8, EU_SOC_EVENTS_EVENT_MASK_BITS-1, EU_SOC_EVENTS_EVENT_MASK_OFFSET



    #
    # UDMA channel events
    #

    # UDMA CHANNEL EVENTS
    li x9, ARCHI_SOC_EVENT_UDMA_NB_EVT
    bge x10, x9, pos_soc_event_no_udma_channel_asm

    # We have the channel ID in x10, get pointer to the corresponding channel
    slli x11, x10, 2

#ifdef __USE_ASM_OPTIM__

    lw   x9, %tiny(pos_udma_channel_callback)(x11)
    lw   x11, %tiny(pos_udma_channel_callback_arg)(x11)
    jr   x9

#else

    lw   x12, %tiny(pos_udma_channel_callback)(x11)
    lw   x11, %tiny(pos_udma_channel_callback_arg)(x11)
    la   x9, pos_soc_event_handler_end_asm
    j    pos_irq_call_external_c_function

#endif




    #
    # UDMA channel extra events
    #

pos_soc_event_no_udma_channel_asm:

    li x9, ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT + ARCHI_SOC_EVENT_UDMA_NB_EXTRA_EVT
    bge x10, x9, pos_soc_event_no_udma_asm

    addi x8, x10, -ARCHI_SOC_EVENT_UDMA_FIRST_EXTRA_EVT
    slli x11, x8, 2

#ifdef __USE_ASM_OPTIM__

    lw   x9, %tiny(pos_udma_extra_callback)(x11)
    lw   x11, %tiny(pos_udma_extra_callback_arg)(x11)
    jr   x9

#else

    lw   x12, %tiny(pos_udma_extra_callback)(x11)
    lw   x11, %tiny(pos_udma_extra_callback_arg)(x11)
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



    #
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
