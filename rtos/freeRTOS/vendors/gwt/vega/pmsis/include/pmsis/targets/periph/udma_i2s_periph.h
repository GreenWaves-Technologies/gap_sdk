/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2021 ETH Zurich, University of Bologna
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

#ifndef __UDMA_I2S_PERIPH_H__
#define __UDMA_I2S_PERIPH_H__


/* ----------------------------------------------------------------------------
   -- I2S Peripheral Access Layer --
   ---------------------------------------------------------------------------- */

/** I2S_Type Register Layout Typedef */
typedef struct
{
    udma_core_t udma; /**< UDMA channels struct. */
    volatile uint32_t clkcfg_setup; /**< Clock configuration for both master, slave and pdm */
    volatile uint32_t slv_setup; /**< Configuration of I2S slave */
    volatile uint32_t mst_setup; /**< Configuration of I2S master */
    volatile uint32_t pdm_setup; /**< Configuration of PDM module */
} i2s_t;


/* ----------------------------------------------------------------------------
   -- I2S Register Bitfield Access --
   ---------------------------------------------------------------------------- */

/*! @name CLKCFG_SETUP */
/* LSB of master clock divider */
#define I2S_CLKCFG_SETUP_MASTER_CLK_DIV_MASK                         (0xff)
#define I2S_CLKCFG_SETUP_MASTER_CLK_DIV_SHIFT                        (0)
#define I2S_CLKCFG_SETUP_MASTER_CLK_DIV(val)                         (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_MASTER_CLK_DIV_SHIFT)) & I2S_CLKCFG_SETUP_MASTER_CLK_DIV_MASK)

/* LSB of slave clock divider */
#define I2S_CLKCFG_SETUP_SLAVE_CLK_DIV_MASK                          (0xff00)
#define I2S_CLKCFG_SETUP_SLAVE_CLK_DIV_SHIFT                         (8)
#define I2S_CLKCFG_SETUP_SLAVE_CLK_DIV(val)                          (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_SLAVE_CLK_DIV_SHIFT)) & I2S_CLKCFG_SETUP_SLAVE_CLK_DIV_MASK)

/* MSBs of both master and slave clock divider */
#define I2S_CLKCFG_SETUP_COMMON_CLK_DIV_MASK                         (0xff0000)
#define I2S_CLKCFG_SETUP_COMMON_CLK_DIV_SHIFT                        (16)
#define I2S_CLKCFG_SETUP_COMMON_CLK_DIV(val)                         (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_COMMON_CLK_DIV_SHIFT)) & I2S_CLKCFG_SETUP_COMMON_CLK_DIV_MASK)

/* Enables Slave clock */
#define I2S_CLKCFG_SETUP_SLAVE_CLK_EN_MASK                           (0x1000000)
#define I2S_CLKCFG_SETUP_SLAVE_CLK_EN_SHIFT                          (24)
#define I2S_CLKCFG_SETUP_SLAVE_CLK_EN(val)                           (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_SLAVE_CLK_EN_SHIFT)) & I2S_CLKCFG_SETUP_SLAVE_CLK_EN_MASK)

/* Enables Master clock */
#define I2S_CLKCFG_SETUP_MASTER_CLK_EN_MASK                          (0x2000000)
#define I2S_CLKCFG_SETUP_MASTER_CLK_EN_SHIFT                         (25)
#define I2S_CLKCFG_SETUP_MASTER_CLK_EN(val)                          (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_MASTER_CLK_EN_SHIFT)) & I2S_CLKCFG_SETUP_MASTER_CLK_EN_MASK)

/* When enabled slave output clock is taken from PDM module */
#define I2S_CLKCFG_SETUP_PDM_CLK_EN_MASK                             (0x4000000)
#define I2S_CLKCFG_SETUP_PDM_CLK_EN_SHIFT                            (26)
#define I2S_CLKCFG_SETUP_PDM_CLK_EN(val)                             (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_PDM_CLK_EN_SHIFT)) & I2S_CLKCFG_SETUP_PDM_CLK_EN_MASK)

