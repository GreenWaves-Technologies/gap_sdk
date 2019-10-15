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
    uHYPERBUS_Flash = 0x0U,  /*!< Access to flash transfer. */
    uHYPERBUS_Ram   = 0x1U   /*!< Access to ram transfer. */
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
 * @name Configuration Register Access
 * @{
 */
/*!
 * @brief Set Hyperbus external address.
 *
 * This function set the hyperbus bus access address for both read and write.
 *
 * @param value   Access address
 */
static inline void HYPERBUS_EXT_SetAddr(uint32_t value)
{
    s_hyperbusBases[0]->EXT_ADDR = value;
}

/*!
 * @brief Get Hyperbus external address.
 *
 * Get Hyperbus IP configuration ext_addr value
 *
 * @return status of status_t.
 */
static inline uint32_t HYPERBUS_EXT_GetAddr()
{
    return (s_hyperbusBases[0]->EXT_ADDR);
}

/*!
 * @brief Set configuration register 0.
 *
 * set Hyperbus IP configuration register 0
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG0(uint16_t value)
{
    s_hyperbusBases[0]->MEM_CFG0 = value;
}

/*!
 * @brief Get configuration register 0.
 *
 * Get Hyperbus IP configuration register 0
 *
 * @return register value.
 */
static inline uint16_t HYPERBUS_MEM_GetCFG0()
{
    return ((uint16_t) s_hyperbusBases[0]->MEM_CFG0);
}

/*!
 * @brief Set configuration register 1.
 *
 * set Hyperbus IP configuration register 1
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG1(uint32_t value)
{
    s_hyperbusBases[0]->MEM_CFG1 = value;
}

/*!
 * @brief Get configuration register 1.
 *
 * Get Hyperbus IP configuration register 1
 *
 * @return register value.
 */
static inline uint32_t HYPERBUS_MEM_GetCFG1()
{
    return ((uint32_t) s_hyperbusBases[0]->MEM_CFG1);
}

/*!
 * @brief Set configuration register 2.
 *
 * set Hyperbus IP configuration register 2
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG2(uint32_t value)
{
    s_hyperbusBases[0]->MEM_CFG2 = value;
}

/*!
 * @brief Get configuration register 2.
 *
 * Get Hyperbus IP configuration register 2
 *
 * @return register value.
 */
static inline uint32_t HYPERBUS_MEM_GetCFG2()
{
    return ((uint32_t) s_hyperbusBases[0]->MEM_CFG2);
}

/*!
 * @brief Set configuration register 3.
 *
 * set Hyperbus IP configuration register 3
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG3(uint16_t value)
{
    s_hyperbusBases[0]->MEM_CFG3 = value;
}

/*!
 * @brief Get configuration register 3.
 *
 * Get Hyperbus IP configuration register 3
 *
 * @return register value.
 */
static inline uint16_t HYPERBUS_MEM_GetCFG3()
{
    return ((uint16_t) s_hyperbusBases[0]->MEM_CFG3);
}

/*!
 * @brief Set configuration register 4.
 *
 * set Hyperbus IP configuration register 4
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG4(uint16_t value)
{
    s_hyperbusBases[0]->MEM_CFG4 = value;
}

/*!
 * @brief Get configuration register 4.
 *
 * Get Hyperbus IP configuration register 4
 *
 * @return register value.
 */
static inline uint16_t HYPERBUS_MEM_GetCFG4()
{
    return ((uint16_t) s_hyperbusBases[0]->MEM_CFG4);
}

/*!
 * @brief Set configuration register 5.
 *
 * set Hyperbus IP configuration register 5
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG5(uint32_t value)
{
    s_hyperbusBases[0]->MEM_CFG5 = value;
}

/*!
 * @brief Get configuration register 5.
 *
 * Get Hyperbus IP configuration register 5
 *
 * @return register value.
 */
static inline uint32_t HYPERBUS_MEM_GetCFG5()
{
    return ((uint32_t) s_hyperbusBases[0]->MEM_CFG5);
}

/*!
 * @brief Set configuration register 6.
 *
 * set Hyperbus IP configuration register 6
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG6(uint32_t value)
{
    s_hyperbusBases[0]->MEM_CFG6 = value;
}

/*!
 * @brief Get configuration register 6.
 *
 * Get Hyperbus IP configuration register 6
 *
 * @return register value.
 */
static inline uint32_t HYPERBUS_MEM_GetCFG6()
{
    return ((uint32_t) s_hyperbusBases[0]->MEM_CFG6);
}

/*!
 * @brief Set configuration register 7.
 *
 * set Hyperbus IP configuration register 7
 *
 * @param value  configuration value
 */
static inline void HYPERBUS_MEM_SetCFG7(uint8_t value)
{
    s_hyperbusBases[0]->MEM_CFG7 = value;
}

/*!
 * @brief Get configuration register 7.
 *
 * Get Hyperbus IP configuration register 7
 *
 * @return register value.
 */
static inline uint8_t HYPERBUS_MEM_GetCFG7()
{
    return ((uint8_t) s_hyperbusBases[0]->MEM_CFG7);
}
/* @} */



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
 * @param addr    The access address of read and write
 *
 */
static inline void HYPERBUS_SetAddr(uint32_t addr)
{
    HYPERBUS_EXT_SetAddr(addr);
}

/*!
 * @brief Set Hyperbus Bus channel 0 base address
 *
 * set hyperbus bus channel 0 base address (mbr0)
 * @param mbr0    The mbr0 address
 *
 */
static inline void HYPERBUS_SetMBR0(uint8_t mbr0)
{
    HYPERBUS_MEM_SetCFG0((HYPERBUS_MEM_GetCFG0() & (~HYPERBUS_MEM_CFG0_MBR0_MASK))
                       | HYPERBUS_MEM_CFG0_MBR0(mbr0));
}

/*!
 * @brief Set Hyperbus Bus channel 0 latency
 *
 * set hyperbus bus channel 0 latency
 * @param latency0    The latency value
 *
 */
static inline void HYPERBUS_SetLatency0(uint8_t latency0)
{
    HYPERBUS_MEM_SetCFG0((HYPERBUS_MEM_GetCFG0() & (~HYPERBUS_MEM_CFG0_LATENCY0_MASK))
                       | HYPERBUS_MEM_CFG0_LATENCY0(latency0));
}

/*!
 * @brief Set Hyperbus Bus channel 0 wrap burst size
 *
 * set hyperbus bus channel 0 wrap burst size
 * @param wrapsize    The wrap burst size value
 *
 */
static inline void HYPERBUS_SetWrapSize0(uint8_t wrapsize)
{
    HYPERBUS_MEM_SetCFG0((HYPERBUS_MEM_GetCFG0() & (~HYPERBUS_MEM_CFG0_WRAP_SIZE0_MASK))
                       | HYPERBUS_MEM_CFG0_WRAP_SIZE0(wrapsize));
}

/*!
 * @brief Set Hyperbus Bus channel 0 RX cshi (Chip Select High Between Operation)
 *
 * set hyperbus bus channel 0 cshi
 * @param value    The cshi value
 *
 */
static inline void HYPERBUS_SetRdCSHI0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1() & (~HYPERBUS_MEM_CFG1_RD_CSHI0_MASK))
                       | HYPERBUS_MEM_CFG1_RD_CSHI0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 RX css (Chip Select Setup to next CK Rising Edge)
 *
 * set hyperbus bus channel 0 css
 * @param value    The css value
 *
 */
