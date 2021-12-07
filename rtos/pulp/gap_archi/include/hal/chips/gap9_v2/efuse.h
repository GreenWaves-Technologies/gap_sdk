/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __HAL_VEGA_EFUSE_H__
#define __HAL_VEGA_EFUSE_H__

#include "hal/pulp.h"

// TODO OLD fields to put back

      //unsigned int signature:1;
      //unsigned int flash_reset:1;
      //unsigned int flash_wait:1;
      //unsigned int flash_wakeup:1;
      //unsigned int flash_init:1;
      //unsigned int flash_reset_wait:1;
      //unsigned int flash_id:1;
  //    unsigned int flash_cmd:1;
  //    unsigned int flash_cmd_ds:1;
  //    unsigned int flash_cmd2:1;
  //    unsigned int flash_cmd2_ds:1;
  //    unsigned int flash_cmd3:1;
  //    unsigned int flash_cmd3_ds:1;
  //    unsigned int flash_cmd4:1;
  //    unsigned int flash_cmd4_ds:1;
  //uint8_t Key[4][4];
  //uint8_t IV[2][4];

  
#define PLP_EFUSE_PLT_OTHER   0
#define PLP_EFUSE_PLT_FPGA    1
#define PLP_EFUSE_PLT_RTL     2
#define PLP_EFUSE_PLT_VP      3
#define PLP_EFUSE_PLT_CHIP    4


#define EFUSE_BOOT_JTAG_STOP             0
#define EFUSE_BOOT_HYPERFLASH            1
#define EFUSE_BOOT_SPIFLASH              2
#define EFUSE_BOOT_MRAM                  3
#define EFUSE_BOOT_SPISLAVE              4
#define EFUSE_BOOT_JTAG_STOP_CLUSTER_ON  5

#define EFUSE_BOOT_CLUSTER_ON (1<<7)


static inline unsigned int plp_efuse_readShort(int lsb, int msb) {
  return plp_efuse_readByte(lsb) | (plp_efuse_readByte(msb) << 8);
}

#endif
