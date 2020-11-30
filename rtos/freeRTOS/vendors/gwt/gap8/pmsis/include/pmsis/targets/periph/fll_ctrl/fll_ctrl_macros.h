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

#ifndef __ARCHI_FLL_CTRL_MACROS_H__
#define __ARCHI_FLL_CTRL_MACROS_H__

#include <stdint.h>


/*! @name FLL_STATUS - FLL_CTRL status register */
#define FLL_CTRL_STATUS_MULTI_FACTOR_MASK              (0xFFFFU)
#define FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT             (0U)
#define FLL_CTRL_STATUS_MULTI_FACTOR(x)                (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT */)) & FLL_CTRL_STATUS_MULTI_FACTOR_MASK)
#define READ_FLL_CTRL_STATUS_MULTI_FACTOR(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_STATUS_MULTI_FACTOR_MASK)) /*>> FLL_CTRL_STATUS_MULTI_FACTOR_SHIFT*/)

/*! @name SOC_CONF1 - FLL_CTRL configuration 1 register */
#define FLL_CTRL_CONF1_MULTI_FACTOR_MASK           (0x0000FFFFU)
#define FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT          (0U)
#define FLL_CTRL_CONF1_MULTI_FACTOR(x)             (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT */)) & FLL_CTRL_CONF1_MULTI_FACTOR_MASK)
#define READ_FLL_CTRL_CONF1_MULTI_FACTOR(x)        (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_MULTI_FACTOR_MASK)) /*>> FLL_CTRL_CONF1_MULTI_FACTOR_SHIFT*/)

#define FLL_CTRL_CONF1_DCO_INPUT_MASK              (0x03FF0000U)
#define FLL_CTRL_CONF1_DCO_INPUT_SHIFT             (16U)
#define FLL_CTRL_CONF1_DCO_INPUT(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_DCO_INPUT_SHIFT)) & FLL_CTRL_CONF1_DCO_INPUT_MASK)
#define READ_FLL_CTRL_CONF1_DCO_INPUT(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_DCO_INPUT_MASK)) >> FLL_CTRL_CONF1_DCO_INPUT_SHIFT)

#define FLL_CTRL_CONF1_CLK_OUT_DIV_MASK            (0x3C000000U)
#define FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT           (26U)
#define FLL_CTRL_CONF1_CLK_OUT_DIV(x)              (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT)) & FLL_CTRL_CONF1_CLK_OUT_DIV_MASK)
#define READ_FLL_CTRL_CONF1_CLK_OUT_DIV(x)         (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_CLK_OUT_DIV_MASK)) >> FLL_CTRL_CONF1_CLK_OUT_DIV_SHIFT)

#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK         (0x40000000U)
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT        (30U)
#define FLL_CTRL_CONF1_OUTPUT_LOCK_EN(x)           (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT)) & FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK)
#define READ_FLL_CTRL_CONF1_OUTPUT_LOCK_EN(x)      (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_OUTPUT_LOCK_EN_MASK)) >> FLL_CTRL_CONF1_OUTPUT_LOCK_EN_SHIFT)

#define FLL_CTRL_CONF1_MODE_MASK                   (0x80000000U)
#define FLL_CTRL_CONF1_MODE_SHIFT                  (31U)
#define FLL_CTRL_CONF1_MODE(x)                     (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF1_MODE_SHIFT)) & FLL_CTRL_CONF1_MODE_MASK)
#define READ_FLL_CTRL_CONF1_MODE(x)                (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF1_MODE_MASK)) >> FLL_CTRL_CONF1_MODE_SHIFT)

/*! @name SOC_CONF2 - FLL_CTRL configuration 2 register */
#define FLL_CTRL_CONF2_LOOPGAIN_MASK               (0x0000000FU)
#define FLL_CTRL_CONF2_LOOPGAIN_SHIFT              (0U)
#define FLL_CTRL_CONF2_LOOPGAIN(x)                 (((uint32_t)(((uint32_t)(x)) /* << FLL_CTRL_CONF2_LOOPGAIN_SHIFT */)) & FLL_CTRL_CONF2_LOOPGAIN_MASK)
#define READ_FLL_CTRL_CONF2_LOOPGAIN(x)            (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_LOOPGAIN_MASK))/* >> FLL_CTRL_CONF2_LOOPGAIN_SHIFT*/)

