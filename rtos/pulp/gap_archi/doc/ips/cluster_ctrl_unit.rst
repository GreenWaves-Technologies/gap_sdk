Input file: docs/IP_REFERENCES/CLUSTER_CRTL_UNIT_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |                                  Name                                   |Offset|Width|                             Description                             |
    +=========================================================================+======+=====+=====================================================================+
    |:ref:`EOC<cluster_ctrl_unit_EOC>`                                        |     0|   32|End Of Computation status register.                                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`FETCH_EN<cluster_ctrl_unit_FETCH_EN>`                              |     8|   32|Cluster cores fetch enable configuration register.                   |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CLOCK_GATE<cluster_ctrl_unit_CLOCK_GATE>`                          |    32|   32|Cluster clock gate configuration register.                           |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_RESUME<cluster_ctrl_unit_DBG_RESUME>`                          |    40|   32|Cluster cores debug resume register.                                 |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_HALT_STATUS<cluster_ctrl_unit_DBG_HALT_STATUS>`                |    40|   32|Cluster cores debug halt status register.                            |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_HALT_MASK<cluster_ctrl_unit_DBG_HALT_MASK>`                    |    56|   32|Cluster cores debug halt mask configuration register.                |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR0<cluster_ctrl_unit_BOOT_ADDR0>`                          |    64|   32|Cluster core 0 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR1<cluster_ctrl_unit_BOOT_ADDR1>`                          |    68|   32|Cluster core 1 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR2<cluster_ctrl_unit_BOOT_ADDR2>`                          |    72|   32|Cluster core 2 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR3<cluster_ctrl_unit_BOOT_ADDR3>`                          |    76|   32|Cluster core 3 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR4<cluster_ctrl_unit_BOOT_ADDR4>`                          |    80|   32|Cluster core 4 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR5<cluster_ctrl_unit_BOOT_ADDR5>`                          |    84|   32|Cluster core 5 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR6<cluster_ctrl_unit_BOOT_ADDR6>`                          |    88|   32|Cluster core 6 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR7<cluster_ctrl_unit_BOOT_ADDR7>`                          |    92|   32|Cluster core 7 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH0<cluster_ctrl_unit_TCDM_ARB_POLICY_CH0>`        |   128|   32|TCDM arbitration policy ch0 for cluster cores configuration register.|
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH1<cluster_ctrl_unit_TCDM_ARB_POLICY_CH1>`        |   136|   32|TCDM arbitration policy ch1 for DMA/HWCE configuration register.     |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH0_REP<cluster_ctrl_unit_TCDM_ARB_POLICY_CH0_REP>`|   192|   32|Read only duplicate of TCDM_ARB_POLICY_CH0 register                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH1_REP<cluster_ctrl_unit_TCDM_ARB_POLICY_CH1_REP>`|   200|   32|Read only duplicate of TCDM_ARB_POLICY_CH1 register                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // End Of Computation status register.
                #define CLUSTER_CTRL_UNIT_EOC_OFFSET             0x0
        
                // Cluster cores fetch enable configuration register.
                #define CLUSTER_CTRL_UNIT_FETCH_EN_OFFSET        0x8
        
                // Cluster clock gate configuration register.
                #define CLUSTER_CTRL_UNIT_CLOCK_GATE_OFFSET      0x20
        
                // Cluster cores debug resume register.
                #define CLUSTER_CTRL_UNIT_DBG_RESUME_OFFSET      0x28
        
                // Cluster cores debug halt status register.
                #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_OFFSET 0x28
        
                // Cluster cores debug halt mask configuration register.
                #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_OFFSET   0x38
        
                // Cluster core 0 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET      0x40
        
                // Cluster core 1 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR1_OFFSET      0x44
        
                // Cluster core 2 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR2_OFFSET      0x48
        
                // Cluster core 3 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR3_OFFSET      0x4c
        
                // Cluster core 4 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR4_OFFSET      0x50
        
                // Cluster core 5 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR5_OFFSET      0x54
        
                // Cluster core 6 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR6_OFFSET      0x58
        
                // Cluster core 7 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR7_OFFSET      0x5c
        
                // TCDM arbitration policy ch0 for cluster cores configuration register.
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_OFFSET 0x80
        
                // TCDM arbitration policy ch1 for DMA/HWCE configuration register.
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_OFFSET 0x88
        
                // Read only duplicate of TCDM_ARB_POLICY_CH0 register
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_OFFSET 0xc0
        
                // Read only duplicate of TCDM_ARB_POLICY_CH1 register
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_OFFSET 0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_eoc_get(uint32_t base);
        static inline void cluster_ctrl_unit_eoc_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_fetch_en_get(uint32_t base);
        static inline void cluster_ctrl_unit_fetch_en_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_clock_gate_get(uint32_t base);
        static inline void cluster_ctrl_unit_clock_gate_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_dbg_resume_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_resume_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_dbg_halt_status_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_halt_status_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_dbg_halt_mask_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_halt_mask_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr0_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr0_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr1_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr1_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr2_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr2_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr3_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr3_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr4_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr4_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr5_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr5_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr6_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr6_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_boot_addr7_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr7_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached (access: R/W)
        #define CLUSTER_CTRL_UNIT_EOC_EOC_BIT                                0
        #define CLUSTER_CTRL_UNIT_EOC_EOC_WIDTH                              1
        #define CLUSTER_CTRL_UNIT_EOC_EOC_MASK                               0x1
        #define CLUSTER_CTRL_UNIT_EOC_EOC_RESET                              0x0
        
        // Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_BIT                         0
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK                        0x1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_RESET                       0x0
        
        // Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_BIT                         1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK                        0x2
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_RESET                       0x0
        
        // Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_BIT                         2
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK                        0x4
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_RESET                       0x0
        
        // Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_BIT                         3
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK                        0x8
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_RESET                       0x0
        
        // Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_BIT                         4
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK                        0x10
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_RESET                       0x0
        
        // Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_BIT                         5
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK                        0x20
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_RESET                       0x0
        
        // Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_BIT                         6
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK                        0x40
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_RESET                       0x0
        
        // Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_BIT                         7
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK                        0x80
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_RESET                       0x0
        
        // Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_BIT                          0
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_WIDTH                        1
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK                         0x1
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_RESET                        0x0
        
        // Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_BIT                       0
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_MASK                      0x1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_RESET                     0x0
        
        // Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_BIT                       1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_MASK                      0x2
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_RESET                     0x0
        
        // Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_BIT                       2
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_MASK                      0x4
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_RESET                     0x0
        
        // Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_BIT                       3
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_MASK                      0x8
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_RESET                     0x0
        
        // Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_BIT                       4
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_MASK                      0x10
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_RESET                     0x0
        
        // Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_BIT                       5
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_MASK                      0x20
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_RESET                     0x0
        
        // Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_BIT                       6
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_MASK                      0x40
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_RESET                     0x0
        
        // Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_BIT                       7
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_MASK                      0x80
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_RESET                     0x0
        
        // Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_BIT                  0
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_MASK                 0x1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_RESET                0x0
        
        // Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_BIT                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_MASK                 0x2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_RESET                0x0
        
        // Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_BIT                  2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_MASK                 0x4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_RESET                0x0
        
        // Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_BIT                  3
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_MASK                 0x8
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_RESET                0x0
        
        // Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_BIT                  4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_MASK                 0x10
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_RESET                0x0
        
        // Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_BIT                  5
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_MASK                 0x20
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_RESET                0x0
        
        // Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_BIT                  6
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_MASK                 0x40
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_RESET                0x0
        
        // Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_BIT                  7
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_MASK                 0x80
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_RESET                0x0
        
        // Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_BIT                    0
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK                   0x1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_RESET                  0x0
        
        // Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_BIT                    1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK                   0x2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_RESET                  0x0
        
        // Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_BIT                    2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK                   0x4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_RESET                  0x0
        
        // Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_BIT                    3
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK                   0x8
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_RESET                  0x0
        
        // Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_BIT                    4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK                   0x10
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_RESET                  0x0
        
        // Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_BIT                    5
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK                   0x20
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_RESET                  0x0
        
        // Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_BIT                    6
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK                   0x40
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_RESET                  0x0
        
        // Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_BIT                    7
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK                   0x80
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_RESET                  0x0
        
        // Cluster core 0 boot address configuration bitfield. (access: R/W)
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_BIT                          0
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_WIDTH                        32
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK                         0xffffffff
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_RESET                        0x0
        
        // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_BIT                0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_WIDTH              1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK               0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_RESET              0x0
        
        // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_BIT                0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_WIDTH              1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK               0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_RESET              0x0
        
        // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_BIT            0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_WIDTH          1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK           0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_RESET          0x0
        
        // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_BIT            0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_WIDTH          1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK           0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_RESET          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_EOC_EOC_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC(val)                     ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0(val)              ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1(val)              ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GET(value)        (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GETS(value)       (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SET(value,field)  (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2(val)              ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GET(value)        (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GETS(value)       (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SET(value,field)  (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3(val)              ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4(val)              ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GET(value)        (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GETS(value)       (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SET(value,field)  (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5(val)              ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GET(value)        (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GETS(value)       (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SET(value,field)  (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6(val)              ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7(val)              ((val) << 7)
        
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GET(value)         (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GETS(value)        (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SET(value,field)   (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(val)               ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0(val)            ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GET(value)      (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GETS(value)     (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1(val)            ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GET(value)      (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GETS(value)     (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2(val)            ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3(val)            ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GET(value)      (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GETS(value)     (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4(val)            ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5(val)            ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6(val)            ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GET(value)      (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GETS(value)     (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7(val)            ((val) << 7)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0(val)       ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1(val)       ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2(val)       ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3(val)       ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GET(value) (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GETS(value) (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4(val)       ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GET(value) (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GETS(value) (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5(val)       ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6(val)       ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GET(value) (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7(val)       ((val) << 7)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GET(value)   (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GETS(value)  (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0(val)         ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GET(value)   (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GETS(value)  (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1(val)         ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GET(value)   (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GETS(value)  (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2(val)         ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GET(value)   (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GETS(value)  (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3(val)         ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GET(value)   (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GETS(value)  (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4(val)         ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GET(value)   (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GETS(value)  (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5(val)         ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GET(value)   (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GETS(value)  (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6(val)         ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GET(value)   (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GETS(value)  (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7(val)         ((val) << 7)
        
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA(val)               ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL(val)     ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL(val)     ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL(val) ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL(val) ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** CLUSTER_CTRL_UNIT_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t eoc;  // End Of Computation status register.
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t fetch_en;  // Cluster cores fetch enable configuration register.
            volatile uint32_t reserved_1[5];  // Reserved/Not used.
            volatile uint32_t clock_gate;  // Cluster clock gate configuration register.
            volatile uint32_t reserved_2[1];  // Reserved/Not used.
            volatile uint32_t dbg_resume;  // Cluster cores debug resume register.
            volatile uint32_t dbg_halt_status;  // Cluster cores debug halt status register.
            volatile uint32_t reserved_3[3];  // Reserved/Not used.
            volatile uint32_t dbg_halt_mask;  // Cluster cores debug halt mask configuration register.
            volatile uint32_t reserved_4[1];  // Reserved/Not used.
            volatile uint32_t boot_addr0;  // Cluster core 0 boot address configuration register.
            volatile uint32_t boot_addr1;  // Cluster core 1 boot address configuration register.
            volatile uint32_t boot_addr2;  // Cluster core 2 boot address configuration register.
            volatile uint32_t boot_addr3;  // Cluster core 3 boot address configuration register.
            volatile uint32_t boot_addr4;  // Cluster core 4 boot address configuration register.
            volatile uint32_t boot_addr5;  // Cluster core 5 boot address configuration register.
            volatile uint32_t boot_addr6;  // Cluster core 6 boot address configuration register.
            volatile uint32_t boot_addr7;  // Cluster core 7 boot address configuration register.
            volatile uint32_t reserved_5[8];  // Reserved/Not used.
            volatile uint32_t tcdm_arb_policy_ch0;  // TCDM arbitration policy ch0 for cluster cores configuration register.
            volatile uint32_t reserved_6[1];  // Reserved/Not used.
            volatile uint32_t tcdm_arb_policy_ch1;  // TCDM arbitration policy ch1 for DMA/HWCE configuration register.
            volatile uint32_t reserved_7[13];  // Reserved/Not used.
            volatile uint32_t tcdm_arb_policy_ch0_rep;  // Read only duplicate of TCDM_ARB_POLICY_CH0 register
            volatile uint32_t reserved_8[1];  // Reserved/Not used.
            volatile uint32_t tcdm_arb_policy_ch1_rep;  // Read only duplicate of TCDM_ARB_POLICY_CH1 register
        } __attribute__((packed)) cluster_ctrl_unit_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int eoc             :1 ; // End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_eoc_t;
        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core1           :1 ; // Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core2           :1 ; // Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core3           :1 ; // Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core4           :1 ; // Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core5           :1 ; // Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core6           :1 ; // Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core7           :1 ; // Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_fetch_en_t;
        
        typedef union {
          struct {
            unsigned int en              :1 ; // Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_clock_gate_t;
        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0
            unsigned int core1           :1 ; // Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1
            unsigned int core2           :1 ; // Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2
            unsigned int core3           :1 ; // Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3
            unsigned int core4           :1 ; // Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4
            unsigned int core5           :1 ; // Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5
            unsigned int core6           :1 ; // Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6
            unsigned int core7           :1 ; // Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_resume_t;
        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core1           :1 ; // Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core2           :1 ; // Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core3           :1 ; // Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core4           :1 ; // Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core5           :1 ; // Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core6           :1 ; // Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core7           :1 ; // Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_halt_status_t;
        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core1           :1 ; // Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core2           :1 ; // Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core3           :1 ; // Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core4           :1 ; // Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core5           :1 ; // Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core6           :1 ; // Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core7           :1 ; // Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_halt_mask_t;
        
        typedef union {
          struct {
            unsigned int ba              :32; // Cluster core 0 boot address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr7_t;
        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_t;
        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_t;
        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_t;
        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_cluster_ctrl_unit : public vp::regmap
        {
        public:
            vp_cluster_ctrl_unit_eoc eoc;
            vp_cluster_ctrl_unit_fetch_en fetch_en;
            vp_cluster_ctrl_unit_clock_gate clock_gate;
            vp_cluster_ctrl_unit_dbg_resume dbg_resume;
            vp_cluster_ctrl_unit_dbg_halt_status dbg_halt_status;
            vp_cluster_ctrl_unit_dbg_halt_mask dbg_halt_mask;
            vp_cluster_ctrl_unit_boot_addr0 boot_addr0;
            vp_cluster_ctrl_unit_boot_addr1 boot_addr1;
            vp_cluster_ctrl_unit_boot_addr2 boot_addr2;
            vp_cluster_ctrl_unit_boot_addr3 boot_addr3;
            vp_cluster_ctrl_unit_boot_addr4 boot_addr4;
            vp_cluster_ctrl_unit_boot_addr5 boot_addr5;
            vp_cluster_ctrl_unit_boot_addr6 boot_addr6;
            vp_cluster_ctrl_unit_boot_addr7 boot_addr7;
            vp_cluster_ctrl_unit_tcdm_arb_policy_ch0 tcdm_arb_policy_ch0;
            vp_cluster_ctrl_unit_tcdm_arb_policy_ch1 tcdm_arb_policy_ch1;
            vp_cluster_ctrl_unit_tcdm_arb_policy_ch0_rep tcdm_arb_policy_ch0_rep;
            vp_cluster_ctrl_unit_tcdm_arb_policy_ch1_rep tcdm_arb_policy_ch1_rep;
        };

|

.. _cluster_ctrl_unit_EOC:

EOC
"""

End Of Computation status register.

.. table:: 

    +-----+---+----+-----------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                   Description                                                   |
    +=====+===+====+=================================================================================================================+
    |    0|R/W|EOC |End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached|
    +-----+---+----+-----------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // End Of Computation status register.
                #define CLUSTER_CTRL_UNIT_EOC_OFFSET             0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_eoc_get(uint32_t base);
        static inline void cluster_ctrl_unit_eoc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached (access: R/W)
        #define CLUSTER_CTRL_UNIT_EOC_EOC_BIT                                0
        #define CLUSTER_CTRL_UNIT_EOC_EOC_WIDTH                              1
        #define CLUSTER_CTRL_UNIT_EOC_EOC_MASK                               0x1
        #define CLUSTER_CTRL_UNIT_EOC_EOC_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_EOC_EOC_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_EOC_EOC(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int eoc             :1 ; // End of computation status flag bitfield: - 1'b0: program execution under going - 1'b1: end of computation reached
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_eoc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_eoc : public vp::reg_32
        {
        public:
            inline void eoc_set(uint32_t value);
            inline uint32_t eoc_get();
        };

|

.. _cluster_ctrl_unit_FETCH_EN:

FETCH_EN
""""""""

Cluster cores fetch enable configuration register.

.. table:: 

    +-----+---+-----+----------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                Description                                 |
    +=====+===+=====+============================================================================+
    |    0|R/W|CORE0|Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    1|R/W|CORE1|Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    2|R/W|CORE2|Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    3|R/W|CORE3|Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    4|R/W|CORE4|Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    5|R/W|CORE5|Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    6|R/W|CORE6|Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+
    |    7|R/W|CORE7|Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+-----+----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster cores fetch enable configuration register.
                #define CLUSTER_CTRL_UNIT_FETCH_EN_OFFSET        0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_fetch_en_get(uint32_t base);
        static inline void cluster_ctrl_unit_fetch_en_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_BIT                         0
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_MASK                        0x1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_RESET                       0x0
        
        // Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_BIT                         1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_MASK                        0x2
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_RESET                       0x0
        
        // Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_BIT                         2
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_MASK                        0x4
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_RESET                       0x0
        
        // Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_BIT                         3
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_MASK                        0x8
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_RESET                       0x0
        
        // Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_BIT                         4
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_MASK                        0x10
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_RESET                       0x0
        
        // Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_BIT                         5
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_MASK                        0x20
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_RESET                       0x0
        
        // Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_BIT                         6
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_MASK                        0x40
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_RESET                       0x0
        
        // Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_BIT                         7
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_WIDTH                       1
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_MASK                        0x80
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE0(val)              ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE1(val)              ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GET(value)        (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_GETS(value)       (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2_SET(value,field)  (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE2(val)              ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GET(value)        (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_GETS(value)       (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3_SET(value,field)  (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE3(val)              ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE4(val)              ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GET(value)        (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_GETS(value)       (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5_SET(value,field)  (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE5(val)              ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GET(value)        (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_GETS(value)       (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6_SET(value,field)  (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE6(val)              ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_FETCH_EN_CORE7(val)              ((val) << 7)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core1           :1 ; // Core 1 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core2           :1 ; // Core 2 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core3           :1 ; // Core 3 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core4           :1 ; // Core 4 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core5           :1 ; // Core 5 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core6           :1 ; // Core 6 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int core7           :1 ; // Core 7 fetch enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_fetch_en_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_fetch_en : public vp::reg_32
        {
        public:
            inline void core0_set(uint32_t value);
            inline uint32_t core0_get();
            inline void core1_set(uint32_t value);
            inline uint32_t core1_get();
            inline void core2_set(uint32_t value);
            inline uint32_t core2_get();
            inline void core3_set(uint32_t value);
            inline uint32_t core3_get();
            inline void core4_set(uint32_t value);
            inline uint32_t core4_get();
            inline void core5_set(uint32_t value);
            inline uint32_t core5_get();
            inline void core6_set(uint32_t value);
            inline uint32_t core6_get();
            inline void core7_set(uint32_t value);
            inline uint32_t core7_get();
        };

|

.. _cluster_ctrl_unit_CLOCK_GATE:

CLOCK_GATE
""""""""""

Cluster clock gate configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                Description                                |
    +=====+===+====+===========================================================================+
    |    0|R/W|EN  |Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+----+---------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster clock gate configuration register.
                #define CLUSTER_CTRL_UNIT_CLOCK_GATE_OFFSET      0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_clock_gate_get(uint32_t base);
        static inline void cluster_ctrl_unit_clock_gate_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_BIT                          0
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_WIDTH                        1
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_MASK                         0x1
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GET(value)         (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_GETS(value)        (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN_SET(value,field)   (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_CLOCK_GATE_EN(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // Cluster clock gate configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_clock_gate_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_clock_gate : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _cluster_ctrl_unit_DBG_RESUME:

DBG_RESUME
""""""""""

Cluster cores debug resume register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |    0|W  |CORE0|Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    1|W  |CORE1|Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    2|W  |CORE2|Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    3|W  |CORE3|Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    4|W  |CORE4|Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    5|W  |CORE5|Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    6|W  |CORE6|Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |    7|W  |CORE7|Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7|
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster cores debug resume register.
                #define CLUSTER_CTRL_UNIT_DBG_RESUME_OFFSET      0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_dbg_resume_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_resume_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_BIT                       0
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_MASK                      0x1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_RESET                     0x0
        
        // Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_BIT                       1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_MASK                      0x2
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_RESET                     0x0
        
        // Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_BIT                       2
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_MASK                      0x4
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_RESET                     0x0
        
        // Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_BIT                       3
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_MASK                      0x8
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_RESET                     0x0
        
        // Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_BIT                       4
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_MASK                      0x10
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_RESET                     0x0
        
        // Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_BIT                       5
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_MASK                      0x20
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_RESET                     0x0
        
        // Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_BIT                       6
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_MASK                      0x40
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_RESET                     0x0
        
        // Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7 (access: W)
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_BIT                       7
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_WIDTH                     1
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_MASK                      0x80
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE0(val)            ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GET(value)      (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_GETS(value)     (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE1(val)            ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GET(value)      (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_GETS(value)     (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE2(val)            ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE3(val)            ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GET(value)      (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_GETS(value)     (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE4(val)            ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE5(val)            ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE6(val)            ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GET(value)      (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_GETS(value)     (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_RESUME_CORE7(val)            ((val) << 7)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 0
            unsigned int core1           :1 ; // Core 1 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 1
            unsigned int core2           :1 ; // Core 2 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 2
            unsigned int core3           :1 ; // Core 3 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 3
            unsigned int core4           :1 ; // Core 4 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 4
            unsigned int core5           :1 ; // Core 5 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 5
            unsigned int core6           :1 ; // Core 6 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 6
            unsigned int core7           :1 ; // Core 7 debug resume configuration bitfield: - 1'b0: stay halted - 1'b1: resume core 7
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_resume_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_dbg_resume : public vp::reg_32
        {
        public:
            inline void core0_set(uint32_t value);
            inline uint32_t core0_get();
            inline void core1_set(uint32_t value);
            inline uint32_t core1_get();
            inline void core2_set(uint32_t value);
            inline uint32_t core2_get();
            inline void core3_set(uint32_t value);
            inline uint32_t core3_get();
            inline void core4_set(uint32_t value);
            inline uint32_t core4_get();
            inline void core5_set(uint32_t value);
            inline uint32_t core5_get();
            inline void core6_set(uint32_t value);
            inline uint32_t core6_get();
            inline void core7_set(uint32_t value);
            inline uint32_t core7_get();
        };

|

.. _cluster_ctrl_unit_DBG_HALT_STATUS:

DBG_HALT_STATUS
"""""""""""""""

Cluster cores debug halt status register.

.. table:: 

    +-----+---+-----+----------------------------------------------------------------------+
    |Bit #|R/W|Name |                             Description                              |
    +=====+===+=====+======================================================================+
    |    0|R  |CORE0|Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    1|R  |CORE1|Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    2|R  |CORE2|Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    3|R  |CORE3|Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    4|R  |CORE4|Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    5|R  |CORE5|Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    6|R  |CORE6|Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+
    |    7|R  |CORE7|Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted|
    +-----+---+-----+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster cores debug halt status register.
                #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_OFFSET 0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_dbg_halt_status_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_halt_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_BIT                  0
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_MASK                 0x1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_RESET                0x0
        
        // Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_BIT                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_MASK                 0x2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_RESET                0x0
        
        // Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_BIT                  2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_MASK                 0x4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_RESET                0x0
        
        // Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_BIT                  3
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_MASK                 0x8
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_RESET                0x0
        
        // Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_BIT                  4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_MASK                 0x10
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_RESET                0x0
        
        // Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_BIT                  5
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_MASK                 0x20
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_RESET                0x0
        
        // Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_BIT                  6
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_MASK                 0x40
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_RESET                0x0
        
        // Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted (access: R)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_BIT                  7
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_WIDTH                1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_MASK                 0x80
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_RESET                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE0(val)       ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE1(val)       ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE2(val)       ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE3(val)       ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GET(value) (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_GETS(value) (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE4(val)       ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GET(value) (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_GETS(value) (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE5(val)       ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE6(val)       ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GET(value) (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_STATUS_CORE7(val)       ((val) << 7)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core1           :1 ; // Core 1 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core2           :1 ; // Core 2 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core3           :1 ; // Core 3 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core4           :1 ; // Core 4 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core5           :1 ; // Core 5 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core6           :1 ; // Core 6 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
            unsigned int core7           :1 ; // Core 7 debug halt status flag bitfield: - 1'b0: running - 1'b1: halted
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_halt_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_dbg_halt_status : public vp::reg_32
        {
        public:
            inline void core0_set(uint32_t value);
            inline uint32_t core0_get();
            inline void core1_set(uint32_t value);
            inline uint32_t core1_get();
            inline void core2_set(uint32_t value);
            inline uint32_t core2_get();
            inline void core3_set(uint32_t value);
            inline uint32_t core3_get();
            inline void core4_set(uint32_t value);
            inline uint32_t core4_get();
            inline void core5_set(uint32_t value);
            inline uint32_t core5_get();
            inline void core6_set(uint32_t value);
            inline uint32_t core6_get();
            inline void core7_set(uint32_t value);
            inline uint32_t core7_get();
        };

|

.. _cluster_ctrl_unit_DBG_HALT_MASK:

DBG_HALT_MASK
"""""""""""""

Cluster cores debug halt mask configuration register.

.. table:: 

    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                               Description                                                               |
    +=====+===+=====+=========================================================================================================================================+
    |    0|R/W|CORE0|Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|CORE1|Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|CORE2|Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|CORE3|Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|CORE4|Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|CORE5|Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|CORE6|Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|CORE7|Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster cores debug halt mask configuration register.
                #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_OFFSET   0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_dbg_halt_mask_get(uint32_t base);
        static inline void cluster_ctrl_unit_dbg_halt_mask_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_BIT                    0
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_MASK                   0x1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_RESET                  0x0
        
        // Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_BIT                    1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_MASK                   0x2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_RESET                  0x0
        
        // Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_BIT                    2
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_MASK                   0x4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_RESET                  0x0
        
        // Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_BIT                    3
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_MASK                   0x8
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_RESET                  0x0
        
        // Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_BIT                    4
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_MASK                   0x10
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_RESET                  0x0
        
        // Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_BIT                    5
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_MASK                   0x20
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_RESET                  0x0
        
        // Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_BIT                    6
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_MASK                   0x40
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_RESET                  0x0
        
        // Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops. (access: R/W)
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_BIT                    7
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_WIDTH                  1
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_MASK                   0x80
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GET(value)   (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_GETS(value)  (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE0(val)         ((val) << 0)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GET(value)   (GAP_BEXTRACTU((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_GETS(value)  (GAP_BEXTRACT((value),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE1(val)         ((val) << 1)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GET(value)   (GAP_BEXTRACTU((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_GETS(value)  (GAP_BEXTRACT((value),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE2(val)         ((val) << 2)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GET(value)   (GAP_BEXTRACTU((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_GETS(value)  (GAP_BEXTRACT((value),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE3(val)         ((val) << 3)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GET(value)   (GAP_BEXTRACTU((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_GETS(value)  (GAP_BEXTRACT((value),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE4(val)         ((val) << 4)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GET(value)   (GAP_BEXTRACTU((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_GETS(value)  (GAP_BEXTRACT((value),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE5(val)         ((val) << 5)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GET(value)   (GAP_BEXTRACTU((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_GETS(value)  (GAP_BEXTRACT((value),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE6(val)         ((val) << 6)
        
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GET(value)   (GAP_BEXTRACTU((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_GETS(value)  (GAP_BEXTRACT((value),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define CLUSTER_CTRL_UNIT_DBG_HALT_MASK_CORE7(val)         ((val) << 7)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core0           :1 ; // Core 0 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core1           :1 ; // Core 1 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core2           :1 ; // Core 2 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core3           :1 ; // Core 3 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core4           :1 ; // Core 4 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core5           :1 ; // Core 5 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core6           :1 ; // Core 6 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
            unsigned int core7           :1 ; // Core 7 debug halt mask bitfield. When bit is set, core will be part of mask group and stopped when one of the members of the group stops.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_dbg_halt_mask_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_dbg_halt_mask : public vp::reg_32
        {
        public:
            inline void core0_set(uint32_t value);
            inline uint32_t core0_get();
            inline void core1_set(uint32_t value);
            inline uint32_t core1_get();
            inline void core2_set(uint32_t value);
            inline uint32_t core2_get();
            inline void core3_set(uint32_t value);
            inline uint32_t core3_get();
            inline void core4_set(uint32_t value);
            inline uint32_t core4_get();
            inline void core5_set(uint32_t value);
            inline uint32_t core5_get();
            inline void core6_set(uint32_t value);
            inline uint32_t core6_get();
            inline void core7_set(uint32_t value);
            inline uint32_t core7_get();
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR0:

BOOT_ADDR0
""""""""""

Cluster core 0 boot address configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------------+
    |Bit #|R/W|Name|                    Description                    |
    +=====+===+====+===================================================+
    |31:0 |R/W|BA  |Cluster core 0 boot address configuration bitfield.|
    +-----+---+----+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster core 0 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_OFFSET      0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr0_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster core 0 boot address configuration bitfield. (access: R/W)
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_BIT                          0
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_WIDTH                        32
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_MASK                         0xffffffff
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define CLUSTER_CTRL_UNIT_BOOT_ADDR0_BA(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ba              :32; // Cluster core 0 boot address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr0 : public vp::reg_32
        {
        public:
            inline void ba_set(uint32_t value);
            inline uint32_t ba_get();
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR1:

BOOT_ADDR1
""""""""""

Cluster core 1 boot address configuration register.

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

        
                // Cluster core 1 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR1_OFFSET      0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr1_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr1 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR2:

BOOT_ADDR2
""""""""""

Cluster core 2 boot address configuration register.

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

        
                // Cluster core 2 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR2_OFFSET      0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr2_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr2 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR3:

BOOT_ADDR3
""""""""""

Cluster core 3 boot address configuration register.

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

        
                // Cluster core 3 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR3_OFFSET      0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr3_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr3_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr3 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR4:

BOOT_ADDR4
""""""""""

Cluster core 4 boot address configuration register.

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

        
                // Cluster core 4 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR4_OFFSET      0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr4_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr4_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr4 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR5:

BOOT_ADDR5
""""""""""

Cluster core 5 boot address configuration register.

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

        
                // Cluster core 5 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR5_OFFSET      0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr5_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr5_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr5 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR6:

BOOT_ADDR6
""""""""""

Cluster core 6 boot address configuration register.

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

        
                // Cluster core 6 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR6_OFFSET      0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr6_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr6_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr6 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_BOOT_ADDR7:

BOOT_ADDR7
""""""""""

Cluster core 7 boot address configuration register.

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

        
                // Cluster core 7 boot address configuration register.
                #define CLUSTER_CTRL_UNIT_BOOT_ADDR7_OFFSET      0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_boot_addr7_get(uint32_t base);
        static inline void cluster_ctrl_unit_boot_addr7_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) cluster_ctrl_unit_boot_addr7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_boot_addr7 : public vp::reg_32
        {
        public:
        };

|

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH0:

TCDM_ARB_POLICY_CH0
"""""""""""""""""""

TCDM arbitration policy ch0 for cluster cores configuration register.

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                 Description                                                  |
    +=====+===+====+==============================================================================================================+
    |    0|R/W|POL |TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order|
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // TCDM arbitration policy ch0 for cluster cores configuration register.
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_OFFSET 0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_BIT                0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_WIDTH              1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_MASK               0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_POL(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_tcdm_arb_policy_ch0 : public vp::reg_32
        {
        public:
            inline void pol_set(uint32_t value);
            inline uint32_t pol_get();
        };

|

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH1:

TCDM_ARB_POLICY_CH1
"""""""""""""""""""

TCDM arbitration policy ch1 for DMA/HWCE configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                               Description                                               |
    +=====+===+====+=========================================================================================================+
    |    0|R/W|POL |TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order|
    +-----+---+----+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // TCDM arbitration policy ch1 for DMA/HWCE configuration register.
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_OFFSET 0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_BIT                0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_WIDTH              1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_MASK               0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_POL(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_tcdm_arb_policy_ch1 : public vp::reg_32
        {
        public:
            inline void pol_set(uint32_t value);
            inline uint32_t pol_get();
        };

|

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH0_REP:

TCDM_ARB_POLICY_CH0_REP
"""""""""""""""""""""""

Read only duplicate of TCDM_ARB_POLICY_CH0 register

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                 Description                                                  |
    +=====+===+====+==============================================================================================================+
    |    0|R/W|POL |TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order|
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Read only duplicate of TCDM_ARB_POLICY_CH0 register
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_OFFSET 0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_BIT            0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_WIDTH          1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_MASK           0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_RESET          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH0_REP_POL(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for cluster cores configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch0_rep_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_tcdm_arb_policy_ch0_rep : public vp::reg_32
        {
        public:
            inline void pol_set(uint32_t value);
            inline uint32_t pol_get();
        };

|

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH1_REP:

TCDM_ARB_POLICY_CH1_REP
"""""""""""""""""""""""

Read only duplicate of TCDM_ARB_POLICY_CH1 register

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                               Description                                               |
    +=====+===+====+=========================================================================================================+
    |    0|R/W|POL |TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order|
    +-----+---+----+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Read only duplicate of TCDM_ARB_POLICY_CH1 register
                #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_OFFSET 0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_get(uint32_t base);
        static inline void cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order (access: R/W)
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_BIT            0
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_WIDTH          1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_MASK           0x1
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_RESET          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_CTRL_UNIT_TCDM_ARB_POLICY_CH1_REP_POL(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pol             :1 ; // TCDM arbitration policy for DMA/HWCE configuration bitfield: - 1'b0: fair round robin - 1'b1: fixed order
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_ctrl_unit_tcdm_arb_policy_ch1_rep_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_ctrl_unit_tcdm_arb_policy_ch1_rep : public vp::reg_32
        {
        public:
            inline void pol_set(uint32_t value);
            inline uint32_t pol_get();
        };

|