static inline void HYPERBUS_SetRdCSS0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1() & (~HYPERBUS_MEM_CFG1_RD_CSS0_MASK))
                       | HYPERBUS_MEM_CFG1_RD_CSS0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 RX csh (Chip Select Hold after CK Falling Edge)
 *
 * set hyperbus bus channel 0 csh
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetRdCSH0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1()  & ~(HYPERBUS_MEM_CFG1_RD_CSH0_MASK))
                       | HYPERBUS_MEM_CFG1_RD_CSH0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 TX cshi (Chip Select High Between Operation)
 *
 * set hyperbus bus channel 0 cshi
 * @param value    The cshi value
 *
 */
static inline void HYPERBUS_SetWrCSHI0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1() & (~HYPERBUS_MEM_CFG1_WR_CSHI0_MASK))
                        | HYPERBUS_MEM_CFG1_WR_CSHI0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 TX css (Chip Select Setup to next CK Rising Edge)
 *
 * set hyperbus bus channel 0 css
 * @param value    The css value
 *
 */
static inline void HYPERBUS_SetWrCSS0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1() & ~(HYPERBUS_MEM_CFG1_WR_CSS0_MASK))
                       | HYPERBUS_MEM_CFG1_WR_CSS0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 TX csh (Chip Select Hold after CK Falling Edge)
 *
 * set hyperbus bus channel 0 csh
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetWrCSH0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG1((HYPERBUS_MEM_GetCFG1() & (~HYPERBUS_MEM_CFG1_WR_CSH0_MASK))
                       | HYPERBUS_MEM_CFG1_WR_CSH0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 RX maximum length
 *
 * set hyperbus bus channel 0 RX maximum length, the bit is ignored when MAXEN bit is 0
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetRdMaxLength0(uint16_t value)
{
    HYPERBUS_MEM_SetCFG2((HYPERBUS_MEM_GetCFG2() & (~HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0_MASK))
                       | HYPERBUS_MEM_CFG2_RD_MAX_LENGTH0(value));
}


/*!
 * @brief Set Hyperbus Bus channel 0 TX maximum length
 *
 * set hyperbus bus channel 0 TX maximum length, the bit is ignored when MAXEN bit is 0
 * @param value    The MAXLEN value
 *
 */
static inline void HYPERBUS_SetWrMaxLength0(uint16_t value)
{
    HYPERBUS_MEM_SetCFG2((HYPERBUS_MEM_GetCFG2() & (~HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0_MASK))
                        | HYPERBUS_MEM_CFG2_WR_MAX_LENGTH0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 ACS (Asymmetry Cache System Support)
 *
 * set hyperbus bus channel 0 ACS
 * @param value    The acs value
 *
 */
static inline void HYPERBUS_SetACS0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_ACS0_MASK))
                       | HYPERBUS_MEM_CFG3_ACS0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 TCO.
 *
 * set hyperbus bus channel 0 TCO
 * @param value    The tco value
 *
 */
static inline void HYPERBUS_SetTCO0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_TCO0_MASK))
                       | HYPERBUS_MEM_CFG3_TCO0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 device type.
 *
 * set hyperbus bus channel 0 dt
 * @param value    The dt value
 *
 */