/* When set uses external clock for slave */
#define I2S_CLKCFG_SETUP_SLAVE_EXT_MASK                              (0x10000000)
#define I2S_CLKCFG_SETUP_SLAVE_EXT_SHIFT                             (28)
#define I2S_CLKCFG_SETUP_SLAVE_EXT(val)                              (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_SLAVE_EXT_SHIFT)) & I2S_CLKCFG_SETUP_SLAVE_EXT_MASK)

/* Selects slave clock source(either ext or generated):
  -1’b0:selects master
  -1’b1:selects slave */
#define I2S_CLKCFG_SETUP_SLAVE_NUM_MASK                              (0x20000000)
#define I2S_CLKCFG_SETUP_SLAVE_NUM_SHIFT                             (29)
#define I2S_CLKCFG_SETUP_SLAVE_NUM(val)                              (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_SLAVE_NUM_SHIFT)) & I2S_CLKCFG_SETUP_SLAVE_NUM_MASK)

/* When set uses external clock for master */
#define I2S_CLKCFG_SETUP_MASTER_EXT_MASK                             (0x40000000)
#define I2S_CLKCFG_SETUP_MASTER_EXT_SHIFT                            (30)
#define I2S_CLKCFG_SETUP_MASTER_EXT(val)                             (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_MASTER_EXT_SHIFT)) & I2S_CLKCFG_SETUP_MASTER_EXT_MASK)

/* Selects master clock source(either ext or generated):
  -1’b0:selects master
  -1’b1:selects slave */
#define I2S_CLKCFG_SETUP_MASTER_NUM_MASK                             (0x80000000)
#define I2S_CLKCFG_SETUP_MASTER_NUM_SHIFT                            (31)
#define I2S_CLKCFG_SETUP_MASTER_NUM(val)                             (((uint32_t)(((uint32_t)(val)) << I2S_CLKCFG_SETUP_MASTER_NUM_SHIFT)) & I2S_CLKCFG_SETUP_MASTER_NUM_MASK)


/*! @name SLV_SETUP */
/* Sets how many words for each I2S phase */
#define I2S_SLV_SETUP_SLAVE_WORDS_MASK                               (0x7)
#define I2S_SLV_SETUP_SLAVE_WORDS_SHIFT                              (0)
#define I2S_SLV_SETUP_SLAVE_WORDS(val)                               (((uint32_t)(((uint32_t)(val)) << I2S_SLV_SETUP_SLAVE_WORDS_SHIFT)) & I2S_SLV_SETUP_SLAVE_WORDS_MASK)

/* Sets how many bits per word */
#define I2S_SLV_SETUP_SLAVE_BITS_MASK                                (0x1f00)
#define I2S_SLV_SETUP_SLAVE_BITS_SHIFT                               (8)
#define I2S_SLV_SETUP_SLAVE_BITS(val)                                (((uint32_t)(((uint32_t)(val)) << I2S_SLV_SETUP_SLAVE_BITS_SHIFT)) & I2S_SLV_SETUP_SLAVE_BITS_MASK)

/* Enables LSB shifting */
#define I2S_SLV_SETUP_SLAVE_LSB_MASK                                 (0x10000)
#define I2S_SLV_SETUP_SLAVE_LSB_SHIFT                                (16)
#define I2S_SLV_SETUP_SLAVE_LSB(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_SLV_SETUP_SLAVE_LSB_SHIFT)) & I2S_SLV_SETUP_SLAVE_LSB_MASK)

/* Enables both channels */
#define I2S_SLV_SETUP_SLAVE_2CH_MASK                                 (0x20000)
#define I2S_SLV_SETUP_SLAVE_2CH_SHIFT                                (17)
#define I2S_SLV_SETUP_SLAVE_2CH(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_SLV_SETUP_SLAVE_2CH_SHIFT)) & I2S_SLV_SETUP_SLAVE_2CH_MASK)

