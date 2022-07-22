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

#ifndef __UDMA_CORE_CMD_PERIPH_H__
#define __UDMA_CORE_CMD_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- UDMA_CORE_CMD Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** UDMA_CORE_CMD_Type Register Layout Typedef */
typedef struct
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
} udma_core_cmd_t;


/* ----------------------------------------------------------------------------
   -- UDMA_CORE_CMD Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name RX_SADDR */
/* RX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets RX buffer base address */
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR_MASK                         (0x1fffff)
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR_SHIFT                        (0)
#define UDMA_CORE_CMD_RX_SADDR_RX_SADDR(val)                         (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_SADDR_RX_SADDR_SHIFT)) & UDMA_CORE_CMD_RX_SADDR_RX_SADDR_MASK)


/*! @name RX_SIZE */
/* RX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE_MASK                           (0xfffff)
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE_SHIFT                          (0)
#define UDMA_CORE_CMD_RX_SIZE_RX_SIZE(val)                           (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_SIZE_RX_SIZE_SHIFT)) & UDMA_CORE_CMD_RX_SIZE_RX_SIZE_MASK)


/*! @name RX_CFG */
/* RX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS_MASK                          (0x1)
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS_SHIFT                         (0)
#define UDMA_CORE_CMD_RX_CFG_CONTINOUS(val)                          (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_CFG_CONTINOUS_SHIFT)) & UDMA_CORE_CMD_RX_CFG_CONTINOUS_MASK)

/* RX channel enable and start transfer bitfield:
  -1'b0: disable
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_RX_CFG_EN_MASK                                 (0x10)
#define UDMA_CORE_CMD_RX_CFG_EN_SHIFT                                (4)
#define UDMA_CORE_CMD_RX_CFG_EN(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_CFG_EN_SHIFT)) & UDMA_CORE_CMD_RX_CFG_EN_MASK)

/* RX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_RX_CFG_PENDING_MASK                            (0x20)
#define UDMA_CORE_CMD_RX_CFG_PENDING_SHIFT                           (5)
#define UDMA_CORE_CMD_RX_CFG_PENDING(val)                            (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_CFG_PENDING_SHIFT)) & UDMA_CORE_CMD_RX_CFG_PENDING_MASK)

/* RX channel clear and stop transfer:
  -1'b0: disable
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_RX_CFG_CLR_MASK                                (0x40)
#define UDMA_CORE_CMD_RX_CFG_CLR_SHIFT                               (6)
#define UDMA_CORE_CMD_RX_CFG_CLR(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_RX_CFG_CLR_SHIFT)) & UDMA_CORE_CMD_RX_CFG_CLR_MASK)


/*! @name RX_INITCFG */

/*! @name TX_SADDR */
/* TX buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR_MASK                         (0x1fffff)
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR_SHIFT                        (0)
#define UDMA_CORE_CMD_TX_SADDR_TX_SADDR(val)                         (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_SADDR_TX_SADDR_SHIFT)) & UDMA_CORE_CMD_TX_SADDR_TX_SADDR_MASK)


/*! @name TX_SIZE */
/* TX buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE_MASK                           (0xfffff)
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE_SHIFT                          (0)
#define UDMA_CORE_CMD_TX_SIZE_TX_SIZE(val)                           (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_SIZE_TX_SIZE_SHIFT)) & UDMA_CORE_CMD_TX_SIZE_TX_SIZE_MASK)


/*! @name TX_CFG */
/* TX channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS_MASK                          (0x1)
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS_SHIFT                         (0)
#define UDMA_CORE_CMD_TX_CFG_CONTINOUS(val)                          (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_CFG_CONTINOUS_SHIFT)) & UDMA_CORE_CMD_TX_CFG_CONTINOUS_MASK)

/* TX channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_TX_CFG_EN_MASK                                 (0x10)
#define UDMA_CORE_CMD_TX_CFG_EN_SHIFT                                (4)
#define UDMA_CORE_CMD_TX_CFG_EN(val)                                 (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_CFG_EN_SHIFT)) & UDMA_CORE_CMD_TX_CFG_EN_MASK)

/* TX transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_TX_CFG_PENDING_MASK                            (0x20)
#define UDMA_CORE_CMD_TX_CFG_PENDING_SHIFT                           (5)
#define UDMA_CORE_CMD_TX_CFG_PENDING(val)                            (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_CFG_PENDING_SHIFT)) & UDMA_CORE_CMD_TX_CFG_PENDING_MASK)

/* TX channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_TX_CFG_CLR_MASK                                (0x40)
#define UDMA_CORE_CMD_TX_CFG_CLR_SHIFT                               (6)
#define UDMA_CORE_CMD_TX_CFG_CLR(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_TX_CFG_CLR_SHIFT)) & UDMA_CORE_CMD_TX_CFG_CLR_MASK)


/*! @name TX_INITCFG */

