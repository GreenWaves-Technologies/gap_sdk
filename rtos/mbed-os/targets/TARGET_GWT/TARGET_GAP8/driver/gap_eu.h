/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GAP_EU_H_
#define _GAP_EU_H_

#include <GAP8.h>

/*!
 * @addtogroup event_unit
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @name Event management
 *  The following function can be used for selecting which events can wake-up the core, put the core the sleep, clear events and so on.
 */
/**@{*/

/** Clock-gated load.
 * On architectures that support it, this executes the special p.elw instruction which is used to put the core in idle mode until it receives an event.
  On other architectures, it executes a classic read which has the same effect but does not handle correctly interrupts while the core is sleeping.
  \param base   The base address of the event unit.
  \param offset The offset in the event unit where to load from. Depending on this offset, this will trigger different behaviors (barrier, wait, wait&clear, etc).
  \return       The loaded value, after the core has been waken-up. This value depends on the feature which is accessed.
  */
static inline uint32_t EVT_Read32(uint32_t base, uint32_t offset)
{
  uint32_t value;
  value = __builtin_pulp_event_unit_read((int *)base, offset);
  return value;
}

/** Get event status.
  Return the value of the event status register. This register contains one bit per event, 1 means the event is set. Note that this register is actually used
  as a status register for both events and interrupts, so clearing one will clear the other. This register is a buffer, which means if an event is received
  while its bit is already at 1, the event is somehow lost as the bit stays at 1. Thus events should be used as Linux signals.
  \return       The content of the status register.
  */
static inline uint32_t EU_EVT_Status()
{
  return EU_CORE_DEMUX->BUFFER;
}

/** Get active events.
  Same as eu_evt_status, but the content of the status register is ANDed with the content of the event mask register so that
  only relevant events (that can wake-up the core) are shown. It can for example be used to know which events must be handled
  after a core wake-up as the core has been waken-up due to one of them.
  \return       The content of the status register ANDed with the event mask register.
  */
static inline uint32_t EU_EVT_StatusMasked()
{
  return EU_CORE_DEMUX->BUFFER_MASKED;
}

/** Clear some bits of the event status register.
  This allows clearing events after they have been received so that they do not prevent the core from sleeping anymore.
  \param evtMask Bits to be cleared. 1 means the bit must be cleared, 0 it should remain the same.
  */
static inline void EU_EVT_Clr(uint32_t evtMask)
{
  EU_CORE_DEMUX->BUFFER_CLEAR = evtMask;
}

/** Modify the event mask.
  The event mask tells the event unit which events can wake-up the core. When the core executes the p.elw instruction it goes to sleep unless
  there is one bit which is set in both the event mask and the event status register. If the event is received while the core is already sleeping
  and the corresponding bit is set in the event mask, the core is waken-up and cannot go back to sleep until either the event mask or the event status
  register is modified (usually the bit is cleared using eu_evt_clr)
  \param evtMask Value to write to the event mask. There is 1 bit per event, 1 means it can wake-up the core, 0 it cannot
  */
static inline void EU_EVT_Mask(uint32_t evtMask)
{
  EU_CORE_DEMUX->MASK = evtMask;
}

/** Set part of the event mask to 1.
  This is similar to eu_evt_mask but for each bit at 1 in the new value, the corresponding bit is set in the event mask and the others remain the same.
  This is useful when the mask must be updated before waiting for a specific event without modifying the other events (this saves a few instructions)
  \param evtMask Bit mask used to update the event mask. There is 1 bit per event, 1 means the corresponding bit is set in the event mask.
  */
static inline void EU_EVT_MaskSet(uint32_t evtMask)
{
  EU_CORE_DEMUX->MASK_OR = evtMask;
}

/** Set part of the event mask to 0.
  This is the opposite of eu_evt_maskSet. For each bit at 1 in the new value, the corresponding bit is set to 0 in the event mask and the others remain the same.
  \param evtMask Bit mask used to update the event mask. There is 1 bit per event, 1 means the corresponding bit is cleared in the event mask.
  */
static inline void EU_EVT_MaskClr(uint32_t evtMask)
{
  EU_CORE_DEMUX->MASK_AND = evtMask;
}

/** Put the core to sleep mode until it receives an event
  The core goes to sleep until there is one bit at 1 in both the event status register and the event mask register. If it is already the case, when this function is called,
  the core does not go to sleep and this function returns immediately.
  */
static inline uint32_t EU_EVT_Wait()
{
  return EVT_Read32((uint32_t)&EU_CORE_DEMUX->EVENT_WAIT, 0);
}

/** Put the core to sleep mode until it receives an event and clears the active events
  This is similar to eu_evt_wait but when the core is waken-up, the active events are cleared in the status register. An event is active if its bit is at 1 in both
  the event status register and the event mask register.
  */
static inline uint32_t EU_EVT_WaitAndClr()
{
  return EVT_Read32((uint32_t)&EU_CORE_DEMUX->EVENT_WAIT_CLEAR, 0);
}

