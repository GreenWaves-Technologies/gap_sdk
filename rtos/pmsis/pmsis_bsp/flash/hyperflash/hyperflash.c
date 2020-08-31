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

#include "pmsis.h"
#include "bsp/bsp.h"
#include "pmsis/drivers/hyperbus.h"


#define SECTOR_SIZE (1<<18)

#define STALL_TASK_PROGRAM      0
#define STALL_TASK_ERASE_CHIP   1
#define STALL_TASK_ERASE_SECTOR 2
#define STALL_TASK_REG_SET      3
#define STALL_TASK_REG_GET      4
#define STALL_TASK_READ         5
#define STALL_TASK_READ_2D      6

typedef struct {
  struct pi_device hyper_device;
  // Used for communications with hyperflash through udma
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
  uint32_t pending_hyper_addr;
  uint32_t pending_data;
  uint32_t pending_size;

  // Description of on-going task for erase operation. The FSM will keep executing
  // until this operation is done
  uint32_t pending_erase_hyper_addr;
  uint32_t pending_erase_size;

} hyperflash_t;



static void hyperflash_program_async(struct pi_device *device, uint32_t hyper_addr, const void *data, uint32_t size, pi_task_t *task);

static void hyperflash_check_program(void *arg);

static void hyperflash_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task);

static int hyperflash_stall_task(hyperflash_t *hyperflash, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

static void hyperflash_handle_pending_task(void *arg);

static void hyperflash_erase_chip_async(struct pi_device *device, pi_task_t *task);

static void hyperflash_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task);

static void hyperflash_set_reg_exec(hyperflash_t *hyperflash, unsigned int addr, unsigned short value)
{
  hyperflash->udma_buffer[0] = value;
  pi_hyper_write(&hyperflash->hyper_device, addr, hyperflash->udma_buffer, 2);
}



// TODO should be moved to pmsis api
static void pi_task_enqueue(pi_task_t *task)
{
  pi_task_push(task);
}



static unsigned short hyperflash_get_reg_exec(hyperflash_t *hyperflash, unsigned int addr)
{
  pi_hyper_read(&hyperflash->hyper_device, addr, hyperflash->udma_buffer, 4);
  return hyperflash->udma_buffer[0];
}



static unsigned int hyperflash_get_status_reg(hyperflash_t *hyperflash)
{
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0x70);
  return hyperflash_get_reg_exec(hyperflash, 0);
}



static int hyperflash_open(struct pi_device *device)
{
  struct pi_hyperflash_conf *conf = (struct pi_hyperflash_conf *)device->config;

  hyperflash_t *hyperflash = (hyperflash_t *)pmsis_l2_malloc(sizeof(hyperflash_t));
  if (hyperflash == NULL)
  {
    return -1;
  }

  device->data = (void *)hyperflash;

  if (bsp_hyperflash_open(conf))
  {
    goto error;
  }

  struct pi_hyper_conf hyper_conf = {0};
  pi_hyper_conf_init(&hyper_conf);

  hyper_conf.id = (unsigned char) conf->hyper_itf;
  hyper_conf.cs = conf->hyper_cs;
  hyper_conf.type = PI_HYPER_TYPE_FLASH;
  hyper_conf.xip_en = conf->xip_en;

  pi_open_from_conf(&hyperflash->hyper_device, &hyper_conf);

  int32_t error = pi_hyper_open(&hyperflash->hyper_device);
  if (error)
  {
      goto error;
  }

  hyperflash->pending_task = NULL;
  hyperflash->waiting_first = NULL;

  hyperflash->erase_task = NULL;
  hyperflash->erase_waiting_first = NULL;

  return 0;

error:
  pmsis_l2_malloc_free(hyperflash, sizeof(hyperflash_t));
  return -2;
}



static void hyperflash_close(struct pi_device *device)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;
  pi_hyper_close(&hyperflash->hyper_device);
  pmsis_l2_malloc_free(hyperflash, sizeof(hyperflash_t));
}



