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

// Driver for the octo spi flash Adesto MX25U

#include "pmsis.h"
#include "bsp/bsp.h"
#include "pmsis/drivers/octospi.h"

#ifndef PI_LOCAL_CODE
#define PI_LOCAL_CODE
#endif

#if defined(CONFIG_XIP)
// For now we always activate XIP locking since we only have flash which do not support concurrent read and write.
// This has to be deactivated for flash which support it (e.g. ATXP064R)
#define MX25U_LOCK_XIP
#endif

#define MX25U_READ_STATUS_CMD_SPI (0x05 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_READ_STATUS_CMD_OCTO (0x05FA | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_READ_STATUS_LATENCY_SPI 0
#define MX25U_READ_STATUS_LATENCY_OCTO 4
#define MX25U_READ_STATUS_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_0 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define MX25U_READ_STATUS_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_WRITE_STATUS_CMD (0x71 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_WRITE_STATUS_LATENCY_SPI 0
#define MX25U_WRITE_STATUS_LATENCY_OCTO 0
#define MX25U_WRITE_STATUS_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define MX25U_WRITE_STATUS_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_0 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_READ_CONFREG_CMD_SPI (0x71 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_READ_CONFREG_CMD_OCTO (0x718E | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_READ_CONFREG_LATENCY_SPI 0
#define MX25U_READ_CONFREG_LATENCY_OCTO 4
#define MX25U_READ_CONFREG_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define MX25U_READ_CONFREG_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_WRITE_CONFREG_CMD (0x72 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_WRITE_CONFREG_LATENCY_SPI 0
#define MX25U_WRITE_CONFREG_LATENCY_OCTO 0
#define MX25U_WRITE_CONFREG_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define MX25U_WRITE_CONFREG_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_ADDR_SIZE_0 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_WRITE_ENABLE_CMD_SPI (0x0006 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_WRITE_ENABLE_CMD_OCTO (0x06F9 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_WRITE_ENABLE_LATENCY_SPI 0
#define MX25U_WRITE_ENABLE_LATENCY_OCTO 0
#define MX25U_WRITE_ENABLE_FLAGS_SPI (PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_LINE_SINGLE | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_DATA_STR)
#define MX25U_WRITE_ENABLE_FLAGS_OCTO (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_ERASE_CMD (0x21DE | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_ERASE_LATENCY 0
#define MX25U_ERASE_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR)

#define MX25U_PROGRAM_CMD (0x12ED | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_PROGRAM_LATENCY 0
#define MX25U_PROGRAM_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR | PI_OCTOSPI_FLAG_DATA_DTR_MSB)

#define MX25U_READ_CMD (0xEE11 | PI_OCTOSPI_CMD_ADDR_EVEN)
#define MX25U_READ_LATENCY 20
#define MX25U_READ_FLAGS (PI_OCTOSPI_FLAG_CMD_SIZE_2 | PI_OCTOSPI_FLAG_ADDR_SIZE_4 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_DTR | PI_OCTOSPI_FLAG_ADDR_DTR | PI_OCTOSPI_FLAG_DATA_DTR | PI_OCTOSPI_FLAG_DATA_DTR_MSB)

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
    // Used for communications with mx25u through udma
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

} mx25u_t;


static pi_octospi_op_conf_t mx25u_erase_op = { .cmd=MX25U_ERASE_CMD, .latency=MX25U_ERASE_LATENCY, .flags=MX25U_ERASE_FLAGS };

static pi_octospi_op_conf_t mx25u_program_op = { .cmd=MX25U_PROGRAM_CMD, .latency=MX25U_PROGRAM_LATENCY, .flags=MX25U_PROGRAM_FLAGS };

static pi_octospi_op_conf_t mx25u_read_op = { .cmd=MX25U_READ_CMD, .latency=MX25U_READ_LATENCY, .flags=MX25U_READ_FLAGS };



static void mx25u_program_async(struct pi_device *device, uint32_t octospi_addr, const void *data, uint32_t size, pi_task_t *task);

static void mx25u_check_program(void *arg);

static void mx25u_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task);

static int mx25u_stall_task(mx25u_t *mx25u, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

static void mx25u_handle_pending_task(void *arg);

static void mx25u_erase_chip_async(struct pi_device *device, pi_task_t *task);

static void mx25u_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task);

