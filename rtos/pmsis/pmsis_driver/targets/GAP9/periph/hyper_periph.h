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

#ifndef __HYPER_PERIPH_H__
#define __HYPER_PERIPH_H__

#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- HYPERBUS Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** HYPERBUS_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t ext_addr; /**< Memory access address register. */
    volatile uint32_t timing_cfg; /**< HYPERBUS and Octo SPI Memory Timing configuration register. */
    volatile uint32_t mba0; /**< Device start address register. */
    volatile uint32_t mba1; /**< Device start address register. */
    volatile uint32_t device; /**< Device type register(RAM or FLASH). */
    volatile uint32_t ospi_cmd; /**< OSPI command and alternative bytes (each has 2 bytes). */
    volatile uint32_t ospi_addr; /**< OSPI address. */
    volatile uint32_t ospi_cfg; /**< OSPI configuration. */
    volatile uint32_t ospi_csn; /**< OSPI chip select configuration. */
    volatile uint32_t burst_size; /**< OSPI burst size for transfer size big than 1K. */
    volatile uint32_t burst_enable; /**< OSPI burst mode enable. */
    volatile uint32_t irq_en; /**< OSPI interrupt enable register. */
    volatile uint32_t clk_div; /**< Clock divide. */
    volatile uint32_t status; /**< Transfer status for error. */
} hyperbus_t;


/* ----------------------------------------------------------------------------
   -- HYPERBUS Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name EXT_ADDR */
/* Memory access address bitfield. */
#define HYPERBUS_EXT_ADDR_SADDR_MASK                                 (0x7fffffff)
#define HYPERBUS_EXT_ADDR_SADDR_SHIFT                                (0)
#define HYPERBUS_EXT_ADDR_SADDR(val)                                 (((uint32_t)(((uint32_t)(val)) << HYPERBUS_EXT_ADDR_SADDR_SHIFT)) & HYPERBUS_EXT_ADDR_SADDR_MASK)

/* Register access flag bitfield. */
#define HYPERBUS_EXT_ADDR_REG_ACCESS_MASK                            (0x80000000)
#define HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT                           (31)
#define HYPERBUS_EXT_ADDR_REG_ACCESS(val)                            (((uint32_t)(((uint32_t)(val)) << HYPERBUS_EXT_ADDR_REG_ACCESS_SHIFT)) & HYPERBUS_EXT_ADDR_REG_ACCESS_MASK)


/*! @name TIMING_CFG */
/* Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0.
  When using HyperRAM memory, this bit should be set to the same value
  as the read latency in configuration register of HyperRAM memory.
  For OctoSPI, this is the dummy cycle after ADDRESS stage :
  - 5'b00000: 0 CK
  - 5'b00001: 1 CK
  - 5'b00010: 2 CK
  ...
  - 5'b11111: 31 CK */
#define HYPERBUS_TIMING_CFG_LATENCY0_MASK                            (0x1f)
#define HYPERBUS_TIMING_CFG_LATENCY0_SHIFT                           (0)
#define HYPERBUS_TIMING_CFG_LATENCY0(val)                            (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_LATENCY0_SHIFT)) & HYPERBUS_TIMING_CFG_LATENCY0_MASK)

/* Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1.
  When using HyperRAM memory, this bit should be set to the same value
  as the read latency in configuration register of HyperRAM memory.
  For OctoSPI, this is the dummy cycle after ADDRESS stage :
  - 5'b00000: 0 CK
  - 5'b00001: 1 CK
  - 5'b00010: 2 CK
  ...
  - 5'b11111: 31 CK */
#define HYPERBUS_TIMING_CFG_LATENCY1_MASK                            (0x3e0)
#define HYPERBUS_TIMING_CFG_LATENCY1_SHIFT                           (5)
#define HYPERBUS_TIMING_CFG_LATENCY1(val)                            (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_LATENCY1_SHIFT)) & HYPERBUS_TIMING_CFG_LATENCY1_MASK)

/* Some HyperBus devices may require a minimum time between the end
  of a prior transaction and the start of a new access.
  This time is referred to as Read-Write-Recovery time (tRWR).
  The master interface must start driving CS# Low only at a time when
  the CA1 transfer will complete after tRWR is satisfied.
  - 4'b0000: 16 CK
  - 4'b0001: 1 CK
  - 4'b0001: 2 CK
  ...
  - 4'b1111: 15 CK */
