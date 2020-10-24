Input file: fe/ips/udma/udma_ffc/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |                   Name                   |Offset|Width|                                   Description                                    |
    +==========================================+======+=====+==================================================================================+
    |:ref:`RX_DEST<udma_ffc_RX_DEST>`          |     0|   32|Stream ID for the uDMA channel                                                    |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`TX_DEST<udma_ffc_TX_DEST>`          |     4|   32|Stream ID for the uDMA channel                                                    |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`FL_FORMAT<udma_ffc_FL_FORMAT>`      |     8|   32|Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32         |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`FP_FORMAT<udma_ffc_FP_FORMAT>`      |    12|   32|Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits|
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`FP_PREC<udma_ffc_FP_PREC>`          |    16|   32|Fix point precision, must less than fix point size                                |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`FP_SCALE<udma_ffc_FP_SCALE>`        |    20|   32|Fix point scale, signed number, FL * 2^scale                                      |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`MODE<udma_ffc_MODE>`                |    24|   32|Direction,  0: Float to Fix   1: Fix to Float                                     |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`TRANS_MODE<udma_ffc_TRANS_MODE>`    |    28|   32|Use auto tranfer or not                                                           |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CONV_NUM<udma_ffc_CONV_NUM>`        |    32|   32|Convertion data number                                                            |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CONV_TX_ADDR<udma_ffc_CONV_TX_ADDR>`|    36|   32|Only use when auto mode is set, convertion data tx channel read from l2 address   |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CONV_RX_ADDR<udma_ffc_CONV_RX_ADDR>`|    40|   32|Only use when auto mode is set, convertion data tx channel read from l2 address   |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`IRQ_EN<udma_ffc_IRQ_EN>`            |    44|   32|Enable or disable transfer interrupt                                              |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`STATUS<udma_ffc_STATUS>`            |    48|   32|Show if the transfer is finished or not                                           |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`START<udma_ffc_START>`              |    52|   32|Start transition                                                                  |
    +------------------------------------------+------+-----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_FFC_RX_DEST_OFFSET                  0x0
        
                // Stream ID for the uDMA channel
                #define UDMA_FFC_TX_DEST_OFFSET                  0x4
        
                // Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32
                #define UDMA_FFC_FL_FORMAT_OFFSET                0x8
        
                // Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits
                #define UDMA_FFC_FP_FORMAT_OFFSET                0xc
        
                // Fix point precision, must less than fix point size
                #define UDMA_FFC_FP_PREC_OFFSET                  0x10
        
                // Fix point scale, signed number, FL * 2^scale
                #define UDMA_FFC_FP_SCALE_OFFSET                 0x14
        
                // Direction,  0: Float to Fix   1: Fix to Float
                #define UDMA_FFC_MODE_OFFSET                     0x18
        
                // Use auto tranfer or not
                #define UDMA_FFC_TRANS_MODE_OFFSET               0x1c
        
                // Convertion data number
                #define UDMA_FFC_CONV_NUM_OFFSET                 0x20
        
                // Only use when auto mode is set, convertion data tx channel read from l2 address
                #define UDMA_FFC_CONV_TX_ADDR_OFFSET             0x24
        
                // Only use when auto mode is set, convertion data tx channel read from l2 address
                #define UDMA_FFC_CONV_RX_ADDR_OFFSET             0x28
        
                // Enable or disable transfer interrupt
                #define UDMA_FFC_IRQ_EN_OFFSET                   0x2c
        
                // Show if the transfer is finished or not
                #define UDMA_FFC_STATUS_OFFSET                   0x30
        
                // Start transition
                #define UDMA_FFC_START_OFFSET                    0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_rx_dest_get(uint32_t base);
        static inline void udma_ffc_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_tx_dest_get(uint32_t base);
        static inline void udma_ffc_tx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_fl_format_get(uint32_t base);
        static inline void udma_ffc_fl_format_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_fp_format_get(uint32_t base);
        static inline void udma_ffc_fp_format_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_fp_prec_get(uint32_t base);
        static inline void udma_ffc_fp_prec_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_fp_scale_get(uint32_t base);
        static inline void udma_ffc_fp_scale_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_mode_get(uint32_t base);
        static inline void udma_ffc_mode_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_trans_mode_get(uint32_t base);
        static inline void udma_ffc_trans_mode_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_conv_num_get(uint32_t base);
        static inline void udma_ffc_conv_num_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_conv_tx_addr_get(uint32_t base);
        static inline void udma_ffc_conv_tx_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_conv_rx_addr_get(uint32_t base);
        static inline void udma_ffc_conv_rx_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_irq_en_get(uint32_t base);
        static inline void udma_ffc_irq_en_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_status_get(uint32_t base);
        static inline void udma_ffc_status_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ffc_start_get(uint32_t base);
        static inline void udma_ffc_start_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_FFC_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t rx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t tx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t fl_format;  // Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32
            volatile uint32_t fp_format;  // Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits
            volatile uint32_t fp_prec;  // Fix point precision, must less than fix point size
            volatile uint32_t fp_scale;  // Fix point scale, signed number, FL * 2^scale
            volatile uint32_t mode;  // Direction,  0: Float to Fix   1: Fix to Float
            volatile uint32_t trans_mode;  // Use auto tranfer or not
            volatile uint32_t conv_num;  // Convertion data number
            volatile uint32_t conv_tx_addr;  // Only use when auto mode is set, convertion data tx channel read from l2 address
            volatile uint32_t conv_rx_addr;  // Only use when auto mode is set, convertion data tx channel read from l2 address
            volatile uint32_t irq_en;  // Enable or disable transfer interrupt
            volatile uint32_t status;  // Show if the transfer is finished or not
            volatile uint32_t start;  // Start transition
        } __attribute__((packed)) udma_ffc_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_rx_dest_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_tx_dest_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_fl_format_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_fp_format_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_fp_prec_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_fp_scale_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_mode_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_trans_mode_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_conv_num_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_conv_tx_addr_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_conv_rx_addr_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_irq_en_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_status_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ffc_start_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_ffc : public vp::regmap
        {
        public:
            vp_udma_ffc_rx_dest rx_dest;
            vp_udma_ffc_tx_dest tx_dest;
            vp_udma_ffc_fl_format fl_format;
            vp_udma_ffc_fp_format fp_format;
            vp_udma_ffc_fp_prec fp_prec;
            vp_udma_ffc_fp_scale fp_scale;
            vp_udma_ffc_mode mode;
            vp_udma_ffc_trans_mode trans_mode;
            vp_udma_ffc_conv_num conv_num;
            vp_udma_ffc_conv_tx_addr conv_tx_addr;
            vp_udma_ffc_conv_rx_addr conv_rx_addr;
            vp_udma_ffc_irq_en irq_en;
            vp_udma_ffc_status status;
            vp_udma_ffc_start start;
        };

|

.. _udma_ffc_RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA channel

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

        
                // Stream ID for the uDMA channel
                #define UDMA_FFC_RX_DEST_OFFSET                  0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_rx_dest_get(uint32_t base);
        static inline void udma_ffc_rx_dest_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_rx_dest : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA channel

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

        
                // Stream ID for the uDMA channel
                #define UDMA_FFC_TX_DEST_OFFSET                  0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_tx_dest_get(uint32_t base);
        static inline void udma_ffc_tx_dest_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_tx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_tx_dest : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_FL_FORMAT:

FL_FORMAT
"""""""""

Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32

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

        
                // Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32
                #define UDMA_FFC_FL_FORMAT_OFFSET                0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_fl_format_get(uint32_t base);
        static inline void udma_ffc_fl_format_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_fl_format_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_fl_format : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_FP_FORMAT:

FP_FORMAT
"""""""""

Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits

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

        
                // Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits
                #define UDMA_FFC_FP_FORMAT_OFFSET                0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_fp_format_get(uint32_t base);
        static inline void udma_ffc_fp_format_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_fp_format_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_fp_format : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_FP_PREC:

FP_PREC
"""""""

Fix point precision, must less than fix point size

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

        
                // Fix point precision, must less than fix point size
                #define UDMA_FFC_FP_PREC_OFFSET                  0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_fp_prec_get(uint32_t base);
        static inline void udma_ffc_fp_prec_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_fp_prec_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_fp_prec : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_FP_SCALE:

FP_SCALE
""""""""

Fix point scale, signed number, FL * 2^scale

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

        
                // Fix point scale, signed number, FL * 2^scale
                #define UDMA_FFC_FP_SCALE_OFFSET                 0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_fp_scale_get(uint32_t base);
        static inline void udma_ffc_fp_scale_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_fp_scale_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_fp_scale : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_MODE:

MODE
""""

Direction,  0: Float to Fix   1: Fix to Float

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

        
                // Direction,  0: Float to Fix   1: Fix to Float
                #define UDMA_FFC_MODE_OFFSET                     0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_mode_get(uint32_t base);
        static inline void udma_ffc_mode_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_mode_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_mode : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_TRANS_MODE:

TRANS_MODE
""""""""""

Use auto tranfer or not

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

        
                // Use auto tranfer or not
                #define UDMA_FFC_TRANS_MODE_OFFSET               0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_trans_mode_get(uint32_t base);
        static inline void udma_ffc_trans_mode_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_trans_mode_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_trans_mode : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_CONV_NUM:

CONV_NUM
""""""""

Convertion data number

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

        
                // Convertion data number
                #define UDMA_FFC_CONV_NUM_OFFSET                 0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_conv_num_get(uint32_t base);
        static inline void udma_ffc_conv_num_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_conv_num_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_conv_num : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_CONV_TX_ADDR:

CONV_TX_ADDR
""""""""""""

Only use when auto mode is set, convertion data tx channel read from l2 address

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

        
                // Only use when auto mode is set, convertion data tx channel read from l2 address
                #define UDMA_FFC_CONV_TX_ADDR_OFFSET             0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_conv_tx_addr_get(uint32_t base);
        static inline void udma_ffc_conv_tx_addr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_conv_tx_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_conv_tx_addr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_CONV_RX_ADDR:

CONV_RX_ADDR
""""""""""""

Only use when auto mode is set, convertion data tx channel read from l2 address

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

        
                // Only use when auto mode is set, convertion data tx channel read from l2 address
                #define UDMA_FFC_CONV_RX_ADDR_OFFSET             0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_conv_rx_addr_get(uint32_t base);
        static inline void udma_ffc_conv_rx_addr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_conv_rx_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_conv_rx_addr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_IRQ_EN:

IRQ_EN
""""""

Enable or disable transfer interrupt

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

        
                // Enable or disable transfer interrupt
                #define UDMA_FFC_IRQ_EN_OFFSET                   0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_irq_en_get(uint32_t base);
        static inline void udma_ffc_irq_en_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_irq_en_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_irq_en : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_STATUS:

STATUS
""""""

Show if the transfer is finished or not

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

        
                // Show if the transfer is finished or not
                #define UDMA_FFC_STATUS_OFFSET                   0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_status_get(uint32_t base);
        static inline void udma_ffc_status_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_status : public vp::reg_32
        {
        public:
        };

|

.. _udma_ffc_START:

START
"""""

Start transition

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

        
                // Start transition
                #define UDMA_FFC_START_OFFSET                    0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ffc_start_get(uint32_t base);
        static inline void udma_ffc_start_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ffc_start_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ffc_start : public vp::reg_32
        {
        public:
        };

|
