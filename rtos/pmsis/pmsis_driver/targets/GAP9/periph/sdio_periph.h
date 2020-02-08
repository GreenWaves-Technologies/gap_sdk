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

#ifndef __SDIO_PERIPH_H__
#define __SDIO_PERIPH_H__

#include "udma_core_periph.h"
/* ----------------------------------------------------------------------------
   -- SDIO Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** SDIO_Type Register Layout Typedef */
typedef struct sdio
{
  udma_core_t udma_sdio; // core channels, 2 channels type
  volatile uint32_t cmd_op; /**< SDIO command */
  volatile uint32_t cmd_arg; /**< SDIO argument */
  volatile uint32_t data_setup; /**< Data transfer setup */
  volatile uint32_t start; /**< Start */
  volatile uint32_t rsp0; /**< Response byte0 */
  volatile uint32_t rsp1; /**< Response byte1 */
  volatile uint32_t rsp2; /**< Response byte2 */
  volatile uint32_t rsp3; /**< Response byte3 */
  volatile uint32_t clk_div; /**< Clock Divider */
  volatile uint32_t status; /**< STATUS */
  volatile uint32_t stopcmd_op; /**< SDIO STOP command op */
  volatile uint32_t stopcmd_arg; /**< SDIO STOP command arg */
} __attribute__((packed)) sdio_t;


/* ----------------------------------------------------------------------------
   -- SDIO Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name sdio_rx_saddr_t */