#define HYPERBUS_TIMING_CFG_RW_RECOVERY_MASK                         (0x3c00)
#define HYPERBUS_TIMING_CFG_RW_RECOVERY_SHIFT                        (10)
#define HYPERBUS_TIMING_CFG_RW_RECOVERY(val)                         (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_RW_RECOVERY_SHIFT)) & HYPERBUS_TIMING_CFG_RW_RECOVERY_MASK)

/* Delay of RWDS for center aligned read:
  - 3'b000: 0 logic delay
  - 3'b001: 1 logic delay
  - 3'b010: 2 logic delay
  …
  - 3'b111: 7 logic delay */
#define HYPERBUS_TIMING_CFG_RWDS_DELAY_MASK                          (0x1c000)
#define HYPERBUS_TIMING_CFG_RWDS_DELAY_SHIFT                         (14)
#define HYPERBUS_TIMING_CFG_RWDS_DELAY(val)                          (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_RWDS_DELAY_SHIFT)) & HYPERBUS_TIMING_CFG_RWDS_DELAY_MASK)

/* Auto check for RWDS high or low for additional latency :
  - 1'b0: additional latency no autocheck
  - 1'b1: additional latency autocheck */
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK     (0x20000)
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SHIFT    (17)
#define HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(val)     (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SHIFT)) & HYPERBUS_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK)

/* Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer :
  - 14'h0000: 1 CK
  - 14'h0001: 2 CK
  - 14'h0011: 3 CK
  - …
  - 14'h3FFF: 16383 CK */
#define HYPERBUS_TIMING_CFG_CS_MAX_MASK                              (0xfffc0000)
#define HYPERBUS_TIMING_CFG_CS_MAX_SHIFT                             (18)
#define HYPERBUS_TIMING_CFG_CS_MAX(val)                              (((uint32_t)(((uint32_t)(val)) << HYPERBUS_TIMING_CFG_CS_MAX_SHIFT)) & HYPERBUS_TIMING_CFG_CS_MAX_MASK)


/*! @name MBA0 */
/* Memory Base Address 0 for both RAM and FLASH bitfield.
  The base address of addressable region to each memory is set up.
  Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0.
  MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.
  - MBA0 < MBA1, if (MBA1 <= EXT_ADDR) CS1 = 0;  else CS0 = 0;
  - MBA0 > MBA1, if (MBA0 <= EXT_ADDR) CS0 = 0;  else CS1 = 0; */
#define HYPERBUS_MBA0_MBA0_MASK                                      (0x7f000000)
#define HYPERBUS_MBA0_MBA0_SHIFT                                     (24)
#define HYPERBUS_MBA0_MBA0(val)                                      (((uint32_t)(((uint32_t)(val)) << HYPERBUS_MBA0_MBA0_SHIFT)) & HYPERBUS_MBA0_MBA0_MASK)


/*! @name MBA1 */
/* Memory Base Address  for both RAM and FLASH bitfield.
  The base address of addressable region to each memory is set up.
  Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0.
  MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.
  - MBA0 < MBA1, if (MBA1 <= EXT_ADDR) CSn1 = 0;  else CSn0 = 0;
  - MBA0 > MBA1, if (MBA0 <= EXT_ADDR) CSn0 = 0;  else CSn1 = 0; */
#define HYPERBUS_MBA1_MBA1_MASK                                      (0x7f000000)
#define HYPERBUS_MBA1_MBA1_SHIFT                                     (24)
#define HYPERBUS_MBA1_MBA1(val)                                      (((uint32_t)(((uint32_t)(val)) << HYPERBUS_MBA1_MBA1_SHIFT)) & HYPERBUS_MBA1_MBA1_MASK)


/*! @name DEVICE */
/* Device type :
  - 1'b00: Octo/ Single SPI
  - 1'b01: HYPERBUS */
#define HYPERBUS_DEVICE_TYPE_MASK                                    (0x1)
#define HYPERBUS_DEVICE_TYPE_SHIFT                                   (0)
#define HYPERBUS_DEVICE_TYPE(val)                                    (((uint32_t)(((uint32_t)(val)) << HYPERBUS_DEVICE_TYPE_SHIFT)) & HYPERBUS_DEVICE_TYPE_MASK)

/* When in HYPERBUS mode :
  - 1'b0: HYPERRAM
  - 1'b1: HYPERLASH */
