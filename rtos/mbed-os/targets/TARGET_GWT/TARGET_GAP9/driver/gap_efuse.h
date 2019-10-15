/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _GAP_EFUSE_H_
#define _GAP_EFUSE_H_

#include "cmsis.h"
#include "assert.h"

#define GAP_EFUSE_TOTAL_NUM     128

/*!
 * @addtogroup efuse
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */


/*!
 * @name Efuse read and program control.
 * @{
 */

/*
 *sets the values for the 3 diffrent timing intervals that are used by the IP
 * bits[ 5: 0] sets the short interval  default   2 cycles
 * bits[15:6]  sets the medium interval default  50 cycles
 * bits[29:16] sets the long interval   default 500 cycles
 * bits[31:30] sets the margin interval default   0 cycles
 */
static inline void EFUSE_ConfigTimings(uint32_t timervalues)
{
    EFUSE_CTRL->CFG = timervalues;
}

static inline void EFUSE_StartRead()
{
    EFUSE_CTRL->CMD = EFUSE_CTRL_CMD_READ;
}

static inline void EFUSE_StartProgram()
{
    EFUSE_CTRL->CMD = EFUSE_CTRL_CMD_WRITE;
}

static inline void EFUSE_Sleep()
{
    EFUSE_CTRL->CMD = EFUSE_CTRL_CMD_SLEEP;
}

/* @} */


/*!
 * @name Efuse register set and get.
 * @{
 */
static inline void EFUSE_Program(uint32_t index, uint32_t value)
{
    assert(index < GAP_EFUSE_TOTAL_NUM);

    for (int i = 0; i < 32; i++)
    {
        if(value & 0x1)
            EFUSE_REGS->REGS[index] = i;
        value >>= 1;
    }
}

static inline uint32_t EFUSE_Read(uint32_t index)
{
    assert(index < GAP_EFUSE_TOTAL_NUM);

    return EFUSE_REGS->REGS[index];
}
/* @} */

/* @} */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_EFUSE_H_*/
