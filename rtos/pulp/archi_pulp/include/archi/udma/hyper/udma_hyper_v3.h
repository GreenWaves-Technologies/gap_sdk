/*
 * Copyright (C) 2020 ETH Zurich and University of Bologna
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

/* 
 * Authors: Nazareno Bruschi, Unibo (nazareno.bruschi@unibo.it)
            Hayate Okuhara, Unibo (hayate.okuhara@unibo.it)
 */

#ifndef __ARCHI_UDMA_HYPER_V3_H__
#define __ARCHI_UDMA_HYPER_V3_H__

#define UDMA_HYPER_OFFSET(id)          UDMA_PERIPH_OFFSET(ARCHI_UDMA_HYPER_ID((id)))
#define UDMA_HYPER_BASE_ADDR(id)       (ARCHI_UDMA_ADDR + UDMA_HYPER_OFFSET((id)))
#define UDMA_HYPER_RX_ADDR(id)         (ARCHI_UDMA_ADDR + UDMA_HYPER_OFFSET((id)) + UDMA_CHANNEL_RX_OFFSET)
#define UDMA_HYPER_TX_ADDR(id)         (ARCHI_UDMA_ADDR + UDMA_HYPER_OFFSET((id)) + UDMA_CHANNEL_TX_OFFSET)
#define UDMA_HYPER_CUSTOM_ADDR(id)     (ARCHI_UDMA_ADDR + UDMA_HYPER_OFFSET((id)) + UDMA_CHANNEL_CUSTOM_OFFSET)

/* How many devices in the same hyperchip: cs=0 HyperRam, cs=1 HyperFlash
*/
#define HYPER_NB_CS 2
/* Architectural configuration
*/
#define HYPER_FIFO_DEPTH 8
#define HYPER_NB_CHANNELS 8 /* 8 tran_id and 1 common space */

#define HYPER_CHANNELS_OFFSET 0x80
#define HYPER_CHANNELS_ID_OFFSET(id) HYPER_CHANNELS_OFFSET*(id)
#define HYPER_COMMON_REGS_OFFSET HYPER_CHANNELS_OFFSET*HYPER_NB_CHANNELS

#define HYPER_NB_COMMON_REGS 10
#define HYPER_NB_CONFIG_REGS 16
#define HYPER_NB_REGS HYPER_NB_COMMON_REGS + HYPER_NB_CONFIG_REGS*HYPER_NB_CHANNELS
#define HYPER_NB_TWD_REGS 6
#define HYPER_NB_CTL_REGS 2

/* Status reg fields
*/
#define HYPER_BUSY_OFFSET 0
#define HYPER_NB_TRAN_OFFSET 1
#define HYPER_BUSY_SIZE 1
#define HYPER_NB_TRAN_SIZE 8

/* From data path
*/
#define ARCHI_UDMA_HYPER_RX_EVT           0
#define ARCHI_UDMA_HYPER_TX_EVT           1
/* From command path
*/
#define ARCHI_UDMA_HYPER_EOT_RX_EVT       2
#define ARCHI_UDMA_HYPER_EOT_TX_EVT       3

/* Start from periph id*4 */
#define ARCHI_SOC_EVENT_HYPER_RX      32
#define ARCHI_SOC_EVENT_HYPER_TX      33
#define ARCHI_SOC_EVENT_HYPER_EOT_RX  34
#define ARCHI_SOC_EVENT_HYPER_EOT_TX  35


/* Common registers offsets
*/
#define REG_PAGE_BOUND_OFFSET 0x00 // Page boundary setting for the external memory 
#define REG_T_LATENCY_ACCESS_OFFSET 0x04 // T_latency_access
#define REG_EN_LATENCY_ADD_OFFSET 0x08 // The additional latency of the Hyper bus protocol is activated
#define REG_T_CS_MAX_OFFSET 0x0C // Maximum cycle counts for negating the chip select
#define REG_T_RW_RECOVERY_OFFSET 0x10 // T_ready_write_recovery 
#define REG_T_RWDS_DELAY_LINE_OFFSET 0x14 // Configuration for the delay line
#define REG_T_VARI_LATENCY_OFFSET 0x18 // Cycle counts for capturing the input rwds signal
#define N_HYPER_DEVICE_OFFSET 0x1C // Reserved for the future extension
#define MEM_SEL_OFFSET 0x20 // Reserved for the future extension
#define TRANS_ID_ALLOC_OFFSET 0x24 // The smallest number of the transaction id which is not busy