#define HYPERBUS_DEVICE_DT0_MASK                                     (0x2)
#define HYPERBUS_DEVICE_DT0_SHIFT                                    (1)
#define HYPERBUS_DEVICE_DT0(val)                                     (((uint32_t)(((uint32_t)(val)) << HYPERBUS_DEVICE_DT0_SHIFT)) & HYPERBUS_DEVICE_DT0_MASK)

/* When in HYPERBUS mode :
  - 1'b0: HYPERRAM
  - 1'b1: HYPERLASH */
#define HYPERBUS_DEVICE_DT1_MASK                                     (0x4)
#define HYPERBUS_DEVICE_DT1_SHIFT                                    (2)
#define HYPERBUS_DEVICE_DT1(val)                                     (((uint32_t)(((uint32_t)(val)) << HYPERBUS_DEVICE_DT1_SHIFT)) & HYPERBUS_DEVICE_DT1_MASK)


/*! @name OSPI_CMD */
/* 2 Bytes SPI command */
#define HYPERBUS_OSPI_CMD_CMD_MASK                                   (0xffff)
#define HYPERBUS_OSPI_CMD_CMD_SHIFT                                  (0)
#define HYPERBUS_OSPI_CMD_CMD(val)                                   (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CMD_CMD_SHIFT)) & HYPERBUS_OSPI_CMD_CMD_MASK)

/* 2 Bytes SPI alternative */
#define HYPERBUS_OSPI_CMD_MODE_MASK                                  (0xffff0000)
#define HYPERBUS_OSPI_CMD_MODE_SHIFT                                 (16)
#define HYPERBUS_OSPI_CMD_MODE(val)                                  (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CMD_MODE_SHIFT)) & HYPERBUS_OSPI_CMD_MODE_MASK)


/*! @name OSPI_ADDR */
/* 4 Bytes SPI address */
#define HYPERBUS_OSPI_ADDR_ADDR_MASK                                 (0xffffffff)
#define HYPERBUS_OSPI_ADDR_ADDR_SHIFT                                (0)
#define HYPERBUS_OSPI_ADDR_ADDR(val)                                 (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_ADDR_ADDR_SHIFT)) & HYPERBUS_OSPI_ADDR_ADDR_MASK)


/*! @name OSPI_CFG */
/* Octo SPI command size :
  - 2’b0: 0 byte
  - 2'b1: 1 byte
  - 2'b2: 2 byte */
#define HYPERBUS_OSPI_CFG_CMD_SIZE_MASK                              (0x3)
#define HYPERBUS_OSPI_CFG_CMD_SIZE_SHIFT                             (0)
#define HYPERBUS_OSPI_CFG_CMD_SIZE(val)                              (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_CMD_SIZE_SHIFT)) & HYPERBUS_OSPI_CFG_CMD_SIZE_MASK)

/* Octo SPI address size :
  - 3'b000: 0 byte (Jump ADDRESS stage)
  - 3'b001: 1 byte
  - 3'b010: 2 byte
  - 3'b011: 3 byte
  - 3'b100: 4 byte */
#define HYPERBUS_OSPI_CFG_ADDR_SIZE_MASK                             (0x70)
#define HYPERBUS_OSPI_CFG_ADDR_SIZE_SHIFT                            (4)
#define HYPERBUS_OSPI_CFG_ADDR_SIZE(val)                             (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_ADDR_SIZE_SHIFT)) & HYPERBUS_OSPI_CFG_ADDR_SIZE_MASK)

/* Octo SPI line number for Single SPI or Octo SPI:
  - 2'b00: 8 line for Octo SPI
  - 2'b01: 4 line for QUAD SPI (sdio{3:0])
  - 2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) */
#define HYPERBUS_OSPI_CFG_LINE_MASK                                  (0x300)
#define HYPERBUS_OSPI_CFG_LINE_SHIFT                                 (8)
#define HYPERBUS_OSPI_CFG_LINE(val)                                  (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_LINE_SHIFT)) & HYPERBUS_OSPI_CFG_LINE_MASK)

/* Octo SPI command ddr mode or single mode :
  - 1'b0: DTR mode
  - 1'b1: STR mode */
#define HYPERBUS_OSPI_CFG_CMD_DTR_STR_MASK                           (0x1000)
#define HYPERBUS_OSPI_CFG_CMD_DTR_STR_SHIFT                          (12)
#define HYPERBUS_OSPI_CFG_CMD_DTR_STR(val)                           (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_CMD_DTR_STR_SHIFT)) & HYPERBUS_OSPI_CFG_CMD_DTR_STR_MASK)

