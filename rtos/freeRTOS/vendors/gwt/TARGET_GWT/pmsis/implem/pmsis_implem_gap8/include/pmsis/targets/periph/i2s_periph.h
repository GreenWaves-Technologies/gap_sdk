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

#ifndef __I2S_PERIPH_H__
#define __I2S_PERIPH_H__

#include "udma_core_periph.h"


/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** I2S_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t ext_clk; /**< External clock configuration */
    volatile uint32_t cfg_clkgen0; /**< Clock/WS generator 0 configuration */
    volatile uint32_t cfg_clkgen1; /**< Clock/WS generator 1 configuration */
    volatile uint32_t chmode; /**< Channels mode configuration */
    volatile uint32_t filt_ch0; /**< Channel 0 filtering configuration */
    volatile uint32_t filt_ch1; /**< Channel 1 filtering configuration */
} i2s_t;


/* ----------------------------------------------------------------------------
   -- I2S Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name EXT_CLK */
/* When using EXT Clock and internal WS, selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
#define I2S_EXT_CLK_EXT_BITS_WORD_MASK                               (0x1f)
#define I2S_EXT_CLK_EXT_BITS_WORD_SHIFT                              (0)
#define I2S_EXT_CLK_EXT_BITS_WORD(val)                               (((uint32_t)(((uint32_t)(val)) << I2S_EXT_CLK_EXT_BITS_WORD_SHIFT)) & I2S_EXT_CLK_EXT_BITS_WORD_MASK)


/*! @name CFG_CLKGEN0 */
/* Selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
#define I2S_CFG_CLKGEN0_BITS_WORD_MASK                               (0x1f)
#define I2S_CFG_CLKGEN0_BITS_WORD_SHIFT                              (0)
#define I2S_CFG_CLKGEN0_BITS_WORD(val)                               (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN0_BITS_WORD_SHIFT)) & I2S_CFG_CLKGEN0_BITS_WORD_MASK)

/* When set enables the clkgen0 clock and ws generator */
#define I2S_CFG_CLKGEN0_CLK_EN_MASK                                  (0x100)
#define I2S_CFG_CLKGEN0_CLK_EN_SHIFT                                 (8)
#define I2S_CFG_CLKGEN0_CLK_EN(val)                                  (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN0_CLK_EN_SHIFT)) & I2S_CFG_CLKGEN0_CLK_EN_MASK)

/* Sets the clock divider ratio for clockgen0 */
#define I2S_CFG_CLKGEN0_CLK_DIV_MASK                                 (0xffff0000)
#define I2S_CFG_CLKGEN0_CLK_DIV_SHIFT                                (16)
#define I2S_CFG_CLKGEN0_CLK_DIV(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN0_CLK_DIV_SHIFT)) & I2S_CFG_CLKGEN0_CLK_DIV_MASK)


/*! @name CFG_CLKGEN1 */
/* Selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
#define I2S_CFG_CLKGEN1_BITS_WORD_MASK                               (0x1f)
#define I2S_CFG_CLKGEN1_BITS_WORD_SHIFT                              (0)
#define I2S_CFG_CLKGEN1_BITS_WORD(val)                               (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN1_BITS_WORD_SHIFT)) & I2S_CFG_CLKGEN1_BITS_WORD_MASK)

/* When set enables the clkgen1 clock and ws generator */
#define I2S_CFG_CLKGEN1_CLK_EN_MASK                                  (0x100)
#define I2S_CFG_CLKGEN1_CLK_EN_SHIFT                                 (8)
#define I2S_CFG_CLKGEN1_CLK_EN(val)                                  (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN1_CLK_EN_SHIFT)) & I2S_CFG_CLKGEN1_CLK_EN_MASK)