/* Common registers
*/
#define REG_PAGE_BOUND REG_PAGE_BOUND_OFFSET + HYPER_COMMON_REGS_OFFSET 
#define REG_T_LATENCY_ACCESS REG_T_LATENCY_ACCESS_OFFSET + HYPER_COMMON_REGS_OFFSET
#define REG_EN_LATENCY_ADD REG_EN_LATENCY_ADD_OFFSET + HYPER_COMMON_REGS_OFFSET
#define REG_T_CS_MAX REG_T_CS_MAX_OFFSET + HYPER_COMMON_REGS_OFFSET
#define REG_T_RW_RECOVERY REG_T_RW_RECOVERY_OFFSET + HYPER_COMMON_REGS_OFFSET 
#define REG_T_RWDS_DELAY_LINE REG_T_RWDS_DELAY_LINE_OFFSET + HYPER_COMMON_REGS_OFFSET
#define REG_T_VARI_LATENCY REG_T_VARI_LATENCY_OFFSET + HYPER_COMMON_REGS_OFFSET
#define N_HYPER_DEVICE N_HYPER_DEVICE_OFFSET + HYPER_COMMON_REGS_OFFSET
#define MEM_SEL MEM_SEL_OFFSET + HYPER_COMMON_REGS_OFFSET
#define TRANS_ID_ALLOC TRANS_ID_ALLOC_OFFSET + HYPER_COMMON_REGS_OFFSET

/*Configuration registers offset
*/
#define UDMA_HYPER_SADDR_OFFSET 0x00
#define UDMA_HYPER_SIZE_OFFSET	0x04
#define UDMA_HYPER_CFG_OFFSET 0x08

#define UDMA_HYPER_CHANNEL_RX_OFFSET 0x00
#define UDMA_HYPER_CHANNEL_TX_OFFSET 0x0C
#define UDMA_HYPER_CHANNEL_CUSTOM_OFFSET 0x18

#define REG_RX_SADDR_OFFSET 0x00 // L2 start address for RX region
#define REG_RX_SIZE_OFFSET 0x04 // Size of the software buffer in L2 (in byte) 
#define REG_RX_CFG_OFFSET 0x08 // µDMA configuration data for the RX direction (clr, en, datasize, continuous)

#define REG_TX_SADDR_OFFSET 0x0C // L2 start address for TX region
#define REG_TX_SIZE_OFFSET 0x10 // Size of the total TX data to be sent (in byte) 
#define REG_TX_CFG_OFFSET 0x14 // µDMA configuration data for the TX direction (clr, en, datasize, continuous)

#define HYPER_CA_SETUP_OFFSET 0x18 // Command/Address setup for Hyperbus protocol
#define REG_HYPER_ADDR_OFFSET 0x1C // Start address of the external memory (e.g. Hyper RAM) 
#define REG_HYPER_CFG_OFFSET 0x20 // Data to be written in the Hyper RAM configuration registers
#define STATUS_OFFSET 0x24 // Busy signal and the number of issued transactions in each id
#define TWD_ACT_OFFSET 0x28 // Enabling the 2D transfer functionality
#define TWD_COUNT_OFFSET 0x2C // Size of the 2D transfer count in byte 
#define TWD_STRIDE_OFFSET 0x30 // Stride value of the 2D transfer
#define TWD_ACT_L2_OFFSET 0x34 // Activate the 2D transaction for the L2
#define TWD_COUNT_L2_OFFSET 0x38 // Count value of the 2D transfer for the L2
#define TWD_STRIDE_L2_OFFSET 0x3C // Stride value of the 2D transfer for the L2

/* Configuration registers
*/
#define UDMA_HYPER_CHANNEL_RX(id) UDMA_HYPER_CHANNEL_RX_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define UDMA_HYPER_CHANNEL_TX(id) UDMA_HYPER_CHANNEL_TX_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define UDMA_HYPER_CHANNEL_CUSTOM(id) UDMA_HYPER_CHANNEL_CUSTOM_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))

#define REG_RX_SADDR(id) REG_RX_SADDR_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_RX_SIZE(id) REG_RX_SIZE_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_RX_CFG(id) REG_RX_CFG_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))

#define REG_TX_SADDR(id) REG_TX_SADDR_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_TX_SIZE(id) REG_TX_SIZE_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_TX_CFG(id) REG_TX_CFG_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))

#define HYPER_CA_SETUP(id) HYPER_CA_SETUP_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_HYPER_ADDR(id) REG_HYPER_ADDR_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define REG_HYPER_CFG(id) REG_HYPER_CFG_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define STATUS(id) STATUS_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_ACT(id) TWD_ACT_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_COUNT(id) TWD_COUNT_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_STRIDE(id) TWD_STRIDE_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_ACT_L2(id) TWD_ACT_L2_OFFSET  + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_COUNT_L2(id) TWD_COUNT_L2_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))
#define TWD_STRIDE_L2(id) TWD_STRIDE_L2_OFFSET + HYPER_CHANNELS_ID_OFFSET((id))

#endif
