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

#ifndef __SPIM_PERIPH_H__
#define __SPIM_PERIPH_H__

#include "udma_core_cmd_periph.h"
/* ----------------------------------------------------------------------------
   -- SPIM Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** SPIM_Type Register Layout Typedef */
typedef struct spim
{
  udma_core_cmd_t udma_spim; // core channels, cmd type
} __attribute__((packed)) spim_t;


/* ----------------------------------------------------------------------------
   -- SPIM Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name spim_spim_rx_saddr_t */

typedef union {
  struct {
    unsigned int rx_saddr        :32; // Configure pointer to memory buffer: - Read: value of the pointer until transfer is over. Else returns 0 - Write: set Address Pointer to memory buffer start address
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_rx_saddr_t;

/*! @name SPIM_RX_SADDR */
/* Configure pointer to memory buffer:
  - Read: value of the pointer until transfer is over. Else returns 0
  - Write: set Address Pointer to memory buffer start address */
#define SPIM_SPIM_RX_SADDR_RX_SADDR_MASK                             0xffffffff
#define SPIM_SPIM_RX_SADDR_RX_SADDR_SHIFT                            0
#define SPIM_SPIM_RX_SADDR_RX_SADDR(val)                             ((val) << 0)


/*! @name spim_spim_rx_size_t */

typedef union {
  struct {
    unsigned int rx_size         :20; // Buffer size in bytes. (1MBytes maximum) - Read: buffer size left - Write: set buffer size
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_rx_size_t;

/*! @name SPIM_RX_SIZE */
/* Buffer size in bytes. (1MBytes maximum)
  - Read: buffer size left
  - Write: set buffer size */
#define SPIM_SPIM_RX_SIZE_RX_SIZE_MASK                               0xfffff
#define SPIM_SPIM_RX_SIZE_RX_SIZE_SHIFT                              0
#define SPIM_SPIM_RX_SIZE_RX_SIZE(val)                               ((val) << 0)


/*! @name spim_spim_rx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // Channel continuous mode: -1'b0: disable -1'b1: enable At the end of the buffer the uDMA reloads the address and size and starts a new transfer.
    unsigned int datasize        :2 ; // Channel transfer size used to increment uDMA buffer address pointer: - 2'b00: +1 (8 bits) - 2'b01: +2 (16 bits) - 2'b10: +4 (32 bits)(default) - 2'b11: +0
    unsigned int padding0:1 ;
    unsigned int en              :1 ; // Channel enable and start transfer: -1'b0: disable -1'b1: enable This signal is used also to queue a transfer if one is already ongoing.
    unsigned int clr             :1 ; // Channel clear and stop transfer: -1'b0: disable -1'b1: enable
    unsigned int pending         :1 ; // Transfer pending in queue status flag: -1'b0: free -1'b1: pending
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_rx_cfg_t;

/*! @name SPIM_RX_CFG */
/* Channel continuous mode:
  -1'b0: disable
  -1'b1: enable
  At the end of the buffer the uDMA reloads the address and size and starts a new transfer. */
#define SPIM_SPIM_RX_CFG_CONTINOUS_MASK                              0x1
#define SPIM_SPIM_RX_CFG_CONTINOUS_SHIFT                             0
#define SPIM_SPIM_RX_CFG_CONTINOUS(val)                              ((val) << 0)

/* Channel transfer size used to increment uDMA buffer address pointer:
  - 2'b00: +1 (8 bits)
  - 2'b01: +2 (16 bits)
  - 2'b10: +4 (32 bits)(default)
  - 2'b11: +0 */
#define SPIM_SPIM_RX_CFG_DATASIZE_MASK                               0x6
#define SPIM_SPIM_RX_CFG_DATASIZE_SHIFT                              1
#define SPIM_SPIM_RX_CFG_DATASIZE(val)                               ((val) << 1)

/* Channel enable and start transfer:
  -1'b0: disable
  -1'b1: enable
  This signal is used also to queue a transfer if one is already ongoing. */
#define SPIM_SPIM_RX_CFG_EN_MASK                                     0x10
#define SPIM_SPIM_RX_CFG_EN_SHIFT                                    4
#define SPIM_SPIM_RX_CFG_EN(val)                                     ((val) << 4)

/* Channel clear and stop transfer:
  -1'b0: disable
  -1'b1: enable */
#define SPIM_SPIM_RX_CFG_CLR_MASK                                    0x20
#define SPIM_SPIM_RX_CFG_CLR_SHIFT                                   5
#define SPIM_SPIM_RX_CFG_CLR(val)                                    ((val) << 5)

/* Transfer pending in queue status flag:
  -1'b0: free
  -1'b1: pending */
#define SPIM_SPIM_RX_CFG_PENDING_MASK                                0x20
#define SPIM_SPIM_RX_CFG_PENDING_SHIFT                               5
#define SPIM_SPIM_RX_CFG_PENDING(val)                                ((val) << 5)


/*! @name spim_spim_rx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_rx_initcfg_t;

/*! @name SPIM_RX_INITCFG */

/*! @name spim_spim_tx_saddr_t */

typedef union {
  struct {
    unsigned int tx_saddr        :32; // Configure pointer to memory buffer: - Read: value of the pointer until transfer is over. Else returns 0 - Write: set Address Pointer to memory buffer start address
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_tx_saddr_t;

/*! @name SPIM_TX_SADDR */
/* Configure pointer to memory buffer:
  - Read: value of the pointer until transfer is over. Else returns 0
  - Write: set Address Pointer to memory buffer start address */
#define SPIM_SPIM_TX_SADDR_TX_SADDR_MASK                             0xffffffff
#define SPIM_SPIM_TX_SADDR_TX_SADDR_SHIFT                            0
#define SPIM_SPIM_TX_SADDR_TX_SADDR(val)                             ((val) << 0)


/*! @name spim_spim_tx_size_t */

typedef union {
  struct {
    unsigned int tx_size         :20; // Buffer size in bytes. (1MBytes maximum) - Read: buffer size left - Write: set buffer size
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_tx_size_t;

/*! @name SPIM_TX_SIZE */
/* Buffer size in bytes. (1MBytes maximum)
  - Read: buffer size left
  - Write: set buffer size */
#define SPIM_SPIM_TX_SIZE_TX_SIZE_MASK                               0xfffff
#define SPIM_SPIM_TX_SIZE_TX_SIZE_SHIFT                              0
#define SPIM_SPIM_TX_SIZE_TX_SIZE(val)                               ((val) << 0)


/*! @name spim_spim_tx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // Channel continuous mode: -1'b0: disable -1'b1: enable At the end of the buffer the uDMA reloads the address and size and starts a new transfer.
    unsigned int datasize        :2 ; // Channel transfer size used to increment uDMA buffer address pointer: - 2'b00: +1 (8 bits) - 2'b01: +2 (16 bits) - 2'b10: +4 (32 bits)(default) - 2'b11: +0
    unsigned int padding0:1 ;
    unsigned int en              :1 ; // Channel enable and start transfer: -1'b0: disable -1'b1: enable This signal is used also to queue a transfer if one is already ongoing.
    unsigned int clr             :1 ; // Channel clear and stop transfer: -1'b0: disable -1'b1: enable
    unsigned int pending         :1 ; // Transfer pending in queue status flag: -1'b0: free -1'b1: pending
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_tx_cfg_t;

/*! @name SPIM_TX_CFG */
/* Channel continuous mode:
  -1'b0: disable
  -1'b1: enable
  At the end of the buffer the uDMA reloads the address and size and starts a new transfer. */
#define SPIM_SPIM_TX_CFG_CONTINOUS_MASK                              0x1
#define SPIM_SPIM_TX_CFG_CONTINOUS_SHIFT                             0
#define SPIM_SPIM_TX_CFG_CONTINOUS(val)                              ((val) << 0)

/* Channel transfer size used to increment uDMA buffer address pointer:
  - 2'b00: +1 (8 bits)
  - 2'b01: +2 (16 bits)
  - 2'b10: +4 (32 bits)(default)
  - 2'b11: +0 */
#define SPIM_SPIM_TX_CFG_DATASIZE_MASK                               0x6
#define SPIM_SPIM_TX_CFG_DATASIZE_SHIFT                              1
#define SPIM_SPIM_TX_CFG_DATASIZE(val)                               ((val) << 1)

/* Channel enable and start transfer:
  -1'b0: disable
  -1'b1: enable
  This signal is used also to queue a transfer if one is already ongoing. */
#define SPIM_SPIM_TX_CFG_EN_MASK                                     0x10
#define SPIM_SPIM_TX_CFG_EN_SHIFT                                    4
#define SPIM_SPIM_TX_CFG_EN(val)                                     ((val) << 4)

/* Channel clear and stop transfer:
  -1'b0: disable
  -1'b1: enable */
#define SPIM_SPIM_TX_CFG_CLR_MASK                                    0x20
#define SPIM_SPIM_TX_CFG_CLR_SHIFT                                   5
#define SPIM_SPIM_TX_CFG_CLR(val)                                    ((val) << 5)

/* Transfer pending in queue status flag:
  -1'b0: free
  -1'b1: pending */
#define SPIM_SPIM_TX_CFG_PENDING_MASK                                0x20
#define SPIM_SPIM_TX_CFG_PENDING_SHIFT                               5
#define SPIM_SPIM_TX_CFG_PENDING(val)                                ((val) << 5)


/*! @name spim_spim_tx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_tx_initcfg_t;

/*! @name SPIM_TX_INITCFG */

/*! @name spim_spim_cmd_saddr_t */

typedef union {
  struct {
    unsigned int cmd_saddr       :32; // Configure pointer to memory buffer: - Read: value of the pointer until transfer is over. Else returns 0 - Write: set Address Pointer to memory buffer start address
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_cmd_saddr_t;

/*! @name SPIM_CMD_SADDR */
/* Configure pointer to memory buffer:
  - Read: value of the pointer until transfer is over. Else returns 0
  - Write: set Address Pointer to memory buffer start address */
#define SPIM_SPIM_CMD_SADDR_CMD_SADDR_MASK                           0xffffffff
#define SPIM_SPIM_CMD_SADDR_CMD_SADDR_SHIFT                          0
#define SPIM_SPIM_CMD_SADDR_CMD_SADDR(val)                           ((val) << 0)


/*! @name spim_spim_cmd_size_t */

typedef union {
  struct {
    unsigned int cmd_size        :20; // Buffer size in bytes. (1MBytes maximum) - Read: buffer size left - Write: set buffer size
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_cmd_size_t;

/*! @name SPIM_CMD_SIZE */
/* Buffer size in bytes. (1MBytes maximum)
  - Read: buffer size left
  - Write: set buffer size */
#define SPIM_SPIM_CMD_SIZE_CMD_SIZE_MASK                             0xfffff
#define SPIM_SPIM_CMD_SIZE_CMD_SIZE_SHIFT                            0
#define SPIM_SPIM_CMD_SIZE_CMD_SIZE(val)                             ((val) << 0)


/*! @name spim_spim_cmd_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // Channel continuous mode: -1'b0: disable -1'b1: enable At the end of the buffer the uDMA reloads the address and size and starts a new transfer.
    unsigned int datasize        :2 ; // Channel transfer size used to increment uDMA buffer address pointer: - 2'b00: +1 (8 bits) - 2'b01: +2 (16 bits) - 2'b10: +4 (32 bits)(default) - 2'b11: +0
    unsigned int padding0:1 ;
    unsigned int en              :1 ; // Channel enable and start transfer: -1'b0: disable -1'b1: enable This signal is used also to queue a transfer if one is already ongoing.
    unsigned int clr             :1 ; // Channel clear and stop transfer: -1'b0: disable -1'b1: enable
    unsigned int pending         :1 ; // Transfer pending in queue status flag: -1'b0: free -1'b1: pending
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_cmd_cfg_t;

/*! @name SPIM_CMD_CFG */
/* Channel continuous mode:
  -1'b0: disable
  -1'b1: enable
  At the end of the buffer the uDMA reloads the address and size and starts a new transfer. */
#define SPIM_SPIM_CMD_CFG_CONTINOUS_MASK                             0x1
#define SPIM_SPIM_CMD_CFG_CONTINOUS_SHIFT                            0
#define SPIM_SPIM_CMD_CFG_CONTINOUS(val)                             ((val) << 0)

/* Channel transfer size used to increment uDMA buffer address pointer:
  - 2'b00: +1 (8 bits)
  - 2'b01: +2 (16 bits)
  - 2'b10: +4 (32 bits)(default)
  - 2'b11: +0 */
#define SPIM_SPIM_CMD_CFG_DATASIZE_MASK                              0x6
#define SPIM_SPIM_CMD_CFG_DATASIZE_SHIFT                             1
#define SPIM_SPIM_CMD_CFG_DATASIZE(val)                              ((val) << 1)

/* Channel enable and start transfer:
  -1'b0: disable
  -1'b1: enable
  This signal is used also to queue a transfer if one is already ongoing. */
#define SPIM_SPIM_CMD_CFG_EN_MASK                                    0x10
#define SPIM_SPIM_CMD_CFG_EN_SHIFT                                   4
#define SPIM_SPIM_CMD_CFG_EN(val)                                    ((val) << 4)

/* Channel clear and stop transfer:
  -1'b0: disable
  -1'b1: enable */
#define SPIM_SPIM_CMD_CFG_CLR_MASK                                   0x20
#define SPIM_SPIM_CMD_CFG_CLR_SHIFT                                  5
#define SPIM_SPIM_CMD_CFG_CLR(val)                                   ((val) << 5)

/* Transfer pending in queue status flag:
  -1'b0: free
  -1'b1: pending */
#define SPIM_SPIM_CMD_CFG_PENDING_MASK                               0x20
#define SPIM_SPIM_CMD_CFG_PENDING_SHIFT                              5
#define SPIM_SPIM_CMD_CFG_PENDING(val)                               ((val) << 5)


/*! @name spim_spim_cmd_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) spim_spim_cmd_initcfg_t;

/*! @name SPIM_CMD_INITCFG */

/* ----------------------------------------------------------------------------

 CMD IDs and macros

----------------------------------------------------------------------------*/
#define SPI_CMD_CFG(clkdiv,cpha,cpol) ((((uint32_t)clkdiv) << 0x0UL)|(((uint32_t)cpha) << 0x8UL)|(((uint32_t)cpol) << 0x9UL)|(0x0UL << 0x1cUL))
#define SPI_CMD_SOT(cs) ((((uint32_t)cs) << 0x0UL)|(0x1UL << 0x1cUL))
#define SPI_CMD_SEND_CMD(data_value,data_size,lsb,qpi) ((((uint32_t)data_value) << 0x0UL)|(((uint32_t)data_size) << 0x10UL)|(((uint32_t)lsb) << 0x1aUL)|(((uint32_t)qpi) << 0x1bUL)|(0x2UL << 0x1cUL))
#define SPI_CMD_DUMMY(dummy_cycle) ((((uint32_t)dummy_cycle) << 0x10UL)|(0x4UL << 0x1cUL))
#define SPI_CMD_WAIT(event_id_cycle_count,wait_type) ((((uint32_t)event_id_cycle_count) << 0x0UL)|(((uint32_t)wait_type) << 0x8UL)|(0x5UL << 0x1cUL))
#define SPI_CMD_TX_DATA(word_num,word_size,word_per_transf,lsb,qpi) ((((uint32_t)word_num) << 0x0UL)|(((uint32_t)word_size) << 0x10UL)|(((uint32_t)word_per_transf) << 0x15UL)|(((uint32_t)lsb) << 0x1aUL)|(((uint32_t)qpi) << 0x1bUL)|(0x6UL << 0x1cUL))
#define SPI_CMD_RX_DATA(word_num,word_size,word_per_transf,lsb,qpi) ((((uint32_t)word_num) << 0x0UL)|(((uint32_t)word_size) << 0x10UL)|(((uint32_t)word_per_transf) << 0x15UL)|(((uint32_t)lsb) << 0x1aUL)|(((uint32_t)qpi) << 0x1bUL)|(0x7UL << 0x1cUL))
#define SPI_CMD_RPT(rpt_cnt) ((((uint32_t)rpt_cnt) << 0x0UL)|(0x8UL << 0x1cUL))
#define SPI_CMD_EOT(event_gen) ((((uint32_t)event_gen) << 0x0UL)|(0x9UL << 0x1cUL))
#define SPI_CMD_RPT_END() ((0xaUL << 0x1cUL))
#define SPI_CMD_RX_CHECK(comp_data,status_size,check_type,lsb,qpi) ((((uint32_t)comp_data) << 0x0UL)|(((uint32_t)status_size) << 0x10UL)|(((uint32_t)check_type) << 0x18UL)|(((uint32_t)lsb) << 0x1aUL)|(((uint32_t)qpi) << 0x1bUL)|(0xbUL << 0x1cUL))
#define SPI_CMD_FULL_DUPL(data_size,lsb) ((((uint32_t)data_size) << 0x0UL)|(((uint32_t)lsb) << 0x1aUL)|(0xcUL << 0x1cUL))
#define SPI_CMD_SETUP_UCA(start_addr,word_per_transf,tx_rxn) ((((uint32_t)start_addr) << 0x0UL)|(0xdUL << 0x1cUL)|(((uint32_t)word_per_transf) << 0x19UL)|(((uint32_t)tx_rxn) << 0x1bUL))
#define SPI_CMD_SETUP_UCS(size,word_per_transf,tx_rxn) ((((uint32_t)size) << 0x0UL)|(((uint32_t)word_per_transf) << 0x19UL)|(((uint32_t)tx_rxn) << 0x1bUL)|(0xeUL << 0x1cUL))

#endif /* __SPIM_PERIPH_H__ */
