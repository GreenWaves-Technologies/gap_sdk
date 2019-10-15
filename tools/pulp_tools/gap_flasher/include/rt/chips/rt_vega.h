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

#ifndef __RT_CHIPS_RT_VEGA_H__
#define __RT_CHIPS_RT_VEGA_H__

#ifdef CONFIG_VEGA

/// @cond IMPLEM

#define __RT_FLAGS_MUX_BIT   16
#define __RT_FLAGS_MUX_WIDTH 2

/// @endcond


/** \enum rt_pm_domain_e
 * \brief Power domains.
 *
 * Describes the available power domains (other than the chip itself).
 */
typedef enum {
  RT_PM_DOMAIN_MRAM=0,  /*!< Embedded MRAM. */
  RT_PM_DOMAIN_CSI =1   /*!< CSI2 interface. */
} rt_pm_domain_e;



/** Specify that the I2S channel should be multiplexed with only 1 stream. */
#define RT_I2S_FLAGS_MUX_1CH       (0<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams on the same pad. */
#define RT_I2S_FLAGS_MUX_1CH_DUAL  (1<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 2 streams, each one on a distinct pad. */
#define RT_I2S_FLAGS_MUX_2CH       (2<<__RT_FLAGS_MUX_BIT)

/** Specify that the I2S channel should be multiplexed with 4 streams, with the 2 first on the first pad, and the 2 lasts on the second pad. */
#define RT_I2S_FLAGS_MUX_2CH_DUAL  (3<<__RT_FLAGS_MUX_BIT)

#endif

#endif