static void mx25u_set_reg_exec(mx25u_t *mx25u, unsigned int addr, unsigned short value)
{
    mx25u->udma_buffer[0] = value;
    //pi_octospi_write(&mx25u->octospi_device, addr, mx25u->udma_buffer, 2);
}



// TODO should be moved to pmsis api
static void pi_task_enqueue(pi_task_t *task)
{
    pi_task_push(task);
}



static unsigned short mx25u_get_reg_exec(mx25u_t *mx25u, unsigned int addr)
{
    //pi_octospi_read(&mx25u->octospi_device, addr, mx25u->udma_buffer, 4);
    return mx25u->udma_buffer[0];
}


#ifdef CONFIG_XIP

PI_LOCAL_CODE static uint32_t mx25u_get_status(mx25u_t *mx25u)
{
    struct pi_task task;
    uint32_t data;
    pi_octospi_op_conf_t op = { .cmd=MX25U_READ_STATUS_CMD_OCTO, .latency=MX25U_READ_STATUS_LATENCY_OCTO, .flags=MX25U_READ_STATUS_FLAGS_OCTO };
    pi_octospi_read_async(&mx25u->octospi_device, 0, &data, 4, &op, pi_task_block(&task));
    pi_task_wait_on_xip(&task);
    return data;
}

#else

static uint32_t mx25u_get_status(mx25u_t *mx25u)
{
    uint32_t data;
    pi_octospi_op_conf_t op = { .cmd=MX25U_READ_STATUS_CMD_OCTO, .latency=MX25U_READ_STATUS_LATENCY_OCTO, .flags=MX25U_READ_STATUS_FLAGS_OCTO };
    pi_octospi_read(&mx25u->octospi_device, 0, &data, 4, &op);
    return data;
}

#endif


PI_LOCAL_CODE static int mx25u_is_busy(mx25u_t *mx25u)
{
    uint32_t value = mx25u_get_status(mx25u);
    return (value >> 0) & 1;
}


static void mx25u_write_enable(mx25u_t *mx25u)
{
    pi_octospi_op_conf_t op = { .cmd=MX25U_WRITE_ENABLE_CMD_OCTO, .latency=MX25U_WRITE_ENABLE_LATENCY_OCTO, .flags=MX25U_WRITE_ENABLE_FLAGS_OCTO };
    int dummy = 0;
    pi_octospi_write(&mx25u->octospi_device, 0, &dummy, 0, &op);
}



static int mx25u_open(struct pi_device *device)
{
    struct pi_mx25u51245g_conf *conf = (struct pi_mx25u51245g_conf *)device->config;

    mx25u_t *mx25u = (mx25u_t *)pmsis_l2_malloc(sizeof(mx25u_t));
    if (mx25u == NULL)
    {
        return -1;
    }

    device->data = (void *)mx25u;

    if (bsp_mx25u51245g_open(conf))
    {
        goto error;
    }

    struct pi_octospi_conf octospi_conf;
    pi_octospi_conf_init(&octospi_conf);

    octospi_conf.id = (unsigned char) conf->spi_itf;
    octospi_conf.cs = conf->spi_cs;
    octospi_conf.type = PI_OCTOSPI_TYPE_FLASH;
    octospi_conf.xip_en = conf->xip_en;
    octospi_conf.baudrate = conf->baudrate;

    pi_open_from_conf(&mx25u->octospi_device, &octospi_conf);

    int32_t error = pi_octospi_open(&mx25u->octospi_device);
    if (error)
    {
        goto error;
    }

    mx25u->pending_task = NULL;
    mx25u->waiting_first = NULL;

    mx25u->erase_task = NULL;
    mx25u->erase_waiting_first = NULL;

    // Activate DTR octospi mode and DTR
    {
        pi_octospi_op_conf_t op_we = { .cmd=MX25U_WRITE_ENABLE_CMD_SPI, .latency=MX25U_WRITE_ENABLE_LATENCY_SPI, .flags=MX25U_WRITE_ENABLE_FLAGS_SPI };
        pi_octospi_write(&mx25u->octospi_device, 0, NULL, 0, &op_we);

        pi_octospi_op_conf_t op_ws = { .cmd=MX25U_WRITE_CONFREG_CMD, .latency=MX25U_WRITE_CONFREG_LATENCY_SPI, .flags=MX25U_WRITE_CONFREG_FLAGS_SPI };
        uint32_t data = 1 << 1;
        pi_octospi_write(&mx25u->octospi_device, 0, &data, 1, &op_ws);

        pi_time_wait_us(60);
    }

    return 0;

error:
    pmsis_l2_malloc_free(mx25u, sizeof(mx25u_t));
    return -2;
}



