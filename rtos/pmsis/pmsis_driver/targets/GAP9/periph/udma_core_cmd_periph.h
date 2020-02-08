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

#ifndef __UDMA_CORE_CMD_PERIPH_H__
#define __UDMA_CORE_CMD_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- UDMA_CORE_CMD Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** UDMA_CORE_CMD_Type Register Layout Typedef */
typedef struct udma_core_cmd
{
  volatile uint32_t rx_saddr; /**< uDMA RX buffer base address configuration register. */
  volatile uint32_t rx_size; /**< uDMA RX buffer size configuration register. */
  volatile uint32_t rx_cfg; /**< uDMA RX stream configuration register. */
  volatile uint32_t rx_initcfg; /**< nan */
  volatile uint32_t tx_saddr; /**< uDMA TX  buffer base address configuration register. */
  volatile uint32_t tx_size; /**< uDMA TX  buffer size configuration register. */
  volatile uint32_t tx_cfg; /**< uDMA TX  stream configuration register. */
  volatile uint32_t tx_initcfg; /**< nan */
  volatile uint32_t cmd_saddr; /**< uDMA CMD  buffer base address configuration register. */
  volatile uint32_t cmd_size; /**< uDMA CMD  buffer size configuration register. */
  volatile uint32_t cmd_cfg; /**< uDMA CMD  stream configuration register. */
  volatile uint32_t cmd_initcfg; /**< nan */
} __attribute__((packed)) udma_core_cmd_t;


/* ----------------------------------------------------------------------------
   -- UDMA_CORE_CMD Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name udma_core_cmd_rx_saddr_t */

