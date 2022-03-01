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

#pragma once


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define UDMA_NB_TIMESTAMP_EVT               (4)
#define UDMA_NB_TIMESTAMP_INPUT             (8)

struct pi_udma_timestamp_cnt_t
{
    uint32_t base;          /*!< base addr of TS */
    uint8_t  device_id;     /*!< device ID for timestamp */
    uint8_t  cnt_trig_gpio; /*!< gpio number for trigger the timestamp cnter */
    uint8_t  cnt_trig_type; /*!< how the gpio trigger the timestamp counter */
    uint8_t  cnt_src;       /*!< timestamp counter source */
    uint8_t  cnt_src_id;    /*!< GPIO/PWM number depends on the counter source */
    uint8_t  prescaler;     /*!< Prescaler for timestamp counter */
    uint8_t  cnt_en;        /*!< If the counter is enabled */
};

struct pi_udma_ts_evt_t
{
    uint8_t  dest_id;       /*!< fifo ID for timestamp */
    uint8_t  soc_evt;       /*!< soc event ID which should be propagated to periph */
    uint8_t  ts_evt_id;     /*!< The udma cfg evt number */
};

struct pi_udma_ts_input_t
{
    uint8_t dest_id;
    uint8_t ts_input_id;    /*!< Timestamp input ID, max 8 input. Reg0-7 */
    uint8_t input_sel;      /*!< Timestamp input selction:
                              if input_type=3, then 0-7 are SFU, 8-10 are SAI.
                              Else input sel are GPIO 0-63 */
    uint8_t input_type;     /*!< Timestamp input GPIO trigger or input from AUX */
};

/*******************************************************************************
 * Driver data
 ******************************************************************************/

/*******************************************************************************
 * Function declaration
 ******************************************************************************/
