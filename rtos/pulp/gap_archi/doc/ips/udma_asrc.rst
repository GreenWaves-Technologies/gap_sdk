Input file: fe/ips/udma/udma_asrc/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------+------+-----+--------------------------------+
    |                    Name                     |Offset|Width|          Description           |
    +=============================================+======+=====+================================+
    |:ref:`CTRL_CFG_0<udma_asrc_CTRL_CFG_0>`      |     0|   32|Configuration for control block0|
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`CTRL_CFG_1<udma_asrc_CTRL_CFG_1>`      |     4|   32|Configuration for control block1|
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDIN_0<udma_asrc_LANE_IDIN_0>`    |     8|   32|Input stream IDs for lane0      |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDOUT_0<udma_asrc_LANE_IDOUT_0>`  |    12|   32|Output stream IDs for lane0     |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_CFG_0<udma_asrc_LANE_CFG_0>`      |    16|   32|Configuration for lane0         |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDIN_1<udma_asrc_LANE_IDIN_1>`    |    20|   32|Input stream IDs for lane1      |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDOUT_1<udma_asrc_LANE_IDOUT_1>`  |    24|   32|Output stream IDs for lane1     |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_CFG_1<udma_asrc_LANE_CFG_1>`      |    28|   32|Configuration for lane1         |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDIN_2<udma_asrc_LANE_IDIN_2>`    |    32|   32|Input stream IDs for lane2      |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDOUT_2<udma_asrc_LANE_IDOUT_2>`  |    36|   32|Output stream IDs for lane2     |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_CFG_2<udma_asrc_LANE_CFG_2>`      |    40|   32|Configuration for lane2         |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDIN_3<udma_asrc_LANE_IDIN_3>`    |    44|   32|Input stream IDs for lane3      |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_IDOUT_3<udma_asrc_LANE_IDOUT_3>`  |    48|   32|Output stream IDs for lane3     |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`LANE_CFG_3<udma_asrc_LANE_CFG_3>`      |    52|   32|Configuration for lane3         |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`MEM2MEM_ID<udma_asrc_MEM2MEM_ID>`      |    56|   32|Configuration for lane2         |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`MEM2MEM_RATIO<udma_asrc_MEM2MEM_RATIO>`|    60|   32|Input stream IDs for lane3      |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`MEM2MEM_CFG<udma_asrc_MEM2MEM_CFG>`    |    64|   32|Output stream IDs for lane3     |
    +---------------------------------------------+------+-----+--------------------------------+
    |:ref:`STATUS<udma_asrc_STATUS>`              |    68|   32|Global configuration of ASRC    |
    +---------------------------------------------+------+-----+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for control block0
                #define UDMA_ASRC_CTRL_CFG_0_OFFSET              0x0
        
                // Configuration for control block1
                #define UDMA_ASRC_CTRL_CFG_1_OFFSET              0x4
        
                // Input stream IDs for lane0
                #define UDMA_ASRC_LANE_IDIN_0_OFFSET             0x8
        
                // Output stream IDs for lane0
                #define UDMA_ASRC_LANE_IDOUT_0_OFFSET            0xc
        
                // Configuration for lane0
                #define UDMA_ASRC_LANE_CFG_0_OFFSET              0x10
        
                // Input stream IDs for lane1
                #define UDMA_ASRC_LANE_IDIN_1_OFFSET             0x14
        
                // Output stream IDs for lane1
                #define UDMA_ASRC_LANE_IDOUT_1_OFFSET            0x18
        
                // Configuration for lane1
                #define UDMA_ASRC_LANE_CFG_1_OFFSET              0x1c
        
                // Input stream IDs for lane2
                #define UDMA_ASRC_LANE_IDIN_2_OFFSET             0x20
        
                // Output stream IDs for lane2
                #define UDMA_ASRC_LANE_IDOUT_2_OFFSET            0x24
        
                // Configuration for lane2
                #define UDMA_ASRC_LANE_CFG_2_OFFSET              0x28
        
                // Input stream IDs for lane3
                #define UDMA_ASRC_LANE_IDIN_3_OFFSET             0x2c
        
                // Output stream IDs for lane3
                #define UDMA_ASRC_LANE_IDOUT_3_OFFSET            0x30
        
                // Configuration for lane3
                #define UDMA_ASRC_LANE_CFG_3_OFFSET              0x34
        
                // Configuration for lane2
                #define UDMA_ASRC_MEM2MEM_ID_OFFSET              0x38
        
                // Input stream IDs for lane3
                #define UDMA_ASRC_MEM2MEM_RATIO_OFFSET           0x3c
        
                // Output stream IDs for lane3
                #define UDMA_ASRC_MEM2MEM_CFG_OFFSET             0x40
        
                // Global configuration of ASRC
                #define UDMA_ASRC_STATUS_OFFSET                  0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_ctrl_cfg_0_get(uint32_t base);
        static inline void udma_asrc_ctrl_cfg_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_ctrl_cfg_1_get(uint32_t base);
        static inline void udma_asrc_ctrl_cfg_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idin_0_get(uint32_t base);
        static inline void udma_asrc_lane_idin_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idout_0_get(uint32_t base);
        static inline void udma_asrc_lane_idout_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_cfg_0_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idin_1_get(uint32_t base);
        static inline void udma_asrc_lane_idin_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idout_1_get(uint32_t base);
        static inline void udma_asrc_lane_idout_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_cfg_1_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idin_2_get(uint32_t base);
        static inline void udma_asrc_lane_idin_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idout_2_get(uint32_t base);
        static inline void udma_asrc_lane_idout_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_cfg_2_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idin_3_get(uint32_t base);
        static inline void udma_asrc_lane_idin_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_idout_3_get(uint32_t base);
        static inline void udma_asrc_lane_idout_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_lane_cfg_3_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_mem2mem_id_get(uint32_t base);
        static inline void udma_asrc_mem2mem_id_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_mem2mem_ratio_get(uint32_t base);
        static inline void udma_asrc_mem2mem_ratio_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_mem2mem_cfg_get(uint32_t base);
        static inline void udma_asrc_mem2mem_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_asrc_status_get(uint32_t base);
        static inline void udma_asrc_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_BIT                              0
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_RESET                            0x0
        
        // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_BIT                                1
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_WIDTH                              1
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_MASK                               0x2
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_RESET                              0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_BIT                               2
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_WIDTH                             3
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_MASK                              0x1c
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_RESET                             0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_BIT                              5
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_WIDTH                            3
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_MASK                             0xe0
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_RESET                            0x0
        
        // Sets how many ratio calc cycles the error should be below threshold before releasing the lock (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_BIT                            8
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_WIDTH                          3
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_MASK                           0x700
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_RESET                          0x7
        
        // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_BIT                              0
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_RESET                            0x0
        
        // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_BIT                                1
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_WIDTH                              1
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_MASK                               0x2
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_RESET                              0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_BIT                               2
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_WIDTH                             3
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_MASK                              0x1c
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_RESET                             0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_BIT                              5
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_WIDTH                            3
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_MASK                             0xe0
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_RESET                            0x0
        
        // Sets how many ratio calc cycles the error should be below threshold before releasing the lock (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_BIT                            8
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_WIDTH                          3
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_MASK                           0x700
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_RESET                          0x7
        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_RESET                           0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_RESET                          0xff
        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_0_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_0_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_0_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_RESET                      0x1
        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_RESET                           0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_RESET                          0xff
        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_1_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_1_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_1_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_RESET                      0x1
        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_RESET                           0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_RESET                          0xff
        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_2_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_2_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_2_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_RESET                      0x1
        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_RESET                           0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_RESET                          0xff
        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_3_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_3_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_3_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_RESET                      0x1
        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_BIT                          0
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_WIDTH                        8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_MASK                         0xff
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_RESET                        0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_BIT                          8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_WIDTH                        8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_MASK                         0xff00
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_RESET                        0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_BIT                         16
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_WIDTH                       8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_MASK                        0xff0000
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_RESET                       0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_BIT                         24
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_WIDTH                       8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_MASK                        0xff000000
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_RESET                       0xff
        
        // Forces specific ratio (access: R/W)
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_BIT                     0
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_WIDTH                   1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_MASK                    0x1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_RESET                   0x0
        
        // Ratio's value (access: R/W)
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_BIT                        1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_WIDTH                      26
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_MASK                       0x7fffffe
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_RESET                      0x0
        
        // Clock Enable for 2 channel mem2mem block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_BIT                             0
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_WIDTH                           1
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_MASK                            0x1
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_RESET                           0x0
        
        // Reset value for 2 channel mem2mem block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_BIT                               1
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_WIDTH                             1
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_MASK                              0x2
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_RESET                             0x0
        
        // Enables single channels in a mem2mem block (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_BIT                              2
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_WIDTH                            2
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_MASK                             0xc
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_RESET                            0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_BIT                              4
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_WIDTH                            3
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_MASK                             0x70
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_RESET                            0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_BIT                             7
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_WIDTH                           3
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_MASK                            0x380
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_RESET                           0x0
        
        // Stores the filter state to memory (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_BIT                              10
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_WIDTH                            1
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_MASK                             0x400
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_RESET                            0x0
        
        // Restore the filter state from memory (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_BIT                            11
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_WIDTH                          1
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_MASK                           0x800
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_RESET                          0x0
        
        // Stream ID used for store/restore (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_BIT                             16
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_WIDTH                           8
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_MASK                            0xff0000
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_RESET                           0xff
        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_BIT                             0
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_WIDTH                           1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_MASK                            0x1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_RESET                           0x0
        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_BIT                             1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_WIDTH                           1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_MASK                            0x2
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_RESET                           0x0
        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_M2M_BIT                                2
        #define UDMA_ASRC_STATUS_LOCK_M2M_WIDTH                              1
        #define UDMA_ASRC_STATUS_LOCK_M2M_MASK                               0x4
        #define UDMA_ASRC_STATUS_LOCK_M2M_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_GET(value)              (GAP_BEXTRACTU((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_GETS(value)             (GAP_BEXTRACT((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_SET(value,field)        (GAP_BINSERT((value),(field),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN(val)                    ((val) << 2)
        
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_GET(value)             (GAP_BEXTRACTU((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_GETS(value)            (GAP_BEXTRACT((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_SET(value,field)       (GAP_BINSERT((value),(field),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT(val)                   ((val) << 5)
        
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_GET(value)           (GAP_BEXTRACTU((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_GETS(value)          (GAP_BEXTRACT((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_SET(value,field)     (GAP_BINSERT((value),(field),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND(val)                 ((val) << 8)
        
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_GET(value)              (GAP_BEXTRACTU((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_GETS(value)             (GAP_BEXTRACT((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_SET(value,field)        (GAP_BINSERT((value),(field),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN(val)                    ((val) << 2)
        
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_GET(value)             (GAP_BEXTRACTU((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_GETS(value)            (GAP_BEXTRACT((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_SET(value,field)       (GAP_BINSERT((value),(field),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT(val)                   ((val) << 5)
        
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_GET(value)           (GAP_BEXTRACTU((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_GETS(value)          (GAP_BEXTRACT((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_SET(value,field)     (GAP_BINSERT((value),(field),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3(val)                  ((val) << 24)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3(val)                 ((val) << 24)
        
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_0_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT(val)             ((val) << 18)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3(val)                  ((val) << 24)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3(val)                 ((val) << 24)
        
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_1_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT(val)             ((val) << 18)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3(val)                  ((val) << 24)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3(val)                 ((val) << 24)
        
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_2_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT(val)             ((val) << 18)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3(val)                  ((val) << 24)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3(val)                 ((val) << 24)
        
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_3_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT(val)             ((val) << 18)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0(val)               ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_GET(value)         (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_GETS(value)        (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_SET(value,field)   (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1(val)               ((val) << 8)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_GET(value)        (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_GETS(value)       (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_SET(value,field)  (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0(val)              ((val) << 16)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_GET(value)        (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_GETS(value)       (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_SET(value,field)  (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1(val)              ((val) << 24)
        
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_GET(value)    (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_GETS(value)   (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN(val)          ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_GET(value)       (GAP_BEXTRACTU((value),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_GETS(value)      (GAP_BEXTRACT((value),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_SET(value,field) (GAP_BINSERT((value),(field),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO(val)             ((val) << 1)
        
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN(val)                  ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN(val)                    ((val) << 1)
        
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_GET(value)             (GAP_BEXTRACTU((value),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_GETS(value)            (GAP_BEXTRACT((value),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_SET(value,field)       (GAP_BINSERT((value),(field),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN(val)                   ((val) << 2)
        
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_GET(value)             (GAP_BEXTRACTU((value),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_GETS(value)            (GAP_BEXTRACT((value),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_SET(value,field)       (GAP_BINSERT((value),(field),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN(val)                   ((val) << 4)
        
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_GET(value)            (GAP_BEXTRACTU((value),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_GETS(value)           (GAP_BEXTRACT((value),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_SET(value,field)      (GAP_BINSERT((value),(field),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT(val)                  ((val) << 7)
        
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_GET(value)             (GAP_BEXTRACTU((value),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_GETS(value)            (GAP_BEXTRACT((value),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_SET(value,field)       (GAP_BINSERT((value),(field),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE(val)                   ((val) << 10)
        
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_GET(value)           (GAP_BEXTRACTU((value),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_GETS(value)          (GAP_BEXTRACT((value),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_SET(value,field)     (GAP_BINSERT((value),(field),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE(val)                 ((val) << 11)
        
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID(val)                  ((val) << 16)
        
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1(val)                  ((val) << 1)
        
        #define UDMA_ASRC_STATUS_LOCK_M2M_GET(value)               (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M_GETS(value)              (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M_SET(value,field)         (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M(val)                     ((val) << 2)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_ASRC_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t ctrl_cfg_0;  // Configuration for control block0
            volatile uint32_t ctrl_cfg_1;  // Configuration for control block1
            volatile uint32_t lane_idin_0;  // Input stream IDs for lane0
            volatile uint32_t lane_idout_0;  // Output stream IDs for lane0
            volatile uint32_t lane_cfg_0;  // Configuration for lane0
            volatile uint32_t lane_idin_1;  // Input stream IDs for lane1
            volatile uint32_t lane_idout_1;  // Output stream IDs for lane1
            volatile uint32_t lane_cfg_1;  // Configuration for lane1
            volatile uint32_t lane_idin_2;  // Input stream IDs for lane2
            volatile uint32_t lane_idout_2;  // Output stream IDs for lane2
            volatile uint32_t lane_cfg_2;  // Configuration for lane2
            volatile uint32_t lane_idin_3;  // Input stream IDs for lane3
            volatile uint32_t lane_idout_3;  // Output stream IDs for lane3
            volatile uint32_t lane_cfg_3;  // Configuration for lane3
            volatile uint32_t mem2mem_id;  // Configuration for lane2
            volatile uint32_t mem2mem_ratio;  // Input stream IDs for lane3
            volatile uint32_t mem2mem_cfg;  // Output stream IDs for lane3
            volatile uint32_t status;  // Global configuration of ASRC
        } __attribute__((packed)) udma_asrc_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int lock_wnd        :3 ; // Sets how many ratio calc cycles the error should be below threshold before releasing the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_ctrl_cfg_0_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int lock_wnd        :3 ; // Sets how many ratio calc cycles the error should be below threshold before releasing the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_ctrl_cfg_1_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_0_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_0_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_0_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_1_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_1_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_1_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_2_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_2_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_2_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_3_t;
        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_3_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_3_t;
        
        typedef union {
          struct {
            unsigned int m2m_in_ch0      :8 ; // Stream ID for input channel0
            unsigned int m2m_in_ch1      :8 ; // Stream ID for input channel1
            unsigned int m2m_out_ch0     :8 ; // Stream ID for output channel0
            unsigned int m2m_out_ch1     :8 ; // Stream ID for output channel1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_id_t;
        
        typedef union {
          struct {
            unsigned int m2m_ratio_en    :1 ; // Forces specific ratio
            unsigned int m2m_ratio       :26; // Ratio's value
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_ratio_t;
        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 2 channel mem2mem block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 2 channel mem2mem block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ch_en           :2 ; // Enables single channels in a mem2mem block
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int store           :1 ; // Stores the filter state to memory
            unsigned int restore         :1 ; // Restore the filter state from memory
            unsigned int padding0:4 ;
            unsigned int ctx_id          :8 ; // Stream ID used for store/restore
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_cfg_t;
        
        typedef union {
          struct {
            unsigned int lock_ctrl_0     :1 ; // Status of the lock
            unsigned int lock_ctrl_1     :1 ; // Status of the lock
            unsigned int lock_m2m        :1 ; // Status of the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_asrc : public vp::regmap
        {
        public:
            vp_udma_asrc_ctrl_cfg_0 ctrl_cfg_0;
            vp_udma_asrc_ctrl_cfg_1 ctrl_cfg_1;
            vp_udma_asrc_lane_idin_0 lane_idin_0;
            vp_udma_asrc_lane_idout_0 lane_idout_0;
            vp_udma_asrc_lane_cfg_0 lane_cfg_0;
            vp_udma_asrc_lane_idin_1 lane_idin_1;
            vp_udma_asrc_lane_idout_1 lane_idout_1;
            vp_udma_asrc_lane_cfg_1 lane_cfg_1;
            vp_udma_asrc_lane_idin_2 lane_idin_2;
            vp_udma_asrc_lane_idout_2 lane_idout_2;
            vp_udma_asrc_lane_cfg_2 lane_cfg_2;
            vp_udma_asrc_lane_idin_3 lane_idin_3;
            vp_udma_asrc_lane_idout_3 lane_idout_3;
            vp_udma_asrc_lane_cfg_3 lane_cfg_3;
            vp_udma_asrc_mem2mem_id mem2mem_id;
            vp_udma_asrc_mem2mem_ratio mem2mem_ratio;
            vp_udma_asrc_mem2mem_cfg mem2mem_cfg;
            vp_udma_asrc_status status;
        };

|

.. _udma_asrc_CTRL_CFG_0:

CTRL_CFG_0
""""""""""

Configuration for control block0

.. table:: 

    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                         Description                                         |
    +=====+===+========+=============================================================================================+
    |    0|R/W|CLK_EN  |Clock Enable for controll block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled        |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN    |Reset value for control block - 1’b0 : Reset        - 1’b1 : No Reset                        |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |4:2  |R/W|FS_IN   |Selects input sampling frequency                                                             |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |7:5  |R/W|FS_OUT  |Selects output sampling frequency                                                            |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |10:8 |R/W|LOCK_WND|Sets how many ratio calc cycles the error should be below threshold before releasing the lock|
    +-----+---+--------+---------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for control block0
                #define UDMA_ASRC_CTRL_CFG_0_OFFSET              0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_ctrl_cfg_0_get(uint32_t base);
        static inline void udma_asrc_ctrl_cfg_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_BIT                              0
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_RESET                            0x0
        
        // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_BIT                                1
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_WIDTH                              1
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_MASK                               0x2
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_RESET                              0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_BIT                               2
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_WIDTH                             3
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_MASK                              0x1c
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_RESET                             0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_BIT                              5
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_WIDTH                            3
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_MASK                             0xe0
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_RESET                            0x0
        
        // Sets how many ratio calc cycles the error should be below threshold before releasing the lock (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_BIT                            8
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_WIDTH                          3
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_MASK                           0x700
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_RESET                          0x7

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_CTRL_CFG_0_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_CTRL_CFG_0_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_GET(value)              (GAP_BEXTRACTU((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_GETS(value)             (GAP_BEXTRACT((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN_SET(value,field)        (GAP_BINSERT((value),(field),3,2))
        #define UDMA_ASRC_CTRL_CFG_0_FS_IN(val)                    ((val) << 2)
        
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_GET(value)             (GAP_BEXTRACTU((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_GETS(value)            (GAP_BEXTRACT((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT_SET(value,field)       (GAP_BINSERT((value),(field),3,5))
        #define UDMA_ASRC_CTRL_CFG_0_FS_OUT(val)                   ((val) << 5)
        
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_GET(value)           (GAP_BEXTRACTU((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_GETS(value)          (GAP_BEXTRACT((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND_SET(value,field)     (GAP_BINSERT((value),(field),3,8))
        #define UDMA_ASRC_CTRL_CFG_0_LOCK_WND(val)                 ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int lock_wnd        :3 ; // Sets how many ratio calc cycles the error should be below threshold before releasing the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_ctrl_cfg_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_ctrl_cfg_0 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void fs_in_set(uint32_t value);
            inline uint32_t fs_in_get();
            inline void fs_out_set(uint32_t value);
            inline uint32_t fs_out_get();
            inline void lock_wnd_set(uint32_t value);
            inline uint32_t lock_wnd_get();
        };

|

.. _udma_asrc_CTRL_CFG_1:

CTRL_CFG_1
""""""""""

Configuration for control block1

.. table:: 

    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                         Description                                         |
    +=====+===+========+=============================================================================================+
    |    0|R/W|CLK_EN  |Clock Enable for controll block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled        |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN    |Reset value for control block - 1’b0 : Reset        - 1’b1 : No Reset                        |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |4:2  |R/W|FS_IN   |Selects input sampling frequency                                                             |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |7:5  |R/W|FS_OUT  |Selects output sampling frequency                                                            |
    +-----+---+--------+---------------------------------------------------------------------------------------------+
    |10:8 |R/W|LOCK_WND|Sets how many ratio calc cycles the error should be below threshold before releasing the lock|
    +-----+---+--------+---------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for control block1
                #define UDMA_ASRC_CTRL_CFG_1_OFFSET              0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_ctrl_cfg_1_get(uint32_t base);
        static inline void udma_asrc_ctrl_cfg_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_BIT                              0
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_RESET                            0x0
        
        // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_BIT                                1
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_WIDTH                              1
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_MASK                               0x2
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_RESET                              0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_BIT                               2
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_WIDTH                             3
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_MASK                              0x1c
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_RESET                             0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_BIT                              5
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_WIDTH                            3
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_MASK                             0xe0
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_RESET                            0x0
        
        // Sets how many ratio calc cycles the error should be below threshold before releasing the lock (access: R/W)
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_BIT                            8
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_WIDTH                          3
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_MASK                           0x700
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_RESET                          0x7

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_CTRL_CFG_1_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_CTRL_CFG_1_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_GET(value)              (GAP_BEXTRACTU((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_GETS(value)             (GAP_BEXTRACT((value),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN_SET(value,field)        (GAP_BINSERT((value),(field),3,2))
        #define UDMA_ASRC_CTRL_CFG_1_FS_IN(val)                    ((val) << 2)
        
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_GET(value)             (GAP_BEXTRACTU((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_GETS(value)            (GAP_BEXTRACT((value),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT_SET(value,field)       (GAP_BINSERT((value),(field),3,5))
        #define UDMA_ASRC_CTRL_CFG_1_FS_OUT(val)                   ((val) << 5)
        
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_GET(value)           (GAP_BEXTRACTU((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_GETS(value)          (GAP_BEXTRACT((value),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND_SET(value,field)     (GAP_BINSERT((value),(field),3,8))
        #define UDMA_ASRC_CTRL_CFG_1_LOCK_WND(val)                 ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for controll block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for control block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int lock_wnd        :3 ; // Sets how many ratio calc cycles the error should be below threshold before releasing the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_ctrl_cfg_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_ctrl_cfg_1 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void fs_in_set(uint32_t value);
            inline uint32_t fs_in_get();
            inline void fs_out_set(uint32_t value);
            inline uint32_t fs_out_get();
            inline void lock_wnd_set(uint32_t value);
            inline uint32_t lock_wnd_get();
        };

|

.. _udma_asrc_LANE_IDIN_0:

LANE_IDIN_0
"""""""""""

Input stream IDs for lane0

.. table:: 

    +-----+---+------+----------------------------+
    |Bit #|R/W| Name |        Description         |
    +=====+===+======+============================+
    |7:0  |R/W|ID_CH0|Stream ID for input channel0|
    +-----+---+------+----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for input channel1|
    +-----+---+------+----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for input channel2|
    +-----+---+------+----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for input channel3|
    +-----+---+------+----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Input stream IDs for lane0
                #define UDMA_ASRC_LANE_IDIN_0_OFFSET             0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idin_0_get(uint32_t base);
        static inline void udma_asrc_lane_idin_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_RESET                           0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_0_ID_CH3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idin_0 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_IDOUT_0:

LANE_IDOUT_0
""""""""""""

Output stream IDs for lane0

.. table:: 

    +-----+---+------+-----------------------------+
    |Bit #|R/W| Name |         Description         |
    +=====+===+======+=============================+
    |7:0  |R/W|ID_CH0|Stream ID for output channel0|
    +-----+---+------+-----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for output channel1|
    +-----+---+------+-----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for output channel2|
    +-----+---+------+-----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for output channel3|
    +-----+---+------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Output stream IDs for lane0
                #define UDMA_ASRC_LANE_IDOUT_0_OFFSET            0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idout_0_get(uint32_t base);
        static inline void udma_asrc_lane_idout_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_RESET                          0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_0_ID_CH3(val)                 ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idout_0 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_CFG_0:

LANE_CFG_0
""""""""""

Configuration for lane0

.. table:: 

    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                              Description                                              |
    +=====+===+==============+=======================================================================================================+
    |    0|R/W|CLK_EN        |Clock Enable for 4 channel lane block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled            |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN          |Reset value for 4 channel lane block - 1’b0 : Reset        - 1’b1 : No Reset                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    2|R/W|CTRL_MUX      |Select asrc controller - 1’b0 : Use controller0        - 1’b1 : Use controller1                        |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|USE_STREAM_IN |Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|USE_STREAM_OUT|Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |15:12|R/W|CH_EN         |Enables single channels in a 4 channel block                                                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   16|R/W|WAIT_LOCK_IN  |Stalls input samples until lock goes high  - 1’b0 : Allows input samples  - 1’b1 : Halts input samples |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   17|R/W|WAIT_LOCK_OUT |Stalls output samples until lock goes high - 1’b0 : Allows output samples - 1’b1 : Halts output samples|
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   18|R/W|DROP_ON_WAIT  |Drops input packets when WAIT_LOCK=0 and force 0 at the input                                          |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for lane0
                #define UDMA_ASRC_LANE_CFG_0_OFFSET              0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_cfg_0_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_0_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_0_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_0_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_RESET                      0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_0_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_0_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_0_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_0_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_0_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_0_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_0_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_0_DROP_ON_WAIT(val)             ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_cfg_0 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void ctrl_mux_set(uint32_t value);
            inline uint32_t ctrl_mux_get();
            inline void use_stream_in_set(uint32_t value);
            inline uint32_t use_stream_in_get();
            inline void use_stream_out_set(uint32_t value);
            inline uint32_t use_stream_out_get();
            inline void ch_en_set(uint32_t value);
            inline uint32_t ch_en_get();
            inline void wait_lock_in_set(uint32_t value);
            inline uint32_t wait_lock_in_get();
            inline void wait_lock_out_set(uint32_t value);
            inline uint32_t wait_lock_out_get();
            inline void drop_on_wait_set(uint32_t value);
            inline uint32_t drop_on_wait_get();
        };

|

.. _udma_asrc_LANE_IDIN_1:

LANE_IDIN_1
"""""""""""

Input stream IDs for lane1

.. table:: 

    +-----+---+------+----------------------------+
    |Bit #|R/W| Name |        Description         |
    +=====+===+======+============================+
    |7:0  |R/W|ID_CH0|Stream ID for input channel0|
    +-----+---+------+----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for input channel1|
    +-----+---+------+----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for input channel2|
    +-----+---+------+----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for input channel3|
    +-----+---+------+----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Input stream IDs for lane1
                #define UDMA_ASRC_LANE_IDIN_1_OFFSET             0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idin_1_get(uint32_t base);
        static inline void udma_asrc_lane_idin_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_RESET                           0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_1_ID_CH3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idin_1 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_IDOUT_1:

LANE_IDOUT_1
""""""""""""

Output stream IDs for lane1

.. table:: 

    +-----+---+------+-----------------------------+
    |Bit #|R/W| Name |         Description         |
    +=====+===+======+=============================+
    |7:0  |R/W|ID_CH0|Stream ID for output channel0|
    +-----+---+------+-----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for output channel1|
    +-----+---+------+-----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for output channel2|
    +-----+---+------+-----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for output channel3|
    +-----+---+------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Output stream IDs for lane1
                #define UDMA_ASRC_LANE_IDOUT_1_OFFSET            0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idout_1_get(uint32_t base);
        static inline void udma_asrc_lane_idout_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_RESET                          0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_1_ID_CH3(val)                 ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idout_1 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_CFG_1:

LANE_CFG_1
""""""""""

Configuration for lane1

.. table:: 

    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                              Description                                              |
    +=====+===+==============+=======================================================================================================+
    |    0|R/W|CLK_EN        |Clock Enable for 4 channel lane block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled            |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN          |Reset value for 4 channel lane block - 1’b0 : Reset        - 1’b1 : No Reset                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    2|R/W|CTRL_MUX      |Select asrc controller - 1’b0 : Use controller0        - 1’b1 : Use controller1                        |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|USE_STREAM_IN |Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|USE_STREAM_OUT|Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |15:12|R/W|CH_EN         |Enables single channels in a 4 channel block                                                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   16|R/W|WAIT_LOCK_IN  |Stalls input samples until lock goes high  - 1’b0 : Allows input samples  - 1’b1 : Halts input samples |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   17|R/W|WAIT_LOCK_OUT |Stalls output samples until lock goes high - 1’b0 : Allows output samples - 1’b1 : Halts output samples|
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   18|R/W|DROP_ON_WAIT  |Drops input packets when WAIT_LOCK=0 and force 0 at the input                                          |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for lane1
                #define UDMA_ASRC_LANE_CFG_1_OFFSET              0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_cfg_1_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_1_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_1_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_1_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_RESET                      0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_1_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_1_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_1_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_1_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_1_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_1_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_1_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_1_DROP_ON_WAIT(val)             ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_cfg_1 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void ctrl_mux_set(uint32_t value);
            inline uint32_t ctrl_mux_get();
            inline void use_stream_in_set(uint32_t value);
            inline uint32_t use_stream_in_get();
            inline void use_stream_out_set(uint32_t value);
            inline uint32_t use_stream_out_get();
            inline void ch_en_set(uint32_t value);
            inline uint32_t ch_en_get();
            inline void wait_lock_in_set(uint32_t value);
            inline uint32_t wait_lock_in_get();
            inline void wait_lock_out_set(uint32_t value);
            inline uint32_t wait_lock_out_get();
            inline void drop_on_wait_set(uint32_t value);
            inline uint32_t drop_on_wait_get();
        };

|

.. _udma_asrc_LANE_IDIN_2:

LANE_IDIN_2
"""""""""""

Input stream IDs for lane2

.. table:: 

    +-----+---+------+----------------------------+
    |Bit #|R/W| Name |        Description         |
    +=====+===+======+============================+
    |7:0  |R/W|ID_CH0|Stream ID for input channel0|
    +-----+---+------+----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for input channel1|
    +-----+---+------+----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for input channel2|
    +-----+---+------+----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for input channel3|
    +-----+---+------+----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Input stream IDs for lane2
                #define UDMA_ASRC_LANE_IDIN_2_OFFSET             0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idin_2_get(uint32_t base);
        static inline void udma_asrc_lane_idin_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_RESET                           0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_2_ID_CH3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idin_2 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_IDOUT_2:

LANE_IDOUT_2
""""""""""""

Output stream IDs for lane2

.. table:: 

    +-----+---+------+-----------------------------+
    |Bit #|R/W| Name |         Description         |
    +=====+===+======+=============================+
    |7:0  |R/W|ID_CH0|Stream ID for output channel0|
    +-----+---+------+-----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for output channel1|
    +-----+---+------+-----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for output channel2|
    +-----+---+------+-----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for output channel3|
    +-----+---+------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Output stream IDs for lane2
                #define UDMA_ASRC_LANE_IDOUT_2_OFFSET            0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idout_2_get(uint32_t base);
        static inline void udma_asrc_lane_idout_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_RESET                          0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_2_ID_CH3(val)                 ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idout_2 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_CFG_2:

LANE_CFG_2
""""""""""

Configuration for lane2

.. table:: 

    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                              Description                                              |
    +=====+===+==============+=======================================================================================================+
    |    0|R/W|CLK_EN        |Clock Enable for 4 channel lane block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled            |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN          |Reset value for 4 channel lane block - 1’b0 : Reset        - 1’b1 : No Reset                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    2|R/W|CTRL_MUX      |Select asrc controller - 1’b0 : Use controller0        - 1’b1 : Use controller1                        |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|USE_STREAM_IN |Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|USE_STREAM_OUT|Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |15:12|R/W|CH_EN         |Enables single channels in a 4 channel block                                                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   16|R/W|WAIT_LOCK_IN  |Stalls input samples until lock goes high  - 1’b0 : Allows input samples  - 1’b1 : Halts input samples |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   17|R/W|WAIT_LOCK_OUT |Stalls output samples until lock goes high - 1’b0 : Allows output samples - 1’b1 : Halts output samples|
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   18|R/W|DROP_ON_WAIT  |Drops input packets when WAIT_LOCK=0 and force 0 at the input                                          |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for lane2
                #define UDMA_ASRC_LANE_CFG_2_OFFSET              0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_cfg_2_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_2_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_2_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_2_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_RESET                      0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_2_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_2_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_2_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_2_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_2_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_2_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_2_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_2_DROP_ON_WAIT(val)             ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_cfg_2 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void ctrl_mux_set(uint32_t value);
            inline uint32_t ctrl_mux_get();
            inline void use_stream_in_set(uint32_t value);
            inline uint32_t use_stream_in_get();
            inline void use_stream_out_set(uint32_t value);
            inline uint32_t use_stream_out_get();
            inline void ch_en_set(uint32_t value);
            inline uint32_t ch_en_get();
            inline void wait_lock_in_set(uint32_t value);
            inline uint32_t wait_lock_in_get();
            inline void wait_lock_out_set(uint32_t value);
            inline uint32_t wait_lock_out_get();
            inline void drop_on_wait_set(uint32_t value);
            inline uint32_t drop_on_wait_get();
        };

|

.. _udma_asrc_LANE_IDIN_3:

LANE_IDIN_3
"""""""""""

Input stream IDs for lane3

.. table:: 

    +-----+---+------+----------------------------+
    |Bit #|R/W| Name |        Description         |
    +=====+===+======+============================+
    |7:0  |R/W|ID_CH0|Stream ID for input channel0|
    +-----+---+------+----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for input channel1|
    +-----+---+------+----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for input channel2|
    +-----+---+------+----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for input channel3|
    +-----+---+------+----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Input stream IDs for lane3
                #define UDMA_ASRC_LANE_IDIN_3_OFFSET             0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idin_3_get(uint32_t base);
        static inline void udma_asrc_lane_idin_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_BIT                             0
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_MASK                            0xff
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_RESET                           0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_BIT                             8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_MASK                            0xff00
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_RESET                           0xff
        
        // Stream ID for input channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_BIT                             16
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_MASK                            0xff0000
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_RESET                           0xff
        
        // Stream ID for input channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_BIT                             24
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_WIDTH                           8
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_MASK                            0xff000000
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_RESET                           0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH1(val)                  ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH2(val)                  ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDIN_3_ID_CH3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for input channel0
            unsigned int id_ch1          :8 ; // Stream ID for input channel1
            unsigned int id_ch2          :8 ; // Stream ID for input channel2
            unsigned int id_ch3          :8 ; // Stream ID for input channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idin_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idin_3 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_IDOUT_3:

LANE_IDOUT_3
""""""""""""

Output stream IDs for lane3

.. table:: 

    +-----+---+------+-----------------------------+
    |Bit #|R/W| Name |         Description         |
    +=====+===+======+=============================+
    |7:0  |R/W|ID_CH0|Stream ID for output channel0|
    +-----+---+------+-----------------------------+
    |15:8 |R/W|ID_CH1|Stream ID for output channel1|
    +-----+---+------+-----------------------------+
    |23:16|R/W|ID_CH2|Stream ID for output channel2|
    +-----+---+------+-----------------------------+
    |31:24|R/W|ID_CH3|Stream ID for output channel3|
    +-----+---+------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Output stream IDs for lane3
                #define UDMA_ASRC_LANE_IDOUT_3_OFFSET            0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_idout_3_get(uint32_t base);
        static inline void udma_asrc_lane_idout_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_BIT                            0
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_MASK                           0xff
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_RESET                          0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_BIT                            8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_MASK                           0xff00
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_RESET                          0xff
        
        // Stream ID for output channel2 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_BIT                            16
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_MASK                           0xff0000
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_RESET                          0xff
        
        // Stream ID for output channel3 (access: R/W)
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_BIT                            24
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_WIDTH                          8
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_MASK                           0xff000000
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_RESET                          0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_GET(value)           (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_GETS(value)          (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0_SET(value,field)     (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH0(val)                 ((val) << 0)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH1(val)                 ((val) << 8)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH2(val)                 ((val) << 16)
        
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_GET(value)           (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_GETS(value)          (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3_SET(value,field)     (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_LANE_IDOUT_3_ID_CH3(val)                 ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_ch0          :8 ; // Stream ID for output channel0
            unsigned int id_ch1          :8 ; // Stream ID for output channel1
            unsigned int id_ch2          :8 ; // Stream ID for output channel2
            unsigned int id_ch3          :8 ; // Stream ID for output channel3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_idout_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_idout_3 : public vp::reg_32
        {
        public:
            inline void id_ch0_set(uint32_t value);
            inline uint32_t id_ch0_get();
            inline void id_ch1_set(uint32_t value);
            inline uint32_t id_ch1_get();
            inline void id_ch2_set(uint32_t value);
            inline uint32_t id_ch2_get();
            inline void id_ch3_set(uint32_t value);
            inline uint32_t id_ch3_get();
        };

|

.. _udma_asrc_LANE_CFG_3:

LANE_CFG_3
""""""""""

Configuration for lane3

.. table:: 

    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                              Description                                              |
    +=====+===+==============+=======================================================================================================+
    |    0|R/W|CLK_EN        |Clock Enable for 4 channel lane block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled            |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN          |Reset value for 4 channel lane block - 1’b0 : Reset        - 1’b1 : No Reset                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |    2|R/W|CTRL_MUX      |Select asrc controller - 1’b0 : Use controller0        - 1’b1 : Use controller1                        |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|USE_STREAM_IN |Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|USE_STREAM_OUT|Use uDMA stream - 1’b0 : Use memory IF       - 1’b1 : Use uDMA Stream                                  |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |15:12|R/W|CH_EN         |Enables single channels in a 4 channel block                                                           |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   16|R/W|WAIT_LOCK_IN  |Stalls input samples until lock goes high  - 1’b0 : Allows input samples  - 1’b1 : Halts input samples |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   17|R/W|WAIT_LOCK_OUT |Stalls output samples until lock goes high - 1’b0 : Allows output samples - 1’b1 : Halts output samples|
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+
    |   18|R/W|DROP_ON_WAIT  |Drops input packets when WAIT_LOCK=0 and force 0 at the input                                          |
    +-----+---+--------------+-------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for lane3
                #define UDMA_ASRC_LANE_CFG_3_OFFSET              0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_lane_cfg_3_get(uint32_t base);
        static inline void udma_asrc_lane_cfg_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_BIT                              0
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_WIDTH                            1
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_MASK                             0x1
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_RESET                            0x0
        
        // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_RSTN_BIT                                1
        #define UDMA_ASRC_LANE_CFG_3_RSTN_WIDTH                              1
        #define UDMA_ASRC_LANE_CFG_3_RSTN_MASK                               0x2
        #define UDMA_ASRC_LANE_CFG_3_RSTN_RESET                              0x0
        
        // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1 (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_BIT                            2
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_WIDTH                          1
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_MASK                           0x4
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_RESET                          0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_BIT                       4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_WIDTH                     4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_MASK                      0xf0
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_RESET                     0x0
        
        // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_BIT                      8
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_WIDTH                    4
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_MASK                     0xf00
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_RESET                    0x0
        
        // Enables single channels in a 4 channel block (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_BIT                               12
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_WIDTH                             4
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_MASK                              0xf000
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_RESET                             0x0
        
        // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_BIT                        16
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_MASK                       0x10000
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_RESET                      0x1
        
        // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_BIT                       17
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_WIDTH                     1
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_MASK                      0x20000
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_RESET                     0x1
        
        // Drops input packets when WAIT_LOCK=0 and force 0 at the input (access: R/W)
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_BIT                        18
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_WIDTH                      1
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_MASK                       0x40000
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_RESET                      0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_LANE_CFG_3_CLK_EN(val)                   ((val) << 0)
        
        #define UDMA_ASRC_LANE_CFG_3_RSTN_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_LANE_CFG_3_RSTN(val)                     ((val) << 1)
        
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_LANE_CFG_3_CTRL_MUX(val)                 ((val) << 2)
        
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_GET(value)      (GAP_BEXTRACTU((value),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_GETS(value)     (GAP_BEXTRACT((value),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN_SET(value,field) (GAP_BINSERT((value),(field),4,4))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_IN(val)            ((val) << 4)
        
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define UDMA_ASRC_LANE_CFG_3_USE_STREAM_OUT(val)           ((val) << 8)
        
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_GET(value)              (GAP_BEXTRACTU((value),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_GETS(value)             (GAP_BEXTRACT((value),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN_SET(value,field)        (GAP_BINSERT((value),(field),4,12))
        #define UDMA_ASRC_LANE_CFG_3_CH_EN(val)                    ((val) << 12)
        
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_GET(value)       (GAP_BEXTRACTU((value),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_GETS(value)      (GAP_BEXTRACT((value),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_IN(val)             ((val) << 16)
        
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_GET(value)      (GAP_BEXTRACTU((value),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_GETS(value)     (GAP_BEXTRACT((value),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_ASRC_LANE_CFG_3_WAIT_LOCK_OUT(val)            ((val) << 17)
        
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_ASRC_LANE_CFG_3_DROP_ON_WAIT(val)             ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 4 channel lane block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 4 channel lane block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ctrl_mux        :1 ; // Select asrc controller - 1b0 : Use controller0        - 1b1 : Use controller1
            unsigned int padding0:1 ;
            unsigned int use_stream_in   :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int use_stream_out  :4 ; // Use uDMA stream - 1b0 : Use memory IF       - 1b1 : Use uDMA Stream
            unsigned int ch_en           :4 ; // Enables single channels in a 4 channel block
            unsigned int wait_lock_in    :1 ; // Stalls input samples until lock goes high  - 1b0 : Allows input samples  - 1b1 : Halts input samples
            unsigned int wait_lock_out   :1 ; // Stalls output samples until lock goes high - 1b0 : Allows output samples - 1b1 : Halts output samples
            unsigned int drop_on_wait    :1 ; // Drops input packets when WAIT_LOCK=0 and force 0 at the input
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_lane_cfg_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_lane_cfg_3 : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void ctrl_mux_set(uint32_t value);
            inline uint32_t ctrl_mux_get();
            inline void use_stream_in_set(uint32_t value);
            inline uint32_t use_stream_in_get();
            inline void use_stream_out_set(uint32_t value);
            inline uint32_t use_stream_out_get();
            inline void ch_en_set(uint32_t value);
            inline uint32_t ch_en_get();
            inline void wait_lock_in_set(uint32_t value);
            inline uint32_t wait_lock_in_get();
            inline void wait_lock_out_set(uint32_t value);
            inline uint32_t wait_lock_out_get();
            inline void drop_on_wait_set(uint32_t value);
            inline uint32_t drop_on_wait_get();
        };

|

.. _udma_asrc_MEM2MEM_ID:

MEM2MEM_ID
""""""""""

Configuration for lane2

.. table:: 

    +-----+---+-----------+-----------------------------+
    |Bit #|R/W|   Name    |         Description         |
    +=====+===+===========+=============================+
    |7:0  |R/W|M2M_IN_CH0 |Stream ID for input channel0 |
    +-----+---+-----------+-----------------------------+
    |15:8 |R/W|M2M_IN_CH1 |Stream ID for input channel1 |
    +-----+---+-----------+-----------------------------+
    |23:16|R/W|M2M_OUT_CH0|Stream ID for output channel0|
    +-----+---+-----------+-----------------------------+
    |31:24|R/W|M2M_OUT_CH1|Stream ID for output channel1|
    +-----+---+-----------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for lane2
                #define UDMA_ASRC_MEM2MEM_ID_OFFSET              0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_mem2mem_id_get(uint32_t base);
        static inline void udma_asrc_mem2mem_id_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for input channel0 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_BIT                          0
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_WIDTH                        8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_MASK                         0xff
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_RESET                        0xff
        
        // Stream ID for input channel1 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_BIT                          8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_WIDTH                        8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_MASK                         0xff00
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_RESET                        0xff
        
        // Stream ID for output channel0 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_BIT                         16
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_WIDTH                       8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_MASK                        0xff0000
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_RESET                       0xff
        
        // Stream ID for output channel1 (access: R/W)
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_BIT                         24
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_WIDTH                       8
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_MASK                        0xff000000
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_RESET                       0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH0(val)               ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_GET(value)         (GAP_BEXTRACTU((value),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_GETS(value)        (GAP_BEXTRACT((value),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1_SET(value,field)   (GAP_BINSERT((value),(field),8,8))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_IN_CH1(val)               ((val) << 8)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_GET(value)        (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_GETS(value)       (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0_SET(value,field)  (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH0(val)              ((val) << 16)
        
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_GET(value)        (GAP_BEXTRACTU((value),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_GETS(value)       (GAP_BEXTRACT((value),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1_SET(value,field)  (GAP_BINSERT((value),(field),8,24))
        #define UDMA_ASRC_MEM2MEM_ID_M2M_OUT_CH1(val)              ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int m2m_in_ch0      :8 ; // Stream ID for input channel0
            unsigned int m2m_in_ch1      :8 ; // Stream ID for input channel1
            unsigned int m2m_out_ch0     :8 ; // Stream ID for output channel0
            unsigned int m2m_out_ch1     :8 ; // Stream ID for output channel1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_id_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_mem2mem_id : public vp::reg_32
        {
        public:
            inline void m2m_in_ch0_set(uint32_t value);
            inline uint32_t m2m_in_ch0_get();
            inline void m2m_in_ch1_set(uint32_t value);
            inline uint32_t m2m_in_ch1_get();
            inline void m2m_out_ch0_set(uint32_t value);
            inline uint32_t m2m_out_ch0_get();
            inline void m2m_out_ch1_set(uint32_t value);
            inline uint32_t m2m_out_ch1_get();
        };

|

.. _udma_asrc_MEM2MEM_RATIO:

MEM2MEM_RATIO
"""""""""""""

Input stream IDs for lane3

.. table:: 

    +-----+---+------------+---------------------+
    |Bit #|R/W|    Name    |     Description     |
    +=====+===+============+=====================+
    |    0|R/W|M2M_RATIO_EN|Forces specific ratio|
    +-----+---+------------+---------------------+
    |26:1 |R/W|M2M_RATIO   |Ratio's value        |
    +-----+---+------------+---------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Input stream IDs for lane3
                #define UDMA_ASRC_MEM2MEM_RATIO_OFFSET           0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_mem2mem_ratio_get(uint32_t base);
        static inline void udma_asrc_mem2mem_ratio_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Forces specific ratio (access: R/W)
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_BIT                     0
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_WIDTH                   1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_MASK                    0x1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_RESET                   0x0
        
        // Ratio's value (access: R/W)
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_BIT                        1
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_WIDTH                      26
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_MASK                       0x7fffffe
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_GET(value)    (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_GETS(value)   (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_EN(val)          ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_GET(value)       (GAP_BEXTRACTU((value),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_GETS(value)      (GAP_BEXTRACT((value),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO_SET(value,field) (GAP_BINSERT((value),(field),26,1))
        #define UDMA_ASRC_MEM2MEM_RATIO_M2M_RATIO(val)             ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int m2m_ratio_en    :1 ; // Forces specific ratio
            unsigned int m2m_ratio       :26; // Ratio's value
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_ratio_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_mem2mem_ratio : public vp::reg_32
        {
        public:
            inline void m2m_ratio_en_set(uint32_t value);
            inline uint32_t m2m_ratio_en_get();
            inline void m2m_ratio_set(uint32_t value);
            inline uint32_t m2m_ratio_get();
        };

|

.. _udma_asrc_MEM2MEM_CFG:

MEM2MEM_CFG
"""""""""""

Output stream IDs for lane3

.. table:: 

    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                         Description                                          |
    +=====+===+=======+==============================================================================================+
    |    0|R/W|CLK_EN |Clock Enable for 2 channel mem2mem block - 1’b0 : Clock Disabled        - 1’b1 : Clock Enabled|
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |    1|R/W|RSTN   |Reset value for 2 channel mem2mem block - 1’b0 : Reset        - 1’b1 : No Reset               |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |3:2  |R/W|CH_EN  |Enables single channels in a mem2mem block                                                    |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |6:4  |R/W|FS_IN  |Selects input sampling frequency                                                              |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |9:7  |R/W|FS_OUT |Selects output sampling frequency                                                             |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |10   |R/W|STORE  |Stores the filter state to memory                                                             |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |11   |R/W|RESTORE|Restore the filter state from memory                                                          |
    +-----+---+-------+----------------------------------------------------------------------------------------------+
    |23:16|R/W|CTX_ID |Stream ID used for store/restore                                                              |
    +-----+---+-------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Output stream IDs for lane3
                #define UDMA_ASRC_MEM2MEM_CFG_OFFSET             0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_mem2mem_cfg_get(uint32_t base);
        static inline void udma_asrc_mem2mem_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Enable for 2 channel mem2mem block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_BIT                             0
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_WIDTH                           1
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_MASK                            0x1
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_RESET                           0x0
        
        // Reset value for 2 channel mem2mem block - 1b0 : Reset        - 1b1 : No Reset (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_BIT                               1
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_WIDTH                             1
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_MASK                              0x2
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_RESET                             0x0
        
        // Enables single channels in a mem2mem block (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_BIT                              2
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_WIDTH                            2
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_MASK                             0xc
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_RESET                            0x0
        
        // Selects input sampling frequency (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_BIT                              4
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_WIDTH                            3
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_MASK                             0x70
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_RESET                            0x0
        
        // Selects output sampling frequency (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_BIT                             7
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_WIDTH                           3
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_MASK                            0x380
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_RESET                           0x0
        
        // Stores the filter state to memory (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_BIT                              10
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_WIDTH                            1
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_MASK                             0x400
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_RESET                            0x0
        
        // Restore the filter state from memory (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_BIT                            11
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_WIDTH                          1
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_MASK                           0x800
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_RESET                          0x0
        
        // Stream ID used for store/restore (access: R/W)
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_BIT                             16
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_WIDTH                           8
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_MASK                            0xff0000
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_RESET                           0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_MEM2MEM_CFG_CLK_EN(val)                  ((val) << 0)
        
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_MEM2MEM_CFG_RSTN(val)                    ((val) << 1)
        
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_GET(value)             (GAP_BEXTRACTU((value),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_GETS(value)            (GAP_BEXTRACT((value),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN_SET(value,field)       (GAP_BINSERT((value),(field),2,2))
        #define UDMA_ASRC_MEM2MEM_CFG_CH_EN(val)                   ((val) << 2)
        
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_GET(value)             (GAP_BEXTRACTU((value),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_GETS(value)            (GAP_BEXTRACT((value),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN_SET(value,field)       (GAP_BINSERT((value),(field),3,4))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_IN(val)                   ((val) << 4)
        
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_GET(value)            (GAP_BEXTRACTU((value),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_GETS(value)           (GAP_BEXTRACT((value),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT_SET(value,field)      (GAP_BINSERT((value),(field),3,7))
        #define UDMA_ASRC_MEM2MEM_CFG_FS_OUT(val)                  ((val) << 7)
        
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_GET(value)             (GAP_BEXTRACTU((value),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_GETS(value)            (GAP_BEXTRACT((value),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE_SET(value,field)       (GAP_BINSERT((value),(field),1,10))
        #define UDMA_ASRC_MEM2MEM_CFG_STORE(val)                   ((val) << 10)
        
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_GET(value)           (GAP_BEXTRACTU((value),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_GETS(value)          (GAP_BEXTRACT((value),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE_SET(value,field)     (GAP_BINSERT((value),(field),1,11))
        #define UDMA_ASRC_MEM2MEM_CFG_RESTORE(val)                 ((val) << 11)
        
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_ASRC_MEM2MEM_CFG_CTX_ID(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_en          :1 ; // Clock Enable for 2 channel mem2mem block - 1b0 : Clock Disabled        - 1b1 : Clock Enabled
            unsigned int rstn            :1 ; // Reset value for 2 channel mem2mem block - 1b0 : Reset        - 1b1 : No Reset
            unsigned int ch_en           :2 ; // Enables single channels in a mem2mem block
            unsigned int fs_in           :3 ; // Selects input sampling frequency
            unsigned int fs_out          :3 ; // Selects output sampling frequency
            unsigned int store           :1 ; // Stores the filter state to memory
            unsigned int restore         :1 ; // Restore the filter state from memory
            unsigned int padding0:4 ;
            unsigned int ctx_id          :8 ; // Stream ID used for store/restore
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_mem2mem_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_mem2mem_cfg : public vp::reg_32
        {
        public:
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void rstn_set(uint32_t value);
            inline uint32_t rstn_get();
            inline void ch_en_set(uint32_t value);
            inline uint32_t ch_en_get();
            inline void fs_in_set(uint32_t value);
            inline uint32_t fs_in_get();
            inline void fs_out_set(uint32_t value);
            inline uint32_t fs_out_get();
            inline void store_set(uint32_t value);
            inline uint32_t store_get();
            inline void restore_set(uint32_t value);
            inline uint32_t restore_get();
            inline void ctx_id_set(uint32_t value);
            inline uint32_t ctx_id_get();
        };

|

.. _udma_asrc_STATUS:

STATUS
""""""

Global configuration of ASRC

.. table:: 

    +-----+---+-----------+------------------+
    |Bit #|R/W|   Name    |   Description    |
    +=====+===+===========+==================+
    |    0|RO |LOCK_CTRL_0|Status of the lock|
    +-----+---+-----------+------------------+
    |    1|RO |LOCK_CTRL_1|Status of the lock|
    +-----+---+-----------+------------------+
    |    2|RO |LOCK_M2M   |Status of the lock|
    +-----+---+-----------+------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Global configuration of ASRC
                #define UDMA_ASRC_STATUS_OFFSET                  0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_asrc_status_get(uint32_t base);
        static inline void udma_asrc_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_BIT                             0
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_WIDTH                           1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_MASK                            0x1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_RESET                           0x0
        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_BIT                             1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_WIDTH                           1
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_MASK                            0x2
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_RESET                           0x0
        
        // Status of the lock (access: RO)
        #define UDMA_ASRC_STATUS_LOCK_M2M_BIT                                2
        #define UDMA_ASRC_STATUS_LOCK_M2M_WIDTH                              1
        #define UDMA_ASRC_STATUS_LOCK_M2M_MASK                               0x4
        #define UDMA_ASRC_STATUS_LOCK_M2M_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_0(val)                  ((val) << 0)
        
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_ASRC_STATUS_LOCK_CTRL_1(val)                  ((val) << 1)
        
        #define UDMA_ASRC_STATUS_LOCK_M2M_GET(value)               (GAP_BEXTRACTU((value),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M_GETS(value)              (GAP_BEXTRACT((value),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M_SET(value,field)         (GAP_BINSERT((value),(field),1,2))
        #define UDMA_ASRC_STATUS_LOCK_M2M(val)                     ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int lock_ctrl_0     :1 ; // Status of the lock
            unsigned int lock_ctrl_1     :1 ; // Status of the lock
            unsigned int lock_m2m        :1 ; // Status of the lock
          };
          unsigned int raw;
        } __attribute__((packed)) udma_asrc_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_asrc_status : public vp::reg_32
        {
        public:
            inline void lock_ctrl_0_set(uint32_t value);
            inline uint32_t lock_ctrl_0_get();
            inline void lock_ctrl_1_set(uint32_t value);
            inline uint32_t lock_ctrl_1_get();
            inline void lock_m2m_set(uint32_t value);
            inline uint32_t lock_m2m_get();
        };

|
