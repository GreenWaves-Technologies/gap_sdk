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

#ifndef __PMSIS_HAL_EFUSE_H__
#define __PMSIS_HAL_EFUSE_H__

#include "pmsis/targets/target.h"

static inline void hal_efuse_start_read() {
  hal_write32(&efuse_ctrl->cmd, EFUSE_CTRL_CMD_READ);
}

static inline void hal_efuse_start_program() {
  hal_write32(&efuse_ctrl->cmd, EFUSE_CTRL_CMD_WRITE);
}

static inline void hal_efuse_sleep() {
  hal_write32(&efuse_ctrl->cmd, EFUSE_CTRL_CMD_SLEEP);
}

static inline char hal_efuse_read_byte(unsigned int regId) {
  return hal_read8(&efuse_regs_array[regId]);
}

static inline int hal_efuse_read_word(unsigned int regId) {
  return hal_read32(&efuse_regs_array[regId]);
}

static inline void hal_efuse_write_byte(unsigned int regId, unsigned char value) {
	for (unsigned int i=0;i<8;i++) {
		if ((value & 0x1) == 0x1) hal_write8(&efuse_regs_array[regId], i);
		value >>= 1;
	}
}

// sets the values for the 3 diffrent timing intervals that are used by the IP
// bits[ 9: 0] sets the short interval  default   5 cycles
// bits[19:10] sets the medium interval default  50 cycles
// bits[29:20] sets the long interval   default 400 cycles
static inline void hal_efuse_config_timings(unsigned int timervalues) {
	hal_write32(&efuse_ctrl->cfg, timervalues);
}

#endif  /* __PMSIS_HAL_EFUSE_H__ */
