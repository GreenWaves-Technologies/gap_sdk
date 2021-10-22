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
 * Authors: Yao ZHANG, GreenWaves Technologies (yao.zhang@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "pmsis/errno.h"


// I2C command IDS definition
#define NEW_I2C_CMD_START() ((0x0UL << 0x1cUL))
#define NEW_I2C_CMD_STOP() ((0x2UL << 0x1cUL))
#define NEW_I2C_CMD_RD_ACK() ((0x4UL << 0x1cUL))
#define NEW_I2C_CMD_RD_NACK() ((0x6UL << 0x1cUL))
#define NEW_I2C_CMD_WR() ((0x8UL << 0x1cUL))
#define NEW_I2C_CMD_WAIT(dummy_cycles) ((0xaUL << 0x1cUL)|(((uint32_t)dummy_cycles) << 0x0))
#define NEW_I2C_CMD_RPT(rpt_nb) ((0xcUL << 0x1cUL)|(((uint32_t)rpt_nb) << 0x0))
#define NEW_I2C_CMD_CFG(cfg) ((0xeUL << 0x1cUL)|(((uint32_t)cfg) << 0x0))
#define NEW_I2C_CMD_WAIT_EV(event) ((0x1UL << 0x1cUL) | ((event&0x3)<<24))
#define NEW_I2C_CMD_WRB(data_byte) ((0x7UL << 0x1cUL)|(((uint32_t)data_byte) << 0x0))
#define NEW_I2C_CMD_EOT() ((0x9UL << 0x1cUL))
#define NEW_I2C_CMD_SETUP_UCA(txrxn,saddr) ((0x3UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)saddr) << 0x0))
#define NEW_I2C_CMD_SETUP_UCS(txrxn,size) ((0x5UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)size) << 0x0))

#define NEW_I2C_CMD_SETUP_WRA(addr,rx) (NEW_I2C_CMD_WRB((addr&0xFE) | (rx)))

static L2_DATA pi_i2c_t __rt_i2c[ARCHI_UDMA_NB_I2C];


void pos_i2c_handle_copy_asm();


void pos_i2c_handle_copy(int event, void *arg)
{
  pi_i2c_t *i2c = (pi_i2c_t *)arg;
  pi_task_t *task = i2c->pending_copy;
  i2c->pending_copy = NULL;

  __rt_event_handle_end_of_task(task);
}


void __rt_i2c_handle_tx_copy(int event, void *arg)
{
  pi_i2c_t *i2c = (pi_i2c_t *)arg;

  void (*cb)(pi_i2c_t *) = (void (*)(pi_i2c_t *))i2c->pending_step;

  cb(i2c);
}

void __rt_i2c_handle_rx_copy(int event, void *arg)
{
}

void __rt_i2c_step1(pi_i2c_t *i2c)
{
  plp_udma_enqueue(i2c->pending_base, i2c->pending_data, i2c->pending_length, UDMA_CHANNEL_CFG_EN);
  i2c->pending_step = i2c->pending_next_step;
}

void __rt_i2c_step3(pi_i2c_t *i2c)
{
  pi_task_t *task = i2c->pending_copy;
  i2c->pending_copy = NULL;
  __rt_event_handle_end_of_task(task);
}

void __rt_i2c_step2(pi_i2c_t *i2c)
{
  i2c->pending_step = (uint32_t)__rt_i2c_step3;
  plp_udma_enqueue(i2c->pending_base, (uint32_t)&i2c->udma_stop_cmd, 1, UDMA_CHANNEL_CFG_EN);
}

//void udma_event_handler_end(pi_i2c_t *i2c)
//{
//  pi_task_t *task = i2c->pending_copy;
//  i2c->pending_copy = NULL;
//  __rt_event_handle_end_of_task(task);
//}



static int __rt_i2c_get_div(int i2c_freq)
{
    // Round-up the divider to obtain an i2c frequency which is below the maximum
    // Also the HW divider is adding a factor of 4 on top of on what we give
  int div = (__rt_freq_periph_get() + i2c_freq - 1)/(i2c_freq*4);

  // The counter will count from 0 to div included
  if (div <= 1)
    return 0;
  else
    return div - 1;
}



void pi_i2c_ioctl(struct pi_device *device, uint32_t cmd, void *_arg)
{
  pi_i2c_t *handle = (pi_i2c_t *)device->data;
  uint32_t arg = (uint32_t)_arg;
  int irq = rt_irq_disable();

  int set_freq = (cmd >> __PI_I2C_CTRL_SET_MAX_BAUDRATE_BIT) & 1;

  if (set_freq)
  {
    handle->max_baudrate = arg;
    handle->div = __rt_i2c_get_div(handle->max_baudrate);
  }

  rt_irq_restore(irq);
}


void pi_i2c_write_dual_async(struct pi_device *device, void *buffer0, void *buffer1, uint32_t size0, uint32_t size1, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_i2c_t *i2c = (pi_i2c_t *)device->data;

  if (i2c->pending_copy)
  {
    task->implem.data[0] = (unsigned int)buffer0;
    task->implem.data[2] = (unsigned int)buffer1;
    task->implem.data[3] = (unsigned int)size0;
    task->implem.data[4] = (unsigned int)size1;

    if (i2c->waiting_first)
      i2c->waiting_last->implem.next = task;
    else
      i2c->waiting_first = task;

    i2c->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  i2c->pending_copy = task;

  int seq_index = 0;

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_START();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_WRA(i2c->cs, 0);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(size0);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WR();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)buffer0) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)size0) & 0x1FFFFF);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(size1);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WR();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)buffer1) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)size1) & 0x1FFFFF);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_STOP();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_EOT();

  unsigned int base = hal_udma_periph_base(i2c->channel) + UDMA_CHANNEL_OFFSET(2);

  plp_udma_enqueue(base, (unsigned int)i2c->udma_cmd, seq_index*4, UDMA_CHANNEL_CFG_EN);

