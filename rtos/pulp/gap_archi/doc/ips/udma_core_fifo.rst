Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_FIFO_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |                                Name                                |Offset|Width|          Description          |
    +====================================================================+======+=====+===============================+
    |:ref:`ADDRGEN_CFG_SA_BUF<udma_core_fifo_ADDRGEN_CFG_SA_BUF>`        |     0|   32|Start Address of fifo buffer   |
    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |:ref:`ADDRGEN_CFG_SIZE<udma_core_fifo_ADDRGEN_CFG_SIZE>`            |     8|   32|Size of buffer                 |
    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |:ref:`ADDRGEN_CFG_EVT<udma_core_fifo_ADDRGEN_CFG_EVT>`              |    12|   32|Event config                   |
    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |:ref:`ADDRGEN_CFG_CURR_ADDR<udma_core_fifo_ADDRGEN_CFG_CURR_ADDR>`  |    20|   32|Current address                |
    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |:ref:`ADDRGEN_CFG_FIFO_LEVEL<udma_core_fifo_ADDRGEN_CFG_FIFO_LEVEL>`|    24|   32|Current number of bytes in fifo|
    +--------------------------------------------------------------------+------+-----+-------------------------------+
    |:ref:`ADDRGEN_CFG_CTRL<udma_core_fifo_ADDRGEN_CFG_CTRL>`            |    28|   32|Control register               |
    +--------------------------------------------------------------------+------+-----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Address of fifo buffer
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_OFFSET 0x0
        
                // Size of buffer
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_OFFSET   0x8
        
                // Event config
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_OFFSET    0xc
        
                // Current address
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_OFFSET 0x14
        
                // Current number of bytes in fifo
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_OFFSET 0x18
        
                // Control register
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_OFFSET   0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_sa_buf_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_sa_buf_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_fifo_addrgen_cfg_size_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_fifo_addrgen_cfg_evt_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_evt_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_fifo_addrgen_cfg_curr_addr_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_curr_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_fifo_addrgen_cfg_fifo_level_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_fifo_level_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_fifo_addrgen_cfg_ctrl_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start address of buffer (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_BIT                0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_WIDTH              32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_MASK               0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_RESET              0x0
        
        // Size of transfer (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_BIT                     0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_WIDTH                   32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_MASK                    0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_RESET                   0x0
        
        // Number of bytes to receive to trigger event (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_BIT             0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_WIDTH           16
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_MASK            0xffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_RESET           0x0
        
        // Enable event trigger when EVT_NUM_BYTES bytes are filled in fifo (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_BIT                   31
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_WIDTH                 1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_MASK                  0x80000000
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_RESET                 0x0
        
        // Current address (access: R)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_BIT           0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_WIDTH         32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_MASK          0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_RESET         0x0
        
        // Bytes filled in fifo buffer (access: R)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_BIT         0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_WIDTH       32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_MASK        0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_RESET       0x0
        
        // FIFO enable - 1b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_BIT                       1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_WIDTH                     1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_MASK                      0x2
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_RESET                     0x0
        
        // FIFO disable - 1b1 : Stops the FIFO (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_BIT                     4
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_WIDTH                   1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_MASK                    0x10
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR(val)     ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE(val)          ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES(val)  ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_GET(value)  (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_GETS(value) (GAP_BEXTRACT((value),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA(val)        ((val) << 31)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR(val) ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL(val) ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_GET(value)      (GAP_BEXTRACTU((value),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_GETS(value)     (GAP_BEXTRACT((value),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN(val)            ((val) << 1)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP(val)          ((val) << 4)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_CORE_FIFO_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t addrgen_cfg_sa_buf;  // Start Address of fifo buffer
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t addrgen_cfg_size;  // Size of buffer
            volatile uint32_t addrgen_cfg_evt;  // Event config
            volatile uint32_t reserved_1[1];  // Reserved/Not used.
            volatile uint32_t addrgen_cfg_curr_addr;  // Current address
            volatile uint32_t addrgen_cfg_fifo_level;  // Current number of bytes in fifo
            volatile uint32_t addrgen_cfg_ctrl;  // Control register
        } __attribute__((packed)) udma_core_fifo_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sa_addr         :32; // Start address of buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_sa_buf_t;
        
        typedef union {
          struct {
            unsigned int size            :32; // Size of transfer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_size_t;
        
        typedef union {
          struct {
            unsigned int evt_num_bytes   :16; // Number of bytes to receive to trigger event
            unsigned int padding0:15;
            unsigned int evt_ena         :1 ; // Enable event trigger when EVT_NUM_BYTES bytes are filled in fifo
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_evt_t;
        
        typedef union {
          struct {
            unsigned int curr_addr       :32; // Current address
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_curr_addr_t;
        
        typedef union {
          struct {
            unsigned int bytes_fill      :32; // Bytes filled in fifo buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_fifo_level_t;
        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int en              :1 ; // FIFO enable - 1b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled
            unsigned int padding1:2 ;
            unsigned int stop            :1 ; // FIFO disable - 1b1 : Stops the FIFO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_core_fifo : public vp::regmap
        {
        public:
            vp_udma_core_fifo_addrgen_cfg_sa_buf addrgen_cfg_sa_buf;
            vp_udma_core_fifo_addrgen_cfg_size addrgen_cfg_size;
            vp_udma_core_fifo_addrgen_cfg_evt addrgen_cfg_evt;
            vp_udma_core_fifo_addrgen_cfg_curr_addr addrgen_cfg_curr_addr;
            vp_udma_core_fifo_addrgen_cfg_fifo_level addrgen_cfg_fifo_level;
            vp_udma_core_fifo_addrgen_cfg_ctrl addrgen_cfg_ctrl;
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_SA_BUF:

ADDRGEN_CFG_SA_BUF
""""""""""""""""""

Start Address of fifo buffer

.. table:: 

    +-----+---+-------+-----------------------+
    |Bit #|R/W| Name  |      Description      |
    +=====+===+=======+=======================+
    |31:0 |R/W|SA_ADDR|Start address of buffer|
    +-----+---+-------+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Address of fifo buffer
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_OFFSET 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_sa_buf_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_sa_buf_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start address of buffer (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_BIT                0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_WIDTH              32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_MASK               0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SA_BUF_SA_ADDR(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sa_addr         :32; // Start address of buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_sa_buf_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_sa_buf : public vp::reg_32
        {
        public:
            inline void sa_addr_set(uint32_t value);
            inline uint32_t sa_addr_get();
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_SIZE:

ADDRGEN_CFG_SIZE
""""""""""""""""

Size of buffer

.. table:: 

    +-----+---+----+----------------+
    |Bit #|R/W|Name|  Description   |
    +=====+===+====+================+
    |31:0 |R/W|SIZE|Size of transfer|
    +-----+---+----+----------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of buffer
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_OFFSET   0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_size_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of transfer (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_BIT                     0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_WIDTH                   32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_MASK                    0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_SIZE_SIZE(val)          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :32; // Size of transfer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_size : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_EVT:

ADDRGEN_CFG_EVT
"""""""""""""""

Event config

.. table:: 

    +-----+---+-------------+----------------------------------------------------------------+
    |Bit #|R/W|    Name     |                          Description                           |
    +=====+===+=============+================================================================+
    |15:0 |R/W|EVT_NUM_BYTES|Number of bytes to receive to trigger event                     |
    +-----+---+-------------+----------------------------------------------------------------+
    |31   |R/W|EVT_ENA      |Enable event trigger when EVT_NUM_BYTES bytes are filled in fifo|
    +-----+---+-------------+----------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Event config
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_OFFSET    0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_evt_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_evt_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Number of bytes to receive to trigger event (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_BIT             0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_WIDTH           16
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_MASK            0xffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_RESET           0x0
        
        // Enable event trigger when EVT_NUM_BYTES bytes are filled in fifo (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_BIT                   31
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_WIDTH                 1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_MASK                  0x80000000
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_RESET                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_NUM_BYTES(val)  ((val) << 0)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_GET(value)  (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_GETS(value) (GAP_BEXTRACT((value),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_EVT_EVT_ENA(val)        ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt_num_bytes   :16; // Number of bytes to receive to trigger event
            unsigned int padding0:15;
            unsigned int evt_ena         :1 ; // Enable event trigger when EVT_NUM_BYTES bytes are filled in fifo
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_evt_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_evt : public vp::reg_32
        {
        public:
            inline void evt_num_bytes_set(uint32_t value);
            inline uint32_t evt_num_bytes_get();
            inline void evt_ena_set(uint32_t value);
            inline uint32_t evt_ena_get();
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_CURR_ADDR:

ADDRGEN_CFG_CURR_ADDR
"""""""""""""""""""""

Current address

.. table:: 

    +-----+---+---------+---------------+
    |Bit #|R/W|  Name   |  Description  |
    +=====+===+=========+===============+
    |31:0 |R  |CURR_ADDR|Current address|
    +-----+---+---------+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Current address
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_OFFSET 0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_curr_addr_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_curr_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Current address (access: R)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_BIT           0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_WIDTH         32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_MASK          0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_RESET         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int curr_addr       :32; // Current address
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_curr_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_curr_addr : public vp::reg_32
        {
        public:
            inline void curr_addr_set(uint32_t value);
            inline uint32_t curr_addr_get();
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_FIFO_LEVEL:

ADDRGEN_CFG_FIFO_LEVEL
""""""""""""""""""""""

Current number of bytes in fifo

.. table:: 

    +-----+---+----------+---------------------------+
    |Bit #|R/W|   Name   |        Description        |
    +=====+===+==========+===========================+
    |31:0 |R  |BYTES_FILL|Bytes filled in fifo buffer|
    +-----+---+----------+---------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Current number of bytes in fifo
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_OFFSET 0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_fifo_level_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_fifo_level_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Bytes filled in fifo buffer (access: R)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_BIT         0
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_WIDTH       32
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_MASK        0xffffffff
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_RESET       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_FIFO_LEVEL_BYTES_FILL(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int bytes_fill      :32; // Bytes filled in fifo buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_fifo_level_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_fifo_level : public vp::reg_32
        {
        public:
            inline void bytes_fill_set(uint32_t value);
            inline uint32_t bytes_fill_get();
        };

|

.. _udma_core_fifo_ADDRGEN_CFG_CTRL:

ADDRGEN_CFG_CTRL
""""""""""""""""

Control register

.. table:: 

    +-----+---+----+-----------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                    Description                                    |
    +=====+===+====+===================================================================================+
    |    1|R/W|EN  |FIFO enable - 1’b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled|
    +-----+---+----+-----------------------------------------------------------------------------------+
    |    4|R/W|STOP|FIFO disable - 1’b1 : Stops the FIFO                                               |
    +-----+---+----+-----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Control register
                #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_OFFSET   0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_fifo_addrgen_cfg_ctrl_get(uint32_t base);
        static inline void udma_core_fifo_addrgen_cfg_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FIFO enable - 1b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_BIT                       1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_WIDTH                     1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_MASK                      0x2
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_RESET                     0x0
        
        // FIFO disable - 1b1 : Stops the FIFO (access: R/W)
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_BIT                     4
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_WIDTH                   1
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_MASK                    0x10
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_GET(value)      (GAP_BEXTRACTU((value),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_GETS(value)     (GAP_BEXTRACT((value),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_EN(val)            ((val) << 1)
        
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_CORE_FIFO_ADDRGEN_CFG_CTRL_STOP(val)          ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int en              :1 ; // FIFO enable - 1b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled
            unsigned int padding1:2 ;
            unsigned int stop            :1 ; // FIFO disable - 1b1 : Stops the FIFO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_fifo_addrgen_cfg_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_fifo_addrgen_cfg_ctrl : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
        };

|
