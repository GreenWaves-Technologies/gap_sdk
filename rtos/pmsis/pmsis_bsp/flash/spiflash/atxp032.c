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
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

// Driver for the octo spi flash Adesto ATXP032

#include "pmsis.h"
#include "bsp/bsp.h"
#include "pmsis/drivers/octospi.h"

#define ATXP032_READ_STATUS_CMD 0x65
#define ATXP032_READ_STATUS_LATENCY_SPI 8
#define ATXP032_READ_STATUS_LATENCY_OCTO 3
#define ATXP032_READ_STATUS_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_1 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define ATXP032_READ_STATUS_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_1 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define ATXP032_WRITE_STATUS_CMD 0x71
#define ATXP032_WRITE_STATUS_LATENCY_SPI 0
#define ATXP032_WRITE_STATUS_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_1 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)

#define ATXP032_WRITE_ENABLE_CMD 0x6
#define ATXP032_WRITE_ENABLE_LATENCY_SPI 0
#define ATXP032_WRITE_ENABLE_LATENCY_OCTO 0
#define ATXP032_WRITE_ENABLE_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define ATXP032_WRITE_ENABLE_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_DATA_DTR)

#define ATXP032_ERASE_CMD 0x20
#define ATXP032_ERASE_LATENCY 0
#define ATXP032_ERASE_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define ATXP032_PROGRAM_CMD 0x02
#define ATXP032_PROGRAM_LATENCY 0
#define ATXP032_PROGRAM_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define ATXP032_READ_CMD 0x0B
#define ATXP032_READ_LATENCY 22
#define ATXP032_READ_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define SECTOR_SIZE (1<<12)

#define STALL_TASK_PROGRAM      0
#define STALL_TASK_ERASE_CHIP   1
#define STALL_TASK_ERASE_SECTOR 2
#define STALL_TASK_REG_SET      3
#define STALL_TASK_REG_GET      4
#define STALL_TASK_READ         5
#define STALL_TASK_READ_2D      6

typedef struct {
  struct pi_device octospi_device;
  // Used for communications with atxp032 through udma
  uint16_t udma_buffer[2];

  // Waiting queue for common operations (only 1 is handled at the same time)
  pi_task_t *waiting_first;
  pi_task_t *waiting_last;

  // Task to be enqueued when the on-going operation is done
  pi_task_t *pending_task;

  // Waiting queue for erase operation (only 1 is handled at the same time) as it needs a
  // second level FSM
  pi_task_t *erase_waiting_first;
  pi_task_t *erase_waiting_last;

  // Task to be enqueued when the on-going erase operation is done
  pi_task_t *erase_task;

  // Task used for internal FSM scheduling for common operations
  pi_task_t task;

  // Task used for internal second-level FSM scheduling (for erase operation)
  pi_task_t task2;

  // Description of on-going task for common operations. The FSM will keep executing
  // until this operation is done
  uint32_t pending_octospi_addr;
  uint32_t pending_data;
  uint32_t pending_size;

  // Description of on-going task for erase operation. The FSM will keep executing
  // until this operation is done
  uint32_t pending_erase_octospi_addr;
  uint32_t pending_erase_size;

} atxp032_t;


static pi_octospi_op_conf_t atxp032_erase_op = { .cmd=ATXP032_ERASE_CMD, .latency=ATXP032_ERASE_LATENCY, .flags=ATXP032_ERASE_FLAGS };

static pi_octospi_op_conf_t atxp032_program_op = { .cmd=ATXP032_PROGRAM_CMD, .latency=ATXP032_PROGRAM_LATENCY, .flags=ATXP032_PROGRAM_FLAGS };

static pi_octospi_op_conf_t atxp032_read_op = { .cmd=ATXP032_READ_CMD, .latency=ATXP032_READ_LATENCY, .flags=ATXP032_READ_FLAGS };


static void atxp032_program_async(struct pi_device *device, uint32_t octospi_addr, const void *data, uint32_t size, pi_task_t *task);

static void atxp032_check_program(void *arg);

static void atxp032_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task);

static int atxp032_stall_task(atxp032_t *atxp032, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

static void atxp032_handle_pending_task(void *arg);

static void atxp032_erase_chip_async(struct pi_device *device, pi_task_t *task);

static void atxp032_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task);

