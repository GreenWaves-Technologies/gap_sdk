Input file: fe/ips/udma/udma_aes/doc/udma_aes_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------+------+-----+---------------------------------------------+
    |                 Name                 |Offset|Width|                 Description                 |
    +======================================+======+=====+=============================================+
    |:ref:`AES_KEY_0<udma_aes_AES_KEY_0>`  |     0|   32|bits[31:0] of aes key                        |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_1<udma_aes_AES_KEY_1>`  |     4|   32|bits[65:32] of aes key                       |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_2<udma_aes_AES_KEY_2>`  |     8|   32|bits[95:64] of aes key                       |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_3<udma_aes_AES_KEY_3>`  |    12|   32|bits[127:96] of aes key                      |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_4<udma_aes_AES_KEY_4>`  |    16|   32|bits[159:128] of aes key                     |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_5<udma_aes_AES_KEY_5>`  |    20|   32|bits[191:160] of aes key                     |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_6<udma_aes_AES_KEY_6>`  |    24|   32|bits[223:192] of aes key                     |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_KEY_7<udma_aes_AES_KEY_7>`  |    28|   32|bits[255:224] of aes key                     |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_IV_0<udma_aes_AES_IV_0>`    |    32|   32|bits[31:0] of IV used on first round of CBC  |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_IV_1<udma_aes_AES_IV_1>`    |    36|   32|bits[65:32] of IV used on first round of CBC |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_IV_2<udma_aes_AES_IV_2>`    |    40|   32|bits[95:64] of IV used on first round of CBC |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_IV_3<udma_aes_AES_IV_3>`    |    44|   32|bits[127:96] of IV used on first round of CBC|
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_ID_CFG<udma_aes_AES_ID_CFG>`|    48|   32|Stream ID of uDMA channels                   |
    +--------------------------------------+------+-----+---------------------------------------------+
    |:ref:`AES_SETUP<udma_aes_AES_SETUP>`  |    52|   32|Global configuration register                |
    +--------------------------------------+------+-----+---------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[31:0] of aes key
                #define UDMA_AES_AES_KEY_0_OFFSET                0x0
        
                // bits[65:32] of aes key
                #define UDMA_AES_AES_KEY_1_OFFSET                0x4
        
                // bits[95:64] of aes key
                #define UDMA_AES_AES_KEY_2_OFFSET                0x8
        
                // bits[127:96] of aes key
                #define UDMA_AES_AES_KEY_3_OFFSET                0xc
        
                // bits[159:128] of aes key
                #define UDMA_AES_AES_KEY_4_OFFSET                0x10
        
                // bits[191:160] of aes key
                #define UDMA_AES_AES_KEY_5_OFFSET                0x14
        
                // bits[223:192] of aes key
                #define UDMA_AES_AES_KEY_6_OFFSET                0x18
        
                // bits[255:224] of aes key
                #define UDMA_AES_AES_KEY_7_OFFSET                0x1c
        
                // bits[31:0] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_0_OFFSET                 0x20
        
                // bits[65:32] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_1_OFFSET                 0x24
        
                // bits[95:64] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_2_OFFSET                 0x28
        
                // bits[127:96] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_3_OFFSET                 0x2c
        
                // Stream ID of uDMA channels
                #define UDMA_AES_AES_ID_CFG_OFFSET               0x30
        
                // Global configuration register
                #define UDMA_AES_AES_SETUP_OFFSET                0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_0_get(uint32_t base);
        static inline void udma_aes_aes_key_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_1_get(uint32_t base);
        static inline void udma_aes_aes_key_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_2_get(uint32_t base);
        static inline void udma_aes_aes_key_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_3_get(uint32_t base);
        static inline void udma_aes_aes_key_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_4_get(uint32_t base);
        static inline void udma_aes_aes_key_4_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_5_get(uint32_t base);
        static inline void udma_aes_aes_key_5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_6_get(uint32_t base);
        static inline void udma_aes_aes_key_6_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_key_7_get(uint32_t base);
        static inline void udma_aes_aes_key_7_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_iv_0_get(uint32_t base);
        static inline void udma_aes_aes_iv_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_iv_1_get(uint32_t base);
        static inline void udma_aes_aes_iv_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_iv_2_get(uint32_t base);
        static inline void udma_aes_aes_iv_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_iv_3_get(uint32_t base);
        static inline void udma_aes_aes_iv_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_id_cfg_get(uint32_t base);
        static inline void udma_aes_aes_id_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_aes_setup_get(uint32_t base);
        static inline void udma_aes_aes_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[31:0] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_0_KEY_0_BIT                                 0
        #define UDMA_AES_AES_KEY_0_KEY_0_WIDTH                               32
        #define UDMA_AES_AES_KEY_0_KEY_0_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_0_KEY_0_RESET                               0x0
        
        // bits[65:32] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_1_KEY_1_BIT                                 0
        #define UDMA_AES_AES_KEY_1_KEY_1_WIDTH                               32
        #define UDMA_AES_AES_KEY_1_KEY_1_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_1_KEY_1_RESET                               0x0
        
        // bits[65:32] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_2_KEY_2_BIT                                 0
        #define UDMA_AES_AES_KEY_2_KEY_2_WIDTH                               32
        #define UDMA_AES_AES_KEY_2_KEY_2_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_2_KEY_2_RESET                               0x0
        
        // bits[127:96] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_3_KEY_3_BIT                                 0
        #define UDMA_AES_AES_KEY_3_KEY_3_WIDTH                               32
        #define UDMA_AES_AES_KEY_3_KEY_3_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_3_KEY_3_RESET                               0x0
        
        // bits[159:128] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_4_KEY_4_BIT                                 0
        #define UDMA_AES_AES_KEY_4_KEY_4_WIDTH                               32
        #define UDMA_AES_AES_KEY_4_KEY_4_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_4_KEY_4_RESET                               0x0
        
        // bits[191:160] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_5_KEY_5_BIT                                 0
        #define UDMA_AES_AES_KEY_5_KEY_5_WIDTH                               32
        #define UDMA_AES_AES_KEY_5_KEY_5_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_5_KEY_5_RESET                               0x0
        
        // bits[223:192] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_6_KEY_6_BIT                                 0
        #define UDMA_AES_AES_KEY_6_KEY_6_WIDTH                               32
        #define UDMA_AES_AES_KEY_6_KEY_6_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_6_KEY_6_RESET                               0x0
        
        // bits[255:224] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_7_KEY_7_BIT                                 0
        #define UDMA_AES_AES_KEY_7_KEY_7_WIDTH                               32
        #define UDMA_AES_AES_KEY_7_KEY_7_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_7_KEY_7_RESET                               0x0
        
        // bits[31:0] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_0_IV_0_BIT                                   0
        #define UDMA_AES_AES_IV_0_IV_0_WIDTH                                 32
        #define UDMA_AES_AES_IV_0_IV_0_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_0_IV_0_RESET                                 0x0
        
        // bits[65:32] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_1_IV_1_BIT                                   0
        #define UDMA_AES_AES_IV_1_IV_1_WIDTH                                 32
        #define UDMA_AES_AES_IV_1_IV_1_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_1_IV_1_RESET                                 0x0
        
        // bits[95:64] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_2_IV_2_BIT                                   0
        #define UDMA_AES_AES_IV_2_IV_2_WIDTH                                 32
        #define UDMA_AES_AES_IV_2_IV_2_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_2_IV_2_RESET                                 0x0
        
        // bits[127:96] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_3_IV_3_BIT                                   0
        #define UDMA_AES_AES_IV_3_IV_3_WIDTH                                 32
        #define UDMA_AES_AES_IV_3_IV_3_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_3_IV_3_RESET                                 0x0
        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_AES_AES_ID_CFG_RX_DEST_BIT                              0
        #define UDMA_AES_AES_ID_CFG_RX_DEST_WIDTH                            7
        #define UDMA_AES_AES_ID_CFG_RX_DEST_MASK                             0x7f
        #define UDMA_AES_AES_ID_CFG_RX_DEST_RESET                            0x7f
        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_AES_AES_ID_CFG_TX_DEST_BIT                              8
        #define UDMA_AES_AES_ID_CFG_TX_DEST_WIDTH                            7
        #define UDMA_AES_AES_ID_CFG_TX_DEST_MASK                             0x7f00
        #define UDMA_AES_AES_ID_CFG_TX_DEST_RESET                            0x7f
        
        // When writing a 1 enables the key init round. Read returns 1 when the key init has terminated (access: R/W)
        #define UDMA_AES_AES_SETUP_KEY_INIT_BIT                              0
        #define UDMA_AES_AES_SETUP_KEY_INIT_WIDTH                            1
        #define UDMA_AES_AES_SETUP_KEY_INIT_MASK                             0x1
        #define UDMA_AES_AES_SETUP_KEY_INIT_RESET                            0x0
        
        // AES Key Length: - 0: 128bits - 1: 256bits (access: R/W)
        #define UDMA_AES_AES_SETUP_KEY_LEN_BIT                               1
        #define UDMA_AES_AES_SETUP_KEY_LEN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_KEY_LEN_MASK                              0x2
        #define UDMA_AES_AES_SETUP_KEY_LEN_RESET                             0x0
        
        // ENC/DEC behaviour: - 0: Decode - 1: Encode (access: R/W)
        #define UDMA_AES_AES_SETUP_ENCDECN_BIT                               2
        #define UDMA_AES_AES_SETUP_ENCDECN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_ENCDECN_MASK                              0x4
        #define UDMA_AES_AES_SETUP_ENCDECN_RESET                             0x0
        
        // ENC/DEC type: - 0: ECB - 1: CBC (access: R/W)
        #define UDMA_AES_AES_SETUP_CBCECBN_BIT                               3
        #define UDMA_AES_AES_SETUP_CBCECBN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_CBCECBN_MASK                              0x8
        #define UDMA_AES_AES_SETUP_CBCECBN_RESET                             0x0
        
        // When set and CBC is enabled triggers a block restart using IV as first xor (access: W)
        #define UDMA_AES_AES_SETUP_BLK_RESTART_BIT                           4
        #define UDMA_AES_AES_SETUP_BLK_RESTART_WIDTH                         1
        #define UDMA_AES_AES_SETUP_BLK_RESTART_MASK                          0x10
        #define UDMA_AES_AES_SETUP_BLK_RESTART_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_0_KEY_0_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_1_KEY_1_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_2_KEY_2_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_3_KEY_3_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_4_KEY_4_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_5_KEY_5_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_6_KEY_6_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_KEY_7_KEY_7_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7(val)                      ((val) << 0)
        
        #define UDMA_AES_AES_IV_0_IV_0_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_0_IV_0_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_0_IV_0_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_0_IV_0(val)                        ((val) << 0)
        
        #define UDMA_AES_AES_IV_1_IV_1_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_1_IV_1_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_1_IV_1_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_1_IV_1(val)                        ((val) << 0)
        
        #define UDMA_AES_AES_IV_2_IV_2_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_2_IV_2_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_2_IV_2_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_2_IV_2(val)                        ((val) << 0)
        
        #define UDMA_AES_AES_IV_3_IV_3_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_3_IV_3_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_3_IV_3_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_3_IV_3(val)                        ((val) << 0)
        
        #define UDMA_AES_AES_ID_CFG_RX_DEST_GET(value)             (GAP_BEXTRACTU((value),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST_GETS(value)            (GAP_BEXTRACT((value),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST(val)                   ((val) << 0)
        
        #define UDMA_AES_AES_ID_CFG_TX_DEST_GET(value)             (GAP_BEXTRACTU((value),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST_GETS(value)            (GAP_BEXTRACT((value),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST(val)                   ((val) << 8)
        
        #define UDMA_AES_AES_SETUP_KEY_INIT_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT(val)                   ((val) << 0)
        
        #define UDMA_AES_AES_SETUP_KEY_LEN_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN(val)                    ((val) << 1)
        
        #define UDMA_AES_AES_SETUP_ENCDECN_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN(val)                    ((val) << 2)
        
        #define UDMA_AES_AES_SETUP_CBCECBN_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN(val)                    ((val) << 3)
        
        #define UDMA_AES_AES_SETUP_BLK_RESTART_GET(value)          (GAP_BEXTRACTU((value),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART_GETS(value)         (GAP_BEXTRACT((value),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART_SET(value,field)    (GAP_BINSERT((value),(field),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART(val)                ((val) << 4)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_AES_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t aes_key_0;  // bits[31:0] of aes key
            volatile uint32_t aes_key_1;  // bits[65:32] of aes key
            volatile uint32_t aes_key_2;  // bits[95:64] of aes key
            volatile uint32_t aes_key_3;  // bits[127:96] of aes key
            volatile uint32_t aes_key_4;  // bits[159:128] of aes key
            volatile uint32_t aes_key_5;  // bits[191:160] of aes key
            volatile uint32_t aes_key_6;  // bits[223:192] of aes key
            volatile uint32_t aes_key_7;  // bits[255:224] of aes key
            volatile uint32_t aes_iv_0;  // bits[31:0] of IV used on first round of CBC
            volatile uint32_t aes_iv_1;  // bits[65:32] of IV used on first round of CBC
            volatile uint32_t aes_iv_2;  // bits[95:64] of IV used on first round of CBC
            volatile uint32_t aes_iv_3;  // bits[127:96] of IV used on first round of CBC
            volatile uint32_t aes_id_cfg;  // Stream ID of uDMA channels
            volatile uint32_t aes_setup;  // Global configuration register
        } __attribute__((packed)) udma_aes_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_0           :32; // bits[31:0] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_0_t;
        
        typedef union {
          struct {
            unsigned int key_1           :32; // bits[65:32] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_1_t;
        
        typedef union {
          struct {
            unsigned int key_2           :32; // bits[65:32] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_2_t;
        
        typedef union {
          struct {
            unsigned int key_3           :32; // bits[127:96] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_3_t;
        
        typedef union {
          struct {
            unsigned int key_4           :32; // bits[159:128] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_4_t;
        
        typedef union {
          struct {
            unsigned int key_5           :32; // bits[191:160] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_5_t;
        
        typedef union {
          struct {
            unsigned int key_6           :32; // bits[223:192] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_6_t;
        
        typedef union {
          struct {
            unsigned int key_7           :32; // bits[255:224] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_7_t;
        
        typedef union {
          struct {
            unsigned int iv_0            :32; // bits[31:0] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_0_t;
        
        typedef union {
          struct {
            unsigned int iv_1            :32; // bits[65:32] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_1_t;
        
        typedef union {
          struct {
            unsigned int iv_2            :32; // bits[95:64] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_2_t;
        
        typedef union {
          struct {
            unsigned int iv_3            :32; // bits[127:96] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_3_t;
        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled)
            unsigned int padding0:1 ;
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_id_cfg_t;
        
        typedef union {
          struct {
            unsigned int key_init        :1 ; // When writing a 1 enables the key init round. Read returns 1 when the key init has terminated
            unsigned int key_len         :1 ; // AES Key Length: - 0: 128bits - 1: 256bits
            unsigned int encdecn         :1 ; // ENC/DEC behaviour: - 0: Decode - 1: Encode
            unsigned int cbcecbn         :1 ; // ENC/DEC type: - 0: ECB - 1: CBC
            unsigned int blk_restart     :1 ; // When set and CBC is enabled triggers a block restart using IV as first xor
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_aes : public vp::regmap
        {
        public:
            vp_udma_aes_aes_key_0 aes_key_0;
            vp_udma_aes_aes_key_1 aes_key_1;
            vp_udma_aes_aes_key_2 aes_key_2;
            vp_udma_aes_aes_key_3 aes_key_3;
            vp_udma_aes_aes_key_4 aes_key_4;
            vp_udma_aes_aes_key_5 aes_key_5;
            vp_udma_aes_aes_key_6 aes_key_6;
            vp_udma_aes_aes_key_7 aes_key_7;
            vp_udma_aes_aes_iv_0 aes_iv_0;
            vp_udma_aes_aes_iv_1 aes_iv_1;
            vp_udma_aes_aes_iv_2 aes_iv_2;
            vp_udma_aes_aes_iv_3 aes_iv_3;
            vp_udma_aes_aes_id_cfg aes_id_cfg;
            vp_udma_aes_aes_setup aes_setup;
        };

|

.. _udma_aes_AES_KEY_0:

AES_KEY_0
"""""""""

bits[31:0] of aes key

.. table:: 

    +-----+---+-----+---------------------+
    |Bit #|R/W|Name |     Description     |
    +=====+===+=====+=====================+
    |31:0 |R/W|KEY_0|bits[31:0] of aes key|
    +-----+---+-----+---------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[31:0] of aes key
                #define UDMA_AES_AES_KEY_0_OFFSET                0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_0_get(uint32_t base);
        static inline void udma_aes_aes_key_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[31:0] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_0_KEY_0_BIT                                 0
        #define UDMA_AES_AES_KEY_0_KEY_0_WIDTH                               32
        #define UDMA_AES_AES_KEY_0_KEY_0_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_0_KEY_0_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_0_KEY_0_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_0_KEY_0(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_0           :32; // bits[31:0] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_0 : public vp::reg_32
        {
        public:
            inline void key_0_set(uint32_t value);
            inline uint32_t key_0_get();
        };

|

.. _udma_aes_AES_KEY_1:

AES_KEY_1
"""""""""

bits[65:32] of aes key

.. table:: 

    +-----+---+-----+----------------------+
    |Bit #|R/W|Name |     Description      |
    +=====+===+=====+======================+
    |31:0 |R/W|KEY_1|bits[65:32] of aes key|
    +-----+---+-----+----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[65:32] of aes key
                #define UDMA_AES_AES_KEY_1_OFFSET                0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_1_get(uint32_t base);
        static inline void udma_aes_aes_key_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[65:32] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_1_KEY_1_BIT                                 0
        #define UDMA_AES_AES_KEY_1_KEY_1_WIDTH                               32
        #define UDMA_AES_AES_KEY_1_KEY_1_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_1_KEY_1_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_1_KEY_1_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_1_KEY_1(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_1           :32; // bits[65:32] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_1 : public vp::reg_32
        {
        public:
            inline void key_1_set(uint32_t value);
            inline uint32_t key_1_get();
        };

|

.. _udma_aes_AES_KEY_2:

AES_KEY_2
"""""""""

bits[95:64] of aes key

.. table:: 

    +-----+---+-----+----------------------+
    |Bit #|R/W|Name |     Description      |
    +=====+===+=====+======================+
    |31:0 |R/W|KEY_2|bits[65:32] of aes key|
    +-----+---+-----+----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[95:64] of aes key
                #define UDMA_AES_AES_KEY_2_OFFSET                0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_2_get(uint32_t base);
        static inline void udma_aes_aes_key_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[65:32] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_2_KEY_2_BIT                                 0
        #define UDMA_AES_AES_KEY_2_KEY_2_WIDTH                               32
        #define UDMA_AES_AES_KEY_2_KEY_2_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_2_KEY_2_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_2_KEY_2_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_2_KEY_2(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_2           :32; // bits[65:32] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_2 : public vp::reg_32
        {
        public:
            inline void key_2_set(uint32_t value);
            inline uint32_t key_2_get();
        };

|

.. _udma_aes_AES_KEY_3:

AES_KEY_3
"""""""""

bits[127:96] of aes key

.. table:: 

    +-----+---+-----+-----------------------+
    |Bit #|R/W|Name |      Description      |
    +=====+===+=====+=======================+
    |31:0 |R/W|KEY_3|bits[127:96] of aes key|
    +-----+---+-----+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[127:96] of aes key
                #define UDMA_AES_AES_KEY_3_OFFSET                0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_3_get(uint32_t base);
        static inline void udma_aes_aes_key_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[127:96] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_3_KEY_3_BIT                                 0
        #define UDMA_AES_AES_KEY_3_KEY_3_WIDTH                               32
        #define UDMA_AES_AES_KEY_3_KEY_3_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_3_KEY_3_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_3_KEY_3_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_3_KEY_3(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_3           :32; // bits[127:96] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_3 : public vp::reg_32
        {
        public:
            inline void key_3_set(uint32_t value);
            inline uint32_t key_3_get();
        };

|

.. _udma_aes_AES_KEY_4:

AES_KEY_4
"""""""""

bits[159:128] of aes key

.. table:: 

    +-----+---+-----+------------------------+
    |Bit #|R/W|Name |      Description       |
    +=====+===+=====+========================+
    |31:0 |R/W|KEY_4|bits[159:128] of aes key|
    +-----+---+-----+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[159:128] of aes key
                #define UDMA_AES_AES_KEY_4_OFFSET                0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_4_get(uint32_t base);
        static inline void udma_aes_aes_key_4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[159:128] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_4_KEY_4_BIT                                 0
        #define UDMA_AES_AES_KEY_4_KEY_4_WIDTH                               32
        #define UDMA_AES_AES_KEY_4_KEY_4_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_4_KEY_4_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_4_KEY_4_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_4_KEY_4(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_4           :32; // bits[159:128] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_4 : public vp::reg_32
        {
        public:
            inline void key_4_set(uint32_t value);
            inline uint32_t key_4_get();
        };

|

.. _udma_aes_AES_KEY_5:

AES_KEY_5
"""""""""

bits[191:160] of aes key

.. table:: 

    +-----+---+-----+------------------------+
    |Bit #|R/W|Name |      Description       |
    +=====+===+=====+========================+
    |31:0 |R/W|KEY_5|bits[191:160] of aes key|
    +-----+---+-----+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[191:160] of aes key
                #define UDMA_AES_AES_KEY_5_OFFSET                0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_5_get(uint32_t base);
        static inline void udma_aes_aes_key_5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[191:160] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_5_KEY_5_BIT                                 0
        #define UDMA_AES_AES_KEY_5_KEY_5_WIDTH                               32
        #define UDMA_AES_AES_KEY_5_KEY_5_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_5_KEY_5_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_5_KEY_5_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_5_KEY_5(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_5           :32; // bits[191:160] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_5 : public vp::reg_32
        {
        public:
            inline void key_5_set(uint32_t value);
            inline uint32_t key_5_get();
        };

|

.. _udma_aes_AES_KEY_6:

AES_KEY_6
"""""""""

bits[223:192] of aes key

.. table:: 

    +-----+---+-----+------------------------+
    |Bit #|R/W|Name |      Description       |
    +=====+===+=====+========================+
    |31:0 |R/W|KEY_6|bits[223:192] of aes key|
    +-----+---+-----+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[223:192] of aes key
                #define UDMA_AES_AES_KEY_6_OFFSET                0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_6_get(uint32_t base);
        static inline void udma_aes_aes_key_6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[223:192] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_6_KEY_6_BIT                                 0
        #define UDMA_AES_AES_KEY_6_KEY_6_WIDTH                               32
        #define UDMA_AES_AES_KEY_6_KEY_6_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_6_KEY_6_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_6_KEY_6_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_6_KEY_6(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_6           :32; // bits[223:192] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_6 : public vp::reg_32
        {
        public:
            inline void key_6_set(uint32_t value);
            inline uint32_t key_6_get();
        };

|

.. _udma_aes_AES_KEY_7:

AES_KEY_7
"""""""""

bits[255:224] of aes key

.. table:: 

    +-----+---+-----+------------------------+
    |Bit #|R/W|Name |      Description       |
    +=====+===+=====+========================+
    |31:0 |R/W|KEY_7|bits[255:224] of aes key|
    +-----+---+-----+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[255:224] of aes key
                #define UDMA_AES_AES_KEY_7_OFFSET                0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_key_7_get(uint32_t base);
        static inline void udma_aes_aes_key_7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[255:224] of aes key (access: R/W)
        #define UDMA_AES_AES_KEY_7_KEY_7_BIT                                 0
        #define UDMA_AES_AES_KEY_7_KEY_7_WIDTH                               32
        #define UDMA_AES_AES_KEY_7_KEY_7_MASK                                0xffffffff
        #define UDMA_AES_AES_KEY_7_KEY_7_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_KEY_7_KEY_7_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_KEY_7_KEY_7(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_7           :32; // bits[255:224] of aes key
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_key_7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_key_7 : public vp::reg_32
        {
        public:
            inline void key_7_set(uint32_t value);
            inline uint32_t key_7_get();
        };

|

.. _udma_aes_AES_IV_0:

AES_IV_0
""""""""

bits[31:0] of IV used on first round of CBC

.. table:: 

    +-----+---+----+-------------------------------------------+
    |Bit #|R/W|Name|                Description                |
    +=====+===+====+===========================================+
    |31:0 |R/W|IV_0|bits[31:0] of IV used on first round of CBC|
    +-----+---+----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[31:0] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_0_OFFSET                 0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_iv_0_get(uint32_t base);
        static inline void udma_aes_aes_iv_0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[31:0] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_0_IV_0_BIT                                   0
        #define UDMA_AES_AES_IV_0_IV_0_WIDTH                                 32
        #define UDMA_AES_AES_IV_0_IV_0_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_0_IV_0_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_IV_0_IV_0_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_0_IV_0_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_0_IV_0_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_0_IV_0(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int iv_0            :32; // bits[31:0] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_iv_0 : public vp::reg_32
        {
        public:
            inline void iv_0_set(uint32_t value);
            inline uint32_t iv_0_get();
        };

|

.. _udma_aes_AES_IV_1:

AES_IV_1
""""""""

bits[65:32] of IV used on first round of CBC

.. table:: 

    +-----+---+----+--------------------------------------------+
    |Bit #|R/W|Name|                Description                 |
    +=====+===+====+============================================+
    |31:0 |R/W|IV_1|bits[65:32] of IV used on first round of CBC|
    +-----+---+----+--------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[65:32] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_1_OFFSET                 0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_iv_1_get(uint32_t base);
        static inline void udma_aes_aes_iv_1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[65:32] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_1_IV_1_BIT                                   0
        #define UDMA_AES_AES_IV_1_IV_1_WIDTH                                 32
        #define UDMA_AES_AES_IV_1_IV_1_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_1_IV_1_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_IV_1_IV_1_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_1_IV_1_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_1_IV_1_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_1_IV_1(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int iv_1            :32; // bits[65:32] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_iv_1 : public vp::reg_32
        {
        public:
            inline void iv_1_set(uint32_t value);
            inline uint32_t iv_1_get();
        };

|

.. _udma_aes_AES_IV_2:

AES_IV_2
""""""""

bits[95:64] of IV used on first round of CBC

.. table:: 

    +-----+---+----+--------------------------------------------+
    |Bit #|R/W|Name|                Description                 |
    +=====+===+====+============================================+
    |31:0 |R/W|IV_2|bits[95:64] of IV used on first round of CBC|
    +-----+---+----+--------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[95:64] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_2_OFFSET                 0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_iv_2_get(uint32_t base);
        static inline void udma_aes_aes_iv_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[95:64] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_2_IV_2_BIT                                   0
        #define UDMA_AES_AES_IV_2_IV_2_WIDTH                                 32
        #define UDMA_AES_AES_IV_2_IV_2_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_2_IV_2_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_IV_2_IV_2_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_2_IV_2_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_2_IV_2_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_2_IV_2(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int iv_2            :32; // bits[95:64] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_iv_2 : public vp::reg_32
        {
        public:
            inline void iv_2_set(uint32_t value);
            inline uint32_t iv_2_get();
        };

|

.. _udma_aes_AES_IV_3:

AES_IV_3
""""""""

bits[127:96] of IV used on first round of CBC

.. table:: 

    +-----+---+----+---------------------------------------------+
    |Bit #|R/W|Name|                 Description                 |
    +=====+===+====+=============================================+
    |31:0 |R/W|IV_3|bits[127:96] of IV used on first round of CBC|
    +-----+---+----+---------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // bits[127:96] of IV used on first round of CBC
                #define UDMA_AES_AES_IV_3_OFFSET                 0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_iv_3_get(uint32_t base);
        static inline void udma_aes_aes_iv_3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // bits[127:96] of IV used on first round of CBC (access: R/W)
        #define UDMA_AES_AES_IV_3_IV_3_BIT                                   0
        #define UDMA_AES_AES_IV_3_IV_3_WIDTH                                 32
        #define UDMA_AES_AES_IV_3_IV_3_MASK                                  0xffffffff
        #define UDMA_AES_AES_IV_3_IV_3_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_IV_3_IV_3_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_AES_AES_IV_3_IV_3_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_AES_AES_IV_3_IV_3_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_AES_AES_IV_3_IV_3(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int iv_3            :32; // bits[127:96] of IV used on first round of CBC
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_iv_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_iv_3 : public vp::reg_32
        {
        public:
            inline void iv_3_set(uint32_t value);
            inline uint32_t iv_3_get();
        };

|

.. _udma_aes_AES_ID_CFG:

AES_ID_CFG
""""""""""

Stream ID of uDMA channels

.. table:: 

    +-----+---+-------+--------------------------------------------------------------------+
    |Bit #|R/W| Name  |                            Description                             |
    +=====+===+=======+====================================================================+
    |6:0  |R/W|RX_DEST|Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled)|
    +-----+---+-------+--------------------------------------------------------------------+
    |14:8 |R/W|TX_DEST|Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled)|
    +-----+---+-------+--------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID of uDMA channels
                #define UDMA_AES_AES_ID_CFG_OFFSET               0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_id_cfg_get(uint32_t base);
        static inline void udma_aes_aes_id_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_AES_AES_ID_CFG_RX_DEST_BIT                              0
        #define UDMA_AES_AES_ID_CFG_RX_DEST_WIDTH                            7
        #define UDMA_AES_AES_ID_CFG_RX_DEST_MASK                             0x7f
        #define UDMA_AES_AES_ID_CFG_RX_DEST_RESET                            0x7f
        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_AES_AES_ID_CFG_TX_DEST_BIT                              8
        #define UDMA_AES_AES_ID_CFG_TX_DEST_WIDTH                            7
        #define UDMA_AES_AES_ID_CFG_TX_DEST_MASK                             0x7f00
        #define UDMA_AES_AES_ID_CFG_TX_DEST_RESET                            0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_ID_CFG_RX_DEST_GET(value)             (GAP_BEXTRACTU((value),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST_GETS(value)            (GAP_BEXTRACT((value),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,0))
        #define UDMA_AES_AES_ID_CFG_RX_DEST(val)                   ((val) << 0)
        
        #define UDMA_AES_AES_ID_CFG_TX_DEST_GET(value)             (GAP_BEXTRACTU((value),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST_GETS(value)            (GAP_BEXTRACT((value),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,8))
        #define UDMA_AES_AES_ID_CFG_TX_DEST(val)                   ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled)
            unsigned int padding0:1 ;
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_id_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_id_cfg : public vp::reg_32
        {
        public:
            inline void rx_dest_set(uint32_t value);
            inline uint32_t rx_dest_get();
            inline void tx_dest_set(uint32_t value);
            inline uint32_t tx_dest_get();
        };

|

.. _udma_aes_AES_SETUP:

AES_SETUP
"""""""""

Global configuration register

.. table:: 

    +-----+---+-----------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                        Description                                         |
    +=====+===+===========+============================================================================================+
    |    0|R/W|KEY_INIT   |When writing a 1 enables the key init round. Read returns 1 when the key init has terminated|
    +-----+---+-----------+--------------------------------------------------------------------------------------------+
    |    1|R/W|KEY_LEN    |AES Key Length: - 0: 128bits - 1: 256bits                                                   |
    +-----+---+-----------+--------------------------------------------------------------------------------------------+
    |    2|R/W|ENCDECN    |ENC/DEC behaviour: - 0: Decode - 1: Encode                                                  |
    +-----+---+-----------+--------------------------------------------------------------------------------------------+
    |    3|R/W|CBCECBN    |ENC/DEC type: - 0: ECB - 1: CBC                                                             |
    +-----+---+-----------+--------------------------------------------------------------------------------------------+
    |    4|W  |BLK_RESTART|When set and CBC is enabled triggers a block restart using IV as first xor                  |
    +-----+---+-----------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Global configuration register
                #define UDMA_AES_AES_SETUP_OFFSET                0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_aes_setup_get(uint32_t base);
        static inline void udma_aes_aes_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // When writing a 1 enables the key init round. Read returns 1 when the key init has terminated (access: R/W)
        #define UDMA_AES_AES_SETUP_KEY_INIT_BIT                              0
        #define UDMA_AES_AES_SETUP_KEY_INIT_WIDTH                            1
        #define UDMA_AES_AES_SETUP_KEY_INIT_MASK                             0x1
        #define UDMA_AES_AES_SETUP_KEY_INIT_RESET                            0x0
        
        // AES Key Length: - 0: 128bits - 1: 256bits (access: R/W)
        #define UDMA_AES_AES_SETUP_KEY_LEN_BIT                               1
        #define UDMA_AES_AES_SETUP_KEY_LEN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_KEY_LEN_MASK                              0x2
        #define UDMA_AES_AES_SETUP_KEY_LEN_RESET                             0x0
        
        // ENC/DEC behaviour: - 0: Decode - 1: Encode (access: R/W)
        #define UDMA_AES_AES_SETUP_ENCDECN_BIT                               2
        #define UDMA_AES_AES_SETUP_ENCDECN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_ENCDECN_MASK                              0x4
        #define UDMA_AES_AES_SETUP_ENCDECN_RESET                             0x0
        
        // ENC/DEC type: - 0: ECB - 1: CBC (access: R/W)
        #define UDMA_AES_AES_SETUP_CBCECBN_BIT                               3
        #define UDMA_AES_AES_SETUP_CBCECBN_WIDTH                             1
        #define UDMA_AES_AES_SETUP_CBCECBN_MASK                              0x8
        #define UDMA_AES_AES_SETUP_CBCECBN_RESET                             0x0
        
        // When set and CBC is enabled triggers a block restart using IV as first xor (access: W)
        #define UDMA_AES_AES_SETUP_BLK_RESTART_BIT                           4
        #define UDMA_AES_AES_SETUP_BLK_RESTART_WIDTH                         1
        #define UDMA_AES_AES_SETUP_BLK_RESTART_MASK                          0x10
        #define UDMA_AES_AES_SETUP_BLK_RESTART_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_AES_SETUP_KEY_INIT_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_AES_AES_SETUP_KEY_INIT(val)                   ((val) << 0)
        
        #define UDMA_AES_AES_SETUP_KEY_LEN_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_AES_AES_SETUP_KEY_LEN(val)                    ((val) << 1)
        
        #define UDMA_AES_AES_SETUP_ENCDECN_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define UDMA_AES_AES_SETUP_ENCDECN(val)                    ((val) << 2)
        
        #define UDMA_AES_AES_SETUP_CBCECBN_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define UDMA_AES_AES_SETUP_CBCECBN(val)                    ((val) << 3)
        
        #define UDMA_AES_AES_SETUP_BLK_RESTART_GET(value)          (GAP_BEXTRACTU((value),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART_GETS(value)         (GAP_BEXTRACT((value),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART_SET(value,field)    (GAP_BINSERT((value),(field),1,4))
        #define UDMA_AES_AES_SETUP_BLK_RESTART(val)                ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_init        :1 ; // When writing a 1 enables the key init round. Read returns 1 when the key init has terminated
            unsigned int key_len         :1 ; // AES Key Length: - 0: 128bits - 1: 256bits
            unsigned int encdecn         :1 ; // ENC/DEC behaviour: - 0: Decode - 1: Encode
            unsigned int cbcecbn         :1 ; // ENC/DEC type: - 0: ECB - 1: CBC
            unsigned int blk_restart     :1 ; // When set and CBC is enabled triggers a block restart using IV as first xor
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_aes_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_aes_setup : public vp::reg_32
        {
        public:
            inline void key_init_set(uint32_t value);
            inline uint32_t key_init_get();
            inline void key_len_set(uint32_t value);
            inline uint32_t key_len_get();
            inline void encdecn_set(uint32_t value);
            inline uint32_t encdecn_get();
            inline void cbcecbn_set(uint32_t value);
            inline uint32_t cbcecbn_get();
            inline void blk_restart_set(uint32_t value);
            inline uint32_t blk_restart_get();
        };

|