typedef union {
  struct {
    unsigned int rx_saddr        :21; // RX buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets RX buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_rx_saddr_t;

/*! @name RX_SADDR */
/* RX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets RX buffer base address */
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR_MASK                         0x1fffff
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR_SHIFT                        0
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR(val)                         ((val) << 0)


/*! @name udma_core_cmd_rx_size_t */

typedef union {
  struct {
    unsigned int rx_size         :20; // RX buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_rx_size_t;

/*! @name RX_SIZE */
/* RX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE_MASK                           0xfffff
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE_SHIFT                          0
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE(val)                           ((val) << 0)


/*! @name udma_core_cmd_rx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // RX channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // RX channel enable and start transfer bitfield: -1'b0: disable -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // RX transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // RX channel clear and stop transfer: -1'b0: disable -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_rx_cfg_t;

/*! @name RX_CFG */
/* RX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS_MASK                          0x1
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS_SHIFT                         0
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS(val)                          ((val) << 0)

/* RX channel enable and start transfer bitfield:
  -1'b0: disable
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_RX_CFG_EN_MASK                                 0x10
#define UDMA_CORE_CMD_RX_CFG_EN_SHIFT                                4
#define UDMA_CORE_CMD_RX_CFG_EN(val)                                 ((val) << 4)

/* RX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_RX_CFG_PENDING_MASK                            0x20
#define UDMA_CORE_CMD_RX_CFG_PENDING_SHIFT                           5
#define UDMA_CORE_CMD_RX_CFG_PENDING(val)                            ((val) << 5)

/* RX channel clear and stop transfer:
  -1'b0: disable
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_RX_CFG_CLR_MASK                                0x40
#define UDMA_CORE_CMD_RX_CFG_CLR_SHIFT                               6
#define UDMA_CORE_CMD_RX_CFG_CLR(val)                                ((val) << 6)


/*! @name udma_core_cmd_rx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_rx_initcfg_t;

/*! @name RX_INITCFG */

/*! @name udma_core_cmd_tx_saddr_t */

typedef union {
  struct {
    unsigned int tx_saddr        :21; // TX buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_tx_saddr_t;

/*! @name TX_SADDR */
/* TX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR_MASK                         0x1fffff
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR_SHIFT                        0
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR(val)                         ((val) << 0)


/*! @name udma_core_cmd_tx_size_t */

typedef union {
  struct {
    unsigned int tx_size         :20; // TX buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_tx_size_t;

/*! @name TX_SIZE */
/* TX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE_MASK                           0xfffff
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE_SHIFT                          0
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE(val)                           ((val) << 0)


/*! @name udma_core_cmd_tx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // TX channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // TX channel enable and start transfer bitfield: -1'b0: disabled -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // TX transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // TX channel clear and stop transfer bitfield: -1'b0: disabled -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_tx_cfg_t;

/*! @name TX_CFG */
/* TX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS_MASK                          0x1
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS_SHIFT                         0
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS(val)                          ((val) << 0)

/* TX channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_TX_CFG_EN_MASK                                 0x10
#define UDMA_CORE_CMD_TX_CFG_EN_SHIFT                                4
#define UDMA_CORE_CMD_TX_CFG_EN(val)                                 ((val) << 4)

/* TX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_TX_CFG_PENDING_MASK                            0x20
#define UDMA_CORE_CMD_TX_CFG_PENDING_SHIFT                           5
#define UDMA_CORE_CMD_TX_CFG_PENDING(val)                            ((val) << 5)

/* TX channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_TX_CFG_CLR_MASK                                0x40
#define UDMA_CORE_CMD_TX_CFG_CLR_SHIFT                               6
#define UDMA_CORE_CMD_TX_CFG_CLR(val)                                ((val) << 6)


/*! @name udma_core_cmd_tx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_tx_initcfg_t;

/*! @name TX_INITCFG */

/*! @name udma_core_cmd_cmd_saddr_t */

typedef union {
  struct {
    unsigned int cmd_saddr       :21; // CMD buffer base address bitfield: - Read: returns value of the buffer pointer until transfer is finished. Else returns 0. - Write: sets buffer base address
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_cmd_saddr_t;

/*! @name CMD_SADDR */
/* CMD buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_MASK                       0x1fffff
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_SHIFT                      0
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR(val)                       ((val) << 0)


/*! @name udma_core_cmd_cmd_size_t */

typedef union {
  struct {
    unsigned int cmd_size        :20; // CMD buffer size bitfield in bytes. (128kBytes maximum) - Read: returns remaining buffer size to transfer. - Write: sets buffer size.
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_cmd_size_t;

/*! @name CMD_SIZE */
/* CMD buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_MASK                         0xfffff
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_SHIFT                        0
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE(val)                         ((val) << 0)


/*! @name udma_core_cmd_cmd_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // CMD channel continuous mode bitfield: -1'b0: disabled -1'b1: enabled At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer.
    unsigned int padding0:3 ;
    unsigned int en              :1 ; // CMD channel enable and start transfer bitfield: -1'b0: disabled -1'b1: enable and start the transfer This signal is used also to queue a transfer if one is already ongoing.
    unsigned int pending         :1 ; // CMD transfer pending in queue status flag: -1'b0: no pending transfer in the queue -1'b1: pending transfer in the queue
    unsigned int clr             :1 ; // CMD channel clear and stop transfer bitfield: -1'b0: disabled -1'b1: stop and clear the on-going transfer
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_cmd_cfg_t;

/*! @name CMD_CFG */
/* CMD channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS_MASK                         0x1
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS_SHIFT                        0
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS(val)                         ((val) << 0)

/* CMD channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_CMD_CFG_EN_MASK                                0x10
#define UDMA_CORE_CMD_CMD_CFG_EN_SHIFT                               4
#define UDMA_CORE_CMD_CMD_CFG_EN(val)                                ((val) << 4)

/* CMD transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_CMD_CFG_PENDING_MASK                           0x20
#define UDMA_CORE_CMD_CMD_CFG_PENDING_SHIFT                          5
#define UDMA_CORE_CMD_CMD_CFG_PENDING(val)                           ((val) << 5)

/* CMD channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_CMD_CFG_CLR_MASK                               0x40
#define UDMA_CORE_CMD_CMD_CFG_CLR_SHIFT                              6
#define UDMA_CORE_CMD_CMD_CFG_CLR(val)                               ((val) << 6)


/*! @name udma_core_cmd_cmd_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) udma_core_cmd_cmd_initcfg_t;

/*! @name CMD_INITCFG */

/* ----------------------------------------------------------------------------

 CMD IDs and macros

----------------------------------------------------------------------------*/

#endif /* __UDMA_CORE_CMD_PERIPH_H__ */
