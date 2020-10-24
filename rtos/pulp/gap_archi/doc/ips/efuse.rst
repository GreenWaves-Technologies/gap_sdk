Input file: fe/ips/apb/apb_efuse_if/doc/EFUSE_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------+------+-----+-------------------------------+
    |        Name         |Offset|Width|          Description          |
    +=====================+======+=====+===============================+
    |:ref:`CMD<efuse_CMD>`|     0|   32|EFUSE command register         |
    +---------------------+------+-----+-------------------------------+
    |:ref:`CFG<efuse_CFG>`|     4|   32|EFUSE timing configure register|
    +---------------------+------+-----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // EFUSE command register
                #define EFUSE_CMD_OFFSET                         0x0
        
                // EFUSE timing configure register
                #define EFUSE_CFG_OFFSET                         0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t efuse_cmd_get(uint32_t base);
        static inline void efuse_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t efuse_cfg_get(uint32_t base);
        static inline void efuse_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Set timer counter short period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET1_BIT                                    0
        #define EFUSE_CFG_CNT_TARGET1_WIDTH                                  32
        #define EFUSE_CFG_CNT_TARGET1_MASK                                   0xffffffff
        #define EFUSE_CFG_CNT_TARGET1_RESET                                  0x0
        
        // Set timer counter medium period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET2_BIT                                    6
        #define EFUSE_CFG_CNT_TARGET2_WIDTH                                  2
        #define EFUSE_CFG_CNT_TARGET2_MASK                                   0xc0
        #define EFUSE_CFG_CNT_TARGET2_RESET                                  0x0
        
        // Set timer counter long period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET3_BIT                                    16
        #define EFUSE_CFG_CNT_TARGET3_WIDTH                                  2
        #define EFUSE_CFG_CNT_TARGET3_MASK                                   0x30000
        #define EFUSE_CFG_CNT_TARGET3_RESET                                  0x0
        
        // Margin (access: R/W)
        #define EFUSE_CFG_MARGIN_BIT                                         30
        #define EFUSE_CFG_MARGIN_WIDTH                                       32
        #define EFUSE_CFG_MARGIN_MASK                                        0x3fffffffc0000000
        #define EFUSE_CFG_MARGIN_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define EFUSE_CFG_CNT_TARGET1_GET(value)                   (GAP_BEXTRACTU((value),32,0))
        #define EFUSE_CFG_CNT_TARGET1_GETS(value)                  (GAP_BEXTRACT((value),32,0))
        #define EFUSE_CFG_CNT_TARGET1_SET(value,field)             (GAP_BINSERT((value),(field),32,0))
        #define EFUSE_CFG_CNT_TARGET1(val)                         ((val) << 0)
        
        #define EFUSE_CFG_CNT_TARGET2_GET(value)                   (GAP_BEXTRACTU((value),2,6))
        #define EFUSE_CFG_CNT_TARGET2_GETS(value)                  (GAP_BEXTRACT((value),2,6))
        #define EFUSE_CFG_CNT_TARGET2_SET(value,field)             (GAP_BINSERT((value),(field),2,6))
        #define EFUSE_CFG_CNT_TARGET2(val)                         ((val) << 6)
        
        #define EFUSE_CFG_CNT_TARGET3_GET(value)                   (GAP_BEXTRACTU((value),2,16))
        #define EFUSE_CFG_CNT_TARGET3_GETS(value)                  (GAP_BEXTRACT((value),2,16))
        #define EFUSE_CFG_CNT_TARGET3_SET(value,field)             (GAP_BINSERT((value),(field),2,16))
        #define EFUSE_CFG_CNT_TARGET3(val)                         ((val) << 16)
        
        #define EFUSE_CFG_MARGIN_GET(value)                        (GAP_BEXTRACTU((value),32,30))
        #define EFUSE_CFG_MARGIN_GETS(value)                       (GAP_BEXTRACT((value),32,30))
        #define EFUSE_CFG_MARGIN_SET(value,field)                  (GAP_BINSERT((value),(field),32,30))
        #define EFUSE_CFG_MARGIN(val)                              ((val) << 30)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** EFUSE_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cmd;  // EFUSE command register
            volatile uint32_t cfg;  // EFUSE timing configure register
        } __attribute__((packed)) efuse_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) efuse_cmd_t;
        
        typedef union {
          struct {
            unsigned int cnt_target1     :32; // Set timer counter short period
            unsigned int cnt_target2     :2 ; // Set timer counter medium period
            unsigned int padding0:8 ;
            unsigned int cnt_target3     :2 ; // Set timer counter long period
            unsigned int padding1:12;
            unsigned int margin          :32; // Margin
          };
          unsigned int raw;
        } __attribute__((packed)) efuse_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_efuse : public vp::regmap
        {
        public:
            vp_efuse_cmd cmd;
            vp_efuse_cfg cfg;
        };

|

.. _efuse_CMD:

