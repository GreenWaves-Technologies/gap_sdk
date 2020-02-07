/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __UART_PERIPH_H__
#define __UART_PERIPH_H__

#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- UART Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** UART_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t status; /**< uDMA UART status register. */
    volatile uint32_t setup; /**< UDMA UART configuration register. */
    volatile uint32_t error; /**< uDMA UART Error status */
    volatile uint32_t irq_en; /**< uDMA UART Read or Error interrupt enable register. */
    volatile uint32_t valid; /**< uDMA UART Read polling data valid flag register. */
    volatile uint32_t data; /**< uDMA UART Read polling data register. */
} uart_t;


/* ----------------------------------------------------------------------------
   -- UART Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name STATUS */
/* TX busy status flag:
  - 1'b0: no TX transfer on-going
  - 1'b1: TX transfer on-going */
#define UART_STATUS_TX_BUSY_MASK                                     (0x1)
#define UART_STATUS_TX_BUSY_SHIFT                                    (0)
#define UART_STATUS_TX_BUSY(val)                                     (((uint32_t)(((uint32_t)(val)) << UART_STATUS_TX_BUSY_SHIFT)) & UART_STATUS_TX_BUSY_MASK)

/* RX busy status flag:
  - 1'b0: no RX transfer on-going
  - 1'b1: RX transfer on-going */
#define UART_STATUS_RX_BUSY_MASK                                     (0x2)
#define UART_STATUS_RX_BUSY_SHIFT                                    (1)
#define UART_STATUS_RX_BUSY(val)                                     (((uint32_t)(((uint32_t)(val)) << UART_STATUS_RX_BUSY_SHIFT)) & UART_STATUS_RX_BUSY_MASK)

/* Reserved/Not used. */
#define UART_STATUS_RESERVED_0_MASK                                  (0xfffffffc)
#define UART_STATUS_RESERVED_0_SHIFT                                 (2)
#define UART_STATUS_RESERVED_0(val)                                  (((uint32_t)(((uint32_t)(val)) << UART_STATUS_RESERVED_0_SHIFT)) & UART_STATUS_RESERVED_0_MASK)


/*! @name SETUP */
/* Parity bit generation and check configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define UART_SETUP_PARITY_ENA_MASK                                   (0x1)
#define UART_SETUP_PARITY_ENA_SHIFT                                  (0)
#define UART_SETUP_PARITY_ENA(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_SETUP_PARITY_ENA_SHIFT)) & UART_SETUP_PARITY_ENA_MASK)

/* Character length bitfield:
  - 2'b00: 5 bits
  - 2'b01: 6 bits
  - 2'b10: 7 bits
  - 2'b11: 8 bits */
#define UART_SETUP_BIT_LENGTH_MASK                                   (0x6)
#define UART_SETUP_BIT_LENGTH_SHIFT                                  (1)
#define UART_SETUP_BIT_LENGTH(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_SETUP_BIT_LENGTH_SHIFT)) & UART_SETUP_BIT_LENGTH_MASK)

/* Stop bits length bitfield:
  - 1'b0: 1 stop bit
  - 1'b1: 2 stop bits */
#define UART_SETUP_STOP_BITS_MASK                                    (0x8)
#define UART_SETUP_STOP_BITS_SHIFT                                   (3)
#define UART_SETUP_STOP_BITS(val)                                    (((uint32_t)(((uint32_t)(val)) << UART_SETUP_STOP_BITS_SHIFT)) & UART_SETUP_STOP_BITS_MASK)

/* When in uart read, use polling method to read the data,  read interrupt enable flag will be ignored:
  - 1'b0: Do not using polling method to read data.
  - 1'b1: Using polling method to read data. Interrupt enable flag will be ignored. */
#define UART_SETUP_POLLING_EN_MASK                                   (0x10)
#define UART_SETUP_POLLING_EN_SHIFT                                  (4)
#define UART_SETUP_POLLING_EN(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_SETUP_POLLING_EN_SHIFT)) & UART_SETUP_POLLING_EN_MASK)

/* Reserved/Not used. */
#define UART_SETUP_RESERVED_0_MASK                                   (0xe0)
#define UART_SETUP_RESERVED_0_SHIFT                                  (5)
#define UART_SETUP_RESERVED_0(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_SETUP_RESERVED_0_SHIFT)) & UART_SETUP_RESERVED_0_MASK)

/* TX transceiver configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define UART_SETUP_TX_ENA_MASK                                       (0x100)
#define UART_SETUP_TX_ENA_SHIFT                                      (8)
#define UART_SETUP_TX_ENA(val)                                       (((uint32_t)(((uint32_t)(val)) << UART_SETUP_TX_ENA_SHIFT)) & UART_SETUP_TX_ENA_MASK)

/* RX transceiver configuration bitfield:
  - 1'b0: disabled
  - 1'b1: enabled */
