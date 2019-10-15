/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#include "rt/rt_api.h"

#define TRIM_CFG_SIZE 532

#define MRAM_CMD_TRIM_CFG    0x01
#define MRAM_CMD_NORMAL_TX   0x02
#define MRAM_CMD_ERASE_CHIP  0x04
#define MRAM_CMD_ERASE_SECT  0x08
#define MRAM_CMD_ERASE_WORD  0x10
#define MRAM_CMD_PWDN        0x20
#define MRAM_CMD_READ_RX     0x40
#define MRAM_CMD_REF_LINE_P  0x80
#define MRAM_CMD_REF_LINE_AP 0xC0


#define SECTOR_ERASE 0
#define NUM_PULSE    1



static inline void __rt_mram_read_exec(rt_mram_t *mram, void *data, void *addr, size_t size)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_READ_RX;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_rx_daddr_set(periph_base, (((unsigned int)addr) + ARCHI_MRAM_ADDR) >> 3);

  plp_udma_enqueue(periph_base + UDMA_CHANNEL_RX_OFFSET, (int)data, size, UDMA_CHANNEL_CFG_EN);
}



static inline void __rt_mram_program_exec(rt_mram_t *mram, void *data, void *addr, size_t size)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_NORMAL_TX;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_tx_daddr_set(periph_base, (((unsigned int)addr) + ARCHI_MRAM_ADDR) >> 3);

  plp_udma_enqueue(periph_base + UDMA_CHANNEL_TX_OFFSET, (int)data, size, UDMA_CHANNEL_CFG_EN);
}



static inline void __rt_mram_erase_exec(rt_mram_t *mram, void *addr, size_t size)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_ERASE_WORD;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_erase_addr_set(periph_base, (((unsigned int)addr) + ARCHI_MRAM_ADDR) >> 3);
  udma_mram_erase_size_set(periph_base, (size >> 3) - 1);
  udma_mram_trigger_set(periph_base, 0x1);
}



static inline void __rt_mram_erase_sector_exec(rt_mram_t *mram, void *addr)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_ERASE_SECT;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_erase_addr_set(periph_base, (((unsigned int)addr) + ARCHI_MRAM_ADDR) >> 3);
  udma_mram_trigger_set(periph_base, 0x1);
}



static inline void __rt_mram_erase_chip_exec(rt_mram_t *mram)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_ERASE_CHIP;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_trigger_set(periph_base, 0x1);
}



static inline void __rt_mram_trim_cfg_exec(rt_mram_t *mram, void *data, void *addr, size_t size)
{
  unsigned int periph_base = mram->periph_base;

  udma_mram_mram_mode_t mode = { .raw = udma_mram_mram_mode_get(periph_base) };
  mode.cmd = MRAM_CMD_TRIM_CFG;
  udma_mram_mram_mode_set(periph_base, mode.raw);

  udma_mram_tx_daddr_set(periph_base, (((unsigned int)addr) + ARCHI_MRAM_ADDR) >> 3);

  plp_udma_enqueue(periph_base + UDMA_CHANNEL_TX_OFFSET, (int)data, size, UDMA_CHANNEL_CFG_EN);
}



static void __rt_mram_free(rt_mram_t *mram)
{
  if (mram != NULL) {
    rt_free(RT_ALLOC_FC_DATA, (void *)mram, sizeof(rt_mram_t));
  }
}



static void __rt_mram_do_trim(rt_mram_t *mram, void *_trim_cfg_buff)
{
  unsigned int *trim_cfg_buff = _trim_cfg_buff;

  // Init the CFG zone in L2 to scan the TRIM CGF in the MRAM
  // TODO this should come from efuses
  for(int index = 0; index < 133; index ++)
  {
      trim_cfg_buff[index] = 0x00000000u;
  }

  // Write the info to num Pulses and Sector enable
  trim_cfg_buff[123] = (SECTOR_ERASE << 24) | ( NUM_PULSE << 17 );

  // section erase_en = cr_lat[3960]
  // prog_pulse_cfg   = cr_lat[3955:3953]
  //                    cr_lat[3955:3953]= 000 => number of program pulse = 8 (the default)
  //                    cr_lat[3955:3953]= 001 => number of program pulse = 1
  //                    cr_lat[3955:3953]= 010 => number of program pulse = 2
  //                    cr_lat[3955:3953]= 011 => number of program pulse = 3
  //                    cr_lat[3955:3953]= 100 => number of program pulse = 4
  //                    cr_lat[3955:3953]= 101 => number of program pulse = 5
  //                    cr_lat[3955:3953]= 110 => number of program pulse = 6
  //                    cr_lat[3955:3953]= 111 => number of program pulse = 7

  rt_event_t *event = rt_event_get_blocking(NULL);

  rt_periph_copy_t *copy = &event->implem.copy;
  mram->first_pending_copy = copy;
  mram->last_pending_copy = copy;
  copy->next = NULL;
  copy->event = event;

  __rt_mram_trim_cfg_exec(mram, trim_cfg_buff, 0, 532);

  rt_event_wait(event);
}