static int32_t hyperflash_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
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



static void hyperflash_reg_set_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_REG_SET, addr, (uint32_t)value, 0, 0, 0))
    return;

  hyperflash_set_reg_exec(hyperflash, addr, *(uint16_t *)value);

  hyperflash_handle_pending_task(device);
}



static void hyperflash_reg_get_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_REG_GET, addr, (uint32_t)value, 0, 0, 0))
    return;

  *(uint16_t *)value = hyperflash_get_reg_exec(hyperflash, addr);

  hyperflash_handle_pending_task(device);
}



static void hyperflash_read_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_READ, addr, (uint32_t)data, size, 0, 0))
    return;

  pi_hyper_read_async(&hyperflash->hyper_device, addr, data, size, pi_task_callback(&hyperflash->task, hyperflash_handle_pending_task, device));
}



static void hyperflash_read_2d_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, uint32_t stride, uint32_t length, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_READ_2D, addr, (uint32_t)data, size, stride, length))
  {
    return;
  }

  pi_hyper_read_2d_async(&hyperflash->hyper_device, addr, data, size, stride, length, pi_task_callback(&hyperflash->task, hyperflash_handle_pending_task, device));
}



static void hyperflash_handle_pending_task(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  uint32_t irq = disable_irq();

  pi_task_enqueue(hyperflash->pending_task);
  hyperflash->pending_task = NULL;

  pi_task_t *task = hyperflash->waiting_first;

  if (task)
  {
#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
    hyperflash->waiting_first = task->next;
#else
    hyperflash->waiting_first = task->implem.next;
#endif  /* PMSIS_DRIVERS */
  }

  restore_irq(irq);

  if (task)
  {
#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
    if (task->data[0] == STALL_TASK_PROGRAM)
    {
      hyperflash_program_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
    }
    else if (task->data[0] == STALL_TASK_ERASE_CHIP)
    {
      hyperflash_erase_chip_async(device, task);
    }
    else if (task->data[0] == STALL_TASK_ERASE_SECTOR)
    {
      hyperflash_erase_sector_async(device, task->data[1], task);
    }
    else if (task->data[0] == STALL_TASK_REG_SET)
    {
      hyperflash_reg_set_async(device, task->data[1], (uint8_t *)task->data[2], task);
    }
    else if (task->data[0] == STALL_TASK_REG_GET)
    {
      hyperflash_reg_get_async(device, task->data[1], (uint8_t *)task->data[2], task);
    }
    else if (task->data[0] == STALL_TASK_READ)
    {
      hyperflash_read_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
    }
    else if (task->data[0] == STALL_TASK_READ_2D)
    {
      hyperflash_read_2d_async(device, task->data[1], (void *)task->data[2], task->data[3], task->data[4], task->data[5], task);
    }
#else
    if (task->implem.data[0] == STALL_TASK_PROGRAM)
    {
      hyperflash_program_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task);
    }
    else if (task->implem.data[0] == STALL_TASK_ERASE_CHIP)
    {
      hyperflash_erase_chip_async(device, task);
    }
    else if (task->implem.data[0] == STALL_TASK_ERASE_SECTOR)
    {
      hyperflash_erase_sector_async(device, task->implem.data[1], task);
    }
    else if (task->implem.data[0] == STALL_TASK_REG_SET)
    {
      hyperflash_reg_set_async(device, task->implem.data[1], (uint8_t *)task->implem.data[2], task);
    }
    else if (task->implem.data[0] == STALL_TASK_REG_GET)
    {
      hyperflash_reg_get_async(device, task->implem.data[1], (uint8_t *)task->implem.data[2], task);
    }
    else if (task->implem.data[0] == STALL_TASK_READ)
    {
      hyperflash_read_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task);
    }
    else if (task->implem.data[0] == STALL_TASK_READ_2D)
    {
      hyperflash_read_2d_async(device, task->implem.data[1], (void *)task->implem.data[2], task->implem.data[3], task->implem.data[4], task->implem.data[5], task);
    }
