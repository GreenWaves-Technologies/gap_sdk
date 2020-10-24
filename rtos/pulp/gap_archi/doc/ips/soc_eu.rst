Input file: docs/IP_REFERENCES/SOC_EVENT_GENERATOR_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |                   Name                   |Offset|Width|                         Description                         |
    +==========================================+======+=====+=============================================================+
    |:ref:`SW_EVENT<soc_eu_SW_EVENT>`          |     0|   32|SoC software events trigger command register.                |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`FC_MASK_MSB<soc_eu_FC_MASK_MSB>`    |     4|   32|MSB FC event unit event dispatch mask configuration register.|
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`FC_MASK_LSB<soc_eu_FC_MASK_LSB>`    |     8|   32|LSB FC event unit event dispatch mask configuration register.|
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`CL_MASK_MSB<soc_eu_CL_MASK_MSB>`    |    12|   32|MSB Cluster event dispatch mask configuration register.      |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`CL_MASK_LSB<soc_eu_CL_MASK_LSB>`    |    16|   32|LSB Cluster event dispatch mask configuration register.      |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`PR_MASK_MSB<soc_eu_PR_MASK_MSB>`    |    20|   32|MSB uDMA event dispatch mask configuration register.         |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`PR_MASK_LSB<soc_eu_PR_MASK_LSB>`    |    24|   32|LSB uDMA event dispatch mask configuration register.         |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`ERR_MSB<soc_eu_ERR_MSB>`            |    28|   32|MSB event queue overflow status register.                    |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`ERR_LSB<soc_eu_ERR_LSB>`            |    32|   32|LSB event queue overflow status register.                    |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`TIMER1_SEL_HI<soc_eu_TIMER1_SEL_HI>`|   132|   32|FC High Timer1 source event configuration register.          |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`TIMER1_SEL_LO<soc_eu_TIMER1_SEL_LO>`|   136|   32|FC Low Timer1 source event configuration register.           |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`TIMER2_SEL_HI<soc_eu_TIMER2_SEL_HI>`|   140|   32|FC High Timer2 source event configuration register.          |
    +------------------------------------------+------+-----+-------------------------------------------------------------+
    |:ref:`TIMER2_SEL_LO<soc_eu_TIMER2_SEL_LO>`|   144|   32|FC Low Timer2 source event configuration register.           |
    +------------------------------------------+------+-----+-------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SoC software events trigger command register.
                #define SOC_EU_SW_EVENT_OFFSET                   0x0
        
                // MSB FC event unit event dispatch mask configuration register.
                #define SOC_EU_FC_MASK_MSB_OFFSET                0x4
        
                // LSB FC event unit event dispatch mask configuration register.
                #define SOC_EU_FC_MASK_LSB_OFFSET                0x8
        
                // MSB Cluster event dispatch mask configuration register.
                #define SOC_EU_CL_MASK_MSB_OFFSET                0xc
        
                // LSB Cluster event dispatch mask configuration register.
                #define SOC_EU_CL_MASK_LSB_OFFSET                0x10
        
                // MSB uDMA event dispatch mask configuration register.
                #define SOC_EU_PR_MASK_MSB_OFFSET                0x14
        
                // LSB uDMA event dispatch mask configuration register.
                #define SOC_EU_PR_MASK_LSB_OFFSET                0x18
        
                // MSB event queue overflow status register.
                #define SOC_EU_ERR_MSB_OFFSET                    0x1c
        
                // LSB event queue overflow status register.
                #define SOC_EU_ERR_LSB_OFFSET                    0x20
        
                // FC High Timer1 source event configuration register.
                #define SOC_EU_TIMER1_SEL_HI_OFFSET              0x84
        
                // FC Low Timer1 source event configuration register.
                #define SOC_EU_TIMER1_SEL_LO_OFFSET              0x88
        
                // FC High Timer2 source event configuration register.
                #define SOC_EU_TIMER2_SEL_HI_OFFSET              0x8c
        
                // FC Low Timer2 source event configuration register.
                #define SOC_EU_TIMER2_SEL_LO_OFFSET              0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_sw_event_get(uint32_t base);
        static inline void soc_eu_sw_event_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_fc_mask_msb_get(uint32_t base);
        static inline void soc_eu_fc_mask_msb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_fc_mask_lsb_get(uint32_t base);
        static inline void soc_eu_fc_mask_lsb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_cl_mask_msb_get(uint32_t base);
        static inline void soc_eu_cl_mask_msb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_cl_mask_lsb_get(uint32_t base);
        static inline void soc_eu_cl_mask_lsb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_pr_mask_msb_get(uint32_t base);
        static inline void soc_eu_pr_mask_msb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_pr_mask_lsb_get(uint32_t base);
        static inline void soc_eu_pr_mask_lsb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_err_msb_get(uint32_t base);
        static inline void soc_eu_err_msb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_err_lsb_get(uint32_t base);
        static inline void soc_eu_err_lsb_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_timer1_sel_hi_get(uint32_t base);
        static inline void soc_eu_timer1_sel_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_timer1_sel_lo_get(uint32_t base);
        static inline void soc_eu_timer1_sel_lo_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_timer2_sel_hi_get(uint32_t base);
        static inline void soc_eu_timer2_sel_hi_set(uint32_t base, uint32_t value);

        static inline uint32_t soc_eu_timer2_sel_lo_get(uint32_t base);
        static inline void soc_eu_timer2_sel_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided. (access: W)
        #define SOC_EU_SW_EVENT_EVENT_BIT                                    0
        #define SOC_EU_SW_EVENT_EVENT_WIDTH                                  8
        #define SOC_EU_SW_EVENT_EVENT_MASK                                   0xff
        #define SOC_EU_SW_EVENT_EVENT_RESET                                  0x0
        
        // MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_BIT                           0
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_WIDTH                         32
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_RESET                         0xffffffff
        
        // LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit. (access: R/W)
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_BIT                           0
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_WIDTH                         32
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_RESET                         0xffffffff
        
        // MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_BIT                           0
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_WIDTH                         32
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_RESET                         0xffffffff
        
        // LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit. (access: R/W)
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_BIT                           0
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_WIDTH                         32
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_RESET                         0xffffffff
        
        // MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_BIT                           0
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_WIDTH                         32
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_RESET                         0xffffffff
        
        // LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA. (access: R/W)
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_BIT                           0
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_WIDTH                         32
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_RESET                         0xffffffff
        
        // Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R)
        #define SOC_EU_ERR_MSB_ERR_MSB_BIT                                   0
        #define SOC_EU_ERR_MSB_ERR_MSB_WIDTH                                 32
        #define SOC_EU_ERR_MSB_ERR_MSB_MASK                                  0xffffffff
        #define SOC_EU_ERR_MSB_ERR_MSB_RESET                                 0x0
        
        // Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue. (access: R)
        #define SOC_EU_ERR_LSB_ERR_LSB_BIT                                   0
        #define SOC_EU_ERR_LSB_ERR_LSB_WIDTH                                 32
        #define SOC_EU_ERR_LSB_ERR_LSB_MASK                                  0xffffffff
        #define SOC_EU_ERR_LSB_ERR_LSB_RESET                                 0x0
        
        // Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_HI_EVT_BIT                                 0
        #define SOC_EU_TIMER1_SEL_HI_EVT_WIDTH                               8
        #define SOC_EU_TIMER1_SEL_HI_EVT_MASK                                0xff
        #define SOC_EU_TIMER1_SEL_HI_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_HI_ENA_BIT                                 31
        #define SOC_EU_TIMER1_SEL_HI_ENA_WIDTH                               1
        #define SOC_EU_TIMER1_SEL_HI_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER1_SEL_HI_ENA_RESET                               0x0
        
        // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_LO_EVT_BIT                                 0
        #define SOC_EU_TIMER1_SEL_LO_EVT_WIDTH                               8
        #define SOC_EU_TIMER1_SEL_LO_EVT_MASK                                0xff
        #define SOC_EU_TIMER1_SEL_LO_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_LO_ENA_BIT                                 31
        #define SOC_EU_TIMER1_SEL_LO_ENA_WIDTH                               1
        #define SOC_EU_TIMER1_SEL_LO_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER1_SEL_LO_ENA_RESET                               0x0
        
        // Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_HI_EVT_BIT                                 0
        #define SOC_EU_TIMER2_SEL_HI_EVT_WIDTH                               8
        #define SOC_EU_TIMER2_SEL_HI_EVT_MASK                                0xff
        #define SOC_EU_TIMER2_SEL_HI_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_HI_ENA_BIT                                 31
        #define SOC_EU_TIMER2_SEL_HI_ENA_WIDTH                               1
        #define SOC_EU_TIMER2_SEL_HI_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER2_SEL_HI_ENA_RESET                               0x0
        
        // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_LO_EVT_BIT                                 0
        #define SOC_EU_TIMER2_SEL_LO_EVT_WIDTH                               8
        #define SOC_EU_TIMER2_SEL_LO_EVT_MASK                                0xff
        #define SOC_EU_TIMER2_SEL_LO_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_LO_ENA_BIT                                 31
        #define SOC_EU_TIMER2_SEL_LO_ENA_WIDTH                               1
        #define SOC_EU_TIMER2_SEL_LO_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER2_SEL_LO_ENA_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_SW_EVENT_EVENT_GET(value)                   (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_SW_EVENT_EVENT_GETS(value)                  (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_SW_EVENT_EVENT_SET(value,field)             (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_SW_EVENT_EVENT(val)                         ((val) << 0)
        
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB(val)                ((val) << 0)
        
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB(val)                ((val) << 0)
        
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB(val)                ((val) << 0)
        
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB(val)                ((val) << 0)
        
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB(val)                ((val) << 0)
        
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB(val)                ((val) << 0)
        
        #define SOC_EU_ERR_MSB_ERR_MSB_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB(val)                        ((val) << 0)
        
        #define SOC_EU_ERR_LSB_ERR_LSB_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB(val)                        ((val) << 0)
        
        #define SOC_EU_TIMER1_SEL_HI_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER1_SEL_HI_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA(val)                      ((val) << 31)
        
        #define SOC_EU_TIMER1_SEL_LO_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER1_SEL_LO_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA(val)                      ((val) << 31)
        
        #define SOC_EU_TIMER2_SEL_HI_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER2_SEL_HI_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA(val)                      ((val) << 31)
        
        #define SOC_EU_TIMER2_SEL_LO_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER2_SEL_LO_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** SOC_EU_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t sw_event;  // SoC software events trigger command register.
            volatile uint32_t fc_mask_msb;  // MSB FC event unit event dispatch mask configuration register.
            volatile uint32_t fc_mask_lsb;  // LSB FC event unit event dispatch mask configuration register.
            volatile uint32_t cl_mask_msb;  // MSB Cluster event dispatch mask configuration register.
            volatile uint32_t cl_mask_lsb;  // LSB Cluster event dispatch mask configuration register.
            volatile uint32_t pr_mask_msb;  // MSB uDMA event dispatch mask configuration register.
            volatile uint32_t pr_mask_lsb;  // LSB uDMA event dispatch mask configuration register.
            volatile uint32_t err_msb;  // MSB event queue overflow status register.
            volatile uint32_t err_lsb;  // LSB event queue overflow status register.
            volatile uint32_t reserved_0[24];  // Reserved/Not used.
            volatile uint32_t timer1_sel_hi;  // FC High Timer1 source event configuration register.
            volatile uint32_t timer1_sel_lo;  // FC Low Timer1 source event configuration register.
            volatile uint32_t timer2_sel_hi;  // FC High Timer2 source event configuration register.
            volatile uint32_t timer2_sel_lo;  // FC Low Timer2 source event configuration register.
        } __attribute__((packed)) soc_eu_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int event           :8 ; // Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_sw_event_t;
        
        typedef union {
          struct {
            unsigned int fc_mask_msb     :32; // MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_fc_mask_msb_t;
        
        typedef union {
          struct {
            unsigned int fc_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_fc_mask_lsb_t;
        
        typedef union {
          struct {
            unsigned int cl_mask_msb     :32; // MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_cl_mask_msb_t;
        
        typedef union {
          struct {
            unsigned int cl_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_cl_mask_lsb_t;
        
        typedef union {
          struct {
            unsigned int pr_mask_msb     :32; // MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_pr_mask_msb_t;
        
        typedef union {
          struct {
            unsigned int pr_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_pr_mask_lsb_t;
        
        typedef union {
          struct {
            unsigned int err_msb         :32; // Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_err_msb_t;
        
        typedef union {
          struct {
            unsigned int err_lsb         :32; // Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_err_lsb_t;
        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer1_sel_hi_t;
        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer1_sel_lo_t;
        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer2_sel_hi_t;
        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer2_sel_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_soc_eu : public vp::regmap
        {
        public:
            vp_soc_eu_sw_event sw_event;
            vp_soc_eu_fc_mask_msb fc_mask_msb;
            vp_soc_eu_fc_mask_lsb fc_mask_lsb;
            vp_soc_eu_cl_mask_msb cl_mask_msb;
            vp_soc_eu_cl_mask_lsb cl_mask_lsb;
            vp_soc_eu_pr_mask_msb pr_mask_msb;
            vp_soc_eu_pr_mask_lsb pr_mask_lsb;
            vp_soc_eu_err_msb err_msb;
            vp_soc_eu_err_lsb err_lsb;
            vp_soc_eu_timer1_sel_hi timer1_sel_hi;
            vp_soc_eu_timer1_sel_lo timer1_sel_lo;
            vp_soc_eu_timer2_sel_hi timer2_sel_hi;
            vp_soc_eu_timer2_sel_lo timer2_sel_lo;
        };

|

.. _soc_eu_SW_EVENT:

SW_EVENT
""""""""

SoC software events trigger command register.

.. table:: 

    +-----+---+-----+----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                               Description                                                |
    +=====+===+=====+==========================================================================================================+
    |7:0  |W  |EVENT|Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided.|
    +-----+---+-----+----------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SoC software events trigger command register.
                #define SOC_EU_SW_EVENT_OFFSET                   0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_sw_event_get(uint32_t base);
        static inline void soc_eu_sw_event_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided. (access: W)
        #define SOC_EU_SW_EVENT_EVENT_BIT                                    0
        #define SOC_EU_SW_EVENT_EVENT_WIDTH                                  8
        #define SOC_EU_SW_EVENT_EVENT_MASK                                   0xff
        #define SOC_EU_SW_EVENT_EVENT_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_SW_EVENT_EVENT_GET(value)                   (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_SW_EVENT_EVENT_GETS(value)                  (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_SW_EVENT_EVENT_SET(value,field)             (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_SW_EVENT_EVENT(val)                         ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int event           :8 ; // Writing a one-hot value into EVENT bitfield triggers SoC software event i. 8 software events are provided.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_sw_event_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_sw_event : public vp::reg_32
        {
        public:
            inline void event_set(uint32_t value);
            inline uint32_t event_get();
        };

|

.. _soc_eu_FC_MASK_MSB:

FC_MASK_MSB
"""""""""""

MSB FC event unit event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                                     Description                                                                                                      |
    +=====+===+===========+======================================================================================================================================================================================================================+
    |31:0 |R/W|FC_MASK_MSB|MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.|
    +-----+---+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MSB FC event unit event dispatch mask configuration register.
                #define SOC_EU_FC_MASK_MSB_OFFSET                0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_fc_mask_msb_get(uint32_t base);
        static inline void soc_eu_fc_mask_msb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit. (access: R/W)
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_BIT                           0
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_WIDTH                         32
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_FC_MASK_MSB_FC_MASK_MSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int fc_mask_msb     :32; // MSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to FC event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_fc_mask_msb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_fc_mask_msb : public vp::reg_32
        {
        public:
            inline void fc_mask_msb_set(uint32_t value);
            inline uint32_t fc_mask_msb_get();
        };

|

.. _soc_eu_FC_MASK_LSB:

FC_MASK_LSB
"""""""""""

LSB FC event unit event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                                  Description                                                                                                   |
    +=====+===+===========+================================================================================================================================================================================================================+
    |31:0 |R/W|FC_MASK_LSB|LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit.|
    +-----+---+-----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // LSB FC event unit event dispatch mask configuration register.
                #define SOC_EU_FC_MASK_LSB_OFFSET                0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_fc_mask_lsb_get(uint32_t base);
        static inline void soc_eu_fc_mask_lsb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit. (access: R/W)
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_BIT                           0
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_WIDTH                         32
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_FC_MASK_LSB_FC_MASK_LSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int fc_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to FC event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to FC event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to FC event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_fc_mask_lsb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_fc_mask_lsb : public vp::reg_32
        {
        public:
            inline void fc_mask_lsb_set(uint32_t value);
            inline uint32_t fc_mask_lsb_get();
        };

|

.. _soc_eu_CL_MASK_MSB:

CL_MASK_MSB
"""""""""""

MSB Cluster event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                                             Description                                                                                                             |
    +=====+===+===========+=====================================================================================================================================================================================================================================+
    |31:0 |R/W|CL_MASK_MSB|MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.|
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MSB Cluster event dispatch mask configuration register.
                #define SOC_EU_CL_MASK_MSB_OFFSET                0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_cl_mask_msb_get(uint32_t base);
        static inline void soc_eu_cl_mask_msb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit. (access: R/W)
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_BIT                           0
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_WIDTH                         32
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_CL_MASK_MSB_CL_MASK_MSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cl_mask_msb     :32; // MSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to Cluster event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_cl_mask_msb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_cl_mask_msb : public vp::reg_32
        {
        public:
            inline void cl_mask_msb_set(uint32_t value);
            inline uint32_t cl_mask_msb_get();
        };

|

.. _soc_eu_CL_MASK_LSB:

CL_MASK_LSB
"""""""""""

LSB Cluster event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                                          Description                                                                                                          |
    +=====+===+===========+===============================================================================================================================================================================================================================+
    |31:0 |R/W|CL_MASK_LSB|LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit.|
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // LSB Cluster event dispatch mask configuration register.
                #define SOC_EU_CL_MASK_LSB_OFFSET                0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_cl_mask_lsb_get(uint32_t base);
        static inline void soc_eu_cl_mask_lsb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit. (access: R/W)
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_BIT                           0
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_WIDTH                         32
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_CL_MASK_LSB_CL_MASK_LSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cl_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to Cluster event unit. - Setting bit[i] to 1'b1 disable dispatching event[i] to Cluster event unit. - Setting bit[i] to 1'b0 enable dispatching event[i] to Cluster event unit.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_cl_mask_lsb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_cl_mask_lsb : public vp::reg_32
        {
        public:
            inline void cl_mask_lsb_set(uint32_t value);
            inline uint32_t cl_mask_lsb_get();
        };

|

.. _soc_eu_PR_MASK_MSB:

PR_MASK_MSB
"""""""""""

MSB uDMA event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                              Description                                                                                              |
    +=====+===+===========+=======================================================================================================================================================================================================+
    |31:0 |R/W|PR_MASK_MSB|MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.|
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MSB uDMA event dispatch mask configuration register.
                #define SOC_EU_PR_MASK_MSB_OFFSET                0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_pr_mask_msb_get(uint32_t base);
        static inline void soc_eu_pr_mask_msb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA. (access: R/W)
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_BIT                           0
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_WIDTH                         32
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_MASK                          0xffffffff
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_PR_MASK_MSB_PR_MASK_MSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pr_mask_msb     :32; // MSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[32+i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[32+i] to uDMA.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_pr_mask_msb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_pr_mask_msb : public vp::reg_32
        {
        public:
            inline void pr_mask_msb_set(uint32_t value);
            inline uint32_t pr_mask_msb_get();
        };

|

.. _soc_eu_PR_MASK_LSB:

PR_MASK_LSB
"""""""""""

LSB uDMA event dispatch mask configuration register.

.. table:: 

    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                                                           Description                                                                                           |
    +=====+===+===========+=================================================================================================================================================================================================+
    |31:0 |R/W|PR_MASK_LSB|LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA.|
    +-----+---+-----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // LSB uDMA event dispatch mask configuration register.
                #define SOC_EU_PR_MASK_LSB_OFFSET                0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_pr_mask_lsb_get(uint32_t base);
        static inline void soc_eu_pr_mask_lsb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA. (access: R/W)
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_BIT                           0
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_WIDTH                         32
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_MASK                          0xffffffff
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_RESET                         0xffffffff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_PR_MASK_LSB_PR_MASK_LSB(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pr_mask_lsb     :32; // LSB event mask to enable/disable event dispatch to UDMA peripherals. - Setting bit[i] to 1'b1 disable dispatching event[i] to uDMA. - Setting bit[i] to 1'b0 enable dispatching event[i] to uDMA.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_pr_mask_lsb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_pr_mask_lsb : public vp::reg_32
        {
        public:
            inline void pr_mask_lsb_set(uint32_t value);
            inline uint32_t pr_mask_lsb_get();
        };

|

.. _soc_eu_ERR_MSB:

ERR_MSB
"""""""

MSB event queue overflow status register.

.. table:: 

    +-----+---+-------+--------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                   Description                                                                    |
    +=====+===+=======+==================================================================================================================================================+
    |31:0 |R  |ERR_MSB|Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.|
    +-----+---+-------+--------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MSB event queue overflow status register.
                #define SOC_EU_ERR_MSB_OFFSET                    0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_err_msb_get(uint32_t base);
        static inline void soc_eu_err_msb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue. (access: R)
        #define SOC_EU_ERR_MSB_ERR_MSB_BIT                                   0
        #define SOC_EU_ERR_MSB_ERR_MSB_WIDTH                                 32
        #define SOC_EU_ERR_MSB_ERR_MSB_MASK                                  0xffffffff
        #define SOC_EU_ERR_MSB_ERR_MSB_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_ERR_MSB_ERR_MSB_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_ERR_MSB_ERR_MSB(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int err_msb         :32; // Report MSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_MSB[i] means that an overflow occurred for SoC event[32+i] FIFO queue.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_err_msb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_err_msb : public vp::reg_32
        {
        public:
            inline void err_msb_set(uint32_t value);
            inline uint32_t err_msb_get();
        };

|

.. _soc_eu_ERR_LSB:

ERR_LSB
"""""""

LSB event queue overflow status register.

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                  Description                                                                  |
    +=====+===+=======+===============================================================================================================================================+
    |31:0 |R  |ERR_LSB|Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue.|
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // LSB event queue overflow status register.
                #define SOC_EU_ERR_LSB_OFFSET                    0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_err_lsb_get(uint32_t base);
        static inline void soc_eu_err_lsb_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue. (access: R)
        #define SOC_EU_ERR_LSB_ERR_LSB_BIT                                   0
        #define SOC_EU_ERR_LSB_ERR_LSB_WIDTH                                 32
        #define SOC_EU_ERR_LSB_ERR_LSB_MASK                                  0xffffffff
        #define SOC_EU_ERR_LSB_ERR_LSB_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_ERR_LSB_ERR_LSB_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define SOC_EU_ERR_LSB_ERR_LSB(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int err_lsb         :32; // Report LSB event queue overflows. Cleared after read. Reading a 1'b1 at ERR_LSB[i] means that an overflow occurred for SoC event[i] FIFO queue.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_err_lsb_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_err_lsb : public vp::reg_32
        {
        public:
            inline void err_lsb_set(uint32_t value);
            inline uint32_t err_lsb_get();
        };

|

.. _soc_eu_TIMER1_SEL_HI:

TIMER1_SEL_HI
"""""""""""""

FC High Timer1 source event configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                            Description                                            |
    +=====+===+====+===================================================================================================+
    |7:0  |R/W|EVT |Configure which SoC event generator input event is propagated to FC Timer High trigger event input.|
    +-----+---+----+---------------------------------------------------------------------------------------------------+
    |31   |R/W|ENA |Enable SoC event generator event propagation to FC Timer High trigger event input.                 |
    +-----+---+----+---------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC High Timer1 source event configuration register.
                #define SOC_EU_TIMER1_SEL_HI_OFFSET              0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_timer1_sel_hi_get(uint32_t base);
        static inline void soc_eu_timer1_sel_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_HI_EVT_BIT                                 0
        #define SOC_EU_TIMER1_SEL_HI_EVT_WIDTH                               8
        #define SOC_EU_TIMER1_SEL_HI_EVT_MASK                                0xff
        #define SOC_EU_TIMER1_SEL_HI_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_HI_ENA_BIT                                 31
        #define SOC_EU_TIMER1_SEL_HI_ENA_WIDTH                               1
        #define SOC_EU_TIMER1_SEL_HI_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER1_SEL_HI_ENA_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_TIMER1_SEL_HI_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER1_SEL_HI_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER1_SEL_HI_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER1_SEL_HI_ENA(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer1_sel_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_timer1_sel_hi : public vp::reg_32
        {
        public:
            inline void evt_set(uint32_t value);
            inline uint32_t evt_get();
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|

.. _soc_eu_TIMER1_SEL_LO:

TIMER1_SEL_LO
"""""""""""""

FC Low Timer1 source event configuration register.

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                           Description                                            |
    +=====+===+====+==================================================================================================+
    |7:0  |R/W|EVT |Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.|
    +-----+---+----+--------------------------------------------------------------------------------------------------+
    |31   |R/W|ENA |Enable SoC event generator event propagation to FC Timer Low trigger event input.                 |
    +-----+---+----+--------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC Low Timer1 source event configuration register.
                #define SOC_EU_TIMER1_SEL_LO_OFFSET              0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_timer1_sel_lo_get(uint32_t base);
        static inline void soc_eu_timer1_sel_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_LO_EVT_BIT                                 0
        #define SOC_EU_TIMER1_SEL_LO_EVT_WIDTH                               8
        #define SOC_EU_TIMER1_SEL_LO_EVT_MASK                                0xff
        #define SOC_EU_TIMER1_SEL_LO_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER1_SEL_LO_ENA_BIT                                 31
        #define SOC_EU_TIMER1_SEL_LO_ENA_WIDTH                               1
        #define SOC_EU_TIMER1_SEL_LO_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER1_SEL_LO_ENA_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_TIMER1_SEL_LO_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER1_SEL_LO_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER1_SEL_LO_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER1_SEL_LO_ENA(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer1_sel_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_timer1_sel_lo : public vp::reg_32
        {
        public:
            inline void evt_set(uint32_t value);
            inline uint32_t evt_get();
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|

.. _soc_eu_TIMER2_SEL_HI:

TIMER2_SEL_HI
"""""""""""""

FC High Timer2 source event configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                            Description                                            |
    +=====+===+====+===================================================================================================+
    |7:0  |R/W|EVT |Configure which SoC event generator input event is propagated to FC Timer High trigger event input.|
    +-----+---+----+---------------------------------------------------------------------------------------------------+
    |31   |R/W|ENA |Enable SoC event generator event propagation to FC Timer High trigger event input.                 |
    +-----+---+----+---------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC High Timer2 source event configuration register.
                #define SOC_EU_TIMER2_SEL_HI_OFFSET              0x8c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_timer2_sel_hi_get(uint32_t base);
        static inline void soc_eu_timer2_sel_hi_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Configure which SoC event generator input event is propagated to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_HI_EVT_BIT                                 0
        #define SOC_EU_TIMER2_SEL_HI_EVT_WIDTH                               8
        #define SOC_EU_TIMER2_SEL_HI_EVT_MASK                                0xff
        #define SOC_EU_TIMER2_SEL_HI_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer High trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_HI_ENA_BIT                                 31
        #define SOC_EU_TIMER2_SEL_HI_ENA_WIDTH                               1
        #define SOC_EU_TIMER2_SEL_HI_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER2_SEL_HI_ENA_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_TIMER2_SEL_HI_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER2_SEL_HI_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER2_SEL_HI_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER2_SEL_HI_ENA(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer High trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer High trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer2_sel_hi_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_timer2_sel_hi : public vp::reg_32
        {
        public:
            inline void evt_set(uint32_t value);
            inline uint32_t evt_get();
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|

.. _soc_eu_TIMER2_SEL_LO:

TIMER2_SEL_LO
"""""""""""""

FC Low Timer2 source event configuration register.

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                           Description                                            |
    +=====+===+====+==================================================================================================+
    |7:0  |R/W|EVT |Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.|
    +-----+---+----+--------------------------------------------------------------------------------------------------+
    |31   |R/W|ENA |Enable SoC event generator event propagation to FC Timer Low trigger event input.                 |
    +-----+---+----+--------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC Low Timer2 source event configuration register.
                #define SOC_EU_TIMER2_SEL_LO_OFFSET              0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t soc_eu_timer2_sel_lo_get(uint32_t base);
        static inline void soc_eu_timer2_sel_lo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_LO_EVT_BIT                                 0
        #define SOC_EU_TIMER2_SEL_LO_EVT_WIDTH                               8
        #define SOC_EU_TIMER2_SEL_LO_EVT_MASK                                0xff
        #define SOC_EU_TIMER2_SEL_LO_EVT_RESET                               0x0
        
        // Enable SoC event generator event propagation to FC Timer Low trigger event input. (access: R/W)
        #define SOC_EU_TIMER2_SEL_LO_ENA_BIT                                 31
        #define SOC_EU_TIMER2_SEL_LO_ENA_WIDTH                               1
        #define SOC_EU_TIMER2_SEL_LO_ENA_MASK                                0x80000000
        #define SOC_EU_TIMER2_SEL_LO_ENA_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SOC_EU_TIMER2_SEL_LO_EVT_GET(value)                (GAP_BEXTRACTU((value),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT_GETS(value)               (GAP_BEXTRACT((value),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT_SET(value,field)          (GAP_BINSERT((value),(field),8,0))
        #define SOC_EU_TIMER2_SEL_LO_EVT(val)                      ((val) << 0)
        
        #define SOC_EU_TIMER2_SEL_LO_ENA_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define SOC_EU_TIMER2_SEL_LO_ENA(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt             :8 ; // Configure which SoC event generator input event is propagated to FC Timer Low trigger event input.
            unsigned int padding0:23;
            unsigned int ena             :1 ; // Enable SoC event generator event propagation to FC Timer Low trigger event input.
          };
          unsigned int raw;
        } __attribute__((packed)) soc_eu_timer2_sel_lo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_soc_eu_timer2_sel_lo : public vp::reg_32
        {
        public:
            inline void evt_set(uint32_t value);
            inline uint32_t evt_get();
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|
