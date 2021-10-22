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

#ifndef __HAL_UDMA_HYPER_V3_H__
#define __HAL_UDMA_HYPER_V3_H__

#include "pmsis.h"
#include "archi/udma/hyper/udma_hyper_v3.h"

/* Transaction configuration parameters
*/
#define CONFIG_HYPERFLASH_EN_LATENCY_ADD 0
#define CONFIG_HYPERRAM_EN_LATENCY_ADD 1
#define CONFIG_HYPERFLASH_T_LATENCY_ACCESS 0x0f
#define CONFIG_HYPERRAM_T_LATENCY_ACCESS 0x06

static inline int plp_hyper_get_reg(unsigned int offset){
  return pulp_read32(offset);
}

static inline int plp_hyper_check_memsel(unsigned char hyper_id){
  return plp_hyper_get_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + MEM_SEL);
}

static inline int plp_hyper_nb_tran(unsigned char hyper_id, unsigned int tran_id){
  return plp_hyper_get_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + STATUS(tran_id)) >> 1;
}

static inline int plp_hyper_is_busy(unsigned char hyper_id, unsigned int tran_id){
  return plp_hyper_get_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + STATUS(tran_id)) & 0x00000001;
}

static inline void plp_hyper_wait(unsigned char hyper_id, unsigned int tran_id){
   while(plp_hyper_is_busy(hyper_id, tran_id)){
   }
}

static inline int plp_hyper_id_alloc(unsigned char hyper_id){
  return plp_hyper_get_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TRANS_ID_ALLOC);
}

static inline void plp_hyper_set_reg(unsigned int offset, unsigned int value){
  pulp_write32(offset, value);
}

static inline void plp_hyper_setup(unsigned char hyper_id, unsigned int en_latency_add, unsigned int memsel, unsigned int t_latency_access)
{
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_EN_LATENCY_ADD, en_latency_add);
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + MEM_SEL, memsel);
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_T_CS_MAX, 0xffffffff);
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_T_LATENCY_ACCESS, t_latency_access);
}

static inline void plp_hyper_set_pagebound(unsigned char hyper_id, unsigned int page_bound){

  switch(page_bound){
     case 128:
        plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_PAGE_BOUND, 0x00 ); // page boundary is set to every 128 bytes
        break;
     case 256:
        plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_PAGE_BOUND, 0x01 ); // page boundary is set to every 256 bytes
        break;
     case 512:
        plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_PAGE_BOUND, 0x02 ); // page boundary is set to every 128 bytes
        break;
     case 1024:
        plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_PAGE_BOUND, 0x03 ); // page boundary is set to every 256 bytes
        break;
     default:
        plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_PAGE_BOUND, 0x04 ); // page boundary is not considered
  }

}

static inline void plp_hyper_set_twd_param(unsigned char hyper_id, unsigned int *args, unsigned int tran_id){
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_ACT(tran_id), args[0]); // 2D TRAN is deactivated
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_COUNT(tran_id), args[1]); // 2D COUNT for ext mem
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_STRIDE(tran_id), args[2]); // 2D STRIDE for ext mem
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_ACT_L2(tran_id), args[3]); // 2D TRAN is deactivated
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_COUNT_L2(tran_id), args[4]); // 2D COUNT for l2
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + TWD_STRIDE_L2(tran_id), args[5]); // 2D STRIDE for l2
}

static inline void plp_hyper_set_ctl_param(unsigned char hyper_id, unsigned int *args, unsigned int tran_id){
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + HYPER_CA_SETUP(tran_id), args[0]); // Write is declared for the external mem
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_HYPER_ADDR(tran_id), args[1] ); // external memory address
}

static inline void plp_hyper_set_cfg(unsigned char hyper_id, unsigned int data, unsigned int tran_id)
{
  plp_hyper_set_reg(UDMA_HYPER_BASE_ADDR(hyper_id) + REG_HYPER_CFG(tran_id), data); 
}

static inline void plp_hyper_enqueue(unsigned int chan_offset, unsigned int l2_addr, unsigned int len, unsigned int cfg){
  plp_hyper_set_reg(chan_offset + UDMA_HYPER_SADDR_OFFSET, l2_addr); // Data address in L2
  plp_hyper_set_reg(chan_offset + UDMA_HYPER_SIZE_OFFSET, len );     // Data size to be sent
  plp_hyper_set_reg(chan_offset + UDMA_HYPER_CFG_OFFSET, cfg); // Write transaction is kicked
}

#endif