static void mx25u_close(struct pi_device *device)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;
    pi_octospi_close(&mx25u->octospi_device);
    pmsis_l2_malloc_free(mx25u, sizeof(mx25u_t));
}



static int32_t mx25u_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
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


void pi_mx25u_deep_sleep_enter(pi_device_t *device)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;
    pi_octospi_op_conf_t op_we = {
        .cmd=0xB9,
        .latency=0,
        .flags=PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_DATA_DTR
    };
    int dummy = 0;
    pi_octospi_write(&mx25u->octospi_device, 0, &dummy, 1, &op_we);
}


void pi_mx25u_deep_sleep_exit(pi_device_t *device)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;
    pi_octospi_op_conf_t op_we = {
        .cmd=0xAB,
        .latency=0,
        .flags=PI_OCTOSPI_FLAG_CMD_SIZE_1 | PI_OCTOSPI_FLAG_LINE_OCTO | PI_OCTOSPI_FLAG_CMD_STR | PI_OCTOSPI_FLAG_DATA_DTR
    };
    int dummy = 0;
    pi_octospi_write(&mx25u->octospi_device, 0, &dummy, 1, &op_we);
}


static void mx25u_reg_set_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_REG_SET, addr, (uint32_t)value, 0, 0, 0))
        return;

    mx25u_set_reg_exec(mx25u, addr, *(uint16_t *)value);

    mx25u_handle_pending_task(device);
}



static void mx25u_reg_get_async(struct pi_device *device, uint32_t addr, uint8_t *value, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_REG_GET, addr, (uint32_t)value, 0, 0, 0))
        return;

    *(uint16_t *)value = mx25u_get_reg_exec(mx25u, addr);

    mx25u_handle_pending_task(device);
}



static void mx25u_read_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_READ, addr, (uint32_t)data, size, 0, 0))
        return;

    pi_octospi_read_async(&mx25u->octospi_device, addr, data, size, &mx25u_read_op, pi_task_callback(&mx25u->task, mx25u_handle_pending_task, device));
}



static void mx25u_read_2d_async(struct pi_device *device, uint32_t addr, void *data, uint32_t size, uint32_t stride, uint32_t length, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_READ_2D, addr, (uint32_t)data, size, stride, length))
    {
        return;
    }

    pi_octospi_read_2d_async(&mx25u->octospi_device, addr, data, size, stride, length, &mx25u_read_op, pi_task_callback(&mx25u->task, mx25u_handle_pending_task, device));
}



static void mx25u_handle_pending_task(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    uint32_t irq = disable_irq();

    pi_task_enqueue(mx25u->pending_task);
    mx25u->pending_task = NULL;

    pi_task_t *task = mx25u->waiting_first;

    if (task)
    {
        mx25u->waiting_first = task->next;
    }

    restore_irq(irq);

    if (task)
    {
        if (task->data[0] == STALL_TASK_PROGRAM)
        {
            mx25u_program_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
        }
        else if (task->data[0] == STALL_TASK_ERASE_CHIP)
        {
            mx25u_erase_chip_async(device, task);
        }
        else if (task->data[0] == STALL_TASK_ERASE_SECTOR)
        {
            mx25u_erase_sector_async(device, task->data[1], task);
        }
        else if (task->data[0] == STALL_TASK_REG_SET)
        {
            mx25u_reg_set_async(device, task->data[1], (uint8_t *)task->data[2], task);
        }
        else if (task->data[0] == STALL_TASK_REG_GET)
        {
            mx25u_reg_get_async(device, task->data[1], (uint8_t *)task->data[2], task);
        }
        else if (task->data[0] == STALL_TASK_READ)
        {
            mx25u_read_async(device, task->data[1], (void *)task->data[2], task->data[3], task);
        }
        else if (task->data[0] == STALL_TASK_READ_2D)
        {
            mx25u_read_2d_async(device, task->data[1], (void *)task->data[2], task->data[3], task->data[4], task->data[5], task);
        }
    }
}



static void mx25u_handle_pending_erase_task(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    uint32_t irq = disable_irq();

    pi_task_enqueue(mx25u->erase_task);
    mx25u->erase_task = NULL;

    pi_task_t *task = mx25u->erase_waiting_first;
    if (task)
    {
        mx25u->erase_waiting_first = task->next;
    }

    restore_irq(irq);

    if (task)
    {
        mx25u_erase_async(device, task->data[1], task->data[2], task);
    }
}



