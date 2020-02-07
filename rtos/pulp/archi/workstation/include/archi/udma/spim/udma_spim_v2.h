/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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

#ifndef __ARCHI_UDMA_UDMA_SPIM_V2_H__
#define __ARCHI_UDMA_UDMA_SPIM_V2_H__

// SPI commands IDS definition
#define SPI_CMD_CFG_ID       0
#define SPI_CMD_SOT_ID       1
#define SPI_CMD_SEND_CMD_ID  2
#define SPI_CMD_SEND_ADDR_ID 3
#define SPI_CMD_DUMMY_ID     4
#define SPI_CMD_WAIT_ID      5
#define SPI_CMD_TX_DATA_ID   6
#define SPI_CMD_RX_DATA_ID   7
#define SPI_CMD_RPT_ID       8
#define SPI_CMD_EOT_ID       9
#define SPI_CMD_RPT_END_ID   10
#define SPI_CMD_RX_CHECK_ID  11
#define SPI_CMD_FUL_ID       12

// SPI command fields offset, mask, value definition
// SPI commands fields offsets
#define SPI_CMD_ID_OFFSET       28
#define SPI_CMD_ID_WIDTH         4

// COMMON definitions
#define SPI_CMD_QPI_ENA   1
#define SPI_CMD_QPI_DIS   0
#define SPI_CMD_BYTE_ALIGN_DIS  1
#define SPI_CMD_BYTE_ALIGN_ENA  0
#define SPI_CMD_DATA_WITDH(val) (val)
#define SPI_CMD_CMD_SIZE(val) (val)

// CFG
#define SPI_CMD_CFG_CLK_DIV_OFFSET      0
#define SPI_CMD_CFG_CLK_DIV_WIDTH   8
#define SPI_CMD_CFG_CPHA_OFFSET         8
#define SPI_CMD_CFG_CPOL_OFFSET       9

#define SPI_CMD_CFG_CLKDIV(val) (val)
#define SPI_CMD_CFG_CPOL_POS  1
#define SPI_CMD_CFG_CPOL_NEG  0
#define SPI_CMD_CFG_CPHA_STD  1
#define SPI_CMD_CFG_CPHA_OPP  0

// SOT
#define SPI_CMD_SOT_CS_OFFSET    0
#define SPI_CMD_SOT_CS_WIDTH     2

#define SPI_CMD_SOT_CS0   0
#define SPI_CMD_SOT_CS1   1
#define SPI_CMD_SOT_CS2   2
#define SPI_CMD_SOT_CS3   3

// SEND_CMD
#define SPI_CMD_SEND_CMD_CMD_OFFSET   0
#define SPI_CMD_SEND_CMD_CMD_WIDTH    16
#define SPI_CMD_SEND_CMD_SIZE_OFFSET  16
#define SPI_CMD_SEND_CMD_SIZE_WIDTH   4
#define SPI_CMD_SEND_CMD_QPI_OFFSET   27

// SEND_ADDR
#define SPI_CMD_SEND_ADDR_SIZE_OFFSET   16
#define SPI_CMD_SEND_ADDR_SIZE_WIDTH  5
#define SPI_CMD_SEND_ADDR_QPI_OFFSET    27

//#define SPI_CMD_SEND_ADDR_VALUE(value)  ((((value) & 0xff000000) >> 24) | (((value) & 0xff0000) >> 8) | (((value) & 0xff00) << 8) | (((value) & 0xff) << 24))
#define SPI_CMD_SEND_ADDR_VALUE(value)  (value)


// SEND_DUMMY
#define SPI_CMD_DUMMY_CYCLE_OFFSET      16
#define SPI_CMD_DUMMY_CYCLE_WIDTH   5

// TX_DATA
#define SPI_CMD_TX_DATA_SIZE_OFFSET     0
#define SPI_CMD_TX_DATA_SIZE_WIDTH      16
#define SPI_CMD_TX_DATA_BYTE_ALIGN_OFFSET   26
#define SPI_CMD_TX_DATA_QPI_OFFSET    27
#define SPI_CMD_TX_DATA_QPI_WIDTH           1

// RX_DATA
#define SPI_CMD_RX_DATA_SIZE_OFFSET         0
#define SPI_CMD_RX_DATA_SIZE_WIDTH      16
#define SPI_CMD_RX_DATA_BYTE_ALIGN_OFFSET   26
#define SPI_CMD_RX_DATA_BYTE_ALIGN_WIDTH    1
#define SPI_CMD_RX_DATA_QPI_OFFSET          27
#define SPI_CMD_RX_DATA_QPI_WIDTH           1

// RPT
#define SPI_CMD_RPT_NB_OFFSET           0
#define SPI_CMD_RPT_NB_WIDTH        16

// EOT
#define SPI_CMD_EOT_GEN_EVT_OFFSET      0

#define SPI_CMD_EOT_EVENT_ENA     1
#define SPI_CMD_EOT_EVENT_DIS     0

// WAIT
#define SPI_CMD_WAIT_EVENT_OFFSET       0
#define SPI_CMD_WAIT_EVENT_WIDTH        2

// RX_CHECK
#define SPI_CMD_RX_CHECK_VALUE_OFFSET       0
#define SPI_CMD_RX_CHECK_VALUE_WIDTH    16

#define SPI_CMD_RX_CHECK_SIZE_OFFSET        16
#define SPI_CMD_RX_CHECK_SIZE_WIDTH     4

#define SPI_CMD_RX_CHECK_MODE_OFFSET        24
#define SPI_CMD_RX_CHECK_MODE_WIDTH     2

#define SPI_CMD_RX_CHECK_BYTE_ALIGN_OFFSET  26