/* Octo SPI address ddr mode or single mode :
  - 1'b0: DTR mode
  - 1'b1: STR mode */
#define HYPERBUS_OSPI_CFG_ADDR_DTR_STR_MASK                          (0x2000)
#define HYPERBUS_OSPI_CFG_ADDR_DTR_STR_SHIFT                         (13)
#define HYPERBUS_OSPI_CFG_ADDR_DTR_STR(val)                          (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_ADDR_DTR_STR_SHIFT)) & HYPERBUS_OSPI_CFG_ADDR_DTR_STR_MASK)

/* Octo SPI data ddr mode or single mode :
  - 1'b0: DTR mode
  - 1'b1: STR mode */
#define HYPERBUS_OSPI_CFG_DATA_DTR_STR_MASK                          (0x4000)
#define HYPERBUS_OSPI_CFG_DATA_DTR_STR_SHIFT                         (14)
#define HYPERBUS_OSPI_CFG_DATA_DTR_STR(val)                          (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_DATA_DTR_STR_SHIFT)) & HYPERBUS_OSPI_CFG_DATA_DTR_STR_MASK)

/* Octo SPI data ddr mode data MSB:
  - 1'b0: LSB
  - 1'b1: MSB */
#define HYPERBUS_OSPI_CFG_DATA_DTR_MSB_MASK                          (0x8000)
#define HYPERBUS_OSPI_CFG_DATA_DTR_MSB_SHIFT                         (15)
#define HYPERBUS_OSPI_CFG_DATA_DTR_MSB(val)                          (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CFG_DATA_DTR_MSB_SHIFT)) & HYPERBUS_OSPI_CFG_DATA_DTR_MSB_MASK)


/*! @name OSPI_CSN */
/* Octo SPI chip select index :
  - 1'b0: CSN0
  - 1'b1: CSN1 */
#define HYPERBUS_OSPI_CSN_INDEX_MASK                                 (0x1)
#define HYPERBUS_OSPI_CSN_INDEX_SHIFT                                (0)
#define HYPERBUS_OSPI_CSN_INDEX(val)                                 (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CSN_INDEX_SHIFT)) & HYPERBUS_OSPI_CSN_INDEX_MASK)

/* Octo SPI chip select controlled by user  :
  - 1'b0: IP control CSN
  - 1'b1: USER control CSN */
#define HYPERBUS_OSPI_CSN_USER_CTRL_CSN_MASK                         (0x10)
#define HYPERBUS_OSPI_CSN_USER_CTRL_CSN_SHIFT                        (4)
#define HYPERBUS_OSPI_CSN_USER_CTRL_CSN(val)                         (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CSN_USER_CTRL_CSN_SHIFT)) & HYPERBUS_OSPI_CSN_USER_CTRL_CSN_MASK)

/* Octo SPI chip select user control value :
  - 1'b0: CSN low
  - 1'b1: CSN high */
#define HYPERBUS_OSPI_CSN_CSN_VAL_MASK                               (0x20)
#define HYPERBUS_OSPI_CSN_CSN_VAL_SHIFT                              (5)
#define HYPERBUS_OSPI_CSN_CSN_VAL(val)                               (((uint32_t)(((uint32_t)(val)) << HYPERBUS_OSPI_CSN_CSN_VAL_SHIFT)) & HYPERBUS_OSPI_CSN_CSN_VAL_MASK)


/*! @name BURST_SIZE */
/* OSPI burst size for transfer size bigger than 1K.
  For example, 4 K transfer will be separated into 4 transfers (BURST_SIZE=1K). */
#define HYPERBUS_BURST_SIZE_BURST_SIZE_MASK                          (0xffffffff)
#define HYPERBUS_BURST_SIZE_BURST_SIZE_SHIFT                         (0)
#define HYPERBUS_BURST_SIZE_BURST_SIZE(val)                          (((uint32_t)(((uint32_t)(val)) << HYPERBUS_BURST_SIZE_BURST_SIZE_SHIFT)) & HYPERBUS_BURST_SIZE_BURST_SIZE_MASK)


/*! @name BURST_ENABLE */
/* OSPI burst mode enable :
  - 1'b0: BURST mode disable
  - 1'b1: BURST mode enable */