/* Sets the clock divider ratio for clockgen1 */
#define I2S_CFG_CLKGEN1_CLK_DIV_MASK                                 (0xffff0000)
#define I2S_CFG_CLKGEN1_CLK_DIV_SHIFT                                (16)
#define I2S_CFG_CLKGEN1_CLK_DIV(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_CFG_CLKGEN1_CLK_DIV_SHIFT)) & I2S_CFG_CLKGEN1_CLK_DIV_MASK)


/*! @name CHMODE */
/* RESERVED */
#define I2S_CHMODE_CH0_SNAP_CAM_MASK                                 (0x1)
#define I2S_CHMODE_CH0_SNAP_CAM_SHIFT                                (0)
#define I2S_CHMODE_CH0_SNAP_CAM(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_SNAP_CAM_SHIFT)) & I2S_CHMODE_CH0_SNAP_CAM_MASK)

/* Enables LSB shifting for channel 0 */
#define I2S_CHMODE_CH0_LSB_FIRST_MASK                                (0x10)
#define I2S_CHMODE_CH0_LSB_FIRST_SHIFT                               (4)
#define I2S_CHMODE_CH0_LSB_FIRST(val)                                (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_LSB_FIRST_SHIFT)) & I2S_CHMODE_CH0_LSB_FIRST_MASK)

/* Enables PDM filter for channel 0 */
#define I2S_CHMODE_CH0_PDM_USEFILTER_MASK                            (0x100)
#define I2S_CHMODE_CH0_PDM_USEFILTER_SHIFT                           (8)
#define I2S_CHMODE_CH0_PDM_USEFILTER(val)                            (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_PDM_USEFILTER_SHIFT)) & I2S_CHMODE_CH0_PDM_USEFILTER_MASK)

/* Enables PDM demodulation on channel 0 */
#define I2S_CHMODE_CH0_PDM_EN_MASK                                   (0x1000)
#define I2S_CHMODE_CH0_PDM_EN_SHIFT                                  (12)
#define I2S_CHMODE_CH0_PDM_EN(val)                                   (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_PDM_EN_SHIFT)) & I2S_CHMODE_CH0_PDM_EN_MASK)

/* Enables Double Data Rate(Sampling on both edges) for channel 0 */
#define I2S_CHMODE_CH0_USEDDR_MASK                                   (0x10000)
#define I2S_CHMODE_CH0_USEDDR_SHIFT                                  (16)
#define I2S_CHMODE_CH0_USEDDR(val)                                   (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_USEDDR_SHIFT)) & I2S_CHMODE_CH0_USEDDR_MASK)

/* Selects the mode for channel 0:
  - 2'b00: Use clkgen0 (clock and WS generated by clkgen)
  - 2'b01: Use clkgen1 (clock and WS generated by clkgen)
  - 2'b10: External clock but internally generated WS
  - 2'b11: External clock and external WS */
#define I2S_CHMODE_CH0_MODE_MASK                                     (0x3000000)
#define I2S_CHMODE_CH0_MODE_SHIFT                                    (24)
#define I2S_CHMODE_CH0_MODE(val)                                     (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH0_MODE_SHIFT)) & I2S_CHMODE_CH0_MODE_MASK)

/* RESERVED */
#define I2S_CHMODE_CH1_SNAP_CAM_MASK                                 (0x2)
#define I2S_CHMODE_CH1_SNAP_CAM_SHIFT                                (1)
#define I2S_CHMODE_CH1_SNAP_CAM(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_SNAP_CAM_SHIFT)) & I2S_CHMODE_CH1_SNAP_CAM_MASK)

/* Enables LSB shifting for channel 1 */
#define I2S_CHMODE_CH1_LSB_FIRST_MASK                                (0x20)
#define I2S_CHMODE_CH1_LSB_FIRST_SHIFT                               (5)
#define I2S_CHMODE_CH1_LSB_FIRST(val)                                (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_LSB_FIRST_SHIFT)) & I2S_CHMODE_CH1_LSB_FIRST_MASK)