static int mx25u_stall_task(mx25u_t *mx25u, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    uint32_t irq = disable_irq();

    if (mx25u->pending_task != NULL)
    {
        task->data[0] = id;
        task->data[1] = arg0;
        task->data[2] = arg1;
        task->data[3] = arg2;
        task->data[4] = arg3;
        task->data[5] = arg4;
        task->next = NULL;

        if (mx25u->waiting_first)
        {
        mx25u->waiting_last->next = task;
        }
        else
        {
        mx25u->waiting_first = task;
        }

        mx25u->waiting_last = task;

        restore_irq(irq);
        return 1;
    }

    mx25u->pending_task = task;

    restore_irq(irq);
    return 0;
}



static int mx25u_stall_erase_task(mx25u_t *mx25u, pi_task_t *task, uint32_t id, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
  uint32_t irq = disable_irq();

    if (mx25u->erase_task != NULL)
    {
        task->data[0] = id;
        task->data[1] = arg0;
        task->data[2] = arg1;
        task->data[3] = arg2;
        task->next = NULL;

        if (mx25u->erase_waiting_first)
        {
            mx25u->erase_waiting_last->next = task;
        }
        else
        {
            mx25u->erase_waiting_first = task;
        }

        mx25u->erase_waiting_last = task;

        restore_irq(irq);
        return 1;
    }

    mx25u->erase_task = task;

    restore_irq(irq);
    return 0;
}


PI_LOCAL_CODE static void mx25u_program_resume(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u->pending_size == 0)
    {
        mx25u_handle_pending_task(device);
    }
    else
    {
    #ifdef MX25U_LOCK_XIP
        // When XIP is active and flash does not support concurrent read and write, loop on the program operation until it is done
        // Since XIP can not work at the same time.
        // On multi-threaded systems, we should also put on hold any request to this driver and resume them after the program operation is done,
        // since the octosp drover will let other requests execute between 2 operations, to let other devices being used.
        while (mx25u->pending_size > 0)
        {
        mx25u_write_enable(mx25u);

        unsigned int iter_size = 256 - (mx25u->pending_octospi_addr & 0xff);
        if (iter_size > mx25u->pending_size)
            iter_size = mx25u->pending_size;

        uint32_t octospi_addr = mx25u->pending_octospi_addr;
        uint32_t data = mx25u->pending_data;

        mx25u->pending_octospi_addr += iter_size;
        mx25u->pending_data += iter_size;
        mx25u->pending_size -= iter_size;

        // In XIP mode, we need to lock XIP refills to avoid having a read while the flash is doing the program operation.
        pi_octospi_xip_lock(&mx25u->octospi_device);

        // Even though the operation should be asynchronous, do everything synchronously to avoid XIP refills until the operation is done
        struct pi_task task;
        pi_octospi_write_async(&mx25u->octospi_device, octospi_addr, (void *)data, iter_size, &mx25u_program_op, pi_task_block(&task));
        pi_task_wait_on_xip(&task);
        while (mx25u_is_busy(mx25u))
        {
            for (int i=0; i<32768/1000; i++)
            {
                pos_wait_for_event(1<<ARCHI_FC_EVT_CLK_REF_RISE);
            }
        }
        pi_octospi_xip_unlock(&mx25u->octospi_device);
        }

        mx25u_handle_pending_task(device);
    #else
        unsigned int iter_size = 256 - (mx25u->pending_octospi_addr & 0xff);
        if (iter_size > mx25u->pending_size)
            iter_size = mx25u->pending_size;

        uint32_t octospi_addr = mx25u->pending_octospi_addr;
        uint32_t data = mx25u->pending_data;

        mx25u->pending_octospi_addr += iter_size;
        mx25u->pending_data += iter_size;
        mx25u->pending_size -= iter_size;

        mx25u_write_enable(mx25u);
        pi_octospi_write_async(&mx25u->octospi_device, octospi_addr, (void *)data, iter_size, &mx25u_program_op, pi_task_callback(&mx25u->task, mx25u_check_program, device));
    #endif
    }
}