#define HYPERBUS_BURST_ENABLE_BURST_ENABLE_MASK                      (0x1)
#define HYPERBUS_BURST_ENABLE_BURST_ENABLE_SHIFT                     (0)
#define HYPERBUS_BURST_ENABLE_BURST_ENABLE(val)                      (((uint32_t)(((uint32_t)(val)) << HYPERBUS_BURST_ENABLE_BURST_ENABLE_SHIFT)) & HYPERBUS_BURST_ENABLE_BURST_ENABLE_MASK)


/*! @name IRQ_EN */
/* Octo SPI interrupt enable control :
  - 1'b0: interrupt disable
  - 1'b1: Interrupt enable  */
#define HYPERBUS_IRQ_EN_EN_MASK                                      (0x1)
#define HYPERBUS_IRQ_EN_EN_SHIFT                                     (0)
#define HYPERBUS_IRQ_EN_EN(val)                                      (((uint32_t)(((uint32_t)(val)) << HYPERBUS_IRQ_EN_EN_SHIFT)) & HYPERBUS_IRQ_EN_EN_MASK)


/*! @name CLK_DIV */
/* Clock divide data, form 0 – 255, frequency divide table is :
  - 8’h0 – IO_FREQUENCY / 1
  - 8’h1 – IO_FREQUENCY / 2
  - 8’h2 – IO_FREQUENCY / 4
  …
   */
#define HYPERBUS_CLK_DIV_DATA_MASK                                   (0xff)
#define HYPERBUS_CLK_DIV_DATA_SHIFT                                  (0)
#define HYPERBUS_CLK_DIV_DATA(val)                                   (((uint32_t)(((uint32_t)(val)) << HYPERBUS_CLK_DIV_DATA_SHIFT)) & HYPERBUS_CLK_DIV_DATA_MASK)

/* Clock divide valid, user can not control.
  Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
   */
#define HYPERBUS_CLK_DIV_VALID_MASK                                  (0x100)
#define HYPERBUS_CLK_DIV_VALID_SHIFT                                 (8)
#define HYPERBUS_CLK_DIV_VALID(val)                                  (((uint32_t)(((uint32_t)(val)) << HYPERBUS_CLK_DIV_VALID_SHIFT)) & HYPERBUS_CLK_DIV_VALID_MASK)


/*! @name STATUS */
/* TX transfer error because of tcsm, write 1 to clear:
  - 1'b0: no error
  - 1'b1: error */
#define HYPERBUS_STATUS_TX_ERROR_MASK                                (0x1)
#define HYPERBUS_STATUS_TX_ERROR_SHIFT                               (0)
#define HYPERBUS_STATUS_TX_ERROR(val)                                (((uint32_t)(((uint32_t)(val)) << HYPERBUS_STATUS_TX_ERROR_SHIFT)) & HYPERBUS_STATUS_TX_ERROR_MASK)

/* RX transfer error because of tcsm, write 1 to clear:
  - 1'b0: no error
  - 1'b1: error */
#define HYPERBUS_STATUS_RX_ERROR_MASK                                (0x2)
#define HYPERBUS_STATUS_RX_ERROR_SHIFT                               (1)
#define HYPERBUS_STATUS_RX_ERROR(val)                                (((uint32_t)(((uint32_t)(val)) << HYPERBUS_STATUS_RX_ERROR_SHIFT)) & HYPERBUS_STATUS_RX_ERROR_MASK)

/* RX TX transfer end flag, can be polling by user, write 1 to clear:
  - 1'b0: not end
  - 1'b1: end */
#define HYPERBUS_STATUS_RX_TX_END_MASK                               (0x4)
#define HYPERBUS_STATUS_RX_TX_END_SHIFT                              (2)
#define HYPERBUS_STATUS_RX_TX_END(val)                               (((uint32_t)(((uint32_t)(val)) << HYPERBUS_STATUS_RX_TX_END_SHIFT)) & HYPERBUS_STATUS_RX_TX_END_MASK)


/*! @name EXT_ADDR */
typedef union
{
    struct
    {
        /* Memory access address bitfield. */
        uint32_t saddr:31;
        /* Register access flag bitfield. */
        uint32_t reg_access:1;
    } field;
    uint32_t word;
} hyperbus_ext_addr_t;

