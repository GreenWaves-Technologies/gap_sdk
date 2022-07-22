/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2021 ETH Zurich, University of Bologna
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

#ifndef __UDMA_I2C_PERIPH_H__
#define __UDMA_I2C_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** I2C_Type Register Layout Typedef */
typedef struct
{
    udma_core_cmd_t udma; /**< UDMA CMD channels struct. */
    volatile uint32_t status; /**< uDMA I2C Status register. */
    volatile uint32_t setup; /**< uDMA I2C Configuration register. */
} i2c_t;


/* ----------------------------------------------------------------------------
   -- I2C Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name STATUS */
/* I2C bus busy status flag:
  - 1'b0: no transfer on-going
  - 1'b1: transfer on-going */
#define I2C_STATUS_BUSY_MASK                                         (0x1)
#define I2C_STATUS_BUSY_SHIFT                                        (0)
#define I2C_STATUS_BUSY(val)                                         (((uint32_t)(((uint32_t)(val)) << I2C_STATUS_BUSY_SHIFT)) & I2C_STATUS_BUSY_MASK)

/* I2C arbitration lost status flag:
  - 1'b0: no error
  - 1'b1: arbitration lost error */
#define I2C_STATUS_ARB_LOST_MASK                                     (0x2)
#define I2C_STATUS_ARB_LOST_SHIFT                                    (1)
#define I2C_STATUS_ARB_LOST(val)                                     (((uint32_t)(((uint32_t)(val)) << I2C_STATUS_ARB_LOST_SHIFT)) & I2C_STATUS_ARB_LOST_MASK)

/* I2C ack flag, can be polling for busy:
  - 1'b0: ACK
  - 1'b1: NAK */
#define I2C_STATUS_ACK_MASK                                          (0x4)
#define I2C_STATUS_ACK_SHIFT                                         (2)
#define I2C_STATUS_ACK(val)                                          (((uint32_t)(((uint32_t)(val)) << I2C_STATUS_ACK_SHIFT)) & I2C_STATUS_ACK_MASK)


/*! @name SETUP */
/* Reset command used to abort the on-going transfer and clear busy and arbitration lost status flags. */
#define I2C_SETUP_DO_RST_MASK                                        (0x1)
#define I2C_SETUP_DO_RST_SHIFT                                       (0)
#define I2C_SETUP_DO_RST(val)                                        (((uint32_t)(((uint32_t)(val)) << I2C_SETUP_DO_RST_SHIFT)) & I2C_SETUP_DO_RST_MASK)


/*! @name STATUS */
typedef union
{
    struct
    {
        /* I2C bus busy status flag:
        - 1'b0: no transfer on-going
        - 1'b1: transfer on-going */
        uint32_t busy:1;
        /* I2C arbitration lost status flag:
        - 1'b0: no error
        - 1'b1: arbitration lost error */
        uint32_t arb_lost:1;
        /* I2C ack flag, can be polling for busy:
        - 1'b0: ACK
        - 1'b1: NAK */
        uint32_t ack:1;
    } field;
    uint32_t word;
} i2c_status_t;

/*! @name SETUP */
typedef union
{
    struct
    {
        /* Reset command used to abort the on-going transfer and clear busy and arbitration lost status flags. */
        uint32_t do_rst:1;
    } field;
    uint32_t word;
} i2c_setup_t;


/* ----------------------------------------------------------------------------
   -- I2C CMD IDs and macros --
   ---------------------------------------------------------------------------- */

#define I2C_CMD_START        0x0 /* I2C Start of Transfer command.. */
#define I2C_CMD_STOP         0x2 /* I2C End of Transfer command.. */
#define I2C_CMD_RD_ACK       0x4 /* I2C receive data and acknowledge command.. */
#define I2C_CMD_RD_NACK      0x6 /* I2C receive data and not acknowledge command.. */
#define I2C_CMD_WR           0x8 /* I2C send data and wait acknowledge command.. */
#define I2C_CMD_WAIT         0xA /* I2C wait dummy cycles command.. */
#define I2C_CMD_RPT          0xC /* I2C next command repeat command.. */
#define I2C_CMD_CFG          0xE /* I2C configuration command.. */
#define I2C_CMD_WAIT_EV      0x1 /* I2C wait uDMA external event command.. */
#define I2C_CMD_WRB          0x7 /* I2C write byte command. */
#define I2C_CMD_EOT          0x9 /* Signal end of transfer (next transfer may be en-queued). */
#define I2C_CMD_SETUP_UCA    0x3 /* Setup RX or TX channel start address. */
#define I2C_CMD_SETUP_UCS    0x5 /* Setup RX or TX channel transfer size+enable channel. */

#define I2C_I2C_CMD_START() ((0x0 << I2C_CMD_START))
#define I2C_I2C_CMD_STOP() ((0x2 << I2C_CMD_STOP))
#define I2C_I2C_CMD_RD_ACK() ((0x4 << I2C_CMD_RD_ACK))
#define I2C_I2C_CMD_RD_NACK() ((0x6 << I2C_CMD_RD_NACK))
#define I2C_I2C_CMD_WR() ((0x8 << I2C_CMD_WR))
#define I2C_I2C_CMD_WAIT(dummy_cycles) ((0xA << I2C_CMD_WAIT)|(dummy_cycles << 0))
#define I2C_I2C_CMD_RPT(rpt_nb) ((0xC << I2C_CMD_RPT)|(rpt_nb << 0))
#define I2C_I2C_CMD_CFG(cfg) ((0xE << I2C_CMD_CFG)|(cfg << 0))
#define I2C_I2C_CMD_WAIT_EV() ((0x1 << I2C_CMD_WAIT_EV))
#define I2C_I2C_CMD_WRB(data_byte) ((0x7 << I2C_CMD_WRB)|(data_byte << 0))
#define I2C_I2C_CMD_EOT() ((0x9 << I2C_CMD_EOT))
#define I2C_I2C_CMD_SETUP_UCA(txrxn,saddr) ((0x3 << I2C_CMD_SETUP_UCA)|(txrxn << 27)|(saddr << 0))
#define I2C_I2C_CMD_SETUP_UCS(txrxn,size) ((0x5 << I2C_CMD_SETUP_UCS)|(txrxn << 27)|(size << 0))


#endif /* __UDMA_I2C_PERIPH_H__ */