#define SPI_CMD_RX_CHECK_QPI_OFFSET     27

#define SPI_CMD_RX_CHECK_MODE_MATCH     0
#define SPI_CMD_RX_CHECK_MODE_ONES      1
#define SPI_CMD_RX_CHECK_MODE_ZEROS     2
#define SPI_CMD_RX_CHECK_MODE_MASK      3

// FULL DUPLEX
#define SPI_CMD_FUL_SIZE_OFFSET           0
#define SPI_CMD_FUL_SIZE_WIDTH            16
#define SPI_CMD_FUL_BYTE_ALIGN_OFFSET       26
#define SPI_CMD_FUL_BYTE_ALIGN_WIDTH        1

// SPI CMD encoding
#define SPI_CMD_CFG(clockDiv,cpol,cpha)   ((SPI_CMD_CFG_ID<<SPI_CMD_ID_OFFSET) | ((cpol)<<SPI_CMD_CFG_CPOL_OFFSET) | ((cpha)<<SPI_CMD_CFG_CPHA_OFFSET) | ((clockDiv)<<SPI_CMD_CFG_CLK_DIV_OFFSET))
#define SPI_CMD_SOT(cs)                   ((SPI_CMD_SOT_ID << SPI_CMD_ID_OFFSET) | ((cs) << SPI_CMD_SOT_CS_OFFSET))
#define SPI_CMD_SEND_CMD(cmd,bits,qpi)    ((SPI_CMD_SEND_CMD_ID<<SPI_CMD_ID_OFFSET) | ((qpi)<<SPI_CMD_SEND_CMD_QPI_OFFSET) | (((bits)-1)<<SPI_CMD_SEND_CMD_SIZE_OFFSET) | (((cmd)>>8)<<SPI_CMD_SEND_CMD_CMD_OFFSET) | (((cmd)&0xff)<<(SPI_CMD_SEND_CMD_CMD_OFFSET+8)))
#define SPI_CMD_SEND_ADDR(bits,qpi)       ((SPI_CMD_SEND_ADDR_ID<<SPI_CMD_ID_OFFSET) | ((qpi)<<SPI_CMD_SEND_ADDR_QPI_OFFSET) | (((bits)-1)<<SPI_CMD_SEND_ADDR_SIZE_OFFSET))
#define SPI_CMD_DUMMY(cycles)             ((SPI_CMD_DUMMY_ID<<SPI_CMD_ID_OFFSET) | (((cycles)-1)<<SPI_CMD_DUMMY_CYCLE_OFFSET))
#define SPI_CMD_TX_DATA(bits,qpi,byte_align)         ((SPI_CMD_TX_DATA_ID<<SPI_CMD_ID_OFFSET) | ((qpi)<<SPI_CMD_TX_DATA_QPI_OFFSET) | (((bits)-1) << SPI_CMD_TX_DATA_SIZE_OFFSET) | ((byte_align)<<SPI_CMD_TX_DATA_BYTE_ALIGN_OFFSET))
#define SPI_CMD_RX_DATA(bits,qpi,byte_align) \
({ \
  unsigned int result = __BITINSERT(0, SPI_CMD_RX_DATA_ID, SPI_CMD_ID_WIDTH, SPI_CMD_ID_OFFSET); \
  result = __BITINSERT_R(result, qpi, SPI_CMD_RX_DATA_QPI_WIDTH, SPI_CMD_RX_DATA_QPI_OFFSET); \
  result = __BITINSERT_R(result, (bits)-1, SPI_CMD_RX_DATA_SIZE_WIDTH, SPI_CMD_RX_DATA_SIZE_OFFSET); \
  result = __BITINSERT_R(result, (byte_align), SPI_CMD_RX_DATA_BYTE_ALIGN_WIDTH, SPI_CMD_RX_DATA_BYTE_ALIGN_OFFSET); \
  result; \
})
#define SPI_CMD_RPT(iter)                 ((SPI_CMD_RPT_ID<<SPI_CMD_ID_OFFSET) | ((iter)<<SPI_CMD_RPT_NB_OFFSET))
#define SPI_CMD_EOT(evt)                  ((SPI_CMD_EOT_ID<<SPI_CMD_ID_OFFSET) | ((evt)<<SPI_CMD_EOT_GEN_EVT_OFFSET))

#define SPI_CMD_RX_CHECK(mode,bits,value,qpi,byte_align) \
  ((SPI_CMD_RX_CHECK_ID<<SPI_CMD_ID_OFFSET) | \
  ((value) << SPI_CMD_RX_CHECK_VALUE_OFFSET) | \
  ((mode) << SPI_CMD_RX_CHECK_MODE_OFFSET) | \
  (((bits)-1) << SPI_CMD_RX_CHECK_SIZE_OFFSET) | \
  ((byte_align)<<SPI_CMD_RX_CHECK_BYTE_ALIGN_OFFSET) | \
  ((qpi)<<SPI_CMD_RX_CHECK_QPI_OFFSET))

#define SPI_CMD_WAIT(event)               ((SPI_CMD_WAIT_ID<<SPI_CMD_ID_OFFSET) | ((event) << SPI_CMD_WAIT_EVENT_OFFSET))
#define SPI_CMD_RPT_END()                 ((SPI_CMD_RPT_END_ID<<SPI_CMD_ID_OFFSET))
#define SPI_CMD_FUL(bits,byte_align)      ((SPI_CMD_FUL_ID<<SPI_CMD_ID_OFFSET) | (((bits)-1) << SPI_CMD_FUL_SIZE_OFFSET) | ((byte_align)<<SPI_CMD_FUL_BYTE_ALIGN_OFFSET))

#endif