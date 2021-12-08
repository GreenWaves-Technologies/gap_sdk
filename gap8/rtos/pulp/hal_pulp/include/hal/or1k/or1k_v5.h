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

#ifndef __HAL_OR1K_OR1K_V5_H__
#define __HAL_OR1K_OR1K_V5_H__


#include "archi/or1k/spr-defs.h"

static inline void hal_mtspr(unsigned long int  spr, unsigned long int  value) {       
  __asm__ __volatile__ ("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}

static inline unsigned long int hal_mfspr(unsigned long spr) {       
  unsigned long value;
  __asm__ __volatile__ ("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
  return value;
}


static inline unsigned int hal_cluster_id() {
  unsigned int value;
  __asm__ ("l.mfspr\t\t%0,r0,%1" : "=r" (value) : "I" (SPR_CLUSTER_ID));
  return value;
}

static inline unsigned int hal_has_fc() {
#ifdef PLP_HAS_FC
  return 1;
#else
  return 0;
#endif
}

static inline unsigned int hal_is_fc() {
  if (hal_has_fc()) return hal_cluster_id() == 32;
  else return 0;
}


static inline unsigned int hal_core_id() {
  unsigned int value;
  __asm__ ("l.mfspr\t\t%0,r0,%1" : "=r" (value) : "I" (SPR_CORE_ID));
  return value;
}

#include "hal/eu/eu_v1.h"

static inline int hal_irq_disable()
{
  unsigned int sr = hal_mfspr(SPR_SR);
  hal_mtspr(SPR_SR, sr & ~SPR_SR_IEE);
  return sr;
}

static inline void hal_irq_restore(int state)
{
  hal_mtspr(SPR_SR, state);
}

static inline void hal_irq_enable()
{
  hal_mtspr(SPR_SR, hal_mfspr(SPR_SR) | (SPR_SR_IEE));
}

static inline unsigned int hal_spr_read(unsigned long spr) {       
  unsigned long value;
  __asm__ __volatile__ ("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
  return value;
}

#if 0

#include "archi/pulp.h"
#include "hal/or10nv2_builtins.h"
#include "or10n5/spr-defs.h"

static inline unsigned int hal_mepc_read() {
  return 0;
}

static inline void hal_mtspr(unsigned long int  spr, unsigned long int  value) {       
  __asm__ __volatile__ ("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}

static inline void set_SR(unsigned int value) { 
  __asm__ __volatile__ ("l.mtspr\t\tr0,%0,%1": : "r" (value), "I" (SPR_SR));
}

static inline unsigned int get_SR() {
  unsigned int value;
  __asm__ ("l.mfspr\t\t%0,r0,%1" : "=r" (value) : "I" (SPR_SR));
  return value;
}

static inline unsigned int core_id() {
  unsigned int value;
  __asm__ ("l.mfspr\t\t%0,r0,%1" : "=r" (value) : "I" (SPR_CORE_ID));
  return value;
}

static inline unsigned int cluster_id() {
  unsigned int value;
  __asm__ ("l.mfspr\t\t%0,r0,%1" : "=r" (value) : "I" (SPR_CLUSTER_ID));
  return value;
}

// Due to a HW bug with interrupt enable bit, we cannot use it.
// So in order to mask interrupts we use the mask in the event unit.

#include "hal/eu_v1.h"

static inline int hal_irq_disable()
{
  int core_id = hal_core_id();
  int state = pulp_irq_mask_low_read(core_id);
  pulp_irq_mask_low_set(core_id, 0);
  // As we are deactivating the interrupts in a peripheral,
  // we have to put some nops to make sure interrupts are really inactive
  // when we execute the next code.
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  __asm__ volatile ("l.nop" :  :  : "memory");
  return state;
}

static inline void hal_irq_restore(int state)
{
  pulp_irq_mask_low_set(hal_core_id(), state);
}

static inline void hal_irq_enable()
{
}
#endif

/*
 * PERFORMANCE COUNTERS
 * 
 * API for accessing performance counters registers.
 * Have a look at file spr-defs.h to speficy registers through defines
 * SPR_PCER_* and SPR_PCMR_*
 */

#define PCER_NB_EVENTS SPR_PCER_NB_EVENTS
#define PCER_ALL_EVENTS_MASK SPR_PCER_ALL_EVENTS_MASK
#define CSR_PCER_ALL_EVENTS_MASK PCER_ALL_EVENTS_MASK
#define PCMR_ACTIVE SPR_PCMR_ACTIVE
#define PCMR_SATURATE SPR_PCMR_SATURATE
 

/* Configure the active events. eventMask is an OR of events got through SPR_PCER_EVENT_MASK */
static inline void cpu_perf_conf_events(unsigned int eventMask)
{
  hal_mtspr(SPR_PCER, eventMask);
}

/* Return events configuration */
static inline unsigned int cpu_perf_conf_events_get()
{
  return hal_mfspr(SPR_PCER);
}

/* Configure the mode. confMask is an OR of all SPR_PCMR_* macros */
static inline void cpu_perf_conf(unsigned int confMask)
{
  hal_mtspr(SPR_PCMR, confMask);
}

/* Starts counting in all counters. As this is using the mode register,
 * the rest of the config can be given through conf parameter */
static inline void cpu_perf_start(unsigned int conf) {
  hal_mtspr(SPR_PCMR, SPR_PCMR_ACTIVE | conf);
}

/* Stops counting in all counters. As this is using the mode register,
 * the rest of the config can be given through conf parameter */
static inline void cpu_perf_stop(unsigned int conf) {
  hal_mtspr(SPR_PCMR, conf);
}

/* Set the specified counter to the specified value */
static inline void cpu_perf_set(unsigned int counterId, unsigned int value) {
  hal_mtspr(SPR_PCCR(counterId), value);
}

/* Set all counters to the specified value */
static inline void cpu_perf_setall(unsigned int value) {
  hal_mtspr(SPR_PCCR(31), value);
}

/* Return the value of the specified counter */
static inline unsigned int cpu_perf_get(unsigned int counterId) {
  return hal_mfspr(SPR_PCCR(counterId));
}

static inline char *cpu_perf_name(int event) {
  switch (event) 
    {
    case 0: return "CYCLES";
    case 1: return "INSTR";
    case 2: return "LD_STALL";
    case 3: return "JMP_STALL";
    case 4: return "IMISS";
    case 5: return "WBRANCH";
    case 6: return "WBRANCH_CYC";
    case 7: return "LD";
    case 8: return "ST";
    case 9: return "JUMP";
    case 10: return "BRANCH";
    case 11: return "DELAY_NOP";
    case 12: return "LD_EXT";
    case 13: return "ST_EXT";
    case 14: return "LD_EXT_CYC";
    case 15: return "ST_EXT_CYC";
    case 16: return "TCDM_CONT";
    }
  return (char *)0;
}

static inline unsigned int __builtin_bitinsert(unsigned int dst, unsigned int src, unsigned int size, unsigned int off) {
  return ((dst & ~(((1<<size)-1)<<off)) | (src << off));
}


#endif