static void atxp032_set_reg_exec(atxp032_t *atxp032, unsigned int addr, unsigned short value)
{
  atxp032->udma_buffer[0] = value;
  //pi_octospi_write(&atxp032->octospi_device, addr, atxp032->udma_buffer, 2);
}



// TODO should be moved to pmsis api
static void pi_task_enqueue(pi_task_t *task)
{
  pi_task_push(task);
}



static unsigned short atxp032_get_reg_exec(atxp032_t *atxp032, unsigned int addr)
{
  //pi_octospi_read(&atxp032->octospi_device, addr, atxp032->udma_buffer, 4);
  return atxp032->udma_buffer[0];
}


static uint32_t atxp032_get_status(atxp032_t *atxp032)
{
  uint32_t data;
  pi_octospi_op_conf_t op = { .cmd=ATXP032_READ_STATUS_CMD, .latency=ATXP032_READ_STATUS_LATENCY_OCTO, .flags=ATXP032_READ_STATUS_FLAGS_OCTO };
  pi_octospi_read(&atxp032->octospi_device, 0, &data, 4, &op);
  return data;
}


static int atxp032_is_busy(atxp032_t *atxp032)
{
    uint32_t value = atxp032_get_status(atxp032);
    return (value >> 8) & 1;
}


static void atxp032_write_enable(atxp032_t *atxp032)
{
  pi_octospi_op_conf_t op = { .cmd=ATXP032_WRITE_ENABLE_CMD, .latency=ATXP032_WRITE_ENABLE_LATENCY_OCTO, .flags=ATXP032_WRITE_ENABLE_FLAGS_OCTO };
  int dummy = 0;
  pi_octospi_write(&atxp032->octospi_device, 0, &dummy, 1, &op);
}



static int atxp032_open(struct pi_device *device)
{
  struct pi_atxp032_conf *conf = (struct pi_atxp032_conf *)device->config;

  atxp032_t *atxp032 = (atxp032_t *)pmsis_l2_malloc(sizeof(atxp032_t));
  if (atxp032 == NULL)
  {
    return -1;
  }

  device->data = (void *)atxp032;

  if (bsp_atxp032_open(conf))
  {
    goto error;
  }

  struct pi_octospi_conf octospi_conf = {0};
  pi_octospi_conf_init(&octospi_conf);

  octospi_conf.id = (unsigned char) conf->spi_itf;
  octospi_conf.cs = conf->spi_cs;
  octospi_conf.type = PI_OCTOSPI_TYPE_FLASH;
  octospi_conf.xip_en = conf->xip_en;

  pi_open_from_conf(&atxp032->octospi_device, &octospi_conf);

  int32_t error = pi_octospi_open(&atxp032->octospi_device);
  if (error)
  {
      goto error;
  }

  atxp032->pending_task = NULL;
  atxp032->waiting_first = NULL;

  atxp032->erase_task = NULL;
  atxp032->erase_waiting_first = NULL;

  uint32_t data;

  pi_octospi_op_conf_t op1 = { .cmd=ATXP032_READ_STATUS_CMD, .latency=ATXP032_READ_STATUS_LATENCY_SPI, .flags=ATXP032_READ_STATUS_FLAGS_SPI };
  pi_octospi_read(&atxp032->octospi_device, 0, &data, 4, &op1);

  pi_octospi_op_conf_t op2 = { .cmd=ATXP032_WRITE_ENABLE_CMD, .latency=ATXP032_WRITE_ENABLE_LATENCY_SPI, .flags=ATXP032_WRITE_ENABLE_FLAGS_SPI };
  int dummy = 0;
  pi_octospi_write(&atxp032->octospi_device, 0, &dummy, 1, &op2);

  // Activate octospi mode and DTR
  data |= (1 << 19) | (1 << 23);
  // Unprotect all sectors
  data &= ~(0x00000C00);

  pi_octospi_op_conf_t op3 = { .cmd=ATXP032_WRITE_STATUS_CMD, .latency=ATXP032_WRITE_STATUS_LATENCY_SPI, .flags=ATXP032_WRITE_STATUS_FLAGS_SPI };
  pi_octospi_write(&atxp032->octospi_device, 0, &data, 4, &op3);

  pi_octospi_op_conf_t op4 = { .cmd=ATXP032_READ_STATUS_CMD, .latency=ATXP032_READ_STATUS_LATENCY_OCTO, .flags=ATXP032_READ_STATUS_FLAGS_OCTO };
  pi_octospi_read(&atxp032->octospi_device, 0, &data, 4, &op4);

  return 0;

error:
  pmsis_l2_malloc_free(atxp032, sizeof(atxp032_t));
  return -2;
}