/*! @name TIMING_CFG */
typedef union
{
    struct
    {
        /* Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0.
        When using HyperRAM memory, this bit should be set to the same value
        as the read latency in configuration register of HyperRAM memory.
        For OctoSPI, this is the dummy cycle after ADDRESS stage :
        - 5'b00000: 0 CK
        - 5'b00001: 1 CK
        - 5'b00010: 2 CK
        ...
        - 5'b11111: 31 CK */
        uint32_t latency0:5;
        /* Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1.
        When using HyperRAM memory, this bit should be set to the same value
        as the read latency in configuration register of HyperRAM memory.
        For OctoSPI, this is the dummy cycle after ADDRESS stage :
        - 5'b00000: 0 CK
        - 5'b00001: 1 CK
        - 5'b00010: 2 CK
        ...
        - 5'b11111: 31 CK */
        uint32_t latency1:5;
        /* Some HyperBus devices may require a minimum time between the end
        of a prior transaction and the start of a new access.
        This time is referred to as Read-Write-Recovery time (tRWR).
        The master interface must start driving CS# Low only at a time when
        the CA1 transfer will complete after tRWR is satisfied.
        - 4'b0000: 16 CK
        - 4'b0001: 1 CK
        - 4'b0001: 2 CK
        ...
        - 4'b1111: 15 CK */
        uint32_t rw_recovery:4;
        /* Delay of RWDS for center aligned read:
        - 3'b000: 0 logic delay
        - 3'b001: 1 logic delay
        - 3'b010: 2 logic delay
        …
        - 3'b111: 7 logic delay */
        uint32_t rwds_delay:3;
        /* Auto check for RWDS high or low for additional latency :
        - 1'b0: additional latency no autocheck
        - 1'b1: additional latency autocheck */
        uint32_t additional_latency_autocheck_en:1;
        /* Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer :
        - 14'h0000: 1 CK
        - 14'h0001: 2 CK
        - 14'h0011: 3 CK
        - …
        - 14'h3FFF: 16383 CK */
        uint32_t cs_max:14;
    } field;
    uint32_t word;
} hyperbus_timing_cfg_t;

/*! @name MBA0 */
typedef union
{
    struct
    {
        uint32_t reserved_0:24;
        /* Memory Base Address 0 for both RAM and FLASH bitfield.
        The base address of addressable region to each memory is set up.
        Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0.
        MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.
        - MBA0 < MBA1, if (MBA1 <= EXT_ADDR) CS1 = 0;  else CS0 = 0;
        - MBA0 > MBA1, if (MBA0 <= EXT_ADDR) CS0 = 0;  else CS1 = 0; */
        uint32_t mba0:7;
    } field;
    uint32_t word;
} hyperbus_mba0_t;

/*! @name MBA1 */
typedef union
{
    struct
    {
        uint32_t reserved_0:24;
        /* Memory Base Address  for both RAM and FLASH bitfield.
        The base address of addressable region to each memory is set up.
        Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0.
        MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.
        - MBA0 < MBA1, if (MBA1 <= EXT_ADDR) CSn1 = 0;  else CSn0 = 0;
        - MBA0 > MBA1, if (MBA0 <= EXT_ADDR) CSn0 = 0;  else CSn1 = 0; */
        uint32_t mba1:7;
    } field;
    uint32_t word;
} hyperbus_mba1_t;

/*! @name DEVICE */
typedef union
{
    struct
    {
        /* Device type :
        - 1'b00: Octo/ Single SPI
        - 1'b01: HYPERBUS */
        uint32_t type:1;
        /* When in HYPERBUS mode :
        - 1'b0: HYPERRAM
        - 1'b1: HYPERLASH */
        uint32_t dt0:1;
        /* When in HYPERBUS mode :
        - 1'b0: HYPERRAM
        - 1'b1: HYPERLASH */
        uint32_t dt1:1;
    } field;
    uint32_t word;
} hyperbus_device_t;

/*! @name OSPI_CMD */
typedef union
{
    struct
    {
        /* 2 Bytes SPI command */
        uint32_t cmd:16;
        /* 2 Bytes SPI alternative */
        uint32_t mode:16;
    } field;
    uint32_t word;
} hyperbus_ospi_cmd_t;

/*! @name OSPI_ADDR */
typedef union
{
    struct
    {
        /* 4 Bytes SPI address */
        uint32_t addr:32;
    } field;
    uint32_t word;
} hyperbus_ospi_addr_t;

