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

#ifndef __GAP_HYPERBUS_H__
#define __GAP_HYPERBUS_H__


#include "gap_udma.h"

/*!
 * @addtogroup hyperbus
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Hyperbus csn0 for RAM, csn1 for FLASH or inverse. */
#define  __HYPERBUS_CSN0_FOR_RAM__  1

/*! @brief Hyperbus transfer request type definition.*/
#define  hyperbus_req_t udma_req_t

/*! @brief Hyperbus address size, everyone address for two bytes.*/
#define  HYPERBUS_RAM_SIZE     (0x00400000)

/*! @brief Hyperbus bus RAM register Space.*/
enum _hyperbus_mode
{
    uHYPERBUS_OCTOSPI_MODE  = 0x00U,  /*!< Support normal OCTO SPI, including PSRAM, OCTO SPI, Single SPI */
    uHYPERBUS_HYPERBUS_MODE = 0x01U,  /*!< Support for HYPERBUS RAM and flash */
};

/*! @brief Hyperbus bus device address.*/
enum _hyperbus_device_address
{
#if (__HYPERBUS_CSN0_FOR_RAM__ == 1)
    uHYPERBUS_Ram_Address   = 0x0U,        /*!< RAM base address. RAM sapce is 8M bytes*/
                                        /*!< Eah address has two bytes, total 4M addresses*/
    uHYPERBUS_Flash_Address = 0x01000000U  /*!< Flash base address = 16M. */
#else
    uHYPERBUS_Ram_Address   = 0x20000000U,        /*!< RAM base address. RAM sapce is 8M bytes*/
                                        /*!< Eah address has two bytes, total 4M addresses*/
    uHYPERBUS_Flash_Address = 0x0  /*!< Flash base address = 16M. */
#endif
};

/*! @brief Hyperbus bus RAM register Space.*/
enum _hyperbus_ram_register_address
{
    uHYPERBUS_ID_REG0_Addr  = 0x0000U,  /*!< RAM Identification Register 0 address */
    uHYPERBUS_ID_REG1_Addr  = 0x0002U,  /*!< RAM Identification Register 1 address */
    uHYPERBUS_CFG_REG0_Addr = 0x1000U,  /*!< RAM Configuration register 0 address */
    uHYPERBUS_CFG_REG1_Addr = 0x1002U   /*!< RAM Configuration register 1 address */
};

/*! @brief Hyperbus bus access type - Memory or Register.*/
enum _hyperbus_access_type
{
    uHYPERBUS_Mem_Access = 0x0U,  /*!< Access to memory transfer. */
    uHYPERBUS_Reg_Access = 0x1U   /*!< Access to register transfer. */
};

/*! @brief Hyperbus bus device type - Flash or RAM.*/
enum _hyperbus_device_type
{
    uHYPERBUS_Ram   = 0x0U,   /*!< Access to ram transfer. */
    uHYPERBUS_Flash = 0x1U    /*!< Access to flash transfer. */
};

/*! @brief Hyperbus bus acesse type - Read or Write.*/
enum _hyperbus_rdwr_type
{
    uHYPERBUS_Read = 0x0U,  /*!< Read transfer. */
    uHYPERBUS_Write = 0x1U  /*!< Write transfer. */
};

/*! @brief HYPERBUS transfer status.*/
enum _hyperbus_transfer_state
{
    uHYPERBUS_Idle = 0x0U, /*!< Nothing in the transmitter/receiver. */
    uHYPERBUS_Busy,        /*!< Transfer queue is not finished. */
    uHYPERBUS_Error        /*!< Transfer error. */
};

/*! @brief HYPERBUS configuration structure.*/
typedef struct _hyperbus_config
{
    uint8_t   mode;/*!< Operation mode hyperbus or normal OCTO SPI */
    uint32_t  baudRate;/*!< The baud rate of Tx and Rx. */
    uint32_t  mbr0;    /*!< Memory base address 0. */
    uint32_t  mbr1;    /*!< Memory base address 1. */
    uint8_t   dt0;     /*!< Device type 0. */
    uint8_t   dt1;     /*!< Device type 1. */
} hyperbus_config_t;

