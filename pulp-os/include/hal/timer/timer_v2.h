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

#ifndef __HAL_TIMER_TIMER_V2_H__
#define __HAL_TIMER_TIMER_V2_H__

#include "hal/pulp.h"
#include "archi/timer/timer_v2.h"

#if TIMER_VERSION != 2
#error This file must be included only with timer version 2
#endif


static inline unsigned int hal_timer_conf_get(
  int enable, int reset, int irq_enable, int event_mask, int cmp_clr,
  int one_shot, int clk_source, int prescaler_enable, int prescaler, int mode_64)
{
  return
      (enable << PLP_TIMER_ENABLE_BIT)
    | (reset << PLP_TIMER_RESET_BIT)
    | (irq_enable << PLP_TIMER_IRQ_ENABLE_BIT)
    | (event_mask << PLP_TIMER_IEM_BIT)
    | (cmp_clr << PLP_TIMER_CMP_CLR_BIT)
    | (one_shot << PLP_TIMER_ONE_SHOT_BIT)
    | (clk_source << PLP_TIMER_CLOCK_SOURCE_BIT)
    | (prescaler_enable << PLP_TIMER_PRESCALER_ENABLE_BIT)
    | (prescaler << PLP_TIMER_PRESCALER_VALUE_BIT)
    | (mode_64 << PLP_TIMER_64_BIT);
}

#if defined(ARCHI_FC_TIMER_ADDR)
static inline unsigned int hal_timer_fc_addr(int id, int sub_id)
{
  return ARCHI_FC_TIMER_ADDR + id * PLP_TIMER_AREA_SIZE + sub_id * 4;
}
#endif

static inline unsigned int hal_timer_addr(int id, int sub_id)
{
  return ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + id * PLP_TIMER_AREA_SIZE + sub_id * 4;
}

static inline void hal_timer_conf(
  unsigned int timer_base, int enable, int reset, int irq_enable,
  int event_mask, int cmp_clr, int one_shot, int clk_source,
  int prescaler_enable, int prescaler, int mode_64)
{
  unsigned int conf = hal_timer_conf_get(
    enable, reset, irq_enable, event_mask, cmp_clr, one_shot, clk_source,
    prescaler_enable, prescaler, mode_64
  );
  pulp_write32(timer_base + PLP_TIMER_CFG_REG_LO, conf);
}

static inline unsigned int hal_timer_count_get(unsigned int addr)
{
  return pulp_read32(addr + PLP_TIMER_VALUE_LO);
}

static inline void hal_timer_count_set(
  unsigned int addr, unsigned int value)
{
  pulp_write32(addr + PLP_TIMER_VALUE_LO, value);
}

static inline unsigned long long hal_timer_count_get_64(unsigned int addr)
{
  unsigned int msb;
  unsigned int lsb;

  // As we have to issue 2 reads to get the 64 bits values, the 64 bit MSB
  // part could get incremented while we are reading the LSB.
  // To solve that we iterate until we see a stable value in MSB.
  while (1) {
    msb = pulp_read32(addr + PLP_TIMER_VALUE_HI);
    lsb = pulp_read32(addr + PLP_TIMER_VALUE_LO);
    if (pulp_read32(addr + PLP_TIMER_VALUE_HI) == msb) break;
  }

  return (((unsigned long long)msb) << 32) | lsb;
}

static inline void hal_timer_count_set_64(
  unsigned int addr, unsigned long long value)
{
  pulp_write32(addr + PLP_TIMER_VALUE_LO, (int)value);
  pulp_write32(addr + PLP_TIMER_VALUE_HI, (int)(value >> 32));
}

static inline unsigned int hal_timer_cmp_get(unsigned int addr)
{
  return pulp_read32(addr + PLP_TIMER_CMP_LO);
}

static inline void hal_timer_cmp_set(unsigned int addr, unsigned int cmp)
{
  pulp_write32(addr + PLP_TIMER_CMP_LO, cmp);
}

static inline void hal_timer_reset(unsigned int addr)
{
  pulp_write32(addr + PLP_TIMER_RESET_LOW, 1);
}

static inline void hal_timer_start(unsigned int addr)
{
  pulp_write32(addr + PLP_TIMER_START_LOW, 1);
}




static inline unsigned int plp_timer_conf_get(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler, char mode_64)
{
  return
      (enable << PLP_TIMER_ENABLE_BIT)
    | (reset << PLP_TIMER_RESET_BIT)
    | (irq_enable << PLP_TIMER_IRQ_ENABLE_BIT)
    | (event_mask << PLP_TIMER_IEM_BIT)
    | (cmp_clr << PLP_TIMER_CMP_CLR_BIT)
    | (one_shot << PLP_TIMER_ONE_SHOT_BIT)
    | (prescaler_enable << PLP_TIMER_PRESCALER_ENABLE_BIT)
    | (prescaler << PLP_TIMER_PRESCALER_VALUE_BIT)
    | (mode_64 << PLP_TIMER_64_BIT);
}