typedef union {
  struct {
    unsigned int rx_saddr        :32; // Configure pointer to memory buffer: - Read: value of the pointer until transfer is over. Else returns 0 - Write: set Address Pointer to memory buffer start address
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rx_saddr_t;

/*! @name RX_SADDR */
/* Configure pointer to memory buffer:
  - Read: value of the pointer until transfer is over. Else returns 0
  - Write: set Address Pointer to memory buffer start address */
#define SDIO_RX_SADDR_RX_SADDR_MASK                                  0xffffffff
#define SDIO_RX_SADDR_RX_SADDR_SHIFT                                 0
#define SDIO_RX_SADDR_RX_SADDR(val)                                  ((val) << 0)


/*! @name sdio_rx_size_t */

typedef union {
  struct {
    unsigned int rx_size         :20; // Buffer size in bytes. (1MBytes maximum) - Read: buffer size left - Write: set buffer size
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rx_size_t;

/*! @name RX_SIZE */
/* Buffer size in bytes. (1MBytes maximum)
  - Read: buffer size left
  - Write: set buffer size */
#define SDIO_RX_SIZE_RX_SIZE_MASK                                    0xfffff
#define SDIO_RX_SIZE_RX_SIZE_SHIFT                                   0
#define SDIO_RX_SIZE_RX_SIZE(val)                                    ((val) << 0)


/*! @name sdio_rx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // Channel continuous mode: -1'b0: disable -1'b1: enable At the end of the buffer the uDMA reloads the address and size and starts a new transfer.
    unsigned int datasize        :2 ; // Channel transfer size used to increment uDMA buffer address pointer: - 2'b00: +1 (8 bits) - 2'b01: +2 (16 bits) - 2'b10: +4 (32 bits)(default) - 2'b11: +0
    unsigned int padding0:1 ;
    unsigned int en              :1 ; // Channel enable and start transfer: -1'b0: disable -1'b1: enable This signal is used also to queue a transfer if one is already ongoing.
    unsigned int padding1:1 ;
    unsigned int clr             :1 ; // Channel clear and stop transfer: -1'b0: disable -1'b1: enable
    unsigned int pending         :1 ; // Transfer pending in queue status flag: -1'b0: free -1'b1: pending
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rx_cfg_t;

/*! @name RX_CFG */
/* Channel continuous mode:
  -1'b0: disable
  -1'b1: enable
  At the end of the buffer the uDMA reloads the address and size and starts a new transfer. */
#define SDIO_RX_CFG_CONTINOUS_MASK                                   0x1
#define SDIO_RX_CFG_CONTINOUS_SHIFT                                  0
#define SDIO_RX_CFG_CONTINOUS(val)                                   ((val) << 0)

/* Channel transfer size used to increment uDMA buffer address pointer:
  - 2'b00: +1 (8 bits)
  - 2'b01: +2 (16 bits)
  - 2'b10: +4 (32 bits)(default)
  - 2'b11: +0 */
#define SDIO_RX_CFG_DATASIZE_MASK                                    0x6
#define SDIO_RX_CFG_DATASIZE_SHIFT                                   1
#define SDIO_RX_CFG_DATASIZE(val)                                    ((val) << 1)

/* Channel enable and start transfer:
  -1'b0: disable
  -1'b1: enable
  This signal is used also to queue a transfer if one is already ongoing. */
#define SDIO_RX_CFG_EN_MASK                                          0x10
#define SDIO_RX_CFG_EN_SHIFT                                         4
#define SDIO_RX_CFG_EN(val)                                          ((val) << 4)

/* Channel clear and stop transfer:
  -1'b0: disable
  -1'b1: enable */
#define SDIO_RX_CFG_CLR_MASK                                         0x40
#define SDIO_RX_CFG_CLR_SHIFT                                        6
#define SDIO_RX_CFG_CLR(val)                                         ((val) << 6)

/* Transfer pending in queue status flag:
  -1'b0: free
  -1'b1: pending */
#define SDIO_RX_CFG_PENDING_MASK                                     0x20
#define SDIO_RX_CFG_PENDING_SHIFT                                    5
#define SDIO_RX_CFG_PENDING(val)                                     ((val) << 5)


/*! @name sdio_rx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rx_initcfg_t;

/*! @name RX_INITCFG */

/*! @name sdio_tx_saddr_t */

typedef union {
  struct {
    unsigned int tx_saddr        :32; // Configure pointer to memory buffer: - Read: value of the pointer until transfer is over. Else returns 0 - Write: set Address Pointer to memory buffer start address
  };
  unsigned int raw;
} __attribute__((packed)) sdio_tx_saddr_t;

/*! @name TX_SADDR */
/* Configure pointer to memory buffer:
  - Read: value of the pointer until transfer is over. Else returns 0
  - Write: set Address Pointer to memory buffer start address */
#define SDIO_TX_SADDR_TX_SADDR_MASK                                  0xffffffff
#define SDIO_TX_SADDR_TX_SADDR_SHIFT                                 0
#define SDIO_TX_SADDR_TX_SADDR(val)                                  ((val) << 0)


/*! @name sdio_tx_size_t */

typedef union {
  struct {
    unsigned int tx_size         :20; // Buffer size in bytes. (1MBytes maximum) - Read: buffer size left - Write: set buffer size
  };
  unsigned int raw;
} __attribute__((packed)) sdio_tx_size_t;

/*! @name TX_SIZE */
/* Buffer size in bytes. (1MBytes maximum)
  - Read: buffer size left
  - Write: set buffer size */
#define SDIO_TX_SIZE_TX_SIZE_MASK                                    0xfffff
#define SDIO_TX_SIZE_TX_SIZE_SHIFT                                   0
#define SDIO_TX_SIZE_TX_SIZE(val)                                    ((val) << 0)


/*! @name sdio_tx_cfg_t */

typedef union {
  struct {
    unsigned int continous       :1 ; // Channel continuous mode: -1'b0: disable -1'b1: enable At the end of the buffer the uDMA reloads the address and size and starts a new transfer.
    unsigned int datasize        :2 ; // Channel transfer size used to increment uDMA buffer address pointer: - 2'b00: +1 (8 bits) - 2'b01: +2 (16 bits) - 2'b10: +4 (32 bits)(default) - 2'b11: +0
    unsigned int padding0:1 ;
    unsigned int en              :1 ; // Channel enable and start transfer: -1'b0: disable -1'b1: enable This signal is used also to queue a transfer if one is already ongoing.
    unsigned int padding1:1 ;
    unsigned int clr             :1 ; // Channel clear and stop transfer: -1'b0: disable -1'b1: enable
    unsigned int pending         :1 ; // Transfer pending in queue status flag: -1'b0: free -1'b1: pending
  };
  unsigned int raw;
} __attribute__((packed)) sdio_tx_cfg_t;

/*! @name TX_CFG */
/* Channel continuous mode:
  -1'b0: disable
  -1'b1: enable
  At the end of the buffer the uDMA reloads the address and size and starts a new transfer. */
#define SDIO_TX_CFG_CONTINOUS_MASK                                   0x1
#define SDIO_TX_CFG_CONTINOUS_SHIFT                                  0
#define SDIO_TX_CFG_CONTINOUS(val)                                   ((val) << 0)

/* Channel transfer size used to increment uDMA buffer address pointer:
  - 2'b00: +1 (8 bits)
  - 2'b01: +2 (16 bits)
  - 2'b10: +4 (32 bits)(default)
  - 2'b11: +0 */
#define SDIO_TX_CFG_DATASIZE_MASK                                    0x6
#define SDIO_TX_CFG_DATASIZE_SHIFT                                   1
#define SDIO_TX_CFG_DATASIZE(val)                                    ((val) << 1)

/* Channel enable and start transfer:
  -1'b0: disable
  -1'b1: enable
  This signal is used also to queue a transfer if one is already ongoing. */
#define SDIO_TX_CFG_EN_MASK                                          0x10
#define SDIO_TX_CFG_EN_SHIFT                                         4
#define SDIO_TX_CFG_EN(val)                                          ((val) << 4)

/* Channel clear and stop transfer:
  -1'b0: disable
  -1'b1: enable */
#define SDIO_TX_CFG_CLR_MASK                                         0x40
#define SDIO_TX_CFG_CLR_SHIFT                                        6
#define SDIO_TX_CFG_CLR(val)                                         ((val) << 6)

/* Transfer pending in queue status flag:
  -1'b0: free
  -1'b1: pending */
#define SDIO_TX_CFG_PENDING_MASK                                     0x20
#define SDIO_TX_CFG_PENDING_SHIFT                                    5
#define SDIO_TX_CFG_PENDING(val)                                     ((val) << 5)


/*! @name sdio_tx_initcfg_t */

typedef union {
  struct {
  };
  unsigned int raw;
} __attribute__((packed)) sdio_tx_initcfg_t;

/*! @name TX_INITCFG */

/*! @name sdio_cmd_op_t */

typedef union {
  struct {
    unsigned int cmd_rsp_type    :3 ; // Response type: -3’b000: No Responce -3’b001: 48 bits with CRC -3’b010: 48 bits no CRC -3’b011: 136bits -3’b100: 48 bits with BUSY check
    unsigned int padding0:5 ;
    unsigned int cmd_op          :6 ; // SDIO command opcode
  };
  unsigned int raw;
} __attribute__((packed)) sdio_cmd_op_t;

/*! @name CMD_OP */
/* Response type:
  -3’b000: No Responce
  -3’b001: 48 bits with CRC
  -3’b010: 48 bits no CRC
  -3’b011: 136bits
  -3’b100: 48 bits with BUSY check */
#define SDIO_CMD_OP_CMD_RSP_TYPE_MASK                                0x7
#define SDIO_CMD_OP_CMD_RSP_TYPE_SHIFT                               0
#define SDIO_CMD_OP_CMD_RSP_TYPE(val)                                ((val) << 0)

/* SDIO command opcode */
#define SDIO_CMD_OP_CMD_OP_MASK                                      0x3f00
#define SDIO_CMD_OP_CMD_OP_SHIFT                                     8
#define SDIO_CMD_OP_CMD_OP(val)                                      ((val) << 8)


/*! @name sdio_cmd_arg_t */

typedef union {
  struct {
    unsigned int cmd_arg         :32; // Argument to be sent with the command
  };
  unsigned int raw;
} __attribute__((packed)) sdio_cmd_arg_t;

/*! @name CMD_ARG */
/* Argument to be sent with the command */
#define SDIO_CMD_ARG_CMD_ARG_MASK                                    0xffffffff
#define SDIO_CMD_ARG_CMD_ARG_SHIFT                                   0
#define SDIO_CMD_ARG_CMD_ARG(val)                                    ((val) << 0)


/*! @name sdio_data_setup_t */

typedef union {
  struct {
    unsigned int data_en         :1 ; // Enables Data transfer for current command
    unsigned int data_rwn        :1 ; // Selects the direction of transfer: -1'b0: write -1'b1: read
    unsigned int data_quad       :1 ; // Enables QUAD mode
    unsigned int padding0:5 ;
    unsigned int block_num       :8 ; // Sets the number of blocks to be sent
    unsigned int block_size      :10; // Sets the block size
  };
  unsigned int raw;
} __attribute__((packed)) sdio_data_setup_t;

/*! @name DATA_SETUP */
/* Enables Data transfer for current command */
#define SDIO_DATA_SETUP_DATA_EN_MASK                                 0x1
#define SDIO_DATA_SETUP_DATA_EN_SHIFT                                0
#define SDIO_DATA_SETUP_DATA_EN(val)                                 ((val) << 0)

/* Selects the direction of transfer:
  -1'b0: write
  -1'b1: read */
#define SDIO_DATA_SETUP_DATA_RWN_MASK                                0x2
#define SDIO_DATA_SETUP_DATA_RWN_SHIFT                               1
#define SDIO_DATA_SETUP_DATA_RWN(val)                                ((val) << 1)

/* Enables QUAD mode */
#define SDIO_DATA_SETUP_DATA_QUAD_MASK                               0x4
#define SDIO_DATA_SETUP_DATA_QUAD_SHIFT                              2
#define SDIO_DATA_SETUP_DATA_QUAD(val)                               ((val) << 2)

/* Sets the number of blocks to be sent */
#define SDIO_DATA_SETUP_BLOCK_NUM_MASK                               0xff00
#define SDIO_DATA_SETUP_BLOCK_NUM_SHIFT                              8
#define SDIO_DATA_SETUP_BLOCK_NUM(val)                               ((val) << 8)

/* Sets the block size */
#define SDIO_DATA_SETUP_BLOCK_SIZE_MASK                              0x3ff0000
#define SDIO_DATA_SETUP_BLOCK_SIZE_SHIFT                             16
#define SDIO_DATA_SETUP_BLOCK_SIZE(val)                              ((val) << 16)


/*! @name sdio_start_t */

typedef union {
  struct {
    unsigned int start           :1 ; // Starts the SDIO transfer
  };
  unsigned int raw;
} __attribute__((packed)) sdio_start_t;

/*! @name START */
/* Starts the SDIO transfer */
#define SDIO_START_START_MASK                                        0x1
#define SDIO_START_START_SHIFT                                       0
#define SDIO_START_START(val)                                        ((val) << 0)


/*! @name sdio_rsp0_t */

typedef union {
  struct {
    unsigned int rsp0            :32; // Bytes0..3 of response
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rsp0_t;

/*! @name RSP0 */
/* Bytes0..3 of response */
#define SDIO_RSP0_RSP0_MASK                                          0xffffffff
#define SDIO_RSP0_RSP0_SHIFT                                         0
#define SDIO_RSP0_RSP0(val)                                          ((val) << 0)


/*! @name sdio_rsp1_t */

typedef union {
  struct {
    unsigned int rsp1            :32; // Bytes4..7 of response
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rsp1_t;

/*! @name RSP1 */
/* Bytes4..7 of response */
#define SDIO_RSP1_RSP1_MASK                                          0xffffffff
#define SDIO_RSP1_RSP1_SHIFT                                         0
#define SDIO_RSP1_RSP1(val)                                          ((val) << 0)


/*! @name sdio_rsp2_t */

typedef union {
  struct {
    unsigned int rsp2            :32; // Bytes8..11 of response
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rsp2_t;

/*! @name RSP2 */
/* Bytes8..11 of response */
#define SDIO_RSP2_RSP2_MASK                                          0xffffffff
#define SDIO_RSP2_RSP2_SHIFT                                         0
#define SDIO_RSP2_RSP2(val)                                          ((val) << 0)


/*! @name sdio_rsp3_t */

typedef union {
  struct {
    unsigned int rsp3            :32; // Bytes12..15 of response
  };
  unsigned int raw;
} __attribute__((packed)) sdio_rsp3_t;

/*! @name RSP3 */
/* Bytes12..15 of response */
#define SDIO_RSP3_RSP3_MASK                                          0xffffffff
#define SDIO_RSP3_RSP3_SHIFT                                         0
#define SDIO_RSP3_RSP3(val)                                          ((val) << 0)


/*! @name sdio_clk_div_t */

typedef union {
  struct {
    unsigned int clk_div         :9 ; // Clock diviser
  };
  unsigned int raw;
} __attribute__((packed)) sdio_clk_div_t;

/*! @name CLK_DIV */
/* Clock diviser */
#define SDIO_CLK_DIV_CLK_DIV_MASK                                    0x1ff
#define SDIO_CLK_DIV_CLK_DIV_SHIFT                                   0
#define SDIO_CLK_DIV_CLK_DIV(val)                                    ((val) << 0)


/*! @name sdio_status_t */

typedef union {
  struct {
    unsigned int eot             :1 ; // Indicate the end of the transfer of  command or data : -1'b0: not end -1'b1: end
    unsigned int error           :1 ; // Indicate the error of the transfer of  command or data : -1'b0: no error -1'b1: error
    unsigned int padding0:14;
    unsigned int cmd_err_status  :6 ; // Indicate the error status of the transfer of  command : -6’b00000: no error -6'b00001: Response Time Out -6'b00010: Response Wrong Direction -6'b00100: Response Busy Timeout
    unsigned int padding1:2 ;
    unsigned int data_err_status :6 ; // Indicate the error status of the transfer of  data : -6’b00000: no error -6'b00001: Response Time Out
  };
  unsigned int raw;
} __attribute__((packed)) sdio_status_t;

/*! @name STATUS */
/* Indicate the end of the transfer of  command or data :
  -1'b0: not end
  -1'b1: end */
#define SDIO_STATUS_EOT_MASK                                         0x1
#define SDIO_STATUS_EOT_SHIFT                                        0
#define SDIO_STATUS_EOT(val)                                         ((val) << 0)

/* Indicate the error of the transfer of  command or data :
  -1'b0: no error
  -1'b1: error */
#define SDIO_STATUS_ERROR_MASK                                       0x2
#define SDIO_STATUS_ERROR_SHIFT                                      1
#define SDIO_STATUS_ERROR(val)                                       ((val) << 1)

/* Indicate the error status of the transfer of  command :
  -6’b00000: no error
  -6'b00001: Response Time Out
  -6'b00010: Response Wrong Direction
  -6'b00100: Response Busy Timeout */
#define SDIO_STATUS_CMD_ERR_STATUS_MASK                              0x3f0000
#define SDIO_STATUS_CMD_ERR_STATUS_SHIFT                             16
#define SDIO_STATUS_CMD_ERR_STATUS(val)                              ((val) << 16)

/* Indicate the error status of the transfer of  data :
  -6’b00000: no error
  -6'b00001: Response Time Out */
#define SDIO_STATUS_DATA_ERR_STATUS_MASK                             0x3f000000
#define SDIO_STATUS_DATA_ERR_STATUS_SHIFT                            24
#define SDIO_STATUS_DATA_ERR_STATUS(val)                             ((val) << 24)


/*! @name sdio_stopcmd_op_t */

typedef union {
  struct {
    unsigned int padding0:8 ;
    unsigned int stopcmd_op      :6 ; // SDIO STOP command opcode
    unsigned int stopcmd_rsp_type:3 ; // SDIO STOP command response type: -3’b000: No Responce -3’b001: 48 bits with CRC -3’b010: 48 bits no CRC -3’b011: 136bits -3’b100: 48 bits with BUSY check
  };
  unsigned int raw;
} __attribute__((packed)) sdio_stopcmd_op_t;

/*! @name STOPCMD_OP */
/* SDIO STOP command opcode */
#define SDIO_STOPCMD_OP_STOPCMD_OP_MASK                              0x3f00
#define SDIO_STOPCMD_OP_STOPCMD_OP_SHIFT                             8
#define SDIO_STOPCMD_OP_STOPCMD_OP(val)                              ((val) << 8)

/* SDIO STOP command response type:
  -3’b000: No Responce
  -3’b001: 48 bits with CRC
  -3’b010: 48 bits no CRC
  -3’b011: 136bits
  -3’b100: 48 bits with BUSY check */
#define SDIO_STOPCMD_OP_STOPCMD_RSP_TYPE_MASK                        0x7
#define SDIO_STOPCMD_OP_STOPCMD_RSP_TYPE_SHIFT                       0
#define SDIO_STOPCMD_OP_STOPCMD_RSP_TYPE(val)                        ((val) << 0)


/*! @name sdio_stopcmd_arg_t */

typedef union {
  struct {
    unsigned int stopcmd_arg     :32; // Argument to be sent with the STOP command
  };
  unsigned int raw;
} __attribute__((packed)) sdio_stopcmd_arg_t;

/*! @name STOPCMD_ARG */
/* Argument to be sent with the STOP command */
#define SDIO_STOPCMD_ARG_STOPCMD_ARG_MASK                            0xffffffff
#define SDIO_STOPCMD_ARG_STOPCMD_ARG_SHIFT                           0
#define SDIO_STOPCMD_ARG_STOPCMD_ARG(val)                            ((val) << 0)


/* ----------------------------------------------------------------------------

 CMD IDs and macros

----------------------------------------------------------------------------*/
#define SDIO_CMD_GO_IDLE_STATE() ((0x0UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_SEND_CID() ((0x2UL << 0x8UL)|(0x3UL << 0x0UL))
#define SDIO_CMD_SEND_RCA() ((0x3UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_SEND_VOLTAGE() ((0x8UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_APP_SPEC_CMD() ((0x37UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_ACMD_HCS() ((0x29UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_SELECT() ((0x7UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_ACMD_CFG_QUAD() ((0x6UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_READ_SINGLE_BLOCK() ((0x11UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_READ_MULT_BLOCK() ((0x12UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_WRITE_SINGLE_BLOCK() ((0x18UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_WRITE_MULT_BLOCK() ((0x19UL << 0x8UL)|(0x1UL << 0x0UL))
#define SDIO_CMD_SD_STOP() ((0xcUL << 0x8UL)|(0x1UL << 0x0UL))

#endif /* __SDIO_PERIPH_H__ */