#endif  /* PMSIS_DRIVERS */
  }
}



static void hyperflash_handle_pending_erase_task(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  uint32_t irq = disable_irq();

  pi_task_enqueue(hyperflash->erase_task);
  hyperflash->erase_task = NULL;

  pi_task_t *task = hyperflash->erase_waiting_first;
  if (task)
  {
#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
    hyperflash->erase_waiting_first = task->next;
#else
    hyperflash->erase_waiting_first = task->implem.next;
#endif  /* PMSIS_DRIVERS */
  }

  restore_irq(irq);

  if (task)
  {
#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
    hyperflash_erase_async(device, task->data[1], task->data[2], task);
#else
    hyperflash_erase_async(device, task->implem.data[1], task->implem.data[2], task);
#endif  /* PMSIS_DRIVERS */
  }
}



static int hyperflash_stall_task(hyperflash_t *hyperflash, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
  uint32_t irq = disable_irq();

#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
  if (hyperflash->pending_task != NULL)
  {
    task->data[0] = id;
    task->data[1] = arg0;
    task->data[2] = arg1;
    task->data[3] = arg2;
    task->data[4] = arg3;
    task->data[5] = arg4;
    task->next = NULL;

    if (hyperflash->waiting_first)
    {
      hyperflash->waiting_last->next = task;
    }
    else
    {
      hyperflash->waiting_first = task;
    }

    hyperflash->waiting_last = task;

    restore_irq(irq);
    return 1;
  }
#else
  if (hyperflash->pending_task != NULL)
  {
    task->implem.data[0] = id;
    task->implem.data[1] = arg0;
    task->implem.data[2] = arg1;
    task->implem.data[3] = arg2;
    task->implem.data[4] = arg3;
    task->implem.data[5] = arg4;

    if (hyperflash->waiting_first)
      hyperflash->waiting_last->implem.next = task;
    else
      hyperflash->waiting_first = task;

    hyperflash->waiting_last = task;
    task->implem.next = NULL;

    restore_irq(irq);
    return 1;
  }
#endif  /* PMSIS_DRIVERS */

  hyperflash->pending_task = task;

  restore_irq(irq);
  return 0;
}



static int hyperflash_stall_erase_task(hyperflash_t *hyperflash, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
  uint32_t irq = disable_irq();

#if defined(PMSIS_DRIVERS) || defined(__PULPOS2__)
  if (hyperflash->erase_task != NULL)
  {
    task->data[0] = id;
    task->data[1] = arg0;
    task->data[2] = arg1;
    task->data[3] = arg2;
    task->next = NULL;

    if (hyperflash->erase_waiting_first)
    {
      hyperflash->erase_waiting_last->next = task;
    }
    else
    {
      hyperflash->erase_waiting_first = task;
    }

    hyperflash->erase_waiting_last = task;

    restore_irq(irq);
    return 1;
  }
#else
  if (hyperflash->erase_task != NULL)
  {
    task->implem.data[0] = id;
    task->implem.data[1] = arg0;
    task->implem.data[2] = arg1;
    task->implem.data[3] = arg2;

    if (hyperflash->erase_waiting_first)
      hyperflash->erase_waiting_last->implem.next = task;
    else
      hyperflash->erase_waiting_first = task;

    hyperflash->erase_waiting_last = task;
    task->implem.next = NULL;
    restore_irq(irq);
    return 1;
  }
#endif  /* PMSIS_DRIVERS */

  hyperflash->erase_task = task;

  restore_irq(irq);
  return 0;
}