static inline void HYPERBUS_SetDT0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_DT0_MASK))
                       | HYPERBUS_MEM_CFG3_DT0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 (Configuration Register Target).
 *
 * set hyperbus bus channel 0 crt
 * @param value    The crt value
 *
 */
static inline void HYPERBUS_SetCRT0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_CRT0_MASK))
                       | HYPERBUS_MEM_CFG3_CRT0(value));
}


/*!
 * @brief Set Hyperbus Bus channel 0 RX maximum length enable
 *
 * @param value    The MAXEN value
 *
 */
static inline void HYPERBUS_SetRdMaxLengthEn0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0_MASK))
                       | HYPERBUS_MEM_CFG3_RD_MAX_LEN_EN0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 TX maximum length enable
 *
 * @param value    The MAXEN value
 *
 */
static inline void HYPERBUS_SetWrMaxLengthEN0(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0_MASK))
                       | HYPERBUS_MEM_CFG3_WR_MAX_LEN_EN0(value));
}

/*!
 * @brief Set Hyperbus Bus channel 0 Delay adj
 *
 * @param value    The Delay adj value
 *
 */
static inline void HYPERBUS_SetRdsDelayAdj(uint8_t value)
{
    HYPERBUS_MEM_SetCFG3((HYPERBUS_MEM_GetCFG3() & (~HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ_MASK))
                       | HYPERBUS_MEM_CFG3_RDS_DELAY_ADJ(value));
}



/*!
 * @brief Set Hyperbus Bus channel 1 base address
 *
 * set hyperbus bus channel 1 base address (mbr1)
 * @param mbr1    The mbr1 address
 *
 */
static inline void HYPERBUS_SetMBR1(uint8_t mbr1)
{
    HYPERBUS_MEM_SetCFG4((HYPERBUS_MEM_GetCFG4() & (~HYPERBUS_MEM_CFG4_MBR1_MASK))
                       | HYPERBUS_MEM_CFG4_MBR1(mbr1));
}

/*!
 * @brief Set Hyperbus Bus channel 1 latency
 *
 * set hyperbus bus channel 1 latency
 * @param latency1    The latency value
 *
 */
static inline void HYPERBUS_SetLatency1(uint8_t latency1)
{
    HYPERBUS_MEM_SetCFG4((HYPERBUS_MEM_GetCFG4() & (~HYPERBUS_MEM_CFG4_LATENCY1_MASK))
                       | HYPERBUS_MEM_CFG4_LATENCY1(latency1));
}

/*!
 * @brief Set Hyperbus Bus channel 1 wrap burst size
 *
 * set hyperbus bus channel 1 wrap burst size
 * @param wrapsize    The wrap burst size value
 *
 */