#define UART_SETUP_RX_ENA_MASK                                       (0x200)
#define UART_SETUP_RX_ENA_SHIFT                                      (9)
#define UART_SETUP_RX_ENA(val)                                       (((uint32_t)(((uint32_t)(val)) << UART_SETUP_RX_ENA_SHIFT)) & UART_SETUP_RX_ENA_MASK)

/* Reserved/Not used. */
#define UART_SETUP_RESERVED_1_MASK                                   (0xfc00)
#define UART_SETUP_RESERVED_1_SHIFT                                  (10)
#define UART_SETUP_RESERVED_1(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_SETUP_RESERVED_1_SHIFT)) & UART_SETUP_RESERVED_1_MASK)

/* UART Clock divider configuration bitfield. The baudrate is equal to SOC_FREQ/CLKDIV. */
#define UART_SETUP_CLKDIV_MASK                                       (0xffff0000)
#define UART_SETUP_CLKDIV_SHIFT                                      (16)
#define UART_SETUP_CLKDIV(val)                                       (((uint32_t)(((uint32_t)(val)) << UART_SETUP_CLKDIV_SHIFT)) & UART_SETUP_CLKDIV_MASK)


/*! @name ERROR */
/* RX overflow error status flag:
  - 1'b0: no error
  - 1'b1: RX overflow error occurred  */
#define UART_ERROR_RX_ERR_OVERFLOW_MASK                              (0x1)
#define UART_ERROR_RX_ERR_OVERFLOW_SHIFT                             (0)
#define UART_ERROR_RX_ERR_OVERFLOW(val)                              (((uint32_t)(((uint32_t)(val)) << UART_ERROR_RX_ERR_OVERFLOW_SHIFT)) & UART_ERROR_RX_ERR_OVERFLOW_MASK)

/* RX parity error status flag:
  - 1'b0: no error
  - 1'b1: RX parity error occurred  */
#define UART_ERROR_RX_ERR_PARITY_MASK                                (0x2)
#define UART_ERROR_RX_ERR_PARITY_SHIFT                               (1)
#define UART_ERROR_RX_ERR_PARITY(val)                                (((uint32_t)(((uint32_t)(val)) << UART_ERROR_RX_ERR_PARITY_SHIFT)) & UART_ERROR_RX_ERR_PARITY_MASK)

/* Reserved/Not used. */
#define UART_ERROR_RESERVED_0_MASK                                   (0xfffffffc)
#define UART_ERROR_RESERVED_0_SHIFT                                  (2)
#define UART_ERROR_RESERVED_0(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_ERROR_RESERVED_0_SHIFT)) & UART_ERROR_RESERVED_0_MASK)


/*! @name IRQ_EN */
/* RX interrupt enable flag:
  - 1'b0: RX IRQ disable
  - 1'b1: RX IRQ enable  */
#define UART_IRQ_EN_RX_MASK                                          (0x1)
#define UART_IRQ_EN_RX_SHIFT                                         (0)
#define UART_IRQ_EN_RX(val)                                          (((uint32_t)(((uint32_t)(val)) << UART_IRQ_EN_RX_SHIFT)) & UART_IRQ_EN_RX_MASK)

/* Error interrupt in enable flag:
  - 1'b0: Error IRQ disable
  - 1'b1: Error IRQ enable  */
#define UART_IRQ_EN_ERROR_MASK                                       (0x2)
#define UART_IRQ_EN_ERROR_SHIFT                                      (1)
#define UART_IRQ_EN_ERROR(val)                                       (((uint32_t)(((uint32_t)(val)) << UART_IRQ_EN_ERROR_SHIFT)) & UART_IRQ_EN_ERROR_MASK)

/* Reserved/Not used. */
#define UART_IRQ_EN_RESERVED_0_MASK                                  (0xfffffffc)
#define UART_IRQ_EN_RESERVED_0_SHIFT                                 (2)
#define UART_IRQ_EN_RESERVED_0(val)                                  (((uint32_t)(((uint32_t)(val)) << UART_IRQ_EN_RESERVED_0_SHIFT)) & UART_IRQ_EN_RESERVED_0_MASK)


/*! @name VALID */
/* Used only in RX polling method to indicate data is ready for read:
  - 1'b0: Data is not ready to read
  - 1'b1: Data is ready to read */
#define UART_VALID_READY_MASK                                        (0x1)
#define UART_VALID_READY_SHIFT                                       (0)
#define UART_VALID_READY(val)                                        (((uint32_t)(((uint32_t)(val)) << UART_VALID_READY_SHIFT)) & UART_VALID_READY_MASK)