static void hyperflash_program_resume(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash->pending_size == 0)
  {
    hyperflash_handle_pending_task(device);
  }
  else
  {
    unsigned int iter_size = 512 - (hyperflash->pending_hyper_addr & 0x1ff);
      if (iter_size > hyperflash->pending_size)
        iter_size = hyperflash->pending_size;

    // Enqueue command header synchronously as this should be quick
    hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xAA);
    hyperflash_set_reg_exec(hyperflash, 0x2AA<<1, 0x55);
    hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xA0);

    uint32_t hyper_addr = hyperflash->pending_hyper_addr;
    uint32_t data = hyperflash->pending_data;

    hyperflash->pending_hyper_addr += iter_size;
    hyperflash->pending_data += iter_size;
    hyperflash->pending_size -= iter_size;

    pi_hyper_write_async(&hyperflash->hyper_device, hyper_addr, (void *)data, iter_size, pi_task_callback(&hyperflash->task, hyperflash_check_program, device));
  }
}



static void hyperflash_check_program(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (((hyperflash_get_status_reg(hyperflash) >> 7) & 1) == 0)
  {
    // Typical buffer programming time is 475us
    pi_task_push_delayed_us(pi_task_callback(&hyperflash->task, hyperflash_check_program, device), 250);
  }
  else
  {
    hyperflash_program_resume(device);
  }
}



static void hyperflash_program_async(struct pi_device *device, uint32_t hyper_addr, const void *data, uint32_t size, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_PROGRAM, hyper_addr, (uint32_t)data, size, 0, 0))
    return;

  hyperflash->pending_hyper_addr = hyper_addr;
  hyperflash->pending_data = (uint32_t)data;
  hyperflash->pending_size = size;

  hyperflash_program_resume(device);
}





static void hyperflash_check_erase(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  uint32_t reg_status = hyperflash_get_status_reg(hyperflash);
  if (((reg_status >> 7) & 1) == 0)
  {
    pi_task_push_delayed_us(pi_task_callback(&hyperflash->task, hyperflash_check_erase, device), 100000);
  }
  else
  {
    hyperflash_handle_pending_task(device);
  }
}


static void hyperflash_erase_chip_async(struct pi_device *device, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_ERASE_CHIP, 0, 0, 0, 0, 0))
    return;

  // Enqueue command header synchronously as this should be quick
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xAA);
  hyperflash_set_reg_exec(hyperflash, 0x2AA<<1, 0x55);
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0x80);
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xAA);
  hyperflash_set_reg_exec(hyperflash, 0x2AA<<1, 0x55);
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0x10);

  pi_task_push_delayed_us(pi_task_callback(&hyperflash->task, hyperflash_check_erase, device), 100000);
}


static void hyperflash_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_task(hyperflash, task, STALL_TASK_ERASE_SECTOR, addr, 0, 0, 0, 0))
    return;

  // Enqueue command header synchronously as this should be quick
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xAA);
  hyperflash_set_reg_exec(hyperflash, 0x2AA<<1, 0x55);
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0x80);
  hyperflash_set_reg_exec(hyperflash, 0x555<<1, 0xAA);
  hyperflash_set_reg_exec(hyperflash, 0x2AA<<1, 0x55);
  hyperflash_set_reg_exec(hyperflash, addr, 0x30);

  // Typical sector erase time is 930ms but keep it short as this time is shorter or some platform
  pi_task_push_delayed_us(pi_task_callback(&hyperflash->task, hyperflash_check_erase, device), 10000);
}



static void hyperflash_erase_resume(void *arg)
{
  struct pi_device *device = (struct pi_device *)arg;
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash->pending_erase_size == 0)
  {
    hyperflash_handle_pending_erase_task(device);
  }
  else
  {
    unsigned int iter_size = SECTOR_SIZE - (hyperflash->pending_erase_hyper_addr & (SECTOR_SIZE - 1));
    if (iter_size > hyperflash->pending_erase_size)
      iter_size = hyperflash->pending_erase_size;

    uint32_t hyper_addr = hyperflash->pending_erase_hyper_addr;
    hyperflash_erase_sector_async(device, hyper_addr, pi_task_callback(&hyperflash->task2, hyperflash_erase_resume, device));

    hyperflash->pending_erase_hyper_addr += iter_size;
    hyperflash->pending_erase_size -= iter_size;
  }
}