static void atxp032_close(struct pi_device *device)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;
  pi_octospi_close(&atxp032->octospi_device);
  pmsis_l2_malloc_free(atxp032, sizeof(atxp032_t));
}



static int32_t atxp032_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
  switch (cmd)
  {
    case PI_FLASH_IOCTL_INFO:
    {
      struct pi_flash_info *flash_info = (struct pi_flash_info *)arg;
      flash_info->sector_size = 1<<18;
      // TODO find a way to know what is on the flash, as they may be a boot binary
      flash_info->flash_start = flash_info->sector_size;
    }
  }
  return 0;
}



static void atxp032_reg_set_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_REG_SET, addr, (uint32_t)value, 0, 0, 0))
    return;

  atxp032_set_reg_exec(atxp032, addr, *(uint16_t *)value);

  atxp032_handle_pending_task(device);
}



static void atxp032_reg_get_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_REG_GET, addr, (uint32_t)value, 0, 0, 0))
    return;

  *(uint16_t *)value = atxp032_get_reg_exec(atxp032, addr);

  atxp032_handle_pending_task(device);
}



static void atxp032_read_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_READ, addr, (uint32_t)data, size, 0, 0))
    return;

  pi_octospi_read_async(&atxp032->octospi_device, addr, data, size, &atxp032_read_op, pi_task_callback(&atxp032->task, atxp032_handle_pending_task, device));
}



static void atxp032_read_2d_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, uint32_t stride, uint32_t length, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_READ_2D, addr, (uint32_t)data, size, stride, length))
  {
    return;
  }

  pi_octospi_read_2d_async(&atxp032->octospi_device, addr, data, size, stride, length, &atxp032_read_op, pi_task_callback(&atxp032->task, atxp032_handle_pending_task, device));
}



static void atxp032_handle_pending_task(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  uint32_t irq = disable_irq();

  pi_task_enqueue(atxp032->pending_task);
  atxp032->pending_task = NULL;

  pi_task_t *task = atxp032->waiting_first;

  if (task)
  {
    atxp032->waiting_first = task->next;
  }

  restore_irq(irq);

  if (task)
  {
    if (task->data[0] == STALL_TASK_PROGRAM)
    {
      atxp032_program_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
    }
    else if (task->data[0] == STALL_TASK_ERASE_CHIP)
    {
      atxp032_erase_chip_async(device, task);
    }
    else if (task->data[0] == STALL_TASK_ERASE_SECTOR)
    {
      atxp032_erase_sector_async(device, task->data[1], task);
    }
    else if (task->data[0] == STALL_TASK_REG_SET)
    {
      atxp032_reg_set_async(device, task->data[1], (uint8_t *)task->data[2], task);
    }
    else if (task->data[0] == STALL_TASK_REG_GET)
    {
      atxp032_reg_get_async(device, task->data[1], (uint8_t *)task->data[2], task);
    }
    else if (task->data[0] == STALL_TASK_READ)
    {
      atxp032_read_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
    }
    else if (task->data[0] == STALL_TASK_READ_2D)
    {
      atxp032_read_2d_async(device, task->data[1], (void *)task->data[2], task->data[3], task->data[4], task->data[5], task);
    }
  }
}



static void atxp032_handle_pending_erase_task(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  uint32_t irq = disable_irq();

  pi_task_enqueue(atxp032->erase_task);
  atxp032->erase_task = NULL;

  pi_task_t *task = atxp032->erase_waiting_first;
  if (task)
  {
    atxp032->erase_waiting_first = task->next;
  }

  restore_irq(irq);

  if (task)
  {
    atxp032_erase_async(device, task->data[1], task->data[2], task);
  }
}



