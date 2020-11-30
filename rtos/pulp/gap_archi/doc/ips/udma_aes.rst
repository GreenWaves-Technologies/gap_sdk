Input file: fe/ips/udma/udma_aes/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------+------+-----+---------------------------+
    |             Name             |Offset|Width|        Description        |
    +==============================+======+=====+===========================+
    |:ref:`KEY0_0<udma_aes_KEY0_0>`|     0|   32|core 0 key 0               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_1<udma_aes_KEY0_1>`|     4|   32|core 0 key 1               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_2<udma_aes_KEY0_2>`|     8|   32|core 0 key 2               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_3<udma_aes_KEY0_3>`|    12|   32|core 0 key 3               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_4<udma_aes_KEY0_4>`|    16|   32|core 0 key 4               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_5<udma_aes_KEY0_5>`|    20|   32|core 0 key 5               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_6<udma_aes_KEY0_6>`|    24|   32|core 0 key 6               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`KEY0_7<udma_aes_KEY0_7>`|    28|   32|core 0 key 7               |
    +------------------------------+------+-----+---------------------------+
    |:ref:`IV0_0<udma_aes_IV0_0>`  |    32|   32|core 0 IV 0                |
    +------------------------------+------+-----+---------------------------+
    |:ref:`IV0_1<udma_aes_IV0_1>`  |    36|   32|core 0 IV 1                |
    +------------------------------+------+-----+---------------------------+
    |:ref:`IV0_2<udma_aes_IV0_2>`  |    40|   32|core 0 IV 2                |
    +------------------------------+------+-----+---------------------------+
    |:ref:`IV0_3<udma_aes_IV0_3>`  |    44|   32|core 0 IV 3                |
    +------------------------------+------+-----+---------------------------+
    |:ref:`DEST<udma_aes_DEST>`    |    48|   32|RX TX destination channel  |
    +------------------------------+------+-----+---------------------------+
    |:ref:`SETUP<udma_aes_SETUP>`  |    52|   32|core setup                 |
    +------------------------------+------+-----+---------------------------+
    |:ref:`CFG<udma_aes_CFG>`      |    56|   32|AES data flow configuration|
    +------------------------------+------+-----+---------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // core 0 key 0
                #define UDMA_AES_KEY0_0_OFFSET                   0x0
        
                // core 0 key 1
                #define UDMA_AES_KEY0_1_OFFSET                   0x4
        
                // core 0 key 2
                #define UDMA_AES_KEY0_2_OFFSET                   0x8
        
                // core 0 key 3
                #define UDMA_AES_KEY0_3_OFFSET                   0xc
        
                // core 0 key 4
                #define UDMA_AES_KEY0_4_OFFSET                   0x10
        
                // core 0 key 5
                #define UDMA_AES_KEY0_5_OFFSET                   0x14
        
                // core 0 key 6
                #define UDMA_AES_KEY0_6_OFFSET                   0x18
        
                // core 0 key 7
                #define UDMA_AES_KEY0_7_OFFSET                   0x1c
        
                // core 0 IV 0
                #define UDMA_AES_IV0_0_OFFSET                    0x20
        
                // core 0 IV 1
                #define UDMA_AES_IV0_1_OFFSET                    0x24
        
                // core 0 IV 2
                #define UDMA_AES_IV0_2_OFFSET                    0x28
        
                // core 0 IV 3
                #define UDMA_AES_IV0_3_OFFSET                    0x2c
        
                // RX TX destination channel
                #define UDMA_AES_DEST_OFFSET                     0x30
        
                // core setup
                #define UDMA_AES_SETUP_OFFSET                    0x34
        
                // AES data flow configuration
                #define UDMA_AES_CFG_OFFSET                      0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_0_get(uint32_t base);
        static inline void udma_aes_key0_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_1_get(uint32_t base);
        static inline void udma_aes_key0_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_2_get(uint32_t base);
        static inline void udma_aes_key0_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_3_get(uint32_t base);
        static inline void udma_aes_key0_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_4_get(uint32_t base);
        static inline void udma_aes_key0_4_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_5_get(uint32_t base);
        static inline void udma_aes_key0_5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_6_get(uint32_t base);
        static inline void udma_aes_key0_6_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_key0_7_get(uint32_t base);
        static inline void udma_aes_key0_7_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_iv0_0_get(uint32_t base);
        static inline void udma_aes_iv0_0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_iv0_1_get(uint32_t base);
        static inline void udma_aes_iv0_1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_iv0_2_get(uint32_t base);
        static inline void udma_aes_iv0_2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_iv0_3_get(uint32_t base);
        static inline void udma_aes_iv0_3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_dest_get(uint32_t base);
        static inline void udma_aes_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_setup_get(uint32_t base);
        static inline void udma_aes_setup_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_aes_cfg_get(uint32_t base);
        static inline void udma_aes_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_AES_DEST_RX_DEST_BIT                                    0
        #define UDMA_AES_DEST_RX_DEST_WIDTH                                  8
        #define UDMA_AES_DEST_RX_DEST_MASK                                   0xff
        #define UDMA_AES_DEST_RX_DEST_RESET                                  0xff
        
        // Stream ID for the TX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_AES_DEST_TX_DEST_BIT                                    8
        #define UDMA_AES_DEST_TX_DEST_WIDTH                                  8
        #define UDMA_AES_DEST_TX_DEST_MASK                                   0xff00
        #define UDMA_AES_DEST_TX_DEST_RESET                                  0xff
        
        // Indicate the key configuration is finished (access: R)
        #define UDMA_AES_SETUP_KEY_INIT_BIT                                  0
        #define UDMA_AES_SETUP_KEY_INIT_WIDTH                                1
        #define UDMA_AES_SETUP_KEY_INIT_MASK                                 0x1
        #define UDMA_AES_SETUP_KEY_INIT_RESET                                0x0
        
        // KEY type, 0 for 128B, 1 for 256B (access: R/W)
        #define UDMA_AES_SETUP_KEY_TYPE_BIT                                  1
        #define UDMA_AES_SETUP_KEY_TYPE_WIDTH                                1
        #define UDMA_AES_SETUP_KEY_TYPE_MASK                                 0x2
        #define UDMA_AES_SETUP_KEY_TYPE_RESET                                0x0
        
        // Operation type, 0 for DEC, 1 for ENC (access: R/W)
        #define UDMA_AES_SETUP_ENC_DEC_BIT                                   2
        #define UDMA_AES_SETUP_ENC_DEC_WIDTH                                 1
        #define UDMA_AES_SETUP_ENC_DEC_MASK                                  0x4
        #define UDMA_AES_SETUP_ENC_DEC_RESET                                 0x0
        
        // Enc_type, 0 for ECB, 1 for CBC (access: R/W)
        #define UDMA_AES_SETUP_ECB_CBC_BIT                                   3
        #define UDMA_AES_SETUP_ECB_CBC_WIDTH                                 1
        #define UDMA_AES_SETUP_ECB_CBC_MASK                                  0x8
        #define UDMA_AES_SETUP_ECB_CBC_RESET                                 0x0
        
        // Block reset (access: W)
        #define UDMA_AES_SETUP_BLOCK_RST_BIT                                 4
        #define UDMA_AES_SETUP_BLOCK_RST_WIDTH                               1
        #define UDMA_AES_SETUP_BLOCK_RST_MASK                                0x10
        #define UDMA_AES_SETUP_BLOCK_RST_RESET                               0x0
        
        //  (access: -)
        #define UDMA_AES_SETUP_RESERVED_BIT                                  5
        #define UDMA_AES_SETUP_RESERVED_WIDTH                                3
        #define UDMA_AES_SETUP_RESERVED_MASK                                 0xe0
        #define UDMA_AES_SETUP_RESERVED_RESET                                0x0
        
        // Clean the fifo (access: W)
        #define UDMA_AES_SETUP_FIFO_CLR_BIT                                  8
        #define UDMA_AES_SETUP_FIFO_CLR_WIDTH                                1
        #define UDMA_AES_SETUP_FIFO_CLR_MASK                                 0x100
        #define UDMA_AES_SETUP_FIFO_CLR_RESET                                0x0
        
        // Transfer MODE 2'b00: memory 2 memory 2'b01: Stream 2 memory 2'b10: Memory 2 Stream 2'b11: Stream 2 Stream (access: R/W)
        #define UDMA_AES_CFG_MODE_BIT                                        0
        #define UDMA_AES_CFG_MODE_WIDTH                                      2
        #define UDMA_AES_CFG_MODE_MASK                                       0x3
        #define UDMA_AES_CFG_MODE_RESET                                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_DEST_RX_DEST_GET(value)                   (GAP_BEXTRACTU((value),8,0))
        #define UDMA_AES_DEST_RX_DEST_GETS(value)                  (GAP_BEXTRACT((value),8,0))
        #define UDMA_AES_DEST_RX_DEST_SET(value,field)             (GAP_BINSERT((value),(field),8,0))
        #define UDMA_AES_DEST_RX_DEST(val)                         ((val) << 0)
        
        #define UDMA_AES_DEST_TX_DEST_GET(value)                   (GAP_BEXTRACTU((value),8,8))
        #define UDMA_AES_DEST_TX_DEST_GETS(value)                  (GAP_BEXTRACT((value),8,8))
        #define UDMA_AES_DEST_TX_DEST_SET(value,field)             (GAP_BINSERT((value),(field),8,8))
        #define UDMA_AES_DEST_TX_DEST(val)                         ((val) << 8)
        
        #define UDMA_AES_SETUP_KEY_INIT_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define UDMA_AES_SETUP_KEY_INIT_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define UDMA_AES_SETUP_KEY_INIT_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define UDMA_AES_SETUP_KEY_INIT(val)                       ((val) << 0)
        
        #define UDMA_AES_SETUP_KEY_TYPE_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE(val)                       ((val) << 1)
        
        #define UDMA_AES_SETUP_ENC_DEC_GET(value)                  (GAP_BEXTRACTU((value),1,2))
        #define UDMA_AES_SETUP_ENC_DEC_GETS(value)                 (GAP_BEXTRACT((value),1,2))
        #define UDMA_AES_SETUP_ENC_DEC_SET(value,field)            (GAP_BINSERT((value),(field),1,2))
        #define UDMA_AES_SETUP_ENC_DEC(val)                        ((val) << 2)
        
        #define UDMA_AES_SETUP_ECB_CBC_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define UDMA_AES_SETUP_ECB_CBC_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define UDMA_AES_SETUP_ECB_CBC_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define UDMA_AES_SETUP_ECB_CBC(val)                        ((val) << 3)
        
        #define UDMA_AES_SETUP_BLOCK_RST_GET(value)                (GAP_BEXTRACTU((value),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST_GETS(value)               (GAP_BEXTRACT((value),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST_SET(value,field)          (GAP_BINSERT((value),(field),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST(val)                      ((val) << 4)
        
        #define UDMA_AES_SETUP_RESERVED_GET(value)                 (GAP_BEXTRACTU((value),3,5))
        #define UDMA_AES_SETUP_RESERVED_GETS(value)                (GAP_BEXTRACT((value),3,5))
        #define UDMA_AES_SETUP_RESERVED_SET(value,field)           (GAP_BINSERT((value),(field),3,5))
        #define UDMA_AES_SETUP_RESERVED(val)                       ((val) << 5)
        
        #define UDMA_AES_SETUP_FIFO_CLR_GET(value)                 (GAP_BEXTRACTU((value),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR_GETS(value)                (GAP_BEXTRACT((value),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR_SET(value,field)           (GAP_BINSERT((value),(field),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR(val)                       ((val) << 8)
        
        #define UDMA_AES_CFG_MODE_GET(value)                       (GAP_BEXTRACTU((value),2,0))
        #define UDMA_AES_CFG_MODE_GETS(value)                      (GAP_BEXTRACT((value),2,0))
        #define UDMA_AES_CFG_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),2,0))
        #define UDMA_AES_CFG_MODE(val)                             ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_AES_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t key0_0;  // core 0 key 0
            volatile uint32_t key0_1;  // core 0 key 1
            volatile uint32_t key0_2;  // core 0 key 2
            volatile uint32_t key0_3;  // core 0 key 3
            volatile uint32_t key0_4;  // core 0 key 4
            volatile uint32_t key0_5;  // core 0 key 5
            volatile uint32_t key0_6;  // core 0 key 6
            volatile uint32_t key0_7;  // core 0 key 7
            volatile uint32_t iv0_0;  // core 0 IV 0
            volatile uint32_t iv0_1;  // core 0 IV 1
            volatile uint32_t iv0_2;  // core 0 IV 2
            volatile uint32_t iv0_3;  // core 0 IV 3
            volatile uint32_t dest;  // RX TX destination channel
            volatile uint32_t setup;  // core setup
            volatile uint32_t cfg;  // AES data flow configuration
        } __attribute__((packed)) udma_aes_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_key0_7_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_iv0_0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_iv0_1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_iv0_2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_iv0_3_t;
        
        typedef union {
          struct {
            unsigned int rx_dest         :8 ; // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)
            unsigned int tx_dest         :8 ; // Stream ID for the TX uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_dest_t;
        
        typedef union {
          struct {
            unsigned int key_init        :1 ; // Indicate the key configuration is finished
            unsigned int key_type        :1 ; // KEY type, 0 for 128B, 1 for 256B
            unsigned int enc_dec         :1 ; // Operation type, 0 for DEC, 1 for ENC
            unsigned int ecb_cbc         :1 ; // Enc_type, 0 for ECB, 1 for CBC
            unsigned int block_rst       :1 ; // Block reset
            unsigned int reserved        :3 ; // 
            unsigned int fifo_clr        :1 ; // Clean the fifo
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_setup_t;
        
        typedef union {
          struct {
            unsigned int mode            :2 ; // Transfer MODE 2'b00: memory 2 memory 2'b01: Stream 2 memory 2'b10: Memory 2 Stream 2'b11: Stream 2 Stream
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_aes : public vp::regmap
        {
        public:
            vp_udma_aes_key0_0 key0_0;
            vp_udma_aes_key0_1 key0_1;
            vp_udma_aes_key0_2 key0_2;
            vp_udma_aes_key0_3 key0_3;
            vp_udma_aes_key0_4 key0_4;
            vp_udma_aes_key0_5 key0_5;
            vp_udma_aes_key0_6 key0_6;
            vp_udma_aes_key0_7 key0_7;
            vp_udma_aes_iv0_0 iv0_0;
            vp_udma_aes_iv0_1 iv0_1;
            vp_udma_aes_iv0_2 iv0_2;
            vp_udma_aes_iv0_3 iv0_3;
            vp_udma_aes_dest dest;
            vp_udma_aes_setup setup;
            vp_udma_aes_cfg cfg;
        };

|

.. _udma_aes_KEY0_0:

KEY0_0
""""""

core 0 key 0

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

        
                // core 0 key 0
                #define UDMA_AES_KEY0_0_OFFSET                   0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_0_get(uint32_t base);
        static inline void udma_aes_key0_0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_1:

KEY0_1
""""""

core 0 key 1

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

        
                // core 0 key 1
                #define UDMA_AES_KEY0_1_OFFSET                   0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_1_get(uint32_t base);
        static inline void udma_aes_key0_1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_2:

KEY0_2
""""""

core 0 key 2

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

        
                // core 0 key 2
                #define UDMA_AES_KEY0_2_OFFSET                   0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_2_get(uint32_t base);
        static inline void udma_aes_key0_2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_2 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_3:

KEY0_3
""""""

core 0 key 3

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

        
                // core 0 key 3
                #define UDMA_AES_KEY0_3_OFFSET                   0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_3_get(uint32_t base);
        static inline void udma_aes_key0_3_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_3 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_4:

KEY0_4
""""""

core 0 key 4

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

        
                // core 0 key 4
                #define UDMA_AES_KEY0_4_OFFSET                   0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_4_get(uint32_t base);
        static inline void udma_aes_key0_4_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_4 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_5:

KEY0_5
""""""

core 0 key 5

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

        
                // core 0 key 5
                #define UDMA_AES_KEY0_5_OFFSET                   0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_5_get(uint32_t base);
        static inline void udma_aes_key0_5_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_5 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_6:

KEY0_6
""""""

core 0 key 6

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

        
                // core 0 key 6
                #define UDMA_AES_KEY0_6_OFFSET                   0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_6_get(uint32_t base);
        static inline void udma_aes_key0_6_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_6 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_KEY0_7:

KEY0_7
""""""

core 0 key 7

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

        
                // core 0 key 7
                #define UDMA_AES_KEY0_7_OFFSET                   0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_key0_7_get(uint32_t base);
        static inline void udma_aes_key0_7_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_key0_7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_key0_7 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_IV0_0:

IV0_0
"""""

core 0 IV 0

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

        
                // core 0 IV 0
                #define UDMA_AES_IV0_0_OFFSET                    0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_iv0_0_get(uint32_t base);
        static inline void udma_aes_iv0_0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_iv0_0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_iv0_0 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_IV0_1:

IV0_1
"""""

core 0 IV 1

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

        
                // core 0 IV 1
                #define UDMA_AES_IV0_1_OFFSET                    0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_iv0_1_get(uint32_t base);
        static inline void udma_aes_iv0_1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_iv0_1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_iv0_1 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_IV0_2:

IV0_2
"""""

core 0 IV 2

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

        
                // core 0 IV 2
                #define UDMA_AES_IV0_2_OFFSET                    0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_iv0_2_get(uint32_t base);
        static inline void udma_aes_iv0_2_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_iv0_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_iv0_2 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_IV0_3:

IV0_3
"""""

core 0 IV 3

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

        
                // core 0 IV 3
                #define UDMA_AES_IV0_3_OFFSET                    0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_iv0_3_get(uint32_t base);
        static inline void udma_aes_iv0_3_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_aes_iv0_3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_iv0_3 : public vp::reg_32
        {
        public:
        };

|

.. _udma_aes_DEST:

DEST
""""

RX TX destination channel

.. table:: 

    +-----+---+-------+--------------------------------------------------------------------+
    |Bit #|R/W| Name  |                            Description                             |
    +=====+===+=======+====================================================================+
    |7:0  |R/W|RX_DEST|Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+-------+--------------------------------------------------------------------+
    |15:8 |R/W|TX_DEST|Stream ID for the TX uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+-------+--------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RX TX destination channel
                #define UDMA_AES_DEST_OFFSET                     0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_dest_get(uint32_t base);
        static inline void udma_aes_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_AES_DEST_RX_DEST_BIT                                    0
        #define UDMA_AES_DEST_RX_DEST_WIDTH                                  8
        #define UDMA_AES_DEST_RX_DEST_MASK                                   0xff
        #define UDMA_AES_DEST_RX_DEST_RESET                                  0xff
        
        // Stream ID for the TX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_AES_DEST_TX_DEST_BIT                                    8
        #define UDMA_AES_DEST_TX_DEST_WIDTH                                  8
        #define UDMA_AES_DEST_TX_DEST_MASK                                   0xff00
        #define UDMA_AES_DEST_TX_DEST_RESET                                  0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_DEST_RX_DEST_GET(value)                   (GAP_BEXTRACTU((value),8,0))
        #define UDMA_AES_DEST_RX_DEST_GETS(value)                  (GAP_BEXTRACT((value),8,0))
        #define UDMA_AES_DEST_RX_DEST_SET(value,field)             (GAP_BINSERT((value),(field),8,0))
        #define UDMA_AES_DEST_RX_DEST(val)                         ((val) << 0)
        
        #define UDMA_AES_DEST_TX_DEST_GET(value)                   (GAP_BEXTRACTU((value),8,8))
        #define UDMA_AES_DEST_TX_DEST_GETS(value)                  (GAP_BEXTRACT((value),8,8))
        #define UDMA_AES_DEST_TX_DEST_SET(value,field)             (GAP_BINSERT((value),(field),8,8))
        #define UDMA_AES_DEST_TX_DEST(val)                         ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :8 ; // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)
            unsigned int tx_dest         :8 ; // Stream ID for the TX uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_dest : public vp::reg_32
        {
        public:
            inline void rx_dest_set(uint32_t value);
            inline uint32_t rx_dest_get();
            inline void tx_dest_set(uint32_t value);
            inline uint32_t tx_dest_get();
        };

|

.. _udma_aes_SETUP:

SETUP
"""""

core setup

.. table:: 

    +-----+---+---------+------------------------------------------+
    |Bit #|R/W|  Name   |               Description                |
    +=====+===+=========+==========================================+
    |    0|R  |KEY_INIT |Indicate the key configuration is finished|
    +-----+---+---------+------------------------------------------+
    |    1|R/W|KEY_TYPE |KEY type, 0 for 128B, 1 for 256B          |
    +-----+---+---------+------------------------------------------+
    |    2|R/W|ENC_DEC  |Operation type, 0 for DEC, 1 for ENC      |
    +-----+---+---------+------------------------------------------+
    |    3|R/W|ECB_CBC  |Enc_type, 0 for ECB, 1 for CBC            |
    +-----+---+---------+------------------------------------------+
    |    4|W  |BLOCK_RST|Block reset                               |
    +-----+---+---------+------------------------------------------+
    |7:5  |-  |RESERVED |                                          |
    +-----+---+---------+------------------------------------------+
    |    8|W  |FIFO_CLR |Clean the fifo                            |
    +-----+---+---------+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // core setup
                #define UDMA_AES_SETUP_OFFSET                    0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_setup_get(uint32_t base);
        static inline void udma_aes_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Indicate the key configuration is finished (access: R)
        #define UDMA_AES_SETUP_KEY_INIT_BIT                                  0
        #define UDMA_AES_SETUP_KEY_INIT_WIDTH                                1
        #define UDMA_AES_SETUP_KEY_INIT_MASK                                 0x1
        #define UDMA_AES_SETUP_KEY_INIT_RESET                                0x0
        
        // KEY type, 0 for 128B, 1 for 256B (access: R/W)
        #define UDMA_AES_SETUP_KEY_TYPE_BIT                                  1
        #define UDMA_AES_SETUP_KEY_TYPE_WIDTH                                1
        #define UDMA_AES_SETUP_KEY_TYPE_MASK                                 0x2
        #define UDMA_AES_SETUP_KEY_TYPE_RESET                                0x0
        
        // Operation type, 0 for DEC, 1 for ENC (access: R/W)
        #define UDMA_AES_SETUP_ENC_DEC_BIT                                   2
        #define UDMA_AES_SETUP_ENC_DEC_WIDTH                                 1
        #define UDMA_AES_SETUP_ENC_DEC_MASK                                  0x4
        #define UDMA_AES_SETUP_ENC_DEC_RESET                                 0x0
        
        // Enc_type, 0 for ECB, 1 for CBC (access: R/W)
        #define UDMA_AES_SETUP_ECB_CBC_BIT                                   3
        #define UDMA_AES_SETUP_ECB_CBC_WIDTH                                 1
        #define UDMA_AES_SETUP_ECB_CBC_MASK                                  0x8
        #define UDMA_AES_SETUP_ECB_CBC_RESET                                 0x0
        
        // Block reset (access: W)
        #define UDMA_AES_SETUP_BLOCK_RST_BIT                                 4
        #define UDMA_AES_SETUP_BLOCK_RST_WIDTH                               1
        #define UDMA_AES_SETUP_BLOCK_RST_MASK                                0x10
        #define UDMA_AES_SETUP_BLOCK_RST_RESET                               0x0
        
        //  (access: -)
        #define UDMA_AES_SETUP_RESERVED_BIT                                  5
        #define UDMA_AES_SETUP_RESERVED_WIDTH                                3
        #define UDMA_AES_SETUP_RESERVED_MASK                                 0xe0
        #define UDMA_AES_SETUP_RESERVED_RESET                                0x0
        
        // Clean the fifo (access: W)
        #define UDMA_AES_SETUP_FIFO_CLR_BIT                                  8
        #define UDMA_AES_SETUP_FIFO_CLR_WIDTH                                1
        #define UDMA_AES_SETUP_FIFO_CLR_MASK                                 0x100
        #define UDMA_AES_SETUP_FIFO_CLR_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_SETUP_KEY_INIT_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define UDMA_AES_SETUP_KEY_INIT_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define UDMA_AES_SETUP_KEY_INIT_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define UDMA_AES_SETUP_KEY_INIT(val)                       ((val) << 0)
        
        #define UDMA_AES_SETUP_KEY_TYPE_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define UDMA_AES_SETUP_KEY_TYPE(val)                       ((val) << 1)
        
        #define UDMA_AES_SETUP_ENC_DEC_GET(value)                  (GAP_BEXTRACTU((value),1,2))
        #define UDMA_AES_SETUP_ENC_DEC_GETS(value)                 (GAP_BEXTRACT((value),1,2))
        #define UDMA_AES_SETUP_ENC_DEC_SET(value,field)            (GAP_BINSERT((value),(field),1,2))
        #define UDMA_AES_SETUP_ENC_DEC(val)                        ((val) << 2)
        
        #define UDMA_AES_SETUP_ECB_CBC_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define UDMA_AES_SETUP_ECB_CBC_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define UDMA_AES_SETUP_ECB_CBC_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define UDMA_AES_SETUP_ECB_CBC(val)                        ((val) << 3)
        
        #define UDMA_AES_SETUP_BLOCK_RST_GET(value)                (GAP_BEXTRACTU((value),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST_GETS(value)               (GAP_BEXTRACT((value),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST_SET(value,field)          (GAP_BINSERT((value),(field),1,4))
        #define UDMA_AES_SETUP_BLOCK_RST(val)                      ((val) << 4)
        
        #define UDMA_AES_SETUP_RESERVED_GET(value)                 (GAP_BEXTRACTU((value),3,5))
        #define UDMA_AES_SETUP_RESERVED_GETS(value)                (GAP_BEXTRACT((value),3,5))
        #define UDMA_AES_SETUP_RESERVED_SET(value,field)           (GAP_BINSERT((value),(field),3,5))
        #define UDMA_AES_SETUP_RESERVED(val)                       ((val) << 5)
        
        #define UDMA_AES_SETUP_FIFO_CLR_GET(value)                 (GAP_BEXTRACTU((value),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR_GETS(value)                (GAP_BEXTRACT((value),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR_SET(value,field)           (GAP_BINSERT((value),(field),1,8))
        #define UDMA_AES_SETUP_FIFO_CLR(val)                       ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_init        :1 ; // Indicate the key configuration is finished
            unsigned int key_type        :1 ; // KEY type, 0 for 128B, 1 for 256B
            unsigned int enc_dec         :1 ; // Operation type, 0 for DEC, 1 for ENC
            unsigned int ecb_cbc         :1 ; // Enc_type, 0 for ECB, 1 for CBC
            unsigned int block_rst       :1 ; // Block reset
            unsigned int reserved        :3 ; // 
            unsigned int fifo_clr        :1 ; // Clean the fifo
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_setup : public vp::reg_32
        {
        public:
            inline void key_init_set(uint32_t value);
            inline uint32_t key_init_get();
            inline void key_type_set(uint32_t value);
            inline uint32_t key_type_get();
            inline void enc_dec_set(uint32_t value);
            inline uint32_t enc_dec_get();
            inline void ecb_cbc_set(uint32_t value);
            inline uint32_t ecb_cbc_get();
            inline void block_rst_set(uint32_t value);
            inline uint32_t block_rst_get();
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void fifo_clr_set(uint32_t value);
            inline uint32_t fifo_clr_get();
        };

|

.. _udma_aes_CFG:

CFG
"""

AES data flow configuration

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                               Description                                               |
    +=====+===+====+=========================================================================================================+
    |1:0  |R/W|MODE|Transfer MODE 2'b00: memory 2 memory 2'b01: Stream 2 memory 2'b10: Memory 2 Stream 2'b11: Stream 2 Stream|
    +-----+---+----+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // AES data flow configuration
                #define UDMA_AES_CFG_OFFSET                      0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_aes_cfg_get(uint32_t base);
        static inline void udma_aes_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer MODE 2'b00: memory 2 memory 2'b01: Stream 2 memory 2'b10: Memory 2 Stream 2'b11: Stream 2 Stream (access: R/W)
        #define UDMA_AES_CFG_MODE_BIT                                        0
        #define UDMA_AES_CFG_MODE_WIDTH                                      2
        #define UDMA_AES_CFG_MODE_MASK                                       0x3
        #define UDMA_AES_CFG_MODE_RESET                                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_AES_CFG_MODE_GET(value)                       (GAP_BEXTRACTU((value),2,0))
        #define UDMA_AES_CFG_MODE_GETS(value)                      (GAP_BEXTRACT((value),2,0))
        #define UDMA_AES_CFG_MODE_SET(value,field)                 (GAP_BINSERT((value),(field),2,0))
        #define UDMA_AES_CFG_MODE(val)                             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mode            :2 ; // Transfer MODE 2'b00: memory 2 memory 2'b01: Stream 2 memory 2'b10: Memory 2 Stream 2'b11: Stream 2 Stream
          };
          unsigned int raw;
        } __attribute__((packed)) udma_aes_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_aes_cfg : public vp::reg_32
        {
        public:
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|
