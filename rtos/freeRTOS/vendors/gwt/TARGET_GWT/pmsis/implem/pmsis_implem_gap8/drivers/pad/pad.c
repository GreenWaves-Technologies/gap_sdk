/*
 * Copyright (c) 2019, GreenWaves Technologies, Inc.
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

#include "pmsis/chips/gap8/pad.h"
#include "pmsis/drivers/pad.h"
#include "pmsis.h"
#include "pmsis/implem/hal/hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Driver data
 *****************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/

#if defined(TRACE_PAD)
/* Print pad info. */
void pi_pad_print();
#endif  /* TRACE_PAD */

/*******************************************************************************
 * Internal functions
 ******************************************************************************/

#if defined(TRACE_PAD)
void pi_pad_print()
{
    printf("\nPadfun :\n");
    for (uint32_t i=0; i<(uint32_t) ARCHI_PAD_NB_PADFUNC_REG; i+=4)
    {
        printf("%x %x %x %x\n",
               soc_ctrl_safe_padfun_get(i + 0), soc_ctrl_safe_padfun_get(i + 1),
               soc_ctrl_safe_padfun_get(i + 2), soc_ctrl_safe_padfun_get(i + 3));
    }
    printf("Padcfg :\n");
    for (uint32_t i=0; i<(uint32_t) ARCHI_PAD_NB_PADCFG_REG; i+=4)
    {
        printf("%x %x %x %x\n",
               soc_ctrl_safe_padcfg_get(i + 0), soc_ctrl_safe_padcfg_get(i + 1),
               soc_ctrl_safe_padcfg_get(i + 2), soc_ctrl_safe_padcfg_get(i + 3));
    }
}
#endif  /* TRACE_PAD */

/*******************************************************************************
 * API implementation
 ******************************************************************************/

void pi_pad_set_function(pi_pad_e pad, pi_pad_func_e function)
{
    PAD_TRACE("Setting pad=%d func=%d\n", pad, function);
    hal_pad_function_set(pad, function);
}

void pi_pad_init(uint32_t pad_values[])
{
    for (uint32_t pad_reg = 0; pad_reg < (uint32_t) ARCHI_PAD_NB_PADFUNC_REG; pad_reg++)
    {
        PAD_TRACE("Setting padfunc_%d=%lx\n", pad_reg, pad_values[pad_reg]);
        hal_pad_padfunc_set(pad_reg, pad_values[pad_reg]);
    }
}

void pi_pad_set_configuration(pi_pad_e pad, pi_pad_flags_e cfg)
{
    PAD_TRACE("Setting config pad=%d, pull=%d, drive=%d\n",
              pad, (cfg >> PI_PAD_PULL_OFFSET), (cfg >> PI_PAD_DRIVE_OFFSET));
    hal_pad_padcfg_configuration_set(pad,
                                     (cfg >> PI_PAD_PULL_OFFSET),
                                     (cfg >> PI_PAD_DRIVE_OFFSET));
}

void pi_pad_sleepcfg_set(uint32_t sleepcfg[], uint8_t sleep_ena)
{
    for (uint32_t pad_reg = 0; pad_reg < (uint32_t) ARCHI_PAD_NB_SLEEPPADCFG_REG; pad_reg++)
    {
        PAD_TRACE("Setting pad sleepcfg_%d=%lx\n", pad_reg, sleepcfg[pad_reg]);
        hal_pad_sleeppadcfg_set(pad_reg, sleepcfg[pad_reg]);
    }
    PAD_TRACE("Pad sleep enable=%d\n", sleep_ena);
    hal_pad_padsleep_enable(sleep_ena);
}