/* Enables PDM filter for channel 1 */
#define I2S_CHMODE_CH1_PDM_USEFILTER_MASK                            (0x200)
#define I2S_CHMODE_CH1_PDM_USEFILTER_SHIFT                           (9)
#define I2S_CHMODE_CH1_PDM_USEFILTER(val)                            (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_PDM_USEFILTER_SHIFT)) & I2S_CHMODE_CH1_PDM_USEFILTER_MASK)

/* Enables PDM demodulation on channel 1 */
#define I2S_CHMODE_CH1_PDM_EN_MASK                                   (0x2000)
#define I2S_CHMODE_CH1_PDM_EN_SHIFT                                  (13)
#define I2S_CHMODE_CH1_PDM_EN(val)                                   (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_PDM_EN_SHIFT)) & I2S_CHMODE_CH1_PDM_EN_MASK)

/* Enables Double Data Rate(Sampling on both edges) for channel 1 */
#define I2S_CHMODE_CH1_USEDDR_MASK                                   (0x20000)
#define I2S_CHMODE_CH1_USEDDR_SHIFT                                  (17)
#define I2S_CHMODE_CH1_USEDDR(val)                                   (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_USEDDR_SHIFT)) & I2S_CHMODE_CH1_USEDDR_MASK)

/* Selects the mode for channel 1:
  - 2'b00: Use clkgen0 (clock and WS generated by clkgen)
  - 2'b01: Use clkgen1 (clock and WS generated by clkgen)
  - 2'b10: External clock but internally generated WS
  - 2'b11: External clock and external WS */
#define I2S_CHMODE_CH1_MODE_MASK                                     (0xc000000)
#define I2S_CHMODE_CH1_MODE_SHIFT                                    (26)
#define I2S_CHMODE_CH1_MODE(val)                                     (((uint32_t)(((uint32_t)(val)) << I2S_CHMODE_CH1_MODE_SHIFT)) & I2S_CHMODE_CH1_MODE_MASK)


/*! @name FILT_CH0 */
/* Sets the decimation value for channel 0 */
#define I2S_FILT_CH0_DECIMATION_MASK                                 (0x3ff)
#define I2S_FILT_CH0_DECIMATION_SHIFT                                (0)
#define I2S_FILT_CH0_DECIMATION(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_FILT_CH0_DECIMATION_SHIFT)) & I2S_FILT_CH0_DECIMATION_MASK)

/* Sets the right shift value for channel 0 */
#define I2S_FILT_CH0_SHIFT_MASK                                      (0x70000)
#define I2S_FILT_CH0_SHIFT_SHIFT                                     (16)
#define I2S_FILT_CH0_SHIFT(val)                                      (((uint32_t)(((uint32_t)(val)) << I2S_FILT_CH0_SHIFT_SHIFT)) & I2S_FILT_CH0_SHIFT_MASK)


/*! @name FILT_CH1 */
/* Sets the decimation value for channel 1 */
#define I2S_FILT_CH1_DECIMATION_MASK                                 (0x3ff)
#define I2S_FILT_CH1_DECIMATION_SHIFT                                (0)
#define I2S_FILT_CH1_DECIMATION(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_FILT_CH1_DECIMATION_SHIFT)) & I2S_FILT_CH1_DECIMATION_MASK)

/* Sets the right shift value for channel 1 */
#define I2S_FILT_CH1_SHIFT_MASK                                      (0x70000)
#define I2S_FILT_CH1_SHIFT_SHIFT                                     (16)
#define I2S_FILT_CH1_SHIFT(val)                                      (((uint32_t)(((uint32_t)(val)) << I2S_FILT_CH1_SHIFT_SHIFT)) & I2S_FILT_CH1_SHIFT_MASK)