static void __rt_mram_do_ref_line(rt_mram_t *mram)
{
  unsigned int periph_base = mram->periph_base;
  rt_event_t *event = rt_event_get_blocking(NULL);

  rt_periph_copy_t *copy = &event->implem.copy;
  mram->first_pending_copy = copy;
  mram->last_pending_copy = copy;
  copy->next = NULL;
  copy->event = event;

  // Setting  AREF and TMEN to 1
  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1) |
    UDMA_MRAM_MRAM_MODE_RET_B(1) |
    UDMA_MRAM_MRAM_MODE_RST_B(1) |
    UDMA_MRAM_MRAM_MODE_TMEN(1)  |
    UDMA_MRAM_MRAM_MODE_AREF(1)  |
    UDMA_MRAM_MRAM_MODE_CMD(MRAM_CMD_REF_LINE_P)
  );

  udma_mram_trigger_set(periph_base, 0x1);

  rt_event_wait(event);



  event = rt_event_get_blocking(NULL);
  copy = &event->implem.copy;
  mram->first_pending_copy = copy;
  mram->last_pending_copy = copy;
  copy->next = NULL;
  copy->event = event;

  // Setting  AREF and TMEN to 1
  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1) |
    UDMA_MRAM_MRAM_MODE_RET_B(1) |
    UDMA_MRAM_MRAM_MODE_RST_B(1) |
    UDMA_MRAM_MRAM_MODE_TMEN(1)  |
    UDMA_MRAM_MRAM_MODE_AREF(1)  |
    UDMA_MRAM_MRAM_MODE_CMD(MRAM_CMD_REF_LINE_AP)
  );
  
  udma_mram_trigger_set(periph_base, 0x1);

  rt_event_wait(event);


  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1) |
    UDMA_MRAM_MRAM_MODE_RET_B(1) |
    UDMA_MRAM_MRAM_MODE_RST_B(1)
  );

  rt_time_wait_us(8);
}



static void __rt_mram_init(rt_mram_t *mram, void *trim_cfg_buff)
{
  unsigned int periph_base = mram->periph_base;

  // Setup clock divider
  // TODO should take periph clock into account
  udma_mram_clock_div_set(periph_base,
    UDMA_MRAM_CLOCK_DIV_EN(1)   |
    UDMA_MRAM_CLOCK_DIV_DATA(2)
  );

  // Activate all interrupts
  udma_mram_ier_set(periph_base,
    UDMA_MRAM_IER_ERASE_EN(1)    |
    UDMA_MRAM_IER_PROGRAM_EN(1)  |
    UDMA_MRAM_IER_TRIM_CFG_EN(1) |
    UDMA_MRAM_IER_REF_LINE_EN(1)
  );

  // Set PORb, RETb, RSTb, NVR, TMEN, AREF, DPD, ECCBYPS to 0
  udma_mram_mram_mode_set(periph_base, 0x0);

  // Perform Setup sequence : POR-RET-RST
  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1)
  );

  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1) |
    UDMA_MRAM_MRAM_MODE_RET_B(1)
  );

  rt_time_wait_us(5);

  udma_mram_mram_mode_set(periph_base,
    UDMA_MRAM_MRAM_MODE_POR_B(1) |
    UDMA_MRAM_MRAM_MODE_RET_B(1) |
    UDMA_MRAM_MRAM_MODE_RST_B(1)
  );

  __rt_mram_do_trim(mram, trim_cfg_buff);

  __rt_mram_do_ref_line(mram);
}



