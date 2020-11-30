Input file: fe/ips/fll_new/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-----------------------+------+-----+------------------------------+
    |         Name          |Offset|Width|         Description          |
    +=======================+======+=====+==============================+
    |:ref:`FSR<fll_FSR>`    |     0|   32|FLL status register           |
    +-----------------------+------+-----+------------------------------+
    |:ref:`DRR<fll_DRR>`    |     4|   32|DCO Range register            |
    +-----------------------+------+-----+------------------------------+
    |:ref:`TTR<fll_TTR>`    |     8|   32|Temperature Tracking Register |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F0CR1<fll_F0CR1>`|    12|   32|FLL0 Configuration Register 1 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F0CR2<fll_F0CR2>`|    16|   32|FLL0 Configuration Register 2 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F1CR1<fll_F1CR1>`|    20|   32|FLL1 Configuration Register 1 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F1CR2<fll_F1CR2>`|    24|   32|FLL1 Configuration Register 2 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F2CR1<fll_F2CR1>`|    28|   32|FLL2 Configuration Register 1 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F2CR2<fll_F2CR2>`|    32|   32|FLL2 Configuration Register 2 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F3CR1<fll_F3CR1>`|    36|   32|FLL3 Configuration Register 1 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`F3CR2<fll_F3CR2>`|    40|   32|FLL3 Configuration Register 2 |
    +-----------------------+------+-----+------------------------------+
    |:ref:`CCR1<fll_CCR1>`  |    44|   32|Clock Configuration register 1|
    +-----------------------+------+-----+------------------------------+
    |:ref:`CCR2<fll_CCR2>`  |    48|   32|Clock Configuration register 2|
    +-----------------------+------+-----+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL status register
                #define FLL_FSR_OFFSET                           0x0
        
                // DCO Range register
                #define FLL_DRR_OFFSET                           0x4
        
                // Temperature Tracking Register
                #define FLL_TTR_OFFSET                           0x8
        
                // FLL0 Configuration Register 1
                #define FLL_F0CR1_OFFSET                         0xc
        
                // FLL0 Configuration Register 2
                #define FLL_F0CR2_OFFSET                         0x10
        
                // FLL1 Configuration Register 1
                #define FLL_F1CR1_OFFSET                         0x14
        
                // FLL1 Configuration Register 2
                #define FLL_F1CR2_OFFSET                         0x18
        
                // FLL2 Configuration Register 1
                #define FLL_F2CR1_OFFSET                         0x1c
        
                // FLL2 Configuration Register 2
                #define FLL_F2CR2_OFFSET                         0x20
        
                // FLL3 Configuration Register 1
                #define FLL_F3CR1_OFFSET                         0x24
        
                // FLL3 Configuration Register 2
                #define FLL_F3CR2_OFFSET                         0x28
        
                // Clock Configuration register 1
                #define FLL_CCR1_OFFSET                          0x2c
        
                // Clock Configuration register 2
                #define FLL_CCR2_OFFSET                          0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_fsr_get(uint32_t base);
        static inline void fll_fsr_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_drr_get(uint32_t base);
        static inline void fll_drr_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_ttr_get(uint32_t base);
        static inline void fll_ttr_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f0cr1_get(uint32_t base);
        static inline void fll_f0cr1_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f0cr2_get(uint32_t base);
        static inline void fll_f0cr2_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f1cr1_get(uint32_t base);
        static inline void fll_f1cr1_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f1cr2_get(uint32_t base);
        static inline void fll_f1cr2_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f2cr1_get(uint32_t base);
        static inline void fll_f2cr1_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f2cr2_get(uint32_t base);
        static inline void fll_f2cr2_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f3cr1_get(uint32_t base);
        static inline void fll_f3cr1_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_f3cr2_get(uint32_t base);
        static inline void fll_f3cr2_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_ccr1_get(uint32_t base);
        static inline void fll_ccr1_set(uint32_t base, uint32_t value);

        static inline uint32_t fll_ccr2_get(uint32_t base);
        static inline void fll_ccr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // LOCK0. Lock status for Feedback clock 0. (access: R)
        #define FLL_FSR_LOCK0_BIT                                            0
        #define FLL_FSR_LOCK0_WIDTH                                          1
        #define FLL_FSR_LOCK0_MASK                                           0x1
        #define FLL_FSR_LOCK0_RESET                                          0x0
        
        // Clamp error low for FLL0. Set when new DCO value for FLL0 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR0_BIT                                     1
        #define FLL_FSR_CLMP_LO_ERR0_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR0_MASK                                    0x2
        #define FLL_FSR_CLMP_LO_ERR0_RESET                                   0x0
        
        // Clamp error high for FLL0. Set when new DCO value for FLL0 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR0_BIT                                     2
        #define FLL_FSR_CLMP_HI_ERR0_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR0_MASK                                    0x4
        #define FLL_FSR_CLMP_HI_ERR0_RESET                                   0x0
        
        // FDC saturation error for FLL0. Set when FDC counter of FLL0 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR0_BIT                                     3
        #define FLL_FSR_FDC_SAT_ERR0_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR0_MASK                                    0x8
        #define FLL_FSR_FDC_SAT_ERR0_RESET                                   0x0
        
        // LOCK1. Lock status for Feedback clock 1. (access: R)
        #define FLL_FSR_LOCK1_BIT                                            8
        #define FLL_FSR_LOCK1_WIDTH                                          1
        #define FLL_FSR_LOCK1_MASK                                           0x100
        #define FLL_FSR_LOCK1_RESET                                          0x0
        
        // Clamp error low for FLL1. Set when new DCO value for FLL1 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR1_BIT                                     9
        #define FLL_FSR_CLMP_LO_ERR1_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR1_MASK                                    0x200
        #define FLL_FSR_CLMP_LO_ERR1_RESET                                   0x0
        
        // Clamp error high for FLL1. Set when new DCO value for FLL1 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR1_BIT                                     10
        #define FLL_FSR_CLMP_HI_ERR1_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR1_MASK                                    0x400
        #define FLL_FSR_CLMP_HI_ERR1_RESET                                   0x0
        
        // FDC saturation error for FLL1. Set when FDC counter of FLL1 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR1_BIT                                     11
        #define FLL_FSR_FDC_SAT_ERR1_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR1_MASK                                    0x800
        #define FLL_FSR_FDC_SAT_ERR1_RESET                                   0x0
        
        // LOCK2. Lock status for Feedback clock 3. (access: R)
        #define FLL_FSR_LOCK2_BIT                                            16
        #define FLL_FSR_LOCK2_WIDTH                                          1
        #define FLL_FSR_LOCK2_MASK                                           0x10000
        #define FLL_FSR_LOCK2_RESET                                          0x0
        
        // Clamp error low for FLL2. Set when new DCO value for FLL2 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR2_BIT                                     17
        #define FLL_FSR_CLMP_LO_ERR2_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR2_MASK                                    0x20000
        #define FLL_FSR_CLMP_LO_ERR2_RESET                                   0x0
        
        // Clamp error high for FLL2. Set when new DCO value for FLL2 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR2_BIT                                     18
        #define FLL_FSR_CLMP_HI_ERR2_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR2_MASK                                    0x40000
        #define FLL_FSR_CLMP_HI_ERR2_RESET                                   0x0
        
        // FDC saturation error for FLL2. Set when FDC counter of FLL2 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR2_BIT                                     19
        #define FLL_FSR_FDC_SAT_ERR2_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR2_MASK                                    0x80000
        #define FLL_FSR_FDC_SAT_ERR2_RESET                                   0x0
        
        // LOCK3. Lock status for Feedback clock 3. (access: R)
        #define FLL_FSR_LOCK3_BIT                                            24
        #define FLL_FSR_LOCK3_WIDTH                                          1
        #define FLL_FSR_LOCK3_MASK                                           0x1000000
        #define FLL_FSR_LOCK3_RESET                                          0x0
        
        // Clamp error low for FLL3. Set when new DCO value for FLL3 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR3_BIT                                     25
        #define FLL_FSR_CLMP_LO_ERR3_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR3_MASK                                    0x2000000
        #define FLL_FSR_CLMP_LO_ERR3_RESET                                   0x0
        
        // Clamp error high for FLL3. Set when new DCO value for FLL3 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR3_BIT                                     26
        #define FLL_FSR_CLMP_HI_ERR3_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR3_MASK                                    0x4000000
        #define FLL_FSR_CLMP_HI_ERR3_RESET                                   0x0
        
        // FDC saturation error for FLL3. Set when FDC counter of FLL3 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR3_BIT                                     27
        #define FLL_FSR_FDC_SAT_ERR3_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR3_MASK                                    0x8000000
        #define FLL_FSR_FDC_SAT_ERR3_RESET                                   0x0
        
        // Minimum value allowed for DCO code. (access: R/W)
        #define FLL_DRR_DCO_MIN_BIT                                          0
        #define FLL_DRR_DCO_MIN_WIDTH                                        9
        #define FLL_DRR_DCO_MIN_MASK                                         0x1ff
        #define FLL_DRR_DCO_MIN_RESET                                        0x0
        
        // Maximum value allowed for DCO code. (access: R/W)
        #define FLL_DRR_DCO_MAX_BIT                                          16
        #define FLL_DRR_DCO_MAX_WIDTH                                        9
        #define FLL_DRR_DCO_MAX_MASK                                         0x1ff0000
        #define FLL_DRR_DCO_MAX_RESET                                        0x96
        
        // Number of ref clock cycles between two integration periods. (access: R/W)
        #define FLL_TTR_REFRESH_BIT                                          0
        #define FLL_TTR_REFRESH_WIDTH                                        24
        #define FLL_TTR_REFRESH_MASK                                         0xffffff
        #define FLL_TTR_REFRESH_RESET                                        0x0
        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F0CR1_DCO_EN_BIT                                         0
        #define FLL_F0CR1_DCO_EN_WIDTH                                       1
        #define FLL_F0CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F0CR1_OP_MODE_BIT                                        1
        #define FLL_F0CR1_OP_MODE_WIDTH                                      1
        #define FLL_F0CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F0CR1_TTM_EN_BIT                                         2
        #define FLL_F0CR1_TTM_EN_WIDTH                                       1
        #define FLL_F0CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F0CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F0CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F0CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F0CR1_LOCK_TOL_BIT                                       8
        #define FLL_F0CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F0CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F0CR1_ITG_PER_BIT                                        16
        #define FLL_F0CR1_ITG_PER_WIDTH                                      10
        #define FLL_F0CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F0CR1_STBL_BIT                                           26
        #define FLL_F0CR1_STBL_WIDTH                                         6
        #define FLL_F0CR1_STBL_MASK                                          0xfc000000
        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F0CR2_MFI_BIT                                            0
        #define FLL_F0CR2_MFI_WIDTH                                          16
        #define FLL_F0CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F0CR2_DCO_CODE_BIT                                       16
        #define FLL_F0CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F0CR2_DCO_CODE_MASK                                      0x1ff0000
        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F1CR1_DCO_EN_BIT                                         0
        #define FLL_F1CR1_DCO_EN_WIDTH                                       1
        #define FLL_F1CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F1CR1_OP_MODE_BIT                                        1
        #define FLL_F1CR1_OP_MODE_WIDTH                                      1
        #define FLL_F1CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F1CR1_TTM_EN_BIT                                         2
        #define FLL_F1CR1_TTM_EN_WIDTH                                       1
        #define FLL_F1CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F1CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F1CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F1CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F1CR1_LOCK_TOL_BIT                                       8
        #define FLL_F1CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F1CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F1CR1_ITG_PER_BIT                                        16
        #define FLL_F1CR1_ITG_PER_WIDTH                                      10
        #define FLL_F1CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F1CR1_STBL_BIT                                           26
        #define FLL_F1CR1_STBL_WIDTH                                         6
        #define FLL_F1CR1_STBL_MASK                                          0xfc000000
        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F1CR2_MFI_BIT                                            0
        #define FLL_F1CR2_MFI_WIDTH                                          16
        #define FLL_F1CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F1CR2_DCO_CODE_BIT                                       16
        #define FLL_F1CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F1CR2_DCO_CODE_MASK                                      0x1ff0000
        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F2CR1_DCO_EN_BIT                                         0
        #define FLL_F2CR1_DCO_EN_WIDTH                                       1
        #define FLL_F2CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F2CR1_OP_MODE_BIT                                        1
        #define FLL_F2CR1_OP_MODE_WIDTH                                      1
        #define FLL_F2CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F2CR1_TTM_EN_BIT                                         2
        #define FLL_F2CR1_TTM_EN_WIDTH                                       1
        #define FLL_F2CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F2CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F2CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F2CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F2CR1_LOCK_TOL_BIT                                       8
        #define FLL_F2CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F2CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F2CR1_ITG_PER_BIT                                        16
        #define FLL_F2CR1_ITG_PER_WIDTH                                      10
        #define FLL_F2CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F2CR1_STBL_BIT                                           26
        #define FLL_F2CR1_STBL_WIDTH                                         6
        #define FLL_F2CR1_STBL_MASK                                          0xfc000000
        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F2CR2_MFI_BIT                                            0
        #define FLL_F2CR2_MFI_WIDTH                                          16
        #define FLL_F2CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F2CR2_DCO_CODE_BIT                                       16
        #define FLL_F2CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F2CR2_DCO_CODE_MASK                                      0x1ff0000
        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F3CR1_DCO_EN_BIT                                         0
        #define FLL_F3CR1_DCO_EN_WIDTH                                       1
        #define FLL_F3CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F3CR1_OP_MODE_BIT                                        1
        #define FLL_F3CR1_OP_MODE_WIDTH                                      1
        #define FLL_F3CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F3CR1_TTM_EN_BIT                                         2
        #define FLL_F3CR1_TTM_EN_WIDTH                                       1
        #define FLL_F3CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F3CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F3CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F3CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F3CR1_LOCK_TOL_BIT                                       8
        #define FLL_F3CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F3CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F3CR1_ITG_PER_BIT                                        16
        #define FLL_F3CR1_ITG_PER_WIDTH                                      10
        #define FLL_F3CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F3CR1_STBL_BIT                                           26
        #define FLL_F3CR1_STBL_WIDTH                                         6
        #define FLL_F3CR1_STBL_MASK                                          0xfc000000
        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F3CR2_MFI_BIT                                            0
        #define FLL_F3CR2_MFI_WIDTH                                          16
        #define FLL_F3CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F3CR2_DCO_CODE_BIT                                       16
        #define FLL_F3CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F3CR2_DCO_CODE_MASK                                      0x1ff0000
        
        // Clock divider setting for OUTCLK[0]. OUTCLK[0] = CLK0 / (CLK0_DIV). (access: R/W)
        #define FLL_CCR1_CLK0_DIV_BIT                                        0
        #define FLL_CCR1_CLK0_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK0_DIV_MASK                                       0xff
        
        // Clock divider setting for OUTCLK[1]. OUTCLK[1] = CLK1 / (CLK1_DIV). (access: R/W)
        #define FLL_CCR1_CLK1_DIV_BIT                                        8
        #define FLL_CCR1_CLK1_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK1_DIV_MASK                                       0xff00
        
        // Clock divider setting for OUTCLK[2]. OUTCLK[2] = CLK2 / (CLK2_DIV). (access: R/W)
        #define FLL_CCR1_CLK2_DIV_BIT                                        16
        #define FLL_CCR1_CLK2_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK2_DIV_MASK                                       0xff0000
        
        // Clock divider setting for OUTCLK[3]. OUTCLK[3] = CLK3 / (CLK3_DIV). (access: R/W)
        #define FLL_CCR1_CLK3_DIV_BIT                                        24
        #define FLL_CCR1_CLK3_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK3_DIV_MASK                                       0xff000000
        
        // Clock source selection for OUTCLK[0]: 0: Ref clock (default). 1: FBKCLK[0] clock. (access: R/W)
        #define FLL_CCR2_CLK0_SEL_BIT                                        0
        #define FLL_CCR2_CLK0_SEL_WIDTH                                      1
        #define FLL_CCR2_CLK0_SEL_MASK                                       0x1
        
        // Clock source selection for OUTCLK[1]: 00: Ref clock. 01: FBKCLK[0] clock (default). 1x: FBKCLK[1] clock. (access: R/W)
        #define FLL_CCR2_CLK1_SEL_BIT                                        4
        #define FLL_CCR2_CLK1_SEL_WIDTH                                      2
        #define FLL_CCR2_CLK1_SEL_MASK                                       0x30
        
        // Clock source selection for OUTCLK[2]: 00: Ref clock. 01: FBKCLK[0] clock (default). 10: FBKCLK[1] clock. 11: FBKCLK[2] clock. (access: R/W)
        #define FLL_CCR2_CLK2_SEL_BIT                                        8
        #define FLL_CCR2_CLK2_SEL_WIDTH                                      2
        #define FLL_CCR2_CLK2_SEL_MASK                                       0x300
        
        // Clock source selection for OUTCLK[3]: 000: Ref clock. 001: FBKCLK[0] clock (default). 010: FBKCLK[1] clock. 011: FBKCLK[2] clock. 1xx: FBKCLK[3] clock. (access: R/W)
        #define FLL_CCR2_CLK3_SEL_BIT                                        12
        #define FLL_CCR2_CLK3_SEL_WIDTH                                      3
        #define FLL_CCR2_CLK3_SEL_MASK                                       0x7000
        
        // FBKCLK[0] clock gated. 0: FBKCLK[0] is not gated. 1: FBKCLK[0] is clock gated by LOCK0 signal. (access: R/W)
        #define FLL_CCR2_CKG0_BIT                                            16
        #define FLL_CCR2_CKG0_WIDTH                                          1
        #define FLL_CCR2_CKG0_MASK                                           0x10000
        
        // FBKCLK[1] clock gated. 0: FBKCLK[1] is not gated. 1: FBKCLK[1] is clock gated by LOCK1 signal. (access: R/W)
        #define FLL_CCR2_CKG1_BIT                                            17
        #define FLL_CCR2_CKG1_WIDTH                                          1
        #define FLL_CCR2_CKG1_MASK                                           0x20000
        
        // FBKCLK[2] clock gated. 0: FBKCLK[2] is not gated. 1: FBKCLK[2] is clock gated by LOCK2 signal. (access: R/W)
        #define FLL_CCR2_CKG2_BIT                                            18
        #define FLL_CCR2_CKG2_WIDTH                                          1
        #define FLL_CCR2_CKG2_MASK                                           0x40000
        
        // FBKCLK[3] clock gated. 0: FBKCLK[3] is not gated. 1: FBKCLK[3] is clock gated by LOCK3 signal. (access: R/W)
        #define FLL_CCR2_CKG3_BIT                                            19
        #define FLL_CCR2_CKG3_WIDTH                                          1
        #define FLL_CCR2_CKG3_MASK                                           0x80000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_FSR_LOCK0_GET(value)                           (GAP_BEXTRACTU((value),1,0))
        #define FLL_FSR_LOCK0_GETS(value)                          (GAP_BEXTRACT((value),1,0))
        #define FLL_FSR_LOCK0_SET(value,field)                     (GAP_BINSERT((value),(field),1,0))
        #define FLL_FSR_LOCK0(val)                                 ((val) << 0)
        
        #define FLL_FSR_CLMP_LO_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,1))
        #define FLL_FSR_CLMP_LO_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,1))
        #define FLL_FSR_CLMP_LO_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,1))
        #define FLL_FSR_CLMP_LO_ERR0(val)                          ((val) << 1)
        
        #define FLL_FSR_CLMP_HI_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,2))
        #define FLL_FSR_CLMP_HI_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,2))
        #define FLL_FSR_CLMP_HI_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,2))
        #define FLL_FSR_CLMP_HI_ERR0(val)                          ((val) << 2)
        
        #define FLL_FSR_FDC_SAT_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,3))
        #define FLL_FSR_FDC_SAT_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,3))
        #define FLL_FSR_FDC_SAT_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,3))
        #define FLL_FSR_FDC_SAT_ERR0(val)                          ((val) << 3)
        
        #define FLL_FSR_LOCK1_GET(value)                           (GAP_BEXTRACTU((value),1,8))
        #define FLL_FSR_LOCK1_GETS(value)                          (GAP_BEXTRACT((value),1,8))
        #define FLL_FSR_LOCK1_SET(value,field)                     (GAP_BINSERT((value),(field),1,8))
        #define FLL_FSR_LOCK1(val)                                 ((val) << 8)
        
        #define FLL_FSR_CLMP_LO_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,9))
        #define FLL_FSR_CLMP_LO_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,9))
        #define FLL_FSR_CLMP_LO_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,9))
        #define FLL_FSR_CLMP_LO_ERR1(val)                          ((val) << 9)
        
        #define FLL_FSR_CLMP_HI_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,10))
        #define FLL_FSR_CLMP_HI_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,10))
        #define FLL_FSR_CLMP_HI_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,10))
        #define FLL_FSR_CLMP_HI_ERR1(val)                          ((val) << 10)
        
        #define FLL_FSR_FDC_SAT_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,11))
        #define FLL_FSR_FDC_SAT_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,11))
        #define FLL_FSR_FDC_SAT_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,11))
        #define FLL_FSR_FDC_SAT_ERR1(val)                          ((val) << 11)
        
        #define FLL_FSR_LOCK2_GET(value)                           (GAP_BEXTRACTU((value),1,16))
        #define FLL_FSR_LOCK2_GETS(value)                          (GAP_BEXTRACT((value),1,16))
        #define FLL_FSR_LOCK2_SET(value,field)                     (GAP_BINSERT((value),(field),1,16))
        #define FLL_FSR_LOCK2(val)                                 ((val) << 16)
        
        #define FLL_FSR_CLMP_LO_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,17))
        #define FLL_FSR_CLMP_LO_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,17))
        #define FLL_FSR_CLMP_LO_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,17))
        #define FLL_FSR_CLMP_LO_ERR2(val)                          ((val) << 17)
        
        #define FLL_FSR_CLMP_HI_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,18))
        #define FLL_FSR_CLMP_HI_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,18))
        #define FLL_FSR_CLMP_HI_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,18))
        #define FLL_FSR_CLMP_HI_ERR2(val)                          ((val) << 18)
        
        #define FLL_FSR_FDC_SAT_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,19))
        #define FLL_FSR_FDC_SAT_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,19))
        #define FLL_FSR_FDC_SAT_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,19))
        #define FLL_FSR_FDC_SAT_ERR2(val)                          ((val) << 19)
        
        #define FLL_FSR_LOCK3_GET(value)                           (GAP_BEXTRACTU((value),1,24))
        #define FLL_FSR_LOCK3_GETS(value)                          (GAP_BEXTRACT((value),1,24))
        #define FLL_FSR_LOCK3_SET(value,field)                     (GAP_BINSERT((value),(field),1,24))
        #define FLL_FSR_LOCK3(val)                                 ((val) << 24)
        
        #define FLL_FSR_CLMP_LO_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,25))
        #define FLL_FSR_CLMP_LO_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,25))
        #define FLL_FSR_CLMP_LO_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,25))
        #define FLL_FSR_CLMP_LO_ERR3(val)                          ((val) << 25)
        
        #define FLL_FSR_CLMP_HI_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,26))
        #define FLL_FSR_CLMP_HI_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,26))
        #define FLL_FSR_CLMP_HI_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,26))
        #define FLL_FSR_CLMP_HI_ERR3(val)                          ((val) << 26)
        
        #define FLL_FSR_FDC_SAT_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,27))
        #define FLL_FSR_FDC_SAT_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,27))
        #define FLL_FSR_FDC_SAT_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,27))
        #define FLL_FSR_FDC_SAT_ERR3(val)                          ((val) << 27)
        
        #define FLL_DRR_DCO_MIN_GET(value)                         (GAP_BEXTRACTU((value),9,0))
        #define FLL_DRR_DCO_MIN_GETS(value)                        (GAP_BEXTRACT((value),9,0))
        #define FLL_DRR_DCO_MIN_SET(value,field)                   (GAP_BINSERT((value),(field),9,0))
        #define FLL_DRR_DCO_MIN(val)                               ((val) << 0)
        
        #define FLL_DRR_DCO_MAX_GET(value)                         (GAP_BEXTRACTU((value),9,16))
        #define FLL_DRR_DCO_MAX_GETS(value)                        (GAP_BEXTRACT((value),9,16))
        #define FLL_DRR_DCO_MAX_SET(value,field)                   (GAP_BINSERT((value),(field),9,16))
        #define FLL_DRR_DCO_MAX(val)                               ((val) << 16)
        
        #define FLL_TTR_REFRESH_GET(value)                         (GAP_BEXTRACTU((value),24,0))
        #define FLL_TTR_REFRESH_GETS(value)                        (GAP_BEXTRACT((value),24,0))
        #define FLL_TTR_REFRESH_SET(value,field)                   (GAP_BINSERT((value),(field),24,0))
        #define FLL_TTR_REFRESH(val)                               ((val) << 0)
        
        #define FLL_F0CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F0CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F0CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F0CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F0CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F0CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F0CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F0CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F0CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F0CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F0CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F0CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F0CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F0CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F0CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F0CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F0CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F0CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F0CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F0CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F0CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F0CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F0CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F0CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F0CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F0CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F0CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F0CR1_STBL(val)                                ((val) << 26)
        
        #define FLL_F0CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F0CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F0CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F0CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F0CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F0CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F0CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F0CR2_DCO_CODE(val)                            ((val) << 16)
        
        #define FLL_F1CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F1CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F1CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F1CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F1CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F1CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F1CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F1CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F1CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F1CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F1CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F1CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F1CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F1CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F1CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F1CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F1CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F1CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F1CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F1CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F1CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F1CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F1CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F1CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F1CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F1CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F1CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F1CR1_STBL(val)                                ((val) << 26)
        
        #define FLL_F1CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F1CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F1CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F1CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F1CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F1CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F1CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F1CR2_DCO_CODE(val)                            ((val) << 16)
        
        #define FLL_F2CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F2CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F2CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F2CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F2CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F2CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F2CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F2CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F2CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F2CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F2CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F2CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F2CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F2CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F2CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F2CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F2CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F2CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F2CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F2CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F2CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F2CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F2CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F2CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F2CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F2CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F2CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F2CR1_STBL(val)                                ((val) << 26)
        
        #define FLL_F2CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F2CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F2CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F2CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F2CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F2CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F2CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F2CR2_DCO_CODE(val)                            ((val) << 16)
        
        #define FLL_F3CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F3CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F3CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F3CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F3CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F3CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F3CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F3CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F3CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F3CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F3CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F3CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F3CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F3CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F3CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F3CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F3CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F3CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F3CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F3CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F3CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F3CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F3CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F3CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F3CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F3CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F3CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F3CR1_STBL(val)                                ((val) << 26)
        
        #define FLL_F3CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F3CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F3CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F3CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F3CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F3CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F3CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F3CR2_DCO_CODE(val)                            ((val) << 16)
        
        #define FLL_CCR1_CLK0_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,0))
        #define FLL_CCR1_CLK0_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,0))
        #define FLL_CCR1_CLK0_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,0))
        #define FLL_CCR1_CLK0_DIV(val)                             ((val) << 0)
        
        #define FLL_CCR1_CLK1_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,8))
        #define FLL_CCR1_CLK1_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,8))
        #define FLL_CCR1_CLK1_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,8))
        #define FLL_CCR1_CLK1_DIV(val)                             ((val) << 8)
        
        #define FLL_CCR1_CLK2_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,16))
        #define FLL_CCR1_CLK2_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,16))
        #define FLL_CCR1_CLK2_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,16))
        #define FLL_CCR1_CLK2_DIV(val)                             ((val) << 16)
        
        #define FLL_CCR1_CLK3_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,24))
        #define FLL_CCR1_CLK3_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,24))
        #define FLL_CCR1_CLK3_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,24))
        #define FLL_CCR1_CLK3_DIV(val)                             ((val) << 24)
        
        #define FLL_CCR2_CLK0_SEL_GET(value)                       (GAP_BEXTRACTU((value),1,0))
        #define FLL_CCR2_CLK0_SEL_GETS(value)                      (GAP_BEXTRACT((value),1,0))
        #define FLL_CCR2_CLK0_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),1,0))
        #define FLL_CCR2_CLK0_SEL(val)                             ((val) << 0)
        
        #define FLL_CCR2_CLK1_SEL_GET(value)                       (GAP_BEXTRACTU((value),2,4))
        #define FLL_CCR2_CLK1_SEL_GETS(value)                      (GAP_BEXTRACT((value),2,4))
        #define FLL_CCR2_CLK1_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),2,4))
        #define FLL_CCR2_CLK1_SEL(val)                             ((val) << 4)
        
        #define FLL_CCR2_CLK2_SEL_GET(value)                       (GAP_BEXTRACTU((value),2,8))
        #define FLL_CCR2_CLK2_SEL_GETS(value)                      (GAP_BEXTRACT((value),2,8))
        #define FLL_CCR2_CLK2_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),2,8))
        #define FLL_CCR2_CLK2_SEL(val)                             ((val) << 8)
        
        #define FLL_CCR2_CLK3_SEL_GET(value)                       (GAP_BEXTRACTU((value),3,12))
        #define FLL_CCR2_CLK3_SEL_GETS(value)                      (GAP_BEXTRACT((value),3,12))
        #define FLL_CCR2_CLK3_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),3,12))
        #define FLL_CCR2_CLK3_SEL(val)                             ((val) << 12)
        
        #define FLL_CCR2_CKG0_GET(value)                           (GAP_BEXTRACTU((value),1,16))
        #define FLL_CCR2_CKG0_GETS(value)                          (GAP_BEXTRACT((value),1,16))
        #define FLL_CCR2_CKG0_SET(value,field)                     (GAP_BINSERT((value),(field),1,16))
        #define FLL_CCR2_CKG0(val)                                 ((val) << 16)
        
        #define FLL_CCR2_CKG1_GET(value)                           (GAP_BEXTRACTU((value),1,17))
        #define FLL_CCR2_CKG1_GETS(value)                          (GAP_BEXTRACT((value),1,17))
        #define FLL_CCR2_CKG1_SET(value,field)                     (GAP_BINSERT((value),(field),1,17))
        #define FLL_CCR2_CKG1(val)                                 ((val) << 17)
        
        #define FLL_CCR2_CKG2_GET(value)                           (GAP_BEXTRACTU((value),1,18))
        #define FLL_CCR2_CKG2_GETS(value)                          (GAP_BEXTRACT((value),1,18))
        #define FLL_CCR2_CKG2_SET(value,field)                     (GAP_BINSERT((value),(field),1,18))
        #define FLL_CCR2_CKG2(val)                                 ((val) << 18)
        
        #define FLL_CCR2_CKG3_GET(value)                           (GAP_BEXTRACTU((value),1,19))
        #define FLL_CCR2_CKG3_GETS(value)                          (GAP_BEXTRACT((value),1,19))
        #define FLL_CCR2_CKG3_SET(value,field)                     (GAP_BINSERT((value),(field),1,19))
        #define FLL_CCR2_CKG3(val)                                 ((val) << 19)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** FLL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t fsr;  // FLL status register
            volatile uint32_t drr;  // DCO Range register
            volatile uint32_t ttr;  // Temperature Tracking Register
            volatile uint32_t f0cr1;  // FLL0 Configuration Register 1
            volatile uint32_t f0cr2;  // FLL0 Configuration Register 2
            volatile uint32_t f1cr1;  // FLL1 Configuration Register 1
            volatile uint32_t f1cr2;  // FLL1 Configuration Register 2
            volatile uint32_t f2cr1;  // FLL2 Configuration Register 1
            volatile uint32_t f2cr2;  // FLL2 Configuration Register 2
            volatile uint32_t f3cr1;  // FLL3 Configuration Register 1
            volatile uint32_t f3cr2;  // FLL3 Configuration Register 2
            volatile uint32_t ccr1;  // Clock Configuration register 1
            volatile uint32_t ccr2;  // Clock Configuration register 2
        } __attribute__((packed)) fll_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int lock0           :1 ; // LOCK0. Lock status for Feedback clock 0.
            unsigned int clmp_lo_err0    :1 ; // Clamp error low for FLL0. Set when new DCO value for FLL0 &lt; DCO_MIN value.
            unsigned int clmp_hi_err0    :1 ; // Clamp error high for FLL0. Set when new DCO value for FLL0 &gt; DCO_MAX value.
            unsigned int fdc_sat_err0    :1 ; // FDC saturation error for FLL0. Set when FDC counter of FLL0 overflows.
            unsigned int padding0:4 ;
            unsigned int lock1           :1 ; // LOCK1. Lock status for Feedback clock 1.
            unsigned int clmp_lo_err1    :1 ; // Clamp error low for FLL1. Set when new DCO value for FLL1 &lt; DCO_MIN value.
            unsigned int clmp_hi_err1    :1 ; // Clamp error high for FLL1. Set when new DCO value for FLL1 &gt; DCO_MAX value.
            unsigned int fdc_sat_err1    :1 ; // FDC saturation error for FLL1. Set when FDC counter of FLL1 overflows.
            unsigned int padding1:4 ;
            unsigned int lock2           :1 ; // LOCK2. Lock status for Feedback clock 3.
            unsigned int clmp_lo_err2    :1 ; // Clamp error low for FLL2. Set when new DCO value for FLL2 &lt; DCO_MIN value.
            unsigned int clmp_hi_err2    :1 ; // Clamp error high for FLL2. Set when new DCO value for FLL2 &gt; DCO_MAX value.
            unsigned int fdc_sat_err2    :1 ; // FDC saturation error for FLL2. Set when FDC counter of FLL2 overflows.
            unsigned int padding2:4 ;
            unsigned int lock3           :1 ; // LOCK3. Lock status for Feedback clock 3.
            unsigned int clmp_lo_err3    :1 ; // Clamp error low for FLL3. Set when new DCO value for FLL3 &lt; DCO_MIN value.
            unsigned int clmp_hi_err3    :1 ; // Clamp error high for FLL3. Set when new DCO value for FLL3 &gt; DCO_MAX value.
            unsigned int fdc_sat_err3    :1 ; // FDC saturation error for FLL3. Set when FDC counter of FLL3 overflows.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_fsr_t;
        
        typedef union {
          struct {
            unsigned int dco_min         :9 ; // Minimum value allowed for DCO code.
            unsigned int padding0:7 ;
            unsigned int dco_max         :9 ; // Maximum value allowed for DCO code.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_drr_t;
        
        typedef union {
          struct {
            unsigned int refresh         :24; // Number of ref clock cycles between two integration periods.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ttr_t;
        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f0cr1_t;
        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f0cr2_t;
        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f1cr1_t;
        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f1cr2_t;
        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f2cr1_t;
        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f2cr2_t;
        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f3cr1_t;
        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f3cr2_t;
        
        typedef union {
          struct {
            unsigned int clk0_div        :8 ; // Clock divider setting for OUTCLK[0]. OUTCLK[0] = CLK0 / (CLK0_DIV).
            unsigned int clk1_div        :8 ; // Clock divider setting for OUTCLK[1]. OUTCLK[1] = CLK1 / (CLK1_DIV).
            unsigned int clk2_div        :8 ; // Clock divider setting for OUTCLK[2]. OUTCLK[2] = CLK2 / (CLK2_DIV).
            unsigned int clk3_div        :8 ; // Clock divider setting for OUTCLK[3]. OUTCLK[3] = CLK3 / (CLK3_DIV).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ccr1_t;
        
        typedef union {
          struct {
            unsigned int clk0_sel        :1 ; // Clock source selection for OUTCLK[0]: 0: Ref clock (default). 1: FBKCLK[0] clock.
            unsigned int padding0:3 ;
            unsigned int clk1_sel        :2 ; // Clock source selection for OUTCLK[1]: 00: Ref clock. 01: FBKCLK[0] clock (default). 1x: FBKCLK[1] clock.
            unsigned int padding1:2 ;
            unsigned int clk2_sel        :2 ; // Clock source selection for OUTCLK[2]: 00: Ref clock. 01: FBKCLK[0] clock (default). 10: FBKCLK[1] clock. 11: FBKCLK[2] clock.
            unsigned int padding2:2 ;
            unsigned int clk3_sel        :3 ; // Clock source selection for OUTCLK[3]: 000: Ref clock. 001: FBKCLK[0] clock (default). 010: FBKCLK[1] clock. 011: FBKCLK[2] clock. 1xx: FBKCLK[3] clock.
            unsigned int padding3:1 ;
            unsigned int ckg0            :1 ; // FBKCLK[0] clock gated. 0: FBKCLK[0] is not gated. 1: FBKCLK[0] is clock gated by LOCK0 signal.
            unsigned int ckg1            :1 ; // FBKCLK[1] clock gated. 0: FBKCLK[1] is not gated. 1: FBKCLK[1] is clock gated by LOCK1 signal.
            unsigned int ckg2            :1 ; // FBKCLK[2] clock gated. 0: FBKCLK[2] is not gated. 1: FBKCLK[2] is clock gated by LOCK2 signal.
            unsigned int ckg3            :1 ; // FBKCLK[3] clock gated. 0: FBKCLK[3] is not gated. 1: FBKCLK[3] is clock gated by LOCK3 signal.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ccr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_fll : public vp::regmap
        {
        public:
            vp_fll_fsr fsr;
            vp_fll_drr drr;
            vp_fll_ttr ttr;
            vp_fll_f0cr1 f0cr1;
            vp_fll_f0cr2 f0cr2;
            vp_fll_f1cr1 f1cr1;
            vp_fll_f1cr2 f1cr2;
            vp_fll_f2cr1 f2cr1;
            vp_fll_f2cr2 f2cr2;
            vp_fll_f3cr1 f3cr1;
            vp_fll_f3cr2 f3cr2;
            vp_fll_ccr1 ccr1;
            vp_fll_ccr2 ccr2;
        };

|

.. _fll_FSR:

FSR
"""

FLL status register

.. table:: 

    +-----+---+------------+------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                 Description                                  |
    +=====+===+============+==============================================================================+
    |    0|R  |LOCK0       |LOCK0. Lock status for Feedback clock 0.                                      |
    +-----+---+------------+------------------------------------------------------------------------------+
    |    1|R  |CLMP_LO_ERR0|Clamp error low for FLL0. Set when new DCO value for FLL0 &lt; DCO_MIN value. |
    +-----+---+------------+------------------------------------------------------------------------------+
    |    2|R  |CLMP_HI_ERR0|Clamp error high for FLL0. Set when new DCO value for FLL0 &gt; DCO_MAX value.|
    +-----+---+------------+------------------------------------------------------------------------------+
    |    3|R  |FDC_SAT_ERR0|FDC saturation error for FLL0. Set when FDC counter of FLL0 overflows.        |
    +-----+---+------------+------------------------------------------------------------------------------+
    |    8|R  |LOCK1       |LOCK1. Lock status for Feedback clock 1.                                      |
    +-----+---+------------+------------------------------------------------------------------------------+
    |    9|R  |CLMP_LO_ERR1|Clamp error low for FLL1. Set when new DCO value for FLL1 &lt; DCO_MIN value. |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   10|R  |CLMP_HI_ERR1|Clamp error high for FLL1. Set when new DCO value for FLL1 &gt; DCO_MAX value.|
    +-----+---+------------+------------------------------------------------------------------------------+
    |   11|R  |FDC_SAT_ERR1|FDC saturation error for FLL1. Set when FDC counter of FLL1 overflows.        |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   16|R  |LOCK2       |LOCK2. Lock status for Feedback clock 3.                                      |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   17|R  |CLMP_LO_ERR2|Clamp error low for FLL2. Set when new DCO value for FLL2 &lt; DCO_MIN value. |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   18|R  |CLMP_HI_ERR2|Clamp error high for FLL2. Set when new DCO value for FLL2 &gt; DCO_MAX value.|
    +-----+---+------------+------------------------------------------------------------------------------+
    |   19|R  |FDC_SAT_ERR2|FDC saturation error for FLL2. Set when FDC counter of FLL2 overflows.        |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   24|R  |LOCK3       |LOCK3. Lock status for Feedback clock 3.                                      |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   25|R  |CLMP_LO_ERR3|Clamp error low for FLL3. Set when new DCO value for FLL3 &lt; DCO_MIN value. |
    +-----+---+------------+------------------------------------------------------------------------------+
    |   26|R  |CLMP_HI_ERR3|Clamp error high for FLL3. Set when new DCO value for FLL3 &gt; DCO_MAX value.|
    +-----+---+------------+------------------------------------------------------------------------------+
    |   27|R  |FDC_SAT_ERR3|FDC saturation error for FLL3. Set when FDC counter of FLL3 overflows.        |
    +-----+---+------------+------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL status register
                #define FLL_FSR_OFFSET                           0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_fsr_get(uint32_t base);
        static inline void fll_fsr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // LOCK0. Lock status for Feedback clock 0. (access: R)
        #define FLL_FSR_LOCK0_BIT                                            0
        #define FLL_FSR_LOCK0_WIDTH                                          1
        #define FLL_FSR_LOCK0_MASK                                           0x1
        #define FLL_FSR_LOCK0_RESET                                          0x0
        
        // Clamp error low for FLL0. Set when new DCO value for FLL0 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR0_BIT                                     1
        #define FLL_FSR_CLMP_LO_ERR0_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR0_MASK                                    0x2
        #define FLL_FSR_CLMP_LO_ERR0_RESET                                   0x0
        
        // Clamp error high for FLL0. Set when new DCO value for FLL0 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR0_BIT                                     2
        #define FLL_FSR_CLMP_HI_ERR0_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR0_MASK                                    0x4
        #define FLL_FSR_CLMP_HI_ERR0_RESET                                   0x0
        
        // FDC saturation error for FLL0. Set when FDC counter of FLL0 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR0_BIT                                     3
        #define FLL_FSR_FDC_SAT_ERR0_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR0_MASK                                    0x8
        #define FLL_FSR_FDC_SAT_ERR0_RESET                                   0x0
        
        // LOCK1. Lock status for Feedback clock 1. (access: R)
        #define FLL_FSR_LOCK1_BIT                                            8
        #define FLL_FSR_LOCK1_WIDTH                                          1
        #define FLL_FSR_LOCK1_MASK                                           0x100
        #define FLL_FSR_LOCK1_RESET                                          0x0
        
        // Clamp error low for FLL1. Set when new DCO value for FLL1 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR1_BIT                                     9
        #define FLL_FSR_CLMP_LO_ERR1_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR1_MASK                                    0x200
        #define FLL_FSR_CLMP_LO_ERR1_RESET                                   0x0
        
        // Clamp error high for FLL1. Set when new DCO value for FLL1 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR1_BIT                                     10
        #define FLL_FSR_CLMP_HI_ERR1_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR1_MASK                                    0x400
        #define FLL_FSR_CLMP_HI_ERR1_RESET                                   0x0
        
        // FDC saturation error for FLL1. Set when FDC counter of FLL1 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR1_BIT                                     11
        #define FLL_FSR_FDC_SAT_ERR1_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR1_MASK                                    0x800
        #define FLL_FSR_FDC_SAT_ERR1_RESET                                   0x0
        
        // LOCK2. Lock status for Feedback clock 3. (access: R)
        #define FLL_FSR_LOCK2_BIT                                            16
        #define FLL_FSR_LOCK2_WIDTH                                          1
        #define FLL_FSR_LOCK2_MASK                                           0x10000
        #define FLL_FSR_LOCK2_RESET                                          0x0
        
        // Clamp error low for FLL2. Set when new DCO value for FLL2 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR2_BIT                                     17
        #define FLL_FSR_CLMP_LO_ERR2_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR2_MASK                                    0x20000
        #define FLL_FSR_CLMP_LO_ERR2_RESET                                   0x0
        
        // Clamp error high for FLL2. Set when new DCO value for FLL2 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR2_BIT                                     18
        #define FLL_FSR_CLMP_HI_ERR2_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR2_MASK                                    0x40000
        #define FLL_FSR_CLMP_HI_ERR2_RESET                                   0x0
        
        // FDC saturation error for FLL2. Set when FDC counter of FLL2 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR2_BIT                                     19
        #define FLL_FSR_FDC_SAT_ERR2_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR2_MASK                                    0x80000
        #define FLL_FSR_FDC_SAT_ERR2_RESET                                   0x0
        
        // LOCK3. Lock status for Feedback clock 3. (access: R)
        #define FLL_FSR_LOCK3_BIT                                            24
        #define FLL_FSR_LOCK3_WIDTH                                          1
        #define FLL_FSR_LOCK3_MASK                                           0x1000000
        #define FLL_FSR_LOCK3_RESET                                          0x0
        
        // Clamp error low for FLL3. Set when new DCO value for FLL3 &lt; DCO_MIN value. (access: R)
        #define FLL_FSR_CLMP_LO_ERR3_BIT                                     25
        #define FLL_FSR_CLMP_LO_ERR3_WIDTH                                   1
        #define FLL_FSR_CLMP_LO_ERR3_MASK                                    0x2000000
        #define FLL_FSR_CLMP_LO_ERR3_RESET                                   0x0
        
        // Clamp error high for FLL3. Set when new DCO value for FLL3 &gt; DCO_MAX value. (access: R)
        #define FLL_FSR_CLMP_HI_ERR3_BIT                                     26
        #define FLL_FSR_CLMP_HI_ERR3_WIDTH                                   1
        #define FLL_FSR_CLMP_HI_ERR3_MASK                                    0x4000000
        #define FLL_FSR_CLMP_HI_ERR3_RESET                                   0x0
        
        // FDC saturation error for FLL3. Set when FDC counter of FLL3 overflows. (access: R)
        #define FLL_FSR_FDC_SAT_ERR3_BIT                                     27
        #define FLL_FSR_FDC_SAT_ERR3_WIDTH                                   1
        #define FLL_FSR_FDC_SAT_ERR3_MASK                                    0x8000000
        #define FLL_FSR_FDC_SAT_ERR3_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_FSR_LOCK0_GET(value)                           (GAP_BEXTRACTU((value),1,0))
        #define FLL_FSR_LOCK0_GETS(value)                          (GAP_BEXTRACT((value),1,0))
        #define FLL_FSR_LOCK0_SET(value,field)                     (GAP_BINSERT((value),(field),1,0))
        #define FLL_FSR_LOCK0(val)                                 ((val) << 0)
        
        #define FLL_FSR_CLMP_LO_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,1))
        #define FLL_FSR_CLMP_LO_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,1))
        #define FLL_FSR_CLMP_LO_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,1))
        #define FLL_FSR_CLMP_LO_ERR0(val)                          ((val) << 1)
        
        #define FLL_FSR_CLMP_HI_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,2))
        #define FLL_FSR_CLMP_HI_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,2))
        #define FLL_FSR_CLMP_HI_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,2))
        #define FLL_FSR_CLMP_HI_ERR0(val)                          ((val) << 2)
        
        #define FLL_FSR_FDC_SAT_ERR0_GET(value)                    (GAP_BEXTRACTU((value),1,3))
        #define FLL_FSR_FDC_SAT_ERR0_GETS(value)                   (GAP_BEXTRACT((value),1,3))
        #define FLL_FSR_FDC_SAT_ERR0_SET(value,field)              (GAP_BINSERT((value),(field),1,3))
        #define FLL_FSR_FDC_SAT_ERR0(val)                          ((val) << 3)
        
        #define FLL_FSR_LOCK1_GET(value)                           (GAP_BEXTRACTU((value),1,8))
        #define FLL_FSR_LOCK1_GETS(value)                          (GAP_BEXTRACT((value),1,8))
        #define FLL_FSR_LOCK1_SET(value,field)                     (GAP_BINSERT((value),(field),1,8))
        #define FLL_FSR_LOCK1(val)                                 ((val) << 8)
        
        #define FLL_FSR_CLMP_LO_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,9))
        #define FLL_FSR_CLMP_LO_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,9))
        #define FLL_FSR_CLMP_LO_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,9))
        #define FLL_FSR_CLMP_LO_ERR1(val)                          ((val) << 9)
        
        #define FLL_FSR_CLMP_HI_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,10))
        #define FLL_FSR_CLMP_HI_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,10))
        #define FLL_FSR_CLMP_HI_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,10))
        #define FLL_FSR_CLMP_HI_ERR1(val)                          ((val) << 10)
        
        #define FLL_FSR_FDC_SAT_ERR1_GET(value)                    (GAP_BEXTRACTU((value),1,11))
        #define FLL_FSR_FDC_SAT_ERR1_GETS(value)                   (GAP_BEXTRACT((value),1,11))
        #define FLL_FSR_FDC_SAT_ERR1_SET(value,field)              (GAP_BINSERT((value),(field),1,11))
        #define FLL_FSR_FDC_SAT_ERR1(val)                          ((val) << 11)
        
        #define FLL_FSR_LOCK2_GET(value)                           (GAP_BEXTRACTU((value),1,16))
        #define FLL_FSR_LOCK2_GETS(value)                          (GAP_BEXTRACT((value),1,16))
        #define FLL_FSR_LOCK2_SET(value,field)                     (GAP_BINSERT((value),(field),1,16))
        #define FLL_FSR_LOCK2(val)                                 ((val) << 16)
        
        #define FLL_FSR_CLMP_LO_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,17))
        #define FLL_FSR_CLMP_LO_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,17))
        #define FLL_FSR_CLMP_LO_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,17))
        #define FLL_FSR_CLMP_LO_ERR2(val)                          ((val) << 17)
        
        #define FLL_FSR_CLMP_HI_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,18))
        #define FLL_FSR_CLMP_HI_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,18))
        #define FLL_FSR_CLMP_HI_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,18))
        #define FLL_FSR_CLMP_HI_ERR2(val)                          ((val) << 18)
        
        #define FLL_FSR_FDC_SAT_ERR2_GET(value)                    (GAP_BEXTRACTU((value),1,19))
        #define FLL_FSR_FDC_SAT_ERR2_GETS(value)                   (GAP_BEXTRACT((value),1,19))
        #define FLL_FSR_FDC_SAT_ERR2_SET(value,field)              (GAP_BINSERT((value),(field),1,19))
        #define FLL_FSR_FDC_SAT_ERR2(val)                          ((val) << 19)
        
        #define FLL_FSR_LOCK3_GET(value)                           (GAP_BEXTRACTU((value),1,24))
        #define FLL_FSR_LOCK3_GETS(value)                          (GAP_BEXTRACT((value),1,24))
        #define FLL_FSR_LOCK3_SET(value,field)                     (GAP_BINSERT((value),(field),1,24))
        #define FLL_FSR_LOCK3(val)                                 ((val) << 24)
        
        #define FLL_FSR_CLMP_LO_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,25))
        #define FLL_FSR_CLMP_LO_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,25))
        #define FLL_FSR_CLMP_LO_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,25))
        #define FLL_FSR_CLMP_LO_ERR3(val)                          ((val) << 25)
        
        #define FLL_FSR_CLMP_HI_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,26))
        #define FLL_FSR_CLMP_HI_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,26))
        #define FLL_FSR_CLMP_HI_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,26))
        #define FLL_FSR_CLMP_HI_ERR3(val)                          ((val) << 26)
        
        #define FLL_FSR_FDC_SAT_ERR3_GET(value)                    (GAP_BEXTRACTU((value),1,27))
        #define FLL_FSR_FDC_SAT_ERR3_GETS(value)                   (GAP_BEXTRACT((value),1,27))
        #define FLL_FSR_FDC_SAT_ERR3_SET(value,field)              (GAP_BINSERT((value),(field),1,27))
        #define FLL_FSR_FDC_SAT_ERR3(val)                          ((val) << 27)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int lock0           :1 ; // LOCK0. Lock status for Feedback clock 0.
            unsigned int clmp_lo_err0    :1 ; // Clamp error low for FLL0. Set when new DCO value for FLL0 &lt; DCO_MIN value.
            unsigned int clmp_hi_err0    :1 ; // Clamp error high for FLL0. Set when new DCO value for FLL0 &gt; DCO_MAX value.
            unsigned int fdc_sat_err0    :1 ; // FDC saturation error for FLL0. Set when FDC counter of FLL0 overflows.
            unsigned int padding0:4 ;
            unsigned int lock1           :1 ; // LOCK1. Lock status for Feedback clock 1.
            unsigned int clmp_lo_err1    :1 ; // Clamp error low for FLL1. Set when new DCO value for FLL1 &lt; DCO_MIN value.
            unsigned int clmp_hi_err1    :1 ; // Clamp error high for FLL1. Set when new DCO value for FLL1 &gt; DCO_MAX value.
            unsigned int fdc_sat_err1    :1 ; // FDC saturation error for FLL1. Set when FDC counter of FLL1 overflows.
            unsigned int padding1:4 ;
            unsigned int lock2           :1 ; // LOCK2. Lock status for Feedback clock 3.
            unsigned int clmp_lo_err2    :1 ; // Clamp error low for FLL2. Set when new DCO value for FLL2 &lt; DCO_MIN value.
            unsigned int clmp_hi_err2    :1 ; // Clamp error high for FLL2. Set when new DCO value for FLL2 &gt; DCO_MAX value.
            unsigned int fdc_sat_err2    :1 ; // FDC saturation error for FLL2. Set when FDC counter of FLL2 overflows.
            unsigned int padding2:4 ;
            unsigned int lock3           :1 ; // LOCK3. Lock status for Feedback clock 3.
            unsigned int clmp_lo_err3    :1 ; // Clamp error low for FLL3. Set when new DCO value for FLL3 &lt; DCO_MIN value.
            unsigned int clmp_hi_err3    :1 ; // Clamp error high for FLL3. Set when new DCO value for FLL3 &gt; DCO_MAX value.
            unsigned int fdc_sat_err3    :1 ; // FDC saturation error for FLL3. Set when FDC counter of FLL3 overflows.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_fsr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_fsr : public vp::reg_32
        {
        public:
            inline void lock0_set(uint32_t value);
            inline uint32_t lock0_get();
            inline void clmp_lo_err0_set(uint32_t value);
            inline uint32_t clmp_lo_err0_get();
            inline void clmp_hi_err0_set(uint32_t value);
            inline uint32_t clmp_hi_err0_get();
            inline void fdc_sat_err0_set(uint32_t value);
            inline uint32_t fdc_sat_err0_get();
            inline void lock1_set(uint32_t value);
            inline uint32_t lock1_get();
            inline void clmp_lo_err1_set(uint32_t value);
            inline uint32_t clmp_lo_err1_get();
            inline void clmp_hi_err1_set(uint32_t value);
            inline uint32_t clmp_hi_err1_get();
            inline void fdc_sat_err1_set(uint32_t value);
            inline uint32_t fdc_sat_err1_get();
            inline void lock2_set(uint32_t value);
            inline uint32_t lock2_get();
            inline void clmp_lo_err2_set(uint32_t value);
            inline uint32_t clmp_lo_err2_get();
            inline void clmp_hi_err2_set(uint32_t value);
            inline uint32_t clmp_hi_err2_get();
            inline void fdc_sat_err2_set(uint32_t value);
            inline uint32_t fdc_sat_err2_get();
            inline void lock3_set(uint32_t value);
            inline uint32_t lock3_get();
            inline void clmp_lo_err3_set(uint32_t value);
            inline uint32_t clmp_lo_err3_get();
            inline void clmp_hi_err3_set(uint32_t value);
            inline uint32_t clmp_hi_err3_get();
            inline void fdc_sat_err3_set(uint32_t value);
            inline uint32_t fdc_sat_err3_get();
        };

|

.. _fll_DRR:

DRR
"""

DCO Range register

.. table:: 

    +-----+---+-------+-----------------------------------+
    |Bit #|R/W| Name  |            Description            |
    +=====+===+=======+===================================+
    |8:0  |R/W|DCO_MIN|Minimum value allowed for DCO code.|
    +-----+---+-------+-----------------------------------+
    |24:16|R/W|DCO_MAX|Maximum value allowed for DCO code.|
    +-----+---+-------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // DCO Range register
                #define FLL_DRR_OFFSET                           0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_drr_get(uint32_t base);
        static inline void fll_drr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Minimum value allowed for DCO code. (access: R/W)
        #define FLL_DRR_DCO_MIN_BIT                                          0
        #define FLL_DRR_DCO_MIN_WIDTH                                        9
        #define FLL_DRR_DCO_MIN_MASK                                         0x1ff
        #define FLL_DRR_DCO_MIN_RESET                                        0x0
        
        // Maximum value allowed for DCO code. (access: R/W)
        #define FLL_DRR_DCO_MAX_BIT                                          16
        #define FLL_DRR_DCO_MAX_WIDTH                                        9
        #define FLL_DRR_DCO_MAX_MASK                                         0x1ff0000
        #define FLL_DRR_DCO_MAX_RESET                                        0x96

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_DRR_DCO_MIN_GET(value)                         (GAP_BEXTRACTU((value),9,0))
        #define FLL_DRR_DCO_MIN_GETS(value)                        (GAP_BEXTRACT((value),9,0))
        #define FLL_DRR_DCO_MIN_SET(value,field)                   (GAP_BINSERT((value),(field),9,0))
        #define FLL_DRR_DCO_MIN(val)                               ((val) << 0)
        
        #define FLL_DRR_DCO_MAX_GET(value)                         (GAP_BEXTRACTU((value),9,16))
        #define FLL_DRR_DCO_MAX_GETS(value)                        (GAP_BEXTRACT((value),9,16))
        #define FLL_DRR_DCO_MAX_SET(value,field)                   (GAP_BINSERT((value),(field),9,16))
        #define FLL_DRR_DCO_MAX(val)                               ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dco_min         :9 ; // Minimum value allowed for DCO code.
            unsigned int padding0:7 ;
            unsigned int dco_max         :9 ; // Maximum value allowed for DCO code.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_drr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_drr : public vp::reg_32
        {
        public:
            inline void dco_min_set(uint32_t value);
            inline uint32_t dco_min_get();
            inline void dco_max_set(uint32_t value);
            inline uint32_t dco_max_get();
        };

|

.. _fll_TTR:

TTR
"""

Temperature Tracking Register

.. table:: 

    +-----+---+-------+-----------------------------------------------------------+
    |Bit #|R/W| Name  |                        Description                        |
    +=====+===+=======+===========================================================+
    |23:0 |R/W|REFRESH|Number of ref clock cycles between two integration periods.|
    +-----+---+-------+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Temperature Tracking Register
                #define FLL_TTR_OFFSET                           0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_ttr_get(uint32_t base);
        static inline void fll_ttr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Number of ref clock cycles between two integration periods. (access: R/W)
        #define FLL_TTR_REFRESH_BIT                                          0
        #define FLL_TTR_REFRESH_WIDTH                                        24
        #define FLL_TTR_REFRESH_MASK                                         0xffffff
        #define FLL_TTR_REFRESH_RESET                                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_TTR_REFRESH_GET(value)                         (GAP_BEXTRACTU((value),24,0))
        #define FLL_TTR_REFRESH_GETS(value)                        (GAP_BEXTRACT((value),24,0))
        #define FLL_TTR_REFRESH_SET(value,field)                   (GAP_BINSERT((value),(field),24,0))
        #define FLL_TTR_REFRESH(val)                               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int refresh         :24; // Number of ref clock cycles between two integration periods.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ttr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_ttr : public vp::reg_32
        {
        public:
            inline void refresh_set(uint32_t value);
            inline uint32_t refresh_get();
        };

|

.. _fll_F0CR1:

F0CR1
"""""

FLL0 Configuration Register 1

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                               Description                                                                                                               |
    +=====+===+=========+=========================================================================================================================================================================================================================================+
    |    0|R/W|DCO_EN   |DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled – FBKCLK is managed according to the FLL configuration (default state).                                                                      |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|OP_MODE  |FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.                                                                                                                                                              |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|TTM_EN   |FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.   |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|LOOP_GAIN|FLL loop gain setting. Default: 2-7 = 1 / 256.                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|LOCK_TOL |FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|ITG_PER  |FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:26|R/W|STBL     |FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL0 Configuration Register 1
                #define FLL_F0CR1_OFFSET                         0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f0cr1_get(uint32_t base);
        static inline void fll_f0cr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F0CR1_DCO_EN_BIT                                         0
        #define FLL_F0CR1_DCO_EN_WIDTH                                       1
        #define FLL_F0CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F0CR1_OP_MODE_BIT                                        1
        #define FLL_F0CR1_OP_MODE_WIDTH                                      1
        #define FLL_F0CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F0CR1_TTM_EN_BIT                                         2
        #define FLL_F0CR1_TTM_EN_WIDTH                                       1
        #define FLL_F0CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F0CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F0CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F0CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F0CR1_LOCK_TOL_BIT                                       8
        #define FLL_F0CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F0CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F0CR1_ITG_PER_BIT                                        16
        #define FLL_F0CR1_ITG_PER_WIDTH                                      10
        #define FLL_F0CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F0CR1_STBL_BIT                                           26
        #define FLL_F0CR1_STBL_WIDTH                                         6
        #define FLL_F0CR1_STBL_MASK                                          0xfc000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F0CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F0CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F0CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F0CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F0CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F0CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F0CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F0CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F0CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F0CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F0CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F0CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F0CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F0CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F0CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F0CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F0CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F0CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F0CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F0CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F0CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F0CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F0CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F0CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F0CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F0CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F0CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F0CR1_STBL(val)                                ((val) << 26)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f0cr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f0cr1 : public vp::reg_32
        {
        public:
            inline void dco_en_set(uint32_t value);
            inline uint32_t dco_en_get();
            inline void op_mode_set(uint32_t value);
            inline uint32_t op_mode_get();
            inline void ttm_en_set(uint32_t value);
            inline uint32_t ttm_en_get();
            inline void loop_gain_set(uint32_t value);
            inline uint32_t loop_gain_get();
            inline void lock_tol_set(uint32_t value);
            inline uint32_t lock_tol_get();
            inline void itg_per_set(uint32_t value);
            inline uint32_t itg_per_get();
            inline void stbl_set(uint32_t value);
            inline uint32_t stbl_get();
        };

|

.. _fll_F0CR2:

F0CR2
"""""

FLL0 Configuration Register 2

.. table:: 

    +-----+---+--------+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                     Description                                      |
    +=====+===+========+======================================================================================+
    |15:0 |R/W|MFI     |Target clock multiplication factor per integration period for FLL0 (closed loop mode).|
    +-----+---+--------+--------------------------------------------------------------------------------------+
    |24:16|R/W|DCO_CODE|DCO input code for FLL0 (open loop mode).                                             |
    +-----+---+--------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL0 Configuration Register 2
                #define FLL_F0CR2_OFFSET                         0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f0cr2_get(uint32_t base);
        static inline void fll_f0cr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F0CR2_MFI_BIT                                            0
        #define FLL_F0CR2_MFI_WIDTH                                          16
        #define FLL_F0CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F0CR2_DCO_CODE_BIT                                       16
        #define FLL_F0CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F0CR2_DCO_CODE_MASK                                      0x1ff0000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F0CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F0CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F0CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F0CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F0CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F0CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F0CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F0CR2_DCO_CODE(val)                            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f0cr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f0cr2 : public vp::reg_32
        {
        public:
            inline void mfi_set(uint32_t value);
            inline uint32_t mfi_get();
            inline void dco_code_set(uint32_t value);
            inline uint32_t dco_code_get();
        };

|

.. _fll_F1CR1:

F1CR1
"""""

FLL1 Configuration Register 1

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                               Description                                                                                                               |
    +=====+===+=========+=========================================================================================================================================================================================================================================+
    |    0|R/W|DCO_EN   |DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled – FBKCLK is managed according to the FLL configuration (default state).                                                                      |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|OP_MODE  |FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.                                                                                                                                                              |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|TTM_EN   |FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.   |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|LOOP_GAIN|FLL loop gain setting. Default: 2-7 = 1 / 256.                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|LOCK_TOL |FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|ITG_PER  |FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:26|R/W|STBL     |FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL1 Configuration Register 1
                #define FLL_F1CR1_OFFSET                         0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f1cr1_get(uint32_t base);
        static inline void fll_f1cr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F1CR1_DCO_EN_BIT                                         0
        #define FLL_F1CR1_DCO_EN_WIDTH                                       1
        #define FLL_F1CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F1CR1_OP_MODE_BIT                                        1
        #define FLL_F1CR1_OP_MODE_WIDTH                                      1
        #define FLL_F1CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F1CR1_TTM_EN_BIT                                         2
        #define FLL_F1CR1_TTM_EN_WIDTH                                       1
        #define FLL_F1CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F1CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F1CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F1CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F1CR1_LOCK_TOL_BIT                                       8
        #define FLL_F1CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F1CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F1CR1_ITG_PER_BIT                                        16
        #define FLL_F1CR1_ITG_PER_WIDTH                                      10
        #define FLL_F1CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F1CR1_STBL_BIT                                           26
        #define FLL_F1CR1_STBL_WIDTH                                         6
        #define FLL_F1CR1_STBL_MASK                                          0xfc000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F1CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F1CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F1CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F1CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F1CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F1CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F1CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F1CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F1CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F1CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F1CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F1CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F1CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F1CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F1CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F1CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F1CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F1CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F1CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F1CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F1CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F1CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F1CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F1CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F1CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F1CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F1CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F1CR1_STBL(val)                                ((val) << 26)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f1cr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f1cr1 : public vp::reg_32
        {
        public:
            inline void dco_en_set(uint32_t value);
            inline uint32_t dco_en_get();
            inline void op_mode_set(uint32_t value);
            inline uint32_t op_mode_get();
            inline void ttm_en_set(uint32_t value);
            inline uint32_t ttm_en_get();
            inline void loop_gain_set(uint32_t value);
            inline uint32_t loop_gain_get();
            inline void lock_tol_set(uint32_t value);
            inline uint32_t lock_tol_get();
            inline void itg_per_set(uint32_t value);
            inline uint32_t itg_per_get();
            inline void stbl_set(uint32_t value);
            inline uint32_t stbl_get();
        };

|

.. _fll_F1CR2:

F1CR2
"""""

FLL1 Configuration Register 2

.. table:: 

    +-----+---+--------+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                     Description                                      |
    +=====+===+========+======================================================================================+
    |15:0 |R/W|MFI     |Target clock multiplication factor per integration period for FLL0 (closed loop mode).|
    +-----+---+--------+--------------------------------------------------------------------------------------+
    |24:16|R/W|DCO_CODE|DCO input code for FLL0 (open loop mode).                                             |
    +-----+---+--------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL1 Configuration Register 2
                #define FLL_F1CR2_OFFSET                         0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f1cr2_get(uint32_t base);
        static inline void fll_f1cr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F1CR2_MFI_BIT                                            0
        #define FLL_F1CR2_MFI_WIDTH                                          16
        #define FLL_F1CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F1CR2_DCO_CODE_BIT                                       16
        #define FLL_F1CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F1CR2_DCO_CODE_MASK                                      0x1ff0000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F1CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F1CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F1CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F1CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F1CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F1CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F1CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F1CR2_DCO_CODE(val)                            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f1cr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f1cr2 : public vp::reg_32
        {
        public:
            inline void mfi_set(uint32_t value);
            inline uint32_t mfi_get();
            inline void dco_code_set(uint32_t value);
            inline uint32_t dco_code_get();
        };

|

.. _fll_F2CR1:

F2CR1
"""""

FLL2 Configuration Register 1

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                               Description                                                                                                               |
    +=====+===+=========+=========================================================================================================================================================================================================================================+
    |    0|R/W|DCO_EN   |DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled – FBKCLK is managed according to the FLL configuration (default state).                                                                      |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|OP_MODE  |FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.                                                                                                                                                              |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|TTM_EN   |FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.   |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|LOOP_GAIN|FLL loop gain setting. Default: 2-7 = 1 / 256.                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|LOCK_TOL |FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|ITG_PER  |FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:26|R/W|STBL     |FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL2 Configuration Register 1
                #define FLL_F2CR1_OFFSET                         0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f2cr1_get(uint32_t base);
        static inline void fll_f2cr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F2CR1_DCO_EN_BIT                                         0
        #define FLL_F2CR1_DCO_EN_WIDTH                                       1
        #define FLL_F2CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F2CR1_OP_MODE_BIT                                        1
        #define FLL_F2CR1_OP_MODE_WIDTH                                      1
        #define FLL_F2CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F2CR1_TTM_EN_BIT                                         2
        #define FLL_F2CR1_TTM_EN_WIDTH                                       1
        #define FLL_F2CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F2CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F2CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F2CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F2CR1_LOCK_TOL_BIT                                       8
        #define FLL_F2CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F2CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F2CR1_ITG_PER_BIT                                        16
        #define FLL_F2CR1_ITG_PER_WIDTH                                      10
        #define FLL_F2CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F2CR1_STBL_BIT                                           26
        #define FLL_F2CR1_STBL_WIDTH                                         6
        #define FLL_F2CR1_STBL_MASK                                          0xfc000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F2CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F2CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F2CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F2CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F2CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F2CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F2CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F2CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F2CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F2CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F2CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F2CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F2CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F2CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F2CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F2CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F2CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F2CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F2CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F2CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F2CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F2CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F2CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F2CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F2CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F2CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F2CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F2CR1_STBL(val)                                ((val) << 26)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f2cr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f2cr1 : public vp::reg_32
        {
        public:
            inline void dco_en_set(uint32_t value);
            inline uint32_t dco_en_get();
            inline void op_mode_set(uint32_t value);
            inline uint32_t op_mode_get();
            inline void ttm_en_set(uint32_t value);
            inline uint32_t ttm_en_get();
            inline void loop_gain_set(uint32_t value);
            inline uint32_t loop_gain_get();
            inline void lock_tol_set(uint32_t value);
            inline uint32_t lock_tol_get();
            inline void itg_per_set(uint32_t value);
            inline uint32_t itg_per_get();
            inline void stbl_set(uint32_t value);
            inline uint32_t stbl_get();
        };

|

.. _fll_F2CR2:

F2CR2
"""""

FLL2 Configuration Register 2

.. table:: 

    +-----+---+--------+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                     Description                                      |
    +=====+===+========+======================================================================================+
    |15:0 |R/W|MFI     |Target clock multiplication factor per integration period for FLL0 (closed loop mode).|
    +-----+---+--------+--------------------------------------------------------------------------------------+
    |24:16|R/W|DCO_CODE|DCO input code for FLL0 (open loop mode).                                             |
    +-----+---+--------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL2 Configuration Register 2
                #define FLL_F2CR2_OFFSET                         0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f2cr2_get(uint32_t base);
        static inline void fll_f2cr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F2CR2_MFI_BIT                                            0
        #define FLL_F2CR2_MFI_WIDTH                                          16
        #define FLL_F2CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F2CR2_DCO_CODE_BIT                                       16
        #define FLL_F2CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F2CR2_DCO_CODE_MASK                                      0x1ff0000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F2CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F2CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F2CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F2CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F2CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F2CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F2CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F2CR2_DCO_CODE(val)                            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f2cr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f2cr2 : public vp::reg_32
        {
        public:
            inline void mfi_set(uint32_t value);
            inline uint32_t mfi_get();
            inline void dco_code_set(uint32_t value);
            inline uint32_t dco_code_get();
        };

|

.. _fll_F3CR1:

F3CR1
"""""

FLL3 Configuration Register 1

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                               Description                                                                                                               |
    +=====+===+=========+=========================================================================================================================================================================================================================================+
    |    0|R/W|DCO_EN   |DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled – FBKCLK is managed according to the FLL configuration (default state).                                                                      |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|OP_MODE  |FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.                                                                                                                                                              |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|TTM_EN   |FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.   |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|LOOP_GAIN|FLL loop gain setting. Default: 2-7 = 1 / 256.                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|LOCK_TOL |FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|ITG_PER  |FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:26|R/W|STBL     |FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL3 Configuration Register 1
                #define FLL_F3CR1_OFFSET                         0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f3cr1_get(uint32_t base);
        static inline void fll_f3cr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state). (access: R/W)
        #define FLL_F3CR1_DCO_EN_BIT                                         0
        #define FLL_F3CR1_DCO_EN_WIDTH                                       1
        #define FLL_F3CR1_DCO_EN_MASK                                        0x1
        
        // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode. (access: R/W)
        #define FLL_F3CR1_OP_MODE_BIT                                        1
        #define FLL_F3CR1_OP_MODE_WIDTH                                      1
        #define FLL_F3CR1_OP_MODE_MASK                                       0x2
        
        // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter. (access: R/W)
        #define FLL_F3CR1_TTM_EN_BIT                                         2
        #define FLL_F3CR1_TTM_EN_WIDTH                                       1
        #define FLL_F3CR1_TTM_EN_MASK                                        0x4
        
        // FLL loop gain setting. Default: 2-7 = 1 / 256. (access: R/W)
        #define FLL_F3CR1_LOOP_GAIN_BIT                                      4
        #define FLL_F3CR1_LOOP_GAIN_WIDTH                                    4
        #define FLL_F3CR1_LOOP_GAIN_MASK                                     0xf0
        
        // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target (access: R/W)
        #define FLL_F3CR1_LOCK_TOL_BIT                                       8
        #define FLL_F3CR1_LOCK_TOL_WIDTH                                     8
        #define FLL_F3CR1_LOCK_TOL_MASK                                      0xff00
        
        // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles (access: R/W)
        #define FLL_F3CR1_ITG_PER_BIT                                        16
        #define FLL_F3CR1_ITG_PER_WIDTH                                      10
        #define FLL_F3CR1_ITG_PER_MASK                                       0x3ff0000
        
        // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted. (access: R/W)
        #define FLL_F3CR1_STBL_BIT                                           26
        #define FLL_F3CR1_STBL_WIDTH                                         6
        #define FLL_F3CR1_STBL_MASK                                          0xfc000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F3CR1_DCO_EN_GET(value)                        (GAP_BEXTRACTU((value),1,0))
        #define FLL_F3CR1_DCO_EN_GETS(value)                       (GAP_BEXTRACT((value),1,0))
        #define FLL_F3CR1_DCO_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,0))
        #define FLL_F3CR1_DCO_EN(val)                              ((val) << 0)
        
        #define FLL_F3CR1_OP_MODE_GET(value)                       (GAP_BEXTRACTU((value),1,1))
        #define FLL_F3CR1_OP_MODE_GETS(value)                      (GAP_BEXTRACT((value),1,1))
        #define FLL_F3CR1_OP_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),1,1))
        #define FLL_F3CR1_OP_MODE(val)                             ((val) << 1)
        
        #define FLL_F3CR1_TTM_EN_GET(value)                        (GAP_BEXTRACTU((value),1,2))
        #define FLL_F3CR1_TTM_EN_GETS(value)                       (GAP_BEXTRACT((value),1,2))
        #define FLL_F3CR1_TTM_EN_SET(value,field)                  (GAP_BINSERT((value),(field),1,2))
        #define FLL_F3CR1_TTM_EN(val)                              ((val) << 2)
        
        #define FLL_F3CR1_LOOP_GAIN_GET(value)                     (GAP_BEXTRACTU((value),4,4))
        #define FLL_F3CR1_LOOP_GAIN_GETS(value)                    (GAP_BEXTRACT((value),4,4))
        #define FLL_F3CR1_LOOP_GAIN_SET(value,field)               (GAP_BINSERT((value),(field),4,4))
        #define FLL_F3CR1_LOOP_GAIN(val)                           ((val) << 4)
        
        #define FLL_F3CR1_LOCK_TOL_GET(value)                      (GAP_BEXTRACTU((value),8,8))
        #define FLL_F3CR1_LOCK_TOL_GETS(value)                     (GAP_BEXTRACT((value),8,8))
        #define FLL_F3CR1_LOCK_TOL_SET(value,field)                (GAP_BINSERT((value),(field),8,8))
        #define FLL_F3CR1_LOCK_TOL(val)                            ((val) << 8)
        
        #define FLL_F3CR1_ITG_PER_GET(value)                       (GAP_BEXTRACTU((value),10,16))
        #define FLL_F3CR1_ITG_PER_GETS(value)                      (GAP_BEXTRACT((value),10,16))
        #define FLL_F3CR1_ITG_PER_SET(value,field)                 (GAP_BINSERT((value),(field),10,16))
        #define FLL_F3CR1_ITG_PER(val)                             ((val) << 16)
        
        #define FLL_F3CR1_STBL_GET(value)                          (GAP_BEXTRACTU((value),6,26))
        #define FLL_F3CR1_STBL_GETS(value)                         (GAP_BEXTRACT((value),6,26))
        #define FLL_F3CR1_STBL_SET(value,field)                    (GAP_BINSERT((value),(field),6,26))
        #define FLL_F3CR1_STBL(val)                                ((val) << 26)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dco_en          :1 ; // DCO enable for FLL (active high). 0: DCO is disabled. FBKCLK is inactive. 1: DCO is enabled  FBKCLK is managed according to the FLL configuration (default state).
            unsigned int op_mode         :1 ; // FLL Operating mode. 0: open loop mode (default state). 1: closed loop mode.
            unsigned int ttm_en          :1 ; // FLL temperature tracking mode enable. In open loop mode: do not care. In closed loop mode: 0: the frequency is always regulated (at each integration period). 1: the frequency is regulated at a rate controlled by REFRESH parameter.
            unsigned int padding0:1 ;
            unsigned int loop_gain       :4 ; // FLL loop gain setting. Default: 2-7 = 1 / 256.
            unsigned int lock_tol        :8 ; // FLL Lock tolerance. Margin around the target multiplication factor per integration period (MFI) within which the output clock is considered stable (i.e. the clock is stable when target
            unsigned int itg_per         :10; // FLL Integration period. Define the duration of one integration period i.e. the number of REFCLK cycles during which the FDC counter is enabled. Integration period duration = (ITG_PER[9:0] + 1) REFCLK cycles
            unsigned int stbl            :6 ; // FLL stable clock cycles before asserting LOCK. In closed loop mode, number of REFCLK cycles during which the output clock is considered stable until LOCK is asserted. In open loop mode, number of FBKCLK cycles until LOCK is asserted.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f3cr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f3cr1 : public vp::reg_32
        {
        public:
            inline void dco_en_set(uint32_t value);
            inline uint32_t dco_en_get();
            inline void op_mode_set(uint32_t value);
            inline uint32_t op_mode_get();
            inline void ttm_en_set(uint32_t value);
            inline uint32_t ttm_en_get();
            inline void loop_gain_set(uint32_t value);
            inline uint32_t loop_gain_get();
            inline void lock_tol_set(uint32_t value);
            inline uint32_t lock_tol_get();
            inline void itg_per_set(uint32_t value);
            inline uint32_t itg_per_get();
            inline void stbl_set(uint32_t value);
            inline uint32_t stbl_get();
        };

|

.. _fll_F3CR2:

F3CR2
"""""

FLL3 Configuration Register 2

.. table:: 

    +-----+---+--------+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                     Description                                      |
    +=====+===+========+======================================================================================+
    |15:0 |R/W|MFI     |Target clock multiplication factor per integration period for FLL0 (closed loop mode).|
    +-----+---+--------+--------------------------------------------------------------------------------------+
    |24:16|R/W|DCO_CODE|DCO input code for FLL0 (open loop mode).                                             |
    +-----+---+--------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FLL3 Configuration Register 2
                #define FLL_F3CR2_OFFSET                         0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_f3cr2_get(uint32_t base);
        static inline void fll_f3cr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Target clock multiplication factor per integration period for FLL0 (closed loop mode). (access: R/W)
        #define FLL_F3CR2_MFI_BIT                                            0
        #define FLL_F3CR2_MFI_WIDTH                                          16
        #define FLL_F3CR2_MFI_MASK                                           0xffff
        
        // DCO input code for FLL0 (open loop mode). (access: R/W)
        #define FLL_F3CR2_DCO_CODE_BIT                                       16
        #define FLL_F3CR2_DCO_CODE_WIDTH                                     9
        #define FLL_F3CR2_DCO_CODE_MASK                                      0x1ff0000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_F3CR2_MFI_GET(value)                           (GAP_BEXTRACTU((value),16,0))
        #define FLL_F3CR2_MFI_GETS(value)                          (GAP_BEXTRACT((value),16,0))
        #define FLL_F3CR2_MFI_SET(value,field)                     (GAP_BINSERT((value),(field),16,0))
        #define FLL_F3CR2_MFI(val)                                 ((val) << 0)
        
        #define FLL_F3CR2_DCO_CODE_GET(value)                      (GAP_BEXTRACTU((value),9,16))
        #define FLL_F3CR2_DCO_CODE_GETS(value)                     (GAP_BEXTRACT((value),9,16))
        #define FLL_F3CR2_DCO_CODE_SET(value,field)                (GAP_BINSERT((value),(field),9,16))
        #define FLL_F3CR2_DCO_CODE(val)                            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mfi             :16; // Target clock multiplication factor per integration period for FLL0 (closed loop mode).
            unsigned int dco_code        :9 ; // DCO input code for FLL0 (open loop mode).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_f3cr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_f3cr2 : public vp::reg_32
        {
        public:
            inline void mfi_set(uint32_t value);
            inline uint32_t mfi_get();
            inline void dco_code_set(uint32_t value);
            inline uint32_t dco_code_get();
        };

|

.. _fll_CCR1:

CCR1
""""

Clock Configuration register 1

.. table:: 

    +-----+---+--------+-------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                            Description                            |
    +=====+===+========+===================================================================+
    |7:0  |R/W|CLK0_DIV|Clock divider setting for OUTCLK[0]. OUTCLK[0] = CLK0 / (CLK0_DIV).|
    +-----+---+--------+-------------------------------------------------------------------+
    |15:8 |R/W|CLK1_DIV|Clock divider setting for OUTCLK[1]. OUTCLK[1] = CLK1 / (CLK1_DIV).|
    +-----+---+--------+-------------------------------------------------------------------+
    |23:16|R/W|CLK2_DIV|Clock divider setting for OUTCLK[2]. OUTCLK[2] = CLK2 / (CLK2_DIV).|
    +-----+---+--------+-------------------------------------------------------------------+
    |31:24|R/W|CLK3_DIV|Clock divider setting for OUTCLK[3]. OUTCLK[3] = CLK3 / (CLK3_DIV).|
    +-----+---+--------+-------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock Configuration register 1
                #define FLL_CCR1_OFFSET                          0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_ccr1_get(uint32_t base);
        static inline void fll_ccr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock divider setting for OUTCLK[0]. OUTCLK[0] = CLK0 / (CLK0_DIV). (access: R/W)
        #define FLL_CCR1_CLK0_DIV_BIT                                        0
        #define FLL_CCR1_CLK0_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK0_DIV_MASK                                       0xff
        
        // Clock divider setting for OUTCLK[1]. OUTCLK[1] = CLK1 / (CLK1_DIV). (access: R/W)
        #define FLL_CCR1_CLK1_DIV_BIT                                        8
        #define FLL_CCR1_CLK1_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK1_DIV_MASK                                       0xff00
        
        // Clock divider setting for OUTCLK[2]. OUTCLK[2] = CLK2 / (CLK2_DIV). (access: R/W)
        #define FLL_CCR1_CLK2_DIV_BIT                                        16
        #define FLL_CCR1_CLK2_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK2_DIV_MASK                                       0xff0000
        
        // Clock divider setting for OUTCLK[3]. OUTCLK[3] = CLK3 / (CLK3_DIV). (access: R/W)
        #define FLL_CCR1_CLK3_DIV_BIT                                        24
        #define FLL_CCR1_CLK3_DIV_WIDTH                                      8
        #define FLL_CCR1_CLK3_DIV_MASK                                       0xff000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_CCR1_CLK0_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,0))
        #define FLL_CCR1_CLK0_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,0))
        #define FLL_CCR1_CLK0_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,0))
        #define FLL_CCR1_CLK0_DIV(val)                             ((val) << 0)
        
        #define FLL_CCR1_CLK1_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,8))
        #define FLL_CCR1_CLK1_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,8))
        #define FLL_CCR1_CLK1_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,8))
        #define FLL_CCR1_CLK1_DIV(val)                             ((val) << 8)
        
        #define FLL_CCR1_CLK2_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,16))
        #define FLL_CCR1_CLK2_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,16))
        #define FLL_CCR1_CLK2_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,16))
        #define FLL_CCR1_CLK2_DIV(val)                             ((val) << 16)
        
        #define FLL_CCR1_CLK3_DIV_GET(value)                       (GAP_BEXTRACTU((value),8,24))
        #define FLL_CCR1_CLK3_DIV_GETS(value)                      (GAP_BEXTRACT((value),8,24))
        #define FLL_CCR1_CLK3_DIV_SET(value,field)                 (GAP_BINSERT((value),(field),8,24))
        #define FLL_CCR1_CLK3_DIV(val)                             ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk0_div        :8 ; // Clock divider setting for OUTCLK[0]. OUTCLK[0] = CLK0 / (CLK0_DIV).
            unsigned int clk1_div        :8 ; // Clock divider setting for OUTCLK[1]. OUTCLK[1] = CLK1 / (CLK1_DIV).
            unsigned int clk2_div        :8 ; // Clock divider setting for OUTCLK[2]. OUTCLK[2] = CLK2 / (CLK2_DIV).
            unsigned int clk3_div        :8 ; // Clock divider setting for OUTCLK[3]. OUTCLK[3] = CLK3 / (CLK3_DIV).
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ccr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_ccr1 : public vp::reg_32
        {
        public:
            inline void clk0_div_set(uint32_t value);
            inline uint32_t clk0_div_get();
            inline void clk1_div_set(uint32_t value);
            inline uint32_t clk1_div_get();
            inline void clk2_div_set(uint32_t value);
            inline uint32_t clk2_div_get();
            inline void clk3_div_set(uint32_t value);
            inline uint32_t clk3_div_get();
        };

|

.. _fll_CCR2:

CCR2
""""

Clock Configuration register 2

.. table:: 

    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                      Description                                                                      |
    +=====+===+========+=======================================================================================================================================================+
    |    0|R/W|CLK0_SEL|Clock source selection for OUTCLK[0]: 0: Ref clock (default). 1: FBKCLK[0] clock.                                                                      |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |5:4  |R/W|CLK1_SEL|Clock source selection for OUTCLK[1]: 00: Ref clock. 01: FBKCLK[0] clock (default). 1x: FBKCLK[1] clock.                                               |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|CLK2_SEL|Clock source selection for OUTCLK[2]: 00: Ref clock. 01: FBKCLK[0] clock (default). 10: FBKCLK[1] clock. 11: FBKCLK[2] clock.                          |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14:12|R/W|CLK3_SEL|Clock source selection for OUTCLK[3]: 000: Ref clock. 001: FBKCLK[0] clock (default). 010: FBKCLK[1] clock. 011: FBKCLK[2] clock. 1xx: FBKCLK[3] clock.|
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |16   |R/W|CKG0    |FBKCLK[0] clock gated. 0: FBKCLK[0] is not gated. 1: FBKCLK[0] is clock gated by LOCK0 signal.                                                         |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |17   |R/W|CKG1    |FBKCLK[1] clock gated. 0: FBKCLK[1] is not gated. 1: FBKCLK[1] is clock gated by LOCK1 signal.                                                         |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18   |R/W|CKG2    |FBKCLK[2] clock gated. 0: FBKCLK[2] is not gated. 1: FBKCLK[2] is clock gated by LOCK2 signal.                                                         |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19   |R/W|CKG3    |FBKCLK[3] clock gated. 0: FBKCLK[3] is not gated. 1: FBKCLK[3] is clock gated by LOCK3 signal.                                                         |
    +-----+---+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock Configuration register 2
                #define FLL_CCR2_OFFSET                          0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fll_ccr2_get(uint32_t base);
        static inline void fll_ccr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock source selection for OUTCLK[0]: 0: Ref clock (default). 1: FBKCLK[0] clock. (access: R/W)
        #define FLL_CCR2_CLK0_SEL_BIT                                        0
        #define FLL_CCR2_CLK0_SEL_WIDTH                                      1
        #define FLL_CCR2_CLK0_SEL_MASK                                       0x1
        
        // Clock source selection for OUTCLK[1]: 00: Ref clock. 01: FBKCLK[0] clock (default). 1x: FBKCLK[1] clock. (access: R/W)
        #define FLL_CCR2_CLK1_SEL_BIT                                        4
        #define FLL_CCR2_CLK1_SEL_WIDTH                                      2
        #define FLL_CCR2_CLK1_SEL_MASK                                       0x30
        
        // Clock source selection for OUTCLK[2]: 00: Ref clock. 01: FBKCLK[0] clock (default). 10: FBKCLK[1] clock. 11: FBKCLK[2] clock. (access: R/W)
        #define FLL_CCR2_CLK2_SEL_BIT                                        8
        #define FLL_CCR2_CLK2_SEL_WIDTH                                      2
        #define FLL_CCR2_CLK2_SEL_MASK                                       0x300
        
        // Clock source selection for OUTCLK[3]: 000: Ref clock. 001: FBKCLK[0] clock (default). 010: FBKCLK[1] clock. 011: FBKCLK[2] clock. 1xx: FBKCLK[3] clock. (access: R/W)
        #define FLL_CCR2_CLK3_SEL_BIT                                        12
        #define FLL_CCR2_CLK3_SEL_WIDTH                                      3
        #define FLL_CCR2_CLK3_SEL_MASK                                       0x7000
        
        // FBKCLK[0] clock gated. 0: FBKCLK[0] is not gated. 1: FBKCLK[0] is clock gated by LOCK0 signal. (access: R/W)
        #define FLL_CCR2_CKG0_BIT                                            16
        #define FLL_CCR2_CKG0_WIDTH                                          1
        #define FLL_CCR2_CKG0_MASK                                           0x10000
        
        // FBKCLK[1] clock gated. 0: FBKCLK[1] is not gated. 1: FBKCLK[1] is clock gated by LOCK1 signal. (access: R/W)
        #define FLL_CCR2_CKG1_BIT                                            17
        #define FLL_CCR2_CKG1_WIDTH                                          1
        #define FLL_CCR2_CKG1_MASK                                           0x20000
        
        // FBKCLK[2] clock gated. 0: FBKCLK[2] is not gated. 1: FBKCLK[2] is clock gated by LOCK2 signal. (access: R/W)
        #define FLL_CCR2_CKG2_BIT                                            18
        #define FLL_CCR2_CKG2_WIDTH                                          1
        #define FLL_CCR2_CKG2_MASK                                           0x40000
        
        // FBKCLK[3] clock gated. 0: FBKCLK[3] is not gated. 1: FBKCLK[3] is clock gated by LOCK3 signal. (access: R/W)
        #define FLL_CCR2_CKG3_BIT                                            19
        #define FLL_CCR2_CKG3_WIDTH                                          1
        #define FLL_CCR2_CKG3_MASK                                           0x80000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FLL_CCR2_CLK0_SEL_GET(value)                       (GAP_BEXTRACTU((value),1,0))
        #define FLL_CCR2_CLK0_SEL_GETS(value)                      (GAP_BEXTRACT((value),1,0))
        #define FLL_CCR2_CLK0_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),1,0))
        #define FLL_CCR2_CLK0_SEL(val)                             ((val) << 0)
        
        #define FLL_CCR2_CLK1_SEL_GET(value)                       (GAP_BEXTRACTU((value),2,4))
        #define FLL_CCR2_CLK1_SEL_GETS(value)                      (GAP_BEXTRACT((value),2,4))
        #define FLL_CCR2_CLK1_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),2,4))
        #define FLL_CCR2_CLK1_SEL(val)                             ((val) << 4)
        
        #define FLL_CCR2_CLK2_SEL_GET(value)                       (GAP_BEXTRACTU((value),2,8))
        #define FLL_CCR2_CLK2_SEL_GETS(value)                      (GAP_BEXTRACT((value),2,8))
        #define FLL_CCR2_CLK2_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),2,8))
        #define FLL_CCR2_CLK2_SEL(val)                             ((val) << 8)
        
        #define FLL_CCR2_CLK3_SEL_GET(value)                       (GAP_BEXTRACTU((value),3,12))
        #define FLL_CCR2_CLK3_SEL_GETS(value)                      (GAP_BEXTRACT((value),3,12))
        #define FLL_CCR2_CLK3_SEL_SET(value,field)                 (GAP_BINSERT((value),(field),3,12))
        #define FLL_CCR2_CLK3_SEL(val)                             ((val) << 12)
        
        #define FLL_CCR2_CKG0_GET(value)                           (GAP_BEXTRACTU((value),1,16))
        #define FLL_CCR2_CKG0_GETS(value)                          (GAP_BEXTRACT((value),1,16))
        #define FLL_CCR2_CKG0_SET(value,field)                     (GAP_BINSERT((value),(field),1,16))
        #define FLL_CCR2_CKG0(val)                                 ((val) << 16)
        
        #define FLL_CCR2_CKG1_GET(value)                           (GAP_BEXTRACTU((value),1,17))
        #define FLL_CCR2_CKG1_GETS(value)                          (GAP_BEXTRACT((value),1,17))
        #define FLL_CCR2_CKG1_SET(value,field)                     (GAP_BINSERT((value),(field),1,17))
        #define FLL_CCR2_CKG1(val)                                 ((val) << 17)
        
        #define FLL_CCR2_CKG2_GET(value)                           (GAP_BEXTRACTU((value),1,18))
        #define FLL_CCR2_CKG2_GETS(value)                          (GAP_BEXTRACT((value),1,18))
        #define FLL_CCR2_CKG2_SET(value,field)                     (GAP_BINSERT((value),(field),1,18))
        #define FLL_CCR2_CKG2(val)                                 ((val) << 18)
        
        #define FLL_CCR2_CKG3_GET(value)                           (GAP_BEXTRACTU((value),1,19))
        #define FLL_CCR2_CKG3_GETS(value)                          (GAP_BEXTRACT((value),1,19))
        #define FLL_CCR2_CKG3_SET(value,field)                     (GAP_BINSERT((value),(field),1,19))
        #define FLL_CCR2_CKG3(val)                                 ((val) << 19)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk0_sel        :1 ; // Clock source selection for OUTCLK[0]: 0: Ref clock (default). 1: FBKCLK[0] clock.
            unsigned int padding0:3 ;
            unsigned int clk1_sel        :2 ; // Clock source selection for OUTCLK[1]: 00: Ref clock. 01: FBKCLK[0] clock (default). 1x: FBKCLK[1] clock.
            unsigned int padding1:2 ;
            unsigned int clk2_sel        :2 ; // Clock source selection for OUTCLK[2]: 00: Ref clock. 01: FBKCLK[0] clock (default). 10: FBKCLK[1] clock. 11: FBKCLK[2] clock.
            unsigned int padding2:2 ;
            unsigned int clk3_sel        :3 ; // Clock source selection for OUTCLK[3]: 000: Ref clock. 001: FBKCLK[0] clock (default). 010: FBKCLK[1] clock. 011: FBKCLK[2] clock. 1xx: FBKCLK[3] clock.
            unsigned int padding3:1 ;
            unsigned int ckg0            :1 ; // FBKCLK[0] clock gated. 0: FBKCLK[0] is not gated. 1: FBKCLK[0] is clock gated by LOCK0 signal.
            unsigned int ckg1            :1 ; // FBKCLK[1] clock gated. 0: FBKCLK[1] is not gated. 1: FBKCLK[1] is clock gated by LOCK1 signal.
            unsigned int ckg2            :1 ; // FBKCLK[2] clock gated. 0: FBKCLK[2] is not gated. 1: FBKCLK[2] is clock gated by LOCK2 signal.
            unsigned int ckg3            :1 ; // FBKCLK[3] clock gated. 0: FBKCLK[3] is not gated. 1: FBKCLK[3] is clock gated by LOCK3 signal.
          };
          unsigned int raw;
        } __attribute__((packed)) fll_ccr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fll_ccr2 : public vp::reg_32
        {
        public:
            inline void clk0_sel_set(uint32_t value);
            inline uint32_t clk0_sel_get();
            inline void clk1_sel_set(uint32_t value);
            inline uint32_t clk1_sel_get();
            inline void clk2_sel_set(uint32_t value);
            inline uint32_t clk2_sel_get();
            inline void clk3_sel_set(uint32_t value);
            inline uint32_t clk3_sel_get();
            inline void ckg0_set(uint32_t value);
            inline uint32_t ckg0_get();
            inline void ckg1_set(uint32_t value);
            inline uint32_t ckg1_get();
            inline void ckg2_set(uint32_t value);
            inline uint32_t ckg2_get();
            inline void ckg3_set(uint32_t value);
            inline uint32_t ckg3_get();
        };

|