static int atxp032_stall_task(atxp032_t *atxp032, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
  uint32_t irq = disable_irq();

  if (atxp032->pending_task != NULL)
  {
    task->data[0] = id;
    task->data[1] = arg0;
    task->data[2] = arg1;
    task->data[3] = arg2;
    task->data[4] = arg3;
    task->data[5] = arg4;
    task->next = NULL;

    if (atxp032->waiting_first)
    {
      atxp032->waiting_last->next = task;
    }
    else
    {
      atxp032->waiting_first = task;
    }

    atxp032->waiting_last = task;

    restore_irq(irq);
    return 1;
  }

  atxp032->pending_task = task;

  restore_irq(irq);
  return 0;
}



static int atxp032_stall_erase_task(atxp032_t *atxp032, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
  uint32_t irq = disable_irq();

  if (atxp032->erase_task != NULL)
  {
    task->data[0] = id;
    task->data[1] = arg0;
    task->data[2] = arg1;
    task->data[3] = arg2;
    task->next = NULL;

    if (atxp032->erase_waiting_first)
    {
      atxp032->erase_waiting_last->next = task;
    }
    else
    {
      atxp032->erase_waiting_first = task;
    }

    atxp032->erase_waiting_last = task;

    restore_irq(irq);
    return 1;
  }

  atxp032->erase_task = task;

  restore_irq(irq);
  return 0;
}



static void atxp032_program_resume(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032->pending_size == 0)
  {
    atxp032_handle_pending_task(device);
  }
  else
  {
    unsigned int iter_size = 256 - (atxp032->pending_octospi_addr & 0xff);
      if (iter_size > atxp032->pending_size)
        iter_size = atxp032->pending_size;

    uint32_t octospi_addr = atxp032->pending_octospi_addr;
    uint32_t data = atxp032->pending_data;

    atxp032->pending_octospi_addr += iter_size;
    atxp032->pending_data += iter_size;
    atxp032->pending_size -= iter_size;

    atxp032_write_enable(atxp032);
    pi_octospi_write_async(&atxp032->octospi_device, octospi_addr, (void *)data, iter_size, &atxp032_program_op, pi_task_callback(&atxp032->task, atxp032_check_program, device));
  }
}



static void atxp032_check_program(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_is_busy(atxp032))
  {
    // Typical buffer programming time is 4ms. Note that this could be optimzed by taking into account buffer size
    pi_task_push_delayed_us(pi_task_callback(&atxp032->task, atxp032_check_program, device), 1000);
  }
  else
  {
    atxp032_program_resume(device);
  }
}



static void atxp032_program_async(struct pi_device *device, uint32_t octospi_addr, const void *data, uint32_t size, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_PROGRAM, octospi_addr, (uint32_t)data, size, 0, 0))
    return;

  atxp032->pending_octospi_addr = octospi_addr;
  atxp032->pending_data = (uint32_t)data;
  atxp032->pending_size = size;

  atxp032_program_resume(device);
}





static void atxp032_check_erase(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  uint32_t reg_status; // = atxp032_get_status_reg(atxp032);
  if (atxp032_is_busy(atxp032))
  {
    // Typical sector erase time is 250ms but keep it short as this time is shorter or some platform
    pi_task_push_delayed_us(pi_task_callback(&atxp032->task, atxp032_check_erase, device), 10000);
  }
  else
  {
    atxp032_handle_pending_task(device);
  }
}


static void atxp032_erase_chip_async(struct pi_device *device, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_ERASE_CHIP, 0, 0, 0, 0, 0))
    return;



  pi_task_push_delayed_us(pi_task_callback(&atxp032->task, atxp032_check_erase, device), 100000);
}


static void atxp032_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_task(atxp032, task, STALL_TASK_ERASE_SECTOR, addr, 0, 0, 0, 0))
    return;

  atxp032_write_enable(atxp032);

  // We don't need to send data but UDMA needs at least 1 byte, this will be ignored by the flash
  pi_octospi_write_async(&atxp032->octospi_device, addr, atxp032->udma_buffer, 1, &atxp032_erase_op, pi_task_callback(&atxp032->task, atxp032_check_erase, device));
}



