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

#ifndef __HAL_EFUSE_EFUSE_V1_H__
#define __HAL_EFUSE_EFUSE_V1_H__

#include "archi/efuse/efuse_v1.h"
#include "hal/pulp_io.h"

static inline void plp_efuse_startRead() {
  pulp_write32(ARCHI_EFUSE_ADDR + EFUSE_CMD_OFFSET, EFUSE_CMD_READ);
}

static inline void plp_efuse_startProgram() {
  pulp_write32(ARCHI_EFUSE_ADDR + EFUSE_CMD_OFFSET, EFUSE_CMD_WRITE);
}

static inline void plp_efuse_sleep() {
  pulp_write32(ARCHI_EFUSE_ADDR + EFUSE_CMD_OFFSET, EFUSE_CMD_SLEEP);
}

static inline char plp_efuse_readByte(unsigned int regId) {
  return pulp_read8(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4);
}

static inline int plp_efuse_readWord(unsigned int regId) {
  return pulp_read32(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4);
}

static inline void plp_efuse_writeByte(unsigned int regId, unsigned char value) {
	for (unsigned int i=0;i<8;i++) {
		if ((value & 0x1) == 0x1) pulp_write8(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4, i);
		value >>= 1;
	}
}

// sets the values for the 3 diffrent timing intervals that are used by the IP
// bits[ 9: 0] sets the short interval  default   5 cycles
// bits[19:10] sets the medium interval default  50 cycles
// bits[29:20] sets the long interval   default 400 cycles
static inline void plp_efuse_configTimings(unsigned int timervalues) {
	pulp_write32(ARCHI_EFUSE_ADDR + EFUSE_CFG_OFFSET, timervalues);
}

#endif