/** Modify the event mask, put the core to sleep mode until it receives an event, clears the active events and restore the mask.
  This is similar to eu_evt_waitAndClr but the events whose bit is set to 1 in the given mask, are activated in the event mask before going to sleep, and are cleared
  when the core is waken-up.
  Not that this function is using 3 instructions instead of 1 for most of other primitives, as it needs 2 instruction to set and clear the mask.
  \param evtMask Event mask used for activating and deactivating events. Each bit to 1 means the corresponding event is activated before sleeping and deactivated after.
  */
static inline uint32_t EU_EVT_MaskWaitAndClr(uint32_t evtMask)
{
  EU_EVT_MaskSet(evtMask);
  uint32_t result = EU_EVT_WaitAndClr();
  EU_EVT_MaskClr(evtMask);
  return result;
}

static inline uint32_t EU_FC_EVT_MaskWaitAndClr(uint32_t evtMask)
{
  EU_EVT_MaskSet(evtMask);
  uint32_t result;
  do {
    result = EU_EVT_WaitAndClr();
  } while (result == 0);
  EU_EVT_MaskClr(evtMask);
  return result;
}

//!@}




/** @name Interrupt management
 *  Functions for setting-up which IRQ can trigger an interrupt handler, clear interrupts and so on
 */
/**@{*/

static inline uint32_t EU_EVT_StatusIrqMasked()
{
  return EU_CORE_DEMUX->BUFFER_IRQ_MASKED;
}

static inline void EU_IRQ_Mask(uint32_t irqMask)
{
  EU_CORE_DEMUX->MASK_IRQ = irqMask;
}

static inline void EU_IRQ_SecMask(uint32_t irqMask)
{
  EU_CORE_DEMUX->MASK_SEC_IRQ = irqMask;
}

static inline void EU_IRQ_MaskSet(uint32_t irqMask)
{
  EU_CORE_DEMUX->MASK_IRQ_OR = irqMask;
}

static inline void EU_IRQ_MaskClr(uint32_t irqMask)
{
  EU_CORE_DEMUX->MASK_IRQ_AND = irqMask;
}

static inline void EU_IRQ_SecMaskSet(uint32_t irqMask)
{
  EU_SEC_DEMUX->MASK_OR = irqMask;
}

static inline void EU_IRQ_SecMaskClr(uint32_t irqMask)
{
  EU_SEC_DEMUX->MASK_AND = irqMask;
}

static inline uint32_t EU_IRQ_Status()
{
  return EU_CORE_DEMUX->BUFFER_IRQ_MASKED;
}

//!@}


/** @name SW event trigger.
 *  Functions for triggering SW events which can be used to wake-up cluster cores from SW.
 */
/**@{*/

#ifdef FEATURE_CLUSTER
static inline void EU_CLUSTER_EVT_TrigSet(int event_num, uint32_t value)
{
  CLUSTER_EU_SW_EVENTS->TRIGGER_SET[event_num] = value;
}
#endif

static inline void EU_FC_EVT_TrigSet(int event_num, uint32_t value)
{
  FC_EU_SW_EVENTS->TRIGGER_SET[event_num] = value;
}

static inline void EU_EVT_DemuxTrigSet(int event_num, uint32_t value)
{
  EU_SW_EVENTS_DEMUX->TRIGGER_SET[event_num] = value;
}

//!@}


/** @name Hardware barriers.
 *  Functions for managing the hardware barriers which can be used to implement a rendez-vous
 */
/**@{*/

// Convert barrier ID into barrier address, that can be used more efficiently by
// subsequent operations (usually leads to only 1 lw or sw)

// All-in-one barrier: Trigger barrier notif, wait for barrier and clear the event
// This also return the pending active events.
// Barrier event must be active for this to work
static inline uint32_t EU_BarrierTriggerWaitClear()
{
    return EVT_Read32((uint32_t)&EU_BARRIER_DEMUX->TRIGGER_WAIT_CLEAR, 0);
}

static inline void EU_BarrierSetup(uint32_t coreMask)
{
    EU_BARRIER_DEMUX->TRIGGER_MASK = coreMask;
    EU_BARRIER_DEMUX->TARGET_MASK = coreMask;
}

static inline void EU_BarrierTrigger(uint32_t mask)
{
    EU_BARRIER_DEMUX->TRIGGER = mask;
}

//!@}

/** @name SoC events FIFO.
 *  Functions for managing SoC events coming from outside the cluster and pushed by the HW to the SoC event FIFO.
 */
/**@{*/

static inline uint32_t EU_SOC_EventsPop()
{
  return EU_SOC_EVENTS->CURRENT_EVENT;
}

static inline uint32_t FC_EU_SOC_EventsPop()
{
  return EU_SOC_EVENTS->CURRENT_EVENT;
}

static inline int32_t EU_SOC_EventsIsValid(uint32_t word)
{
  return ((word & EU_CURRENT_VALID_BIT_MASK) >> EU_CURRENT_VALID_BIT_SHIFT);
}

static inline uint32_t EU_SOC_EventsGetEvent(uint32_t word)
{
  return word & EU_CURRENT_SOC_EVENT_MASK;
}