/*! @brief HYPERBUS transfer structure.*/
typedef struct _hyperbus_transfer
{
    uint16_t *txData;            /*!< Send buffer. */
    uint16_t *rxData;            /*!< Receive buffer. */
    volatile size_t txDataSize; /*!< TX Transfer bytes. */
    volatile size_t rxDataSize; /*!< RX Transfer bytes. */
    uint32_t addr;               /*!< Address. */
    uint8_t device;             /*!< Device type. */
    uint8_t reg_access;         /*!< Memory type, register or memory. */
    uint32_t configFlags;       /*!< Transfer transfer configuration flags.*/
} hyperbus_transfer_t;

/*!
* @brief Forward declaration of the _hyperbus_handle typedefs.
*/
typedef struct _hyperbus_handle hyperbus_handle_t;

/*!
 * @brief Completion callback function pointer type.
 *
 * @param base HYPERBUS peripheral address.
 * @param handle Pointer to the handle for the HYPERBUS.
 * @param status Success or error code describing whether the transfer completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*hyperbus_transfer_callback_t)(HYPERBUS_Type *base,
                                                hyperbus_handle_t *handle,
                                                status_t status,
                                                void *userData);



/*! @brief HYPERBUS transfer handle structure used for transactional API. */
struct _hyperbus_handle
{
    uint8_t *volatile txData;                  /*!< Send buffer. */
    uint8_t *volatile rxData;                  /*!< Receive buffer. */
    size_t totalByteCount;                     /*!< A number of transfer bytes*/

    volatile uint8_t state; /*!< HYPERBUS transfer state, see _hyperbus_transfer_state.*/