static rt_flash_t *__rt_mram_open(rt_dev_t *dev, rt_flash_conf_t *conf, rt_event_t *event)
{
  rt_mram_t *mram = NULL;
  void *trim_cfg_buff = NULL;

  mram = rt_alloc(RT_ALLOC_FC_DATA, sizeof(rt_mram_t));
  if (mram == NULL) goto error;

  trim_cfg_buff = rt_alloc(RT_ALLOC_PERIPH, TRIM_CFG_SIZE);
  if (trim_cfg_buff == NULL) goto error;

  mram->periph_id = ARCHI_UDMA_MRAM_ID(conf->id);
  mram->periph_base = hal_udma_periph_base(mram->periph_id);

  mram->first_pending_copy = NULL;

  __rt_udma_callback[mram->periph_id] = __rt_mram_handle_event;
  __rt_udma_callback_data[mram->periph_id] = mram;

  soc_eu_fcEventMask_setEvent(UDMA_EVENT_ID(mram->periph_id));
  soc_eu_fcEventMask_setEvent(UDMA_EVENT_ID(mram->periph_id)+2);
  soc_eu_fcEventMask_setEvent(UDMA_EVENT_ID(mram->periph_id)+3);
  plp_udma_cg_set(plp_udma_cg_get() | (1<<mram->periph_id));

  rt_pm_domain_state_switch(RT_PM_DOMAIN_MRAM, RT_PM_DOMAIN_STATE_ON, NULL);

  __rt_mram_init(mram, trim_cfg_buff);

  rt_free(RT_ALLOC_PERIPH, trim_cfg_buff, TRIM_CFG_SIZE);

  return (rt_flash_t *)mram;

error:
  if (trim_cfg_buff)
    rt_free(RT_ALLOC_PERIPH, trim_cfg_buff, TRIM_CFG_SIZE);

  __rt_mram_free(mram);
  return NULL;
}



static void __rt_mram_close(rt_flash_t *flash, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;
  __rt_mram_free(mram);
}



static void __rt_mram_read(rt_flash_t *flash, void *data, void *addr, size_t size, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->implem.copy;

  if (mram->first_pending_copy == NULL)
  {
    mram->first_pending_copy = copy;
    mram->last_pending_copy = copy;
    copy->next = NULL;
    copy->event = call_event;
    __rt_mram_read_exec((rt_mram_t *)flash, data, addr, size);
  }
  else
  {
    // TODO multiple asynchronous copies is not yet supprted
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



static void __rt_mram_program(rt_flash_t *flash, void *data, void *addr, size_t size, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->implem.copy;

  if (mram->first_pending_copy == NULL)
  {
    mram->first_pending_copy = copy;
    mram->last_pending_copy = copy;
    copy->next = NULL;
    copy->event = call_event;
    __rt_mram_program_exec((rt_mram_t *)flash, data, addr, size);
  }
  else
  {
    // TODO multiple asynchronous copies is not yet supprted
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



static void __rt_mram_erase_chip(rt_flash_t *flash, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->implem.copy;

  if (mram->first_pending_copy == NULL)
  {
    mram->first_pending_copy = copy;
    mram->last_pending_copy = copy;
    copy->next = NULL;
    copy->event = call_event;
    __rt_mram_erase_chip_exec((rt_mram_t *)flash);
  }
  else
  {
    // TODO multiple asynchronous copies is not yet supprted
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



static void __rt_mram_erase_sector(rt_flash_t *flash, void *addr, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->implem.copy;

  if (mram->first_pending_copy == NULL)
  {
    mram->first_pending_copy = copy;
    mram->last_pending_copy = copy;
    copy->next = NULL;
    copy->event = call_event;
    __rt_mram_erase_sector_exec((rt_mram_t *)flash, addr);
  }
  else
  {
    // TODO multiple asynchronous copies is not yet supprted
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



static void __rt_mram_erase(rt_flash_t *flash, void *addr, int size, rt_event_t *event)
{
  rt_mram_t *mram = (rt_mram_t *)flash;

  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->implem.copy;

  if (mram->first_pending_copy == NULL)
  {
    mram->first_pending_copy = copy;
    mram->last_pending_copy = copy;
    copy->next = NULL;
    copy->event = call_event;
    __rt_mram_erase_exec((rt_mram_t *)flash, addr, size);
  }
  else
  {
    // TODO multiple asynchronous copies is not yet supprted
  }

  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



rt_flash_dev_t mram_desc = {
  .open         = &__rt_mram_open,
  .close        = &__rt_mram_close,
  .read         = &__rt_mram_read,
  .program      = &__rt_mram_program,
  .erase_chip   = &__rt_mram_erase_chip,
  .erase_sector = &__rt_mram_erase_sector,
  .erase = &__rt_mram_erase
};