/*! @name CMD_SADDR */
/* CMD buffer base address bitfield:
  - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
  - Write: sets buffer base address */
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_MASK                       (0x1fffff)
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_SHIFT                      (0)
#define UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR(val)                       (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_SHIFT)) & UDMA_CORE_CMD_CMD_SADDR_CMD_SADDR_MASK)


/*! @name CMD_SIZE */
/* CMD buffer size bitfield in bytes. (128kBytes maximum)
  - Read: returns remaining buffer size to transfer.
  - Write: sets buffer size. */
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_MASK                         (0xfffff)
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_SHIFT                        (0)
#define UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE(val)                         (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_SHIFT)) & UDMA_CORE_CMD_CMD_SIZE_CMD_SIZE_MASK)


/*! @name CMD_CFG */
/* CMD channel continuous mode bitfield:
  -1'b0: disabled
  -1'b1: enabled
  At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS_MASK                         (0x1)
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS_SHIFT                        (0)
#define UDMA_CORE_CMD_CMD_CFG_CONTINOUS(val)                         (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_CFG_CONTINOUS_SHIFT)) & UDMA_CORE_CMD_CMD_CFG_CONTINOUS_MASK)

/* CMD channel enable and start transfer bitfield:
  -1'b0: disabled
  -1'b1: enable and start the transfer
  This signal is used also to queue a transfer if one is already ongoing. */
#define UDMA_CORE_CMD_CMD_CFG_EN_MASK                                (0x10)
#define UDMA_CORE_CMD_CMD_CFG_EN_SHIFT                               (4)
#define UDMA_CORE_CMD_CMD_CFG_EN(val)                                (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_CFG_EN_SHIFT)) & UDMA_CORE_CMD_CMD_CFG_EN_MASK)

/* CMD transfer pending in queue status flag:
  -1'b0: no pending transfer in the queue
  -1'b1: pending transfer in the queue */
#define UDMA_CORE_CMD_CMD_CFG_PENDING_MASK                           (0x20)
#define UDMA_CORE_CMD_CMD_CFG_PENDING_SHIFT                          (5)
#define UDMA_CORE_CMD_CMD_CFG_PENDING(val)                           (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_CFG_PENDING_SHIFT)) & UDMA_CORE_CMD_CMD_CFG_PENDING_MASK)

/* CMD channel clear and stop transfer bitfield:
  -1'b0: disabled
  -1'b1: stop and clear the on-going transfer */
#define UDMA_CORE_CMD_CMD_CFG_CLR_MASK                               (0x40)
#define UDMA_CORE_CMD_CMD_CFG_CLR_SHIFT                              (6)
#define UDMA_CORE_CMD_CMD_CFG_CLR(val)                               (((uint32_t)(((uint32_t)(val)) << UDMA_CORE_CMD_CMD_CFG_CLR_SHIFT)) & UDMA_CORE_CMD_CMD_CFG_CLR_MASK)


/*! @name CMD_INITCFG */

/*! @name RX_SADDR */
typedef union
{
    struct
    {
        /* RX buffer base address bitfield:
        - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
        - Write: sets RX buffer base address */
        uint32_t rx_saddr:21;
    } field;
    uint32_t word;
} udma_core_cmd_rx_saddr_t;

/*! @name RX_SIZE */
typedef union
{
    struct
    {
        /* RX buffer size bitfield in bytes. (128kBytes maximum)
        - Read: returns remaining buffer size to transfer.
        - Write: sets buffer size. */
        uint32_t rx_size:20;
    } field;
    uint32_t word;
} udma_core_cmd_rx_size_t;

/*! @name RX_CFG */
typedef union
{
    struct
    {
        /* RX channel continuous mode bitfield:
        -1'b0: disabled
        -1'b1: enabled
        At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
        uint32_t continous:1;
        uint32_t reserved_0:3;
        /* RX channel enable and start transfer bitfield:
        -1'b0: disable
        -1'b1: enable and start the transfer
        This signal is used also to queue a transfer if one is already ongoing. */
        uint32_t en:1;
        /* RX transfer pending in queue status flag:
        -1'b0: no pending transfer in the queue
        -1'b1: pending transfer in the queue */
        uint32_t pending:1;
        /* RX channel clear and stop transfer:
        -1'b0: disable
        -1'b1: stop and clear the on-going transfer */
        uint32_t clr:1;
    } field;
    uint32_t word;
} udma_core_cmd_rx_cfg_t;