static void mx25u_check_program(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_is_busy(mx25u))
    {
        // Typical buffer programming time is 4ms. Note that this could be optimzed by taking into account buffer size
        pi_task_push_delayed_us(pi_task_callback(&mx25u->task, mx25u_check_program, device), 1000);
    }
    else
    {
        mx25u_program_resume(device);
    }
}



PI_LOCAL_CODE static void mx25u_program_async(struct pi_device *device, uint32_t octospi_addr, const void *data, uint32_t size, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_PROGRAM, octospi_addr, (uint32_t)data, size, 0, 0))
        return;

    mx25u->pending_octospi_addr = octospi_addr;
    mx25u->pending_data = (uint32_t)data;
    mx25u->pending_size = size;

    mx25u_program_resume(device);
}





static void mx25u_check_erase(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    uint32_t reg_status; // = mx25u_get_status_reg(mx25u);
    if (mx25u_is_busy(mx25u))
    {
        // Typical sector erase time is 25ms but keep it short as this time is shorter or some platform
        pi_task_push_delayed_us(pi_task_callback(&mx25u->task, mx25u_check_erase, device), 10000);
    }
    else
    {
        mx25u_handle_pending_task(device);
    }
}


static void mx25u_erase_chip_async(struct pi_device *device, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_ERASE_CHIP, 0, 0, 0, 0, 0))
        return;



    pi_task_push_delayed_us(pi_task_callback(&mx25u->task, mx25u_check_erase, device), 100000);
}


static void mx25u_erase_sector_async(struct pi_device *device, uint32_t addr, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_task(mx25u, task, STALL_TASK_ERASE_SECTOR, addr, 0, 0, 0, 0))
        return;

    mx25u_write_enable(mx25u);

    // We don't need to send data but UDMA needs at least 1 byte, this will be ignored by the flash
    pi_octospi_write_async(&mx25u->octospi_device, addr, mx25u->udma_buffer, 0, &mx25u_erase_op, pi_task_callback(&mx25u->task, mx25u_check_erase, device));
}



PI_LOCAL_CODE static void mx25u_erase_resume(void *arg)
{
    struct pi_device *device = (struct pi_device *)arg;
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u->pending_erase_size == 0)
    {
        mx25u_handle_pending_erase_task(device);
    }
    else
    {
    #ifdef MX25U_LOCK_XIP
        // When XIP is active and flash does not support concurrent read and write, loop on the erase operation until it is done
        // Since XIP can not work at the same time.
        // On multi-threaded systems, we should also put on hold any request to this driver and resume them after the program operation is done,
        // since the octosp drover will let other requests execute between 2 operations, to let other devices being used.
        while (mx25u->pending_erase_size > 0)
        {
        mx25u_write_enable(mx25u);

        // In XIP mode, we need to lock XIP refills to avoid having a read while the flash is doing the program operation.
        pi_octospi_xip_lock(&mx25u->octospi_device);

        unsigned int iter_size = SECTOR_SIZE - (mx25u->pending_erase_octospi_addr & (SECTOR_SIZE - 1));
        if (iter_size > mx25u->pending_erase_size)
            iter_size = mx25u->pending_erase_size;

        uint32_t octospi_addr = mx25u->pending_erase_octospi_addr;

        mx25u->pending_erase_octospi_addr += iter_size;
        mx25u->pending_erase_size -= iter_size;

        struct pi_task task;

        // We don't need to send data but UDMA needs at least 1 byte, this will be ignored by the flash
        pi_octospi_write_async(&mx25u->octospi_device, octospi_addr, mx25u->udma_buffer, 1, &mx25u_erase_op, pi_task_block(&task));
        pi_task_wait_on_xip(&task);

        while (mx25u_is_busy(mx25u))
        {
            for (int i=0; i<32768/100; i++)
            {
                pos_wait_for_event(1<<ARCHI_FC_EVT_CLK_REF_RISE);
            }
        }
        pi_octospi_xip_unlock(&mx25u->octospi_device);
        }

        mx25u_handle_pending_erase_task(device);
    #else
        unsigned int iter_size = SECTOR_SIZE - (mx25u->pending_erase_octospi_addr & (SECTOR_SIZE - 1));
        if (iter_size > mx25u->pending_erase_size)
            iter_size = mx25u->pending_erase_size;

        uint32_t octospi_addr = mx25u->pending_erase_octospi_addr;
        mx25u_erase_sector_async(device, octospi_addr, pi_task_callback(&mx25u->task2, mx25u_erase_resume, device));

        mx25u->pending_erase_octospi_addr += iter_size;
        mx25u->pending_erase_size -= iter_size;
    #endif
    }
}



