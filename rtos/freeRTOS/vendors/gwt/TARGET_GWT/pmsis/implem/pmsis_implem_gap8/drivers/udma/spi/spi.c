/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include "pmsis.h"
#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "pmsis/implem/drivers/udma/spi/spi_internal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef DEBUG
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(...) ((void) 0)
#endif  /* DEBUG */

/*******************************************************************************
 * API implementation
 ******************************************************************************/

int pi_spi_open(struct pi_device *device)
{
    struct pi_spi_conf *conf = (struct pi_spi_conf *) device->config;
    int irq = __disable_irq();
    int status = __pi_spi_open((struct spim_cs_data**)(&device->data), conf);
    restore_irq(irq);
    return status;
}

void pi_spi_close(struct pi_device *device)
{
    int irq = __disable_irq();
    __pi_spi_close(device->data);
    restore_irq(irq);
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
    // TODO
}

void pi_spi_send(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_send_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_send_async(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_send_async(device->data, data, len, flag, task);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flag)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_receive_async(device, data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_receive_async(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flag, pi_task_t *task)
{
    __pi_spi_receive_async(device->data, data, len, flag, task);
}

void pi_spi_receive_with_ucode(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size, void *ucode)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    __pi_spi_receive_async_with_ucode(device->data, data, len, flags,
            ucode_size, ucode, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

void pi_spi_send_with_ucode(struct pi_device *device, void *data, size_t len,
        pi_spi_flags_e flags, int ucode_size, void *ucode)
{
    pi_task_t task_block;
    pi_task_block(&task_block);
    __pi_spi_send_async_with_ucode(device->data, data, len, flags,
            ucode_size, ucode, &task_block);
    pi_task_wait_on(&task_block);
    pi_task_destroy(&task_block);
}

uint32_t pi_spi_get_config(struct pi_device *device)
{
    return __pi_spi_get_config(device->data);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data,
        size_t len, pi_spi_flags_e flag)
{
    // TODO
    pi_task_t task_block;
    pi_task_block(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_spi_transfer_async(device, tx_data, rx_data, len, flag, &task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_wait_on(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
    pi_task_destroy(&task_block);
    DEBUG_PRINTF("%s:%d\n",__func__,__LINE__);
}

void pi_spi_transfer_async(struct pi_device *device, void *tx_data,
        void *rx_data, size_t len, pi_spi_flags_e flag, pi_task_t *task)
{
    // TODO
    __pi_spi_xfer_async(device->data, tx_data, rx_data, len, flag, task);
}


#define SPI_UCODE_CMD_SEND_CMD(cmd,bits,qpi)    ((2<<28) | ((qpi)<<27) | (((bits)-1)<<16) | (((cmd)>>8)<<0) | (((cmd)&0xff)<<(0+8)))
#define SPI_UCODE_CMD_SEND_ADDR(bits,qpi)       ((3<<28) | ((qpi)<<27) | (((bits)-1)<<16))
#define SPI_UCODE_CMD_DUMMY(cycles)             ((4<<28) | (((cycles)-1)<<16))

void *pi_spi_receive_ucode_set(struct pi_device *device, uint8_t *ucode,
        uint32_t ucode_size)
{
    return __pi_spi_receive_ucode_set(device->data, ucode, ucode_size);
}

void pi_spi_receive_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode,
        uint32_t ucode_size)
{
    __pi_spi_receive_ucode_set_addr_info(device->data, ucode, ucode_size);
}

void *pi_spi_send_ucode_set(struct pi_device *device, uint8_t *ucode,
        uint32_t ucode_size)
{
    return __pi_spi_send_ucode_set(device->data, ucode, ucode_size);
}

void pi_spi_send_ucode_set_addr_info(struct pi_device *device, uint8_t *ucode,
        uint32_t ucode_size)
{
    __pi_spi_send_ucode_set_addr_info(device->data, ucode, ucode_size);
}

void pi_spi_copy(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_task_block(&task);
    __pi_spi_copy_async(device->data, addr, data, size*8, flags, &task);
    pi_task_wait_on(&task);
    pi_task_destroy(&task);
}

void pi_spi_copy_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size,
  pi_spi_flags_e flags, pi_task_t *task)
{
    __pi_spi_copy_async(device->data, addr, data, size*8, flags, task);
}

void pi_spi_copy_2d(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags)
{
    pi_task_t task;
    pi_task_block(&task);
    __pi_spi_copy_async_2d(device->data, addr, data, size*8, stride, length,
            flags, &task);
    pi_task_wait_on(&task);
    pi_task_destroy(&task);
}

void pi_spi_copy_2d_async(struct pi_device *device,
  uint32_t addr, void *data, uint32_t size, uint32_t stride,
  uint32_t length, pi_spi_flags_e flags, pi_task_t *task)
{
    __pi_spi_copy_async_2d(device->data, addr, data, size*8, stride, length,
            flags, task);
    return;
}