static void atxp032_erase_resume(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032->pending_erase_size == 0)
  {
    atxp032_handle_pending_erase_task(device);
  }
  else
  {
    unsigned int iter_size = SECTOR_SIZE - (atxp032->pending_erase_octospi_addr & (SECTOR_SIZE - 1));
    if (iter_size > atxp032->pending_erase_size)
      iter_size = atxp032->pending_erase_size;

    uint32_t octospi_addr = atxp032->pending_erase_octospi_addr;
    atxp032_erase_sector_async(device, octospi_addr, pi_task_callback(&atxp032->task2, atxp032_erase_resume, device));

    atxp032->pending_erase_octospi_addr += iter_size;
    atxp032->pending_erase_size -= iter_size;
  }
}



static void atxp032_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task)
{
  atxp032_t *atxp032 = (atxp032_t *)device->data;

  if (atxp032_stall_erase_task(atxp032, task, 3, addr, size, 0))
  {
    return;
  }

  atxp032->pending_erase_octospi_addr = addr;
  atxp032->pending_erase_size = size;

  atxp032_erase_resume(device);
}



static int atxp032_copy_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
  if (!ext2loc)
    atxp032_program_async(device, flash_addr, buffer, size, task);
  else
    atxp032_read_async(device, flash_addr, buffer, size, task);

  return 0;
}



static int atxp032_copy_2d_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
  if (!ext2loc)
    return -1;

  atxp032_read_2d_async(device, flash_addr, buffer, size, stride, length, task);

  return 0;
}

static int atxp032_read(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size)
{
  pi_task_t task;
  atxp032_read_async(device, pi_flash_addr, data, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}


static int atxp032_program(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size)
{
  pi_task_t task;
  atxp032_program_async(device, pi_flash_addr, data, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_erase_chip(struct pi_device *device)
{
  pi_task_t task;
  atxp032_erase_chip_async(device, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_erase_sector(struct pi_device *device, uint32_t pi_flash_addr)
{
  pi_task_t task;
  atxp032_erase_sector_async(device, pi_flash_addr, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_erase(struct pi_device *device, uint32_t pi_flash_addr, int size)
{
  pi_task_t task;
  pi_task_block(&task);
  atxp032_erase_async(device, pi_flash_addr, size, &task);
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_reg_set(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_task_t task;
  atxp032_reg_set_async(device, pi_flash_addr, value, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_reg_get(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_task_t task;
  atxp032_reg_get_async(device, pi_flash_addr, value, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_copy(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc)
{
  pi_task_t task;
  pi_task_block(&task);
  if (atxp032_copy_async(device, pi_flash_addr, buffer, size, ext2loc, &task))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}

static inline int atxp032_copy_2d(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc)
{
  pi_task_t task;
  pi_task_block(&task);
  if (atxp032_copy_2d_async(device, pi_flash_addr, buffer, size, stride, length, ext2loc, &task))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}

static pi_flash_api_t atxp032_api = {
  .open                 = &atxp032_open,
  .close                = &atxp032_close,
  .ioctl                = &atxp032_ioctl,
  .read_async           = &atxp032_read_async,
  .program_async        = &atxp032_program_async,
  .erase_chip_async     = &atxp032_erase_chip_async,
  .erase_sector_async   = &atxp032_erase_sector_async,
  .erase_async          = &atxp032_erase_async,
  .reg_set_async        = &atxp032_reg_set_async,
  .reg_get_async        = &atxp032_reg_get_async,
  .copy_async           = &atxp032_copy_async,
  .copy_2d_async        = &atxp032_copy_2d_async,
  .read                 = &atxp032_read,
  .program              = &atxp032_program,
  .erase_chip           = &atxp032_erase_chip,
  .erase_sector         = &atxp032_erase_sector,
  .erase                = &atxp032_erase,
  .reg_set              = &atxp032_reg_set,
  .reg_get              = &atxp032_reg_get,
  .copy                 = &atxp032_copy,
  .copy_2d              = &atxp032_copy_2d,
};



void pi_atxp032_conf_init(struct pi_atxp032_conf *conf)
{
  conf->flash.api = &atxp032_api;
  bsp_atxp032_conf_init(conf);
  __flash_conf_init(&conf->flash);
  conf->xip_en = 0;
}