static void hyperflash_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task)
{
  hyperflash_t *hyperflash = (hyperflash_t *)device->data;

  if (hyperflash_stall_erase_task(hyperflash, task, 3, addr, size, 0))
  {
    return;
  }

  hyperflash->pending_erase_hyper_addr = addr;
  hyperflash->pending_erase_size = size;

  hyperflash_erase_resume(device);
}



static int hyperflash_copy_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
  if (!ext2loc)
    hyperflash_program_async(device, flash_addr, buffer, size, task);
  else
    hyperflash_read_async(device, flash_addr, buffer, size, task);

  return 0;
}



static int hyperflash_copy_2d_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
  if (!ext2loc)
    return -1;

  hyperflash_read_2d_async(device, flash_addr, buffer, size, stride, length, task);

  return 0;
}

static int hyperflash_read(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size)
{
  pi_task_t task;
  hyperflash_read_async(device, pi_flash_addr, data, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}


static int hyperflash_program(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size)
{
  pi_task_t task;
  hyperflash_program_async(device, pi_flash_addr, data, size, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_erase_chip(struct pi_device *device)
{
  pi_task_t task;
  hyperflash_erase_chip_async(device, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_erase_sector(struct pi_device *device, uint32_t pi_flash_addr)
{
  pi_task_t task;
  hyperflash_erase_sector_async(device, pi_flash_addr, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_erase(struct pi_device *device, uint32_t pi_flash_addr, int size)
{
  pi_task_t task;
  pi_task_block(&task);
  hyperflash_erase_async(device, pi_flash_addr, size, &task);
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_reg_set(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_task_t task;
  hyperflash_reg_set_async(device, pi_flash_addr, value, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_reg_get(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
  pi_task_t task;
  hyperflash_reg_get_async(device, pi_flash_addr, value, pi_task_block(&task));
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_copy(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc)
{
  pi_task_t task;
  pi_task_block(&task);
  if (hyperflash_copy_async(device, pi_flash_addr, buffer, size, ext2loc, &task))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}

static inline int hyperflash_copy_2d(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc)
{
  pi_task_t task;
  pi_task_block(&task);
  if (hyperflash_copy_2d_async(device, pi_flash_addr, buffer, size, stride, length, ext2loc, &task))
    return -1;
  pi_task_wait_on(&task);
  return 0;
}

static pi_flash_api_t hyperflash_api = {
  .open                 = &hyperflash_open,
  .close                = &hyperflash_close,
  .ioctl                = &hyperflash_ioctl,
  .read_async           = &hyperflash_read_async,
  .program_async        = &hyperflash_program_async,
  .erase_chip_async     = &hyperflash_erase_chip_async,
  .erase_sector_async   = &hyperflash_erase_sector_async,
  .erase_async          = &hyperflash_erase_async,
  .reg_set_async        = &hyperflash_reg_set_async,
  .reg_get_async        = &hyperflash_reg_get_async,
  .copy_async           = &hyperflash_copy_async,
  .copy_2d_async        = &hyperflash_copy_2d_async,
  .read                 = &hyperflash_read,
  .program              = &hyperflash_program,
  .erase_chip           = &hyperflash_erase_chip,
  .erase_sector         = &hyperflash_erase_sector,
  .erase                = &hyperflash_erase,
  .reg_set              = &hyperflash_reg_set,
  .reg_get              = &hyperflash_reg_get,
  .copy                 = &hyperflash_copy,
  .copy_2d              = &hyperflash_copy_2d,
};



void pi_hyperflash_conf_init(struct pi_hyperflash_conf *conf)
{
  conf->flash.api = &hyperflash_api;
  bsp_hyperflash_conf_init(conf);
  __flash_conf_init(&conf->flash);
  conf->xip_en = 0;
}
