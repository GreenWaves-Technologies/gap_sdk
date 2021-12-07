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
 * Copyright (C) 2018 GreenWaves Technologies
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

#include "rt/rt_api.h"



typedef struct {
    unsigned char cmd[RT_PERIPH_COPY_PERIPH_DATA_SIZE];
} rt_i2c_cmd_t;



static RT_FC_DATA rt_i2c_t __rt_i2c[ARCHI_UDMA_NB_I2C];



static inline int __rt_i2c_id(int periph_id)
{
    return periph_id - ARCHI_UDMA_I2C_ID(0);
}



static int __rt_i2c_get_div(int i2c_freq)
{
    // Round-up the divider to obtain an i2c frequency which is below the maximum
    int div = (__rt_freq_periph_get() + i2c_freq - 1)/i2c_freq;
    // The I2C always divide by 4 once we activate the divider, adapt the divider
    // and round it up to make sure we don't go above the maximum frequency.
    div = (div + 3) / 4;

    return div;
}



static void __rt_i2c_write_prologue(rt_periph_copy_t *copy)
{
    rt_i2c_cmd_t *cmd = (rt_i2c_cmd_t *)copy->periph_data;
    unsigned char *udma_cmd = (unsigned char *)cmd->cmd;
    int seq_index = 0;
    copy->end_callback = 0;
    udma_cmd[seq_index++] = I2C_CMD_STOP;
    udma_cmd[seq_index++] = I2C_CMD_WAIT;
    udma_cmd[seq_index++] = 0x1;
    plp_udma_enqueue(copy->u.i2c.base, (unsigned int)udma_cmd, seq_index, UDMA_CHANNEL_CFG_EN);
}



static void __rt_i2c_write_data(rt_periph_copy_t *copy)
{
  if (copy->u.i2c.xfer_pending)
  {
    copy->end_callback = 0;
  }
  else
  {
    copy->end_callback = (uint32_t)__rt_i2c_write_prologue;
  }
  plp_udma_enqueue(copy->u.i2c.base, (unsigned int)copy->u.i2c.data, copy->u.i2c.size, UDMA_CHANNEL_CFG_EN);
}