static inline void __plp_timer_conf(unsigned int addr, char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler, char mode_64)
{
  pulp_write32(addr, plp_timer_conf_get(enable, reset, irq_enable, event_mask, cmp_clr, one_shot, prescaler_enable, prescaler, mode_64));
}

#ifdef ARCHI_HAS_FC
static inline void plp_fc_timer_raw_conf_low(unsigned int conf)
{
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_LO, conf);
}
#endif

static inline void plp_timer_raw_conf_low(unsigned int conf)
{
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_LO, conf);
}

#ifdef ARCHI_HAS_FC
static inline unsigned int plp_fc_timer_raw_conf_low_get()
{
  return pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_LO);
}
#endif

static inline unsigned int plp_timer_raw_conf_low_get()
{
  return pulp_read32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_LO);
}

#ifdef ARCHI_HAS_FC
static inline void plp_fc_timer_raw_conf_high(unsigned int conf)
{
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_HI, conf);
}
#endif

static inline void plp_timer_raw_conf_high(unsigned int conf)
{
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_HI, conf);
}

#ifdef ARCHI_HAS_FC
static inline unsigned int plp_fc_timer_raw_conf_high_get()
{
  return pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_HI);
}
#endif

static inline unsigned int plp_timer_raw_conf_high_get()
{
  return pulp_read32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_HI);
}

static inline void plp_timer_conf_low(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler, char mode_64)
{
  __plp_timer_conf(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_LO, enable, reset, irq_enable, event_mask, cmp_clr, one_shot, prescaler_enable, prescaler, mode_64);
}

static inline void plp_timer_conf_high(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler)
{
  __plp_timer_conf(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CFG_REG_HI, enable, reset, irq_enable, event_mask, cmp_clr, one_shot, prescaler_enable, prescaler, 0);
}

#ifdef ARCHI_HAS_FC
static inline void plp_fc_timer_conf_low(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler, char mode_64)
{
  __plp_timer_conf(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_LO, enable, reset, irq_enable, event_mask, cmp_clr, one_shot, prescaler_enable, prescaler, mode_64);
}

static inline void plp_fc_timer_conf_high(char enable, char reset, char irq_enable, char event_mask, char cmp_clr, char one_shot, char prescaler_enable, char prescaler)
{
  __plp_timer_conf(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CFG_REG_HI, enable, reset, irq_enable, event_mask, cmp_clr, one_shot, prescaler_enable, prescaler, 0);
}

#endif

static inline void plp_timer_cmp_low(int cmp)
{
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CMP_LO, cmp);
}

static inline void plp_timer_cmp_high(int cmp)
{
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_CMP_HI, cmp);
}

#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_cmp_low(int cmp)
{
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CMP_LO, cmp);
}

static inline void plp_fc_timer_cmp_high(int cmp)
{
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_CMP_HI, cmp);
}

#endif

#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_set_count_low(unsigned int value) {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_LO, value);
}

static inline void plp_fc_timer_set_count_high(unsigned int value) {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_HI, value);
}

#endif

static inline void plp_timer_set_count_low(unsigned int value) {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_VALUE_LO, value);
}

static inline void plp_timer_set_count_high(unsigned int value) {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_VALUE_HI, value);
}

#ifdef ARCHI_HAS_FC

static inline int plp_fc_timer_get_count_low() {
  return pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_LO);
}

static inline int plp_fc_timer_get_count_high() {
  return pulp_read32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_VALUE_HI);
}

#endif

static inline int plp_timer_get_count_low() {
  return pulp_read32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_VALUE_LO);
}

static inline int plp_timer_get_count_high() {
  return pulp_read32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_VALUE_HI);
}

#if !defined(ARCHI_HAS_FC) || defined(ARCHI_HAS_FC_ALIAS)

#if 0
// PLP_TIMER_START_LOW is not defined
static inline void start_fc_timer_low() {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_START_LOW, 1);
}

static inline void start_fc_timer_high() {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_START_HIGH, 1);
}

static inline void stop_fc_timer_low() {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_RESET_LOW, 1);
}

static inline void stop_fc_timer_high() {
  pulp_write32(ARCHI_FC_TIMER_ADDR + PLP_TIMER_RESET_HIGH, 1);
}
#endif

#else

#if 0
// PLP_TIMER_START_LOW is not defined
static inline void start_timer_low() {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_START_LOW, 1);
}

static inline void start_timer_high() {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_START_HIGH, 1);
}

static inline void stop_timer_low() {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_STOP_LOW, 1);
}

static inline void stop_timer_high() {
  pulp_write32(ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_TIMER_OFFSET + PLP_TIMER_HIGH_LOW, 1);
}
#endif

