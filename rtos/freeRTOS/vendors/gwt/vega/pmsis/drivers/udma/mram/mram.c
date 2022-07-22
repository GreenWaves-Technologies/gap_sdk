/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
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

#include "pmsis/implem/drivers/udma/mram/mram_internal.h"
#include "pmsis.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MRAM_PORb     (1 << 7)
#define MRAM_RETb     (1 << 6)
#define MRAM_RSTb     (1 << 5)
#define MRAM_NVR      (1 << 4)
#define MRAM_TMEN     (1 << 3)
#define MRAM_AREF     (1 << 2)
#define MRAM_DPD      (1 << 1)
#define MRAM_ECCBYPS  (1 << 0)

// TODO: CHECK
#define MRAM_BASE_ADDRESS (0)


pi_mram_drv_data_t *g_mram_drv_data[1]= {0};

/*! @brief mram transfer state. */
enum _mram_transfer_states
{
    uMRAM_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uMRAM_Busy,        /*!< Transfer queue is not finished. */
    uMRAM_Error        /*!< Transfer error. */
};

/*! @brief Typedef for interrupt handler. */
typedef void (*mram_isr_t)(MRAM_Type *base, mram_handle_t *handle);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get instance number for MRAM module.
 *
 * @param base MRAM peripheral base address.
 */
uint32_t MRAM_GetInstance(MRAM_Type *base);


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to mram bases for each instance. */

/*! @brief Pointers to mram handles for each instance. */

/*! @brief Pointers to mram IRQ number for each instance. */
/* static const IRQn_Type s_mramIrqs[] = MRAM_IRQS; */

/*! @brief Pointer to  IRQ handler for each instance. */

/*******************************************************************************
 * Code
 ******************************************************************************/

void __pi_mram_handler(void *arg)
{
    //printf("HANDLER\n");
    pi_mram_drv_data_t *drv_data = g_mram_drv_data[0];
    if(drv_data->pending_task)
    {
    //printf("HANDLER RELEASE 0x%x\n",drv_data->pending_task);
        pi_task_release(drv_data->pending_task);
    }
}

void MRAM_Init(MRAM_Type *base, const pi_mram_config_t *Config, uint32_t srcClock_Hz)
{
    /* Enable MRAM UDMA clock. */
    udma_init_device(UDMA_MRAM_ID(Config->itf));

    /* Configure baud rate. */
    MRAM_SetFrequency(base, Config->frequency, srcClock_Hz);

    /* Enable erase IRQ */
    base->IER = 0xF;
}

void pi_mram_open(pi_device_t *device)
{
    int irq = disable_irq();
    pi_mram_config_t *config = device->config;

    if(!g_mram_drv_data[0])
    {
        pi_mram_drv_data_t *mram = pi_malloc(sizeof(pi_mram_drv_data_t));
        mram->base = UDMA_MRAM(0);
        mram->pending_task = NULL;
		g_mram_drv_data[0] = mram;
        device->data = mram;
		MRAM_Init(mram->base,config,pi_freq_get(FLL_ID_PERIPH));
        // enable events
        pi_fc_event_handler_set(SOC_EVENT_UDMA_MRAM_ERASE(config->itf), __pi_mram_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_MRAM_ERASE(config->itf));
        pi_fc_event_handler_set(SOC_EVENT_UDMA_MRAM_ERASE(config->itf+1), __pi_mram_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_MRAM_ERASE(config->itf)+1);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_MRAM_ERASE(config->itf)+2, __pi_mram_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_MRAM_ERASE(config->itf)+2);
        pi_fc_event_handler_set(SOC_EVENT_UDMA_MRAM_ERASE(config->itf)+3, __pi_mram_handler);
        hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_MRAM_ERASE(config->itf)+3);
        restore_irq(irq);
        hal_compiler_barrier();
        MRAM_PowerUp(UDMA_MRAM(0));
    }
	else
	{
		device->data = g_mram_drv_data[0];
        restore_irq(irq);
	}
}

void MRAM_Deinit(MRAM_Type *base)
{
    /* Power Down */
    pi_pmu_mram_poweroff();

    /* Disable all IRQ */
    base->IER = 0;

    base->CLK_DIV = MRAM_CLK_DIV_EN(0);

    /* Disable MRAM UDMA clock. */
    udma_deinit_device(base);
}

void pi_mram_close(pi_device_t *device)
{    
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    MRAM_Deinit(base);
}

void pi_mram_conf_init(pi_mram_config_t *config)
{
    /* Default frequency at 25MHz. */
    config->frequency  = 25000000;
    config->itf = 0;
}

void MRAM_SetFrequency(MRAM_Type *base, uint32_t frequency, uint32_t srcClock_Hz)
{
    int index = 0;

    uint32_t clkDivider = (srcClock_Hz >> 1) / frequency;

    base->CLK_DIV = MRAM_CLK_DIV(clkDivider) | MRAM_CLK_DIV_EN(1);
}

