/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __ARCHI_FLL_CTRL_STRUCT_H__
#define __ARCHI_FLL_CTRL_STRUCT_H__

#include <stdint.h>

typedef union
{
    struct
    {
        uint32_t mult_factor:16;  /* Fll current multiplication factor */
        uint32_t reserved:16;
    };
    uint32_t raw;
} fll_ctrl_status_t;

typedef union
{
    struct
    {
        uint32_t mult_factor:16;      /* Fll requested multiplication factor, reset: 0x5f5.
                                         If RefClk=32768 and Div=2 Freq= 24.98 MHz */
        uint32_t dco_input:10;        /* DCO input code for stand alone mode, reset: 0x121 */
        uint32_t clock_out_divider:4; /* Fll clock output divider, reset: 0x1 e.g div 2 */
        uint32_t output_lock_enable:1;/* Fll output gated by lock signal (active high), reset 1 */
        uint32_t mode:1;              /* Fll mode. 0: stand alone (unlocked), 1: normal, reset 0 */
    };
    uint32_t raw;
} fll_ctrl_conf1_t;

typedef union
{
    struct
    {
        uint32_t loop_gain:4;         /* Fll loop gain, reset: 0x9 */
        uint32_t de_assert_cycles:6;  /* Normal mode: number of refclock unstable cycles till lock de-assert
                                         Standalone mode: lower 6 bits of lock assert counter. Reset: 0x10 */
        uint32_t assert_cycles:6;     /* Normal mode: number of refclock stable cycles till lock assert
                                         Standalone mode: upper 6 bits of lock assert counter. Reset: 0x10 */
        uint32_t lock_tolerance:12;   /* Lock tolerance: margin arounf the target mult factor where clock is
                                         considered as stable. Reset: 0x200
                                         With Fmax=250Mhw (Div=2^4), Fmin=32K (Div=2^15)
                                         Tolerance: 32K*(512/16)=1.048MHz .. 512 Hz */
        uint32_t padding:1;
        uint32_t config_clock_sel:1;  /* Select ref clock when mode = standalone, 0:RefClock, 1: DcoOut. Reset:1 */
        uint32_t open_loop:1;         /* When 1 clock operates in open loop when locked */
        uint32_t dithering:1;         /* When 1 Dithering is enabled */
    };
    uint32_t raw;
} fll_ctrl_conf2_t;

typedef union
{
    struct
    {
        uint32_t padding1:6;
        uint32_t fract_part:10;       /* Integrator state: fractional part (dithering input unit) */
        uint32_t int_part:10;         /* Integratot state: integer part (DCO input bits) */
        uint32_t paddding2:6;
    };
    uint32_t raw;
} fll_ctrl_integrator_t;

#endif  /* __ARCHI_FLL_CTRL_STRUCT_H__ */
