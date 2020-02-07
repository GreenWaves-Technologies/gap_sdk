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

#ifndef __SPI_PERIPH_H__
#define __SPI_PERIPH_H__

#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- SPI Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** SPI_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
} spi_t;


/* ----------------------------------------------------------------------------
   -- SPI Register Bitfield Access --
   ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------
   -- SPI CMD IDs and macros --
   ---------------------------------------------------------------------------- */

#define SPI_CMD_CFG_ID          0x0 /* Sets the configuration for the SPI Master IP. */
#define SPI_CMD_SOT_ID          0x1 /* Sets the Chip Select (CS). */
#define SPI_CMD_SEND_CMD_ID     0x2 /* Transmits a configurable size command. */
#define SPI_CMD_SEND_ADDR_ID    0x3 /* Transmits a configurable size address. */
#define SPI_CMD_DUMMY_ID        0x4 /* Receives a number of dummy bits (not sent to the rx interface). */
#define SPI_CMD_WAIT_ID         0x5 /* Waits an external event to move to the next instruction. */
#define SPI_CMD_TX_DATA_ID      0x6 /* Sends data (max 64Kbits). */
#define SPI_CMD_RX_DATA_ID      0x7 /* Receives data (max 64Kbits). */
#define SPI_CMD_RPT_ID          0x8 /* Repeat the next transfer N times. */
#define SPI_CMD_EOT_ID          0x9 /* Clears the Chip Select (CS). */
#define SPI_CMD_RPT_END_ID      0xA /* End of the repeat loop command. */
#define SPI_CMD_RX_CHECK_ID     0xB /* Check up ot 16 bits of data against an expected value. */
#define SPI_CMD_FULL_DUPL_ID    0xC /* Activate full duplex mode. */

#define SPI_CMD_ID_OFFSET       28

#define SPI_CMD_CFG(clkdiv,cpha,cpol)                                      ((clkdiv << 0)|(cpha << 8)|(cpol << 9)|(SPI_CMD_CFG_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_SOT(cs)                                                    ((cs << 0)|(SPI_CMD_SOT_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_SEND_CMD(cmd_value,cmd_size,qpi)                           ((cmd_value << 0)|((cmd_size-1) << 16)|(qpi << 27)|(SPI_CMD_SEND_CMD_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_SEND_ADDR(cmd_size,qpi)                                    (((cmd_size-1) << 16)|(qpi << 27)|(SPI_CMD_SEND_ADDR_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_DUMMY(dummy_cycle)                                         (((dummy_cycle-1) << 16)|(SPI_CMD_DUMMY_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_WAIT(event_id)                                             ((event_id << 0)|(SPI_CMD_WAIT_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_TX_DATA(data_size,qpi,byte_align)                          (((data_size-1) << 0)|(byte_align << 26)|(qpi << 27)|(SPI_CMD_TX_DATA_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_RX_DATA(data_size,qpi,byte_align)                          (((data_size-1) << 0)|(byte_align << 26)|(qpi << 27)|(SPI_CMD_RX_DATA_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_RPT(rpt_cnt)                                               ((rpt_cnt << 0)|(SPI_CMD_RPT_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_EOT(event_gen)                                             ((event_gen << 0)|(SPI_CMD_EOT_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_RPT_END()                                                  ((SPI_CMD_RPT_END_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_RX_CHECK(comp_data,status_size,check_type,byte_align,qpi)  ((comp_data << 0)|(status_size << 16)|(check_type << 24)|(byte_align << 26)|(qpi << 27)|(SPI_CMD_RX_CHECK_ID << SPI_CMD_ID_OFFSET))
#define SPI_CMD_FULL_DUPL(data_size,byte_align)                            (((data_size-1) << 0)|(byte_align << 26)|(SPI_CMD_FULL_DUPL_ID << SPI_CMD_ID_OFFSET))


#endif /* __SPI_PERIPH_H__ */
