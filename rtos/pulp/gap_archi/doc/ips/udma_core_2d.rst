Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_2D_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------------------------------+------+-----+---------------------------------+
    |                               Name                               |Offset|Width|           Description           |
    +==================================================================+======+=====+=================================+
    |:ref:`ADDRGEN_CFG_SA_BUF0<udma_core_2d_ADDRGEN_CFG_SA_BUF0>`      |     0|   32|Start Address of buffer0         |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_SA_BUF1<udma_core_2d_ADDRGEN_CFG_SA_BUF1>`      |     4|   32|Start Address of buffer1         |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_SIZE<udma_core_2d_ADDRGEN_CFG_SIZE>`            |     8|   32|Size of buffers                  |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_STRIDE<udma_core_2d_ADDRGEN_CFG_STRIDE>`        |    12|   32|Size of Stride                   |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_ROW_LEN<udma_core_2d_ADDRGEN_CFG_ROW_LEN>`      |    16|   32|Length of the row                |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_CURR_ADDR<udma_core_2d_ADDRGEN_CFG_CURR_ADDR>`  |    20|   32|Current address                  |
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_BYTES_LEFT<udma_core_2d_ADDRGEN_CFG_BYTES_LEFT>`|    24|   32|Bytes remaining in current buffer|
    +------------------------------------------------------------------+------+-----+---------------------------------+
    |:ref:`ADDRGEN_CFG_CTRL<udma_core_2d_ADDRGEN_CFG_CTRL>`            |    28|   32|Control register                 |
    +------------------------------------------------------------------+------+-----+---------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Address of buffer0
                #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_OFFSET  0x0
        
                // Start Address of buffer1
                #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_OFFSET  0x4
        
                // Size of buffers
                #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_OFFSET     0x8
        
                // Size of Stride
                #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_OFFSET   0xc
        
                // Length of the row
                #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_OFFSET  0x10
        
                // Current address
                #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_OFFSET 0x14
        
                // Bytes remaining in current buffer
                #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_OFFSET 0x18
        
                // Control register
                #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_OFFSET     0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_sa_buf0_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_sa_buf0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_sa_buf1_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_sa_buf1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_size_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_stride_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_stride_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_row_len_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_row_len_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_curr_addr_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_curr_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_bytes_left_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_bytes_left_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_core_2d_addrgen_cfg_ctrl_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start address of buffer0 (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_BIT                0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_WIDTH              32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_MASK               0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_RESET              0x0
        
        // Start address of buffer1 (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_BIT                0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_WIDTH              32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_MASK               0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_RESET              0x0
        
        // Size of transfer (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_BIT                       0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_WIDTH                     32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_MASK                      0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_RESET                     0x0
        
        // Length of stride (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_BIT                   0
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_WIDTH                 32
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_MASK                  0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_RESET                 0x0
        
        // Length of the row (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_BIT                 0
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_WIDTH               32
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_MASK                0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_RESET               0x0
        
        // Current address (access: R)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_BIT             0
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_WIDTH           32
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_MASK            0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_RESET           0x0
        
        // Bytes remaining in current buffer (access: R)
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_BIT           0
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_WIDTH         32
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_MASK          0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_RESET         0x0
        
        // Enable hardware double buffer support - 1b1 : HW Double Buffer -1'b0 Legacy mode (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_BIT                       0
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_WIDTH                     1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_MASK                      0x1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_RESET                     0x0
        
        // Transfer enable - Write : 1b1 : Starts the transfer or queue a new transfer if a transfer is already started. (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_BIT                         1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_WIDTH                       1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_MASK                        0x2
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_RESET                       0x0
        
        // Stops the transfers and cleans the queue (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_BIT                       4
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_WIDTH                     1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_MASK                      0x10
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0(val)     ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1(val)     ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE(val)            ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_GET(value)  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE(val)        ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN(val)      ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR(val)  ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT(val) ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT(val)            ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN(val)              ((val) << 1)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_GET(value)      (GAP_BEXTRACTU((value),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_GETS(value)     (GAP_BEXTRACT((value),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP(val)            ((val) << 4)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_CORE_2D_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t addrgen_cfg_sa_buf0;  // Start Address of buffer0
            volatile uint32_t addrgen_cfg_sa_buf1;  // Start Address of buffer1
            volatile uint32_t addrgen_cfg_size;  // Size of buffers
            volatile uint32_t addrgen_cfg_stride;  // Size of Stride
            volatile uint32_t addrgen_cfg_row_len;  // Length of the row
            volatile uint32_t addrgen_cfg_curr_addr;  // Current address
            volatile uint32_t addrgen_cfg_bytes_left;  // Bytes remaining in current buffer
            volatile uint32_t addrgen_cfg_ctrl;  // Control register
        } __attribute__((packed)) udma_core_2d_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sa_addr0        :32; // Start address of buffer0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_sa_buf0_t;
        
        typedef union {
          struct {
            unsigned int sa_addr1        :32; // Start address of buffer1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_sa_buf1_t;
        
        typedef union {
          struct {
            unsigned int size            :32; // Size of transfer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_size_t;
        
        typedef union {
          struct {
            unsigned int stride          :32; // Length of stride
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_stride_t;
        
        typedef union {
          struct {
            unsigned int row_len         :32; // Length of the row
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_row_len_t;
        
        typedef union {
          struct {
            unsigned int curr_addr       :32; // Current address
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_curr_addr_t;
        
        typedef union {
          struct {
            unsigned int bytes_left      :32; // Bytes remaining in current buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_bytes_left_t;
        
        typedef union {
          struct {
            unsigned int cont            :1 ; // Enable hardware double buffer support - 1b1 : HW Double Buffer -1'b0 Legacy mode
            unsigned int en              :1 ; // Transfer enable - Write : 1b1 : Starts the transfer or queue a new transfer if a transfer is already started.
            unsigned int padding0:2 ;
            unsigned int stop            :1 ; // Stops the transfers and cleans the queue
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_core_2d : public vp::regmap
        {
        public:
            vp_udma_core_2d_addrgen_cfg_sa_buf0 addrgen_cfg_sa_buf0;
            vp_udma_core_2d_addrgen_cfg_sa_buf1 addrgen_cfg_sa_buf1;
            vp_udma_core_2d_addrgen_cfg_size addrgen_cfg_size;
            vp_udma_core_2d_addrgen_cfg_stride addrgen_cfg_stride;
            vp_udma_core_2d_addrgen_cfg_row_len addrgen_cfg_row_len;
            vp_udma_core_2d_addrgen_cfg_curr_addr addrgen_cfg_curr_addr;
            vp_udma_core_2d_addrgen_cfg_bytes_left addrgen_cfg_bytes_left;
            vp_udma_core_2d_addrgen_cfg_ctrl addrgen_cfg_ctrl;
        };

|

.. _udma_core_2d_ADDRGEN_CFG_SA_BUF0:

ADDRGEN_CFG_SA_BUF0
"""""""""""""""""""

Start Address of buffer0

.. table:: 

    +-----+---+--------+------------------------+
    |Bit #|R/W|  Name  |      Description       |
    +=====+===+========+========================+
    |31:0 |R/W|SA_ADDR0|Start address of buffer0|
    +-----+---+--------+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Address of buffer0
                #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_OFFSET  0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_sa_buf0_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_sa_buf0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start address of buffer0 (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_BIT                0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_WIDTH              32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_MASK               0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF0_SA_ADDR0(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sa_addr0        :32; // Start address of buffer0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_sa_buf0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_sa_buf0 : public vp::reg_32
        {
        public:
            inline void sa_addr0_set(uint32_t value);
            inline uint32_t sa_addr0_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_SA_BUF1:

ADDRGEN_CFG_SA_BUF1
"""""""""""""""""""

Start Address of buffer1

.. table:: 

    +-----+---+--------+------------------------+
    |Bit #|R/W|  Name  |      Description       |
    +=====+===+========+========================+
    |31:0 |R/W|SA_ADDR1|Start address of buffer1|
    +-----+---+--------+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start Address of buffer1
                #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_OFFSET  0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_sa_buf1_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_sa_buf1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start address of buffer1 (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_BIT                0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_WIDTH              32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_MASK               0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SA_BUF1_SA_ADDR1(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sa_addr1        :32; // Start address of buffer1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_sa_buf1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_sa_buf1 : public vp::reg_32
        {
        public:
            inline void sa_addr1_set(uint32_t value);
            inline uint32_t sa_addr1_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_SIZE:

ADDRGEN_CFG_SIZE
""""""""""""""""

Size of buffers

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

        
                // Size of buffers
                #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_OFFSET     0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_size_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of transfer (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_BIT                       0
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_WIDTH                     32
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_MASK                      0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_SIZE_SIZE(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :32; // Size of transfer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_size : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_STRIDE:

ADDRGEN_CFG_STRIDE
""""""""""""""""""

Size of Stride

.. table:: 

    +-----+---+------+----------------+
    |Bit #|R/W| Name |  Description   |
    +=====+===+======+================+
    |31:0 |R/W|STRIDE|Length of stride|
    +-----+---+------+----------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of Stride
                #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_OFFSET   0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_stride_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_stride_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Length of stride (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_BIT                   0
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_WIDTH                 32
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_MASK                  0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_RESET                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_GET(value)  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_STRIDE_STRIDE(val)        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int stride          :32; // Length of stride
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_stride_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_stride : public vp::reg_32
        {
        public:
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_ROW_LEN:

ADDRGEN_CFG_ROW_LEN
"""""""""""""""""""

Length of the row

.. table:: 

    +-----+---+-------+-----------------+
    |Bit #|R/W| Name  |   Description   |
    +=====+===+=======+=================+
    |31:0 |R/W|ROW_LEN|Length of the row|
    +-----+---+-------+-----------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Length of the row
                #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_OFFSET  0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_row_len_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_row_len_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Length of the row (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_BIT                 0
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_WIDTH               32
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_MASK                0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_ROW_LEN_ROW_LEN(val)      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int row_len         :32; // Length of the row
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_row_len_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_row_len : public vp::reg_32
        {
        public:
            inline void row_len_set(uint32_t value);
            inline uint32_t row_len_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_CURR_ADDR:

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
                #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_OFFSET 0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_curr_addr_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_curr_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Current address (access: R)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_BIT             0
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_WIDTH           32
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_MASK            0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_RESET           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CURR_ADDR_CURR_ADDR(val)  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int curr_addr       :32; // Current address
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_curr_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_curr_addr : public vp::reg_32
        {
        public:
            inline void curr_addr_set(uint32_t value);
            inline uint32_t curr_addr_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_BYTES_LEFT:

ADDRGEN_CFG_BYTES_LEFT
""""""""""""""""""""""

Bytes remaining in current buffer

.. table:: 

    +-----+---+----------+---------------------------------+
    |Bit #|R/W|   Name   |           Description           |
    +=====+===+==========+=================================+
    |31:0 |R  |BYTES_LEFT|Bytes remaining in current buffer|
    +-----+---+----------+---------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Bytes remaining in current buffer
                #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_OFFSET 0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_bytes_left_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_bytes_left_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Bytes remaining in current buffer (access: R)
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_BIT           0
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_WIDTH         32
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_MASK          0xffffffff
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_RESET         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_BYTES_LEFT_BYTES_LEFT(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int bytes_left      :32; // Bytes remaining in current buffer
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_bytes_left_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_bytes_left : public vp::reg_32
        {
        public:
            inline void bytes_left_set(uint32_t value);
            inline uint32_t bytes_left_get();
        };

|

.. _udma_core_2d_ADDRGEN_CFG_CTRL:

ADDRGEN_CFG_CTRL
""""""""""""""""

Control register

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                 Description                                                  |
    +=====+===+====+==============================================================================================================+
    |    0|R/W|CONT|Enable hardware double buffer support - 1’b1 : HW Double Buffer -1'b0 Legacy mode                             |
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |    1|R/W|EN  |Transfer enable - Write : 1’b1 : Starts the transfer or queue a new transfer if a transfer is already started.|
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |    4|R/W|STOP|Stops the transfers and cleans the queue                                                                      |
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Control register
                #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_OFFSET     0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_core_2d_addrgen_cfg_ctrl_get(uint32_t base);
        static inline void udma_core_2d_addrgen_cfg_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable hardware double buffer support - 1b1 : HW Double Buffer -1'b0 Legacy mode (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_BIT                       0
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_WIDTH                     1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_MASK                      0x1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_RESET                     0x0
        
        // Transfer enable - Write : 1b1 : Starts the transfer or queue a new transfer if a transfer is already started. (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_BIT                         1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_WIDTH                       1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_MASK                        0x2
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_RESET                       0x0
        
        // Stops the transfers and cleans the queue (access: R/W)
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_BIT                       4
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_WIDTH                     1
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_MASK                      0x10
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_CONT(val)            ((val) << 0)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_EN(val)              ((val) << 1)
        
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_GET(value)      (GAP_BEXTRACTU((value),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_GETS(value)     (GAP_BEXTRACT((value),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_CORE_2D_ADDRGEN_CFG_CTRL_STOP(val)            ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cont            :1 ; // Enable hardware double buffer support - 1b1 : HW Double Buffer -1'b0 Legacy mode
            unsigned int en              :1 ; // Transfer enable - Write : 1b1 : Starts the transfer or queue a new transfer if a transfer is already started.
            unsigned int padding0:2 ;
            unsigned int stop            :1 ; // Stops the transfers and cleans the queue
          };
          unsigned int raw;
        } __attribute__((packed)) udma_core_2d_addrgen_cfg_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_core_2d_addrgen_cfg_ctrl : public vp::reg_32
        {
        public:
            inline void cont_set(uint32_t value);
            inline uint32_t cont_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
        };

|