    hyperbus_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                           /*!< Callback user data. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to hyperbus bases for each instance. */
static HYPERBUS_Type *const s_hyperbusBases[] = HYPERBUS_BASE_PTRS;

extern uint8_t hyperbus_is_init;

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */
/*!
 * @brief Initializes the HYPERBUS.
 *
 * This function initializes the HYPERBUS configuration. This is an example use case.
 *  @code
 *   hyperbus_config_t  config;
 *   config.baudRate                 = 50000000U;
 *   config.mbr0                     = uHYPERBUS_Ram_Address;
 *   config.mbr1                     = uHYPERBUS_Flash_Address >> 24;
 *   config.dt0                      = uHYPERBUS_Ram;
 *   config.dt1                      = uHYPERBUS_Flash;
 *   HYPERBUS_Init(base, &config, srcClock_Hz);
 *  @endcode
 *
 * @param base HYPERBUS peripheral address.
 * @param config Pointer to the structure hyperbus_config_t.
 * @param srcClock_Hz Module source input clock in Hertz.
 */
void HYPERBUS_Init(HYPERBUS_Type *base, hyperbus_config_t *config, uint32_t srcClock_Hz);

/*!
 * @brief Sets the hyperbus_config_t structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the HYPERBUS_Init().
 * Users may use the initialized structure unchanged in the HYPERBUS_Init() or modify the structure
 * before calling the HYPERBUS_Init().
 * Example:
 * @code
 *  hyperbus_config_t  config;
 *  HYPERBUS_GetDefaultConfig(&config);
 * @endcode
 * @param config pointer to hyperbus_config_t structure
 */
void HYPERBUS_GetDefaultConfig(hyperbus_config_t *config);

/*!
 * @brief Sets the hyperbus frequecny.
 *
 * Example:
 * @code
 *  hyperbus_config_t  config;
 *  HYPERBUS_HYPERBUS_FrequencyConfig(base, config->baudrate, srcClock);
 * @endcode
 * @param base HYPERBUS peripheral address.
 * @param config pointer to hyperbus_config_t structure
 */
void HYPERBUS_FrequencyConfig(HYPERBUS_Type *base, uint32_t baudRate, uint32_t srcClock_Hz);

/*!
 * @brief De-initializes the HYPERBUS peripheral, Clock Gating. Call this API to disable the HYPERBUS clock.
 * @param base HYPERBUS peripheral address.
 */
void HYPERBUS_DeInit(HYPERBUS_Type *base);

/*!
 * @brief Hyperbus Bus Clear and Stop
 *
 * @param  base     HYPERBUS peripheral address.
 *
 */
static inline void HYPERBUS_ClearAndStop(HYPERBUS_Type *base)
{
    UDMA_RXClearAndStop((UDMA_Type*)base);
    UDMA_TXClearAndStop((UDMA_Type*)base);
}
/*!
 *@}
*/

/*!
 * @name Configuration
 * @{
 */

/*!
 * @brief Set Hyperbus Bus access address.
 *
 * This function sets write read address for both ram and flash
 * Only address[31:24] is valid
 * mbr1 <= mbr0,        always chip select csn0
 * mbr0 <= addr < mbr1,        chip select csn0
 * mbr1 <= addr,               chip select csn1
 *
 * @param base    HYPERBUS peripheral address.
 * @param addr    The access address of read and write
 *
 */
static inline void HYPERBUS_SetAddr(HYPERBUS_Type *base, int32_t addr)
{
    base->EXT_ADDR = addr;
}

/*!
 * @brief Set Hyperbus Bus device type
 *
 * set hyperbus bus device type (dt)
 * @param base     HYPERBUS peripheral address.
 * @param device   The device type
 * @param dt0      If hyperbus, select RAM or FLASH for channel 0
 * @param dt1      If hyperbus, select RAM or FLASH for channel 1
 *
 */
static inline void HYPERBUS_SetDevice(HYPERBUS_Type *base, uint32_t device, int dt0, int dt1)
{
    base->DEVICE_TYPE = HYPERBUS_DEVICE_DT1(dt1) | HYPERBUS_DEVICE_DT0(dt0) | HYPERBUS_DEVICE_TYPE(device);
}

/*!
 * @brief Set Hyperbus Bus channel 0 base address
 *
 * set hyperbus bus channel 0 base address (mbr0)
 * @param base    HYPERBUS peripheral address.
 * @param mbr0    The mbr0 address
 *
 */
static inline void HYPERBUS_SetMBR0(HYPERBUS_Type *base, uint32_t mbr0)
{
    base->MBR0 = mbr0;
}

/*!
 * @brief Set Hyperbus Bus channel 1 base address
 *
 * set hyperbus bus channel 1 base address (mbr1)
 * @param base    HYPERBUS peripheral address.
 * @param mbr1    The mbr1 address
 *
 */
static inline void HYPERBUS_SetMBR1(HYPERBUS_Type *base, uint32_t mbr1)
{
    base->MBR1 = mbr1;
}

/*!
 * @brief Set Hyperbus timing
 *
 */
/*!
 * @brief Configure the HYPERBUS memrory timing
 *
 * Set the latency cycles and timing performence for particular clock frequency
 * @param  base     HYPERBUS peripheral address.
 * @param  latency0  The value of latency cycle for chip select 0
 * @param  latency1  The value of latency cycle for chip select 1
 * @param  recovery The value of recovery cycle for Hyperbus
 * @param  rwds_delay The value of rwds delay
 * @param  latency_autocheck_en  The value of additional latency cycle for HyperbusRAM
 * @param  csm         The type of chip select maximum cycle for HyperbusRAM
 */
static inline void HYPERBUS_SetTiming(HYPERBUS_Type *base, int latency0, int latency1, int recovery, int rwds_delay, int latency_autocheck_en , int csm)
{
    base->TIMING_CFG = HYPERBUS_TIMING_CFG_LATENCY0(latency0)                     |
        HYPERBUS_TIMING_CFG_LATENCY1(latency1)                                    |
        HYPERBUS_TIMING_CFG_RW_RECOVERY(recovery)                                 |
        HYPERBUS_TIMING_CFG_RWDS_DELAY(rwds_delay)                                |
        HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(latency_autocheck_en) |
        HYPERBUS_TIMING_CFG_CSM(csm);
}

/* @} */

/*!
 * @name Transactional
 * @{
 */
/*Transactional APIs*/

/*!
 * @brief Write or Read data into/from the data buffer and waits till complete to return.
 *
 * @param base HYPERBUS peripheral address.
 * @param transfer The pointer to hyperbus_transfer_t structure.
 * @return status of status_t.
 */
status_t HYPERBUS_TransferBlocking(HYPERBUS_Type *base, hyperbus_transfer_t *transfer);


/*!
 * @brief HYPERBUS transfer data using interrupts.
 *
 * This function transfers data using interrupts. This is a non-blocking function, which returns right away. When all
 * data is transferred, the callback function is called.

 * @param base HYPERBUS peripheral base address.
 * @param handle Pointer to the hyperbus_handle_t structure which stores the transfer state.
 * @param transfer Pointer to the hyperbus_transfer_t structure.
 * @return status of status_t.
 */
status_t HYPERBUS_TransferNonBlocking(HYPERBUS_Type *base, hyperbus_handle_t *handle, hyperbus_transfer_t *transfer);

/*!
 * @brief HYPERBUS IRQ Enable.
 *
 * This function enable the HYPERBUS transmit and receive IRQ.

 * @param base HYPERBUS peripheral base address.
 * @param enable Enable the transmission end IRQ.
 */
static inline void HYPERBUS_TransferIRQEnable(HYPERBUS_Type *base, int enable)
{
    base->IRQ_EN = enable;
}

/*!
 * @brief HYPERBUS IRQ handler function.
 *
 * This function processes the HYPERBUS transmit and receive IRQ.

 * @param base HYPERBUS peripheral base address.
 * @param handle Pointer to the hyperbus_handle_t structure which stores the transfer state.
 */
void HYPERBUS_TransferHandleIRQ(HYPERBUS_Type *base, hyperbus_handle_t *handle);


/*!
 * @brief Initializes the HYPERBUS handle.
 *
 * This function initializes the HYPERBUS handle, which can be used for other HYPERBUS transactional APIs.  Usually, for a
 * specified HYPERBUS instance,  call this API once to get the initialized handle.
 *
 * @param base HYPERBUS peripheral base address.
 * @param handle HYPERBUS handle pointer to hyperbus_handle_t.
 * @param callback HYPERBUS callback.
 * @param userData Callback function parameter.
 */
void HYPERBUS_TransferCreateHandle(HYPERBUS_Type *base,
                                   hyperbus_handle_t *handle,
                                   hyperbus_transfer_callback_t callback,
                                   void *userData);

/*!
 *@}
*/

/*!
 * @name Busy state
 * @{
 */

/*!
 * @brief Get HYPERBUS TX channel busy state.
 *
 * @param base The HYPERBUS channel.
 * @note .
 */
static inline int HYPERBUS_TXBusy(HYPERBUS_Type *base)
{
    return UDMA_TXBusy((UDMA_Type*)base);
}

/*!
 * @brief Get HYPERBUS RX channel busy state.
 *
 * @param base The HYPERBUS channel.
 * @note .
 */
static inline int HYPERBUS_RXBusy(HYPERBUS_Type *base)
{
    return UDMA_RXBusy((UDMA_Type*)base);
}

/*!
 * @brief Get HYPERBUS TX channel pending state.
 *
 * @param base The HYPERBUS channel.
 * @note .
 */
static inline int HYPERBUS_TXPending(HYPERBUS_Type *base)
{
    return UDMA_TXPending((UDMA_Type*)base);
}

/*!
 * @brief Get HYPERBUS RX channel pending state.
 *
 * @param base The HYPERBUS channel.
 * @note .
 */
static inline int HYPERBUS_RXPending(HYPERBUS_Type *base)
{
    return UDMA_RXPending((UDMA_Type*)base);
}

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /*_GAP_HYPERBUS_H_*/