static inline void HYPERBUS_SetWrapSize1(uint8_t wrapsize)
{
    HYPERBUS_MEM_SetCFG4((HYPERBUS_MEM_GetCFG4() & (~HYPERBUS_MEM_CFG4_WRAP_SIZE1_MASK))
                       | HYPERBUS_MEM_CFG4_WRAP_SIZE1(wrapsize));
}

/*!
 * @brief Set Hyperbus Bus channel 1 RX cshi (Chip Select High Between Operation)
 *
 * set hyperbus bus channel 1 cshi
 * @param value    The cshi value
 *
 */
static inline void HYPERBUS_SetRdCSHI1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5() & (~HYPERBUS_MEM_CFG5_RD_CSHI1_MASK))
                       | HYPERBUS_MEM_CFG5_RD_CSHI1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 RX css (Chip Select Setup to next CK Rising Edge)
 *
 * set hyperbus bus channel 1 css
 * @param value    The css value
 *
 */
static inline void HYPERBUS_SetRdCSS1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5() & (~HYPERBUS_MEM_CFG5_RD_CSS1_MASK))
                       | HYPERBUS_MEM_CFG5_RD_CSS1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 RX csh (Chip Select Hold after CK Falling Edge)
 *
 * set hyperbus bus channel 1 csh
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetRdCSH1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5()  & ~(HYPERBUS_MEM_CFG5_RD_CSH1_MASK))
                       | HYPERBUS_MEM_CFG5_RD_CSH1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TX cshi (Chip Select High Between Operation)
 *
 * set hyperbus bus channel 1 cshi
 * @param value    The cshi value
 *
 */
static inline void HYPERBUS_SetWrCSHI1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5() & (~HYPERBUS_MEM_CFG5_WR_CSHI1_MASK))
                        | HYPERBUS_MEM_CFG5_WR_CSHI1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TX css (Chip Select Setup to next CK Rising Edge)
 *
 * set hyperbus bus channel 1 css
 * @param value    The css value
 *
 */
static inline void HYPERBUS_SetWrCSS1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5() & ~(HYPERBUS_MEM_CFG5_WR_CSS1_MASK))
                       | HYPERBUS_MEM_CFG5_WR_CSS1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TX csh (Chip Select Hold after CK Falling Edge)
 *
 * set hyperbus bus channel 1 csh
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetWrCSH1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG5((HYPERBUS_MEM_GetCFG5() & (~HYPERBUS_MEM_CFG5_WR_CSH1_MASK))
                       | HYPERBUS_MEM_CFG5_WR_CSH1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 RX maximum length
 *
 * set hyperbus bus channel 1 RX maximum length, the bit is ignored when MAXEN bit is 0
 * @param value    The csh value
 *
 */
static inline void HYPERBUS_SetRdMaxLength1(uint16_t value)
{
    HYPERBUS_MEM_SetCFG6((HYPERBUS_MEM_GetCFG6() & (~HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1_MASK))
                       | HYPERBUS_MEM_CFG6_RD_MAX_LENGTH1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TX maximum length
 *
 * set hyperbus bus channel 1 TX maximum length, the bit is ignored when MAXEN bit is 0
 * @param value    The MAXLEN value
 *
 */
static inline void HYPERBUS_SetWrMaxLength1(uint16_t value)
{
    HYPERBUS_MEM_SetCFG6((HYPERBUS_MEM_GetCFG6() & (~HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1_MASK))
                        | HYPERBUS_MEM_CFG6_WR_MAX_LENGTH1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 ACS (Asymmetry Cache System Support)
 *
 * set hyperbus bus channel 1 ACS
 * @param value    The acs value
 *
 */

static inline void HYPERBUS_SetACS1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_ACS1_MASK))
                       | HYPERBUS_MEM_CFG7_ACS1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TCO.
 *
 * set hyperbus bus channel 1 TCO
 * @param value    The tco value
 *
 */
static inline void HYPERBUS_SetTCO1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_TCO1_MASK))
                       | HYPERBUS_MEM_CFG7_TCO1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 device type.
 *
 * set hyperbus bus channel 1 dt
 * @param value    The dt value
 *
 */

static inline void HYPERBUS_SetDT1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_DT1_MASK))
                       | HYPERBUS_MEM_CFG7_DT1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 (Configuration Register Target).
 *
 * set hyperbus bus channel 1 crt
 * @param value    The crt value
 *
 */
