Input file: README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |                      Name                      |Offset|Width|                        Description                        |
    +================================================+======+=====+===========================================================+
    |:ref:`SLOT_CFG_0<udma_i2s_SLOT_CFG_0>`          |     0|   32|Configuration for slot 0                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_1<udma_i2s_SLOT_CFG_1>`          |     4|   32|Configuration for slot 1                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_2<udma_i2s_SLOT_CFG_2>`          |     8|   32|Configuration for slot 2                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_3<udma_i2s_SLOT_CFG_3>`          |    12|   32|Configuration for slot 3                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_4<udma_i2s_SLOT_CFG_4>`          |    16|   32|Configuration for slot 4                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_5<udma_i2s_SLOT_CFG_5>`          |    20|   32|Configuration for slot 5                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_6<udma_i2s_SLOT_CFG_6>`          |    24|   32|Configuration for slot 6                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_7<udma_i2s_SLOT_CFG_7>`          |    28|   32|Configuration for slot 7                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_8<udma_i2s_SLOT_CFG_8>`          |    32|   32|Configuration for slot 8                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_9<udma_i2s_SLOT_CFG_9>`          |    36|   32|Configuration for slot 9                                   |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_10<udma_i2s_SLOT_CFG_10>`        |    40|   32|Configuration for slot 10                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_11<udma_i2s_SLOT_CFG_11>`        |    44|   32|Configuration for slot 11                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_12<udma_i2s_SLOT_CFG_12>`        |    48|   32|Configuration for slot 12                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_13<udma_i2s_SLOT_CFG_13>`        |    52|   32|Configuration for slot 13                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_14<udma_i2s_SLOT_CFG_14>`        |    56|   32|Configuration for slot 14                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_CFG_15<udma_i2s_SLOT_CFG_15>`        |    60|   32|Configuration for slot 15                                  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_0_1<udma_i2s_WORD_SIZE_0_1>`    |    64|   32|Defines word size for RX and TX channels for slot 0 and 1  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_2_3<udma_i2s_WORD_SIZE_2_3>`    |    68|   32|Defines word size for RX and TX channels for slot 2 and 3  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_4_5<udma_i2s_WORD_SIZE_4_5>`    |    72|   32|Defines word size for RX and TX channels for slot 4 and 5  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_6_7<udma_i2s_WORD_SIZE_6_7>`    |    76|   32|Defines word size for RX and TX channels for slot 6 and 7  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_8_9<udma_i2s_WORD_SIZE_8_9>`    |    80|   32|Defines word size for RX and TX channels for slot 8 and 9  |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_10_11<udma_i2s_WORD_SIZE_10_11>`|    84|   32|Defines word size for RX and TX channels for slot 10 and 11|
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_12_13<udma_i2s_WORD_SIZE_12_13>`|    88|   32|Defines word size for RX and TX channels for slot 12 and 13|
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`WORD_SIZE_14_15<udma_i2s_WORD_SIZE_14_15>`|    92|   32|Defines word size for RX and TX channels for slot 14 and 15|
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SLOT_EN<udma_i2s_SLOT_EN>`                |    96|   32|Slot Enable                                                |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`CLKCFG_SETUP<udma_i2s_CLKCFG_SETUP>`      |   100|   32|Clock configuration for both master, slave and pdm         |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`GLB_SETUP<udma_i2s_GLB_SETUP>`            |   104|   32|Configuration of the global parameters                     |
    +------------------------------------------------+------+-----+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 0
                #define UDMA_I2S_SLOT_CFG_0_OFFSET               0x0
        
                // Configuration for slot 1
                #define UDMA_I2S_SLOT_CFG_1_OFFSET               0x4
        
                // Configuration for slot 2
                #define UDMA_I2S_SLOT_CFG_2_OFFSET               0x8
        
                // Configuration for slot 3
                #define UDMA_I2S_SLOT_CFG_3_OFFSET               0xc
        
                // Configuration for slot 4
                #define UDMA_I2S_SLOT_CFG_4_OFFSET               0x10
        
                // Configuration for slot 5
                #define UDMA_I2S_SLOT_CFG_5_OFFSET               0x14
        
                // Configuration for slot 6
                #define UDMA_I2S_SLOT_CFG_6_OFFSET               0x18
        
                // Configuration for slot 7
                #define UDMA_I2S_SLOT_CFG_7_OFFSET               0x1c
        
                // Configuration for slot 8
                #define UDMA_I2S_SLOT_CFG_8_OFFSET               0x20
        
                // Configuration for slot 9
                #define UDMA_I2S_SLOT_CFG_9_OFFSET               0x24
        
                // Configuration for slot 10
                #define UDMA_I2S_SLOT_CFG_10_OFFSET              0x28
        
                // Configuration for slot 11
                #define UDMA_I2S_SLOT_CFG_11_OFFSET              0x2c
        
                // Configuration for slot 12
                #define UDMA_I2S_SLOT_CFG_12_OFFSET              0x30
        
                // Configuration for slot 13
                #define UDMA_I2S_SLOT_CFG_13_OFFSET              0x34
        
                // Configuration for slot 14
                #define UDMA_I2S_SLOT_CFG_14_OFFSET              0x38
        
                // Configuration for slot 15
                #define UDMA_I2S_SLOT_CFG_15_OFFSET              0x3c
        
                // Defines word size for RX and TX channels for slot 0 and 1
                #define UDMA_I2S_WORD_SIZE_0_1_OFFSET            0x40
        
                // Defines word size for RX and TX channels for slot 2 and 3
                #define UDMA_I2S_WORD_SIZE_2_3_OFFSET            0x44
        
                // Defines word size for RX and TX channels for slot 4 and 5
                #define UDMA_I2S_WORD_SIZE_4_5_OFFSET            0x48
        
                // Defines word size for RX and TX channels for slot 6 and 7
                #define UDMA_I2S_WORD_SIZE_6_7_OFFSET            0x4c
        
                // Defines word size for RX and TX channels for slot 8 and 9
                #define UDMA_I2S_WORD_SIZE_8_9_OFFSET            0x50
        
                // Defines word size for RX and TX channels for slot 10 and 11
                #define UDMA_I2S_WORD_SIZE_10_11_OFFSET          0x54
        
                // Defines word size for RX and TX channels for slot 12 and 13
                #define UDMA_I2S_WORD_SIZE_12_13_OFFSET          0x58
        
                // Defines word size for RX and TX channels for slot 14 and 15
                #define UDMA_I2S_WORD_SIZE_14_15_OFFSET          0x5c
        
                // Slot Enable
                #define UDMA_I2S_SLOT_EN_OFFSET                  0x60
        
                // Clock configuration for both master, slave and pdm
                #define UDMA_I2S_CLKCFG_SETUP_OFFSET             0x64
        
                // Configuration of the global parameters
                #define UDMA_I2S_GLB_SETUP_OFFSET                0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_0_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_1_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_2_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_3_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_4_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_4_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_5_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_6_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_6_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_7_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_7_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_8_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_8_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_9_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_9_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_10_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_10_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_11_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_11_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_12_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_12_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_13_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_13_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_14_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_14_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_cfg_15_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_15_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_0_1_get(uint32_t base);
        static inline void udma_i2s_word_size_0_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_2_3_get(uint32_t base);
        static inline void udma_i2s_word_size_2_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_4_5_get(uint32_t base);
        static inline void udma_i2s_word_size_4_5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_6_7_get(uint32_t base);
        static inline void udma_i2s_word_size_6_7_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_8_9_get(uint32_t base);
        static inline void udma_i2s_word_size_8_9_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_10_11_get(uint32_t base);
        static inline void udma_i2s_word_size_10_11_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_12_13_get(uint32_t base);
        static inline void udma_i2s_word_size_12_13_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_word_size_14_15_get(uint32_t base);
        static inline void udma_i2s_word_size_14_15_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_slot_en_get(uint32_t base);
        static inline void udma_i2s_slot_en_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_clkcfg_setup_get(uint32_t base);
        static inline void udma_i2s_clkcfg_setup_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2s_glb_setup_get(uint32_t base);
        static inline void udma_i2s_glb_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_0_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_0_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_0_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_1_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_1_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_1_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_2_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_2_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_2_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_3_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_3_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_3_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_4_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_4_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_4_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_5_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_5_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_5_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_6_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_6_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_6_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_7_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_7_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_7_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_8_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_8_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_8_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_9_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_9_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_9_BYP_RESET                                0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_10_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_10_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_10_BYP_RESET                               0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_11_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_11_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_11_BYP_RESET                               0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_12_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_12_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_12_BYP_RESET                               0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_13_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_13_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_13_BYP_RESET                               0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_14_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_14_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_14_BYP_RESET                               0x0
        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_15_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_15_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_15_BYP_RESET                               0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_RESET                  0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_RESET                  0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_RESET                  0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_RESET                  0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_RESET                  0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_RESET                0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_RESET                0x0
        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_RESET                0x0
        
        // Enable for slot 0 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_BIT                               0
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_MASK                              0x1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_RESET                             0x0
        
        // Enable for slot 1 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_BIT                               1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_MASK                              0x2
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_RESET                             0x0
        
        // Enable for slot 2 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_BIT                               2
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_MASK                              0x4
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_RESET                             0x0
        
        // Enable for slot 3 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_BIT                               3
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_MASK                              0x8
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_RESET                             0x0
        
        // Enable for slot 4 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_BIT                               4
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_MASK                              0x10
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_RESET                             0x0
        
        // Enable for slot 5 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_BIT                               5
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_MASK                              0x20
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_RESET                             0x0
        
        // Enable for slot 6 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_BIT                               6
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_MASK                              0x40
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_RESET                             0x0
        
        // Enable for slot 7 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_BIT                               7
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_MASK                              0x80
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_RESET                             0x0
        
        // Enable for slot 8 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_BIT                               8
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_MASK                              0x100
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_RESET                             0x0
        
        // Enable for slot 9 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_BIT                               9
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_MASK                              0x200
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_RESET                             0x0
        
        // Enable for slot 10 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_BIT                              10
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_MASK                             0x400
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_RESET                            0x0
        
        // Enable for slot 11 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_BIT                              11
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_MASK                             0x800
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_RESET                            0x0
        
        // Enable for slot 12 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_BIT                              12
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_MASK                             0x1000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_RESET                            0x0
        
        // Enable for slot 13 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_BIT                              13
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_MASK                             0x2000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_RESET                            0x0
        
        // Enable for slot 14 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_BIT                              14
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_MASK                             0x4000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_RESET                            0x0
        
        // Enable for slot 15 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_BIT                              15
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_MASK                             0x8000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_RESET                            0x0
        
        // Clock Divider (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_BIT                            0
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_WIDTH                          16
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_MASK                           0xffff
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_RESET                          0x0
        
        // Enables Clock Generator (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_BIT                             16
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_WIDTH                           1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_MASK                            0x10000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_RESET                           0x0
        
        // Clock internal or external     - 1b0 : internal    - 1b1 : external (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_BIT                            17
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_WIDTH                          1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_MASK                           0x20000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_RESET                          0x0
        
        // Clock External Source     - 1b0 : pad    - 1b1 : internally routed (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_BIT                        18
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_WIDTH                      1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_MASK                       0x40000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_RESET                      0x0
        
        // Sets the clock edge for the data driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_BIT                           19
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_WIDTH                         1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_MASK                          0x80000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_RESET                         0x0
        
        // WS internal or external     - 1b0 : internal    - 1b1 : external (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_BIT                             20
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_WIDTH                           1
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_MASK                            0x100000
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_RESET                           0x0
        
        // WS External Source     - 1b0 : pad    - 1b1 : internally routed (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_BIT                         21
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_WIDTH                       1
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_MASK                        0x200000
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_RESET                       0x0
        
        // Sets the clock edge for the WS driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_BIT                            22
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_WIDTH                          1
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_MASK                           0x400000
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_RESET                          0x0
        
        // WS Type:     - 2b00 : pulse            - 2b01 : state       - 2b10 : N/2 slot       - 1b11 : dont care (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_BIT                            23
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_WIDTH                          2
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_MASK                           0x1800000
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_RESET                          0x0
        
        // Enables the I2s interface (access: R/W)
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_BIT                             0
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_WIDTH                           1
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_MASK                            0x1
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_RESET                           0x0
        
        // Sets how many slots for each frame(1-16) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_BIT                          1
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_WIDTH                        4
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_MASK                         0x1e
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_RESET                        0x0
        
        // Sets the slot width in bits(1-32) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_BIT                            5
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_WIDTH                          5
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_MASK                           0x3e0
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_RESET                          0x0
        
        // Sets the distance in i2s cycles from the WS rising edge to the first bit of the frame (access: R/W)
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_BIT                              10
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_WIDTH                            3
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_MASK                             0x1c00
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_RESET                            0x0
        
        // Enables Full Duplex mode(SDI and SDO) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_BIT                        13
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_WIDTH                      1
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_MASK                       0x2000
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_RESET                      0x0
        
        // Enables the PDM mode in slave configuration (access: R/W)
        #define UDMA_I2S_GLB_SETUP_PDM_EN_BIT                                14
        #define UDMA_I2S_GLB_SETUP_PDM_EN_WIDTH                              1
        #define UDMA_I2S_GLB_SETUP_PDM_EN_MASK                               0x4000
        #define UDMA_I2S_GLB_SETUP_PDM_EN_RESET                              0x0
        
        // Enables both channels (access: R/W)
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_BIT                               15
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_WIDTH                             1
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_MASK                              0x8000
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_0_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_1_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_2_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_3_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_4_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_5_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_6_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_7_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_8_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_9_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP(val)                       ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_10_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_11_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_12_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_13_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_14_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_15_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP(val)                      ((val) << 31)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1(val)         ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1(val)         ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1(val)         ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1(val)         ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1(val)         ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1(val)       ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1(val)       ((val) << 24)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1(val)       ((val) << 24)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1(val)                    ((val) << 1)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2(val)                    ((val) << 2)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3(val)                    ((val) << 3)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_GET(value)              (GAP_BEXTRACTU((value),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_GETS(value)             (GAP_BEXTRACT((value),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_SET(value,field)        (GAP_BINSERT((value),(field),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4(val)                    ((val) << 4)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5(val)                    ((val) << 5)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6(val)                    ((val) << 6)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_GET(value)              (GAP_BEXTRACTU((value),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_GETS(value)             (GAP_BEXTRACT((value),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_SET(value,field)        (GAP_BINSERT((value),(field),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7(val)                    ((val) << 7)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_GET(value)              (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_GETS(value)             (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_SET(value,field)        (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9(val)                    ((val) << 9)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_GET(value)             (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_GETS(value)            (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_SET(value,field)       (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10(val)                   ((val) << 10)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_GET(value)             (GAP_BEXTRACTU((value),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_GETS(value)            (GAP_BEXTRACT((value),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_SET(value,field)       (GAP_BINSERT((value),(field),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11(val)                   ((val) << 11)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_GET(value)             (GAP_BEXTRACTU((value),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_GETS(value)            (GAP_BEXTRACT((value),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_SET(value,field)       (GAP_BINSERT((value),(field),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12(val)                   ((val) << 12)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_GET(value)             (GAP_BEXTRACTU((value),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_GETS(value)            (GAP_BEXTRACT((value),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_SET(value,field)       (GAP_BINSERT((value),(field),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13(val)                   ((val) << 13)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_GET(value)             (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_GETS(value)            (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_SET(value,field)       (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15(val)                   ((val) << 15)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_GET(value)           (GAP_BEXTRACTU((value),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_GETS(value)          (GAP_BEXTRACT((value),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_SET(value,field)     (GAP_BINSERT((value),(field),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV(val)                 ((val) << 0)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_GET(value)            (GAP_BEXTRACTU((value),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_GETS(value)           (GAP_BEXTRACT((value),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN(val)                  ((val) << 16)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_GET(value)           (GAP_BEXTRACTU((value),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_GETS(value)          (GAP_BEXTRACT((value),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_SET(value,field)     (GAP_BINSERT((value),(field),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC(val)                 ((val) << 17)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC(val)             ((val) << 18)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_GET(value)          (GAP_BEXTRACTU((value),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_GETS(value)         (GAP_BEXTRACT((value),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_SET(value,field)    (GAP_BINSERT((value),(field),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE(val)                ((val) << 19)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_GET(value)            (GAP_BEXTRACTU((value),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_GETS(value)           (GAP_BEXTRACT((value),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_SET(value,field)      (GAP_BINSERT((value),(field),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC(val)                  ((val) << 20)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_GET(value)        (GAP_BEXTRACTU((value),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_GETS(value)       (GAP_BEXTRACT((value),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_SET(value,field)  (GAP_BINSERT((value),(field),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC(val)              ((val) << 21)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE(val)                 ((val) << 22)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_GET(value)           (GAP_BEXTRACTU((value),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_GETS(value)          (GAP_BEXTRACT((value),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_SET(value,field)     (GAP_BINSERT((value),(field),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE(val)                 ((val) << 23)
        
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN(val)                  ((val) << 0)
        
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_GET(value)         (GAP_BEXTRACTU((value),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_GETS(value)        (GAP_BEXTRACT((value),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_SET(value,field)   (GAP_BINSERT((value),(field),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH(val)               ((val) << 1)
        
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_GET(value)           (GAP_BEXTRACTU((value),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_GETS(value)          (GAP_BEXTRACT((value),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_SET(value,field)     (GAP_BINSERT((value),(field),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH(val)                 ((val) << 5)
        
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_GET(value)             (GAP_BEXTRACTU((value),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_GETS(value)            (GAP_BEXTRACT((value),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_SET(value,field)       (GAP_BINSERT((value),(field),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY(val)                   ((val) << 10)
        
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_GET(value)       (GAP_BEXTRACTU((value),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_GETS(value)      (GAP_BEXTRACT((value),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_SET(value,field) (GAP_BINSERT((value),(field),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN(val)             ((val) << 13)
        
        #define UDMA_I2S_GLB_SETUP_PDM_EN_GET(value)               (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN_GETS(value)              (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN(val)                     ((val) << 14)
        
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_GET(value)              (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_GETS(value)             (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_SET(value,field)        (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH(val)                    ((val) << 15)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_I2S_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t slot_cfg_0;  // Configuration for slot 0
            volatile uint32_t slot_cfg_1;  // Configuration for slot 1
            volatile uint32_t slot_cfg_2;  // Configuration for slot 2
            volatile uint32_t slot_cfg_3;  // Configuration for slot 3
            volatile uint32_t slot_cfg_4;  // Configuration for slot 4
            volatile uint32_t slot_cfg_5;  // Configuration for slot 5
            volatile uint32_t slot_cfg_6;  // Configuration for slot 6
            volatile uint32_t slot_cfg_7;  // Configuration for slot 7
            volatile uint32_t slot_cfg_8;  // Configuration for slot 8
            volatile uint32_t slot_cfg_9;  // Configuration for slot 9
            volatile uint32_t slot_cfg_10;  // Configuration for slot 10
            volatile uint32_t slot_cfg_11;  // Configuration for slot 11
            volatile uint32_t slot_cfg_12;  // Configuration for slot 12
            volatile uint32_t slot_cfg_13;  // Configuration for slot 13
            volatile uint32_t slot_cfg_14;  // Configuration for slot 14
            volatile uint32_t slot_cfg_15;  // Configuration for slot 15
            volatile uint32_t word_size_0_1;  // Defines word size for RX and TX channels for slot 0 and 1
            volatile uint32_t word_size_2_3;  // Defines word size for RX and TX channels for slot 2 and 3
            volatile uint32_t word_size_4_5;  // Defines word size for RX and TX channels for slot 4 and 5
            volatile uint32_t word_size_6_7;  // Defines word size for RX and TX channels for slot 6 and 7
            volatile uint32_t word_size_8_9;  // Defines word size for RX and TX channels for slot 8 and 9
            volatile uint32_t word_size_10_11;  // Defines word size for RX and TX channels for slot 10 and 11
            volatile uint32_t word_size_12_13;  // Defines word size for RX and TX channels for slot 12 and 13
            volatile uint32_t word_size_14_15;  // Defines word size for RX and TX channels for slot 14 and 15
            volatile uint32_t slot_en;  // Slot Enable
            volatile uint32_t clkcfg_setup;  // Clock configuration for both master, slave and pdm
            volatile uint32_t glb_setup;  // Configuration of the global parameters
        } __attribute__((packed)) udma_i2s_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_0_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_1_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_2_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_3_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_4_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_5_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_6_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_7_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_8_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_9_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_10_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_11_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_12_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_13_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_14_t;
        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_15_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_0_1_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_2_3_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_4_5_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_6_7_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_8_9_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_10_11_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_12_13_t;
        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_14_15_t;
        
        typedef union {
          struct {
            unsigned int slot_en_0       :1 ; // Enable for slot 0
            unsigned int slot_en_1       :1 ; // Enable for slot 1
            unsigned int slot_en_2       :1 ; // Enable for slot 2
            unsigned int slot_en_3       :1 ; // Enable for slot 3
            unsigned int slot_en_4       :1 ; // Enable for slot 4
            unsigned int slot_en_5       :1 ; // Enable for slot 5
            unsigned int slot_en_6       :1 ; // Enable for slot 6
            unsigned int slot_en_7       :1 ; // Enable for slot 7
            unsigned int slot_en_8       :1 ; // Enable for slot 8
            unsigned int slot_en_9       :1 ; // Enable for slot 9
            unsigned int slot_en_10      :1 ; // Enable for slot 10
            unsigned int slot_en_11      :1 ; // Enable for slot 11
            unsigned int slot_en_12      :1 ; // Enable for slot 12
            unsigned int slot_en_13      :1 ; // Enable for slot 13
            unsigned int slot_en_14      :1 ; // Enable for slot 14
            unsigned int slot_en_15      :1 ; // Enable for slot 15
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_en_t;
        
        typedef union {
          struct {
            unsigned int clk_div         :16; // Clock Divider
            unsigned int clk_en          :1 ; // Enables Clock Generator
            unsigned int clk_src         :1 ; // Clock internal or external     - 1b0 : internal    - 1b1 : external
            unsigned int clk_ext_src     :1 ; // Clock External Source     - 1b0 : pad    - 1b1 : internally routed
            unsigned int clk_edge        :1 ; // Sets the clock edge for the data driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge
            unsigned int ws_src          :1 ; // WS internal or external     - 1b0 : internal    - 1b1 : external
            unsigned int ws_ext_src      :1 ; // WS External Source     - 1b0 : pad    - 1b1 : internally routed
            unsigned int ws_edge         :1 ; // Sets the clock edge for the WS driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge
            unsigned int ws_type         :2 ; // WS Type:     - 2b00 : pulse            - 2b01 : state       - 2b10 : N/2 slot       - 1b11 : dont care
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_clkcfg_setup_t;
        
        typedef union {
          struct {
            unsigned int global_en       :1 ; // Enables the I2s interface
            unsigned int frame_length    :4 ; // Sets how many slots for each frame(1-16)
            unsigned int slot_width      :5 ; // Sets the slot width in bits(1-32)
            unsigned int ws_delay        :3 ; // Sets the distance in i2s cycles from the WS rising edge to the first bit of the frame
            unsigned int full_duplex_en  :1 ; // Enables Full Duplex mode(SDI and SDO)
            unsigned int pdm_en          :1 ; // Enables the PDM mode in slave configuration
            unsigned int pdm_2ch         :1 ; // Enables both channels
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_glb_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_i2s : public vp::regmap
        {
        public:
            vp_udma_i2s_slot_cfg_0 slot_cfg_0;
            vp_udma_i2s_slot_cfg_1 slot_cfg_1;
            vp_udma_i2s_slot_cfg_2 slot_cfg_2;
            vp_udma_i2s_slot_cfg_3 slot_cfg_3;
            vp_udma_i2s_slot_cfg_4 slot_cfg_4;
            vp_udma_i2s_slot_cfg_5 slot_cfg_5;
            vp_udma_i2s_slot_cfg_6 slot_cfg_6;
            vp_udma_i2s_slot_cfg_7 slot_cfg_7;
            vp_udma_i2s_slot_cfg_8 slot_cfg_8;
            vp_udma_i2s_slot_cfg_9 slot_cfg_9;
            vp_udma_i2s_slot_cfg_10 slot_cfg_10;
            vp_udma_i2s_slot_cfg_11 slot_cfg_11;
            vp_udma_i2s_slot_cfg_12 slot_cfg_12;
            vp_udma_i2s_slot_cfg_13 slot_cfg_13;
            vp_udma_i2s_slot_cfg_14 slot_cfg_14;
            vp_udma_i2s_slot_cfg_15 slot_cfg_15;
            vp_udma_i2s_word_size_0_1 word_size_0_1;
            vp_udma_i2s_word_size_2_3 word_size_2_3;
            vp_udma_i2s_word_size_4_5 word_size_4_5;
            vp_udma_i2s_word_size_6_7 word_size_6_7;
            vp_udma_i2s_word_size_8_9 word_size_8_9;
            vp_udma_i2s_word_size_10_11 word_size_10_11;
            vp_udma_i2s_word_size_12_13 word_size_12_13;
            vp_udma_i2s_word_size_14_15 word_size_14_15;
            vp_udma_i2s_slot_en slot_en;
            vp_udma_i2s_clkcfg_setup clkcfg_setup;
            vp_udma_i2s_glb_setup glb_setup;
        };

|

.. _udma_i2s_SLOT_CFG_0:

SLOT_CFG_0
""""""""""

Configuration for slot 0

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 0
                #define UDMA_I2S_SLOT_CFG_0_OFFSET               0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_0_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_0_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_0_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_0_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_0_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_0_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_0_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_0_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_0_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_0_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_0_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_0_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_0_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_0_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_0_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_0_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_0_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_0_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_0_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_0 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_1:

SLOT_CFG_1
""""""""""

Configuration for slot 1

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 1
                #define UDMA_I2S_SLOT_CFG_1_OFFSET               0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_1_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_1_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_1_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_1_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_1_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_1_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_1_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_1_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_1_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_1_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_1_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_1_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_1_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_1_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_1_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_1_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_1_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_1_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_1_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_1 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_2:

SLOT_CFG_2
""""""""""

Configuration for slot 2

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 2
                #define UDMA_I2S_SLOT_CFG_2_OFFSET               0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_2_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_2_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_2_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_2_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_2_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_2_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_2_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_2_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_2_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_2_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_2_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_2_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_2_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_2_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_2_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_2_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_2_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_2_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_2_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_2 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_3:

SLOT_CFG_3
""""""""""

Configuration for slot 3

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 3
                #define UDMA_I2S_SLOT_CFG_3_OFFSET               0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_3_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_3_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_3_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_3_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_3_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_3_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_3_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_3_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_3_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_3_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_3_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_3_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_3_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_3_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_3_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_3_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_3_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_3_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_3_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_3 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_4:

SLOT_CFG_4
""""""""""

Configuration for slot 4

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 4
                #define UDMA_I2S_SLOT_CFG_4_OFFSET               0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_4_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_4_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_4_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_4_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_4_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_4_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_4_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_4_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_4_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_4_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_4_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_4_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_4_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_4_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_4_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_4_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_4_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_4_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_4_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_4 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_5:

SLOT_CFG_5
""""""""""

Configuration for slot 5

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 5
                #define UDMA_I2S_SLOT_CFG_5_OFFSET               0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_5_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_5_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_5_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_5_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_5_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_5_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_5_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_5_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_5_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_5_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_5_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_5_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_5_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_5_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_5_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_5_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_5_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_5_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_5_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_5 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_6:

SLOT_CFG_6
""""""""""

Configuration for slot 6

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 6
                #define UDMA_I2S_SLOT_CFG_6_OFFSET               0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_6_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_6_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_6_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_6_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_6_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_6_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_6_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_6_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_6_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_6_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_6_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_6_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_6_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_6_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_6_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_6_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_6_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_6_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_6_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_6 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_7:

SLOT_CFG_7
""""""""""

Configuration for slot 7

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 7
                #define UDMA_I2S_SLOT_CFG_7_OFFSET               0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_7_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_7_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_7_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_7_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_7_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_7_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_7_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_7_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_7_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_7_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_7_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_7_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_7_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_7_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_7_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_7_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_7_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_7_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_7_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_7 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_8:

SLOT_CFG_8
""""""""""

Configuration for slot 8

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 8
                #define UDMA_I2S_SLOT_CFG_8_OFFSET               0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_8_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_8_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_8_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_8_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_8_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_8_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_8_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_8_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_8_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_8_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_8_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_8_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_8_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_8_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_8_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_8_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_8_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_8_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_8_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_8 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_9:

SLOT_CFG_9
""""""""""

Configuration for slot 9

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 9
                #define UDMA_I2S_SLOT_CFG_9_OFFSET               0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_9_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_BIT                                0
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_MASK                               0xff
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_RESET                              0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_BIT                                8
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_MASK                               0x100
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_BIT                         9
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_MASK                        0x200
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_BIT                        10
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_MASK                       0x400
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_BIT                             12
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_MASK                            0x3000
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_BIT                              14
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_MASK                             0x4000
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_RESET                            0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_BIT                                16
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_WIDTH                              8
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_MASK                               0xff0000
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_RESET                              0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_BIT                                24
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_WIDTH                              1
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_MASK                               0x1000000
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_RESET                              0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_BIT                         25
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_WIDTH                       1
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_MASK                        0x2000000
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_RESET                       0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_BIT                        26
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_MASK                       0x4000000
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_RESET                      0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_BIT                             28
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_WIDTH                           2
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_MASK                            0x30000000
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_RESET                           0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_BIT                              30
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_WIDTH                            1
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_MASK                             0x40000000
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_RESET                            0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_9_BYP_BIT                                  31
        #define UDMA_I2S_SLOT_CFG_9_BYP_WIDTH                                1
        #define UDMA_I2S_SLOT_CFG_9_BYP_MASK                                 0x80000000
        #define UDMA_I2S_SLOT_CFG_9_BYP_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_9_ID_RX(val)                     ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_GET(value)               (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_GETS(value)              (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_9_RX_EN(val)                     ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_9_RX_MSB_FIRST(val)              ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_9_RX_LEFT_ALIGN(val)             ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_9_RX_DSIZE(val)                  ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_9_RX_SIGN(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_9_TX_ID(val)                     ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_GET(value)               (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_GETS(value)              (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_9_TX_EN(val)                     ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_GET(value)        (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_GETS(value)       (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST_SET(value,field)  (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_9_TX_MSB_FIRST(val)              ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_GET(value)       (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_GETS(value)      (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_9_TX_LEFT_ALIGN(val)             ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_GET(value)            (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_GETS(value)           (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_9_TX_DSIZE(val)                  ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_9_TX_SIGN(val)                   ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_9_BYP_GET(value)                 (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP_GETS(value)                (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP_SET(value,field)           (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_9_BYP(val)                       ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_9 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_10:

SLOT_CFG_10
"""""""""""

Configuration for slot 10

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 10
                #define UDMA_I2S_SLOT_CFG_10_OFFSET              0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_10_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_10_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_10_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_10_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_10_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_10_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_10_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_10_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_10_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_10_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_10_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_10_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_10_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_10_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_10_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_10_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_10_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_10_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_10_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_10_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_10_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_10 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_11:

SLOT_CFG_11
"""""""""""

Configuration for slot 11

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 11
                #define UDMA_I2S_SLOT_CFG_11_OFFSET              0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_11_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_11_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_11_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_11_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_11_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_11_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_11_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_11_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_11_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_11_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_11_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_11_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_11_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_11_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_11_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_11_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_11_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_11_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_11_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_11 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_12:

SLOT_CFG_12
"""""""""""

Configuration for slot 12

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 12
                #define UDMA_I2S_SLOT_CFG_12_OFFSET              0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_12_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_12_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_12_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_12_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_12_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_12_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_12_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_12_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_12_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_12_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_12_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_12_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_12_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_12_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_12_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_12_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_12_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_12_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_12_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_12 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_13:

SLOT_CFG_13
"""""""""""

Configuration for slot 13

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 13
                #define UDMA_I2S_SLOT_CFG_13_OFFSET              0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_13_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_13_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_13_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_13_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_13_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_13_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_13_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_13_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_13_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_13_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_13_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_13_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_13_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_13_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_13_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_13_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_13_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_13_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_13_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_13 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_14:

SLOT_CFG_14
"""""""""""

Configuration for slot 14

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 14
                #define UDMA_I2S_SLOT_CFG_14_OFFSET              0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_14_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_14_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_14_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_14_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_14_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_14_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_14_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_14_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_14_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_14_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_14_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_14_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_14_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_14_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_14_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_14_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_14_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_14_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_14_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_14_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_14_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_14 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_SLOT_CFG_15:

SLOT_CFG_15
"""""""""""

Configuration for slot 15

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                              Description                                               |
    +=====+===+=============+========================================================================================================+
    |7:0  |R/W|ID_RX        |uDMA Stream ID for RX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |8    |R/W|RX_EN        |Enable RX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |9    |R/W|RX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |10   |R/W|RX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |13:12|R/W|RX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |14   |R/W|RX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |23:16|R/W|TX_ID        |uDMA Stream ID for TX channel                                                                           |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |24   |R/W|TX_EN        |Enable TX for the slot                                                                                  |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |25   |R/W|TX_MSB_FIRST |Configuration of the MSB or LSB first:     - 1’b0 : LSB        - 1’b1 : MSB                             |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |26   |R/W|TX_LEFT_ALIGN|Justification to the left or to the right:     - 1’b0 : right     - 1’b1 : left                         |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |29:28|R/W|TX_DSIZE     |uDMA transfer size:     - 2’b00 : 1 byte     - 2’b01 : 2 bytes     - 2’b10 : 3 bytes   - 2’b11 : 4 bytes|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |30   |R/W|TX_SIGN      |Enables sign extension on RX data                                                                       |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+
    |31   |R/W|BYP          |Enables INput to OUTput Bypass                                                                          |
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for slot 15
                #define UDMA_I2S_SLOT_CFG_15_OFFSET              0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_cfg_15_get(uint32_t base);
        static inline void udma_i2s_slot_cfg_15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // uDMA Stream ID for RX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_BIT                               0
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_MASK                              0xff
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_RESET                             0x7f
        
        // Enable RX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_BIT                               8
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_MASK                              0x100
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_BIT                        9
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_MASK                       0x200
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_BIT                       10
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_MASK                      0x400
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_BIT                            12
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_MASK                           0x3000
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_BIT                             14
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_MASK                            0x4000
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_RESET                           0x0
        
        // uDMA Stream ID for TX channel (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_BIT                               16
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_WIDTH                             8
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_MASK                              0xff0000
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_RESET                             0x7f
        
        // Enable TX for the slot (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_BIT                               24
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_WIDTH                             1
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_MASK                              0x1000000
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_RESET                             0x0
        
        // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_BIT                        25
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_WIDTH                      1
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_MASK                       0x2000000
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_RESET                      0x0
        
        // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_BIT                       26
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_WIDTH                     1
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_MASK                      0x4000000
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_RESET                     0x0
        
        // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_BIT                            28
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_WIDTH                          2
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_MASK                           0x30000000
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_RESET                          0x0
        
        // Enables sign extension on RX data (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_BIT                             30
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_WIDTH                           1
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_MASK                            0x40000000
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_RESET                           0x0
        
        // Enables INput to OUTput Bypass (access: R/W)
        #define UDMA_I2S_SLOT_CFG_15_BYP_BIT                                 31
        #define UDMA_I2S_SLOT_CFG_15_BYP_WIDTH                               1
        #define UDMA_I2S_SLOT_CFG_15_BYP_MASK                                0x80000000
        #define UDMA_I2S_SLOT_CFG_15_BYP_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_I2S_SLOT_CFG_15_ID_RX(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_CFG_15_RX_EN(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_CFG_15_RX_MSB_FIRST(val)             ((val) << 9)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_CFG_15_RX_LEFT_ALIGN(val)            ((val) << 10)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define UDMA_I2S_SLOT_CFG_15_RX_DSIZE(val)                 ((val) << 12)
        
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_CFG_15_RX_SIGN(val)                  ((val) << 14)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define UDMA_I2S_SLOT_CFG_15_TX_ID(val)                    ((val) << 16)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_GET(value)              (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_GETS(value)             (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2S_SLOT_CFG_15_TX_EN(val)                    ((val) << 24)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_GET(value)       (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_GETS(value)      (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2S_SLOT_CFG_15_TX_MSB_FIRST(val)             ((val) << 25)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_I2S_SLOT_CFG_15_TX_LEFT_ALIGN(val)            ((val) << 26)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_GET(value)           (GAP_BEXTRACTU((value),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_GETS(value)          (GAP_BEXTRACT((value),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE_SET(value,field)     (GAP_BINSERT((value),(field),2,28))
        #define UDMA_I2S_SLOT_CFG_15_TX_DSIZE(val)                 ((val) << 28)
        
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define UDMA_I2S_SLOT_CFG_15_TX_SIGN(val)                  ((val) << 30)
        
        #define UDMA_I2S_SLOT_CFG_15_BYP_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_I2S_SLOT_CFG_15_BYP(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_rx           :8 ; // uDMA Stream ID for RX channel
            unsigned int rx_en           :1 ; // Enable RX for the slot
            unsigned int rx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int rx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding0:1 ;
            unsigned int rx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int rx_sign         :1 ; // Enables sign extension on RX data
            unsigned int padding1:1 ;
            unsigned int tx_id           :8 ; // uDMA Stream ID for TX channel
            unsigned int tx_en           :1 ; // Enable TX for the slot
            unsigned int tx_msb_first    :1 ; // Configuration of the MSB or LSB first:     - 1b0 : LSB        - 1b1 : MSB
            unsigned int tx_left_align   :1 ; // Justification to the left or to the right:     - 1b0 : right     - 1b1 : left
            unsigned int padding2:1 ;
            unsigned int tx_dsize        :2 ; // uDMA transfer size:     - 2b00 : 1 byte     - 2b01 : 2 bytes     - 2b10 : 3 bytes   - 2b11 : 4 bytes
            unsigned int tx_sign         :1 ; // Enables sign extension on RX data
            unsigned int byp             :1 ; // Enables INput to OUTput Bypass
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_cfg_15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_cfg_15 : public vp::reg_32
        {
        public:
            inline void id_rx_set(uint32_t value);
            inline uint32_t id_rx_get();
            inline void rx_en_set(uint32_t value);
            inline uint32_t rx_en_get();
            inline void rx_msb_first_set(uint32_t value);
            inline uint32_t rx_msb_first_get();
            inline void rx_left_align_set(uint32_t value);
            inline uint32_t rx_left_align_get();
            inline void rx_dsize_set(uint32_t value);
            inline uint32_t rx_dsize_get();
            inline void rx_sign_set(uint32_t value);
            inline uint32_t rx_sign_get();
            inline void tx_id_set(uint32_t value);
            inline uint32_t tx_id_get();
            inline void tx_en_set(uint32_t value);
            inline uint32_t tx_en_get();
            inline void tx_msb_first_set(uint32_t value);
            inline uint32_t tx_msb_first_get();
            inline void tx_left_align_set(uint32_t value);
            inline uint32_t tx_left_align_get();
            inline void tx_dsize_set(uint32_t value);
            inline uint32_t tx_dsize_get();
            inline void tx_sign_set(uint32_t value);
            inline uint32_t tx_sign_get();
            inline void byp_set(uint32_t value);
            inline uint32_t byp_get();
        };

|

.. _udma_i2s_WORD_SIZE_0_1:

WORD_SIZE_0_1
"""""""""""""

Defines word size for RX and TX channels for slot 0 and 1

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 0 and 1
                #define UDMA_I2S_WORD_SIZE_0_1_OFFSET            0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_0_1_get(uint32_t base);
        static inline void udma_i2s_word_size_0_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_0_1_WORD_SIZE_TX_1(val)         ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_0_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_0_1 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_2_3:

WORD_SIZE_2_3
"""""""""""""

Defines word size for RX and TX channels for slot 2 and 3

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 2 and 3
                #define UDMA_I2S_WORD_SIZE_2_3_OFFSET            0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_2_3_get(uint32_t base);
        static inline void udma_i2s_word_size_2_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_2_3_WORD_SIZE_TX_1(val)         ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_2_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_2_3 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_4_5:

WORD_SIZE_4_5
"""""""""""""

Defines word size for RX and TX channels for slot 4 and 5

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 4 and 5
                #define UDMA_I2S_WORD_SIZE_4_5_OFFSET            0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_4_5_get(uint32_t base);
        static inline void udma_i2s_word_size_4_5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_4_5_WORD_SIZE_TX_1(val)         ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_4_5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_4_5 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_6_7:

WORD_SIZE_6_7
"""""""""""""

Defines word size for RX and TX channels for slot 6 and 7

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 6 and 7
                #define UDMA_I2S_WORD_SIZE_6_7_OFFSET            0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_6_7_get(uint32_t base);
        static inline void udma_i2s_word_size_6_7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_6_7_WORD_SIZE_TX_1(val)         ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_6_7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_6_7 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_8_9:

WORD_SIZE_8_9
"""""""""""""

Defines word size for RX and TX channels for slot 8 and 9

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 8 and 9
                #define UDMA_I2S_WORD_SIZE_8_9_OFFSET            0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_8_9_get(uint32_t base);
        static inline void udma_i2s_word_size_8_9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_BIT                    0
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_MASK                   0x1f
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_RESET                  0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_BIT                    8
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_MASK                   0x1f00
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_RESET                  0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_BIT                    16
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_MASK                   0x1f0000
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_RESET                  0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_BIT                    24
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_WIDTH                  5
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_MASK                   0x1f000000
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_GET(value)   (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_GETS(value)  (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_0(val)         ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_GET(value)   (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_GETS(value)  (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_0(val)         ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_GET(value)   (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_GETS(value)  (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_RX_1(val)         ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_GET(value)   (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_GETS(value)  (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_8_9_WORD_SIZE_TX_1(val)         ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_8_9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_8_9 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_10_11:

WORD_SIZE_10_11
"""""""""""""""

Defines word size for RX and TX channels for slot 10 and 11

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 10 and 11
                #define UDMA_I2S_WORD_SIZE_10_11_OFFSET          0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_10_11_get(uint32_t base);
        static inline void udma_i2s_word_size_10_11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_RESET                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_10_11_WORD_SIZE_TX_1(val)       ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_10_11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_10_11 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_12_13:

WORD_SIZE_12_13
"""""""""""""""

Defines word size for RX and TX channels for slot 12 and 13

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 12 and 13
                #define UDMA_I2S_WORD_SIZE_12_13_OFFSET          0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_12_13_get(uint32_t base);
        static inline void udma_i2s_word_size_12_13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_RESET                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_12_13_WORD_SIZE_TX_1(val)       ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_12_13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_12_13 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_WORD_SIZE_14_15:

WORD_SIZE_14_15
"""""""""""""""

Defines word size for RX and TX channels for slot 14 and 15

.. table:: 

    +-----+---+--------------+-----------------------------------+
    |Bit #|R/W|     Name     |            Description            |
    +=====+===+==============+===================================+
    |4:0  |R/W|WORD_SIZE_RX_0|Word size for RX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |12:8 |R/W|WORD_SIZE_TX_0|Word size for TX channel of slots 0|
    +-----+---+--------------+-----------------------------------+
    |20:16|R/W|WORD_SIZE_RX_1|Word size for RX channel of slots 1|
    +-----+---+--------------+-----------------------------------+
    |28:24|R/W|WORD_SIZE_TX_1|Word size for TX channel of slots 1|
    +-----+---+--------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Defines word size for RX and TX channels for slot 14 and 15
                #define UDMA_I2S_WORD_SIZE_14_15_OFFSET          0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_word_size_14_15_get(uint32_t base);
        static inline void udma_i2s_word_size_14_15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Word size for RX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_BIT                  0
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_MASK                 0x1f
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_RESET                0x0
        
        // Word size for TX channel of slots 0 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_BIT                  8
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_MASK                 0x1f00
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_RESET                0x0
        
        // Word size for RX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_BIT                  16
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_MASK                 0x1f0000
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_RESET                0x0
        
        // Word size for TX channel of slots 1 (access: R/W)
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_BIT                  24
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_WIDTH                5
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_MASK                 0x1f000000
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_RESET                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_GET(value) (GAP_BEXTRACTU((value),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_GETS(value) (GAP_BEXTRACT((value),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_0(val)       ((val) << 0)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_GET(value) (GAP_BEXTRACTU((value),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_GETS(value) (GAP_BEXTRACT((value),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_0(val)       ((val) << 8)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_GET(value) (GAP_BEXTRACTU((value),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_GETS(value) (GAP_BEXTRACT((value),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1_SET(value,field) (GAP_BINSERT((value),(field),5,16))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_RX_1(val)       ((val) << 16)
        
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_GET(value) (GAP_BEXTRACTU((value),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_GETS(value) (GAP_BEXTRACT((value),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1_SET(value,field) (GAP_BINSERT((value),(field),5,24))
        #define UDMA_I2S_WORD_SIZE_14_15_WORD_SIZE_TX_1(val)       ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int word_size_rx_0  :5 ; // Word size for RX channel of slots 0
            unsigned int padding0:3 ;
            unsigned int word_size_tx_0  :5 ; // Word size for TX channel of slots 0
            unsigned int padding1:3 ;
            unsigned int word_size_rx_1  :5 ; // Word size for RX channel of slots 1
            unsigned int padding2:3 ;
            unsigned int word_size_tx_1  :5 ; // Word size for TX channel of slots 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_word_size_14_15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_word_size_14_15 : public vp::reg_32
        {
        public:
            inline void word_size_rx_0_set(uint32_t value);
            inline uint32_t word_size_rx_0_get();
            inline void word_size_tx_0_set(uint32_t value);
            inline uint32_t word_size_tx_0_get();
            inline void word_size_rx_1_set(uint32_t value);
            inline uint32_t word_size_rx_1_get();
            inline void word_size_tx_1_set(uint32_t value);
            inline uint32_t word_size_tx_1_get();
        };

|

.. _udma_i2s_SLOT_EN:

SLOT_EN
"""""""

Slot Enable

.. table:: 

    +-----+---+----------+------------------+
    |Bit #|R/W|   Name   |   Description    |
    +=====+===+==========+==================+
    |    0|R/W|SLOT_EN_0 |Enable for slot 0 |
    +-----+---+----------+------------------+
    |    1|R/W|SLOT_EN_1 |Enable for slot 1 |
    +-----+---+----------+------------------+
    |    2|R/W|SLOT_EN_2 |Enable for slot 2 |
    +-----+---+----------+------------------+
    |    3|R/W|SLOT_EN_3 |Enable for slot 3 |
    +-----+---+----------+------------------+
    |    4|R/W|SLOT_EN_4 |Enable for slot 4 |
    +-----+---+----------+------------------+
    |    5|R/W|SLOT_EN_5 |Enable for slot 5 |
    +-----+---+----------+------------------+
    |    6|R/W|SLOT_EN_6 |Enable for slot 6 |
    +-----+---+----------+------------------+
    |    7|R/W|SLOT_EN_7 |Enable for slot 7 |
    +-----+---+----------+------------------+
    |    8|R/W|SLOT_EN_8 |Enable for slot 8 |
    +-----+---+----------+------------------+
    |    9|R/W|SLOT_EN_9 |Enable for slot 9 |
    +-----+---+----------+------------------+
    |   10|R/W|SLOT_EN_10|Enable for slot 10|
    +-----+---+----------+------------------+
    |   11|R/W|SLOT_EN_11|Enable for slot 11|
    +-----+---+----------+------------------+
    |   12|R/W|SLOT_EN_12|Enable for slot 12|
    +-----+---+----------+------------------+
    |   13|R/W|SLOT_EN_13|Enable for slot 13|
    +-----+---+----------+------------------+
    |   14|R/W|SLOT_EN_14|Enable for slot 14|
    +-----+---+----------+------------------+
    |   15|R/W|SLOT_EN_15|Enable for slot 15|
    +-----+---+----------+------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Slot Enable
                #define UDMA_I2S_SLOT_EN_OFFSET                  0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_slot_en_get(uint32_t base);
        static inline void udma_i2s_slot_en_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable for slot 0 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_BIT                               0
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_MASK                              0x1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_RESET                             0x0
        
        // Enable for slot 1 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_BIT                               1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_MASK                              0x2
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_RESET                             0x0
        
        // Enable for slot 2 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_BIT                               2
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_MASK                              0x4
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_RESET                             0x0
        
        // Enable for slot 3 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_BIT                               3
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_MASK                              0x8
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_RESET                             0x0
        
        // Enable for slot 4 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_BIT                               4
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_MASK                              0x10
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_RESET                             0x0
        
        // Enable for slot 5 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_BIT                               5
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_MASK                              0x20
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_RESET                             0x0
        
        // Enable for slot 6 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_BIT                               6
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_MASK                              0x40
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_RESET                             0x0
        
        // Enable for slot 7 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_BIT                               7
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_MASK                              0x80
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_RESET                             0x0
        
        // Enable for slot 8 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_BIT                               8
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_MASK                              0x100
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_RESET                             0x0
        
        // Enable for slot 9 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_BIT                               9
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_WIDTH                             1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_MASK                              0x200
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_RESET                             0x0
        
        // Enable for slot 10 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_BIT                              10
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_MASK                             0x400
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_RESET                            0x0
        
        // Enable for slot 11 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_BIT                              11
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_MASK                             0x800
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_RESET                            0x0
        
        // Enable for slot 12 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_BIT                              12
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_MASK                             0x1000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_RESET                            0x0
        
        // Enable for slot 13 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_BIT                              13
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_MASK                             0x2000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_RESET                            0x0
        
        // Enable for slot 14 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_BIT                              14
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_MASK                             0x4000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_RESET                            0x0
        
        // Enable for slot 15 (access: R/W)
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_BIT                              15
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_WIDTH                            1
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_MASK                             0x8000
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_0(val)                    ((val) << 0)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_1(val)                    ((val) << 1)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_2(val)                    ((val) << 2)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_3(val)                    ((val) << 3)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_GET(value)              (GAP_BEXTRACTU((value),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_GETS(value)             (GAP_BEXTRACT((value),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4_SET(value,field)        (GAP_BINSERT((value),(field),1,4))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_4(val)                    ((val) << 4)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_5(val)                    ((val) << 5)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_6(val)                    ((val) << 6)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_GET(value)              (GAP_BEXTRACTU((value),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_GETS(value)             (GAP_BEXTRACT((value),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7_SET(value,field)        (GAP_BINSERT((value),(field),1,7))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_7(val)                    ((val) << 7)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_GET(value)              (GAP_BEXTRACTU((value),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_GETS(value)             (GAP_BEXTRACT((value),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8_SET(value,field)        (GAP_BINSERT((value),(field),1,8))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_8(val)                    ((val) << 8)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_GET(value)              (GAP_BEXTRACTU((value),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_GETS(value)             (GAP_BEXTRACT((value),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9_SET(value,field)        (GAP_BINSERT((value),(field),1,9))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_9(val)                    ((val) << 9)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_GET(value)             (GAP_BEXTRACTU((value),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_GETS(value)            (GAP_BEXTRACT((value),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10_SET(value,field)       (GAP_BINSERT((value),(field),1,10))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_10(val)                   ((val) << 10)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_GET(value)             (GAP_BEXTRACTU((value),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_GETS(value)            (GAP_BEXTRACT((value),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11_SET(value,field)       (GAP_BINSERT((value),(field),1,11))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_11(val)                   ((val) << 11)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_GET(value)             (GAP_BEXTRACTU((value),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_GETS(value)            (GAP_BEXTRACT((value),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12_SET(value,field)       (GAP_BINSERT((value),(field),1,12))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_12(val)                   ((val) << 12)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_GET(value)             (GAP_BEXTRACTU((value),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_GETS(value)            (GAP_BEXTRACT((value),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13_SET(value,field)       (GAP_BINSERT((value),(field),1,13))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_13(val)                   ((val) << 13)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_GET(value)             (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_GETS(value)            (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14_SET(value,field)       (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_14(val)                   ((val) << 14)
        
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_GET(value)             (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_GETS(value)            (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15_SET(value,field)       (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2S_SLOT_EN_SLOT_EN_15(val)                   ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int slot_en_0       :1 ; // Enable for slot 0
            unsigned int slot_en_1       :1 ; // Enable for slot 1
            unsigned int slot_en_2       :1 ; // Enable for slot 2
            unsigned int slot_en_3       :1 ; // Enable for slot 3
            unsigned int slot_en_4       :1 ; // Enable for slot 4
            unsigned int slot_en_5       :1 ; // Enable for slot 5
            unsigned int slot_en_6       :1 ; // Enable for slot 6
            unsigned int slot_en_7       :1 ; // Enable for slot 7
            unsigned int slot_en_8       :1 ; // Enable for slot 8
            unsigned int slot_en_9       :1 ; // Enable for slot 9
            unsigned int slot_en_10      :1 ; // Enable for slot 10
            unsigned int slot_en_11      :1 ; // Enable for slot 11
            unsigned int slot_en_12      :1 ; // Enable for slot 12
            unsigned int slot_en_13      :1 ; // Enable for slot 13
            unsigned int slot_en_14      :1 ; // Enable for slot 14
            unsigned int slot_en_15      :1 ; // Enable for slot 15
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_slot_en_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_slot_en : public vp::reg_32
        {
        public:
            inline void slot_en_0_set(uint32_t value);
            inline uint32_t slot_en_0_get();
            inline void slot_en_1_set(uint32_t value);
            inline uint32_t slot_en_1_get();
            inline void slot_en_2_set(uint32_t value);
            inline uint32_t slot_en_2_get();
            inline void slot_en_3_set(uint32_t value);
            inline uint32_t slot_en_3_get();
            inline void slot_en_4_set(uint32_t value);
            inline uint32_t slot_en_4_get();
            inline void slot_en_5_set(uint32_t value);
            inline uint32_t slot_en_5_get();
            inline void slot_en_6_set(uint32_t value);
            inline uint32_t slot_en_6_get();
            inline void slot_en_7_set(uint32_t value);
            inline uint32_t slot_en_7_get();
            inline void slot_en_8_set(uint32_t value);
            inline uint32_t slot_en_8_get();
            inline void slot_en_9_set(uint32_t value);
            inline uint32_t slot_en_9_get();
            inline void slot_en_10_set(uint32_t value);
            inline uint32_t slot_en_10_get();
            inline void slot_en_11_set(uint32_t value);
            inline uint32_t slot_en_11_get();
            inline void slot_en_12_set(uint32_t value);
            inline uint32_t slot_en_12_get();
            inline void slot_en_13_set(uint32_t value);
            inline uint32_t slot_en_13_get();
            inline void slot_en_14_set(uint32_t value);
            inline uint32_t slot_en_14_get();
            inline void slot_en_15_set(uint32_t value);
            inline uint32_t slot_en_15_get();
        };

|

.. _udma_i2s_CLKCFG_SETUP:

CLKCFG_SETUP
""""""""""""

Clock configuration for both master, slave and pdm

.. table:: 

    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                                Description                                                |
    +=====+===+===========+===========================================================================================================+
    |15:0 |R/W|CLK_DIV    |Clock Divider                                                                                              |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |16   |R/W|CLK_EN     |Enables Clock Generator                                                                                    |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |17   |R/W|CLK_SRC    |Clock internal or external     - 1’b0 : internal    - 1’b1 : external                                      |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |18   |R/W|CLK_EXT_SRC|Clock External Source     - 1’b0 : pad    - 1’b1 : internally routed                                       |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |19   |R/W|CLK_EDGE   |Sets the clock edge for the data driving or sampling     - 1’b0 : rising edge     - 1’b1 : falling edge    |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |20   |R/W|WS_SRC     |WS internal or external     - 1’b0 : internal    - 1’b1 : external                                         |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |21   |R/W|WS_EXT_SRC |WS External Source     - 1’b0 : pad    - 1’b1 : internally routed                                          |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |22   |R/W|WS_EDGE    |Sets the clock edge for the WS driving or sampling     - 1’b0 : rising edge     - 1’b1 : falling edge      |
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+
    |24:23|R/W|WS_TYPE    |WS Type:     - 2’b00 : pulse            - 2’b01 : state       - 2’b10 : N/2 slot       - 1’b11 : don’t care|
    +-----+---+-----------+-----------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock configuration for both master, slave and pdm
                #define UDMA_I2S_CLKCFG_SETUP_OFFSET             0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_clkcfg_setup_get(uint32_t base);
        static inline void udma_i2s_clkcfg_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock Divider (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_BIT                            0
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_WIDTH                          16
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_MASK                           0xffff
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_RESET                          0x0
        
        // Enables Clock Generator (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_BIT                             16
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_WIDTH                           1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_MASK                            0x10000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_RESET                           0x0
        
        // Clock internal or external     - 1b0 : internal    - 1b1 : external (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_BIT                            17
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_WIDTH                          1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_MASK                           0x20000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_RESET                          0x0
        
        // Clock External Source     - 1b0 : pad    - 1b1 : internally routed (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_BIT                        18
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_WIDTH                      1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_MASK                       0x40000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_RESET                      0x0
        
        // Sets the clock edge for the data driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_BIT                           19
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_WIDTH                         1
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_MASK                          0x80000
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_RESET                         0x0
        
        // WS internal or external     - 1b0 : internal    - 1b1 : external (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_BIT                             20
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_WIDTH                           1
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_MASK                            0x100000
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_RESET                           0x0
        
        // WS External Source     - 1b0 : pad    - 1b1 : internally routed (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_BIT                         21
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_WIDTH                       1
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_MASK                        0x200000
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_RESET                       0x0
        
        // Sets the clock edge for the WS driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_BIT                            22
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_WIDTH                          1
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_MASK                           0x400000
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_RESET                          0x0
        
        // WS Type:     - 2b00 : pulse            - 2b01 : state       - 2b10 : N/2 slot       - 1b11 : dont care (access: R/W)
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_BIT                            23
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_WIDTH                          2
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_MASK                           0x1800000
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_GET(value)           (GAP_BEXTRACTU((value),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_GETS(value)          (GAP_BEXTRACT((value),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV_SET(value,field)     (GAP_BINSERT((value),(field),16,0))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_DIV(val)                 ((val) << 0)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_GET(value)            (GAP_BEXTRACTU((value),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_GETS(value)           (GAP_BEXTRACT((value),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,16))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EN(val)                  ((val) << 16)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_GET(value)           (GAP_BEXTRACTU((value),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_GETS(value)          (GAP_BEXTRACT((value),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC_SET(value,field)     (GAP_BINSERT((value),(field),1,17))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_SRC(val)                 ((val) << 17)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_GET(value)       (GAP_BEXTRACTU((value),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_GETS(value)      (GAP_BEXTRACT((value),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EXT_SRC(val)             ((val) << 18)
        
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_GET(value)          (GAP_BEXTRACTU((value),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_GETS(value)         (GAP_BEXTRACT((value),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE_SET(value,field)    (GAP_BINSERT((value),(field),1,19))
        #define UDMA_I2S_CLKCFG_SETUP_CLK_EDGE(val)                ((val) << 19)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_GET(value)            (GAP_BEXTRACTU((value),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_GETS(value)           (GAP_BEXTRACT((value),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC_SET(value,field)      (GAP_BINSERT((value),(field),1,20))
        #define UDMA_I2S_CLKCFG_SETUP_WS_SRC(val)                  ((val) << 20)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_GET(value)        (GAP_BEXTRACTU((value),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_GETS(value)       (GAP_BEXTRACT((value),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC_SET(value,field)  (GAP_BINSERT((value),(field),1,21))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EXT_SRC(val)              ((val) << 21)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define UDMA_I2S_CLKCFG_SETUP_WS_EDGE(val)                 ((val) << 22)
        
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_GET(value)           (GAP_BEXTRACTU((value),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_GETS(value)          (GAP_BEXTRACT((value),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE_SET(value,field)     (GAP_BINSERT((value),(field),2,23))
        #define UDMA_I2S_CLKCFG_SETUP_WS_TYPE(val)                 ((val) << 23)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clk_div         :16; // Clock Divider
            unsigned int clk_en          :1 ; // Enables Clock Generator
            unsigned int clk_src         :1 ; // Clock internal or external     - 1b0 : internal    - 1b1 : external
            unsigned int clk_ext_src     :1 ; // Clock External Source     - 1b0 : pad    - 1b1 : internally routed
            unsigned int clk_edge        :1 ; // Sets the clock edge for the data driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge
            unsigned int ws_src          :1 ; // WS internal or external     - 1b0 : internal    - 1b1 : external
            unsigned int ws_ext_src      :1 ; // WS External Source     - 1b0 : pad    - 1b1 : internally routed
            unsigned int ws_edge         :1 ; // Sets the clock edge for the WS driving or sampling     - 1b0 : rising edge     - 1b1 : falling edge
            unsigned int ws_type         :2 ; // WS Type:     - 2b00 : pulse            - 2b01 : state       - 2b10 : N/2 slot       - 1b11 : dont care
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_clkcfg_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_clkcfg_setup : public vp::reg_32
        {
        public:
            inline void clk_div_set(uint32_t value);
            inline uint32_t clk_div_get();
            inline void clk_en_set(uint32_t value);
            inline uint32_t clk_en_get();
            inline void clk_src_set(uint32_t value);
            inline uint32_t clk_src_get();
            inline void clk_ext_src_set(uint32_t value);
            inline uint32_t clk_ext_src_get();
            inline void clk_edge_set(uint32_t value);
            inline uint32_t clk_edge_get();
            inline void ws_src_set(uint32_t value);
            inline uint32_t ws_src_get();
            inline void ws_ext_src_set(uint32_t value);
            inline uint32_t ws_ext_src_get();
            inline void ws_edge_set(uint32_t value);
            inline uint32_t ws_edge_get();
            inline void ws_type_set(uint32_t value);
            inline uint32_t ws_type_get();
        };

|

.. _udma_i2s_GLB_SETUP:

GLB_SETUP
"""""""""

Configuration of the global parameters

.. table:: 

    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                     Description                                     |
    +=====+===+==============+=====================================================================================+
    |    0|R/W|GLOBAL_EN     |Enables the I2s interface                                                            |
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |4:1  |R/W|FRAME_LENGTH  |Sets how many slots for each frame(1-16)                                             |
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |9:5  |R/W|SLOT_WIDTH    |Sets the slot width in bits(1-32)                                                    |
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |12:10|R/W|WS_DELAY      |Sets the distance in i2s cycles from the WS rising edge to the first bit of the frame|
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |13   |R/W|FULL_DUPLEX_EN|Enables Full Duplex mode(SDI and SDO)                                                |
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |14   |R/W|PDM_EN        |Enables the PDM mode in slave configuration                                          |
    +-----+---+--------------+-------------------------------------------------------------------------------------+
    |15   |R/W|PDM_2CH       |Enables both channels                                                                |
    +-----+---+--------------+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the global parameters
                #define UDMA_I2S_GLB_SETUP_OFFSET                0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2s_glb_setup_get(uint32_t base);
        static inline void udma_i2s_glb_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enables the I2s interface (access: R/W)
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_BIT                             0
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_WIDTH                           1
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_MASK                            0x1
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_RESET                           0x0
        
        // Sets how many slots for each frame(1-16) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_BIT                          1
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_WIDTH                        4
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_MASK                         0x1e
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_RESET                        0x0
        
        // Sets the slot width in bits(1-32) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_BIT                            5
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_WIDTH                          5
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_MASK                           0x3e0
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_RESET                          0x0
        
        // Sets the distance in i2s cycles from the WS rising edge to the first bit of the frame (access: R/W)
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_BIT                              10
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_WIDTH                            3
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_MASK                             0x1c00
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_RESET                            0x0
        
        // Enables Full Duplex mode(SDI and SDO) (access: R/W)
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_BIT                        13
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_WIDTH                      1
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_MASK                       0x2000
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_RESET                      0x0
        
        // Enables the PDM mode in slave configuration (access: R/W)
        #define UDMA_I2S_GLB_SETUP_PDM_EN_BIT                                14
        #define UDMA_I2S_GLB_SETUP_PDM_EN_WIDTH                              1
        #define UDMA_I2S_GLB_SETUP_PDM_EN_MASK                               0x4000
        #define UDMA_I2S_GLB_SETUP_PDM_EN_RESET                              0x0
        
        // Enables both channels (access: R/W)
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_BIT                               15
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_WIDTH                             1
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_MASK                              0x8000
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2S_GLB_SETUP_GLOBAL_EN(val)                  ((val) << 0)
        
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_GET(value)         (GAP_BEXTRACTU((value),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_GETS(value)        (GAP_BEXTRACT((value),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH_SET(value,field)   (GAP_BINSERT((value),(field),4,1))
        #define UDMA_I2S_GLB_SETUP_FRAME_LENGTH(val)               ((val) << 1)
        
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_GET(value)           (GAP_BEXTRACTU((value),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_GETS(value)          (GAP_BEXTRACT((value),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH_SET(value,field)     (GAP_BINSERT((value),(field),5,5))
        #define UDMA_I2S_GLB_SETUP_SLOT_WIDTH(val)                 ((val) << 5)
        
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_GET(value)             (GAP_BEXTRACTU((value),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_GETS(value)            (GAP_BEXTRACT((value),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY_SET(value,field)       (GAP_BINSERT((value),(field),3,10))
        #define UDMA_I2S_GLB_SETUP_WS_DELAY(val)                   ((val) << 10)
        
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_GET(value)       (GAP_BEXTRACTU((value),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_GETS(value)      (GAP_BEXTRACT((value),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN_SET(value,field) (GAP_BINSERT((value),(field),1,13))
        #define UDMA_I2S_GLB_SETUP_FULL_DUPLEX_EN(val)             ((val) << 13)
        
        #define UDMA_I2S_GLB_SETUP_PDM_EN_GET(value)               (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN_GETS(value)              (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2S_GLB_SETUP_PDM_EN(val)                     ((val) << 14)
        
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_GET(value)              (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_GETS(value)             (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH_SET(value,field)        (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2S_GLB_SETUP_PDM_2CH(val)                    ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int global_en       :1 ; // Enables the I2s interface
            unsigned int frame_length    :4 ; // Sets how many slots for each frame(1-16)
            unsigned int slot_width      :5 ; // Sets the slot width in bits(1-32)
            unsigned int ws_delay        :3 ; // Sets the distance in i2s cycles from the WS rising edge to the first bit of the frame
            unsigned int full_duplex_en  :1 ; // Enables Full Duplex mode(SDI and SDO)
            unsigned int pdm_en          :1 ; // Enables the PDM mode in slave configuration
            unsigned int pdm_2ch         :1 ; // Enables both channels
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2s_glb_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2s_glb_setup : public vp::reg_32
        {
        public:
            inline void global_en_set(uint32_t value);
            inline uint32_t global_en_get();
            inline void frame_length_set(uint32_t value);
            inline uint32_t frame_length_get();
            inline void slot_width_set(uint32_t value);
            inline uint32_t slot_width_get();
            inline void ws_delay_set(uint32_t value);
            inline uint32_t ws_delay_get();
            inline void full_duplex_en_set(uint32_t value);
            inline uint32_t full_duplex_en_get();
            inline void pdm_en_set(uint32_t value);
            inline uint32_t pdm_en_get();
            inline void pdm_2ch_set(uint32_t value);
            inline uint32_t pdm_2ch_get();
        };

|
