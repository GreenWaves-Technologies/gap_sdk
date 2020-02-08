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

#ifndef __I2C_PERIPH_H__
#define __I2C_PERIPH_H__

#include "udma_core_cmd_periph.h"
/* ----------------------------------------------------------------------------
   -- I2C Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** I2C_Type Register Layout Typedef */
typedef struct i2c
{
  udma_core_cmd_t udma_i2c; // core channels, cmd type
  volatile uint32_t status; /**< uDMA I2C Status register. */
  volatile uint32_t setup; /**< uDMA I2C Configuration register. */
} __attribute__((packed)) i2c_t;


/* ----------------------------------------------------------------------------
   -- I2C Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name i2c_rx_saddr_t */

typedef union {
  struct {
    unsigned int rx_saddr        :21; // RX buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets RX buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) i2c_rx_saddr_t;

/*! @name RX_SADDR */
/* RX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets RX buffer base address */
#define I2C_RX_SADDR_RX_SADDR_MASK                                   0x1fffff
#define I2C_RX_SADDR_RX_SADDR_SHIFT                                  0
#define I2C_RX_SADDR_RX_SADDR(val)                                   ((val) << 0)


/*! @name i2c_rx_size_t */

typedef union {
  struct {
    unsigned int rx_size         :20; // RX buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) i2c_rx_size_t;

/*! @name RX_SIZE */
/* RX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define I2C_RX_SIZE_RX_SIZE_MASK                                     0xfffff
#define I2C_RX_SIZE_RX_SIZE_SHIFT                                    0
#define I2C_RX_SIZE_RX_SIZE(val)                                     ((val) << 0)


/*! @name i2c_rx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // RX channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // RX channel enable and start transfer bitfield: -1'b0: disable -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // RX transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // RX channel clear and stop transfer: -1'b0: disable -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) i2c_rx_cfg_t;

/*! @name RX_CFG */
/* RX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define I2C_RX_CFG_CONTINOUS_MASK                                    0x1
#define I2C_RX_CFG_CONTINOUS_SHIFT                                   0
#define I2C_RX_CFG_CONTINOUS(val)                                    ((val) << 0)

/* RX channel enable and start transfer bitfield:
  -1'b0: disable
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define I2C_RX_CFG_EN_MASK                                           0x10
#define I2C_RX_CFG_EN_SHIFT                                          4
#define I2C_RX_CFG_EN(val)                                           ((val) << 4)

/* RX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define I2C_RX_CFG_PENDING_MASK                                      0x20
#define I2C_RX_CFG_PENDING_SHIFT                                     5
#define I2C_RX_CFG_PENDING(val)                                      ((val) << 5)

/* RX channel clear and stop transfer:
  -1'b0: disable
  -1'b1: stop and clear the on-going transfer */
#define I2C_RX_CFG_CLR_MASK                                          0x40
#define I2C_RX_CFG_CLR_SHIFT                                         6
#define I2C_RX_CFG_CLR(val)                                          ((val) << 6)


/*! @name i2c_rx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i2c_rx_initcfg_t;

/*! @name RX_INITCFG */

/*! @name i2c_tx_saddr_t */

typedef union {
  struct {
    unsigned int tx_saddr        :21; // TX buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) i2c_tx_saddr_t;

/*! @name TX_SADDR */
/* TX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define I2C_TX_SADDR_TX_SADDR_MASK                                   0x1fffff
#define I2C_TX_SADDR_TX_SADDR_SHIFT                                  0
#define I2C_TX_SADDR_TX_SADDR(val)                                   ((val) << 0)


/*! @name i2c_tx_size_t */

typedef union {
  struct {
    unsigned int tx_size         :20; // TX buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) i2c_tx_size_t;

/*! @name TX_SIZE */
/* TX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define I2C_TX_SIZE_TX_SIZE_MASK                                     0xfffff
#define I2C_TX_SIZE_TX_SIZE_SHIFT                                    0
#define I2C_TX_SIZE_TX_SIZE(val)                                     ((val) << 0)


/*! @name i2c_tx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // TX channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // TX channel enable and start transfer bitfield: -1'b0: disabled -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // TX transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // TX channel clear and stop transfer bitfield: -1'b0: disabled -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) i2c_tx_cfg_t;

/*! @name TX_CFG */
/* TX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define I2C_TX_CFG_CONTINOUS_MASK                                    0x1
#define I2C_TX_CFG_CONTINOUS_SHIFT                                   0
#define I2C_TX_CFG_CONTINOUS(val)                                    ((val) << 0)

/* TX channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define I2C_TX_CFG_EN_MASK                                           0x10
#define I2C_TX_CFG_EN_SHIFT                                          4
#define I2C_TX_CFG_EN(val)                                           ((val) << 4)

/* TX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define I2C_TX_CFG_PENDING_MASK                                      0x20
#define I2C_TX_CFG_PENDING_SHIFT                                     5
#define I2C_TX_CFG_PENDING(val)                                      ((val) << 5)

/* TX channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define I2C_TX_CFG_CLR_MASK                                          0x40
#define I2C_TX_CFG_CLR_SHIFT                                         6
#define I2C_TX_CFG_CLR(val)                                          ((val) << 6)


/*! @name i2c_tx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i2c_tx_initcfg_t;

/*! @name TX_INITCFG */

/*! @name i2c_cmd_saddr_t */

typedef union {
  struct {
    unsigned int cmd_saddr       :21; // CMD buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) i2c_cmd_saddr_t;

/*! @name CMD_SADDR */
/* CMD buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define I2C_CMD_SADDR_CMD_SADDR_MASK                                 0x1fffff
#define I2C_CMD_SADDR_CMD_SADDR_SHIFT                                0
#define I2C_CMD_SADDR_CMD_SADDR(val)                                 ((val) << 0)


/*! @name i2c_cmd_size_t */

typedef union {
  struct {
    unsigned int cmd_size        :20; // CMD buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) i2c_cmd_size_t;

/*! @name CMD_SIZE */
/* CMD buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define I2C_CMD_SIZE_CMD_SIZE_MASK                                   0xfffff
#define I2C_CMD_SIZE_CMD_SIZE_SHIFT                                  0
#define I2C_CMD_SIZE_CMD_SIZE(val)                                   ((val) << 0)


/*! @name i2c_cmd_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // CMD channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // CMD channel enable and start transfer bitfield: -1'b0: disabled -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // CMD transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // CMD channel clear and stop transfer bitfield: -1'b0: disabled -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) i2c_cmd_cfg_t;

/*! @name CMD_CFG */
/* CMD channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define I2C_CMD_CFG_CONTINOUS_MASK                                   0x1
#define I2C_CMD_CFG_CONTINOUS_SHIFT                                  0
#define I2C_CMD_CFG_CONTINOUS(val)                                   ((val) << 0)

/* CMD channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define I2C_CMD_CFG_EN_MASK                                          0x10
#define I2C_CMD_CFG_EN_SHIFT                                         4
#define I2C_CMD_CFG_EN(val)                                          ((val) << 4)

/* CMD transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define I2C_CMD_CFG_PENDING_MASK                                     0x20
#define I2C_CMD_CFG_PENDING_SHIFT                                    5
#define I2C_CMD_CFG_PENDING(val)                                     ((val) << 5)

/* CMD channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define I2C_CMD_CFG_CLR_MASK                                         0x40
#define I2C_CMD_CFG_CLR_SHIFT                                        6
#define I2C_CMD_CFG_CLR(val)                                         ((val) << 6)


/*! @name i2c_cmd_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) i2c_cmd_initcfg_t;

/*! @name CMD_INITCFG */

/*! @name i2c_status_t */

typedef union {
  struct {
    unsigned int busy            :1 ; // I2C bus busy status flag: - 1'b0: no transfer on-going - 1'b1: transfer on-going
    unsigned int arb_lost        :1 ; // I2C arbitration lost status flag: - 1'b0: no error - 1'b1: arbitration lost error
  };
  unsigned int raw;
} __attribute__((packed)) i2c_status_t;

/*! @name STATUS */
/* I2C bus busy status flag:
  - 1'b0: no transfer on-going
  - 1'b1: transfer on-going */
#define I2C_STATUS_BUSY_MASK                                         0x1
#define I2C_STATUS_BUSY_SHIFT                                        0
#define I2C_STATUS_BUSY(val)                                         ((val) << 0)

/* I2C arbitration lost status flag:
  - 1'b0: no error
  - 1'b1: arbitration lost error */
#define I2C_STATUS_ARB_LOST_MASK                                     0x2
#define I2C_STATUS_ARB_LOST_SHIFT                                    1
#define I2C_STATUS_ARB_LOST(val)                                     ((val) << 1)


/*! @name i2c_setup_t */

typedef union {
  struct {
    unsigned int do_rst          :1 ; // Reset command used to abort the on-going transfer and clear busy and arbitration lost status flags.
  };
  unsigned int raw;
} __attribute__((packed)) i2c_setup_t;

/*! @name SETUP */
/* Reset command used to abort the on-going transfer and clear busy and arbitration lost status flags. */
#define I2C_SETUP_DO_RST_MASK                                        0x1
#define I2C_SETUP_DO_RST_SHIFT                                       0
#define I2C_SETUP_DO_RST(val)                                        ((val) << 0)


/* ----------------------------------------------------------------------------

 CMD IDs and macros

----------------------------------------------------------------------------*/
#define I2C_CMD_START() ((0x0UL << 0x1cUL))
#define I2C_CMD_STOP() ((0x2UL << 0x1cUL))
#define I2C_CMD_RD_ACK() ((0x4UL << 0x1cUL))
#define I2C_CMD_RD_NACK() ((0x6UL << 0x1cUL))
#define I2C_CMD_WR() ((0x8UL << 0x1cUL))
#define I2C_CMD_WAIT(dummy_cycles) ((0xaUL << 0x1cUL)|(((uint32_t)dummy_cycles) << 0x0))
#define I2C_CMD_RPT(rpt_nb) ((0xcUL << 0x1cUL)|(((uint32_t)rpt_nb) << 0x0))
#define I2C_CMD_CFG(cfg) ((0xeUL << 0x1cUL)|(((uint32_t)cfg) << 0x0))
#define I2C_CMD_WAIT_EV(event) ((0x1UL << 0x1cUL) | ((event&0x3)<<24))
#define I2C_CMD_WRB(data_byte) ((0x7UL << 0x1cUL)|(((uint32_t)data_byte) << 0x0))
#define I2C_CMD_EOT() ((0x9UL << 0x1cUL))
#define I2C_CMD_SETUP_UCA(txrxn,saddr) ((0x3UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)saddr) << 0x0))
#define I2C_CMD_SETUP_UCS(txrxn,size) ((0x5UL << 0x1cUL)|(((uint32_t)txrxn) << 0x1b)|(((uint32_t)size) << 0x0))

#endif /* __I2C_PERIPH_H__ */
