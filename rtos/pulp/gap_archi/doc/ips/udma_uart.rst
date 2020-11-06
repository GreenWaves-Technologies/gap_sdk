Input file: docs/doc.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------+------+-----+-----------------------------------------+
    |              Name               |Offset|Width|               Description               |
    +=================================+======+=====+=========================================+
    |:ref:`RX_DEST<udma_uart_RX_DEST>`|     0|   32|Stream ID for the uDMA channel.          |
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`TX_DEST<udma_uart_TX_DEST>`|     4|   32|Stream ID for the uDMA channel.          |
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`STATUS<udma_uart_STATUS>`  |    32|   32|status register                          |
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`SETUP<udma_uart_SETUP>`    |    36|   32|configuration register                   |
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`ERROR<udma_uart_ERROR>`    |    40|   32|error register. Reading it will clear it.|
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`IRQ_EN<udma_uart_IRQ_EN>`  |    44|   32|enable/disable events                    |
    +---------------------------------+------+-----+-----------------------------------------+
    |:ref:`SETUP_2<udma_uart_SETUP_2>`|    56|   32|configuration register 2                 |
    +---------------------------------+------+-----+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel.
                #define UDMA_UART_RX_DEST_OFFSET                 0x0
        
                // Stream ID for the uDMA channel.
                #define UDMA_UART_TX_DEST_OFFSET                 0x4
        
                // status register
                #define UDMA_UART_STATUS_OFFSET                  0x20
        
                // configuration register
                #define UDMA_UART_SETUP_OFFSET                   0x24
        
                // error register. Reading it will clear it.
                #define UDMA_UART_ERROR_OFFSET                   0x28
        
                // enable/disable events
                #define UDMA_UART_IRQ_EN_OFFSET                  0x2c
        
                // configuration register 2
                #define UDMA_UART_SETUP_2_OFFSET                 0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_rx_dest_get(uint32_t base);
        static inline void udma_uart_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_tx_dest_get(uint32_t base);
        static inline void udma_uart_tx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_status_get(uint32_t base);
        static inline void udma_uart_status_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_setup_get(uint32_t base);
        static inline void udma_uart_setup_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_error_get(uint32_t base);
        static inline void udma_uart_error_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_irq_en_get(uint32_t base);
        static inline void udma_uart_irq_en_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_uart_setup_2_get(uint32_t base);
        static inline void udma_uart_setup_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_UART_RX_DEST_RX_DEST_BIT                                0
        #define UDMA_UART_RX_DEST_RX_DEST_WIDTH                              7
        #define UDMA_UART_RX_DEST_RX_DEST_MASK                               0x7f
        #define UDMA_UART_RX_DEST_RX_DEST_RESET                              0x7f
        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_UART_TX_DEST_TX_DEST_BIT                                0
        #define UDMA_UART_TX_DEST_TX_DEST_WIDTH                              7
        #define UDMA_UART_TX_DEST_TX_DEST_MASK                               0x7f
        #define UDMA_UART_TX_DEST_TX_DEST_RESET                              0x7f
        
        // transmitter is sending a frame (access: r)
        #define UDMA_UART_STATUS_TX_BUSY_BIT                                 0
        #define UDMA_UART_STATUS_TX_BUSY_WIDTH                               1
        #define UDMA_UART_STATUS_TX_BUSY_MASK                                0x1
        #define UDMA_UART_STATUS_TX_BUSY_RESET                               0x0
        
        // receiver is receiving a frame (access: r)
        #define UDMA_UART_STATUS_RX_BUSY_BIT                                 1
        #define UDMA_UART_STATUS_RX_BUSY_WIDTH                               1
        #define UDMA_UART_STATUS_RX_BUSY_MASK                                0x2
        #define UDMA_UART_STATUS_RX_BUSY_RESET                               0x0
        
        // Enable parity bit for tx and rx blocks (access: RW)
        #define UDMA_UART_SETUP_PARITY_ENA_BIT                               0
        #define UDMA_UART_SETUP_PARITY_ENA_WIDTH                             1
        #define UDMA_UART_SETUP_PARITY_ENA_MASK                              0x1
        #define UDMA_UART_SETUP_PARITY_ENA_RESET                             0x0
        
        // rx/tw word width (access: rw)
        #define UDMA_UART_SETUP_BIT_LENGTH_BIT                               1
        #define UDMA_UART_SETUP_BIT_LENGTH_WIDTH                             2
        #define UDMA_UART_SETUP_BIT_LENGTH_MASK                              0x6
        #define UDMA_UART_SETUP_BIT_LENGTH_RESET                             0x0
        
        // stop bits count (access: rw)
        #define UDMA_UART_SETUP_STOP_BITS_BIT                                3
        #define UDMA_UART_SETUP_STOP_BITS_WIDTH                              1
        #define UDMA_UART_SETUP_STOP_BITS_MASK                               0x8
        #define UDMA_UART_SETUP_STOP_BITS_RESET                              0x0
        
        // enable transmitter (access: rw)
        #define UDMA_UART_SETUP_TX_ENA_BIT                                   8
        #define UDMA_UART_SETUP_TX_ENA_WIDTH                                 1
        #define UDMA_UART_SETUP_TX_ENA_MASK                                  0x100
        #define UDMA_UART_SETUP_TX_ENA_RESET                                 0x0
        
        // enable receiver (access: rw)
        #define UDMA_UART_SETUP_RX_ENA_BIT                                   9
        #define UDMA_UART_SETUP_RX_ENA_WIDTH                                 1
        #define UDMA_UART_SETUP_RX_ENA_MASK                                  0x200
        #define UDMA_UART_SETUP_RX_ENA_RESET                                 0x0
        
        // Flow control: enable Clear To Send input pin. Transmitter will send next word if ctsn_i == 0. (access: rw)
        #define UDMA_UART_SETUP_CTS_EN_BIT                                   10
        #define UDMA_UART_SETUP_CTS_EN_WIDTH                                 1
        #define UDMA_UART_SETUP_CTS_EN_MASK                                  0x400
        #define UDMA_UART_SETUP_CTS_EN_RESET                                 0x0
        
        // Flow control: enable Ready To Send output pin. ctsn_i == 0 if the receiver can receive next word. (access: rw)
        #define UDMA_UART_SETUP_RTS_EN_BIT                                   11
        #define UDMA_UART_SETUP_RTS_EN_WIDTH                                 1
        #define UDMA_UART_SETUP_RTS_EN_MASK                                  0x800
        #define UDMA_UART_SETUP_RTS_EN_RESET                                 0x0
        
        // Enable synchronous master mode. (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_EN_BIT                                12
        #define UDMA_UART_SETUP_TX_CLK_EN_WIDTH                              1
        #define UDMA_UART_SETUP_TX_CLK_EN_MASK                               0x1000
        #define UDMA_UART_SETUP_TX_CLK_EN_RESET                              0x0
        
        // Configure tx clock polarity (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_POL_BIT                               13
        #define UDMA_UART_SETUP_TX_CLK_POL_WIDTH                             1
        #define UDMA_UART_SETUP_TX_CLK_POL_MASK                              0x2000
        #define UDMA_UART_SETUP_TX_CLK_POL_RESET                             0x0
        
        // Configure tx clock phase (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_PHA_BIT                               14
        #define UDMA_UART_SETUP_TX_CLK_PHA_WIDTH                             1
        #define UDMA_UART_SETUP_TX_CLK_PHA_MASK                              0x4000
        #define UDMA_UART_SETUP_TX_CLK_PHA_RESET                             0x0
        
        // baudrate divider according to pclk. baudrate = pclk / (CLKDIV + 1) (access: rw)
        #define UDMA_UART_SETUP_CLKDIV_BIT                                   16
        #define UDMA_UART_SETUP_CLKDIV_WIDTH                                 16
        #define UDMA_UART_SETUP_CLKDIV_MASK                                  0xffff0000
        #define UDMA_UART_SETUP_CLKDIV_RESET                                 0x0
        
        // rx overflow flag (access: r)
        #define UDMA_UART_ERROR_ERR_OVERFLOW_BIT                             0
        #define UDMA_UART_ERROR_ERR_OVERFLOW_WIDTH                           1
        #define UDMA_UART_ERROR_ERR_OVERFLOW_MASK                            0x1
        #define UDMA_UART_ERROR_ERR_OVERFLOW_RESET                           0x0
        
        // rx parity flag (access: r)
        #define UDMA_UART_ERROR_ERR_PARITY_BIT                               1
        #define UDMA_UART_ERROR_ERR_PARITY_WIDTH                             1
        #define UDMA_UART_ERROR_ERR_PARITY_MASK                              0x2
        #define UDMA_UART_ERROR_ERR_PARITY_RESET                             0x0
        
        // emit event if rx received a word. (access: rw)
        #define UDMA_UART_IRQ_EN_RX_IRQ_BIT                                  0
        #define UDMA_UART_IRQ_EN_RX_IRQ_WIDTH                                1
        #define UDMA_UART_IRQ_EN_RX_IRQ_MASK                                 0x1
        #define UDMA_UART_IRQ_EN_RX_IRQ_RESET                                0x0
        
        // emit event on an error (see ERROR register) (access: rw)
        #define UDMA_UART_IRQ_EN_ERR_IRQ_BIT                                 1
        #define UDMA_UART_IRQ_EN_ERR_IRQ_WIDTH                               1
        #define UDMA_UART_IRQ_EN_ERR_IRQ_MASK                                0x2
        #define UDMA_UART_IRQ_EN_ERR_IRQ_RESET                               0x0
        
        // emit event after a byte is sent, after stop symbol is transmitted (access: rw)
        #define UDMA_UART_IRQ_EN_TX_IRQ_BIT                                  1
        #define UDMA_UART_IRQ_EN_TX_IRQ_WIDTH                                1
        #define UDMA_UART_IRQ_EN_TX_IRQ_MASK                                 0x2
        #define UDMA_UART_IRQ_EN_TX_IRQ_RESET                                0x0
        
        // deassert rtsn_o when rx 0 &gt;= fifo stored count &gt;= RTS_HIGH_LIMIT &lt;= 8 (access: RW)
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_BIT                         0
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_WIDTH                       4
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_MASK                        0xf
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_RESET                       0x4

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_RX_DEST_RX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST(val)                     ((val) << 0)
        
        #define UDMA_UART_TX_DEST_TX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST(val)                     ((val) << 0)
        
        #define UDMA_UART_STATUS_TX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_STATUS_TX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_STATUS_TX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_STATUS_TX_BUSY(val)                      ((val) << 0)
        
        #define UDMA_UART_STATUS_RX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_STATUS_RX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_STATUS_RX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_STATUS_RX_BUSY(val)                      ((val) << 1)
        
        #define UDMA_UART_SETUP_PARITY_ENA_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA(val)                    ((val) << 0)
        
        #define UDMA_UART_SETUP_BIT_LENGTH_GET(value)              (GAP_BEXTRACTU((value),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH_GETS(value)             (GAP_BEXTRACT((value),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH_SET(value,field)        (GAP_BINSERT((value),(field),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH(val)                    ((val) << 1)
        
        #define UDMA_UART_SETUP_STOP_BITS_GET(value)               (GAP_BEXTRACTU((value),1,3))
        #define UDMA_UART_SETUP_STOP_BITS_GETS(value)              (GAP_BEXTRACT((value),1,3))
        #define UDMA_UART_SETUP_STOP_BITS_SET(value,field)         (GAP_BINSERT((value),(field),1,3))
        #define UDMA_UART_SETUP_STOP_BITS(val)                     ((val) << 3)
        
        #define UDMA_UART_SETUP_TX_ENA_GET(value)                  (GAP_BEXTRACTU((value),1,8))
        #define UDMA_UART_SETUP_TX_ENA_GETS(value)                 (GAP_BEXTRACT((value),1,8))
        #define UDMA_UART_SETUP_TX_ENA_SET(value,field)            (GAP_BINSERT((value),(field),1,8))
        #define UDMA_UART_SETUP_TX_ENA(val)                        ((val) << 8)
        
        #define UDMA_UART_SETUP_RX_ENA_GET(value)                  (GAP_BEXTRACTU((value),1,9))
        #define UDMA_UART_SETUP_RX_ENA_GETS(value)                 (GAP_BEXTRACT((value),1,9))
        #define UDMA_UART_SETUP_RX_ENA_SET(value,field)            (GAP_BINSERT((value),(field),1,9))
        #define UDMA_UART_SETUP_RX_ENA(val)                        ((val) << 9)
        
        #define UDMA_UART_SETUP_CTS_EN_GET(value)                  (GAP_BEXTRACTU((value),1,10))
        #define UDMA_UART_SETUP_CTS_EN_GETS(value)                 (GAP_BEXTRACT((value),1,10))
        #define UDMA_UART_SETUP_CTS_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,10))
        #define UDMA_UART_SETUP_CTS_EN(val)                        ((val) << 10)
        
        #define UDMA_UART_SETUP_RTS_EN_GET(value)                  (GAP_BEXTRACTU((value),1,11))
        #define UDMA_UART_SETUP_RTS_EN_GETS(value)                 (GAP_BEXTRACT((value),1,11))
        #define UDMA_UART_SETUP_RTS_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,11))
        #define UDMA_UART_SETUP_RTS_EN(val)                        ((val) << 11)
        
        #define UDMA_UART_SETUP_TX_CLK_EN_GET(value)               (GAP_BEXTRACTU((value),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN_GETS(value)              (GAP_BEXTRACT((value),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN(val)                     ((val) << 12)
        
        #define UDMA_UART_SETUP_TX_CLK_POL_GET(value)              (GAP_BEXTRACTU((value),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL_GETS(value)             (GAP_BEXTRACT((value),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL_SET(value,field)        (GAP_BINSERT((value),(field),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL(val)                    ((val) << 13)
        
        #define UDMA_UART_SETUP_TX_CLK_PHA_GET(value)              (GAP_BEXTRACTU((value),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA_GETS(value)             (GAP_BEXTRACT((value),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA_SET(value,field)        (GAP_BINSERT((value),(field),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA(val)                    ((val) << 14)
        
        #define UDMA_UART_SETUP_CLKDIV_GET(value)                  (GAP_BEXTRACTU((value),16,16))
        #define UDMA_UART_SETUP_CLKDIV_GETS(value)                 (GAP_BEXTRACT((value),16,16))
        #define UDMA_UART_SETUP_CLKDIV_SET(value,field)            (GAP_BINSERT((value),(field),16,16))
        #define UDMA_UART_SETUP_CLKDIV(val)                        ((val) << 16)
        
        #define UDMA_UART_ERROR_ERR_OVERFLOW_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW(val)                  ((val) << 0)
        
        #define UDMA_UART_ERROR_ERR_PARITY_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY(val)                    ((val) << 1)
        
        #define UDMA_UART_IRQ_EN_RX_IRQ_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ(val)                       ((val) << 0)
        
        #define UDMA_UART_IRQ_EN_ERR_IRQ_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ(val)                      ((val) << 1)
        
        #define UDMA_UART_IRQ_EN_TX_IRQ_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ(val)                       ((val) << 1)
        
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_GET(value)        (GAP_BEXTRACTU((value),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_GETS(value)       (GAP_BEXTRACT((value),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_SET(value,field)  (GAP_BINSERT((value),(field),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT(val)              ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_UART_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t rx_dest;  // Stream ID for the uDMA channel.
            volatile uint32_t tx_dest;  // Stream ID for the uDMA channel.
            volatile uint32_t reserved_0[6];  // Reserved/Not used.
            volatile uint32_t status;  // status register
            volatile uint32_t setup;  // configuration register
            volatile uint32_t error;  // error register. Reading it will clear it.
            volatile uint32_t irq_en;  // enable/disable events
            volatile uint32_t reserved_1[2];  // Reserved/Not used.
            volatile uint32_t setup_2;  // configuration register 2
        } __attribute__((packed)) udma_uart_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_rx_dest_t;
        
        typedef union {
          struct {
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_tx_dest_t;
        
        typedef union {
          struct {
            unsigned int tx_busy         :1 ; // transmitter is sending a frame
            unsigned int rx_busy         :1 ; // receiver is receiving a frame
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_status_t;
        
        typedef union {
          struct {
            unsigned int parity_ena      :1 ; // Enable parity bit for tx and rx blocks
            unsigned int bit_length      :2 ; // rx/tw word width
            unsigned int stop_bits       :1 ; // stop bits count
            unsigned int padding0:4 ;
            unsigned int tx_ena          :1 ; // enable transmitter
            unsigned int rx_ena          :1 ; // enable receiver
            unsigned int cts_en          :1 ; // Flow control: enable Clear To Send input pin. Transmitter will send next word if ctsn_i == 0.
            unsigned int rts_en          :1 ; // Flow control: enable Ready To Send output pin. ctsn_i == 0 if the receiver can receive next word.
            unsigned int tx_clk_en       :1 ; // Enable synchronous master mode.
            unsigned int tx_clk_pol      :1 ; // Configure tx clock polarity
            unsigned int tx_clk_pha      :1 ; // Configure tx clock phase
            unsigned int padding1:1 ;
            unsigned int clkdiv          :16; // baudrate divider according to pclk. baudrate = pclk / (CLKDIV + 1)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_setup_t;
        
        typedef union {
          struct {
            unsigned int err_overflow    :1 ; // rx overflow flag
            unsigned int err_parity      :1 ; // rx parity flag
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_error_t;
        
        typedef union {
          struct {
            unsigned int rx_irq          :1 ; // emit event if rx received a word.
            unsigned int err_irq         :1 ; // emit event on an error (see ERROR register)
            unsigned int tx_irq          :1 ; // emit event after a byte is sent, after stop symbol is transmitted
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_irq_en_t;
        
        typedef union {
          struct {
            unsigned int rts_high_limit  :4 ; // deassert rtsn_o when rx 0 &gt;= fifo stored count &gt;= RTS_HIGH_LIMIT &lt;= 8
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_setup_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_uart : public vp::regmap
        {
        public:
            vp_udma_uart_rx_dest rx_dest;
            vp_udma_uart_tx_dest tx_dest;
            vp_udma_uart_status status;
            vp_udma_uart_setup setup;
            vp_udma_uart_error error;
            vp_udma_uart_irq_en irq_en;
            vp_udma_uart_setup_2 setup_2;
        };

|

.. _udma_uart_RX_DEST:

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
                #define UDMA_UART_RX_DEST_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_rx_dest_get(uint32_t base);
        static inline void udma_uart_rx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_UART_RX_DEST_RX_DEST_BIT                                0
        #define UDMA_UART_RX_DEST_RX_DEST_WIDTH                              7
        #define UDMA_UART_RX_DEST_RX_DEST_MASK                               0x7f
        #define UDMA_UART_RX_DEST_RX_DEST_RESET                              0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_RX_DEST_RX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_UART_RX_DEST_RX_DEST(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the RX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_rx_dest : public vp::reg_32
        {
        public:
            inline void rx_dest_set(uint32_t value);
            inline uint32_t rx_dest_get();
        };

|

.. _udma_uart_TX_DEST:

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
                #define UDMA_UART_TX_DEST_OFFSET                 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_tx_dest_get(uint32_t base);
        static inline void udma_uart_tx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled). (access: RW)
        #define UDMA_UART_TX_DEST_TX_DEST_BIT                                0
        #define UDMA_UART_TX_DEST_TX_DEST_WIDTH                              7
        #define UDMA_UART_TX_DEST_TX_DEST_MASK                               0x7f
        #define UDMA_UART_TX_DEST_TX_DEST_RESET                              0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_TX_DEST_TX_DEST_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_UART_TX_DEST_TX_DEST(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tx_dest         :7 ; // Stream ID for the TX uDMA channel. Default is 0x7F(channel disabled).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_tx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_tx_dest : public vp::reg_32
        {
        public:
            inline void tx_dest_set(uint32_t value);
            inline uint32_t tx_dest_get();
        };

|

.. _udma_uart_STATUS:

STATUS
""""""

status register

.. table:: 

    +-----+---+-------+------------------------------+
    |Bit #|R/W| Name  |         Description          |
    +=====+===+=======+==============================+
    |    0|r  |TX_BUSY|transmitter is sending a frame|
    +-----+---+-------+------------------------------+
    |    1|r  |RX_BUSY|receiver is receiving a frame |
    +-----+---+-------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // status register
                #define UDMA_UART_STATUS_OFFSET                  0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_status_get(uint32_t base);
        static inline void udma_uart_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // transmitter is sending a frame (access: r)
        #define UDMA_UART_STATUS_TX_BUSY_BIT                                 0
        #define UDMA_UART_STATUS_TX_BUSY_WIDTH                               1
        #define UDMA_UART_STATUS_TX_BUSY_MASK                                0x1
        #define UDMA_UART_STATUS_TX_BUSY_RESET                               0x0
        
        // receiver is receiving a frame (access: r)
        #define UDMA_UART_STATUS_RX_BUSY_BIT                                 1
        #define UDMA_UART_STATUS_RX_BUSY_WIDTH                               1
        #define UDMA_UART_STATUS_RX_BUSY_MASK                                0x2
        #define UDMA_UART_STATUS_RX_BUSY_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_STATUS_TX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_STATUS_TX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_STATUS_TX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_STATUS_TX_BUSY(val)                      ((val) << 0)
        
        #define UDMA_UART_STATUS_RX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_STATUS_RX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_STATUS_RX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_STATUS_RX_BUSY(val)                      ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tx_busy         :1 ; // transmitter is sending a frame
            unsigned int rx_busy         :1 ; // receiver is receiving a frame
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_status : public vp::reg_32
        {
        public:
            inline void tx_busy_set(uint32_t value);
            inline uint32_t tx_busy_get();
            inline void rx_busy_set(uint32_t value);
            inline uint32_t rx_busy_get();
        };

|

.. _udma_uart_SETUP:

SETUP
"""""

configuration register

.. table:: 

    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                           Description                                           |
    +=====+===+==========+=================================================================================================+
    |    0|RW |PARITY_ENA|Enable parity bit for tx and rx blocks                                                           |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |2:1  |rw |BIT_LENGTH|rx/tw word width                                                                                 |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |    3|rw |STOP_BITS |stop bits count                                                                                  |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |    8|rw |TX_ENA    |enable transmitter                                                                               |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |    9|rw |RX_ENA    |enable receiver                                                                                  |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |   10|rw |CTS_EN    |Flow control: enable Clear To Send input pin. Transmitter will send next word if ctsn_i == 0.    |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |   11|rw |RTS_EN    |Flow control: enable Ready To Send output pin. ctsn_i == 0 if the receiver can receive next word.|
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |   12|rw |TX_CLK_EN |Enable synchronous master mode.                                                                  |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |   13|rw |TX_CLK_POL|Configure tx clock polarity                                                                      |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |   14|rw |TX_CLK_PHA|Configure tx clock phase                                                                         |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+
    |31:16|rw |CLKDIV    |baudrate divider according to pclk. baudrate = pclk / (CLKDIV + 1)                               |
    +-----+---+----------+-------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // configuration register
                #define UDMA_UART_SETUP_OFFSET                   0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_setup_get(uint32_t base);
        static inline void udma_uart_setup_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable parity bit for tx and rx blocks (access: RW)
        #define UDMA_UART_SETUP_PARITY_ENA_BIT                               0
        #define UDMA_UART_SETUP_PARITY_ENA_WIDTH                             1
        #define UDMA_UART_SETUP_PARITY_ENA_MASK                              0x1
        #define UDMA_UART_SETUP_PARITY_ENA_RESET                             0x0
        
        // rx/tw word width (access: rw)
        #define UDMA_UART_SETUP_BIT_LENGTH_BIT                               1
        #define UDMA_UART_SETUP_BIT_LENGTH_WIDTH                             2
        #define UDMA_UART_SETUP_BIT_LENGTH_MASK                              0x6
        #define UDMA_UART_SETUP_BIT_LENGTH_RESET                             0x0
        
        // stop bits count (access: rw)
        #define UDMA_UART_SETUP_STOP_BITS_BIT                                3
        #define UDMA_UART_SETUP_STOP_BITS_WIDTH                              1
        #define UDMA_UART_SETUP_STOP_BITS_MASK                               0x8
        #define UDMA_UART_SETUP_STOP_BITS_RESET                              0x0
        
        // enable transmitter (access: rw)
        #define UDMA_UART_SETUP_TX_ENA_BIT                                   8
        #define UDMA_UART_SETUP_TX_ENA_WIDTH                                 1
        #define UDMA_UART_SETUP_TX_ENA_MASK                                  0x100
        #define UDMA_UART_SETUP_TX_ENA_RESET                                 0x0
        
        // enable receiver (access: rw)
        #define UDMA_UART_SETUP_RX_ENA_BIT                                   9
        #define UDMA_UART_SETUP_RX_ENA_WIDTH                                 1
        #define UDMA_UART_SETUP_RX_ENA_MASK                                  0x200
        #define UDMA_UART_SETUP_RX_ENA_RESET                                 0x0
        
        // Flow control: enable Clear To Send input pin. Transmitter will send next word if ctsn_i == 0. (access: rw)
        #define UDMA_UART_SETUP_CTS_EN_BIT                                   10
        #define UDMA_UART_SETUP_CTS_EN_WIDTH                                 1
        #define UDMA_UART_SETUP_CTS_EN_MASK                                  0x400
        #define UDMA_UART_SETUP_CTS_EN_RESET                                 0x0
        
        // Flow control: enable Ready To Send output pin. ctsn_i == 0 if the receiver can receive next word. (access: rw)
        #define UDMA_UART_SETUP_RTS_EN_BIT                                   11
        #define UDMA_UART_SETUP_RTS_EN_WIDTH                                 1
        #define UDMA_UART_SETUP_RTS_EN_MASK                                  0x800
        #define UDMA_UART_SETUP_RTS_EN_RESET                                 0x0
        
        // Enable synchronous master mode. (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_EN_BIT                                12
        #define UDMA_UART_SETUP_TX_CLK_EN_WIDTH                              1
        #define UDMA_UART_SETUP_TX_CLK_EN_MASK                               0x1000
        #define UDMA_UART_SETUP_TX_CLK_EN_RESET                              0x0
        
        // Configure tx clock polarity (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_POL_BIT                               13
        #define UDMA_UART_SETUP_TX_CLK_POL_WIDTH                             1
        #define UDMA_UART_SETUP_TX_CLK_POL_MASK                              0x2000
        #define UDMA_UART_SETUP_TX_CLK_POL_RESET                             0x0
        
        // Configure tx clock phase (access: rw)
        #define UDMA_UART_SETUP_TX_CLK_PHA_BIT                               14
        #define UDMA_UART_SETUP_TX_CLK_PHA_WIDTH                             1
        #define UDMA_UART_SETUP_TX_CLK_PHA_MASK                              0x4000
        #define UDMA_UART_SETUP_TX_CLK_PHA_RESET                             0x0
        
        // baudrate divider according to pclk. baudrate = pclk / (CLKDIV + 1) (access: rw)
        #define UDMA_UART_SETUP_CLKDIV_BIT                                   16
        #define UDMA_UART_SETUP_CLKDIV_WIDTH                                 16
        #define UDMA_UART_SETUP_CLKDIV_MASK                                  0xffff0000
        #define UDMA_UART_SETUP_CLKDIV_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_SETUP_PARITY_ENA_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_SETUP_PARITY_ENA(val)                    ((val) << 0)
        
        #define UDMA_UART_SETUP_BIT_LENGTH_GET(value)              (GAP_BEXTRACTU((value),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH_GETS(value)             (GAP_BEXTRACT((value),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH_SET(value,field)        (GAP_BINSERT((value),(field),2,1))
        #define UDMA_UART_SETUP_BIT_LENGTH(val)                    ((val) << 1)
        
        #define UDMA_UART_SETUP_STOP_BITS_GET(value)               (GAP_BEXTRACTU((value),1,3))
        #define UDMA_UART_SETUP_STOP_BITS_GETS(value)              (GAP_BEXTRACT((value),1,3))
        #define UDMA_UART_SETUP_STOP_BITS_SET(value,field)         (GAP_BINSERT((value),(field),1,3))
        #define UDMA_UART_SETUP_STOP_BITS(val)                     ((val) << 3)
        
        #define UDMA_UART_SETUP_TX_ENA_GET(value)                  (GAP_BEXTRACTU((value),1,8))
        #define UDMA_UART_SETUP_TX_ENA_GETS(value)                 (GAP_BEXTRACT((value),1,8))
        #define UDMA_UART_SETUP_TX_ENA_SET(value,field)            (GAP_BINSERT((value),(field),1,8))
        #define UDMA_UART_SETUP_TX_ENA(val)                        ((val) << 8)
        
        #define UDMA_UART_SETUP_RX_ENA_GET(value)                  (GAP_BEXTRACTU((value),1,9))
        #define UDMA_UART_SETUP_RX_ENA_GETS(value)                 (GAP_BEXTRACT((value),1,9))
        #define UDMA_UART_SETUP_RX_ENA_SET(value,field)            (GAP_BINSERT((value),(field),1,9))
        #define UDMA_UART_SETUP_RX_ENA(val)                        ((val) << 9)
        
        #define UDMA_UART_SETUP_CTS_EN_GET(value)                  (GAP_BEXTRACTU((value),1,10))
        #define UDMA_UART_SETUP_CTS_EN_GETS(value)                 (GAP_BEXTRACT((value),1,10))
        #define UDMA_UART_SETUP_CTS_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,10))
        #define UDMA_UART_SETUP_CTS_EN(val)                        ((val) << 10)
        
        #define UDMA_UART_SETUP_RTS_EN_GET(value)                  (GAP_BEXTRACTU((value),1,11))
        #define UDMA_UART_SETUP_RTS_EN_GETS(value)                 (GAP_BEXTRACT((value),1,11))
        #define UDMA_UART_SETUP_RTS_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,11))
        #define UDMA_UART_SETUP_RTS_EN(val)                        ((val) << 11)
        
        #define UDMA_UART_SETUP_TX_CLK_EN_GET(value)               (GAP_BEXTRACTU((value),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN_GETS(value)              (GAP_BEXTRACT((value),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,12))
        #define UDMA_UART_SETUP_TX_CLK_EN(val)                     ((val) << 12)
        
        #define UDMA_UART_SETUP_TX_CLK_POL_GET(value)              (GAP_BEXTRACTU((value),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL_GETS(value)             (GAP_BEXTRACT((value),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL_SET(value,field)        (GAP_BINSERT((value),(field),1,13))
        #define UDMA_UART_SETUP_TX_CLK_POL(val)                    ((val) << 13)
        
        #define UDMA_UART_SETUP_TX_CLK_PHA_GET(value)              (GAP_BEXTRACTU((value),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA_GETS(value)             (GAP_BEXTRACT((value),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA_SET(value,field)        (GAP_BINSERT((value),(field),1,14))
        #define UDMA_UART_SETUP_TX_CLK_PHA(val)                    ((val) << 14)
        
        #define UDMA_UART_SETUP_CLKDIV_GET(value)                  (GAP_BEXTRACTU((value),16,16))
        #define UDMA_UART_SETUP_CLKDIV_GETS(value)                 (GAP_BEXTRACT((value),16,16))
        #define UDMA_UART_SETUP_CLKDIV_SET(value,field)            (GAP_BINSERT((value),(field),16,16))
        #define UDMA_UART_SETUP_CLKDIV(val)                        ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int parity_ena      :1 ; // Enable parity bit for tx and rx blocks
            unsigned int bit_length      :2 ; // rx/tw word width
            unsigned int stop_bits       :1 ; // stop bits count
            unsigned int padding0:4 ;
            unsigned int tx_ena          :1 ; // enable transmitter
            unsigned int rx_ena          :1 ; // enable receiver
            unsigned int cts_en          :1 ; // Flow control: enable Clear To Send input pin. Transmitter will send next word if ctsn_i == 0.
            unsigned int rts_en          :1 ; // Flow control: enable Ready To Send output pin. ctsn_i == 0 if the receiver can receive next word.
            unsigned int tx_clk_en       :1 ; // Enable synchronous master mode.
            unsigned int tx_clk_pol      :1 ; // Configure tx clock polarity
            unsigned int tx_clk_pha      :1 ; // Configure tx clock phase
            unsigned int padding1:1 ;
            unsigned int clkdiv          :16; // baudrate divider according to pclk. baudrate = pclk / (CLKDIV + 1)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_setup_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_setup : public vp::reg_32
        {
        public:
            inline void parity_ena_set(uint32_t value);
            inline uint32_t parity_ena_get();
            inline void bit_length_set(uint32_t value);
            inline uint32_t bit_length_get();
            inline void stop_bits_set(uint32_t value);
            inline uint32_t stop_bits_get();
            inline void tx_ena_set(uint32_t value);
            inline uint32_t tx_ena_get();
            inline void rx_ena_set(uint32_t value);
            inline uint32_t rx_ena_get();
            inline void cts_en_set(uint32_t value);
            inline uint32_t cts_en_get();
            inline void rts_en_set(uint32_t value);
            inline uint32_t rts_en_get();
            inline void tx_clk_en_set(uint32_t value);
            inline uint32_t tx_clk_en_get();
            inline void tx_clk_pol_set(uint32_t value);
            inline uint32_t tx_clk_pol_get();
            inline void tx_clk_pha_set(uint32_t value);
            inline uint32_t tx_clk_pha_get();
            inline void clkdiv_set(uint32_t value);
            inline uint32_t clkdiv_get();
        };

|

.. _udma_uart_ERROR:

ERROR
"""""

error register. Reading it will clear it.

.. table:: 

    +-----+---+------------+----------------+
    |Bit #|R/W|    Name    |  Description   |
    +=====+===+============+================+
    |    0|r  |ERR_OVERFLOW|rx overflow flag|
    +-----+---+------------+----------------+
    |    1|r  |ERR_PARITY  |rx parity flag  |
    +-----+---+------------+----------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // error register. Reading it will clear it.
                #define UDMA_UART_ERROR_OFFSET                   0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_error_get(uint32_t base);
        static inline void udma_uart_error_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // rx overflow flag (access: r)
        #define UDMA_UART_ERROR_ERR_OVERFLOW_BIT                             0
        #define UDMA_UART_ERROR_ERR_OVERFLOW_WIDTH                           1
        #define UDMA_UART_ERROR_ERR_OVERFLOW_MASK                            0x1
        #define UDMA_UART_ERROR_ERR_OVERFLOW_RESET                           0x0
        
        // rx parity flag (access: r)
        #define UDMA_UART_ERROR_ERR_PARITY_BIT                               1
        #define UDMA_UART_ERROR_ERR_PARITY_WIDTH                             1
        #define UDMA_UART_ERROR_ERR_PARITY_MASK                              0x2
        #define UDMA_UART_ERROR_ERR_PARITY_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_ERROR_ERR_OVERFLOW_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_ERROR_ERR_OVERFLOW(val)                  ((val) << 0)
        
        #define UDMA_UART_ERROR_ERR_PARITY_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_ERROR_ERR_PARITY(val)                    ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int err_overflow    :1 ; // rx overflow flag
            unsigned int err_parity      :1 ; // rx parity flag
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_error_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_error : public vp::reg_32
        {
        public:
            inline void err_overflow_set(uint32_t value);
            inline uint32_t err_overflow_get();
            inline void err_parity_set(uint32_t value);
            inline uint32_t err_parity_get();
        };

|

.. _udma_uart_IRQ_EN:

IRQ_EN
""""""

enable/disable events

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------+
    |Bit #|R/W| Name  |                           Description                           |
    +=====+===+=======+=================================================================+
    |    0|rw |RX_IRQ |emit event if rx received a word.                                |
    +-----+---+-------+-----------------------------------------------------------------+
    |    1|rw |ERR_IRQ|emit event on an error (see ERROR register)                      |
    +-----+---+-------+-----------------------------------------------------------------+
    |    1|rw |TX_IRQ |emit event after a byte is sent, after stop symbol is transmitted|
    +-----+---+-------+-----------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // enable/disable events
                #define UDMA_UART_IRQ_EN_OFFSET                  0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_irq_en_get(uint32_t base);
        static inline void udma_uart_irq_en_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // emit event if rx received a word. (access: rw)
        #define UDMA_UART_IRQ_EN_RX_IRQ_BIT                                  0
        #define UDMA_UART_IRQ_EN_RX_IRQ_WIDTH                                1
        #define UDMA_UART_IRQ_EN_RX_IRQ_MASK                                 0x1
        #define UDMA_UART_IRQ_EN_RX_IRQ_RESET                                0x0
        
        // emit event on an error (see ERROR register) (access: rw)
        #define UDMA_UART_IRQ_EN_ERR_IRQ_BIT                                 1
        #define UDMA_UART_IRQ_EN_ERR_IRQ_WIDTH                               1
        #define UDMA_UART_IRQ_EN_ERR_IRQ_MASK                                0x2
        #define UDMA_UART_IRQ_EN_ERR_IRQ_RESET                               0x0
        
        // emit event after a byte is sent, after stop symbol is transmitted (access: rw)
        #define UDMA_UART_IRQ_EN_TX_IRQ_BIT                                  1
        #define UDMA_UART_IRQ_EN_TX_IRQ_WIDTH                                1
        #define UDMA_UART_IRQ_EN_TX_IRQ_MASK                                 0x2
        #define UDMA_UART_IRQ_EN_TX_IRQ_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_IRQ_EN_RX_IRQ_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define UDMA_UART_IRQ_EN_RX_IRQ(val)                       ((val) << 0)
        
        #define UDMA_UART_IRQ_EN_ERR_IRQ_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_IRQ_EN_ERR_IRQ(val)                      ((val) << 1)
        
        #define UDMA_UART_IRQ_EN_TX_IRQ_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define UDMA_UART_IRQ_EN_TX_IRQ(val)                       ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_irq          :1 ; // emit event if rx received a word.
            unsigned int err_irq         :1 ; // emit event on an error (see ERROR register)
            unsigned int tx_irq          :1 ; // emit event after a byte is sent, after stop symbol is transmitted
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_irq_en_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_irq_en : public vp::reg_32
        {
        public:
            inline void rx_irq_set(uint32_t value);
            inline uint32_t rx_irq_get();
            inline void err_irq_set(uint32_t value);
            inline uint32_t err_irq_get();
            inline void tx_irq_set(uint32_t value);
            inline uint32_t tx_irq_get();
        };

|

.. _udma_uart_SETUP_2:

SETUP_2
"""""""

configuration register 2

.. table:: 

    +-----+---+--------------+------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                                 Description                                  |
    +=====+===+==============+==============================================================================+
    |3:0  |RW |RTS_HIGH_LIMIT|deassert rtsn_o when rx 0 &gt;= fifo stored count &gt;= RTS_HIGH_LIMIT &lt;= 8|
    +-----+---+--------------+------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // configuration register 2
                #define UDMA_UART_SETUP_2_OFFSET                 0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_uart_setup_2_get(uint32_t base);
        static inline void udma_uart_setup_2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // deassert rtsn_o when rx 0 &gt;= fifo stored count &gt;= RTS_HIGH_LIMIT &lt;= 8 (access: RW)
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_BIT                         0
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_WIDTH                       4
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_MASK                        0xf
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_RESET                       0x4

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_GET(value)        (GAP_BEXTRACTU((value),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_GETS(value)       (GAP_BEXTRACT((value),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT_SET(value,field)  (GAP_BINSERT((value),(field),4,0))
        #define UDMA_UART_SETUP_2_RTS_HIGH_LIMIT(val)              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rts_high_limit  :4 ; // deassert rtsn_o when rx 0 &gt;= fifo stored count &gt;= RTS_HIGH_LIMIT &lt;= 8
          };
          unsigned int raw;
        } __attribute__((packed)) udma_uart_setup_2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_uart_setup_2 : public vp::reg_32
        {
        public:
            inline void rts_high_limit_set(uint32_t value);
            inline uint32_t rts_high_limit_get();
        };

|