CMD
"""

EFUSE command register

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

        
                // EFUSE command register
                #define EFUSE_CMD_OFFSET                         0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t efuse_cmd_get(uint32_t base);
        static inline void efuse_cmd_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) efuse_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_efuse_cmd : public vp::reg_32
        {
        public:
        };

|

.. _efuse_CFG:

CFG
"""

EFUSE timing configure register

.. table:: 

    +-----+---+-----------+-------------------------------+
    |Bit #|R/W|   Name    |          Description          |
    +=====+===+===========+===============================+
    |31:0 |R/W|CNT_TARGET1|Set timer counter short period |
    +-----+---+-----------+-------------------------------+
    |7:6  |R/W|CNT_TARGET2|Set timer counter medium period|
    +-----+---+-----------+-------------------------------+
    |17:16|R/W|CNT_TARGET3|Set timer counter long period  |
    +-----+---+-----------+-------------------------------+
    |61:30|R/W|MARGIN     |Margin                         |
    +-----+---+-----------+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // EFUSE timing configure register
                #define EFUSE_CFG_OFFSET                         0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t efuse_cfg_get(uint32_t base);
        static inline void efuse_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Set timer counter short period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET1_BIT                                    0
        #define EFUSE_CFG_CNT_TARGET1_WIDTH                                  32
        #define EFUSE_CFG_CNT_TARGET1_MASK                                   0xffffffff
        #define EFUSE_CFG_CNT_TARGET1_RESET                                  0x0
        
        // Set timer counter medium period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET2_BIT                                    6
        #define EFUSE_CFG_CNT_TARGET2_WIDTH                                  2
        #define EFUSE_CFG_CNT_TARGET2_MASK                                   0xc0
        #define EFUSE_CFG_CNT_TARGET2_RESET                                  0x0
        
        // Set timer counter long period (access: R/W)
        #define EFUSE_CFG_CNT_TARGET3_BIT                                    16
        #define EFUSE_CFG_CNT_TARGET3_WIDTH                                  2
        #define EFUSE_CFG_CNT_TARGET3_MASK                                   0x30000
        #define EFUSE_CFG_CNT_TARGET3_RESET                                  0x0
        
        // Margin (access: R/W)
        #define EFUSE_CFG_MARGIN_BIT                                         30
        #define EFUSE_CFG_MARGIN_WIDTH                                       32
        #define EFUSE_CFG_MARGIN_MASK                                        0x3fffffffc0000000
        #define EFUSE_CFG_MARGIN_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define EFUSE_CFG_CNT_TARGET1_GET(value)                   (GAP_BEXTRACTU((value),32,0))
        #define EFUSE_CFG_CNT_TARGET1_GETS(value)                  (GAP_BEXTRACT((value),32,0))
        #define EFUSE_CFG_CNT_TARGET1_SET(value,field)             (GAP_BINSERT((value),(field),32,0))
        #define EFUSE_CFG_CNT_TARGET1(val)                         ((val) << 0)
        
        #define EFUSE_CFG_CNT_TARGET2_GET(value)                   (GAP_BEXTRACTU((value),2,6))
        #define EFUSE_CFG_CNT_TARGET2_GETS(value)                  (GAP_BEXTRACT((value),2,6))
        #define EFUSE_CFG_CNT_TARGET2_SET(value,field)             (GAP_BINSERT((value),(field),2,6))
        #define EFUSE_CFG_CNT_TARGET2(val)                         ((val) << 6)
        
        #define EFUSE_CFG_CNT_TARGET3_GET(value)                   (GAP_BEXTRACTU((value),2,16))
        #define EFUSE_CFG_CNT_TARGET3_GETS(value)                  (GAP_BEXTRACT((value),2,16))
        #define EFUSE_CFG_CNT_TARGET3_SET(value,field)             (GAP_BINSERT((value),(field),2,16))
        #define EFUSE_CFG_CNT_TARGET3(val)                         ((val) << 16)
        
        #define EFUSE_CFG_MARGIN_GET(value)                        (GAP_BEXTRACTU((value),32,30))
        #define EFUSE_CFG_MARGIN_GETS(value)                       (GAP_BEXTRACT((value),32,30))
        #define EFUSE_CFG_MARGIN_SET(value,field)                  (GAP_BINSERT((value),(field),32,30))
        #define EFUSE_CFG_MARGIN(val)                              ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt_target1     :32; // Set timer counter short period
            unsigned int cnt_target2     :2 ; // Set timer counter medium period
            unsigned int padding0:8 ;
            unsigned int cnt_target3     :2 ; // Set timer counter long period
            unsigned int padding1:12;
            unsigned int margin          :32; // Margin
          };
          unsigned int raw;
        } __attribute__((packed)) efuse_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_efuse_cfg : public vp::reg_32
        {
        public:
            inline void cnt_target1_set(uint32_t value);
            inline uint32_t cnt_target1_get();
            inline void cnt_target2_set(uint32_t value);
            inline uint32_t cnt_target2_get();
            inline void cnt_target3_set(uint32_t value);
            inline uint32_t cnt_target3_get();
            inline void margin_set(uint32_t value);
            inline uint32_t margin_get();
        };

|
