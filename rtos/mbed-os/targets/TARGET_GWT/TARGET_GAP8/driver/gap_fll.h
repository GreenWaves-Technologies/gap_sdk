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

#ifndef _GAP_FLL_H_
#define _GAP_FLL_H_

#include "cmsis.h"
#include "gap_soc_eu.h"
#include "gap_regulator.h"

/*!
 * @addtogroup fll
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define    FLL_LV_MAX_FREQUENCY       150000000
#define    FLL_NV_MAX_FREQUENCY       250000000
#define    FLL_SOC_MIN_FREQUENCY      150000000
#define    FLL_SOC_MAX_FREQUENCY      250000000
#define    FLL_CLUSTER_MIN_FREQUENCY   87000000
#define    FLL_CLUSTER_MAX_FREQUENCY  175000000

#define    FLL_SOC_FV_SLOPE       ((FLL_SOC_MAX_FREQUENCY - FLL_SOC_MIN_FREQUENCY) / (DCDC_DEFAULT_NV - DCDC_DEFAULT_LV))
#define    FLL_CLUSTER_FV_SLOPE   ((FLL_CLUSTER_MAX_FREQUENCY - FLL_CLUSTER_MIN_FREQUENCY) / (DCDC_DEFAULT_NV - DCDC_DEFAULT_LV))

typedef enum _fll_type {
  uFLL_SOC=0,
  uFLL_PERI=0,
  uFLL_CLUSTER=1,
} fll_type_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Initialize one FLL.
 *
 * @param which_fll       SoC's or Cluster's fll.
 * @param ret_state       Retention state.
 *
 * @note .
 */
void FLL_Init(fll_type_t which_fll, uint32_t ret_state);

/*!
 * @brief Deinitalize one FLL.
 *
 * @param which_fll       SoC's or Cluster's fll.
 *
 * @note .
 */
void FLL_DeInit(fll_type_t which_fll);

/*!
 * @brief Clean all FLL configuration.
 *
 * @note .
 */
void FLL_Clear();


/*!
 * @brief Set specific FLL to wanted frequency.
 *
 * @param which_fll       SoC's or Cluster's fll.
 * @param frequency       The frequency value to set.
 * @param check           Check frequency.
 *
 * @note .
 * @return check result of frequency.
 */
int FLL_SetFrequency(fll_type_t which_fll, uint32_t frequency, int check);

/*!
 * @brief Get specific FLL's frequency.
 *
 * @param which_fll       SoC's or Cluster's fll.
 *
 * @note .
 * @return frequency value.
 */
int FLL_GetFrequency(fll_type_t which_fll);

/*!
 * @brief Calculate FC SOC domain's max frequency with certain voltage
 *
 * @param  voltage  Given voltage
 *
 * @return max frquency.
 */
static inline int FLL_SoCMaxFreqAtV(int voltage)
{
    return (FLL_SOC_MIN_FREQUENCY + (voltage - DCDC_DEFAULT_LV) * FLL_SOC_FV_SLOPE);
}

/*!
 * @brief Calculate cluster domain's max frequency with certain voltage
 *
 * @param  voltage  Given voltage
 *
 * @return max frquency.
 */
static inline int FLL_ClusterMaxFreqAtV(int voltage)
{
    return (FLL_CLUSTER_MIN_FREQUENCY + (voltage - DCDC_DEFAULT_LV) * FLL_CLUSTER_FV_SLOPE);
}

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/* @} */

#endif /*_GAP_FLL_H_*/