#define FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK        (0x000003F0U)
#define FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT       (4U)
#define FLL_CTRL_CONF2_DEASSERT_CYCLES(x)          (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT)) & FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK)
#define READ_FLL_CTRL_CONF2_DEASSERT_CYCLES(x)     (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_DEASSERT_CYCLES_MASK)) >> FLL_CTRL_CONF2_DEASSERT_CYCLES_SHIFT)

#define FLL_CTRL_CONF2_ASSERT_CYCLES_MASK          (0x0000FC00U)
#define FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT         (10U)
#define FLL_CTRL_CONF2_ASSERT_CYCLES(x)            (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT)) & FLL_CTRL_CONF2_ASSERT_CYCLES_MASK)
#define READ_FLL_CTRL_CONF2_ASSERT_CYCLES(x)       (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_ASSERT_CYCLES_MASK)) >> FLL_CTRL_CONF2_ASSERT_CYCLES_SHIFT)

#define FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK         (0x0FFF0000U)
#define FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT        (16U)
#define FLL_CTRL_CONF2_LOCK_TOLERANCE(x)           (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT)) & FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK)
#define READ_FLL_CTRL_CONF2_LOCK_TOLERANCE(x)      (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_LOCK_TOLERANCE_MASK)) >> FLL_CTRL_CONF2_LOCK_TOLERANCE_SHIFT)

#define FLL_CTRL_CONF2_CONF_CLK_SEL_MASK           (0x20000000U)
#define FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT          (29U)
#define FLL_CTRL_CONF2_CONF_CLK_SEL(x)             (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT)) & FLL_CTRL_CONF2_CONF_CLK_SEL_MASK)
#define READ_FLL_CTRL_CONF2_CONF_CLK_SEL(x)        (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_CONF_CLK_SEL_MASK)) >> FLL_CTRL_CONF2_CONF_CLK_SEL_SHIFT)

#define FLL_CTRL_CONF2_OPEN_LOOP_MASK              (0x40000000U)
#define FLL_CTRL_CONF2_OPEN_LOOP_SHIFT             (30U)
#define FLL_CTRL_CONF2_OPEN_LOOP(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_OPEN_LOOP_SHIFT)) & FLL_CTRL_CONF2_OPEN_LOOP_MASK)
#define READ_FLL_CTRL_CONF2_OPEN_LOOP(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_OPEN_LOOP_MASK)) >> FLL_CTRL_CONF2_OPEN_LOOP_SHIFT)

#define FLL_CTRL_CONF2_DITHERING_MASK              (0x80000000U)
#define FLL_CTRL_CONF2_DITHERING_SHIFT             (31U)
#define FLL_CTRL_CONF2_DITHERING(x)                (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_CONF2_DITHERING_SHIFT)) & FLL_CTRL_CONF2_DITHERING_MASK)
#define READ_FLL_CTRL_CONF2_DITHERING(x)           (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_CONF2_DITHERING_MASK)) >> FLL_CTRL_CONF2_DITHERING_SHIFT)

/*! @name SOC_INTEGRATOR - FLL_CTRL configuration 2 register */
#define FLL_CTRL_INTEGRATOR_FRACT_PART_MASK        (0x0000FFC0U)
#define FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT       (6U)
#define FLL_CTRL_INTEGRATOR_FRACT_PART(x)          (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT)) & FLL_CTRL_INTEGRATOR_FRACT_PART_MASK)
#define READ_FLL_CTRL_INTEGRATOR_FRACT_PART(x)     (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_INTEGRATOR_FRACT_PART_MASK)) >> FLL_CTRL_INTEGRATOR_FRACT_PART_SHIFT)

#define FLL_CTRL_INTEGRATOR_INT_PART_MASK          (0x03FF0000U)
#define FLL_CTRL_INTEGRATOR_INT_PART_SHIFT         (16U)
#define FLL_CTRL_INTEGRATOR_INT_PART(x)            (((uint32_t)(((uint32_t)(x)) << FLL_CTRL_INTEGRATOR_INT_PART_SHIFT)) & FLL_CTRL_INTEGRATOR_INT_PART_MASK)
#define READ_FLL_CTRL_INTEGRATOR_INT_PART(x)       (((uint32_t)(((uint32_t)(x)) & FLL_CTRL_INTEGRATOR_INT_PART_MASK)) >> FLL_CTRL_INTEGRATOR_INT_PART_SHIFT)

#endif  /* __ARCHI_FLL_CTRL_MACROS_H__ */
