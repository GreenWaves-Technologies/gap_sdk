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

#ifndef __CPI_PERIPH_H__
#define __CPI_PERIPH_H__

//#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- CPI Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** CPI_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t cfg_glob; /**< Global configuration register */
    volatile uint32_t cfg_ll; /**< Lower Left corner configuration register */
    volatile uint32_t cfg_ur; /**< Upper Right corner configuration register */
    volatile uint32_t cfg_size; /**< Horizontal Resolution configuration register */
    volatile uint32_t cfg_filter; /**< RGB coefficients configuration register */
} cpi_t;


/* ----------------------------------------------------------------------------
   -- CPI Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name CFG_GLOB */
/* Frame dropping:
  - 1'b0: disable
  - 1'b1: enable */
#define CPI_CFG_GLOB_FRAMEDROP_EN_MASK                               (0x1)
#define CPI_CFG_GLOB_FRAMEDROP_EN_SHIFT                              (0)
#define CPI_CFG_GLOB_FRAMEDROP_EN(val)                               (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_FRAMEDROP_EN_SHIFT)) & CPI_CFG_GLOB_FRAMEDROP_EN_MASK)

/* Sets how many frames should be dropped after each received. */
#define CPI_CFG_GLOB_FRAMEDROP_VAL_MASK                              (0x7e)
#define CPI_CFG_GLOB_FRAMEDROP_VAL_SHIFT                             (1)
#define CPI_CFG_GLOB_FRAMEDROP_VAL(val)                              (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_FRAMEDROP_VAL_SHIFT)) & CPI_CFG_GLOB_FRAMEDROP_VAL_MASK)

/* Input frame slicing:
  - 1'b0: disable
  - 1'b1: enable */
#define CPI_CFG_GLOB_FRAMESLICE_EN_MASK                              (0x80)
#define CPI_CFG_GLOB_FRAMESLICE_EN_SHIFT                             (7)
#define CPI_CFG_GLOB_FRAMESLICE_EN(val)                              (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_FRAMESLICE_EN_SHIFT)) & CPI_CFG_GLOB_FRAMESLICE_EN_MASK)

/* Input frame format:
  - 3'b000: RGB565
  - 3'b001: RGB555
  - 3'b010: RGB444
  - 3'b100: BYPASS_LITEND
  - 3’b101: BYPASS_BIGEND */
#define CPI_CFG_GLOB_FORMAT_MASK                                     (0x700)
#define CPI_CFG_GLOB_FORMAT_SHIFT                                    (8)
#define CPI_CFG_GLOB_FORMAT(val)                                     (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_FORMAT_SHIFT)) & CPI_CFG_GLOB_FORMAT_MASK)

/* Right shift of final pixel value (DivFactor)
  NOTE: not used if FORMAT == BYPASS */
#define CPI_CFG_GLOB_SHIFT_MASK                                      (0x7800)
#define CPI_CFG_GLOB_SHIFT_SHIFT                                     (11)
#define CPI_CFG_GLOB_SHIFT(val)                                      (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_SHIFT_SHIFT)) & CPI_CFG_GLOB_SHIFT_MASK)

/* Enable data rx from camera interface. 
  The enable/disable happens only at the start of a frame.
  - 1'b0: disable
  - 1'b1: enable */
#define CPI_CFG_GLOB_EN_MASK                                         (0x80000000)
#define CPI_CFG_GLOB_EN_SHIFT                                        (31)
#define CPI_CFG_GLOB_EN(val)                                         (((uint32_t)(((uint32_t)(val)) << CPI_CFG_GLOB_EN_SHIFT)) & CPI_CFG_GLOB_EN_MASK)


/*! @name CFG_LL */
/* X coordinate of lower left corner of slice */
#define CPI_CFG_LL_FRAMESLICE_LLX_MASK                               (0xffff)
#define CPI_CFG_LL_FRAMESLICE_LLX_SHIFT                              (0)
#define CPI_CFG_LL_FRAMESLICE_LLX(val)                               (((uint32_t)(((uint32_t)(val)) << CPI_CFG_LL_FRAMESLICE_LLX_SHIFT)) & CPI_CFG_LL_FRAMESLICE_LLX_MASK)

/* Y coordinate of lower left corner of slice */
#define CPI_CFG_LL_FRAMESLICE_LLY_MASK                               (0xffff0000)
#define CPI_CFG_LL_FRAMESLICE_LLY_SHIFT                              (16)
#define CPI_CFG_LL_FRAMESLICE_LLY(val)                               (((uint32_t)(((uint32_t)(val)) << CPI_CFG_LL_FRAMESLICE_LLY_SHIFT)) & CPI_CFG_LL_FRAMESLICE_LLY_MASK)


/*! @name CFG_UR */
/* X coordinate of upper right corner of slice */
#define CPI_CFG_UR_FRAMESLICE_URX_MASK                               (0xffff)
#define CPI_CFG_UR_FRAMESLICE_URX_SHIFT                              (0)
#define CPI_CFG_UR_FRAMESLICE_URX(val)                               (((uint32_t)(((uint32_t)(val)) << CPI_CFG_UR_FRAMESLICE_URX_SHIFT)) & CPI_CFG_UR_FRAMESLICE_URX_MASK)