static inline void HYPERBUS_SetCRT1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_CRT1_MASK))
                       | HYPERBUS_MEM_CFG7_CRT1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 RX maximum length enable
 *
 * @param value    The MAXEN value
 *
 */
static inline void HYPERBUS_SetRdMaxLengthEn1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1_MASK))
                       | HYPERBUS_MEM_CFG7_RD_MAX_LEN_EN1(value));
}

/*!
 * @brief Set Hyperbus Bus channel 1 TX maximum length enable
 *
 * @param value    The MAXEN value
 *
 */
static inline void HYPERBUS_SetWrMaxLengthEN1(uint8_t value)
{
    HYPERBUS_MEM_SetCFG7((HYPERBUS_MEM_GetCFG7() & (~HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1_MASK))
                       | HYPERBUS_MEM_CFG7_WR_MAX_LEN_EN1(value));
}
/* @} */

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
 * @brief De-initializes the HYPERBUS peripheral, Clock Gating. Call this API to disable the HYPERBUS clock.
 * @param base HYPERBUS peripheral address.
 */
void HYPERBUS_DeInit(HYPERBUS_Type *base);

/*!
 * @brief Configure the HYPERBUS memrory configuraton
 *
 * @param  base           HYPERBUS peripheral address.
 * @param  max_length_en  The HYPERBUS maximum access data length enable
 * @param  max_length     The HYPERBUS maximum access data length
 * @param  rd_wr          The type of operation, read or write
 * @param  device         The type of hyperbus, ram or flash
 */
static inline void HYPERBUS_SetMaxLength(HYPERBUS_Type *base, int max_length_en, int max_length, int rd_wr, char device)
{
#if (__HYPERBUS_CSN0_FOR_RAM__ == 1)
    if (device == uHYPERBUS_Ram) {
#else
    if (device == uHYPERBUS_Flash) {
#endif
        if (rd_wr == uHYPERBUS_Read) {
            HYPERBUS_SetRdMaxLengthEn0(max_length_en);
            HYPERBUS_SetRdMaxLength0(max_length);
        } else {
            HYPERBUS_SetWrMaxLengthEN0(max_length_en);
            HYPERBUS_SetWrMaxLength0(max_length);
        }
    }  else {
        if (rd_wr == uHYPERBUS_Read) {
            HYPERBUS_SetRdMaxLengthEn1(max_length_en);
            HYPERBUS_SetRdMaxLength1(max_length);
        } else {
            HYPERBUS_SetRdMaxLengthEn1(max_length_en);
            HYPERBUS_SetWrMaxLength1(max_length);
        }
    }
}

/*!
 * @brief Configure the HYPERBUS memrory timing
 *
 * Set the latency cycles and timing performence for particular clock frequency
 * @param  base     HYPERBUS peripheral address.
 * @param  cshi     The value of chip select high between operations
 * @param  css      The value of chip select setup to next CK rising edge
 * @param  csh      The value of chip select hold after next rising edge
 * @param  latency  The value of latency cycle for HyperbusRAM
 * @param  rd_wr    The type of operation, read or write
 * @param  device   The type of hyperbus, ram or flash
 */
static inline void HYPERBUS_SetTiming(HYPERBUS_Type *base, int cshi, int css, int csh, int latency, int rd_wr, char device)
{
#if (__HYPERBUS_CSN0_FOR_RAM__ == 1)
    if (device == uHYPERBUS_Ram) {
#else
    if (device == uHYPERBUS_Flash) {
#endif
        if (rd_wr == uHYPERBUS_Read) {
            HYPERBUS_SetRdCSHI0(cshi);
            HYPERBUS_SetRdCSS0(css);
            HYPERBUS_SetRdCSH0(csh);
        } else {
            HYPERBUS_SetWrCSHI0(cshi);
            HYPERBUS_SetWrCSS0(css);
            HYPERBUS_SetWrCSH0(csh);
        }
        HYPERBUS_SetLatency0(latency);
    }  else {
        if (rd_wr == uHYPERBUS_Read) {
            HYPERBUS_SetRdCSHI1(cshi);
            HYPERBUS_SetRdCSS1(css);
            HYPERBUS_SetRdCSH1(csh);
        } else {
            HYPERBUS_SetWrCSHI1(cshi);
            HYPERBUS_SetWrCSS1(css);
            HYPERBUS_SetWrCSH1(csh);
        }
        HYPERBUS_SetLatency1(latency);
    }
}

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