/* Enables the Slave */
#define I2S_SLV_SETUP_SLAVE_EN_MASK                                  (0x80000000)
#define I2S_SLV_SETUP_SLAVE_EN_SHIFT                                 (31)
#define I2S_SLV_SETUP_SLAVE_EN(val)                                  (((uint32_t)(((uint32_t)(val)) << I2S_SLV_SETUP_SLAVE_EN_SHIFT)) & I2S_SLV_SETUP_SLAVE_EN_MASK)


/*! @name MST_SETUP */
/* Sets how many words for each I2S phase */
#define I2S_MST_SETUP_MASTER_WORDS_MASK                              (0x7)
#define I2S_MST_SETUP_MASTER_WORDS_SHIFT                             (0)
#define I2S_MST_SETUP_MASTER_WORDS(val)                              (((uint32_t)(((uint32_t)(val)) << I2S_MST_SETUP_MASTER_WORDS_SHIFT)) & I2S_MST_SETUP_MASTER_WORDS_MASK)

/* Sets how many bits per word */
#define I2S_MST_SETUP_MASTER_BITS_MASK                               (0x1f00)
#define I2S_MST_SETUP_MASTER_BITS_SHIFT                              (8)
#define I2S_MST_SETUP_MASTER_BITS(val)                               (((uint32_t)(((uint32_t)(val)) << I2S_MST_SETUP_MASTER_BITS_SHIFT)) & I2S_MST_SETUP_MASTER_BITS_MASK)

/* Enables LSB shifting */
#define I2S_MST_SETUP_MASTER_LSB_MASK                                (0x10000)
#define I2S_MST_SETUP_MASTER_LSB_SHIFT                               (16)
#define I2S_MST_SETUP_MASTER_LSB(val)                                (((uint32_t)(((uint32_t)(val)) << I2S_MST_SETUP_MASTER_LSB_SHIFT)) & I2S_MST_SETUP_MASTER_LSB_MASK)

/* Enables both channels */
#define I2S_MST_SETUP_MASTER_2CH_MASK                                (0x20000)
#define I2S_MST_SETUP_MASTER_2CH_SHIFT                               (17)
#define I2S_MST_SETUP_MASTER_2CH(val)                                (((uint32_t)(((uint32_t)(val)) << I2S_MST_SETUP_MASTER_2CH_SHIFT)) & I2S_MST_SETUP_MASTER_2CH_MASK)

/* Enables the Master */
#define I2S_MST_SETUP_MASTER_EN_MASK                                 (0x80000000)
#define I2S_MST_SETUP_MASTER_EN_SHIFT                                (31)
#define I2S_MST_SETUP_MASTER_EN(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_MST_SETUP_MASTER_EN_SHIFT)) & I2S_MST_SETUP_MASTER_EN_MASK)


/*! @name PDM_SETUP */
/* Shifts the output of the filter */
#define I2S_PDM_SETUP_PDM_SHIFT_MASK                                 (0x7)
#define I2S_PDM_SETUP_PDM_SHIFT_SHIFT                                (0)
#define I2S_PDM_SETUP_PDM_SHIFT(val)                                 (((uint32_t)(((uint32_t)(val)) << I2S_PDM_SETUP_PDM_SHIFT_SHIFT)) & I2S_PDM_SETUP_PDM_SHIFT_MASK)

/* Sets the decimation ratio of the filter */
#define I2S_PDM_SETUP_PDM_DECIMATION_MASK                            (0x1ff8)
#define I2S_PDM_SETUP_PDM_DECIMATION_SHIFT                           (3)
#define I2S_PDM_SETUP_PDM_DECIMATION(val)                            (((uint32_t)(((uint32_t)(val)) << I2S_PDM_SETUP_PDM_DECIMATION_SHIFT)) & I2S_PDM_SETUP_PDM_DECIMATION_MASK)

