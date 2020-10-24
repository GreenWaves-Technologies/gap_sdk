Input file: README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-----------------------------------+------+-----+-------------------------------+
    |               Name                |Offset|Width|          Description          |
    +===================================+======+=====+===============================+
    |:ref:`RX_DEST<udma_spim_RX_DEST>`  |     0|    7|Stream ID for the uDMA channel.|
    +-----------------------------------+------+-----+-------------------------------+
    |:ref:`TX_DEST<udma_spim_TX_DEST>`  |     4|    7|Stream ID for the uDMA channel.|
    +-----------------------------------+------+-----+-------------------------------+
    |:ref:`CMD_DEST<udma_spim_CMD_DEST>`|     8|    7|Stream ID for the uDMA channel.|
    +-----------------------------------+------+-----+-------------------------------+
    |:ref:`STATUS<udma_spim_STATUS>`    |    48|    1|Status register.               |
    +-----------------------------------+------+-----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_RX_DEST_OFFSET                 0x0
        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_TX_DEST_OFFSET                 0x4
        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_CMD_DEST_OFFSET                0x8
        
                // Status register.
                #define UDMA_SPIM_STATUS_OFFSET                  0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_spim_rx_dest_get(uint32_t base);
        static inline void udma_spim_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_spim_tx_dest_get(uint32_t base);
        static inline void udma_spim_tx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_spim_cmd_dest_get(uint32_t base);
        static inline void udma_spim_cmd_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_spim_status_get(uint32_t base);
        static inline void udma_spim_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_RX_DEST_RX_DEST_BIT                                0
        #define UDMA_SPIM_RX_DEST_RX_DEST_WIDTH                              7
        #define UDMA_SPIM_RX_DEST_RX_DEST_MASK                               0x7f
        #define UDMA_SPIM_RX_DEST_RX_DEST_RESET                              0x7f
        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_TX_DEST_TX_DEST_BIT                                0
        #define UDMA_SPIM_TX_DEST_TX_DEST_WIDTH                              7
        #define UDMA_SPIM_TX_DEST_TX_DEST_MASK                               0x7f
        #define UDMA_SPIM_TX_DEST_TX_DEST_RESET                              0x7f
        
        // Stream ID for the CMD uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_BIT                              0
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_WIDTH                            7
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_MASK                             0x7f
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_RESET                            0x7f
        
        // Status register. (access: RW)
        #define UDMA_SPIM_STATUS_STATUS_BIT                                  0
        #define UDMA_SPIM_STATUS_STATUS_WIDTH                                32
        #define UDMA_SPIM_STATUS_STATUS_MASK                                 0xffffffff
        #define UDMA_SPIM_STATUS_STATUS_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_SPIM_RX_DEST_RX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST(val)                     ((val) << 0)
        
        #define UDMA_SPIM_TX_DEST_TX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST(val)                     ((val) << 0)
        
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_GET(value)             (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_GETS(value)            (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST(val)                   ((val) << 0)
        
        #define UDMA_SPIM_STATUS_STATUS_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_SPIM_STATUS_STATUS_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_SPIM_STATUS_STATUS_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_SPIM_STATUS_STATUS(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_SPIM_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t rx_dest;  // Stream ID for the uDMA channel.
            volatile uint32_t tx_dest;  // Stream ID for the uDMA channel.
            volatile uint32_t cmd_dest;  // Stream ID for the uDMA channel.
            volatile uint32_t reserved_0[9];  // Reserved/Not used.
            volatile uint32_t status;  // Status register.
        } __attribute__((packed)) udma_spim_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_rx_dest_t;
        
        typedef union {
          struct {
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_tx_dest_t;
        
        typedef union {
          struct {
            unsigned int cmd_dest        :7 ; // Stream ID for the CMD uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_cmd_dest_t;
        
        typedef union {
          struct {
            unsigned int status          :32; // Status register.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_spim : public vp::regmap
        {
        public:
            vp_udma_spim_rx_dest rx_dest;
            vp_udma_spim_tx_dest tx_dest;
            vp_udma_spim_cmd_dest cmd_dest;
            vp_udma_spim_status status;
        };

|

.. _udma_spim_RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA channel.

.. table:: 

    +-----+---+-------+---------------------------------------------------------------------+
    |Bit #|R/W| Name  |                             Description                             |
    +=====+===+=======+=====================================================================+
    |6:0  |RW |RX_DEST|Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled).|
    +-----+---+-------+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_RX_DEST_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_spim_rx_dest_get(uint32_t base);
        static inline void udma_spim_rx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_RX_DEST_RX_DEST_BIT                                0
        #define UDMA_SPIM_RX_DEST_RX_DEST_WIDTH                              7
        #define UDMA_SPIM_RX_DEST_RX_DEST_MASK                               0x7f
        #define UDMA_SPIM_RX_DEST_RX_DEST_RESET                              0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_SPIM_RX_DEST_RX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_RX_DEST_RX_DEST(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_spim_rx_dest : public vp::reg_8
        {
        public:
            inline void rx_dest_set(uint8_t value);
            inline uint8_t rx_dest_get();
        };

|

.. _udma_spim_TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA channel.

.. table:: 

    +-----+---+-------+---------------------------------------------------------------------+
    |Bit #|R/W| Name  |                             Description                             |
    +=====+===+=======+=====================================================================+
    |6:0  |RW |TX_DEST|Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled).|
    +-----+---+-------+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_TX_DEST_OFFSET                 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_spim_tx_dest_get(uint32_t base);
        static inline void udma_spim_tx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_TX_DEST_TX_DEST_BIT                                0
        #define UDMA_SPIM_TX_DEST_TX_DEST_WIDTH                              7
        #define UDMA_SPIM_TX_DEST_TX_DEST_MASK                               0x7f
        #define UDMA_SPIM_TX_DEST_TX_DEST_RESET                              0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_SPIM_TX_DEST_TX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_TX_DEST_TX_DEST(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_tx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_spim_tx_dest : public vp::reg_8
        {
        public:
            inline void tx_dest_set(uint8_t value);
            inline uint8_t tx_dest_get();
        };

|

.. _udma_spim_CMD_DEST:

CMD_DEST
""""""""

Stream ID for the uDMA channel.

.. table:: 

    +-----+---+--------+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                             Description                              |
    +=====+===+========+======================================================================+
    |6:0  |RW |CMD_DEST|Stream ID for the CMD uDMA channel. Default is 0x7F(channel disabled).|
    +-----+---+--------+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel.
                #define UDMA_SPIM_CMD_DEST_OFFSET                0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_spim_cmd_dest_get(uint32_t base);
        static inline void udma_spim_cmd_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the CMD uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_BIT                              0
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_WIDTH                            7
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_MASK                             0x7f
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_RESET                            0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_GET(value)             (GAP_BEXTRACTU((value),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_GETS(value)            (GAP_BEXTRACT((value),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST_SET(value,field)       (GAP_BINSERT((value),(field),7,0))
        #define UDMA_SPIM_CMD_DEST_CMD_DEST(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd_dest        :7 ; // Stream ID for the CMD uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_cmd_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_spim_cmd_dest : public vp::reg_8
        {
        public:
            inline void cmd_dest_set(uint8_t value);
            inline uint8_t cmd_dest_get();
        };

|

.. _udma_spim_STATUS:

STATUS
""""""

Status register.

.. table:: 

    +-----+---+------+----------------+
    |Bit #|R/W| Name |  Description   |
    +=====+===+======+================+
    |31:0 |RW |STATUS|Status register.|
    +-----+---+------+----------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Status register.
                #define UDMA_SPIM_STATUS_OFFSET                  0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_spim_status_get(uint32_t base);
        static inline void udma_spim_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Status register. (access: RW)
        #define UDMA_SPIM_STATUS_STATUS_BIT                                  0
        #define UDMA_SPIM_STATUS_STATUS_WIDTH                                32
        #define UDMA_SPIM_STATUS_STATUS_MASK                                 0xffffffff
        #define UDMA_SPIM_STATUS_STATUS_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_SPIM_STATUS_STATUS_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_SPIM_STATUS_STATUS_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_SPIM_STATUS_STATUS_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_SPIM_STATUS_STATUS(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int status          :32; // Status register.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_spim_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_spim_status : public vp::reg_1
        {
        public:
            inline void status_set(uint1_t value);
            inline uint1_t status_get();
        };

|

SPI micro-code
^^^^^^^^^^^^^^

.. table:: 

    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |                           Name                           |Offset|Width|                                   Description                                    |
    +==========================================================+======+=====+==================================================================================+
    |:ref:`SPI_CMD_CFG<SPI micro-code_SPI_CMD_CFG>`            |    32|0x0  |Sets the configuration for the SPI Master IP                                      |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_SOT<SPI micro-code_SPI_CMD_SOT>`            |    32|0x1  |Sets the Chip Select (CS)                                                         |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_SEND_CMD<SPI micro-code_SPI_CMD_SEND_CMD>`  |    32|0x2  |Transmits up to 16bits of data sent in the command                                |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_DUMMY<SPI micro-code_SPI_CMD_DUMMY>`        |    32|0x4  |Receives a number of dummy bits (not sent to the rx interface)                    |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_WAIT<SPI micro-code_SPI_CMD_WAIT>`          |    32|0x5  |Waits an external event to move to the next instruction                           |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_TX_DATA<SPI micro-code_SPI_CMD_TX_DATA>`    |    32|0x6  |Sends data (max 256Kbits)                                                         |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_RX_DATA<SPI micro-code_SPI_CMD_RX_DATA>`    |    32|0x7  |Receives data (max 256Kbits)                                                      |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_RPT<SPI micro-code_SPI_CMD_RPT>`            |    32|0x8  |Repeat the commands until RTP_END for N times                                     |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_EOT<SPI micro-code_SPI_CMD_EOT>`            |    32|0x9  |Clears the Chip Select (CS)                                                       |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_RPT_END<SPI micro-code_SPI_CMD_RPT_END>`    |    32|0xA  |End of the repeat loop command                                                    |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_RX_CHECK<SPI micro-code_SPI_CMD_RX_CHECK>`  |    32|0xB  |Checks up to 16 bits of data against an expected value                            |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_FULL_DUPL<SPI micro-code_SPI_CMD_FULL_DUPL>`|    32|0xC  |Activate full duplex mode                                                         |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`SPI_CMD_SETUP_AG<SPI micro-code_SPI_CMD_SETUP_AG>`  |    32|0xD  |Setup a register of the address generator(register value is read in the next word)|
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_CFG:

SPI_CMD_CFG
"""""""""""

.. table:: 

    +-----+-----+---------------------------------------------------+
    |Bit #|Value|                    Description                    |
    +=====+=====+===================================================+
    |7:0  |ARG  |Sets the clock divider value                       |
    +-----+-----+---------------------------------------------------+
    |8    |ARG  |Sets the clock phase: - 1'b0:  - 1'b1:             |
    +-----+-----+---------------------------------------------------+
    |9    |ARG  |Sets the clock polarity: - 1'b0: - 1'b1:           |
    +-----+-----+---------------------------------------------------+
    |31:28|0x0  |Select the SPIM command to be processed. Here “CFG”|
    +-----+-----+---------------------------------------------------+

.. _SPI micro-code_SPI_CMD_SOT:

SPI_CMD_SOT
"""""""""""

.. table:: 

    +-----+-----+--------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                 Description                                                  |
    +=====+=====+==============================================================================================================+
    |1:0  |0x0  |Sets the Chip Select (CS): - 2'b00: select csn0 - 2'b01: select csn1 - 2'b10: select csn2 - 2'b11: select csn3|
    +-----+-----+--------------------------------------------------------------------------------------------------------------+
    |31:28|0x1  |Select the SPIM command to be processed. Here “SOT”.                                                          |
    +-----+-----+--------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_SEND_CMD:

SPI_CMD_SEND_CMD
""""""""""""""""

.. table:: 

    +-----+-----+----------------------------------------------------------------------------------------+
    |Bit #|Value|                                      Description                                       |
    +=====+=====+========================================================================================+
    |15:0 |ARG  |Sets the command to send. MSB must always be at bit15 also if cmd size is lower than 16.|
    +-----+-----+----------------------------------------------------------------------------------------+
    |19:16|ARG  |Size in bits of the command to send. The value written here is num bits – 1.            |
    +-----+-----+----------------------------------------------------------------------------------------+
    |26   |ARG  |Sends the data starting from LSB.  the data starting from LSB.                          |
    +-----+-----+----------------------------------------------------------------------------------------+
    |27   |ARG  |Sends the command using QuadSPI.                                                        |
    +-----+-----+----------------------------------------------------------------------------------------+
    |31:28|0x2  |Select the SPIM command to be processed. Here “SEND_CMD”.                               |
    +-----+-----+----------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_DUMMY:

SPI_CMD_DUMMY
"""""""""""""

.. table:: 

    +-----+-----+------------------------------------------------------+
    |Bit #|Value|                     Description                      |
    +=====+=====+======================================================+
    |20:16|ARG  |Number of dummy cycles to perform.                    |
    +-----+-----+------------------------------------------------------+
    |31:28|0x4  |Select the SPIM command to be processed. Here “DUMMY”.|
    +-----+-----+------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_WAIT:

SPI_CMD_WAIT
""""""""""""

.. table:: 

    +-----+-----+-------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                    Description                                                    |
    +=====+=====+===================================================================================================================+
    |6:0  |ARG  |External event id or Number of wait cycles                                                                         |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------+
    |9:8  |ARG  |Type of wait: -2’b00: wait a soc event selected by EVENT_ID -2’b01: wait CYCLE_COUNT cycles -2’b10: rfu -2’b11: rfu|
    +-----+-----+-------------------------------------------------------------------------------------------------------------------+
    |31:28|0x5  |Select the SPIM command to be processed. Here “WAIT”                                                               |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_TX_DATA:

SPI_CMD_TX_DATA
"""""""""""""""

.. table:: 

    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                                                                                                                                    Description                                                                                                                                                                     |
    +=====+=====+====================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |ARG  |Number of words to send (Max 64K). The number of bits sent is this value multiplied by the word size. The value written here is num words – 1.                                                                                                                                                                                                      |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |20:16|ARG  |Size in bits of each word sent. Each word to be sent will be read from L2 with a transfer, whose size can be configured with WORD_PER_TRANSF. The value written here is num bits – 1.                                                                                                                                                               |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22:21|ARG  |Number of bytes transferred from L2 at each transfer -2’b00: 1 byte per transfer -2’b01: 2 bytes per transfer -2’b10: 4 bytes per transfer. One transfer will be triggered for each word to be sent. Having different values between WORD_SIZE and WORD_PER_TRANSF can be used to insert dummy bits and drop bits, compared to what is read from L2.|
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |26   |ARG  |Sends the data starting from LSB.                                                                                                                                                                                                                                                                                                                   |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |27   |ARG  |Sends the command using QuadSPI.                                                                                                                                                                                                                                                                                                                    |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:28|0x6  |Select the SPIM command to be processed. Here “TX_DATA”.                                                                                                                                                                                                                                                                                            |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_RX_DATA:

SPI_CMD_RX_DATA
"""""""""""""""

.. table:: 

    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                                                                                                                                                                                                                                         Description                                                                                                                                                                                                                                                                         |
    +=====+=====+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |ARG  |Number of words to receive (Max 64K). The number of bits received is this value multiplied by the word size. The value written here is num words – 1.                                                                                                                                                                                                                                                                                                                                                                                                        |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |20:16|ARG  |Size in bits of each word sent. The value written here is num bits – 1.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22:21|ARG  |Number of words transferred from L2 at each transfer -2’b00: 1 word per transfer -2’b01: 2 words per transfer -2’b10: 4 words per transfer. The UDMA always packs 4 bytes into each transfer. This field allows setting how many words (with size specified in WORD_SIZE) should be packed together in one transfer. WORD_SIZE is always rounded-up to the closest supported word-size (8, 16 or 32). If the rounded word-size multiplied by this field is not equal to 32bits, the UDMA will either insert dummy bytes or drop bytes in the 32bits transfer.|
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |26   |ARG  |Sends the data starting from LSB.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |27   |ARG  |Sends the command using QuadSPI.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:28|0x7  |Select the SPIM command to be processed. Here “TX_DATA”                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_RPT:

SPI_CMD_RPT
"""""""""""

.. table:: 

    +-----+-----+----------------------------------------------------+
    |Bit #|Value|                    Description                     |
    +=====+=====+====================================================+
    |15:0 |ARG  |Number of repeat iterations(Max 64K).               |
    +-----+-----+----------------------------------------------------+
    |31:28|0x8  |Select the SPIM command to be processed. Here “RPT”.|
    +-----+-----+----------------------------------------------------+

.. _SPI micro-code_SPI_CMD_EOT:

SPI_CMD_EOT
"""""""""""

.. table:: 

    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                                 Description                                                                  |
    +=====+=====+==============================================================================================================================================+
    |    0|ARG  |Enable EOT event: - 1'b0: disable - 1'b1: enable. This will generate an EOT event only if this field is enabled.                              |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|ARG  |Keep chip-select active: - 1'b0: disable - 1'b1: enable. If enabled, the chip-select will be kept active (low) after the transfer is finished.|
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------+
    |31:28|0x9  |Select the SPIM command to be processed. Here “EOT”.                                                                                          |
    +-----+-----+----------------------------------------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_RPT_END:

SPI_CMD_RPT_END
"""""""""""""""

.. table:: 

    +-----+-----+--------------------------------------------------------+
    |Bit #|Value|                      Description                       |
    +=====+=====+========================================================+
    |31:28|0xA  |Select the SPIM command to be processed. Here “RPT_END”.|
    +-----+-----+--------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_RX_CHECK:

SPI_CMD_RX_CHECK
""""""""""""""""

.. table:: 

    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                               Description                                                |
    +=====+=====+==========================================================================================================+
    |15:0 |ARG  |Data to compare. Max 16bits.                                                                              |
    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |19:16|ARG  |Size in bits of the word to read. The value written here is num bits – 1.                                 |
    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |25:24|ARG  |Select check to process: - 2'b00: compare bit a bit - 2'b01: compare only ones - 2'b10: compare ony zeros.|
    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |26   |ARG  |Receives the data starting from LSB.                                                                      |
    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |27   |ARG  |Receives the command using QuadSPI.                                                                       |
    +-----+-----+----------------------------------------------------------------------------------------------------------+
    |31:28|0xB  |Select the SPIM command to be processed. Here “RX_CHECK”.                                                 |
    +-----+-----+----------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_FULL_DUPL:

SPI_CMD_FULL_DUPL
"""""""""""""""""

.. table:: 

    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                                                                                                                                                                                                                                         Description                                                                                                                                                                                                                                                                         |
    +=====+=====+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |ARG  |Number of words to receive and send (Max 64K). The number of bits received and sent is this value multiplied by the word size. The value written here is num words – 1.                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |20:16|ARG  |Size in bits of each word sent. The value written here is num bits – 1.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22:21|ARG  |Number of words transferred from L2 at each transfer -2’b00: 1 word per transfer -2’b01: 2 words per transfer -2’b10: 4 words per transfer. The UDMA always packs 4 bytes into each transfer. This field allows setting how many words (with size specified in WORD_SIZE) should be packed together in one transfer. WORD_SIZE is always rounded-up to the closest supported word-size (8, 16 or 32). If the rounded word-size multiplied by this field is not equal to 32bits, the UDMA will either insert dummy bytes or drop bytes in the 32bits transfer.|
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |26   |ARG  |Sends and receives the data starting from LSB.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:28|0x7  |Select the SPIM command to be processed. Here “FULL_DUPLEX”                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
    +-----+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _SPI micro-code_SPI_CMD_SETUP_AG:

SPI_CMD_SETUP_AG
""""""""""""""""

.. table:: 

    +-----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|Value|                                                                               Description                                                                                |
    +=====+=====+==========================================================================================================================================================================+
    |1:0  |ARG  |Register of the address generator to set                                                                                                                                  |
    +-----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |8    |0x0  |Selects which address generator to set 0: Chose the Stream ID of the RX channels(set by REG_DEST[22:16]) 1: Chose the Stream ID of the TX channels(set by REG_DEST[14:8]).|
    +-----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:28|0xD  |Select the SPIM command to be processed. Here “SETUP_AG”.                                                                                                                 |
    +-----+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