#endif


#ifdef ARCHI_HAS_FC

static inline void set_timer_fc_clock_low_32K_no_prescaler() {
  if (hal_is_fc()) plp_fc_timer_raw_conf_low(plp_fc_timer_raw_conf_low_get() | (1<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_fc_clock_high_32K_no_prescaler() {
  if (hal_is_fc()) plp_fc_timer_raw_conf_high(plp_fc_timer_raw_conf_high_get() | (1<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_fc_clock_low_fll_and_prescaler() {
  if (hal_is_fc()) plp_fc_timer_raw_conf_low(plp_fc_timer_raw_conf_low_get() | (0<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_fc_clock_high_fll_and_prescaler() {
  if (hal_is_fc()) plp_fc_timer_raw_conf_high(plp_fc_timer_raw_conf_high_get() | (0<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

#else

static inline void set_timer_clock_low_32K_no_prescaler() {
  if (hal_is_fc()) plp_timer_raw_conf_low(plp_timer_raw_conf_low_get() | (1<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_clock_high_32K_no_prescaler() {
  if (hal_is_fc()) plp_timer_raw_conf_high(plp_timer_raw_conf_high_get() | (1<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_clock_low_fll_and_prescaler() {
  if (hal_is_fc()) plp_timer_raw_conf_low(plp_timer_raw_conf_low_get() | (0<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

static inline void set_timer_clock_high_fll_and_prescaler() {
  if (hal_is_fc()) plp_timer_raw_conf_high(plp_timer_raw_conf_high_get() | (0<<PLP_TIMER_CLOCK_SOURCE_BIT));
}

#endif


#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_fire(int timer)
{
  if (timer) {
    plp_fc_timer_conf_high(1, 1, 0, 0, 0, 0, 0, 0);
  } else {
    plp_fc_timer_conf_low(1, 1, 0, 0, 0, 0, 0, 0, 0);
  }
}

#endif

static inline void plp_timer_fire(int timer)
{
  if (timer) {
    plp_timer_conf_high(1, 1, 0, 0, 0, 0, 0, 0);
  } else {
    plp_timer_conf_low(1, 1, 0, 0, 0, 0, 0, 0, 0);
  }
}

static inline void plp_timer_cancel(int timer)
{
  if (timer) {
    plp_timer_conf_high(0, 0, 0, 0, 0, 0, 0, 0);
  } else {
    plp_timer_conf_low(0, 0, 0, 0, 0, 0, 0, 0, 0);
  }
}

static inline void plp_timer_compare_set(int timer, int cmpVal)
{
  if (timer) {
    plp_timer_cmp_high(cmpVal);
    plp_timer_conf_high(1, 0, 1, 0, 1, 0, 0, 0);
  } else {
    plp_timer_cmp_low(cmpVal);
    plp_timer_conf_low(1, 0, 1, 0, 1, 0, 0, 0, 0);
  }
}

#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_compare_set(int timer, int cmpVal)
{
  if (timer) {
    plp_fc_timer_cmp_high(cmpVal);
    plp_fc_timer_conf_high(1, 0, 1, 0, 1, 0, 0, 0);
  } else {
    plp_fc_timer_cmp_low(cmpVal);
    plp_fc_timer_conf_low(1, 0, 1, 0, 1, 0, 0, 0, 0);
  }
}

#endif

static inline void plp_timer_update(int timer, int cmpVal)
{
  if (timer) {
    plp_timer_cmp_high(cmpVal);
  } else {
    plp_timer_cmp_low(cmpVal);
  }
}

#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_update(int timer, int cmpVal)
{
  if (timer) {
    plp_fc_timer_cmp_high(cmpVal);
  } else {
    plp_fc_timer_cmp_low(cmpVal);
  }
}

#endif

static inline void plp_timer_compare_unset(int timer)
{
  if (timer) plp_timer_conf_high(1, 0, 0, 0, 0, 0, 0, 0);
  else plp_timer_conf_low(1, 0, 0, 0, 0, 0, 0, 0, 0);
}

#ifdef ARCHI_HAS_FC

static inline void plp_fc_timer_compare_unset(int timer)
{
  if (timer) plp_fc_timer_conf_high(1, 0, 0, 0, 0, 0, 0, 0);
  else plp_fc_timer_conf_low(1, 0, 0, 0, 0, 0, 0, 0, 0);
}

#endif

#ifdef ARCHI_HAS_FC

static inline unsigned int plp_fc_timer_cycles(int timer) {
  if (timer) return plp_fc_timer_get_count_high();
  else return plp_fc_timer_get_count_low();

}

#endif

static inline unsigned int plp_timer_cycles(int timer) {
  if (timer) return plp_timer_get_count_high();
  else return plp_timer_get_count_low();

}

#endif