void MRAM_PowerUp(MRAM_Type *base)
{
    /* Power Up */
    pi_pmu_mram_poweron();

    for(volatile int i = 0; i < 5000; i++);
    base->MODE = MRAM_MODE(MRAM_CMD_PWUP, MRAM_PORb | MRAM_RETb);

    /* tRSW >= 5us  */
    for(volatile int i = 0; i < 5000; i++);

    base->MODE = MRAM_MODE(MRAM_CMD_PWUP, MRAM_PORb | MRAM_RETb | MRAM_RSTb);
    for(volatile int i = 0; i < 2000; i++);
}

int pi_mram_chip_erase(pi_device_t *device)
{
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    base->ERASE_ADDR = (MRAM_BASE_ADDRESS >> 3);
    base->ERASE_SIZE = 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_CHIP, mode);

    base->TRIGGER = 1;

    //MRAM_Wait(base, MRAM_ISR_ERASE_DONE_MASK);

    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    restore_irq(irq);
    pi_task_wait_on(&task_block);
    return 0;
}

int pi_mram_sector_erase(pi_device_t *device, uint32_t address, uint32_t sector_size)
{    
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;

    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    base->ERASE_ADDR = (address >> 3);
    base->ERASE_SIZE = sector_size - 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_SECTOR, mode);

    base->TRIGGER = 1;

 //   MRAM_Wait(base, MRAM_ISR_ERASE_DONE_MASK);
    restore_irq(irq);
    pi_task_wait_on(&task_block);
    return 0;
}

int pi_mram_word_erase(pi_device_t *device, uint32_t address, uint32_t length)
{
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    base->ERASE_ADDR = (address >> 3);
    base->ERASE_SIZE = length - 1;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_ERASE_WORD, mode);

    base->TRIGGER = 1;

    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    restore_irq(irq);
    pi_task_wait_on(&task_block);

    return 0;
}

int pi_mram_reference_line_config(pi_device_t *device, uint32_t ref_line, int aref, int tmen)
{
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    volatile int mode = base->MODE & 0xFF;

    int irq = __disable_irq();
    if (aref && tmen) {

        mode |= MRAM_AREF | MRAM_TMEN;

        /* Setting  AREF and TMEN to 1 */
        if (ref_line) {
            /* Setting AREF=1, TMEN=1 and MODE to REF LINE 1 */
            base->MODE = MRAM_MODE(MRAM_CMD_REF_LINE_P, mode);
        } else {
            /* Setting AREF=1, TMEN=1 and MODE to REF LINE 0 */
            base->MODE = MRAM_MODE(MRAM_CMD_REF_LINE_AP, mode);
        }
        base->TRIGGER = 1;

        pi_task_t task_block;
        pi_task_block(&task_block);
        drv_data->pending_task = &task_block;
        restore_irq(irq);
        pi_task_wait_on(&task_block);
    } else {
        /* Clear mode flag */
        mode &=  ~(MRAM_AREF | MRAM_TMEN);

        base->MODE = mode & 0xFF;
    }
    restore_irq(irq);
    return 0;
}

int pi_mram_trim_config(pi_device_t *device, uint32_t address, const char *data, int length)
{    
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;

    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_TRIM_CFG, mode);

    /* Destination Address*/
    base->TX_DADDR = (address >> 3);

    //int status = MRAM_TransferBlocking(base, data, length, 0, 0);
    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    hal_udma_enqueue_channel((udma_core_cmd_t*)base,(uint32_t)data,length,UDMA_CORE_RX_CFG_EN(1),TX_CHANNEL);
    restore_irq(irq);
    pi_task_wait_on(&task_block);
    return 0;
}

int pi_mram_program(pi_device_t *device, uint32_t address, const char *data, int length, int ecc_byp)
{
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_NORMAL_TX, mode | (ecc_byp ? MRAM_ECCBYPS : 0));

    /* Destination Address*/
    base->TX_DADDR = (address >> 3);

    //int status = MRAM_TransferBlocking(base, data, length, 0, 0);
    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    hal_udma_enqueue_channel((udma_core_cmd_t*)base,(uint32_t)data,length,UDMA_CORE_RX_CFG_EN(1),TX_CHANNEL);
    restore_irq(irq);
    pi_task_wait_on(&task_block);
    return 0;
}

int pi_mram_read(pi_device_t *device, uint32_t address, char *data, int length, int ecc_byp)
{
    int irq = __disable_irq();
    pi_mram_drv_data_t *drv_data = (pi_mram_drv_data_t *)device->data;
    MRAM_Type *base = drv_data->base;
    volatile int mode = base->MODE & 0xFF;
    base->MODE = MRAM_MODE(MRAM_CMD_READ_RX, mode | (ecc_byp ? MRAM_ECCBYPS : 0));

    /* Destination Address*/
    base->RX_DADDR = (address >> 3);

    pi_task_t task_block;
    pi_task_block(&task_block);
    drv_data->pending_task = &task_block;
    hal_udma_enqueue_channel((udma_core_cmd_t*)base,(uint32_t)data,length,UDMA_CORE_RX_CFG_EN(1),RX_CHANNEL);
    restore_irq(irq);
    pi_task_wait_on(&task_block);
}
