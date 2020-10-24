Input file: fe/ips/udma/udma_filter/docs/FILTER_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------------+------+-----+---------------------------------------------+
    |                       Name                        |Offset|Width|                 Description                 |
    +===================================================+======+=====+=============================================+
    |:ref:`REG_TX_CH0_ADD<udma_filter_REG_TX_CH0_ADD>`  |     0|   32|FILTER tx channel 0 address register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_CFG<udma_filter_REG_TX_CH0_CFG>`  |     4|   32|FILTER tx channel 0 configuration register   |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN0<udma_filter_REG_TX_CH0_LEN0>`|     8|   32|FILTER tx channel 0 length0 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN1<udma_filter_REG_TX_CH0_LEN1>`|    12|   32|FILTER tx channel 0 length1 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN2<udma_filter_REG_TX_CH0_LEN2>`|    16|   32|FILTER tx channel 0 length2 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_ADD<udma_filter_REG_TX_CH1_ADD>`  |    20|   32|FILTER tx channel 1 address register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_CFG<udma_filter_REG_TX_CH1_CFG>`  |    24|   32|FILTER tx channel 1 configuration register   |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN0<udma_filter_REG_TX_CH1_LEN0>`|    28|   32|FILTER tx channel 1 length0 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN1<udma_filter_REG_TX_CH1_LEN1>`|    32|   32|FILTER tx channel 1 length1 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN2<udma_filter_REG_TX_CH1_LEN2>`|    36|   32|FILTER tx channel 1 length2 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_ADD<udma_filter_REG_RX_CH_ADD>`    |    40|   32|FILTER RX channel address register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_CFG<udma_filter_REG_RX_CH_CFG>`    |    44|   32|FILTER RX channel configuration register     |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN0<udma_filter_REG_RX_CH_LEN0>`  |    48|   32|FILTER RX channel length0 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN1<udma_filter_REG_RX_CH_LEN1>`  |    52|   32|FILTER RX channel length1 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN2<udma_filter_REG_RX_CH_LEN2>`  |    56|   32|FILTER RX channel length2 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_CFG<udma_filter_REG_AU_CFG>`          |    60|   32|FILTER arithmetic unit configuration register|
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_REG0<udma_filter_REG_AU_REG0>`        |    64|   32|FILTER arithmetic unit 0 register            |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_REG1<udma_filter_REG_AU_REG1>`        |    68|   32|FILTER arithmetic unit 1 register            |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_TH<udma_filter_REG_BINCU_TH>`      |    72|   32|FILTER binarization threshold register       |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_CNT<udma_filter_REG_BINCU_CNT>`    |    76|   32|FILTER binarization count register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_SETUP<udma_filter_REG_BINCU_SETUP>`|    80|   32|FILTER binarization datasize format register |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_VAL<udma_filter_REG_BINCU_VAL>`    |    84|   32|FILTER binarization result count register    |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_FILT<udma_filter_REG_FILT>`              |    88|   32|FILTER control mode register                 |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_FILT_CMD<udma_filter_REG_FILT_CMD>`      |    92|   32|FILTER start register                        |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_STATUS<udma_filter_REG_STATUS>`          |    96|   32|FILTER status register                       |
    +---------------------------------------------------+------+-----+---------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER tx channel 0 address register
                #define UDMA_FILTER_REG_TX_CH0_ADD_OFFSET        0x0
        
                // FILTER tx channel 0 configuration register
                #define UDMA_FILTER_REG_TX_CH0_CFG_OFFSET        0x4
        
                // FILTER tx channel 0 length0 register
                #define UDMA_FILTER_REG_TX_CH0_LEN0_OFFSET       0x8
        
                // FILTER tx channel 0 length1 register
                #define UDMA_FILTER_REG_TX_CH0_LEN1_OFFSET       0xc
        
                // FILTER tx channel 0 length2 register
                #define UDMA_FILTER_REG_TX_CH0_LEN2_OFFSET       0x10
        
                // FILTER tx channel 1 address register
                #define UDMA_FILTER_REG_TX_CH1_ADD_OFFSET        0x14
        
                // FILTER tx channel 1 configuration register
                #define UDMA_FILTER_REG_TX_CH1_CFG_OFFSET        0x18
        
                // FILTER tx channel 1 length0 register
                #define UDMA_FILTER_REG_TX_CH1_LEN0_OFFSET       0x1c
        
                // FILTER tx channel 1 length1 register
                #define UDMA_FILTER_REG_TX_CH1_LEN1_OFFSET       0x20
        
                // FILTER tx channel 1 length2 register
                #define UDMA_FILTER_REG_TX_CH1_LEN2_OFFSET       0x24
        
                // FILTER RX channel address register
                #define UDMA_FILTER_REG_RX_CH_ADD_OFFSET         0x28
        
                // FILTER RX channel configuration register
                #define UDMA_FILTER_REG_RX_CH_CFG_OFFSET         0x2c
        
                // FILTER RX channel length0 register
                #define UDMA_FILTER_REG_RX_CH_LEN0_OFFSET        0x30
        
                // FILTER RX channel length1 register
                #define UDMA_FILTER_REG_RX_CH_LEN1_OFFSET        0x34
        
                // FILTER RX channel length2 register
                #define UDMA_FILTER_REG_RX_CH_LEN2_OFFSET        0x38
        
                // FILTER arithmetic unit configuration register
                #define UDMA_FILTER_REG_AU_CFG_OFFSET            0x3c
        
                // FILTER arithmetic unit 0 register
                #define UDMA_FILTER_REG_AU_REG0_OFFSET           0x40
        
                // FILTER arithmetic unit 1 register
                #define UDMA_FILTER_REG_AU_REG1_OFFSET           0x44
        
                // FILTER binarization threshold register
                #define UDMA_FILTER_REG_BINCU_TH_OFFSET          0x48
        
                // FILTER binarization count register
                #define UDMA_FILTER_REG_BINCU_CNT_OFFSET         0x4c
        
                // FILTER binarization datasize format register
                #define UDMA_FILTER_REG_BINCU_SETUP_OFFSET       0x50
        
                // FILTER binarization result count register
                #define UDMA_FILTER_REG_BINCU_VAL_OFFSET         0x54
        
                // FILTER control mode register
                #define UDMA_FILTER_REG_FILT_OFFSET              0x58
        
                // FILTER start register
                #define UDMA_FILTER_REG_FILT_CMD_OFFSET          0x5c
        
                // FILTER status register
                #define UDMA_FILTER_REG_STATUS_OFFSET            0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_add_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_add_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch0_cfg_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch0_len0_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch0_len1_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch0_len2_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch1_add_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_add_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch1_cfg_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch1_len0_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch1_len1_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_tx_ch1_len2_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_rx_ch_add_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_add_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_rx_ch_cfg_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_rx_ch_len0_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_rx_ch_len1_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_rx_ch_len2_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_au_cfg_get(uint32_t base);
        static inline void udma_filter_reg_au_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_au_reg0_get(uint32_t base);
        static inline void udma_filter_reg_au_reg0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_au_reg1_get(uint32_t base);
        static inline void udma_filter_reg_au_reg1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_bincu_th_get(uint32_t base);
        static inline void udma_filter_reg_bincu_th_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_bincu_cnt_get(uint32_t base);
        static inline void udma_filter_reg_bincu_cnt_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_bincu_setup_get(uint32_t base);
        static inline void udma_filter_reg_bincu_setup_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_bincu_val_get(uint32_t base);
        static inline void udma_filter_reg_bincu_val_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_filt_get(uint32_t base);
        static inline void udma_filter_reg_filt_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_filt_cmd_get(uint32_t base);
        static inline void udma_filter_reg_filt_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_filter_reg_status_get(uint32_t base);
        static inline void udma_filter_reg_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit (access: R)
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_BIT                          0
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_MASK                         0x3
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_RESET                        0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R)
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_BIT                          8
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_MASK                         0x300
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_RESET                        0x0
        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2b10:32-bit (access: R)
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_BIT                          0
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_MASK                         0x3
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_RESET                        0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R/W)
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_BIT                          8
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_MASK                         0x300
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_RESET                        0x0
        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit (access: R/W)
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_BIT                           0
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_WIDTH                         2
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_MASK                          0x3
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_RESET                         0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R/W)
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_BIT                           8
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_WIDTH                         2
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_MASK                          0x300
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_RESET                         0x0
        
        // Arithmetic Unit result signed or not. -1b0: not signed -1b1: signed (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_BIT                            0
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_WIDTH                          1
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_MASK                           0x1
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_RESET                          0x0
        
        // Arithmetic Unit bypass or not. -1b0: not bypass AU -1b1: bypass AU (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_BIT                            1
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_WIDTH                          1
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_MASK                           0x2
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_RESET                          0x0
        
        // Arithmetic Unit mode: -4b0000: AU_MODE_AxB -4b0001: AU_MODE_AxB+REG0 -4b0010: AU_MODE_AxB accumulation -4b0011: AU_MODE_AxA -4b0100: AU_MODE_AxA+B -4b0101: AU_MODE_AxA-B -4b0110: AU_MODE_AxA accumulation -4b0111: AU_MODE_AxA+REG0 -4b1000: AU_MODE_AxREG1 -4b1001: AU_MODE_AxREG1+B -4b1010: AU_MODE_AxREG1-B -4b1011: AU_MODE_AxREG1+REG0 -4b1100: AU_MODE_AxREG1 accumulation -4b1101: AU_MODE_A+B -4b1110: AU_MODE_A-B -4b1111: AU_MODE_A+REG0 (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_MODE_BIT                              8
        #define UDMA_FILTER_REG_AU_CFG_MODE_WIDTH                            4
        #define UDMA_FILTER_REG_AU_CFG_MODE_MASK                             0xf00
        #define UDMA_FILTER_REG_AU_CFG_MODE_RESET                            0x0
        
        // Arithmetic Unit shift window size, (0  31). (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_BIT                             16
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_WIDTH                           5
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_MASK                            0x1f0000
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_RESET                           0x0
        
        //  Binarization and counting unit count value set. (access: R/W)
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_BIT                          0
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_WIDTH                        20
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_MASK                         0xfffff
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_RESET                        0x0
        
        //  Binarization and counting unit enable: -1b0: not enable  -1b1: enable (access: R/W)
        #define UDMA_FILTER_REG_BINCU_CNT_EN_BIT                             31
        #define UDMA_FILTER_REG_BINCU_CNT_EN_WIDTH                           1
        #define UDMA_FILTER_REG_BINCU_CNT_EN_MASK                            0x80000000
        #define UDMA_FILTER_REG_BINCU_CNT_EN_RESET                           0x0
        
        // Filter start flag, write only, write 1 to start the filter : (access: R/W)
        #define UDMA_FILTER_REG_FILT_CMD_START_BIT                           0
        #define UDMA_FILTER_REG_FILT_CMD_START_WIDTH                         1
        #define UDMA_FILTER_REG_FILT_CMD_START_MASK                          0x1
        #define UDMA_FILTER_REG_FILT_CMD_START_RESET                         0x0
        
        // Filter done flag, write 1 to clear the flag : -1b0: Filter process is not finished -1b1: Filter process is finished (access: R/W)
        #define UDMA_FILTER_REG_STATUS_DONE_BIT                              0
        #define UDMA_FILTER_REG_STATUS_DONE_WIDTH                            1
        #define UDMA_FILTER_REG_STATUS_DONE_MASK                             0x1
        #define UDMA_FILTER_REG_STATUS_DONE_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_GET(value)         (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_GETS(value)        (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_SET(value,field)   (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_GET(value)         (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_GETS(value)        (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE(val)               ((val) << 8)
        
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_GET(value)         (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_GETS(value)        (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_SET(value,field)   (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_GET(value)         (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_GETS(value)        (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE(val)               ((val) << 8)
        
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE(val)                ((val) << 0)
        
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE(val)                ((val) << 8)
        
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED(val)                 ((val) << 0)
        
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_GET(value)           (GAP_BEXTRACTU((value),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_GETS(value)          (GAP_BEXTRACT((value),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_SET(value,field)     (GAP_BINSERT((value),(field),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS(val)                 ((val) << 1)
        
        #define UDMA_FILTER_REG_AU_CFG_MODE_GET(value)             (GAP_BEXTRACTU((value),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE_GETS(value)            (GAP_BEXTRACT((value),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE_SET(value,field)       (GAP_BINSERT((value),(field),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE(val)                   ((val) << 8)
        
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_GET(value)            (GAP_BEXTRACTU((value),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_GETS(value)           (GAP_BEXTRACT((value),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_SET(value,field)      (GAP_BINSERT((value),(field),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT(val)                  ((val) << 16)
        
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_GET(value)         (GAP_BEXTRACTU((value),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_GETS(value)        (GAP_BEXTRACT((value),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_SET(value,field)   (GAP_BINSERT((value),(field),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_BINCU_CNT_EN_GET(value)            (GAP_BEXTRACTU((value),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN_GETS(value)           (GAP_BEXTRACT((value),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN(val)                  ((val) << 31)
        
        #define UDMA_FILTER_REG_FILT_CMD_START_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START(val)                ((val) << 0)
        
        #define UDMA_FILTER_REG_STATUS_DONE_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_FILTER_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t reg_tx_ch0_add;  // FILTER tx channel 0 address register
            volatile uint32_t reg_tx_ch0_cfg;  // FILTER tx channel 0 configuration register
            volatile uint32_t reg_tx_ch0_len0;  // FILTER tx channel 0 length0 register
            volatile uint32_t reg_tx_ch0_len1;  // FILTER tx channel 0 length1 register
            volatile uint32_t reg_tx_ch0_len2;  // FILTER tx channel 0 length2 register
            volatile uint32_t reg_tx_ch1_add;  // FILTER tx channel 1 address register
            volatile uint32_t reg_tx_ch1_cfg;  // FILTER tx channel 1 configuration register
            volatile uint32_t reg_tx_ch1_len0;  // FILTER tx channel 1 length0 register
            volatile uint32_t reg_tx_ch1_len1;  // FILTER tx channel 1 length1 register
            volatile uint32_t reg_tx_ch1_len2;  // FILTER tx channel 1 length2 register
            volatile uint32_t reg_rx_ch_add;  // FILTER RX channel address register
            volatile uint32_t reg_rx_ch_cfg;  // FILTER RX channel configuration register
            volatile uint32_t reg_rx_ch_len0;  // FILTER RX channel length0 register
            volatile uint32_t reg_rx_ch_len1;  // FILTER RX channel length1 register
            volatile uint32_t reg_rx_ch_len2;  // FILTER RX channel length2 register
            volatile uint32_t reg_au_cfg;  // FILTER arithmetic unit configuration register
            volatile uint32_t reg_au_reg0;  // FILTER arithmetic unit 0 register
            volatile uint32_t reg_au_reg1;  // FILTER arithmetic unit 1 register
            volatile uint32_t reg_bincu_th;  // FILTER binarization threshold register
            volatile uint32_t reg_bincu_cnt;  // FILTER binarization count register
            volatile uint32_t reg_bincu_setup;  // FILTER binarization datasize format register
            volatile uint32_t reg_bincu_val;  // FILTER binarization result count register
            volatile uint32_t reg_filt;  // FILTER control mode register
            volatile uint32_t reg_filt_cmd;  // FILTER start register
            volatile uint32_t reg_status;  // FILTER status register
        } __attribute__((packed)) udma_filter_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_add_t;
        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_cfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_add_t;
        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_cfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_add_t;
        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_cfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_len0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_len1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_len2_t;
        
        typedef union {
          struct {
            unsigned int signed          :1 ; // Arithmetic Unit result signed or not. -1b0: not signed -1b1: signed
            unsigned int bypass          :1 ; // Arithmetic Unit bypass or not. -1b0: not bypass AU -1b1: bypass AU
            unsigned int padding0:6 ;
            unsigned int mode            :4 ; // Arithmetic Unit mode: -4b0000: AU_MODE_AxB -4b0001: AU_MODE_AxB+REG0 -4b0010: AU_MODE_AxB accumulation -4b0011: AU_MODE_AxA -4b0100: AU_MODE_AxA+B -4b0101: AU_MODE_AxA-B -4b0110: AU_MODE_AxA accumulation -4b0111: AU_MODE_AxA+REG0 -4b1000: AU_MODE_AxREG1 -4b1001: AU_MODE_AxREG1+B -4b1010: AU_MODE_AxREG1-B -4b1011: AU_MODE_AxREG1+REG0 -4b1100: AU_MODE_AxREG1 accumulation -4b1101: AU_MODE_A+B -4b1110: AU_MODE_A-B -4b1111: AU_MODE_A+REG0
            unsigned int padding1:4 ;
            unsigned int shift           :5 ; // Arithmetic Unit shift window size, (0  31).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_au_cfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_au_reg0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_au_reg1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_bincu_th_t;
        
        typedef union {
          struct {
            unsigned int count           :20; //  Binarization and counting unit count value set.
            unsigned int padding0:11;
            unsigned int en              :1 ; //  Binarization and counting unit enable: -1b0: not enable  -1b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_bincu_cnt_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_bincu_setup_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_bincu_val_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_filt_t;
        
        typedef union {
          struct {
            unsigned int start           :1 ; // Filter start flag, write only, write 1 to start the filter :
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_filt_cmd_t;
        
        typedef union {
          struct {
            unsigned int done            :1 ; // Filter done flag, write 1 to clear the flag : -1b0: Filter process is not finished -1b1: Filter process is finished
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_filter : public vp::regmap
        {
        public:
            vp_udma_filter_reg_tx_ch0_add reg_tx_ch0_add;
            vp_udma_filter_reg_tx_ch0_cfg reg_tx_ch0_cfg;
            vp_udma_filter_reg_tx_ch0_len0 reg_tx_ch0_len0;
            vp_udma_filter_reg_tx_ch0_len1 reg_tx_ch0_len1;
            vp_udma_filter_reg_tx_ch0_len2 reg_tx_ch0_len2;
            vp_udma_filter_reg_tx_ch1_add reg_tx_ch1_add;
            vp_udma_filter_reg_tx_ch1_cfg reg_tx_ch1_cfg;
            vp_udma_filter_reg_tx_ch1_len0 reg_tx_ch1_len0;
            vp_udma_filter_reg_tx_ch1_len1 reg_tx_ch1_len1;
            vp_udma_filter_reg_tx_ch1_len2 reg_tx_ch1_len2;
            vp_udma_filter_reg_rx_ch_add reg_rx_ch_add;
            vp_udma_filter_reg_rx_ch_cfg reg_rx_ch_cfg;
            vp_udma_filter_reg_rx_ch_len0 reg_rx_ch_len0;
            vp_udma_filter_reg_rx_ch_len1 reg_rx_ch_len1;
            vp_udma_filter_reg_rx_ch_len2 reg_rx_ch_len2;
            vp_udma_filter_reg_au_cfg reg_au_cfg;
            vp_udma_filter_reg_au_reg0 reg_au_reg0;
            vp_udma_filter_reg_au_reg1 reg_au_reg1;
            vp_udma_filter_reg_bincu_th reg_bincu_th;
            vp_udma_filter_reg_bincu_cnt reg_bincu_cnt;
            vp_udma_filter_reg_bincu_setup reg_bincu_setup;
            vp_udma_filter_reg_bincu_val reg_bincu_val;
            vp_udma_filter_reg_filt reg_filt;
            vp_udma_filter_reg_filt_cmd reg_filt_cmd;
            vp_udma_filter_reg_status reg_status;
        };

|

.. _udma_filter_REG_TX_CH0_ADD:

REG_TX_CH0_ADD
""""""""""""""

FILTER tx channel 0 address register

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

        
                // FILTER tx channel 0 address register
                #define UDMA_FILTER_REG_TX_CH0_ADD_OFFSET        0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_add_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_add_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch0_add_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch0_add : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH0_CFG:

REG_TX_CH0_CFG
""""""""""""""

FILTER tx channel 0 configuration register

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                  Description                                   |
    +=====+===+====+================================================================================+
    |1:0  |R  |SIZE|Data transfer format: - 2’b00: 8-bit - 2’b01:16-bit - 2;b10:32-bit              |
    +-----+---+----+--------------------------------------------------------------------------------+
    |9:8  |R  |MODE|Data transfer mode: - 2’b00: Linear - 2’b01: Sliding - 2;b10:Circular - 2;b11:2D|
    +-----+---+----+--------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER tx channel 0 configuration register
                #define UDMA_FILTER_REG_TX_CH0_CFG_OFFSET        0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_cfg_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit (access: R)
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_BIT                          0
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_MASK                         0x3
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_RESET                        0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R)
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_BIT                          8
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_MASK                         0x300
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_GET(value)         (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_GETS(value)        (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE_SET(value,field)   (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_TX_CH0_CFG_SIZE(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_GET(value)         (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_GETS(value)        (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_TX_CH0_CFG_MODE(val)               ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch0_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch0_cfg : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _udma_filter_REG_TX_CH0_LEN0:

REG_TX_CH0_LEN0
"""""""""""""""

FILTER tx channel 0 length0 register

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

        
                // FILTER tx channel 0 length0 register
                #define UDMA_FILTER_REG_TX_CH0_LEN0_OFFSET       0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_len0_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch0_len0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH0_LEN1:

REG_TX_CH0_LEN1
"""""""""""""""

FILTER tx channel 0 length1 register

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

        
                // FILTER tx channel 0 length1 register
                #define UDMA_FILTER_REG_TX_CH0_LEN1_OFFSET       0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_len1_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch0_len1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH0_LEN2:

REG_TX_CH0_LEN2
"""""""""""""""

FILTER tx channel 0 length2 register

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

        
                // FILTER tx channel 0 length2 register
                #define UDMA_FILTER_REG_TX_CH0_LEN2_OFFSET       0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch0_len2_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch0_len2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch0_len2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch0_len2 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH1_ADD:

REG_TX_CH1_ADD
""""""""""""""

FILTER tx channel 1 address register

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

        
                // FILTER tx channel 1 address register
                #define UDMA_FILTER_REG_TX_CH1_ADD_OFFSET        0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch1_add_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_add_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch1_add_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch1_add : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH1_CFG:

REG_TX_CH1_CFG
""""""""""""""

FILTER tx channel 1 configuration register

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                  Description                                   |
    +=====+===+====+================================================================================+
    |1:0  |R  |SIZE|Data transfer format: - 2’b00: 8-bit - 2’b01:16-bit - 2’b10:32-bit              |
    +-----+---+----+--------------------------------------------------------------------------------+
    |9:8  |R/W|MODE|Data transfer mode: - 2’b00: Linear - 2’b01: Sliding - 2;b10:Circular - 2;b11:2D|
    +-----+---+----+--------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER tx channel 1 configuration register
                #define UDMA_FILTER_REG_TX_CH1_CFG_OFFSET        0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch1_cfg_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2b10:32-bit (access: R)
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_BIT                          0
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_MASK                         0x3
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_RESET                        0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R/W)
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_BIT                          8
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_WIDTH                        2
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_MASK                         0x300
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_GET(value)         (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_GETS(value)        (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE_SET(value,field)   (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_TX_CH1_CFG_SIZE(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_GET(value)         (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_GETS(value)        (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_TX_CH1_CFG_MODE(val)               ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_tx_ch1_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch1_cfg : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _udma_filter_REG_TX_CH1_LEN0:

REG_TX_CH1_LEN0
"""""""""""""""

FILTER tx channel 1 length0 register

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

        
                // FILTER tx channel 1 length0 register
                #define UDMA_FILTER_REG_TX_CH1_LEN0_OFFSET       0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch1_len0_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch1_len0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH1_LEN1:

REG_TX_CH1_LEN1
"""""""""""""""

FILTER tx channel 1 length1 register

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

        
                // FILTER tx channel 1 length1 register
                #define UDMA_FILTER_REG_TX_CH1_LEN1_OFFSET       0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch1_len1_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch1_len1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_TX_CH1_LEN2:

REG_TX_CH1_LEN2
"""""""""""""""

FILTER tx channel 1 length2 register

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

        
                // FILTER tx channel 1 length2 register
                #define UDMA_FILTER_REG_TX_CH1_LEN2_OFFSET       0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_tx_ch1_len2_get(uint32_t base);
        static inline void udma_filter_reg_tx_ch1_len2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_tx_ch1_len2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_tx_ch1_len2 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_RX_CH_ADD:

REG_RX_CH_ADD
"""""""""""""

FILTER RX channel address register

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

        
                // FILTER RX channel address register
                #define UDMA_FILTER_REG_RX_CH_ADD_OFFSET         0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_rx_ch_add_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_add_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_rx_ch_add_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_rx_ch_add : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_RX_CH_CFG:

REG_RX_CH_CFG
"""""""""""""

FILTER RX channel configuration register

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                  Description                                   |
    +=====+===+====+================================================================================+
    |1:0  |R/W|SIZE|Data transfer format: - 2’b00: 8-bit - 2’b01:16-bit - 2;b10:32-bit              |
    +-----+---+----+--------------------------------------------------------------------------------+
    |9:8  |R/W|MODE|Data transfer mode: - 2’b00: Linear - 2’b01: Sliding - 2;b10:Circular - 2;b11:2D|
    +-----+---+----+--------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER RX channel configuration register
                #define UDMA_FILTER_REG_RX_CH_CFG_OFFSET         0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_rx_ch_cfg_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit (access: R/W)
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_BIT                           0
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_WIDTH                         2
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_MASK                          0x3
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_RESET                         0x0
        
        // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D (access: R/W)
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_BIT                           8
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_WIDTH                         2
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_MASK                          0x300
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define UDMA_FILTER_REG_RX_CH_CFG_SIZE(val)                ((val) << 0)
        
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define UDMA_FILTER_REG_RX_CH_CFG_MODE(val)                ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :2 ; // Data transfer format: - 2b00: 8-bit - 2b01:16-bit - 2;b10:32-bit
            unsigned int padding0:6 ;
            unsigned int mode            :2 ; // Data transfer mode: - 2b00: Linear - 2b01: Sliding - 2;b10:Circular - 2;b11:2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_rx_ch_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_rx_ch_cfg : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _udma_filter_REG_RX_CH_LEN0:

REG_RX_CH_LEN0
""""""""""""""

FILTER RX channel length0 register

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

        
                // FILTER RX channel length0 register
                #define UDMA_FILTER_REG_RX_CH_LEN0_OFFSET        0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_rx_ch_len0_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_rx_ch_len0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_rx_ch_len0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_RX_CH_LEN1:

REG_RX_CH_LEN1
""""""""""""""

FILTER RX channel length1 register

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

        
                // FILTER RX channel length1 register
                #define UDMA_FILTER_REG_RX_CH_LEN1_OFFSET        0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_rx_ch_len1_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_rx_ch_len1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_rx_ch_len1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_RX_CH_LEN2:

REG_RX_CH_LEN2
""""""""""""""

FILTER RX channel length2 register

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

        
                // FILTER RX channel length2 register
                #define UDMA_FILTER_REG_RX_CH_LEN2_OFFSET        0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_rx_ch_len2_get(uint32_t base);
        static inline void udma_filter_reg_rx_ch_len2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_rx_ch_len2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_rx_ch_len2 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_AU_CFG:

REG_AU_CFG
""""""""""

FILTER arithmetic unit configuration register

.. table:: 

    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                                                                                                             Description                                                                                                                                                                                                                             |
    +=====+===+======+=====================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |    0|R/W|SIGNED|Arithmetic Unit result signed or not. -1’b0: not signed -1’b1: signed                                                                                                                                                                                                                                                                                                                                                                                                |
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|BYPASS|Arithmetic Unit bypass or not. -1’b0: not bypass AU -1’b1: bypass AU                                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|MODE  |Arithmetic Unit mode: -4’b0000: AU_MODE_AxB -4’b0001: AU_MODE_AxB+REG0 -4’b0010: AU_MODE_AxB accumulation -4’b0011: AU_MODE_AxA -4’b0100: AU_MODE_AxA+B -4’b0101: AU_MODE_AxA-B -4’b0110: AU_MODE_AxA accumulation -4’b0111: AU_MODE_AxA+REG0 -4’b1000: AU_MODE_AxREG1 -4’b1001: AU_MODE_AxREG1+B -4’b1010: AU_MODE_AxREG1-B -4’b1011: AU_MODE_AxREG1+REG0 -4’b1100: AU_MODE_AxREG1 accumulation -4’b1101: AU_MODE_A+B -4’b1110: AU_MODE_A-B -4’b1111: AU_MODE_A+REG0|
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |20:16|R/W|SHIFT |Arithmetic Unit shift window size, (0 – 31).                                                                                                                                                                                                                                                                                                                                                                                                                         |
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER arithmetic unit configuration register
                #define UDMA_FILTER_REG_AU_CFG_OFFSET            0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_au_cfg_get(uint32_t base);
        static inline void udma_filter_reg_au_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Arithmetic Unit result signed or not. -1b0: not signed -1b1: signed (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_BIT                            0
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_WIDTH                          1
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_MASK                           0x1
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_RESET                          0x0
        
        // Arithmetic Unit bypass or not. -1b0: not bypass AU -1b1: bypass AU (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_BIT                            1
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_WIDTH                          1
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_MASK                           0x2
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_RESET                          0x0
        
        // Arithmetic Unit mode: -4b0000: AU_MODE_AxB -4b0001: AU_MODE_AxB+REG0 -4b0010: AU_MODE_AxB accumulation -4b0011: AU_MODE_AxA -4b0100: AU_MODE_AxA+B -4b0101: AU_MODE_AxA-B -4b0110: AU_MODE_AxA accumulation -4b0111: AU_MODE_AxA+REG0 -4b1000: AU_MODE_AxREG1 -4b1001: AU_MODE_AxREG1+B -4b1010: AU_MODE_AxREG1-B -4b1011: AU_MODE_AxREG1+REG0 -4b1100: AU_MODE_AxREG1 accumulation -4b1101: AU_MODE_A+B -4b1110: AU_MODE_A-B -4b1111: AU_MODE_A+REG0 (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_MODE_BIT                              8
        #define UDMA_FILTER_REG_AU_CFG_MODE_WIDTH                            4
        #define UDMA_FILTER_REG_AU_CFG_MODE_MASK                             0xf00
        #define UDMA_FILTER_REG_AU_CFG_MODE_RESET                            0x0
        
        // Arithmetic Unit shift window size, (0  31). (access: R/W)
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_BIT                             16
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_WIDTH                           5
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_MASK                            0x1f0000
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_AU_CFG_SIGNED(val)                 ((val) << 0)
        
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_GET(value)           (GAP_BEXTRACTU((value),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_GETS(value)          (GAP_BEXTRACT((value),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS_SET(value,field)     (GAP_BINSERT((value),(field),1,1))
        #define UDMA_FILTER_REG_AU_CFG_BYPASS(val)                 ((val) << 1)
        
        #define UDMA_FILTER_REG_AU_CFG_MODE_GET(value)             (GAP_BEXTRACTU((value),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE_GETS(value)            (GAP_BEXTRACT((value),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE_SET(value,field)       (GAP_BINSERT((value),(field),4,8))
        #define UDMA_FILTER_REG_AU_CFG_MODE(val)                   ((val) << 8)
        
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_GET(value)            (GAP_BEXTRACTU((value),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_GETS(value)           (GAP_BEXTRACT((value),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT_SET(value,field)      (GAP_BINSERT((value),(field),5,16))
        #define UDMA_FILTER_REG_AU_CFG_SHIFT(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int signed          :1 ; // Arithmetic Unit result signed or not. -1b0: not signed -1b1: signed
            unsigned int bypass          :1 ; // Arithmetic Unit bypass or not. -1b0: not bypass AU -1b1: bypass AU
            unsigned int padding0:6 ;
            unsigned int mode            :4 ; // Arithmetic Unit mode: -4b0000: AU_MODE_AxB -4b0001: AU_MODE_AxB+REG0 -4b0010: AU_MODE_AxB accumulation -4b0011: AU_MODE_AxA -4b0100: AU_MODE_AxA+B -4b0101: AU_MODE_AxA-B -4b0110: AU_MODE_AxA accumulation -4b0111: AU_MODE_AxA+REG0 -4b1000: AU_MODE_AxREG1 -4b1001: AU_MODE_AxREG1+B -4b1010: AU_MODE_AxREG1-B -4b1011: AU_MODE_AxREG1+REG0 -4b1100: AU_MODE_AxREG1 accumulation -4b1101: AU_MODE_A+B -4b1110: AU_MODE_A-B -4b1111: AU_MODE_A+REG0
            unsigned int padding1:4 ;
            unsigned int shift           :5 ; // Arithmetic Unit shift window size, (0  31).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_au_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_au_cfg : public vp::reg_32
        {
        public:
            inline void signed_set(uint32_t value);
            inline uint32_t signed_get();
            inline void bypass_set(uint32_t value);
            inline uint32_t bypass_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void shift_set(uint32_t value);
            inline uint32_t shift_get();
        };

|

.. _udma_filter_REG_AU_REG0:

REG_AU_REG0
"""""""""""

FILTER arithmetic unit 0 register

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

        
                // FILTER arithmetic unit 0 register
                #define UDMA_FILTER_REG_AU_REG0_OFFSET           0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_au_reg0_get(uint32_t base);
        static inline void udma_filter_reg_au_reg0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_au_reg0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_au_reg0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_AU_REG1:

REG_AU_REG1
"""""""""""

FILTER arithmetic unit 1 register

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

        
                // FILTER arithmetic unit 1 register
                #define UDMA_FILTER_REG_AU_REG1_OFFSET           0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_au_reg1_get(uint32_t base);
        static inline void udma_filter_reg_au_reg1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_au_reg1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_au_reg1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_BINCU_TH:

REG_BINCU_TH
""""""""""""

FILTER binarization threshold register

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

        
                // FILTER binarization threshold register
                #define UDMA_FILTER_REG_BINCU_TH_OFFSET          0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_bincu_th_get(uint32_t base);
        static inline void udma_filter_reg_bincu_th_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_bincu_th_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_bincu_th : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_BINCU_CNT:

REG_BINCU_CNT
"""""""""""""

FILTER binarization count register

.. table:: 

    +-----+---+-----+------------------------------------------------------------------------+
    |Bit #|R/W|Name |                              Description                               |
    +=====+===+=====+========================================================================+
    |19:0 |R/W|COUNT| Binarization and counting unit count value set.                        |
    +-----+---+-----+------------------------------------------------------------------------+
    |31   |R/W|EN   | Binarization and counting unit enable: -1’b0: not enable  -1’b1: enable|
    +-----+---+-----+------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER binarization count register
                #define UDMA_FILTER_REG_BINCU_CNT_OFFSET         0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_bincu_cnt_get(uint32_t base);
        static inline void udma_filter_reg_bincu_cnt_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        //  Binarization and counting unit count value set. (access: R/W)
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_BIT                          0
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_WIDTH                        20
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_MASK                         0xfffff
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_RESET                        0x0
        
        //  Binarization and counting unit enable: -1b0: not enable  -1b1: enable (access: R/W)
        #define UDMA_FILTER_REG_BINCU_CNT_EN_BIT                             31
        #define UDMA_FILTER_REG_BINCU_CNT_EN_WIDTH                           1
        #define UDMA_FILTER_REG_BINCU_CNT_EN_MASK                            0x80000000
        #define UDMA_FILTER_REG_BINCU_CNT_EN_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_GET(value)         (GAP_BEXTRACTU((value),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_GETS(value)        (GAP_BEXTRACT((value),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT_SET(value,field)   (GAP_BINSERT((value),(field),20,0))
        #define UDMA_FILTER_REG_BINCU_CNT_COUNT(val)               ((val) << 0)
        
        #define UDMA_FILTER_REG_BINCU_CNT_EN_GET(value)            (GAP_BEXTRACTU((value),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN_GETS(value)           (GAP_BEXTRACT((value),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,31))
        #define UDMA_FILTER_REG_BINCU_CNT_EN(val)                  ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int count           :20; //  Binarization and counting unit count value set.
            unsigned int padding0:11;
            unsigned int en              :1 ; //  Binarization and counting unit enable: -1b0: not enable  -1b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_bincu_cnt_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_bincu_cnt : public vp::reg_32
        {
        public:
            inline void count_set(uint32_t value);
            inline uint32_t count_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _udma_filter_REG_BINCU_SETUP:

REG_BINCU_SETUP
"""""""""""""""

FILTER binarization datasize format register

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

        
                // FILTER binarization datasize format register
                #define UDMA_FILTER_REG_BINCU_SETUP_OFFSET       0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_bincu_setup_get(uint32_t base);
        static inline void udma_filter_reg_bincu_setup_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_bincu_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_bincu_setup : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_BINCU_VAL:

REG_BINCU_VAL
"""""""""""""

FILTER binarization result count register

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

        
                // FILTER binarization result count register
                #define UDMA_FILTER_REG_BINCU_VAL_OFFSET         0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_bincu_val_get(uint32_t base);
        static inline void udma_filter_reg_bincu_val_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_bincu_val_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_bincu_val : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_FILT:

REG_FILT
""""""""

FILTER control mode register

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

        
                // FILTER control mode register
                #define UDMA_FILTER_REG_FILT_OFFSET              0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_filt_get(uint32_t base);
        static inline void udma_filter_reg_filt_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_filter_reg_filt_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_filt : public vp::reg_32
        {
        public:
        };

|

.. _udma_filter_REG_FILT_CMD:

REG_FILT_CMD
""""""""""""

FILTER start register

.. table:: 

    +-----+---+-----+------------------------------------------------------------+
    |Bit #|R/W|Name |                        Description                         |
    +=====+===+=====+============================================================+
    |    0|R/W|START|Filter start flag, write only, write 1 to start the filter :|
    +-----+---+-----+------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER start register
                #define UDMA_FILTER_REG_FILT_CMD_OFFSET          0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_filt_cmd_get(uint32_t base);
        static inline void udma_filter_reg_filt_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Filter start flag, write only, write 1 to start the filter : (access: R/W)
        #define UDMA_FILTER_REG_FILT_CMD_START_BIT                           0
        #define UDMA_FILTER_REG_FILT_CMD_START_WIDTH                         1
        #define UDMA_FILTER_REG_FILT_CMD_START_MASK                          0x1
        #define UDMA_FILTER_REG_FILT_CMD_START_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_FILT_CMD_START_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_FILT_CMD_START(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // Filter start flag, write only, write 1 to start the filter :
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_filt_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_filt_cmd : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
        };

|

.. _udma_filter_REG_STATUS:

REG_STATUS
""""""""""

FILTER status register

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                     Description                                                     |
    +=====+===+====+=====================================================================================================================+
    |    0|R/W|DONE|Filter done flag, write 1 to clear the flag : -1’b0: Filter process is not finished -1’b1: Filter process is finished|
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FILTER status register
                #define UDMA_FILTER_REG_STATUS_OFFSET            0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_filter_reg_status_get(uint32_t base);
        static inline void udma_filter_reg_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Filter done flag, write 1 to clear the flag : -1b0: Filter process is not finished -1b1: Filter process is finished (access: R/W)
        #define UDMA_FILTER_REG_STATUS_DONE_BIT                              0
        #define UDMA_FILTER_REG_STATUS_DONE_WIDTH                            1
        #define UDMA_FILTER_REG_STATUS_DONE_MASK                             0x1
        #define UDMA_FILTER_REG_STATUS_DONE_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_FILTER_REG_STATUS_DONE_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_FILTER_REG_STATUS_DONE(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int done            :1 ; // Filter done flag, write 1 to clear the flag : -1b0: Filter process is not finished -1b1: Filter process is finished
          };
          unsigned int raw;
        } __attribute__((packed)) udma_filter_reg_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_filter_reg_status : public vp::reg_32
        {
        public:
            inline void done_set(uint32_t value);
            inline uint32_t done_get();
        };

|