end:
  rt_irq_restore(irq);
}

void pi_i2c_write_read_async(struct pi_device *device, void *tx_buffer, void *rx_buffer, uint32_t tx_size, uint32_t rx_size, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);


  pi_i2c_t *i2c = (pi_i2c_t *)device->data;

  if (i2c->pending_copy)
  {
    task->implem.data[0] = (unsigned int)tx_buffer;
    task->implem.data[2] = (unsigned int)rx_buffer;
    task->implem.data[3] = (unsigned int)tx_size;
    task->implem.data[4] = (unsigned int)rx_size;

    if (i2c->waiting_first)
      i2c->waiting_last->implem.next = task;
    else
      i2c->waiting_first = task;

    i2c->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  i2c->pending_copy = task;

  int seq_index = 0;

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_START();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_WRA(i2c->cs, 0);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(tx_size);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WR();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)tx_buffer) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)tx_size) & 0x1FFFFF);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_START();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_WRA(i2c->cs, 1);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(rx_size-1);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(0, ((uint32_t)rx_buffer) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(0, ((uint32_t)rx_size) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RD_ACK();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RD_NACK();

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_STOP();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_EOT();

  unsigned int base = hal_udma_periph_base(i2c->channel) + UDMA_CHANNEL_OFFSET(2);

  plp_udma_enqueue(base, (unsigned int)i2c->udma_cmd, seq_index*4, UDMA_CHANNEL_CFG_EN);

end:
  rt_irq_restore(irq);
}

void pi_i2c_write_async(struct pi_device *device, uint8_t *data, int length, pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_i2c_t *i2c = (pi_i2c_t *)device->data;

  if (i2c->pending_copy)
  {
    task->implem.data[0] = (unsigned int)data;
    task->implem.data[1] = (unsigned int)length;
    task->implem.data[2] = (unsigned int)flags;

    if (i2c->waiting_first)
      i2c->waiting_last->implem.next = task;
    else
      i2c->waiting_first = task;

    i2c->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  i2c->pending_copy = task;

  int seq_index = 0;

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_START();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_WRA(i2c->cs, 0);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(length);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WR();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(1, ((uint32_t)data) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(1, ((uint32_t)length) & 0x1FFFFF);

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_STOP();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_EOT();

  unsigned int base = hal_udma_periph_base(i2c->channel) + UDMA_CHANNEL_OFFSET(2);

  plp_udma_enqueue(base, (unsigned int)i2c->udma_cmd, seq_index*4, UDMA_CHANNEL_CFG_EN);

end:
  rt_irq_restore(irq);
}

int pi_i2c_write(struct pi_device *device, uint8_t *data, int length, pi_i2c_xfer_flags_e flags)
{
  struct pi_task task;
  pi_i2c_write_async(device, data, length, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
  return PI_OK;
}

void pi_i2c_read_async(struct pi_device *device, uint8_t *rx_buff, int length, pi_i2c_xfer_flags_e flags, pi_task_t *task)
{
  int irq = rt_irq_disable();

  __rt_task_init(task);

  pi_i2c_t *i2c = (pi_i2c_t *)device->data;

  if (i2c->pending_copy)
  {
    task->implem.data[0] = (unsigned int)rx_buff;
    task->implem.data[1] = (unsigned int)length;
    task->implem.data[2] = (unsigned int)flags;

    if (i2c->waiting_first)
      i2c->waiting_last->implem.next = task;
    else
      i2c->waiting_first = task;

    i2c->waiting_last = task;
    task->implem.next = NULL;

    goto end;
  }

  i2c->pending_copy = task;

  int seq_index = 0;

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_START();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_WRA(i2c->cs, 1);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RPT(length-1);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCA(0, ((uint32_t)rx_buff) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_SETUP_UCS(0, ((uint32_t)length) & 0x1FFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RD_ACK();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_RD_NACK();

  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_STOP();
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WAIT(0x5UL);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_EOT();

  unsigned int base = hal_udma_periph_base(i2c->channel) + UDMA_CHANNEL_OFFSET(2);

  plp_udma_enqueue(base, (unsigned int)i2c->udma_cmd, seq_index*4, UDMA_CHANNEL_CFG_EN);

end:
  rt_irq_restore(irq);
}

int pi_i2c_read(struct pi_device *device, uint8_t *data, int length, pi_i2c_xfer_flags_e flags)
{
  struct pi_task task;
  pi_i2c_read_async(device, data, length, flags, pi_task_block(&task));
  pi_task_wait_on(&task);
  return PI_OK;
}

int pi_i2c_open(struct pi_device *device)
{
  int irq = rt_irq_disable();

  struct pi_i2c_conf *conf = (struct pi_i2c_conf *)device->config;

  int periph_id = ARCHI_UDMA_I2C_ID(conf->itf);
  int channel_id = UDMA_EVENT_ID(periph_id);
  int first_event = UDMA_EVENT_ID(periph_id);

  pi_i2c_t *i2c = &__rt_i2c[conf->itf];

  device->data = (void *)i2c;
  i2c->open_count++;

  if (i2c->open_count == 1)
  {
    plp_udma_cg_set(plp_udma_cg_get() | (1<<periph_id));
    soc_eu_fcEventMask_setEvent(first_event + 3);

    __rt_udma_register_channel_callback(channel_id, pos_i2c_handle_copy_asm, (void *)i2c);

    i2c->pending_copy = NULL;
    i2c->waiting_first = NULL;
  }


  i2c->channel = periph_id;
  i2c->cs = conf->cs;
  i2c->max_baudrate = conf->max_baudrate;
  i2c->div = __rt_i2c_get_div(i2c->max_baudrate);
  int seq_index = 0;
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_CFG(i2c->div & 0xFFFF);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_WAIT(0xFFUL);
  i2c->udma_cmd[seq_index++] = NEW_I2C_CMD_EOT();

  
  
  rt_irq_restore(irq);

  unsigned int base = hal_udma_channel_base(i2c->channel) + UDMA_CHANNEL_OFFSET(2);;
  plp_udma_enqueue(base, (unsigned int)i2c->udma_cmd, seq_index, UDMA_CHANNEL_CFG_EN);

  pi_time_wait_us(50);

  soc_eu_fcEventMask_setEvent(channel_id + 3);

  return 0;
}



void pi_i2c_close (struct pi_device *device)
{
  int irq = rt_irq_disable();
  pi_i2c_t *i2c = (pi_i2c_t *)device->data;

  int channel = i2c->channel;
  int periph_id = UDMA_PERIPH_ID(channel);

  i2c->open_count--;

  if (i2c->open_count == 0)
  {
    // Deactivate event routing
    soc_eu_fcEventMask_clearEvent(channel);

    // Reactivate clock-gating
    plp_udma_cg_set(plp_udma_cg_get() & ~(1<<periph_id));
  }
  rt_irq_restore(irq);
}



void pi_i2c_conf_init(pi_i2c_conf_t *conf)
{
  conf->cs = 0;
  conf->max_baudrate = 200000;
  conf->itf = 0;
}

static void __attribute__((constructor)) __rt_i2c_init()
{
  for (int i=0; i<ARCHI_UDMA_NB_I2C; i++)
  {
    __rt_i2c[i].open_count = 0;
    __rt_i2c[i].udma_stop_cmd = I2C_CMD_STOP;
    __rt_udma_channel_reg_data(UDMA_EVENT_ID(ARCHI_UDMA_I2C_ID(0) + i), &__rt_i2c[i]);
    __rt_udma_channel_reg_data(UDMA_EVENT_ID(ARCHI_UDMA_I2C_ID(0) + i)+1, &__rt_i2c[i]);
  }
}


#ifdef __ZEPHYR__

#include <zephyr.h>
#include <device.h>
#include <init.h>

static int i2c_init(struct device *device)
{
  ARG_UNUSED(device);

  __rt_i2c_init();

  return 0;
}

struct i2c_config {
};

struct i2c_data {
};

static const struct i2c_config i2c_cfg = {
};

static struct i2c_data i2c_data = {
};

DEVICE_INIT(i2c, "i2c", &i2c_init,
    &i2c_data, &i2c_cfg,
    PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEVICE);

#endif
