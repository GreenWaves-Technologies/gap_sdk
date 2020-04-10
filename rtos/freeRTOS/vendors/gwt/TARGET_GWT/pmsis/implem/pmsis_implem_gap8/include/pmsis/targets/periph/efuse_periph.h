/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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

#ifndef __EFUSE_PERIPH_H__
#define __EFUSE_PERIPH_H__

/* ----------------------------------------------------------------------------
   -- EFUSE CTRL Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_CTRL_Peripheral_Access_Layer EFUSE_CTRL Peripheral Access Layer
 * @{
 */

/** EFUSE_CTRL - Register Layout Typedef */
typedef struct {
 volatile  uint32_t cmd;                       /**< EFUSE_Control register, offset: 0x00 */
 volatile  uint32_t cfg;                       /**< EFUSE_Control register, offset: 0x04 */
} efuse_ctrl_t;

/* ----------------------------------------------------------------------------
   -- EFUSE_CTRL Register Masks
   ---------------------------------------------------------------------------- */
/*! @name CFG - EFUSE control configure register */
#define EFUSE_CTRL_SHORT_MASK                           (0x3FFU)
#define EFUSE_CTRL_SHORT_SHIFT                          (0U)
#define EFUSE_CTRL_SHORT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_CTRL_SHORT_SHIFT */)) & EFUSE_CTRL_SHORT_MASK)

#define EFUSE_CTRL_MEDIUM_MASK                          (0xFFC00U)
#define EFUSE_CTRL_MEDIUM_SHIFT                         (10U)
#define EFUSE_CTRL_MEDIUM(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_MEDIUM_SHIFT)) & EFUSE_CTRL_MEDIUM_MASK)

#define EFUSE_CTRL_LONG_MASK                            (0x3FF00000U)
#define EFUSE_CTRL_LONG_SHIFT                           (20U)
#define EFUSE_CTRL_LONG(x)                              (((uint32_t)(((uint32_t)(x)) << EFUSE_CTRL_LONG_SHIFT)) & EFUSE_CTRL_LONG_MASK)

/*!
 * @addtogroup EFUSE_CTRL_Register_Masks EFUSE_CTRL Register Masks
 * @{
 */
#define    EFUSE_CTRL_CMD_READ       0x1
#define    EFUSE_CTRL_CMD_WRITE      0x2
#define    EFUSE_CTRL_CMD_SLEEP      0x4
/*!
 * @}
 */ /* end of group EFUSE_CTRL_Register_Masks */


/* ----------------------------------------------------------------------------
   -- EFUSE REG Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Peripheral_Access_Layer EFUSE_REGS Peripheral Access Layer
 * @{
 */

/** EFUSE_REGS - Registers Layout Typedef */
typedef struct {
  __IO  uint32_t info;                    /**< EFUSE INFO register, offset: 0x000 */
  __IO  uint32_t info2;                   /**< EFUSE_INFO2 register, offset: 0x004 */
  __IO  uint32_t aes_key[16];             /**< EFUSE_AES_KEY registers, offset: 0x008 */
  __IO  uint32_t aes_iv[8];               /**< EFUSE_AES_IV registers, offset: 0x048 */
  __IO  uint32_t wait_xtal_delta_lsb;     /**< EFUSE_WAIT_XTAL_DELTA_LSB register, offset: 0x068 */
  __IO  uint32_t wait_xtal_delta_msb;     /**< EFUSE_WAIT_XTAL_DELTA_MSB register, offset: 0x06C */
  __IO  uint32_t wait_xtal_min;           /**< EFUSE_WAIT_XTAL_MIN registers, offset: 0x070 */
  __IO  uint32_t wait_xtal_max;           /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x074 */
  __IO  uint32_t hyper_rds_delay;         /**< EFUSE_WAIT_XTAL_MAX registers, offset: 0x078 */
  __IO  uint32_t fll_freq;                /**< EFUSE_FLL_FREQ registers, offset: 0x07C */
  __IO  uint32_t fll_tolerance;           /**< EFUSE_FLL_TOLERANCE registers, offset: 0x080 */
  __IO  uint32_t fll_assert_cycle;        /**< EFUSE_FLL_ASSERT_CYCLE registers, offset: 0x084 */
  __IO  uint32_t _reserved[6];            /**< EFUSE_reserved registers, offset: 0x088 */
  __IO  uint32_t user_reg[88];            /**< EFUSE_USER_REG, offset: 0x0A0 */
} efuse_regs_t;

/* ----------------------------------------------------------------------------
   -- EFUSE_REGS Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup EFUSE_REGS_Register_Masks EFUSE_REGS Register Masks
 * @{
 */
/*! @name INFO - EFUSE information register */
#define EFUSE_INFO_PLT_MASK                           (0x07U)
#define EFUSE_INFO_PLT_SHIFT                          (0U)
#define EFUSE_INFO_PLT(x)                             (((uint32_t)(((uint32_t)(x)) /* << EFUSE_INFO_PLT_SHIFT */)) & EFUSE_INFO_PLT_MASK)

#define EFUSE_INFO_BOOT_MASK                          (0x38U)
#define EFUSE_INFO_BOOT_SHIFT                         (3U)
#define EFUSE_INFO_BOOT(x)                            (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_BOOT_SHIFT)) & EFUSE_INFO_BOOT_MASK)

#define EFUSE_INFO_ENCRYPTED_MASK                     (0x40U)
#define EFUSE_INFO_ENCRYPTED_SHIFT                    (6U)
#define EFUSE_INFO_ENCRYPTED(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_ENCRYPTED_SHIFT)) & EFUSE_INFO_ENCRYPTED_MASK)

#define EFUSE_INFO_WAIT_XTAL_MASK                     (0x80U)
#define EFUSE_INFO_WAIT_XTAL_SHIFT                    (7U)
#define EFUSE_INFO_WAIT_XTAL(x)                       (((uint32_t)(((uint32_t)(x)) << EFUSE_INFO_WAIT_XTAL_SHIFT)) & EFUSE_INFO_WAIT_XTAL_MASK)


/*!
 * @}
 */ /* end of group EFUSE_REGS_Register_Masks */

#endif /* __EFUSE_PERIPH_H__ */