/*! @name EXT_CLK */
typedef union
{
    struct
    {
        /* When using EXT Clock and internal WS, selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
        uint32_t ext_bits_word:5;
    } field;
    uint32_t word;
} i2s_ext_clk_t;

/*! @name CFG_CLKGEN0 */
typedef union
{
    struct
    {
        /* Selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
        uint32_t bits_word:5;
        uint32_t reserved_0:3;
        /* When set enables the clkgen0 clock and ws generator */
        uint32_t clk_en:1;
        uint32_t reserved_1:7;
        /* Sets the clock divider ratio for clockgen0 */
        uint32_t clk_div:16;
    } field;
    uint32_t word;
} i2s_cfg_clkgen0_t;

/*! @name CFG_CLKGEN1 */
typedef union
{
    struct
    {
        /* Selects after how many bits the WS is toggled. The value written here is (num bits – 1). */
        uint32_t bits_word:5;
        uint32_t reserved_0:3;
        /* When set enables the clkgen1 clock and ws generator */
        uint32_t clk_en:1;
        uint32_t reserved_1:7;
        /* Sets the clock divider ratio for clockgen1 */
        uint32_t clk_div:16;
    } field;
    uint32_t word;
} i2s_cfg_clkgen1_t;

/*! @name CHMODE */
typedef union
{
    struct
    {
        /* RESERVED */
        uint32_t ch0_snap_cam:1;
        uint32_t reserved_0:3;
        /* Enables LSB shifting for channel 0 */
        uint32_t ch0_lsb_first:1;
        uint32_t reserved_1:3;
        /* Enables PDM filter for channel 0 */
        uint32_t ch0_pdm_usefilter:1;
        uint32_t reserved_2:3;
        /* Enables PDM demodulation on channel 0 */
        uint32_t ch0_pdm_en:1;
        uint32_t reserved_3:3;
        /* Enables Double Data Rate(Sampling on both edges) for channel 0 */
        uint32_t ch0_useddr:1;
        uint32_t reserved_4:7;
        /* Selects the mode for channel 0:
        - 2'b00: Use clkgen0 (clock and WS generated by clkgen)
        - 2'b01: Use clkgen1 (clock and WS generated by clkgen)
        - 2'b10: External clock but internally generated WS
        - 2'b11: External clock and external WS */
        uint32_t ch0_mode:2;
        /* RESERVED */
        uint32_t ch1_snap_cam:1;
        uint32_t reserved_5:3;
        /* Enables LSB shifting for channel 1 */
        uint32_t ch1_lsb_first:1;
        uint32_t reserved_6:3;
        /* Enables PDM filter for channel 1 */
        uint32_t ch1_pdm_usefilter:1;
        uint32_t reserved_7:3;
        /* Enables PDM demodulation on channel 1 */
        uint32_t ch1_pdm_en:1;
        uint32_t reserved_8:3;
        /* Enables Double Data Rate(Sampling on both edges) for channel 1 */
        uint32_t ch1_useddr:1;
        uint32_t reserved_9:8;
        /* Selects the mode for channel 1:
        - 2'b00: Use clkgen0 (clock and WS generated by clkgen)
        - 2'b01: Use clkgen1 (clock and WS generated by clkgen)
        - 2'b10: External clock but internally generated WS
        - 2'b11: External clock and external WS */
        uint32_t ch1_mode:2;
    } field;
    uint32_t word;
} i2s_chmode_t;

/*! @name FILT_CH0 */
typedef union
{
    struct
    {
        /* Sets the decimation value for channel 0 */
        uint32_t decimation:10;
        uint32_t reserved_0:6;
        /* Sets the right shift value for channel 0 */
        uint32_t shift:3;
    } field;
    uint32_t word;
} i2s_filt_ch0_t;

/*! @name FILT_CH1 */
typedef union
{
    struct
    {
        /* Sets the decimation value for channel 1 */
        uint32_t decimation:10;
        uint32_t reserved_0:6;
        /* Sets the right shift value for channel 1 */
        uint32_t shift:3;
    } field;
    uint32_t word;
} i2s_filt_ch1_t;



#endif /* __I2S_PERIPH_H__ */