static void mx25u_erase_async(struct pi_device *device, uint32_t addr, int size, pi_task_t *task)
{
    mx25u_t *mx25u = (mx25u_t *)device->data;

    if (mx25u_stall_erase_task(mx25u, task, 3, addr, size, 0))
    {
        return;
    }

    mx25u->pending_erase_octospi_addr = addr;
    mx25u->pending_erase_size = size;

    mx25u_erase_resume(device);
}



static int mx25u_copy_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, int ext2loc, pi_task_t *task)
{
    if (!ext2loc)
        mx25u_program_async(device, flash_addr, buffer, size, task);
    else
        mx25u_read_async(device, flash_addr, buffer, size, task);

    return 0;
}



static int mx25u_copy_2d_async(struct pi_device *device, uint32_t flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc, pi_task_t *task)
{
    if (!ext2loc)
        return -1;

    mx25u_read_2d_async(device, flash_addr, buffer, size, stride, length, task);

    return 0;
}

static int mx25u_read(struct pi_device *device, uint32_t pi_flash_addr, void *data, uint32_t size)
{
    pi_task_t task;
    mx25u_read_async(device, pi_flash_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}


static int mx25u_program(struct pi_device *device, uint32_t pi_flash_addr, const void *data, uint32_t size)
{
    pi_task_t task;
    mx25u_program_async(device, pi_flash_addr, data, size, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_erase_chip(struct pi_device *device)
{
    pi_task_t task;
    mx25u_erase_chip_async(device, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_erase_sector(struct pi_device *device, uint32_t pi_flash_addr)
{
    pi_task_t task;
    mx25u_erase_sector_async(device, pi_flash_addr, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_erase(struct pi_device *device, uint32_t pi_flash_addr, int size)
{
    pi_task_t task;
    pi_task_block(&task);
    mx25u_erase_async(device, pi_flash_addr, size, &task);
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_reg_set(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
    pi_task_t task;
    mx25u_reg_set_async(device, pi_flash_addr, value, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_reg_get(struct pi_device *device, uint32_t pi_flash_addr, uint8_t *value)
{
    pi_task_t task;
    mx25u_reg_get_async(device, pi_flash_addr, value, pi_task_block(&task));
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_copy(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, int ext2loc)
{
    pi_task_t task;
    pi_task_block(&task);
    if (mx25u_copy_async(device, pi_flash_addr, buffer, size, ext2loc, &task))
        return -1;
    pi_task_wait_on(&task);
    return 0;
}

static inline int mx25u_copy_2d(struct pi_device *device, uint32_t pi_flash_addr, void *buffer, uint32_t size, uint32_t stride, uint32_t length, int ext2loc)
{
    pi_task_t task;
    pi_task_block(&task);
    if (mx25u_copy_2d_async(device, pi_flash_addr, buffer, size, stride, length, ext2loc, &task))
        return -1;
    pi_task_wait_on(&task);
    return 0;
}

static pi_flash_api_t mx25u_api = {
    .open                 = &mx25u_open,
    .close                = &mx25u_close,
    .ioctl                = &mx25u_ioctl,
    .read_async           = &mx25u_read_async,
    .program_async        = &mx25u_program_async,
    .erase_chip_async     = &mx25u_erase_chip_async,
    .erase_sector_async   = &mx25u_erase_sector_async,
    .erase_async          = &mx25u_erase_async,
    .reg_set_async        = &mx25u_reg_set_async,
    .reg_get_async        = &mx25u_reg_get_async,
    .copy_async           = &mx25u_copy_async,
    .copy_2d_async        = &mx25u_copy_2d_async,
    .read                 = &mx25u_read,
    .program              = &mx25u_program,
    .erase_chip           = &mx25u_erase_chip,
    .erase_sector         = &mx25u_erase_sector,
    .erase                = &mx25u_erase,
    .reg_set              = &mx25u_reg_set,
    .reg_get              = &mx25u_reg_get,
    .copy                 = &mx25u_copy,
    .copy_2d              = &mx25u_copy_2d,
};



void pi_mx25u51245g_conf_init(struct pi_mx25u51245g_conf *conf)
{
    conf->flash.api = &mx25u_api;
    bsp_mx25u51245g_conf_init(conf);
    __flash_conf_init(&conf->flash);
    conf->xip_en = 0;
}