/*! @name OSPI_CFG */
typedef union
{
    struct
    {
        /* Octo SPI command size :
        - 2’b0: 0 byte
        - 2'b1: 1 byte
        - 2'b2: 2 byte */
        uint32_t cmd_size:2;
        uint32_t reserved_0:2;
        /* Octo SPI address size :
        - 3'b000: 0 byte (Jump ADDRESS stage)
        - 3'b001: 1 byte
        - 3'b010: 2 byte
        - 3'b011: 3 byte
        - 3'b100: 4 byte */
        uint32_t addr_size:3;
        uint32_t reserved_1:1;
        /* Octo SPI line number for Single SPI or Octo SPI:
        - 2'b00: 8 line for Octo SPI
        - 2'b01: 4 line for QUAD SPI (sdio{3:0])
        - 2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) */
        uint32_t line:2;
        uint32_t reserved_2:2;
        /* Octo SPI command ddr mode or single mode :
        - 1'b0: DTR mode
        - 1'b1: STR mode */
        uint32_t cmd_dtr_str:1;
        /* Octo SPI address ddr mode or single mode :
        - 1'b0: DTR mode
        - 1'b1: STR mode */
        uint32_t addr_dtr_str:1;
        /* Octo SPI data ddr mode or single mode :
        - 1'b0: DTR mode
        - 1'b1: STR mode */
        uint32_t data_dtr_str:1;
        /* Octo SPI data ddr mode data MSB:
        - 1'b0: LSB
        - 1'b1: MSB */
        uint32_t data_dtr_msb:1;
    } field;
    uint32_t word;
} hyperbus_ospi_cfg_t;

/*! @name OSPI_CSN */
typedef union
{
    struct
    {
        /* Octo SPI chip select index :
        - 1'b0: CSN0
        - 1'b1: CSN1 */
        uint32_t index:1;
        uint32_t reserved_0:3;
        /* Octo SPI chip select controlled by user  :
        - 1'b0: IP control CSN
        - 1'b1: USER control CSN */
        uint32_t user_ctrl_csn:1;
        /* Octo SPI chip select user control value :
        - 1'b0: CSN low
        - 1'b1: CSN high */
        uint32_t csn_val:1;
    } field;
    uint32_t word;
} hyperbus_ospi_csn_t;

/*! @name BURST_SIZE */
typedef union
{
    struct
    {
        /* OSPI burst size for transfer size bigger than 1K.
        For example, 4 K transfer will be separated into 4 transfers (BURST_SIZE=1K). */
        uint32_t burst_size:32;
    } field;
    uint32_t word;
} hyperbus_burst_size_t;

/*! @name BURST_ENABLE */
typedef union
{
    struct
    {
        /* OSPI burst mode enable :
        - 1'b0: BURST mode disable
        - 1'b1: BURST mode enable */
        uint32_t burst_enable:1;
    } field;
    uint32_t word;
} hyperbus_burst_enable_t;

/*! @name IRQ_EN */
typedef union
{
    struct
    {
        /* Octo SPI interrupt enable control :
        - 1'b0: interrupt disable
        - 1'b1: Interrupt enable  */
        uint32_t en:1;
    } field;
    uint32_t word;
} hyperbus_irq_en_t;

/*! @name CLK_DIV */
typedef union
{
    struct
    {
        /* Clock divide data, form 0 – 255, frequency divide table is :
        - 8’h0 – IO_FREQUENCY / 1
        - 8’h1 – IO_FREQUENCY / 2
        - 8’h2 – IO_FREQUENCY / 4
        …
         */
        uint32_t data:8;
        /* Clock divide valid, user can not control.
        Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
         */
        uint32_t valid:1;
    } field;
    uint32_t word;
} hyperbus_clk_div_t;

/*! @name STATUS */
typedef union
{
    struct
    {
        /* TX transfer error because of tcsm, write 1 to clear:
        - 1'b0: no error
        - 1'b1: error */
        uint32_t tx_error:1;
        /* RX transfer error because of tcsm, write 1 to clear:
        - 1'b0: no error
        - 1'b1: error */
        uint32_t rx_error:1;
        /* RX TX transfer end flag, can be polling by user, write 1 to clear:
        - 1'b0: not end
        - 1'b1: end */
        uint32_t rx_tx_end:1;
    } field;
    uint32_t word;
} hyperbus_status_t;



#endif /* __HYPER_PERIPH_H__ */
