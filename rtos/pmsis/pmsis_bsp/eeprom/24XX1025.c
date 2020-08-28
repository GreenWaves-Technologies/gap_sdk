/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/eeprom/24xx1025.h"

// Maximum write burst size the eeprom is supporting
#define CONFIG_24XX1025_BURST_SIZE 128

// Delay to wait after a write burst is done before doing the next one
#define CONFIG_24XX1025_FLUSH_DELAY_US 5000


typedef struct
{
    struct pi_device i2c_device;  // I2C device
    pi_task_t *waiting_first;     // First waiting copy
    pi_task_t *waiting_last;      // Last waiting copy
    pi_task_t *pending_task;      // Copy being handled
    int pending_write_size;       // Remaining size of the write copy being handled
    uint32_t pending_write_addr;  // Current eeprom address of the write copy being handled
    uint32_t pending_write_data;  // Current chip address of the write copy being handled
    pi_task_t internal_task;      // Task used for executing internal callbacks when handling write bursts
    int waiting_page_flush;       // Set to 1 when the driver is waiting for a write burst to be fully flushed. All new write transfers are put on hold.
} eeprom_t;


static void bsp_24xx1025_check_pending_write(eeprom_t *eeprom);


int pi_24xx1025_open(struct pi_device *device)
{
    struct pi_24xx1025_conf *conf = (struct pi_24xx1025_conf *)device->config;

    eeprom_t *eeprom = pi_l2_malloc(sizeof(eeprom_t));
    if (eeprom == NULL)
        return -1;

    device->data = (void *)eeprom;
    device->api = (struct pi_device_api *)conf->eeprom.api;

    struct pi_i2c_conf i2c_conf;
    pi_i2c_conf_init(&i2c_conf);
    i2c_conf.cs = conf->i2c_addr;
    i2c_conf.itf = conf->i2c_itf;
    pi_open_from_conf(&eeprom->i2c_device, &i2c_conf);

    if (pi_i2c_open(&eeprom->i2c_device))
        goto error;

    eeprom->pending_task = NULL;
    eeprom->waiting_first = NULL;
    eeprom->waiting_page_flush = 0;

    return 0;

error:
    pi_l2_free(eeprom, sizeof(eeprom_t));
    return -1;
}


static void pi_24xx1025_close(struct pi_device *device)
{
    eeprom_t *eeprom = device->data;

    pi_i2c_close(&eeprom->i2c_device);

    pi_l2_free(eeprom, sizeof(eeprom_t));
}


// This is called when a write flush is finished to re-allow write transfers
static void bsp_24xx1025_check_end_of_flush(void *arg)
{
    eeprom_t *eeprom = arg;

    // This will re-allow write transfers
    eeprom->waiting_page_flush = 0;

    // Check if we need to enqueue a burst as we may have received a write copy while
    // we were waiting for the write flush
    if (eeprom->pending_write_size != 0)
    {
        bsp_24xx1025_check_pending_write(eeprom);
    }
}


// This is called when a write transfer is done, to maybe notify the user, and prepare a callback when the flush is done
static void bsp_24xx1025_check_end_of_write(void *arg)
{
    eeprom_t *eeprom = arg;

    // In case we reached the end of the write transfer, notify now the user to allow him enqueueing immediately
    // a read transfer, but be careful to remember a flush is still pending to put on hold any incoming write transfer
    if (eeprom->pending_write_size == 0)
    {
        eeprom->waiting_page_flush = 1;
        pi_task_push(eeprom->pending_task);
        eeprom->pending_task = NULL;
    }

    // Register a delayed callback to re-allow write transfers
    pi_task_push_delayed_us(pi_task_callback(&eeprom->internal_task, bsp_24xx1025_check_end_of_flush, (void *)eeprom), CONFIG_24XX1025_FLUSH_DELAY_US);
}


// This is called to check if a pending write can be resumed by pushing one more burst
static void bsp_24xx1025_check_pending_write(eeprom_t *eeprom)
{
    // Since we unblock the user before the write is fully flushed to allow reading immediately after the burst is written,
    // it can happen the user asked for another write while the previous one has not been flushed yet.
    // In this case, just drop it, it will be resumed when the flush is done.
    if (!eeprom->waiting_page_flush)
    {
        // Now send a burst from the pending copy.

        // Be careful to not cross a page during the write.
        int iter_size = CONFIG_24XX1025_BURST_SIZE - (eeprom->pending_write_addr & (CONFIG_24XX1025_BURST_SIZE - 1));

        if (iter_size > eeprom->pending_write_size)
            iter_size = eeprom->pending_write_size;

        // The address is store big-endian, swap it now
        uint32_t addr = (eeprom->pending_write_addr << 8) | (eeprom->pending_write_addr >> 8);
        uint32_t data = eeprom->pending_write_data;

        // Be careful to update the state before calling the asynchronous function, since on multi-threaded systems
        // we can be preempted right after to execute the callback.
        eeprom->pending_write_addr += iter_size;
        eeprom->pending_write_data += iter_size;
        eeprom->pending_write_size -= iter_size;

        // Forward the burst to the I2C driver
        pi_i2c_write_dual_async(&eeprom->i2c_device, &addr, (void *)data, 2, iter_size, pi_task_callback(&eeprom->internal_task, bsp_24xx1025_check_end_of_write, (void *)eeprom));
    }
}


static void bsp_24xx1025_enqueue_copy(eeprom_t *eeprom, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task, int is_write)
{
    // The I2C driver can support several pending copies but we have to put on hold incoming copies if a write copy
    // is pending to keep the copies in order
    if (!eeprom->pending_task)
    {
        if (is_write)
        {
            // Write copies need to be cut into bursts.
            // Remember the user task to prevent other copies to be handled before and
            // push the user task when the full transfer is done.
            eeprom->pending_task = task;
            eeprom->pending_write_addr = eeprom_addr;
            eeprom->pending_write_size = size;
            eeprom->pending_write_data = (uint32_t)data;

            // Now proceed with one step
            bsp_24xx1025_check_pending_write(eeprom);
        }
        else
        {
            // Simple read case, no need to register this copy as pending, the I2C driver will take care
            uint32_t addr = (eeprom_addr << 8) | (eeprom_addr >> 8);
            pi_i2c_write_read_async(&eeprom->i2c_device, &addr, data, 2, size, task);
        }
    }
    else
    {
        // Save the copy to restore it later on
        task->data[0] = (int)eeprom;
        task->data[1] = (int)data;
        task->data[2] = size;
        task->data[3] = is_write;

        if (eeprom->waiting_first)
            eeprom->waiting_last->next = task;
        else
            eeprom->waiting_first = task;

        eeprom->waiting_last = task;
        task->next = NULL;
    }
}


static void pi_24xx1025_read_async(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task)
{
    bsp_24xx1025_enqueue_copy(device->data, eeprom_addr, data, size, task, 0);
}


static void pi_24xx1025_write_async(struct pi_device *device, uint32_t eeprom_addr, void *data, uint32_t size, pi_task_t *task)
{
    bsp_24xx1025_enqueue_copy(device->data, eeprom_addr, data, size, task, 1);
}


static pi_eeprom_api_t pi_24xx1025_api =
{
  .open                 = &pi_24xx1025_open,
  .close                = &pi_24xx1025_close,
  .read_async           = &pi_24xx1025_read_async,
  .write_async          = &pi_24xx1025_write_async
};


void pi_24xx1025_conf_init(struct pi_24xx1025_conf *conf)
{
  conf->eeprom.api = &pi_24xx1025_api;
  bsp_24xx1025_conf_init(conf);
}