void rt_i2c_write(rt_i2c_t *dev_i2c, unsigned char *data, int length, int xfer_pending, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;
  rt_i2c_cmd_t *cmd = (rt_i2c_cmd_t *)copy->periph_data;

  int seq_index = 0;

  rt_periph_copy_init(copy, 0);

  unsigned char *udma_cmd = (unsigned char *)cmd->cmd;
  udma_cmd[seq_index++] = I2C_CMD_START;
  udma_cmd[seq_index++] = I2C_CMD_WR;
  udma_cmd[seq_index++] = dev_i2c->cs;

  if (length > 1){
    udma_cmd[seq_index++] = I2C_CMD_RPT;
    udma_cmd[seq_index++] = length;
  }
  udma_cmd[seq_index++] = I2C_CMD_WR;

  copy->end_callback = (uint32_t )__rt_i2c_write_data;
  copy->u.i2c.data = data;
  copy->u.i2c.size = length;
  copy->u.i2c.base = hal_udma_channel_base(dev_i2c->channel);
  copy->u.i2c.xfer_pending = xfer_pending;


  rt_periph_copy_i2c(copy, dev_i2c->channel, (unsigned int)udma_cmd, seq_index, UDMA_CHANNEL_CFG_EN, dev_i2c->div, call_event);
  
  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



void rt_i2c_read(rt_i2c_t *dev_i2c, unsigned char *rx_buff, int length, int xfer_pending, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_event_t *call_event = __rt_wait_event_prepare(event);
  rt_periph_copy_t *copy = &call_event->copy;
  rt_i2c_cmd_t *cmd = (rt_i2c_cmd_t *)copy->periph_data;

  int seq_index = 0;

  rt_periph_copy_init(copy, 0);

  unsigned char *udma_cmd = (unsigned char *)cmd->cmd;
  udma_cmd[seq_index++] = I2C_CMD_START;
  udma_cmd[seq_index++] = I2C_CMD_WR;
  udma_cmd[seq_index++] = dev_i2c->cs | 0x1;

  if (length > 1){
    udma_cmd[seq_index++] = I2C_CMD_RPT;
    udma_cmd[seq_index++] = length - 1;
    udma_cmd[seq_index++] = I2C_CMD_RD_ACK;
  }
  udma_cmd[seq_index++] = I2C_CMD_RD_NACK;

  if (xfer_pending)
    udma_cmd[seq_index++] = I2C_CMD_STOP;

  rt_periph_copy_i2c(copy, dev_i2c->channel, (unsigned int)udma_cmd, seq_index, UDMA_CHANNEL_CFG_EN, dev_i2c->div, NULL);
  rt_periph_copy_i2c(copy, dev_i2c->channel + 1, (unsigned int)rx_buff, length, UDMA_CHANNEL_CFG_EN, dev_i2c->div, call_event);
  
  __rt_wait_event_check(event, call_event);

  rt_irq_restore(irq);
}



void __rt_i2c_control(rt_i2c_t *handle, rt_i2c_control_e cmd, uint32_t arg)
{
  int irq = rt_irq_disable();

  int set_freq = (cmd >> __RT_I2C_CTRL_SET_MAX_BAUDRATE_BIT) & 1;

  if (set_freq)
  {
    handle->max_baudrate = arg;
    handle->div = __rt_i2c_get_div(handle->max_baudrate);
  }

  rt_irq_restore(irq);
}



rt_i2c_t *rt_i2c_open(char *dev_name, rt_i2c_conf_t *i2c_conf, rt_event_t *event)
{
  int irq = rt_irq_disable();

  rt_i2c_conf_t def_conf;

  if (i2c_conf == NULL)
  {
      i2c_conf = &def_conf;
      rt_i2c_conf_init(i2c_conf);
  }

  int channel = -1;

  if (i2c_conf->id != -1)
  {
      rt_trace(RT_TRACE_DEV_CTRL, "[I2C] Opening i2c device (id: %d)\n", i2c_conf->id);
      channel = ARCHI_UDMA_I2C_ID(i2c_conf->id);
  }
  else if (dev_name != NULL)
  {
      rt_trace(RT_TRACE_DEV_CTRL, "[I2C] Opening i2c device (name: %s)\n", dev_name);

      rt_dev_t *dev = rt_dev_get(dev_name);
      if (dev == NULL) goto error;

      channel = dev->channel;
  }

  if (channel == -1) goto error;

  rt_i2c_t *i2c = &__rt_i2c[__rt_i2c_id(channel)];
  if (i2c->open_count > 0) goto error;

  i2c->open_count++;
  i2c->channel = channel*2;
  i2c->cs = i2c_conf->cs;
  i2c->max_baudrate = i2c_conf->max_baudrate;
  i2c->div = __rt_i2c_get_div(i2c->max_baudrate);

  rt_irq_restore(irq);

  return i2c;

  error:
  rt_warning("[I2C] Failed to open I2C device\n");
  return NULL;
}



void rt_i2c_close (rt_i2c_t *i2c, rt_event_t *event)
{
  i2c->open_count--;
  if (i2c->open_count == 0)
  {
    hal_udma_i2c_setup_set(hal_udma_periph_base(i2c->channel/2), hal_udma_i2c_setup_compute(0, 0));
  }
}



void rt_i2c_conf_init(rt_i2c_conf_t *conf)
{
    conf->cs = -1;
    conf->id = -1;
    conf->max_baudrate = 200000;
}



RT_FC_BOOT_CODE void __attribute__((constructor)) __rt_i2c_init()
{
    for (int i=0; i<ARCHI_UDMA_NB_I2C; i++)
    {
        __rt_i2c[i].open_count = 0;
    }
}

