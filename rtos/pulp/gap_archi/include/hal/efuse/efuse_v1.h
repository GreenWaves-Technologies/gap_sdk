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

#include "archi/chips/gap9_v2/efuse/efuse.h"
#include "hal/pulp_io.h"


// =====

#define  EFUSE_SIZE 128

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  volatile uint32_t regs[EFUSE_SIZE];            /**< EFUSE_Registers, offset: 0x000 */
} efuse_regs_t;


#define efuse_ctrl ((efuse_t*)ARCHI_EFUSE_ADDR)
#define efuse_regs ((efuse_regs_t*)(ARCHI_EFUSE_ADDR+0x200))


/*!
 * @name Efuse read and program control.
 * @{
 */

/*
 *sets the values for the 3 diffrent timing intervals that are used by the IP
 * bits[ 9: 0] sets the short interval  default   5 cycles
 * bits[19:10] sets the medium interval default  50 cycles
 * bits[29:20] sets the long interval   default 400 cycles
 */
static inline void efuse_config_timings(uint32_t timervalues)
{
    efuse_ctrl->cfg = timervalues;
}

static inline void efuse_start_read()
{
    efuse_ctrl->cmd = EFUSE_CMD_READ(1);
}

static inline void efuse_start_program()
{
    efuse_ctrl->cmd = EFUSE_CMD_WRITE(1);
}

static inline void efuse_sleep()
{
    efuse_ctrl->cmd = EFUSE_CMD_IDLE(1);
}

/* @} */

/*!
 * @name Efuse register set and get.
 * @{
 */
static inline void efuse_program(uint32_t index, uint32_t value)
{
    for (int i = 0; i < 32; i++)
    {
        if(value & 0x1)
            pulp_write32(&efuse_regs->regs[index],i);
        value >>= 1;
    }
}

/*!
 * @name Efuse register set and get.
 * @{
 */
static inline void efuse_program_byte(uint32_t index, uint32_t value)
{
    uint8_t *regs = (uint8_t*)efuse_regs->regs;
    for (int i = 0; i < 32; i++)
    {
        if(value & 0x1)
            pulp_write8(&regs[index], i);
        value >>= 1;
    }
}

static inline uint32_t efuse_read(uint32_t index)
{
    return efuse_regs->regs[index];
}

static inline uint32_t efuse_read_word(uint32_t index)
{
    return efuse_regs->regs[index];
}

/** Read only first byte of a register **/
static inline uint32_t efuse_read_byte(uint32_t index)
{
    uint8_t *regs = (uint8_t*)efuse_regs->regs;
    return regs[index*4];
}

static inline void efuse_prot_page(uint32_t page_id)
{
    efuse_ctrl->page_protect = 1 << page_id;
}

static inline void efuse_unprot_page(uint32_t page_id)
{
    efuse_ctrl->page_protect = ~(1 << page_id);
}

static inline void efuse_prot_page_lock(void)
{
    efuse_ctrl->page_protect |= (1 << 31);
}


static inline void plp_efuse_startRead() {
  efuse_start_read();
}

static inline void plp_efuse_startProgram() {
  efuse_start_program();
}

static inline void plp_efuse_sleep() {
  efuse_sleep();
}

static inline char plp_efuse_readByte(unsigned int regId) {
  //return pulp_read8(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4);
  return efuse_read_byte(regId);
}

static inline int plp_efuse_readWord(unsigned int regId) {
  return efuse_read(regId);
  //return pulp_read32(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4);
}

static inline void plp_efuse_writeByte(unsigned int regId, unsigned char value) {
	//for (unsigned int i=0;i<8;i++) {
	//	if ((value & 0x1) == 0x1) pulp_write8(ARCHI_EFUSE_ADDR + EFUSE_REGS_OFFSET + regId*4, i);
	//	value >>= 1;
	//}
    efuse_program_byte(regId,value);
}

// sets the values for the 3 diffrent timing intervals that are used by the IP
// bits[ 9: 0] sets the short interval  default   5 cycles
// bits[19:10] sets the medium interval default  50 cycles
// bits[29:20] sets the long interval   default 400 cycles
static inline void plp_efuse_configTimings(unsigned int timervalues) {
	pulp_write32(ARCHI_EFUSE_ADDR + EFUSE_CFG_OFFSET, timervalues);
}
#endif