/* PDM mode */
#define I2S_PDM_SETUP_PDM_MODE_MASK                                  (0x6000)
#define I2S_PDM_SETUP_PDM_MODE_SHIFT                                 (13)
#define I2S_PDM_SETUP_PDM_MODE(val)                                  (((uint32_t)(((uint32_t)(val)) << I2S_PDM_SETUP_PDM_MODE_SHIFT)) & I2S_PDM_SETUP_PDM_MODE_MASK)

/* Enable PDM mode */
#define I2S_PDM_SETUP_PDM_EN_MASK                                    (0x80000000)
#define I2S_PDM_SETUP_PDM_EN_SHIFT                                   (31)
#define I2S_PDM_SETUP_PDM_EN(val)                                    (((uint32_t)(((uint32_t)(val)) << I2S_PDM_SETUP_PDM_EN_SHIFT)) & I2S_PDM_SETUP_PDM_EN_MASK)


/*! @name CLKCFG_SETUP */
typedef union
{
    struct
    {
        /* LSB of master clock divider */
        uint32_t master_clk_div:8;
        /* LSB of slave clock divider */
        uint32_t slave_clk_div:8;
        /* MSBs of both master and slave clock divider */
        uint32_t common_clk_div:8;
        /* Enables Slave clock */
        uint32_t slave_clk_en:1;
        /* Enables Master clock */
        uint32_t master_clk_en:1;
        /* When enabled slave output clock is taken from PDM module */
        uint32_t pdm_clk_en:1;
        uint32_t reserved_0:1;
        /* When set uses external clock for slave */
        uint32_t slave_ext:1;
        /* Selects slave clock source(either ext or generated):
        -1’b0:selects master
        -1’b1:selects slave */
        uint32_t slave_num:1;
        /* When set uses external clock for master */
        uint32_t master_ext:1;
        /* Selects master clock source(either ext or generated):
        -1’b0:selects master
        -1’b1:selects slave */
        uint32_t master_num:1;
    } field;
    uint32_t word;
} i2s_clkcfg_setup_t;

/*! @name SLV_SETUP */
typedef union
{
    struct
    {
        /* Sets how many words for each I2S phase */
        uint32_t slave_words:3;
        uint32_t reserved_0:5;
        /* Sets how many bits per word */
        uint32_t slave_bits:5;
        uint32_t reserved_1:3;
        /* Enables LSB shifting */
        uint32_t slave_lsb:1;
        /* Enables both channels */
        uint32_t slave_2ch:1;
        uint32_t reserved_2:13;
        /* Enables the Slave */
        uint32_t slave_en:1;
    } field;
    uint32_t word;
} i2s_slv_setup_t;

/*! @name MST_SETUP */
typedef union
{
    struct
    {
        /* Sets how many words for each I2S phase */
        uint32_t master_words:3;
        uint32_t reserved_0:5;
        /* Sets how many bits per word */
        uint32_t master_bits:5;
        uint32_t reserved_1:3;
        /* Enables LSB shifting */
        uint32_t master_lsb:1;
        /* Enables both channels */
        uint32_t master_2ch:1;
        uint32_t reserved_2:13;
        /* Enables the Master */
        uint32_t master_en:1;
    } field;
    uint32_t word;
} i2s_mst_setup_t;

/*! @name PDM_SETUP */
typedef union
{
    struct
    {
        /* Shifts the output of the filter */
        uint32_t pdm_shift:3;
        /* Sets the decimation ratio of the filter */
        uint32_t pdm_decimation:10;
        /* PDM mode */
        uint32_t pdm_mode:2;
        uint32_t reserved_0:16;
        /* Enable PDM mode */
        uint32_t pdm_en:1;
    } field;
    uint32_t word;
} i2s_pdm_setup_t;



#endif /* __UDMA_I2S_PERIPH_H__ */