/* Reserved/Not used. */
#define UART_VALID_RESERVED_0_MASK                                   (0xfffffffe)
#define UART_VALID_RESERVED_0_SHIFT                                  (1)
#define UART_VALID_RESERVED_0(val)                                   (((uint32_t)(((uint32_t)(val)) << UART_VALID_RESERVED_0_SHIFT)) & UART_VALID_RESERVED_0_MASK)


/*! @name DATA */
/* RX read data for polling or interrupt. */
#define UART_DATA_BYTE_MASK                                          (0xff)
#define UART_DATA_BYTE_SHIFT                                         (0)
#define UART_DATA_BYTE(val)                                          (((uint32_t)(((uint32_t)(val)) << UART_DATA_BYTE_SHIFT)) & UART_DATA_BYTE_MASK)

/* Reserved/Not used. */
#define UART_DATA_RESERVED_0_MASK                                    (0xffffff00)
#define UART_DATA_RESERVED_0_SHIFT                                   (8)
#define UART_DATA_RESERVED_0(val)                                    (((uint32_t)(((uint32_t)(val)) << UART_DATA_RESERVED_0_SHIFT)) & UART_DATA_RESERVED_0_MASK)


/*! @name STATUS */
typedef union
{
    struct
    {
        /* TX busy status flag:
        - 1'b0: no TX transfer on-going
        - 1'b1: TX transfer on-going */
        uint32_t tx_busy:1;
        /* RX busy status flag:
        - 1'b0: no RX transfer on-going
        - 1'b1: RX transfer on-going */
        uint32_t rx_busy:1;
        /* Reserved/Not used. */
        uint32_t reserved_0:30;
    } field;
    uint32_t word;
} uart_status_t;

/*! @name SETUP */
typedef union
{
    struct
    {
        /* Parity bit generation and check configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t parity_ena:1;
        /* Character length bitfield:
        - 2'b00: 5 bits
        - 2'b01: 6 bits
        - 2'b10: 7 bits
        - 2'b11: 8 bits */
        uint32_t bit_length:2;
        /* Stop bits length bitfield:
        - 1'b0: 1 stop bit
        - 1'b1: 2 stop bits */
        uint32_t stop_bits:1;
        /* When in uart read, use polling method to read the data,  read interrupt enable flag will be ignored:
        - 1'b0: Do not using polling method to read data.
        - 1'b1: Using polling method to read data. Interrupt enable flag will be ignored. */
        uint32_t polling_en:1;
        /* Reserved/Not used. */
        uint32_t reserved_0:3;
        /* TX transceiver configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t tx_ena:1;
        /* RX transceiver configuration bitfield:
        - 1'b0: disabled
        - 1'b1: enabled */
        uint32_t rx_ena:1;
        /* Reserved/Not used. */
        uint32_t reserved_1:6;
        /* UART Clock divider configuration bitfield. The baudrate is equal to SOC_FREQ/CLKDIV. */
        uint32_t clkdiv:16;
    } field;
    uint32_t word;
} uart_setup_t;

/*! @name ERROR */
typedef union
{
    struct
    {
        /* RX overflow error status flag:
        - 1'b0: no error
        - 1'b1: RX overflow error occurred  */
        uint32_t rx_err_overflow:1;
        /* RX parity error status flag:
        - 1'b0: no error
        - 1'b1: RX parity error occurred  */
        uint32_t rx_err_parity:1;
        /* Reserved/Not used. */
        uint32_t reserved_0:30;
    } field;
    uint32_t word;
} uart_error_t;

/*! @name IRQ_EN */
typedef union
{
    struct
    {
        /* RX interrupt enable flag:
        - 1'b0: RX IRQ disable
        - 1'b1: RX IRQ enable  */
        uint32_t rx:1;
        /* Error interrupt in enable flag:
        - 1'b0: Error IRQ disable
        - 1'b1: Error IRQ enable  */
        uint32_t error:1;
        /* Reserved/Not used. */
        uint32_t reserved_0:30;
    } field;
    uint32_t word;
} uart_irq_en_t;

/*! @name VALID */
typedef union
{
    struct
    {
        /* Used only in RX polling method to indicate data is ready for read:
        - 1'b0: Data is not ready to read
        - 1'b1: Data is ready to read */
        uint32_t ready:1;
        /* Reserved/Not used. */
        uint32_t reserved_0:31;
    } field;
    uint32_t word;
} uart_valid_t;

/*! @name DATA */
typedef union
{
    struct
    {
        /* RX read data for polling or interrupt. */
        uint32_t byte:8;
        /* Reserved/Not used. */
        uint32_t reserved_0:24;
    } field;
    uint32_t word;
} uart_data_t;


#endif /* __UART_PERIPH_H__ */