/*! @name RX_INITCFG */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} udma_core_cmd_rx_initcfg_t;

/*! @name TX_SADDR */
typedef union
{
    struct
    {
        /* TX buffer base address bitfield:
        - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
        - Write: sets buffer base address */
        uint32_t tx_saddr:21;
    } field;
    uint32_t word;
} udma_core_cmd_tx_saddr_t;

/*! @name TX_SIZE */
typedef union
{
    struct
    {
        /* TX buffer size bitfield in bytes. (128kBytes maximum)
        - Read: returns remaining buffer size to transfer.
        - Write: sets buffer size. */
        uint32_t tx_size:20;
    } field;
    uint32_t word;
} udma_core_cmd_tx_size_t;

/*! @name TX_CFG */
typedef union
{
    struct
    {
        /* TX channel continuous mode bitfield:
        -1'b0: disabled
        -1'b1: enabled
        At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
        uint32_t continous:1;
        uint32_t reserved_0:3;
        /* TX channel enable and start transfer bitfield:
        -1'b0: disabled
        -1'b1: enable and start the transfer
        This signal is used also to queue a transfer if one is already ongoing. */
        uint32_t en:1;
        /* TX transfer pending in queue status flag:
        -1'b0: no pending transfer in the queue
        -1'b1: pending transfer in the queue */
        uint32_t pending:1;
        /* TX channel clear and stop transfer bitfield:
        -1'b0: disabled
        -1'b1: stop and clear the on-going transfer */
        uint32_t clr:1;
    } field;
    uint32_t word;
} udma_core_cmd_tx_cfg_t;

/*! @name TX_INITCFG */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} udma_core_cmd_tx_initcfg_t;

/*! @name CMD_SADDR */
typedef union
{
    struct
    {
        /* CMD buffer base address bitfield:
        - Read: returns value of the buffer pointer until transfer is finished. Else returns 0.
        - Write: sets buffer base address */
        uint32_t cmd_saddr:21;
    } field;
    uint32_t word;
} udma_core_cmd_cmd_saddr_t;

/*! @name CMD_SIZE */
typedef union
{
    struct
    {
        /* CMD buffer size bitfield in bytes. (128kBytes maximum)
        - Read: returns remaining buffer size to transfer.
        - Write: sets buffer size. */
        uint32_t cmd_size:20;
    } field;
    uint32_t word;
} udma_core_cmd_cmd_size_t;

/*! @name CMD_CFG */
typedef union
{
    struct
    {
        /* CMD channel continuous mode bitfield:
        -1'b0: disabled
        -1'b1: enabled
        At the end of the buffer transfer, the uDMA reloads the address / buffer size and starts a new transfer. */
        uint32_t continous:1;
        uint32_t reserved_0:3;
        /* CMD channel enable and start transfer bitfield:
        -1'b0: disabled
        -1'b1: enable and start the transfer
        This signal is used also to queue a transfer if one is already ongoing. */
        uint32_t en:1;
        /* CMD transfer pending in queue status flag:
        -1'b0: no pending transfer in the queue
        -1'b1: pending transfer in the queue */
        uint32_t pending:1;
        /* CMD channel clear and stop transfer bitfield:
        -1'b0: disabled
        -1'b1: stop and clear the on-going transfer */
        uint32_t clr:1;
    } field;
    uint32_t word;
} udma_core_cmd_cmd_cfg_t;

/*! @name CMD_INITCFG */
typedef union
{
    struct
    {
    } field;
    uint32_t word;
} udma_core_cmd_cmd_initcfg_t;


#define UDMA_CORE_CFG_DATA_SIZE_MASK              (0x6U)
#define UDMA_CORE_CFG_DATA_SIZE_SHIFT             (1U)
#define UDMA_CORE_CFG_DATA_SIZE(x)                (((uint32_t)(((uint32_t)(x)) << UDMA_CORE_CFG_DATA_SIZE_SHIFT)) & UDMA_CORE_CFG_DATA_SIZE_MASK)

#define UDMA_CORE_CFG_DATASIZE_8                                     (0x0)
#define UDMA_CORE_CFG_DATASIZE_16                                    (0x1)
#define UDMA_CORE_CFG_DATASIZE_32                                    (0x2)

#endif /* __UDMA_CORE_CMD_PERIPH_H__ */
