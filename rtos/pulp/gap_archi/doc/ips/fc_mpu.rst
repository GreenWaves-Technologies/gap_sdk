Input file: docs/IP_REFERENCES/FC_MPU_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------+------+-----+-----------------------------------------------+
    |                   Name                   |Offset|Width|                  Description                  |
    +==========================================+======+=====+===============================================+
    |:ref:`MPU_ENABLE<fc_mpu_MPU_ENABLE>`      |     0|   32|MPU enable configuration register              |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE0<fc_mpu_FC_TCDM_RULE0>`|    64|   32|FC address filter rule 0                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE1<fc_mpu_FC_TCDM_RULE1>`|    68|   32|FC address filter rule 1                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE2<fc_mpu_FC_TCDM_RULE2>`|    72|   32|FC address filter rule 2                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE3<fc_mpu_FC_TCDM_RULE3>`|    76|   32|FC address filter rule 3                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE4<fc_mpu_FC_TCDM_RULE4>`|    80|   32|FC address filter rule 4                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE5<fc_mpu_FC_TCDM_RULE5>`|    84|   32|FC address filter rule 5                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE6<fc_mpu_FC_TCDM_RULE6>`|    88|   32|FC address filter rule 6                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`FC_TCDM_RULE7<fc_mpu_FC_TCDM_RULE7>`|    92|   32|FC address filter rule 7                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE0<fc_mpu_L2_RULE0>`          |   128|   32|L2 address filter rule 0                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE1<fc_mpu_L2_RULE1>`          |   132|   32|L2 address filter rule 1                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE2<fc_mpu_L2_RULE2>`          |   136|   32|L2 address filter rule 2                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE3<fc_mpu_L2_RULE3>`          |   140|   32|L2 address filter rule 3                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE4<fc_mpu_L2_RULE4>`          |   144|   32|L2 address filter rule 4                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE5<fc_mpu_L2_RULE5>`          |   148|   32|L2 address filter rule 5                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE6<fc_mpu_L2_RULE6>`          |   152|   32|L2 address filter rule 6                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`L2_RULE7<fc_mpu_L2_RULE7>`          |   156|   32|L2 address filter rule 7                       |
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE0<fc_mpu_APB_RULE0>`        |   192|   32|External peripheral (APB) address filter rule 0|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE1<fc_mpu_APB_RULE1>`        |   196|   32|External peripheral (APB) address filter rule 1|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE2<fc_mpu_APB_RULE2>`        |   200|   32|External peripheral (APB) address filter rule 2|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE3<fc_mpu_APB_RULE3>`        |   204|   32|External peripheral (APB) address filter rule 3|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE4<fc_mpu_APB_RULE4>`        |   208|   32|External peripheral (APB) address filter rule 4|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE5<fc_mpu_APB_RULE5>`        |   212|   32|External peripheral (APB) address filter rule 5|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE6<fc_mpu_APB_RULE6>`        |   216|   32|External peripheral (APB) address filter rule 6|
    +------------------------------------------+------+-----+-----------------------------------------------+
    |:ref:`APB_RULE7<fc_mpu_APB_RULE7>`        |   220|   32|External peripheral (APB) address filter rule 7|
    +------------------------------------------+------+-----+-----------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MPU enable configuration register
                #define FC_MPU_MPU_ENABLE_OFFSET                 0x0
        
                // FC address filter rule 0
                #define FC_MPU_FC_TCDM_RULE0_OFFSET              0x40
        
                // FC address filter rule 1
                #define FC_MPU_FC_TCDM_RULE1_OFFSET              0x44
        
                // FC address filter rule 2
                #define FC_MPU_FC_TCDM_RULE2_OFFSET              0x48
        
                // FC address filter rule 3
                #define FC_MPU_FC_TCDM_RULE3_OFFSET              0x4c
        
                // FC address filter rule 4
                #define FC_MPU_FC_TCDM_RULE4_OFFSET              0x50
        
                // FC address filter rule 5
                #define FC_MPU_FC_TCDM_RULE5_OFFSET              0x54
        
                // FC address filter rule 6
                #define FC_MPU_FC_TCDM_RULE6_OFFSET              0x58
        
                // FC address filter rule 7
                #define FC_MPU_FC_TCDM_RULE7_OFFSET              0x5c
        
                // L2 address filter rule 0
                #define FC_MPU_L2_RULE0_OFFSET                   0x80
        
                // L2 address filter rule 1
                #define FC_MPU_L2_RULE1_OFFSET                   0x84
        
                // L2 address filter rule 2
                #define FC_MPU_L2_RULE2_OFFSET                   0x88
        
                // L2 address filter rule 3
                #define FC_MPU_L2_RULE3_OFFSET                   0x8c
        
                // L2 address filter rule 4
                #define FC_MPU_L2_RULE4_OFFSET                   0x90
        
                // L2 address filter rule 5
                #define FC_MPU_L2_RULE5_OFFSET                   0x94
        
                // L2 address filter rule 6
                #define FC_MPU_L2_RULE6_OFFSET                   0x98
        
                // L2 address filter rule 7
                #define FC_MPU_L2_RULE7_OFFSET                   0x9c
        
                // External peripheral (APB) address filter rule 0
                #define FC_MPU_APB_RULE0_OFFSET                  0xc0
        
                // External peripheral (APB) address filter rule 1
                #define FC_MPU_APB_RULE1_OFFSET                  0xc4
        
                // External peripheral (APB) address filter rule 2
                #define FC_MPU_APB_RULE2_OFFSET                  0xc8
        
                // External peripheral (APB) address filter rule 3
                #define FC_MPU_APB_RULE3_OFFSET                  0xcc
        
                // External peripheral (APB) address filter rule 4
                #define FC_MPU_APB_RULE4_OFFSET                  0xd0
        
                // External peripheral (APB) address filter rule 5
                #define FC_MPU_APB_RULE5_OFFSET                  0xd4
        
                // External peripheral (APB) address filter rule 6
                #define FC_MPU_APB_RULE6_OFFSET                  0xd8
        
                // External peripheral (APB) address filter rule 7
                #define FC_MPU_APB_RULE7_OFFSET                  0xdc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_mpu_enable_get(uint32_t base);
        static inline void fc_mpu_mpu_enable_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule0_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule0_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule1_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule1_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule2_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule2_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule3_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule3_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule4_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule4_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule5_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule5_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule6_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule6_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_fc_tcdm_rule7_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule7_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule0_get(uint32_t base);
        static inline void fc_mpu_l2_rule0_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule1_get(uint32_t base);
        static inline void fc_mpu_l2_rule1_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule2_get(uint32_t base);
        static inline void fc_mpu_l2_rule2_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule3_get(uint32_t base);
        static inline void fc_mpu_l2_rule3_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule4_get(uint32_t base);
        static inline void fc_mpu_l2_rule4_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule5_get(uint32_t base);
        static inline void fc_mpu_l2_rule5_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule6_get(uint32_t base);
        static inline void fc_mpu_l2_rule6_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_l2_rule7_get(uint32_t base);
        static inline void fc_mpu_l2_rule7_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule0_get(uint32_t base);
        static inline void fc_mpu_apb_rule0_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule1_get(uint32_t base);
        static inline void fc_mpu_apb_rule1_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule2_get(uint32_t base);
        static inline void fc_mpu_apb_rule2_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule3_get(uint32_t base);
        static inline void fc_mpu_apb_rule3_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule4_get(uint32_t base);
        static inline void fc_mpu_apb_rule4_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule5_get(uint32_t base);
        static inline void fc_mpu_apb_rule5_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule6_get(uint32_t base);
        static inline void fc_mpu_apb_rule6_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_mpu_apb_rule7_get(uint32_t base);
        static inline void fc_mpu_apb_rule7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MPU enable status - 1'b0: MPU disabled - 1'b1: MPU enabled (access: R/W)
        #define FC_MPU_MPU_ENABLE_ENABLE_BIT                                 0
        #define FC_MPU_MPU_ENABLE_ENABLE_WIDTH                               1
        #define FC_MPU_MPU_ENABLE_ENABLE_MASK                                0x1
        #define FC_MPU_MPU_ENABLE_ENABLE_RESET                               0x0
        
        // Area (MSB indicates aliased memory area, LSB indicates L1 memory or peripheral memory) - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - L1 memory aliased (0x10000000) - 2'b11 - Peripheral memory aliased (0x10200000) The unprotected area will start at the A base + 64K * base and will be 64K * size long (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_A_BIT                                   30
        #define FC_MPU_FC_TCDM_RULE0_A_WIDTH                                 2
        #define FC_MPU_FC_TCDM_RULE0_A_MASK                                  0xc0000000
        #define FC_MPU_FC_TCDM_RULE0_A_RESET                                 0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_BASE_BIT                                15
        #define FC_MPU_FC_TCDM_RULE0_BASE_WIDTH                              14
        #define FC_MPU_FC_TCDM_RULE0_BASE_MASK                               0x1fff8000
        #define FC_MPU_FC_TCDM_RULE0_BASE_RESET                              0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_SIZE_BIT                                1
        #define FC_MPU_FC_TCDM_RULE0_SIZE_WIDTH                              13
        #define FC_MPU_FC_TCDM_RULE0_SIZE_MASK                               0x3ffe
        #define FC_MPU_FC_TCDM_RULE0_SIZE_RESET                              0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_ST_BIT                                  0
        #define FC_MPU_FC_TCDM_RULE0_ST_WIDTH                                1
        #define FC_MPU_FC_TCDM_RULE0_ST_MASK                                 0x1
        #define FC_MPU_FC_TCDM_RULE0_ST_RESET                                0x0
        
        // Area Not used. Always 0x1C000000. (access: R/W)
        #define FC_MPU_L2_RULE0_A_BIT                                        30
        #define FC_MPU_L2_RULE0_A_WIDTH                                      2
        #define FC_MPU_L2_RULE0_A_MASK                                       0xc0000000
        #define FC_MPU_L2_RULE0_A_RESET                                      0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_L2_RULE0_BASE_BIT                                     15
        #define FC_MPU_L2_RULE0_BASE_WIDTH                                   14
        #define FC_MPU_L2_RULE0_BASE_MASK                                    0x1fff8000
        #define FC_MPU_L2_RULE0_BASE_RESET                                   0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_L2_RULE0_SIZE_BIT                                     1
        #define FC_MPU_L2_RULE0_SIZE_WIDTH                                   13
        #define FC_MPU_L2_RULE0_SIZE_MASK                                    0x3ffe
        #define FC_MPU_L2_RULE0_SIZE_RESET                                   0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_L2_RULE0_ST_BIT                                       0
        #define FC_MPU_L2_RULE0_ST_WIDTH                                     1
        #define FC_MPU_L2_RULE0_ST_MASK                                      0x1
        #define FC_MPU_L2_RULE0_ST_RESET                                     0x0
        
        // Area - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - APB memory (0x1A100000) The unprotected area will start at the A base + 64K * base and will be 64K * size long (access: R/W)
        #define FC_MPU_APB_RULE0_A_BIT                                       30
        #define FC_MPU_APB_RULE0_A_WIDTH                                     2
        #define FC_MPU_APB_RULE0_A_MASK                                      0xc0000000
        #define FC_MPU_APB_RULE0_A_RESET                                     0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_APB_RULE0_BASE_BIT                                    15
        #define FC_MPU_APB_RULE0_BASE_WIDTH                                  14
        #define FC_MPU_APB_RULE0_BASE_MASK                                   0x1fff8000
        #define FC_MPU_APB_RULE0_BASE_RESET                                  0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_APB_RULE0_SIZE_BIT                                    1
        #define FC_MPU_APB_RULE0_SIZE_WIDTH                                  13
        #define FC_MPU_APB_RULE0_SIZE_MASK                                   0x3ffe
        #define FC_MPU_APB_RULE0_SIZE_RESET                                  0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_APB_RULE0_ST_BIT                                      0
        #define FC_MPU_APB_RULE0_ST_WIDTH                                    1
        #define FC_MPU_APB_RULE0_ST_MASK                                     0x1
        #define FC_MPU_APB_RULE0_ST_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_MPU_MPU_ENABLE_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE(val)                      ((val) << 0)
        
        #define FC_MPU_FC_TCDM_RULE0_A_GET(value)                  (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A_GETS(value)                 (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A_SET(value,field)            (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A(val)                        ((val) << 30)
        
        #define FC_MPU_FC_TCDM_RULE0_BASE_GET(value)               (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE_GETS(value)              (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE_SET(value,field)         (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE(val)                     ((val) << 15)
        
        #define FC_MPU_FC_TCDM_RULE0_SIZE_GET(value)               (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE_GETS(value)              (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE(val)                     ((val) << 1)
        
        #define FC_MPU_FC_TCDM_RULE0_ST_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST(val)                       ((val) << 0)
        
        #define FC_MPU_L2_RULE0_A_GET(value)                       (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_L2_RULE0_A_GETS(value)                      (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_L2_RULE0_A_SET(value,field)                 (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_L2_RULE0_A(val)                             ((val) << 30)
        
        #define FC_MPU_L2_RULE0_BASE_GET(value)                    (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_L2_RULE0_BASE_GETS(value)                   (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_L2_RULE0_BASE_SET(value,field)              (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_L2_RULE0_BASE(val)                          ((val) << 15)
        
        #define FC_MPU_L2_RULE0_SIZE_GET(value)                    (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_L2_RULE0_SIZE_GETS(value)                   (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_L2_RULE0_SIZE_SET(value,field)              (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_L2_RULE0_SIZE(val)                          ((val) << 1)
        
        #define FC_MPU_L2_RULE0_ST_GET(value)                      (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_L2_RULE0_ST_GETS(value)                     (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_L2_RULE0_ST_SET(value,field)                (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_L2_RULE0_ST(val)                            ((val) << 0)
        
        #define FC_MPU_APB_RULE0_A_GET(value)                      (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_APB_RULE0_A_GETS(value)                     (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_APB_RULE0_A_SET(value,field)                (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_APB_RULE0_A(val)                            ((val) << 30)
        
        #define FC_MPU_APB_RULE0_BASE_GET(value)                   (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_APB_RULE0_BASE_GETS(value)                  (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_APB_RULE0_BASE_SET(value,field)             (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_APB_RULE0_BASE(val)                         ((val) << 15)
        
        #define FC_MPU_APB_RULE0_SIZE_GET(value)                   (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_APB_RULE0_SIZE_GETS(value)                  (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_APB_RULE0_SIZE_SET(value,field)             (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_APB_RULE0_SIZE(val)                         ((val) << 1)
        
        #define FC_MPU_APB_RULE0_ST_GET(value)                     (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_APB_RULE0_ST_GETS(value)                    (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_APB_RULE0_ST_SET(value,field)               (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_APB_RULE0_ST(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** FC_MPU_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t mpu_enable;  // MPU enable configuration register
            volatile uint32_t reserved_0[15];  // Reserved/Not used.
            volatile uint32_t fc_tcdm_rule0;  // FC address filter rule 0
            volatile uint32_t fc_tcdm_rule1;  // FC address filter rule 1
            volatile uint32_t fc_tcdm_rule2;  // FC address filter rule 2
            volatile uint32_t fc_tcdm_rule3;  // FC address filter rule 3
            volatile uint32_t fc_tcdm_rule4;  // FC address filter rule 4
            volatile uint32_t fc_tcdm_rule5;  // FC address filter rule 5
            volatile uint32_t fc_tcdm_rule6;  // FC address filter rule 6
            volatile uint32_t fc_tcdm_rule7;  // FC address filter rule 7
            volatile uint32_t reserved_1[8];  // Reserved/Not used.
            volatile uint32_t l2_rule0;  // L2 address filter rule 0
            volatile uint32_t l2_rule1;  // L2 address filter rule 1
            volatile uint32_t l2_rule2;  // L2 address filter rule 2
            volatile uint32_t l2_rule3;  // L2 address filter rule 3
            volatile uint32_t l2_rule4;  // L2 address filter rule 4
            volatile uint32_t l2_rule5;  // L2 address filter rule 5
            volatile uint32_t l2_rule6;  // L2 address filter rule 6
            volatile uint32_t l2_rule7;  // L2 address filter rule 7
            volatile uint32_t reserved_2[8];  // Reserved/Not used.
            volatile uint32_t apb_rule0;  // External peripheral (APB) address filter rule 0
            volatile uint32_t apb_rule1;  // External peripheral (APB) address filter rule 1
            volatile uint32_t apb_rule2;  // External peripheral (APB) address filter rule 2
            volatile uint32_t apb_rule3;  // External peripheral (APB) address filter rule 3
            volatile uint32_t apb_rule4;  // External peripheral (APB) address filter rule 4
            volatile uint32_t apb_rule5;  // External peripheral (APB) address filter rule 5
            volatile uint32_t apb_rule6;  // External peripheral (APB) address filter rule 6
            volatile uint32_t apb_rule7;  // External peripheral (APB) address filter rule 7
        } __attribute__((packed)) fc_mpu_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // MPU enable status - 1'b0: MPU disabled - 1'b1: MPU enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_mpu_enable_t;
        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area (MSB indicates aliased memory area, LSB indicates L1 memory or peripheral memory) - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - L1 memory aliased (0x10000000) - 2'b11 - Peripheral memory aliased (0x10200000) The unprotected area will start at the A base + 64K * base and will be 64K * size long
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule7_t;
        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area Not used. Always 0x1C000000.
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule7_t;
        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - APB memory (0x1A100000) The unprotected area will start at the A base + 64K * base and will be 64K * size long
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_fc_mpu : public vp::regmap
        {
        public:
            vp_fc_mpu_mpu_enable mpu_enable;
            vp_fc_mpu_fc_tcdm_rule0 fc_tcdm_rule0;
            vp_fc_mpu_fc_tcdm_rule1 fc_tcdm_rule1;
            vp_fc_mpu_fc_tcdm_rule2 fc_tcdm_rule2;
            vp_fc_mpu_fc_tcdm_rule3 fc_tcdm_rule3;
            vp_fc_mpu_fc_tcdm_rule4 fc_tcdm_rule4;
            vp_fc_mpu_fc_tcdm_rule5 fc_tcdm_rule5;
            vp_fc_mpu_fc_tcdm_rule6 fc_tcdm_rule6;
            vp_fc_mpu_fc_tcdm_rule7 fc_tcdm_rule7;
            vp_fc_mpu_l2_rule0 l2_rule0;
            vp_fc_mpu_l2_rule1 l2_rule1;
            vp_fc_mpu_l2_rule2 l2_rule2;
            vp_fc_mpu_l2_rule3 l2_rule3;
            vp_fc_mpu_l2_rule4 l2_rule4;
            vp_fc_mpu_l2_rule5 l2_rule5;
            vp_fc_mpu_l2_rule6 l2_rule6;
            vp_fc_mpu_l2_rule7 l2_rule7;
            vp_fc_mpu_apb_rule0 apb_rule0;
            vp_fc_mpu_apb_rule1 apb_rule1;
            vp_fc_mpu_apb_rule2 apb_rule2;
            vp_fc_mpu_apb_rule3 apb_rule3;
            vp_fc_mpu_apb_rule4 apb_rule4;
            vp_fc_mpu_apb_rule5 apb_rule5;
            vp_fc_mpu_apb_rule6 apb_rule6;
            vp_fc_mpu_apb_rule7 apb_rule7;
        };

|

.. _fc_mpu_MPU_ENABLE:

MPU_ENABLE
""""""""""

MPU enable configuration register

.. table:: 

    +-----+---+------+----------------------------------------------------------+
    |Bit #|R/W| Name |                       Description                        |
    +=====+===+======+==========================================================+
    |    0|R/W|ENABLE|MPU enable status - 1'b0: MPU disabled - 1'b1: MPU enabled|
    +-----+---+------+----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MPU enable configuration register
                #define FC_MPU_MPU_ENABLE_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_mpu_enable_get(uint32_t base);
        static inline void fc_mpu_mpu_enable_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MPU enable status - 1'b0: MPU disabled - 1'b1: MPU enabled (access: R/W)
        #define FC_MPU_MPU_ENABLE_ENABLE_BIT                                 0
        #define FC_MPU_MPU_ENABLE_ENABLE_WIDTH                               1
        #define FC_MPU_MPU_ENABLE_ENABLE_MASK                                0x1
        #define FC_MPU_MPU_ENABLE_ENABLE_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_MPU_MPU_ENABLE_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_MPU_ENABLE_ENABLE(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // MPU enable status - 1'b0: MPU disabled - 1'b1: MPU enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_mpu_enable_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_mpu_enable : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|

.. _fc_mpu_FC_TCDM_RULE0:

FC_TCDM_RULE0
"""""""""""""

FC address filter rule 0

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                   Description                                                                                                                                                                   |
    +=====+===+====+=================================================================================================================================================================================================================================================================================================================================================+
    |31:30|R/W|A   |Area (MSB indicates aliased memory area, LSB indicates L1 memory or peripheral memory) - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - L1 memory aliased (0x10000000) - 2'b11 - Peripheral memory aliased (0x10200000) The unprotected area will start at the A base + 64K * base and will be 64K * size long|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |28:15|R/W|BASE|Area address base in 64kB pages                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13:1 |R/W|SIZE|Area size in 64KB pages                                                                                                                                                                                                                                                                                                                          |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |0    |R/W|ST  |Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled                                                                                                                                                                                                                                                                                             |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 0
                #define FC_MPU_FC_TCDM_RULE0_OFFSET              0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule0_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Area (MSB indicates aliased memory area, LSB indicates L1 memory or peripheral memory) - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - L1 memory aliased (0x10000000) - 2'b11 - Peripheral memory aliased (0x10200000) The unprotected area will start at the A base + 64K * base and will be 64K * size long (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_A_BIT                                   30
        #define FC_MPU_FC_TCDM_RULE0_A_WIDTH                                 2
        #define FC_MPU_FC_TCDM_RULE0_A_MASK                                  0xc0000000
        #define FC_MPU_FC_TCDM_RULE0_A_RESET                                 0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_BASE_BIT                                15
        #define FC_MPU_FC_TCDM_RULE0_BASE_WIDTH                              14
        #define FC_MPU_FC_TCDM_RULE0_BASE_MASK                               0x1fff8000
        #define FC_MPU_FC_TCDM_RULE0_BASE_RESET                              0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_SIZE_BIT                                1
        #define FC_MPU_FC_TCDM_RULE0_SIZE_WIDTH                              13
        #define FC_MPU_FC_TCDM_RULE0_SIZE_MASK                               0x3ffe
        #define FC_MPU_FC_TCDM_RULE0_SIZE_RESET                              0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_FC_TCDM_RULE0_ST_BIT                                  0
        #define FC_MPU_FC_TCDM_RULE0_ST_WIDTH                                1
        #define FC_MPU_FC_TCDM_RULE0_ST_MASK                                 0x1
        #define FC_MPU_FC_TCDM_RULE0_ST_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_MPU_FC_TCDM_RULE0_A_GET(value)                  (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A_GETS(value)                 (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A_SET(value,field)            (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_FC_TCDM_RULE0_A(val)                        ((val) << 30)
        
        #define FC_MPU_FC_TCDM_RULE0_BASE_GET(value)               (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE_GETS(value)              (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE_SET(value,field)         (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_FC_TCDM_RULE0_BASE(val)                     ((val) << 15)
        
        #define FC_MPU_FC_TCDM_RULE0_SIZE_GET(value)               (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE_GETS(value)              (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_FC_TCDM_RULE0_SIZE(val)                     ((val) << 1)
        
        #define FC_MPU_FC_TCDM_RULE0_ST_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_FC_TCDM_RULE0_ST(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area (MSB indicates aliased memory area, LSB indicates L1 memory or peripheral memory) - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - L1 memory aliased (0x10000000) - 2'b11 - Peripheral memory aliased (0x10200000) The unprotected area will start at the A base + 64K * base and will be 64K * size long
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule0 : public vp::reg_32
        {
        public:
            inline void a_set(uint32_t value);
            inline uint32_t a_get();
            inline void base_set(uint32_t value);
            inline uint32_t base_get();
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void st_set(uint32_t value);
            inline uint32_t st_get();
        };

|

.. _fc_mpu_FC_TCDM_RULE1:

FC_TCDM_RULE1
"""""""""""""

FC address filter rule 1

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 1
                #define FC_MPU_FC_TCDM_RULE1_OFFSET              0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule1_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule1 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE2:

FC_TCDM_RULE2
"""""""""""""

FC address filter rule 2

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 2
                #define FC_MPU_FC_TCDM_RULE2_OFFSET              0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule2_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule2 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE3:

FC_TCDM_RULE3
"""""""""""""

FC address filter rule 3

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 3
                #define FC_MPU_FC_TCDM_RULE3_OFFSET              0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule3_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule3 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE4:

FC_TCDM_RULE4
"""""""""""""

FC address filter rule 4

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 4
                #define FC_MPU_FC_TCDM_RULE4_OFFSET              0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule4_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule4 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE5:

FC_TCDM_RULE5
"""""""""""""

FC address filter rule 5

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 5
                #define FC_MPU_FC_TCDM_RULE5_OFFSET              0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule5_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule5 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE6:

FC_TCDM_RULE6
"""""""""""""

FC address filter rule 6

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 6
                #define FC_MPU_FC_TCDM_RULE6_OFFSET              0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule6_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule6 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_FC_TCDM_RULE7:

FC_TCDM_RULE7
"""""""""""""

FC address filter rule 7

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC address filter rule 7
                #define FC_MPU_FC_TCDM_RULE7_OFFSET              0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_fc_tcdm_rule7_get(uint32_t base);
        static inline void fc_mpu_fc_tcdm_rule7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_fc_tcdm_rule7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_fc_tcdm_rule7 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE0:

L2_RULE0
""""""""

L2 address filter rule 0

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |31:30|R/W|A   |Area Not used. Always 0x1C000000.                   |
    +-----+---+----+----------------------------------------------------+
    |28:15|R/W|BASE|Area address base in 64kB pages                     |
    +-----+---+----+----------------------------------------------------+
    |13:1 |R/W|SIZE|Area size in 64KB pages                             |
    +-----+---+----+----------------------------------------------------+
    |0    |R/W|ST  |Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled|
    +-----+---+----+----------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 0
                #define FC_MPU_L2_RULE0_OFFSET                   0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule0_get(uint32_t base);
        static inline void fc_mpu_l2_rule0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Area Not used. Always 0x1C000000. (access: R/W)
        #define FC_MPU_L2_RULE0_A_BIT                                        30
        #define FC_MPU_L2_RULE0_A_WIDTH                                      2
        #define FC_MPU_L2_RULE0_A_MASK                                       0xc0000000
        #define FC_MPU_L2_RULE0_A_RESET                                      0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_L2_RULE0_BASE_BIT                                     15
        #define FC_MPU_L2_RULE0_BASE_WIDTH                                   14
        #define FC_MPU_L2_RULE0_BASE_MASK                                    0x1fff8000
        #define FC_MPU_L2_RULE0_BASE_RESET                                   0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_L2_RULE0_SIZE_BIT                                     1
        #define FC_MPU_L2_RULE0_SIZE_WIDTH                                   13
        #define FC_MPU_L2_RULE0_SIZE_MASK                                    0x3ffe
        #define FC_MPU_L2_RULE0_SIZE_RESET                                   0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_L2_RULE0_ST_BIT                                       0
        #define FC_MPU_L2_RULE0_ST_WIDTH                                     1
        #define FC_MPU_L2_RULE0_ST_MASK                                      0x1
        #define FC_MPU_L2_RULE0_ST_RESET                                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_MPU_L2_RULE0_A_GET(value)                       (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_L2_RULE0_A_GETS(value)                      (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_L2_RULE0_A_SET(value,field)                 (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_L2_RULE0_A(val)                             ((val) << 30)
        
        #define FC_MPU_L2_RULE0_BASE_GET(value)                    (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_L2_RULE0_BASE_GETS(value)                   (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_L2_RULE0_BASE_SET(value,field)              (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_L2_RULE0_BASE(val)                          ((val) << 15)
        
        #define FC_MPU_L2_RULE0_SIZE_GET(value)                    (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_L2_RULE0_SIZE_GETS(value)                   (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_L2_RULE0_SIZE_SET(value,field)              (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_L2_RULE0_SIZE(val)                          ((val) << 1)
        
        #define FC_MPU_L2_RULE0_ST_GET(value)                      (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_L2_RULE0_ST_GETS(value)                     (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_L2_RULE0_ST_SET(value,field)                (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_L2_RULE0_ST(val)                            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area Not used. Always 0x1C000000.
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule0 : public vp::reg_32
        {
        public:
            inline void a_set(uint32_t value);
            inline uint32_t a_get();
            inline void base_set(uint32_t value);
            inline uint32_t base_get();
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void st_set(uint32_t value);
            inline uint32_t st_get();
        };

|

.. _fc_mpu_L2_RULE1:

L2_RULE1
""""""""

L2 address filter rule 1

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 1
                #define FC_MPU_L2_RULE1_OFFSET                   0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule1_get(uint32_t base);
        static inline void fc_mpu_l2_rule1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule1 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE2:

L2_RULE2
""""""""

L2 address filter rule 2

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 2
                #define FC_MPU_L2_RULE2_OFFSET                   0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule2_get(uint32_t base);
        static inline void fc_mpu_l2_rule2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule2 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE3:

L2_RULE3
""""""""

L2 address filter rule 3

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 3
                #define FC_MPU_L2_RULE3_OFFSET                   0x8c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule3_get(uint32_t base);
        static inline void fc_mpu_l2_rule3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule3 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE4:

L2_RULE4
""""""""

L2 address filter rule 4

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 4
                #define FC_MPU_L2_RULE4_OFFSET                   0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule4_get(uint32_t base);
        static inline void fc_mpu_l2_rule4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule4 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE5:

L2_RULE5
""""""""

L2 address filter rule 5

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 5
                #define FC_MPU_L2_RULE5_OFFSET                   0x94

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule5_get(uint32_t base);
        static inline void fc_mpu_l2_rule5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule5 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE6:

L2_RULE6
""""""""

L2 address filter rule 6

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 6
                #define FC_MPU_L2_RULE6_OFFSET                   0x98

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule6_get(uint32_t base);
        static inline void fc_mpu_l2_rule6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule6 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_L2_RULE7:

L2_RULE7
""""""""

L2 address filter rule 7

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // L2 address filter rule 7
                #define FC_MPU_L2_RULE7_OFFSET                   0x9c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_l2_rule7_get(uint32_t base);
        static inline void fc_mpu_l2_rule7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_l2_rule7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_l2_rule7 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE0:

APB_RULE0
"""""""""

External peripheral (APB) address filter rule 0

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                              Description                                                                                              |
    +=====+===+====+=======================================================================================================================================================================================================+
    |31:30|R/W|A   |Area - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - APB memory (0x1A100000) The unprotected area will start at the A base + 64K * base and will be 64K * size long|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |28:15|R/W|BASE|Area address base in 64kB pages                                                                                                                                                                        |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13:1 |R/W|SIZE|Area size in 64KB pages                                                                                                                                                                                |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |0    |R/W|ST  |Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled                                                                                                                                                   |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 0
                #define FC_MPU_APB_RULE0_OFFSET                  0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule0_get(uint32_t base);
        static inline void fc_mpu_apb_rule0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Area - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - APB memory (0x1A100000) The unprotected area will start at the A base + 64K * base and will be 64K * size long (access: R/W)
        #define FC_MPU_APB_RULE0_A_BIT                                       30
        #define FC_MPU_APB_RULE0_A_WIDTH                                     2
        #define FC_MPU_APB_RULE0_A_MASK                                      0xc0000000
        #define FC_MPU_APB_RULE0_A_RESET                                     0x0
        
        // Area address base in 64kB pages (access: R/W)
        #define FC_MPU_APB_RULE0_BASE_BIT                                    15
        #define FC_MPU_APB_RULE0_BASE_WIDTH                                  14
        #define FC_MPU_APB_RULE0_BASE_MASK                                   0x1fff8000
        #define FC_MPU_APB_RULE0_BASE_RESET                                  0x0
        
        // Area size in 64KB pages (access: R/W)
        #define FC_MPU_APB_RULE0_SIZE_BIT                                    1
        #define FC_MPU_APB_RULE0_SIZE_WIDTH                                  13
        #define FC_MPU_APB_RULE0_SIZE_MASK                                   0x3ffe
        #define FC_MPU_APB_RULE0_SIZE_RESET                                  0x0
        
        // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled (access: R/W)
        #define FC_MPU_APB_RULE0_ST_BIT                                      0
        #define FC_MPU_APB_RULE0_ST_WIDTH                                    1
        #define FC_MPU_APB_RULE0_ST_MASK                                     0x1
        #define FC_MPU_APB_RULE0_ST_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_MPU_APB_RULE0_A_GET(value)                      (GAP_BEXTRACTU((value),2,30))
        #define FC_MPU_APB_RULE0_A_GETS(value)                     (GAP_BEXTRACT((value),2,30))
        #define FC_MPU_APB_RULE0_A_SET(value,field)                (GAP_BINSERT((value),(field),2,30))
        #define FC_MPU_APB_RULE0_A(val)                            ((val) << 30)
        
        #define FC_MPU_APB_RULE0_BASE_GET(value)                   (GAP_BEXTRACTU((value),14,15))
        #define FC_MPU_APB_RULE0_BASE_GETS(value)                  (GAP_BEXTRACT((value),14,15))
        #define FC_MPU_APB_RULE0_BASE_SET(value,field)             (GAP_BINSERT((value),(field),14,15))
        #define FC_MPU_APB_RULE0_BASE(val)                         ((val) << 15)
        
        #define FC_MPU_APB_RULE0_SIZE_GET(value)                   (GAP_BEXTRACTU((value),13,1))
        #define FC_MPU_APB_RULE0_SIZE_GETS(value)                  (GAP_BEXTRACT((value),13,1))
        #define FC_MPU_APB_RULE0_SIZE_SET(value,field)             (GAP_BINSERT((value),(field),13,1))
        #define FC_MPU_APB_RULE0_SIZE(val)                         ((val) << 1)
        
        #define FC_MPU_APB_RULE0_ST_GET(value)                     (GAP_BEXTRACTU((value),1,0))
        #define FC_MPU_APB_RULE0_ST_GETS(value)                    (GAP_BEXTRACT((value),1,0))
        #define FC_MPU_APB_RULE0_ST_SET(value,field)               (GAP_BINSERT((value),(field),1,0))
        #define FC_MPU_APB_RULE0_ST(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:30;
            unsigned int a               :2 ; // Area - 2'b00 - L1 memory (0x01B00000) - 2'b01 - Peripheral memory (0x1B200000) - 2'b10 - APB memory (0x1A100000) The unprotected area will start at the A base + 64K * base and will be 64K * size long
            unsigned int base            :14; // Area address base in 64kB pages
            unsigned int size            :13; // Area size in 64KB pages
            unsigned int st              :1 ; // Rule state - 1'b0: Rule diabled - 1'b1: Rule enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule0 : public vp::reg_32
        {
        public:
            inline void a_set(uint32_t value);
            inline uint32_t a_get();
            inline void base_set(uint32_t value);
            inline uint32_t base_get();
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void st_set(uint32_t value);
            inline uint32_t st_get();
        };

|

.. _fc_mpu_APB_RULE1:

APB_RULE1
"""""""""

External peripheral (APB) address filter rule 1

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 1
                #define FC_MPU_APB_RULE1_OFFSET                  0xc4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule1_get(uint32_t base);
        static inline void fc_mpu_apb_rule1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule1 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE2:

APB_RULE2
"""""""""

External peripheral (APB) address filter rule 2

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 2
                #define FC_MPU_APB_RULE2_OFFSET                  0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule2_get(uint32_t base);
        static inline void fc_mpu_apb_rule2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule2 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE3:

APB_RULE3
"""""""""

External peripheral (APB) address filter rule 3

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 3
                #define FC_MPU_APB_RULE3_OFFSET                  0xcc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule3_get(uint32_t base);
        static inline void fc_mpu_apb_rule3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule3 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE4:

APB_RULE4
"""""""""

External peripheral (APB) address filter rule 4

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 4
                #define FC_MPU_APB_RULE4_OFFSET                  0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule4_get(uint32_t base);
        static inline void fc_mpu_apb_rule4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule4 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE5:

APB_RULE5
"""""""""

External peripheral (APB) address filter rule 5

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 5
                #define FC_MPU_APB_RULE5_OFFSET                  0xd4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule5_get(uint32_t base);
        static inline void fc_mpu_apb_rule5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule5 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE6:

APB_RULE6
"""""""""

External peripheral (APB) address filter rule 6

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 6
                #define FC_MPU_APB_RULE6_OFFSET                  0xd8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule6_get(uint32_t base);
        static inline void fc_mpu_apb_rule6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule6 : public vp::reg_32
        {
        public:
        };

|

.. _fc_mpu_APB_RULE7:

APB_RULE7
"""""""""

External peripheral (APB) address filter rule 7

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // External peripheral (APB) address filter rule 7
                #define FC_MPU_APB_RULE7_OFFSET                  0xdc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_mpu_apb_rule7_get(uint32_t base);
        static inline void fc_mpu_apb_rule7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_mpu_apb_rule7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_mpu_apb_rule7 : public vp::reg_32
        {
        public:
        };

|