/* Y coordinate of upper right corner of slice */
#define CPI_CFG_UR_FRAMESLICE_URY_MASK                               (0xffff0000)
#define CPI_CFG_UR_FRAMESLICE_URY_SHIFT                              (16)
#define CPI_CFG_UR_FRAMESLICE_URY(val)                               (((uint32_t)(((uint32_t)(val)) << CPI_CFG_UR_FRAMESLICE_URY_SHIFT)) & CPI_CFG_UR_FRAMESLICE_URY_MASK)


/*! @name CFG_SIZE */
/* Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1). */
#define CPI_CFG_SIZE_ROWLEN_MASK                                     (0xffff0000)
#define CPI_CFG_SIZE_ROWLEN_SHIFT                                    (16)
#define CPI_CFG_SIZE_ROWLEN(val)                                     (((uint32_t)(((uint32_t)(val)) << CPI_CFG_SIZE_ROWLEN_SHIFT)) & CPI_CFG_SIZE_ROWLEN_MASK)


/*! @name CFG_FILTER */
/* Coefficient that multiplies the B component
  NOTE: not used if FORMAT == BYPASS */
#define CPI_CFG_FILTER_B_COEFF_MASK                                  (0xff)
#define CPI_CFG_FILTER_B_COEFF_SHIFT                                 (0)
#define CPI_CFG_FILTER_B_COEFF(val)                                  (((uint32_t)(((uint32_t)(val)) << CPI_CFG_FILTER_B_COEFF_SHIFT)) & CPI_CFG_FILTER_B_COEFF_MASK)

/* Coefficient that multiplies the G component
  NOTE: not used if FORMAT == BYPASS */
#define CPI_CFG_FILTER_G_COEFF_MASK                                  (0xff00)
#define CPI_CFG_FILTER_G_COEFF_SHIFT                                 (8)
#define CPI_CFG_FILTER_G_COEFF(val)                                  (((uint32_t)(((uint32_t)(val)) << CPI_CFG_FILTER_G_COEFF_SHIFT)) & CPI_CFG_FILTER_G_COEFF_MASK)

/* Coefficient that multiplies the R component
  NOTE: not used if FORMAT == BYPASS */
#define CPI_CFG_FILTER_R_COEFF_MASK                                  (0xff0000)
#define CPI_CFG_FILTER_R_COEFF_SHIFT                                 (16)
#define CPI_CFG_FILTER_R_COEFF(val)                                  (((uint32_t)(((uint32_t)(val)) << CPI_CFG_FILTER_R_COEFF_SHIFT)) & CPI_CFG_FILTER_R_COEFF_MASK)

/*! @name CFG_GLOB */
typedef union
{
    struct
    {
        /* Frame dropping:
        - 1'b0: disable
        - 1'b1: enable */
        uint32_t framedrop_en:1;
        /* Sets how many frames should be dropped after each received. */
        uint32_t framedrop_val:6;
        /* Input frame slicing:
        - 1'b0: disable
        - 1'b1: enable */
        uint32_t frameslice_en:1;
        /* Input frame format:
        - 3'b000: RGB565
        - 3'b001: RGB555
        - 3'b010: RGB444
        - 3'b100: BYPASS_LITEND
        - 3’b101: BYPASS_BIGEND */
        uint32_t format:3;
        /* Right shift of final pixel value (DivFactor)
        NOTE: not used if FORMAT == BYPASS */
        uint32_t shift:4;
        /* Reserved/Not used. */
        uint32_t reserved_0:16;
        /* Enable data rx from camera interface. 
        The enable/disable happens only at the start of a frame.
        - 1'b0: disable
        - 1'b1: enable */
        uint32_t en:1;
    } field;
    uint32_t word;
} cpi_cfg_glob_t;

/*! @name CFG_LL */
typedef union
{
    struct
    {
        /* X coordinate of lower left corner of slice */
        uint32_t frameslice_llx:16;
        /* Y coordinate of lower left corner of slice */
        uint32_t frameslice_lly:16;
    } field;
    uint32_t word;
} cpi_cfg_ll_t;

/*! @name CFG_UR */
typedef union
{
    struct
    {
        /* X coordinate of upper right corner of slice */
        uint32_t frameslice_urx:16;
        /* Y coordinate of upper right corner of slice */
        uint32_t frameslice_ury:16;
    } field;
    uint32_t word;
} cpi_cfg_ur_t;

/*! @name CFG_SIZE */
typedef union
{
    struct
    {
        /* Reserved/Not used. */
        uint32_t reserved_0:16;
        /* Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1). */
        uint32_t rowlen:16;
    } field;
    uint32_t word;
} cpi_cfg_size_t;

/*! @name CFG_FILTER */
typedef union
{
    struct
    {
        /* Coefficient that multiplies the B component
        NOTE: not used if FORMAT == BYPASS */
        uint32_t b_coeff:8;
        /* Coefficient that multiplies the G component
        NOTE: not used if FORMAT == BYPASS */
        uint32_t g_coeff:8;
        /* Coefficient that multiplies the R component
        NOTE: not used if FORMAT == BYPASS */
        uint32_t r_coeff:8;
        /* Reserved/Not used. */
        uint32_t reserved_0:8;
    } field;
    uint32_t word;
} cpi_cfg_filter_t;


#endif /* __CPI_PERIPH_H__ */
