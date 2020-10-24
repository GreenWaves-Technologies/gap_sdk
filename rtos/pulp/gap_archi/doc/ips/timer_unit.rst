Input file: fe/ips/timer_unit/doc/TIMER_UNIT_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------+------+-----+-------------------------------------+
    |                Name                |Offset|Width|             Description             |
    +====================================+======+=====+=====================================+
    |:ref:`CFG_LO<timer_unit_CFG_LO>`    |     0|   32|Timer Low Configuration register.    |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`CFG_HI<timer_unit_CFG_HI>`    |     4|   32|Timer High Configuration register.   |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`CNT_LO<timer_unit_CNT_LO>`    |     8|   32|Timer Low counter value register.    |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`CNT_HI<timer_unit_CNT_HI>`    |    12|   32|Timer High counter value register.   |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`CMP_LO<timer_unit_CMP_LO>`    |    16|   32|Timer Low comparator value register. |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`CMP_HI<timer_unit_CMP_HI>`    |    20|   32|Timer High comparator value register.|
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`START_LO<timer_unit_START_LO>`|    24|   32|Start Timer Low counting register.   |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`START_HI<timer_unit_START_HI>`|    28|   32|Start Timer High counting register.  |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`RESET_LO<timer_unit_RESET_LO>`|    32|   32|Reset Timer Low counter register.    |
    +------------------------------------+------+-----+-------------------------------------+
    |:ref:`RESET_HI<timer_unit_RESET_HI>`|    36|   32|Reset Timer High counter register.   |
    +------------------------------------+------+-----+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer Low Configuration register.
                #define TIMER_UNIT_CFG_LO_OFFSET                 0x0
        
                // Timer High Configuration register.
                #define TIMER_UNIT_CFG_HI_OFFSET                 0x4
        
                // Timer Low counter value register.
                #define TIMER_UNIT_CNT_LO_OFFSET                 0x8
        
                // Timer High counter value register.
                #define TIMER_UNIT_CNT_HI_OFFSET                 0xc
        
                // Timer Low comparator value register.
                #define TIMER_UNIT_CMP_LO_OFFSET                 0x10
        
                // Timer High comparator value register.
                #define TIMER_UNIT_CMP_HI_OFFSET                 0x14
        
                // Start Timer Low counting register.
                #define TIMER_UNIT_START_LO_OFFSET               0x18
        
                // Start Timer High counting register.
                #define TIMER_UNIT_START_HI_OFFSET               0x1c
        
                // Reset Timer Low counter register.
                #define TIMER_UNIT_RESET_LO_OFFSET               0x20
        
                // Reset Timer High counter register.
                #define TIMER_UNIT_RESET_HI_OFFSET               0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cfg_lo_get(uint32_t base);
        static inline void timer_unit_cfg_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_cfg_hi_get(uint32_t base);
        static inline void timer_unit_cfg_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_cnt_lo_get(uint32_t base);
        static inline void timer_unit_cnt_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_cnt_hi_get(uint32_t base);
        static inline void timer_unit_cnt_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_cmp_lo_get(uint32_t base);
        static inline void timer_unit_cmp_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_cmp_hi_get(uint32_t base);
        static inline void timer_unit_cmp_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_start_lo_get(uint32_t base);
        static inline void timer_unit_start_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_start_hi_get(uint32_t base);
        static inline void timer_unit_start_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_reset_lo_get(uint32_t base);
        static inline void timer_unit_reset_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t timer_unit_reset_hi_get(uint32_t base);
        static inline void timer_unit_reset_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer low enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_ENABLE_BIT                                 0
        #define TIMER_UNIT_CFG_LO_ENABLE_WIDTH                               1
        #define TIMER_UNIT_CFG_LO_ENABLE_MASK                                0x1
        #define TIMER_UNIT_CFG_LO_ENABLE_RESET                               0x0
        
        // Timer low counter reset command bitfield. Cleared after Timer Low reset execution. (access: R/W)
        #define TIMER_UNIT_CFG_LO_RESET_BIT                                  1
        #define TIMER_UNIT_CFG_LO_RESET_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_RESET_MASK                                 0x2
        #define TIMER_UNIT_CFG_LO_RESET_RESET                                0x0
        
        // Timer low compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_IRQEN_BIT                                  2
        #define TIMER_UNIT_CFG_LO_IRQEN_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_IRQEN_MASK                                 0x4
        #define TIMER_UNIT_CFG_LO_IRQEN_RESET                                0x0
        
        // Timer low input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_IEM_BIT                                    3
        #define TIMER_UNIT_CFG_LO_IEM_WIDTH                                  1
        #define TIMER_UNIT_CFG_LO_IEM_MASK                                   0x8
        #define TIMER_UNIT_CFG_LO_IEM_RESET                                  0x0
        
        // Timer low continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer low counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer low counter when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_LO_MODE_BIT                                   4
        #define TIMER_UNIT_CFG_LO_MODE_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_MODE_MASK                                  0x10
        #define TIMER_UNIT_CFG_LO_MODE_RESET                                 0x0
        
        // Timer low one shot configuration bitfield: - 1'b0: let Timer low enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer low when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_LO_ONE_S_BIT                                  5
        #define TIMER_UNIT_CFG_LO_ONE_S_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_ONE_S_MASK                                 0x20
        #define TIMER_UNIT_CFG_LO_ONE_S_RESET                                0x0
        
        // Timer low prescaler enable configuration bitfield:- 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_PEN_BIT                                    6
        #define TIMER_UNIT_CFG_LO_PEN_WIDTH                                  1
        #define TIMER_UNIT_CFG_LO_PEN_MASK                                   0x40
        #define TIMER_UNIT_CFG_LO_PEN_RESET                                  0x0
        
        // Timer low clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz (access: R/W)
        #define TIMER_UNIT_CFG_LO_CCFG_BIT                                   7
        #define TIMER_UNIT_CFG_LO_CCFG_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_CCFG_MASK                                  0x80
        #define TIMER_UNIT_CFG_LO_CCFG_RESET                                 0x0
        
        // Timer low prescaler value bitfield. Ftimer = Fclk / (1 + PRESC_VAL) (access: R/W)
        #define TIMER_UNIT_CFG_LO_PVAL_BIT                                   8
        #define TIMER_UNIT_CFG_LO_PVAL_WIDTH                                 8
        #define TIMER_UNIT_CFG_LO_PVAL_MASK                                  0xff00
        #define TIMER_UNIT_CFG_LO_PVAL_RESET                                 0x0
        
        // Timer low  + Timer high 64bit cascaded mode configuration bitfield. (access: R/W)
        #define TIMER_UNIT_CFG_LO_CASC_BIT                                   31
        #define TIMER_UNIT_CFG_LO_CASC_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_CASC_MASK                                  0x80000000
        #define TIMER_UNIT_CFG_LO_CASC_RESET                                 0x0
        
        // Timer high enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_ENABLE_BIT                                 0
        #define TIMER_UNIT_CFG_HI_ENABLE_WIDTH                               1
        #define TIMER_UNIT_CFG_HI_ENABLE_MASK                                0x1
        #define TIMER_UNIT_CFG_HI_ENABLE_RESET                               0x0
        
        // Timer high counter reset command bitfield. Cleared after Timer high reset execution. (access: W)
        #define TIMER_UNIT_CFG_HI_RESET_BIT                                  1
        #define TIMER_UNIT_CFG_HI_RESET_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_RESET_MASK                                 0x2
        #define TIMER_UNIT_CFG_HI_RESET_RESET                                0x0
        
        // Timer high compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_IRQEN_BIT                                  2
        #define TIMER_UNIT_CFG_HI_IRQEN_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_IRQEN_MASK                                 0x4
        #define TIMER_UNIT_CFG_HI_IRQEN_RESET                                0x0
        
        // Timer high input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_IEM_BIT                                    3
        #define TIMER_UNIT_CFG_HI_IEM_WIDTH                                  1
        #define TIMER_UNIT_CFG_HI_IEM_MASK                                   0x8
        #define TIMER_UNIT_CFG_HI_IEM_RESET                                  0x0
        
        // Timer high continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer high counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer high counter when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_HI_MODE_BIT                                   4
        #define TIMER_UNIT_CFG_HI_MODE_WIDTH                                 1
        #define TIMER_UNIT_CFG_HI_MODE_MASK                                  0x10
        #define TIMER_UNIT_CFG_HI_MODE_RESET                                 0x0
        
        // Timer high one shot configuration bitfield: - 1'b0: let Timer high enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer high when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_HI_ONE_S_BIT                                  5
        #define TIMER_UNIT_CFG_HI_ONE_S_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_ONE_S_MASK                                 0x20
        #define TIMER_UNIT_CFG_HI_ONE_S_RESET                                0x0
        
        // Timer high prescaler enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_PEN_BIT                                    6
        #define TIMER_UNIT_CFG_HI_PEN_WIDTH                                  1
        #define TIMER_UNIT_CFG_HI_PEN_MASK                                   0x40
        #define TIMER_UNIT_CFG_HI_PEN_RESET                                  0x0
        
        // Timer high clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz (access: R/W)
        #define TIMER_UNIT_CFG_HI_CLKCFG_BIT                                 7
        #define TIMER_UNIT_CFG_HI_CLKCFG_WIDTH                               1
        #define TIMER_UNIT_CFG_HI_CLKCFG_MASK                                0x80
        #define TIMER_UNIT_CFG_HI_CLKCFG_RESET                               0x0
        
        // Timer Low counter value bitfield. (access: R/W)
        #define TIMER_UNIT_CNT_LO_CNT_LO_BIT                                 0
        #define TIMER_UNIT_CNT_LO_CNT_LO_WIDTH                               32
        #define TIMER_UNIT_CNT_LO_CNT_LO_MASK                                0xffffffff
        #define TIMER_UNIT_CNT_LO_CNT_LO_RESET                               0x0
        
        // Timer High counter value bitfield. (access: R/W)
        #define TIMER_UNIT_CNT_HI_CNT_HI_BIT                                 0
        #define TIMER_UNIT_CNT_HI_CNT_HI_WIDTH                               32
        #define TIMER_UNIT_CNT_HI_CNT_HI_MASK                                0xffffffff
        #define TIMER_UNIT_CNT_HI_CNT_HI_RESET                               0x0
        
        // Timer Low comparator value bitfield. (access: R/W)
        #define TIMER_UNIT_CMP_LO_CMP_LO_BIT                                 0
        #define TIMER_UNIT_CMP_LO_CMP_LO_WIDTH                               32
        #define TIMER_UNIT_CMP_LO_CMP_LO_MASK                                0xffffffff
        #define TIMER_UNIT_CMP_LO_CMP_LO_RESET                               0x0
        
        // Timer High comparator value bitfield. (access: R/W)
        #define TIMER_UNIT_CMP_HI_CMP_HI_BIT                                 0
        #define TIMER_UNIT_CMP_HI_CMP_HI_WIDTH                               32
        #define TIMER_UNIT_CMP_HI_CMP_HI_MASK                                0xffffffff
        #define TIMER_UNIT_CMP_HI_CMP_HI_RESET                               0x0
        
        // Timer Low start command bitfield. When executed, CFG_LO.ENABLE is set. (access: W)
        #define TIMER_UNIT_START_LO_STRT_LO_BIT                              0
        #define TIMER_UNIT_START_LO_STRT_LO_WIDTH                            1
        #define TIMER_UNIT_START_LO_STRT_LO_MASK                             0x1
        #define TIMER_UNIT_START_LO_STRT_LO_RESET                            0x0
        
        // Timer High start command bitfield. When executed, CFG_HI.ENABLE is set. (access: W)
        #define TIMER_UNIT_START_HI_STRT_HI_BIT                              0
        #define TIMER_UNIT_START_HI_STRT_HI_WIDTH                            1
        #define TIMER_UNIT_START_HI_STRT_HI_MASK                             0x1
        #define TIMER_UNIT_START_HI_STRT_HI_RESET                            0x0
        
        // Timer Low counter reset command bitfield. When executed, CFG_LO.RESET is set. (access: W)
        #define TIMER_UNIT_RESET_LO_RST_LO_BIT                               0
        #define TIMER_UNIT_RESET_LO_RST_LO_WIDTH                             1
        #define TIMER_UNIT_RESET_LO_RST_LO_MASK                              0x1
        #define TIMER_UNIT_RESET_LO_RST_LO_RESET                             0x0
        
        // Timer High counter reset command bitfield. When executed, CFG_HI.RESET is set. (access: W)
        #define TIMER_UNIT_RESET_HI_RST_HI_BIT                               0
        #define TIMER_UNIT_RESET_HI_RST_HI_WIDTH                             1
        #define TIMER_UNIT_RESET_HI_RST_HI_MASK                              0x1
        #define TIMER_UNIT_RESET_HI_RST_HI_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CFG_LO_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CFG_LO_RESET_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define TIMER_UNIT_CFG_LO_RESET_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define TIMER_UNIT_CFG_LO_RESET_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define TIMER_UNIT_CFG_LO_RESET(val)                       ((val) << 1)
        
        #define TIMER_UNIT_CFG_LO_IRQEN_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN(val)                       ((val) << 2)
        
        #define TIMER_UNIT_CFG_LO_IEM_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define TIMER_UNIT_CFG_LO_IEM_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define TIMER_UNIT_CFG_LO_IEM_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define TIMER_UNIT_CFG_LO_IEM(val)                         ((val) << 3)
        
        #define TIMER_UNIT_CFG_LO_MODE_GET(value)                  (GAP_BEXTRACTU((value),1,4))
        #define TIMER_UNIT_CFG_LO_MODE_GETS(value)                 (GAP_BEXTRACT((value),1,4))
        #define TIMER_UNIT_CFG_LO_MODE_SET(value,field)            (GAP_BINSERT((value),(field),1,4))
        #define TIMER_UNIT_CFG_LO_MODE(val)                        ((val) << 4)
        
        #define TIMER_UNIT_CFG_LO_ONE_S_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S(val)                       ((val) << 5)
        
        #define TIMER_UNIT_CFG_LO_PEN_GET(value)                   (GAP_BEXTRACTU((value),1,6))
        #define TIMER_UNIT_CFG_LO_PEN_GETS(value)                  (GAP_BEXTRACT((value),1,6))
        #define TIMER_UNIT_CFG_LO_PEN_SET(value,field)             (GAP_BINSERT((value),(field),1,6))
        #define TIMER_UNIT_CFG_LO_PEN(val)                         ((val) << 6)
        
        #define TIMER_UNIT_CFG_LO_CCFG_GET(value)                  (GAP_BEXTRACTU((value),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG_GETS(value)                 (GAP_BEXTRACT((value),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG_SET(value,field)            (GAP_BINSERT((value),(field),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG(val)                        ((val) << 7)
        
        #define TIMER_UNIT_CFG_LO_PVAL_GET(value)                  (GAP_BEXTRACTU((value),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL_GETS(value)                 (GAP_BEXTRACT((value),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL_SET(value,field)            (GAP_BINSERT((value),(field),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL(val)                        ((val) << 8)
        
        #define TIMER_UNIT_CFG_LO_CASC_GET(value)                  (GAP_BEXTRACTU((value),1,31))
        #define TIMER_UNIT_CFG_LO_CASC_GETS(value)                 (GAP_BEXTRACT((value),1,31))
        #define TIMER_UNIT_CFG_LO_CASC_SET(value,field)            (GAP_BINSERT((value),(field),1,31))
        #define TIMER_UNIT_CFG_LO_CASC(val)                        ((val) << 31)
        
        #define TIMER_UNIT_CFG_HI_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CFG_HI_RESET_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define TIMER_UNIT_CFG_HI_RESET_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define TIMER_UNIT_CFG_HI_RESET_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define TIMER_UNIT_CFG_HI_RESET(val)                       ((val) << 1)
        
        #define TIMER_UNIT_CFG_HI_IRQEN_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN(val)                       ((val) << 2)
        
        #define TIMER_UNIT_CFG_HI_IEM_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define TIMER_UNIT_CFG_HI_IEM_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define TIMER_UNIT_CFG_HI_IEM_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define TIMER_UNIT_CFG_HI_IEM(val)                         ((val) << 3)
        
        #define TIMER_UNIT_CFG_HI_MODE_GET(value)                  (GAP_BEXTRACTU((value),1,4))
        #define TIMER_UNIT_CFG_HI_MODE_GETS(value)                 (GAP_BEXTRACT((value),1,4))
        #define TIMER_UNIT_CFG_HI_MODE_SET(value,field)            (GAP_BINSERT((value),(field),1,4))
        #define TIMER_UNIT_CFG_HI_MODE(val)                        ((val) << 4)
        
        #define TIMER_UNIT_CFG_HI_ONE_S_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S(val)                       ((val) << 5)
        
        #define TIMER_UNIT_CFG_HI_PEN_GET(value)                   (GAP_BEXTRACTU((value),1,6))
        #define TIMER_UNIT_CFG_HI_PEN_GETS(value)                  (GAP_BEXTRACT((value),1,6))
        #define TIMER_UNIT_CFG_HI_PEN_SET(value,field)             (GAP_BINSERT((value),(field),1,6))
        #define TIMER_UNIT_CFG_HI_PEN(val)                         ((val) << 6)
        
        #define TIMER_UNIT_CFG_HI_CLKCFG_GET(value)                (GAP_BEXTRACTU((value),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG_GETS(value)               (GAP_BEXTRACT((value),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG_SET(value,field)          (GAP_BINSERT((value),(field),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG(val)                      ((val) << 7)
        
        #define TIMER_UNIT_CNT_LO_CNT_LO_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CNT_HI_CNT_HI_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CMP_LO_CMP_LO_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CMP_HI_CMP_HI_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI(val)                      ((val) << 0)
        
        #define TIMER_UNIT_START_LO_STRT_LO_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO(val)                   ((val) << 0)
        
        #define TIMER_UNIT_START_HI_STRT_HI_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI(val)                   ((val) << 0)
        
        #define TIMER_UNIT_RESET_LO_RST_LO_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO(val)                    ((val) << 0)
        
        #define TIMER_UNIT_RESET_HI_RST_HI_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** TIMER_UNIT_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cfg_lo;  // Timer Low Configuration register.
            volatile uint32_t cfg_hi;  // Timer High Configuration register.
            volatile uint32_t cnt_lo;  // Timer Low counter value register.
            volatile uint32_t cnt_hi;  // Timer High counter value register.
            volatile uint32_t cmp_lo;  // Timer Low comparator value register.
            volatile uint32_t cmp_hi;  // Timer High comparator value register.
            volatile uint32_t start_lo;  // Start Timer Low counting register.
            volatile uint32_t start_hi;  // Start Timer High counting register.
            volatile uint32_t reset_lo;  // Reset Timer Low counter register.
            volatile uint32_t reset_hi;  // Reset Timer High counter register.
        } __attribute__((packed)) timer_unit_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Timer low enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int reset           :1 ; // Timer low counter reset command bitfield. Cleared after Timer Low reset execution.
            unsigned int irqen           :1 ; // Timer low compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int iem             :1 ; // Timer low input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int mode            :1 ; // Timer low continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer low counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer low counter when compare match with CMP_LO occurs.
            unsigned int one_s           :1 ; // Timer low one shot configuration bitfield: - 1'b0: let Timer low enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer low when compare match with CMP_LO occurs.
            unsigned int pen             :1 ; // Timer low prescaler enable configuration bitfield:- 1'b0: disabled - 1'b1: enabled
            unsigned int ccfg            :1 ; // Timer low clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz
            unsigned int pval            :8 ; // Timer low prescaler value bitfield. Ftimer = Fclk / (1 + PRESC_VAL)
            unsigned int padding0:15;
            unsigned int casc            :1 ; // Timer low  + Timer high 64bit cascaded mode configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cfg_lo_t;
        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Timer high enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int reset           :1 ; // Timer high counter reset command bitfield. Cleared after Timer high reset execution.
            unsigned int irqen           :1 ; // Timer high compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int iem             :1 ; // Timer high input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int mode            :1 ; // Timer high continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer high counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer high counter when compare match with CMP_LO occurs.
            unsigned int one_s           :1 ; // Timer high one shot configuration bitfield: - 1'b0: let Timer high enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer high when compare match with CMP_LO occurs.
            unsigned int pen             :1 ; // Timer high prescaler enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int clkcfg          :1 ; // Timer high clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cfg_hi_t;
        
        typedef union {
          struct {
            unsigned int cnt_lo          :32; // Timer Low counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cnt_lo_t;
        
        typedef union {
          struct {
            unsigned int cnt_hi          :32; // Timer High counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cnt_hi_t;
        
        typedef union {
          struct {
            unsigned int cmp_lo          :32; // Timer Low comparator value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cmp_lo_t;
        
        typedef union {
          struct {
            unsigned int cmp_hi          :32; // Timer High comparator value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cmp_hi_t;
        
        typedef union {
          struct {
            unsigned int strt_lo         :1 ; // Timer Low start command bitfield. When executed, CFG_LO.ENABLE is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_start_lo_t;
        
        typedef union {
          struct {
            unsigned int strt_hi         :1 ; // Timer High start command bitfield. When executed, CFG_HI.ENABLE is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_start_hi_t;
        
        typedef union {
          struct {
            unsigned int rst_lo          :1 ; // Timer Low counter reset command bitfield. When executed, CFG_LO.RESET is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_reset_lo_t;
        
        typedef union {
          struct {
            unsigned int rst_hi          :1 ; // Timer High counter reset command bitfield. When executed, CFG_HI.RESET is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_reset_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_timer_unit : public vp::regmap
        {
        public:
            vp_timer_unit_cfg_lo cfg_lo;
            vp_timer_unit_cfg_hi cfg_hi;
            vp_timer_unit_cnt_lo cnt_lo;
            vp_timer_unit_cnt_hi cnt_hi;
            vp_timer_unit_cmp_lo cmp_lo;
            vp_timer_unit_cmp_hi cmp_hi;
            vp_timer_unit_start_lo start_lo;
            vp_timer_unit_start_hi start_hi;
            vp_timer_unit_reset_lo reset_lo;
            vp_timer_unit_reset_hi reset_hi;
        };

|

.. _timer_unit_CFG_LO:

CFG_LO
""""""

Timer Low Configuration register.

.. table:: 

    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                Description                                                                                                                 |
    +=====+===+======+============================================================================================================================================================================================================================================+
    |    0|R/W|ENABLE|Timer low enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                                                   |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|RESET |Timer low counter reset command bitfield. Cleared after Timer Low reset execution.                                                                                                                                                          |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|IRQEN |Timer low compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                           |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|IEM   |Timer low input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                                         |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|MODE  |Timer low continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer low counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer low counter when compare match with CMP_LO occurs.|
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|ONE_S |Timer low one shot configuration bitfield: - 1'b0: let Timer low enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer low when compare match with CMP_LO occurs.                                                   |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|PEN   |Timer low prescaler enable configuration bitfield:- 1'b0: disabled - 1'b1: enabled                                                                                                                                                          |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|CCFG  |Timer low clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz                                                                                                                                |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|PVAL  |Timer low prescaler value bitfield. Ftimer = Fclk / (1 + PRESC_VAL)                                                                                                                                                                         |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   31|R/W|CASC  |Timer low  + Timer high 64bit cascaded mode configuration bitfield.                                                                                                                                                                         |
    +-----+---+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer Low Configuration register.
                #define TIMER_UNIT_CFG_LO_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cfg_lo_get(uint32_t base);
        static inline void timer_unit_cfg_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer low enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_ENABLE_BIT                                 0
        #define TIMER_UNIT_CFG_LO_ENABLE_WIDTH                               1
        #define TIMER_UNIT_CFG_LO_ENABLE_MASK                                0x1
        #define TIMER_UNIT_CFG_LO_ENABLE_RESET                               0x0
        
        // Timer low counter reset command bitfield. Cleared after Timer Low reset execution. (access: R/W)
        #define TIMER_UNIT_CFG_LO_RESET_BIT                                  1
        #define TIMER_UNIT_CFG_LO_RESET_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_RESET_MASK                                 0x2
        #define TIMER_UNIT_CFG_LO_RESET_RESET                                0x0
        
        // Timer low compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_IRQEN_BIT                                  2
        #define TIMER_UNIT_CFG_LO_IRQEN_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_IRQEN_MASK                                 0x4
        #define TIMER_UNIT_CFG_LO_IRQEN_RESET                                0x0
        
        // Timer low input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_IEM_BIT                                    3
        #define TIMER_UNIT_CFG_LO_IEM_WIDTH                                  1
        #define TIMER_UNIT_CFG_LO_IEM_MASK                                   0x8
        #define TIMER_UNIT_CFG_LO_IEM_RESET                                  0x0
        
        // Timer low continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer low counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer low counter when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_LO_MODE_BIT                                   4
        #define TIMER_UNIT_CFG_LO_MODE_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_MODE_MASK                                  0x10
        #define TIMER_UNIT_CFG_LO_MODE_RESET                                 0x0
        
        // Timer low one shot configuration bitfield: - 1'b0: let Timer low enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer low when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_LO_ONE_S_BIT                                  5
        #define TIMER_UNIT_CFG_LO_ONE_S_WIDTH                                1
        #define TIMER_UNIT_CFG_LO_ONE_S_MASK                                 0x20
        #define TIMER_UNIT_CFG_LO_ONE_S_RESET                                0x0
        
        // Timer low prescaler enable configuration bitfield:- 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_LO_PEN_BIT                                    6
        #define TIMER_UNIT_CFG_LO_PEN_WIDTH                                  1
        #define TIMER_UNIT_CFG_LO_PEN_MASK                                   0x40
        #define TIMER_UNIT_CFG_LO_PEN_RESET                                  0x0
        
        // Timer low clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz (access: R/W)
        #define TIMER_UNIT_CFG_LO_CCFG_BIT                                   7
        #define TIMER_UNIT_CFG_LO_CCFG_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_CCFG_MASK                                  0x80
        #define TIMER_UNIT_CFG_LO_CCFG_RESET                                 0x0
        
        // Timer low prescaler value bitfield. Ftimer = Fclk / (1 + PRESC_VAL) (access: R/W)
        #define TIMER_UNIT_CFG_LO_PVAL_BIT                                   8
        #define TIMER_UNIT_CFG_LO_PVAL_WIDTH                                 8
        #define TIMER_UNIT_CFG_LO_PVAL_MASK                                  0xff00
        #define TIMER_UNIT_CFG_LO_PVAL_RESET                                 0x0
        
        // Timer low  + Timer high 64bit cascaded mode configuration bitfield. (access: R/W)
        #define TIMER_UNIT_CFG_LO_CASC_BIT                                   31
        #define TIMER_UNIT_CFG_LO_CASC_WIDTH                                 1
        #define TIMER_UNIT_CFG_LO_CASC_MASK                                  0x80000000
        #define TIMER_UNIT_CFG_LO_CASC_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CFG_LO_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_CFG_LO_ENABLE(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CFG_LO_RESET_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define TIMER_UNIT_CFG_LO_RESET_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define TIMER_UNIT_CFG_LO_RESET_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define TIMER_UNIT_CFG_LO_RESET(val)                       ((val) << 1)
        
        #define TIMER_UNIT_CFG_LO_IRQEN_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define TIMER_UNIT_CFG_LO_IRQEN(val)                       ((val) << 2)
        
        #define TIMER_UNIT_CFG_LO_IEM_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define TIMER_UNIT_CFG_LO_IEM_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define TIMER_UNIT_CFG_LO_IEM_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define TIMER_UNIT_CFG_LO_IEM(val)                         ((val) << 3)
        
        #define TIMER_UNIT_CFG_LO_MODE_GET(value)                  (GAP_BEXTRACTU((value),1,4))
        #define TIMER_UNIT_CFG_LO_MODE_GETS(value)                 (GAP_BEXTRACT((value),1,4))
        #define TIMER_UNIT_CFG_LO_MODE_SET(value,field)            (GAP_BINSERT((value),(field),1,4))
        #define TIMER_UNIT_CFG_LO_MODE(val)                        ((val) << 4)
        
        #define TIMER_UNIT_CFG_LO_ONE_S_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define TIMER_UNIT_CFG_LO_ONE_S(val)                       ((val) << 5)
        
        #define TIMER_UNIT_CFG_LO_PEN_GET(value)                   (GAP_BEXTRACTU((value),1,6))
        #define TIMER_UNIT_CFG_LO_PEN_GETS(value)                  (GAP_BEXTRACT((value),1,6))
        #define TIMER_UNIT_CFG_LO_PEN_SET(value,field)             (GAP_BINSERT((value),(field),1,6))
        #define TIMER_UNIT_CFG_LO_PEN(val)                         ((val) << 6)
        
        #define TIMER_UNIT_CFG_LO_CCFG_GET(value)                  (GAP_BEXTRACTU((value),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG_GETS(value)                 (GAP_BEXTRACT((value),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG_SET(value,field)            (GAP_BINSERT((value),(field),1,7))
        #define TIMER_UNIT_CFG_LO_CCFG(val)                        ((val) << 7)
        
        #define TIMER_UNIT_CFG_LO_PVAL_GET(value)                  (GAP_BEXTRACTU((value),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL_GETS(value)                 (GAP_BEXTRACT((value),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL_SET(value,field)            (GAP_BINSERT((value),(field),8,8))
        #define TIMER_UNIT_CFG_LO_PVAL(val)                        ((val) << 8)
        
        #define TIMER_UNIT_CFG_LO_CASC_GET(value)                  (GAP_BEXTRACTU((value),1,31))
        #define TIMER_UNIT_CFG_LO_CASC_GETS(value)                 (GAP_BEXTRACT((value),1,31))
        #define TIMER_UNIT_CFG_LO_CASC_SET(value,field)            (GAP_BINSERT((value),(field),1,31))
        #define TIMER_UNIT_CFG_LO_CASC(val)                        ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Timer low enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int reset           :1 ; // Timer low counter reset command bitfield. Cleared after Timer Low reset execution.
            unsigned int irqen           :1 ; // Timer low compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int iem             :1 ; // Timer low input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int mode            :1 ; // Timer low continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer low counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer low counter when compare match with CMP_LO occurs.
            unsigned int one_s           :1 ; // Timer low one shot configuration bitfield: - 1'b0: let Timer low enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer low when compare match with CMP_LO occurs.
            unsigned int pen             :1 ; // Timer low prescaler enable configuration bitfield:- 1'b0: disabled - 1'b1: enabled
            unsigned int ccfg            :1 ; // Timer low clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz
            unsigned int pval            :8 ; // Timer low prescaler value bitfield. Ftimer = Fclk / (1 + PRESC_VAL)
            unsigned int padding0:15;
            unsigned int casc            :1 ; // Timer low  + Timer high 64bit cascaded mode configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cfg_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cfg_lo : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void irqen_set(uint32_t value);
            inline uint32_t irqen_get();
            inline void iem_set(uint32_t value);
            inline uint32_t iem_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void one_s_set(uint32_t value);
            inline uint32_t one_s_get();
            inline void pen_set(uint32_t value);
            inline uint32_t pen_get();
            inline void ccfg_set(uint32_t value);
            inline uint32_t ccfg_get();
            inline void pval_set(uint32_t value);
            inline uint32_t pval_get();
            inline void casc_set(uint32_t value);
            inline uint32_t casc_get();
        };

|

.. _timer_unit_CFG_HI:

CFG_HI
""""""

Timer High Configuration register.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                  Description                                                                                                                  |
    +=====+===+======+===============================================================================================================================================================================================================================================+
    |    0|R/W|ENABLE|Timer high enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                                                     |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|W  |RESET |Timer high counter reset command bitfield. Cleared after Timer high reset execution.                                                                                                                                                           |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|IRQEN |Timer high compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                             |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|IEM   |Timer high input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                                           |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|MODE  |Timer high continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer high counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer high counter when compare match with CMP_LO occurs.|
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|ONE_S |Timer high one shot configuration bitfield: - 1'b0: let Timer high enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer high when compare match with CMP_LO occurs.                                                   |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|PEN   |Timer high prescaler enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                                                           |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|CLKCFG|Timer high clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz                                                                                                                                  |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer High Configuration register.
                #define TIMER_UNIT_CFG_HI_OFFSET                 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cfg_hi_get(uint32_t base);
        static inline void timer_unit_cfg_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer high enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_ENABLE_BIT                                 0
        #define TIMER_UNIT_CFG_HI_ENABLE_WIDTH                               1
        #define TIMER_UNIT_CFG_HI_ENABLE_MASK                                0x1
        #define TIMER_UNIT_CFG_HI_ENABLE_RESET                               0x0
        
        // Timer high counter reset command bitfield. Cleared after Timer high reset execution. (access: W)
        #define TIMER_UNIT_CFG_HI_RESET_BIT                                  1
        #define TIMER_UNIT_CFG_HI_RESET_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_RESET_MASK                                 0x2
        #define TIMER_UNIT_CFG_HI_RESET_RESET                                0x0
        
        // Timer high compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_IRQEN_BIT                                  2
        #define TIMER_UNIT_CFG_HI_IRQEN_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_IRQEN_MASK                                 0x4
        #define TIMER_UNIT_CFG_HI_IRQEN_RESET                                0x0
        
        // Timer high input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_IEM_BIT                                    3
        #define TIMER_UNIT_CFG_HI_IEM_WIDTH                                  1
        #define TIMER_UNIT_CFG_HI_IEM_MASK                                   0x8
        #define TIMER_UNIT_CFG_HI_IEM_RESET                                  0x0
        
        // Timer high continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer high counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer high counter when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_HI_MODE_BIT                                   4
        #define TIMER_UNIT_CFG_HI_MODE_WIDTH                                 1
        #define TIMER_UNIT_CFG_HI_MODE_MASK                                  0x10
        #define TIMER_UNIT_CFG_HI_MODE_RESET                                 0x0
        
        // Timer high one shot configuration bitfield: - 1'b0: let Timer high enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer high when compare match with CMP_LO occurs. (access: R/W)
        #define TIMER_UNIT_CFG_HI_ONE_S_BIT                                  5
        #define TIMER_UNIT_CFG_HI_ONE_S_WIDTH                                1
        #define TIMER_UNIT_CFG_HI_ONE_S_MASK                                 0x20
        #define TIMER_UNIT_CFG_HI_ONE_S_RESET                                0x0
        
        // Timer high prescaler enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define TIMER_UNIT_CFG_HI_PEN_BIT                                    6
        #define TIMER_UNIT_CFG_HI_PEN_WIDTH                                  1
        #define TIMER_UNIT_CFG_HI_PEN_MASK                                   0x40
        #define TIMER_UNIT_CFG_HI_PEN_RESET                                  0x0
        
        // Timer high clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz (access: R/W)
        #define TIMER_UNIT_CFG_HI_CLKCFG_BIT                                 7
        #define TIMER_UNIT_CFG_HI_CLKCFG_WIDTH                               1
        #define TIMER_UNIT_CFG_HI_CLKCFG_MASK                                0x80
        #define TIMER_UNIT_CFG_HI_CLKCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CFG_HI_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_CFG_HI_ENABLE(val)                      ((val) << 0)
        
        #define TIMER_UNIT_CFG_HI_RESET_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define TIMER_UNIT_CFG_HI_RESET_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define TIMER_UNIT_CFG_HI_RESET_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define TIMER_UNIT_CFG_HI_RESET(val)                       ((val) << 1)
        
        #define TIMER_UNIT_CFG_HI_IRQEN_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define TIMER_UNIT_CFG_HI_IRQEN(val)                       ((val) << 2)
        
        #define TIMER_UNIT_CFG_HI_IEM_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define TIMER_UNIT_CFG_HI_IEM_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define TIMER_UNIT_CFG_HI_IEM_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define TIMER_UNIT_CFG_HI_IEM(val)                         ((val) << 3)
        
        #define TIMER_UNIT_CFG_HI_MODE_GET(value)                  (GAP_BEXTRACTU((value),1,4))
        #define TIMER_UNIT_CFG_HI_MODE_GETS(value)                 (GAP_BEXTRACT((value),1,4))
        #define TIMER_UNIT_CFG_HI_MODE_SET(value,field)            (GAP_BINSERT((value),(field),1,4))
        #define TIMER_UNIT_CFG_HI_MODE(val)                        ((val) << 4)
        
        #define TIMER_UNIT_CFG_HI_ONE_S_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define TIMER_UNIT_CFG_HI_ONE_S(val)                       ((val) << 5)
        
        #define TIMER_UNIT_CFG_HI_PEN_GET(value)                   (GAP_BEXTRACTU((value),1,6))
        #define TIMER_UNIT_CFG_HI_PEN_GETS(value)                  (GAP_BEXTRACT((value),1,6))
        #define TIMER_UNIT_CFG_HI_PEN_SET(value,field)             (GAP_BINSERT((value),(field),1,6))
        #define TIMER_UNIT_CFG_HI_PEN(val)                         ((val) << 6)
        
        #define TIMER_UNIT_CFG_HI_CLKCFG_GET(value)                (GAP_BEXTRACTU((value),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG_GETS(value)               (GAP_BEXTRACT((value),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG_SET(value,field)          (GAP_BINSERT((value),(field),1,7))
        #define TIMER_UNIT_CFG_HI_CLKCFG(val)                      ((val) << 7)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Timer high enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int reset           :1 ; // Timer high counter reset command bitfield. Cleared after Timer high reset execution.
            unsigned int irqen           :1 ; // Timer high compare match interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int iem             :1 ; // Timer high input event mask configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int mode            :1 ; // Timer high continuous mode configuration bitfield: - 1'b0: Continue mode - continue incrementing Timer high counter when compare match with CMP_LO occurs. - 1'b1: Cycle mode - reset Timer high counter when compare match with CMP_LO occurs.
            unsigned int one_s           :1 ; // Timer high one shot configuration bitfield: - 1'b0: let Timer high enabled counting when compare match with CMP_LO occurs. - 1'b1: disable Timer high when compare match with CMP_LO occurs.
            unsigned int pen             :1 ; // Timer high prescaler enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int clkcfg          :1 ; // Timer high clock source configuration bitfield: - 1'b0: FLL or FLL+Prescaler - 1'b1: Reference clock at 32kHz
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cfg_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cfg_hi : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void irqen_set(uint32_t value);
            inline uint32_t irqen_get();
            inline void iem_set(uint32_t value);
            inline uint32_t iem_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void one_s_set(uint32_t value);
            inline uint32_t one_s_get();
            inline void pen_set(uint32_t value);
            inline uint32_t pen_get();
            inline void clkcfg_set(uint32_t value);
            inline uint32_t clkcfg_get();
        };

|

.. _timer_unit_CNT_LO:

CNT_LO
""""""

Timer Low counter value register.

.. table:: 

    +-----+---+------+---------------------------------+
    |Bit #|R/W| Name |           Description           |
    +=====+===+======+=================================+
    |31:0 |R/W|CNT_LO|Timer Low counter value bitfield.|
    +-----+---+------+---------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer Low counter value register.
                #define TIMER_UNIT_CNT_LO_OFFSET                 0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cnt_lo_get(uint32_t base);
        static inline void timer_unit_cnt_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer Low counter value bitfield. (access: R/W)
        #define TIMER_UNIT_CNT_LO_CNT_LO_BIT                                 0
        #define TIMER_UNIT_CNT_LO_CNT_LO_WIDTH                               32
        #define TIMER_UNIT_CNT_LO_CNT_LO_MASK                                0xffffffff
        #define TIMER_UNIT_CNT_LO_CNT_LO_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CNT_LO_CNT_LO_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CNT_LO_CNT_LO(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt_lo          :32; // Timer Low counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cnt_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cnt_lo : public vp::reg_32
        {
        public:
            inline void cnt_lo_set(uint32_t value);
            inline uint32_t cnt_lo_get();
        };

|

.. _timer_unit_CNT_HI:

CNT_HI
""""""

Timer High counter value register.

.. table:: 

    +-----+---+------+----------------------------------+
    |Bit #|R/W| Name |           Description            |
    +=====+===+======+==================================+
    |31:0 |R/W|CNT_HI|Timer High counter value bitfield.|
    +-----+---+------+----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer High counter value register.
                #define TIMER_UNIT_CNT_HI_OFFSET                 0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cnt_hi_get(uint32_t base);
        static inline void timer_unit_cnt_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer High counter value bitfield. (access: R/W)
        #define TIMER_UNIT_CNT_HI_CNT_HI_BIT                                 0
        #define TIMER_UNIT_CNT_HI_CNT_HI_WIDTH                               32
        #define TIMER_UNIT_CNT_HI_CNT_HI_MASK                                0xffffffff
        #define TIMER_UNIT_CNT_HI_CNT_HI_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CNT_HI_CNT_HI_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CNT_HI_CNT_HI(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt_hi          :32; // Timer High counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cnt_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cnt_hi : public vp::reg_32
        {
        public:
            inline void cnt_hi_set(uint32_t value);
            inline uint32_t cnt_hi_get();
        };

|

.. _timer_unit_CMP_LO:

CMP_LO
""""""

Timer Low comparator value register.

.. table:: 

    +-----+---+------+------------------------------------+
    |Bit #|R/W| Name |            Description             |
    +=====+===+======+====================================+
    |31:0 |R/W|CMP_LO|Timer Low comparator value bitfield.|
    +-----+---+------+------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer Low comparator value register.
                #define TIMER_UNIT_CMP_LO_OFFSET                 0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cmp_lo_get(uint32_t base);
        static inline void timer_unit_cmp_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer Low comparator value bitfield. (access: R/W)
        #define TIMER_UNIT_CMP_LO_CMP_LO_BIT                                 0
        #define TIMER_UNIT_CMP_LO_CMP_LO_WIDTH                               32
        #define TIMER_UNIT_CMP_LO_CMP_LO_MASK                                0xffffffff
        #define TIMER_UNIT_CMP_LO_CMP_LO_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CMP_LO_CMP_LO_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CMP_LO_CMP_LO(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmp_lo          :32; // Timer Low comparator value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cmp_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cmp_lo : public vp::reg_32
        {
        public:
            inline void cmp_lo_set(uint32_t value);
            inline uint32_t cmp_lo_get();
        };

|

.. _timer_unit_CMP_HI:

CMP_HI
""""""

Timer High comparator value register.

.. table:: 

    +-----+---+------+-------------------------------------+
    |Bit #|R/W| Name |             Description             |
    +=====+===+======+=====================================+
    |31:0 |R/W|CMP_HI|Timer High comparator value bitfield.|
    +-----+---+------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timer High comparator value register.
                #define TIMER_UNIT_CMP_HI_OFFSET                 0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_cmp_hi_get(uint32_t base);
        static inline void timer_unit_cmp_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer High comparator value bitfield. (access: R/W)
        #define TIMER_UNIT_CMP_HI_CMP_HI_BIT                                 0
        #define TIMER_UNIT_CMP_HI_CMP_HI_WIDTH                               32
        #define TIMER_UNIT_CMP_HI_CMP_HI_MASK                                0xffffffff
        #define TIMER_UNIT_CMP_HI_CMP_HI_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_CMP_HI_CMP_HI_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define TIMER_UNIT_CMP_HI_CMP_HI(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmp_hi          :32; // Timer High comparator value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_cmp_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_cmp_hi : public vp::reg_32
        {
        public:
            inline void cmp_hi_set(uint32_t value);
            inline uint32_t cmp_hi_get();
        };

|

.. _timer_unit_START_LO:

START_LO
""""""""

Start Timer Low counting register.

.. table:: 

    +-----+---+-------+----------------------------------------------------------------------+
    |Bit #|R/W| Name  |                             Description                              |
    +=====+===+=======+======================================================================+
    |    0|W  |STRT_LO|Timer Low start command bitfield. When executed, CFG_LO.ENABLE is set.|
    +-----+---+-------+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Timer Low counting register.
                #define TIMER_UNIT_START_LO_OFFSET               0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_start_lo_get(uint32_t base);
        static inline void timer_unit_start_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer Low start command bitfield. When executed, CFG_LO.ENABLE is set. (access: W)
        #define TIMER_UNIT_START_LO_STRT_LO_BIT                              0
        #define TIMER_UNIT_START_LO_STRT_LO_WIDTH                            1
        #define TIMER_UNIT_START_LO_STRT_LO_MASK                             0x1
        #define TIMER_UNIT_START_LO_STRT_LO_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_START_LO_STRT_LO_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_START_LO_STRT_LO(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int strt_lo         :1 ; // Timer Low start command bitfield. When executed, CFG_LO.ENABLE is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_start_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_start_lo : public vp::reg_32
        {
        public:
            inline void strt_lo_set(uint32_t value);
            inline uint32_t strt_lo_get();
        };

|

.. _timer_unit_START_HI:

START_HI
""""""""

Start Timer High counting register.

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------------+
    |Bit #|R/W| Name  |                              Description                              |
    +=====+===+=======+=======================================================================+
    |    0|W  |STRT_HI|Timer High start command bitfield. When executed, CFG_HI.ENABLE is set.|
    +-----+---+-------+-----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Timer High counting register.
                #define TIMER_UNIT_START_HI_OFFSET               0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_start_hi_get(uint32_t base);
        static inline void timer_unit_start_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer High start command bitfield. When executed, CFG_HI.ENABLE is set. (access: W)
        #define TIMER_UNIT_START_HI_STRT_HI_BIT                              0
        #define TIMER_UNIT_START_HI_STRT_HI_WIDTH                            1
        #define TIMER_UNIT_START_HI_STRT_HI_MASK                             0x1
        #define TIMER_UNIT_START_HI_STRT_HI_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_START_HI_STRT_HI_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_START_HI_STRT_HI(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int strt_hi         :1 ; // Timer High start command bitfield. When executed, CFG_HI.ENABLE is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_start_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_start_hi : public vp::reg_32
        {
        public:
            inline void strt_hi_set(uint32_t value);
            inline uint32_t strt_hi_get();
        };

|

.. _timer_unit_RESET_LO:

RESET_LO
""""""""

Reset Timer Low counter register.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                 Description                                 |
    +=====+===+======+=============================================================================+
    |    0|W  |RST_LO|Timer Low counter reset command bitfield. When executed, CFG_LO.RESET is set.|
    +-----+---+------+-----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reset Timer Low counter register.
                #define TIMER_UNIT_RESET_LO_OFFSET               0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_reset_lo_get(uint32_t base);
        static inline void timer_unit_reset_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer Low counter reset command bitfield. When executed, CFG_LO.RESET is set. (access: W)
        #define TIMER_UNIT_RESET_LO_RST_LO_BIT                               0
        #define TIMER_UNIT_RESET_LO_RST_LO_WIDTH                             1
        #define TIMER_UNIT_RESET_LO_RST_LO_MASK                              0x1
        #define TIMER_UNIT_RESET_LO_RST_LO_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_RESET_LO_RST_LO_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_RESET_LO_RST_LO(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rst_lo          :1 ; // Timer Low counter reset command bitfield. When executed, CFG_LO.RESET is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_reset_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_reset_lo : public vp::reg_32
        {
        public:
            inline void rst_lo_set(uint32_t value);
            inline uint32_t rst_lo_get();
        };

|

.. _timer_unit_RESET_HI:

RESET_HI
""""""""

Reset Timer High counter register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                 Description                                  |
    +=====+===+======+==============================================================================+
    |    0|W  |RST_HI|Timer High counter reset command bitfield. When executed, CFG_HI.RESET is set.|
    +-----+---+------+------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reset Timer High counter register.
                #define TIMER_UNIT_RESET_HI_OFFSET               0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t timer_unit_reset_hi_get(uint32_t base);
        static inline void timer_unit_reset_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Timer High counter reset command bitfield. When executed, CFG_HI.RESET is set. (access: W)
        #define TIMER_UNIT_RESET_HI_RST_HI_BIT                               0
        #define TIMER_UNIT_RESET_HI_RST_HI_WIDTH                             1
        #define TIMER_UNIT_RESET_HI_RST_HI_MASK                              0x1
        #define TIMER_UNIT_RESET_HI_RST_HI_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define TIMER_UNIT_RESET_HI_RST_HI_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define TIMER_UNIT_RESET_HI_RST_HI(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rst_hi          :1 ; // Timer High counter reset command bitfield. When executed, CFG_HI.RESET is set.
          };
          unsigned int raw;
        } __attribute__((packed)) timer_unit_reset_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_timer_unit_reset_hi : public vp::reg_32
        {
        public:
            inline void rst_hi_set(uint32_t value);
            inline uint32_t rst_hi_get();
        };

|