//!@}


/** @name Hardware mutexes.
 *  Functions for managing, locking and unlocking hardware mutexes
 */
/**@{*/
static inline void EU_MutexLock(uint32_t mutexID)
{
  EVT_Read32((uint32_t)&EU_MUTEX_DEMUX->MUTEX[mutexID], 0);
}

static inline void EU_MutexUnlock(uint32_t mutexID)
{
  __asm__ __volatile__ ("" : : : "memory");
  EU_MUTEX_DEMUX->MUTEX[mutexID] = 0;
  __asm__ __volatile__ ("" : : : "memory");
}

static inline void EU_MutexInit(uint32_t mutexID)
{
    EU_MUTEX_DEMUX->MUTEX[mutexID] = 0;
}

//!@}

/** @name Disable UDMA Peripherals' Interrupt.
 *  Function for manage UDMA Interrupt, for UDMA peripheral transfer, every UDMA peripheral's transfer is depend on its event and
 *  this event can be delivered to FC core to be an interrupt if FC_SOC_EVENT_IRQn is enable, so if user want the UDMA
 * peripherals' interrupt to be masked while allows system interrupt (SYSTICK etc.) to continue, this function should be used.
 */
/**@{*/
static inline int EU_DisableUDMAIRQ()
{
    int ret = NVIC_GetEnableIRQ(FC_SOC_EVENT_IRQn);
    /* Disable IRQ */
    NVIC_DisableIRQ(FC_SOC_EVENT_IRQn);
    return ret;
}

/** @name Restore UDMA Peripherals' Interrupt.
 *  Function for manage UDMA Interrupt, for UDMA peripheral transfer, every UDMA peripheral's transfer is depend on its event and
 *  this event can be delivered to FC core to be an interrupt if FC_SOC_EVENT_IRQn is enable, so if user want the UDMA
 *  peripherals' interrupt to be masked while allows system interrupt (SYSTICK etc.) to continue,
 *  the disble function should be used. To restore the interrupt status, use this function
 *  param enable  previous UDAM Interrupt status to restore
 */
static inline void EU_RestoreUDMAIRQ(int enable)
{
    /* Restore IRQ */
    if (enable)
        NVIC_EnableIRQ(FC_SOC_EVENT_IRQn);
}

/** @name Put FC core in sleep and wake until next UDMA peripheral's event
 *
 */
static inline void EU_UDMASleep()
{
    EU_EVT_MaskWaitAndClr(1 << FC_SW_NOTIFY_EVENT);
}

//!@}


/** @name Hardware dispatcher.
 *  Functions for managing the hardware dispatcher, which can be used to dispatch entry points to sleeping slave cores.
 */
/**@{*/

static inline uint32_t EU_DispatchPop()
{
  return EVT_Read32((uint32_t)&EU_DISPATCH_DEMUX->FIFO_ACCESS, 0);
}

static inline void EU_DispatchPush(uint32_t value)
{
  EU_DISPATCH_DEMUX->FIFO_ACCESS = value;
}

static inline void EU_Dispatch_TeamConfig(uint32_t value)
{
  EU_DISPATCH_DEMUX->TEAM_CONFIG = value;
}

static inline uint32_t EU_Dispatch_ReadTeamConfig()
{
  return EU_DISPATCH_DEMUX->TEAM_CONFIG;
}

//!@}



/** @name Hardware loops (NOT YET AVAILABLE ON HW).
 *  Functions for managing the hardware loops, which can be used to dispatch a range of indexes to a core set.
 */
/**@{*/

static inline uint32_t EU_LoopAddr(uint32_t id)
{
  return CORE_EU_LOOP_DEMUX_BASE;
}

static inline uint32_t EU_LoopGetChunk()
{
  return EU_LOOP_DEMUX->CHUNK;
}

static inline uint32_t EU_LoopGetStart()
{
  return EU_LOOP_DEMUX->START;
}

static inline uint32_t EU_LoopGetState()
{
  return EU_LOOP_DEMUX->STATE;
}

static inline void EU_LoopSetStart(uint32_t value)
{
  EU_LOOP_DEMUX->START = value;
}

static inline void EU_LoopSetEnd(uint32_t value)
{
  EU_LOOP_DEMUX->END = value;
}

static inline void EU_LoopSetIncr(uint32_t value)
{
  EU_LOOP_DEMUX->INCR = value;
}

static inline void EU_Loop_SetChunk(uint32_t value)
{
  EU_LOOP_DEMUX->CHUNK = value;
}

static inline void EU_Loop_InitEpoch(uint32_t coreSet)
{
  EU_LOOP_DEMUX->EPOCH = coreSet;
}

static inline uint32_t EU_LoopGetSingle()
{
  return EU_LOOP_DEMUX->SINGLE;
}

//!@}

//!@}

static inline uint32_t EU_EVT_GetClusterBase(uint32_t coreId)
{
  return EU_EVT_GETCLUSTERBASE(coreId);
}

#endif /*_GAP_EU_H_*/
