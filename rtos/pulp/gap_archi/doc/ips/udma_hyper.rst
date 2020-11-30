Input file: fe/ips/udma/udma_hyperbus_octospi/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |                        Name                        |Offset|Width|                        Description                        |
    +====================================================+======+=====+===========================================================+
    |:ref:`RX_DEST<udma_hyper_RX_DEST>`                  |     0|   32|Stream ID for the uDMA channel                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TX_DEST<udma_hyper_TX_DEST>`                  |     4|   32|Stream ID for the uDMA channel                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TRANS_MODE<udma_hyper_TRANS_MODE>`            |     8|   32|Configure transaction mode                                 |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TRANS_ADDR<udma_hyper_TRANS_ADDR>`            |    12|   32|Configure each transaction addr                            |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TRANS_SIZE<udma_hyper_TRANS_SIZE>`            |    16|   32|Configure each transaction size                            |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TRANS_CFG<udma_hyper_TRANS_CFG>`              |    20|   32|Start  each transaction rx/tx                              |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_CMD_XIP<udma_hyper_OSPI_CMD_XIP>`        |    24|   32|OSPI XIP command and psram command config                  |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_CFG_XIP<udma_hyper_OSPI_CFG_XIP>`        |    28|   32|OSPI XIP configuration                                     |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`EXT_ADDR<udma_hyper_EXT_ADDR>`                |    32|   32|Memory access address register.                            |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`TIMING_CFG<udma_hyper_TIMING_CFG>`            |    36|   32|HYPERBUS and Octo SPI Memory Timing configuration register.|
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`MBA0<udma_hyper_MBA0>`                        |    40|   32|Device start address register.                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`MBA1<udma_hyper_MBA1>`                        |    44|   32|Device start address register.                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`DEVICE<udma_hyper_DEVICE>`                    |    48|   32|Device type register(RAM or FLASH).                        |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_CMD<udma_hyper_OSPI_CMD>`                |    52|   32|OSPI command and psram command config                      |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_ALTER<udma_hyper_OSPI_ALTER>`            |    56|   32|OSPI alternative 2 bytes                                   |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_CFG<udma_hyper_OSPI_CFG>`                |    60|   32|OSPI configuration                                         |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_CSN<udma_hyper_OSPI_CSN>`                |    64|   32|OSPI chip select configuration                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_JEDEC_RESET<udma_hyper_OSPI_JEDEC_RESET>`|    68|   32|OSPI JEDEC Hardware Reset, user can control sdo0 manually  |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_RAM_OPT<udma_hyper_OSPI_RAM_OPT>`        |    72|   32|OSPI RAM DATA transfer optimisation, only in auto mode     |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_ALTER_XIP<udma_hyper_OSPI_ALTER_XIP>`    |    76|   32|OSPI XIP alternative 2 bytes                               |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`OSPI_REG_XIP<udma_hyper_OSPI_REG_XIP>`        |    80|   32|OSPI XIP other configuration                               |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`LINE_2D<udma_hyper_LINE_2D>`                  |    84|   32|OSPI 2D line.                                              |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`STRIDE_2D<udma_hyper_STRIDE_2D>`              |    88|   32|OSPI 2D stride.                                            |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`BURST_ENABLE<udma_hyper_BURST_ENABLE>`        |    92|   32|OSPI burst mode/2D mode enable.                            |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`IRQ_EN<udma_hyper_IRQ_EN>`                    |    96|   32|OSPI interrupt enable register                             |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`CLK_DIV<udma_hyper_CLK_DIV>`                  |   100|   32|Clock divide.                                              |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`STATUS<udma_hyper_STATUS>`                    |   104|   32|Transfer status for error.                                 |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SDIO_CMD_ARG<udma_hyper_SDIO_CMD_ARG>`        |   108|   32|SDIO command argument.                                     |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SDIO_RSP0<udma_hyper_SDIO_RSP0>`              |   112|   32|SDIO response 0.                                           |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SDIO_RSP1<udma_hyper_SDIO_RSP1>`              |   116|   32|SDIO response 1.                                           |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SDIO_RSP2<udma_hyper_SDIO_RSP2>`              |   120|   32|SDIO response 2.                                           |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SDIO_RSP3<udma_hyper_SDIO_RSP3>`              |   124|   32|SDIO response 3.                                           |
    +----------------------------------------------------+------+-----+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_HYPER_RX_DEST_OFFSET                0x0
        
                // Stream ID for the uDMA channel
                #define UDMA_HYPER_TX_DEST_OFFSET                0x4
        
                // Configure transaction mode
                #define UDMA_HYPER_TRANS_MODE_OFFSET             0x8
        
                // Configure each transaction addr
                #define UDMA_HYPER_TRANS_ADDR_OFFSET             0xc
        
                // Configure each transaction size
                #define UDMA_HYPER_TRANS_SIZE_OFFSET             0x10
        
                // Start  each transaction rx/tx
                #define UDMA_HYPER_TRANS_CFG_OFFSET              0x14
        
                // OSPI XIP command and psram command config
                #define UDMA_HYPER_OSPI_CMD_XIP_OFFSET           0x18
        
                // OSPI XIP configuration
                #define UDMA_HYPER_OSPI_CFG_XIP_OFFSET           0x1c
        
                // Memory access address register.
                #define UDMA_HYPER_EXT_ADDR_OFFSET               0x20
        
                // HYPERBUS and Octo SPI Memory Timing configuration register.
                #define UDMA_HYPER_TIMING_CFG_OFFSET             0x24
        
                // Device start address register.
                #define UDMA_HYPER_MBA0_OFFSET                   0x28
        
                // Device start address register.
                #define UDMA_HYPER_MBA1_OFFSET                   0x2c
        
                // Device type register(RAM or FLASH).
                #define UDMA_HYPER_DEVICE_OFFSET                 0x30
        
                // OSPI command and psram command config
                #define UDMA_HYPER_OSPI_CMD_OFFSET               0x34
        
                // OSPI alternative 2 bytes
                #define UDMA_HYPER_OSPI_ALTER_OFFSET             0x38
        
                // OSPI configuration
                #define UDMA_HYPER_OSPI_CFG_OFFSET               0x3c
        
                // OSPI chip select configuration
                #define UDMA_HYPER_OSPI_CSN_OFFSET               0x40
        
                // OSPI JEDEC Hardware Reset, user can control sdo0 manually
                #define UDMA_HYPER_OSPI_JEDEC_RESET_OFFSET       0x44
        
                // OSPI RAM DATA transfer optimisation, only in auto mode
                #define UDMA_HYPER_OSPI_RAM_OPT_OFFSET           0x48
        
                // OSPI XIP alternative 2 bytes
                #define UDMA_HYPER_OSPI_ALTER_XIP_OFFSET         0x4c
        
                // OSPI XIP other configuration
                #define UDMA_HYPER_OSPI_REG_XIP_OFFSET           0x50
        
                // OSPI 2D line.
                #define UDMA_HYPER_LINE_2D_OFFSET                0x54
        
                // OSPI 2D stride.
                #define UDMA_HYPER_STRIDE_2D_OFFSET              0x58
        
                // OSPI burst mode/2D mode enable.
                #define UDMA_HYPER_BURST_ENABLE_OFFSET           0x5c
        
                // OSPI interrupt enable register
                #define UDMA_HYPER_IRQ_EN_OFFSET                 0x60
        
                // Clock divide.
                #define UDMA_HYPER_CLK_DIV_OFFSET                0x64
        
                // Transfer status for error.
                #define UDMA_HYPER_STATUS_OFFSET                 0x68
        
                // SDIO command argument.
                #define UDMA_HYPER_SDIO_CMD_ARG_OFFSET           0x6c
        
                // SDIO response 0.
                #define UDMA_HYPER_SDIO_RSP0_OFFSET              0x70
        
                // SDIO response 1.
                #define UDMA_HYPER_SDIO_RSP1_OFFSET              0x74
        
                // SDIO response 2.
                #define UDMA_HYPER_SDIO_RSP2_OFFSET              0x78
        
                // SDIO response 3.
                #define UDMA_HYPER_SDIO_RSP3_OFFSET              0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_rx_dest_get(uint32_t base);
        static inline void udma_hyper_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_tx_dest_get(uint32_t base);
        static inline void udma_hyper_tx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_trans_mode_get(uint32_t base);
        static inline void udma_hyper_trans_mode_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_trans_addr_get(uint32_t base);
        static inline void udma_hyper_trans_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_trans_size_get(uint32_t base);
        static inline void udma_hyper_trans_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_trans_cfg_get(uint32_t base);
        static inline void udma_hyper_trans_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_cmd_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_cmd_xip_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_cfg_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_cfg_xip_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ext_addr_get(uint32_t base);
        static inline void udma_hyper_ext_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_timing_cfg_get(uint32_t base);
        static inline void udma_hyper_timing_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_mba0_get(uint32_t base);
        static inline void udma_hyper_mba0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_mba1_get(uint32_t base);
        static inline void udma_hyper_mba1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_device_get(uint32_t base);
        static inline void udma_hyper_device_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_cmd_get(uint32_t base);
        static inline void udma_hyper_ospi_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_alter_get(uint32_t base);
        static inline void udma_hyper_ospi_alter_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_cfg_get(uint32_t base);
        static inline void udma_hyper_ospi_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_csn_get(uint32_t base);
        static inline void udma_hyper_ospi_csn_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_jedec_reset_get(uint32_t base);
        static inline void udma_hyper_ospi_jedec_reset_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_ram_opt_get(uint32_t base);
        static inline void udma_hyper_ospi_ram_opt_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_alter_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_alter_xip_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_ospi_reg_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_reg_xip_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_line_2d_get(uint32_t base);
        static inline void udma_hyper_line_2d_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_stride_2d_get(uint32_t base);
        static inline void udma_hyper_stride_2d_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_burst_enable_get(uint32_t base);
        static inline void udma_hyper_burst_enable_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_irq_en_get(uint32_t base);
        static inline void udma_hyper_irq_en_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_clk_div_get(uint32_t base);
        static inline void udma_hyper_clk_div_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_status_get(uint32_t base);
        static inline void udma_hyper_status_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_sdio_cmd_arg_get(uint32_t base);
        static inline void udma_hyper_sdio_cmd_arg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_sdio_rsp0_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_sdio_rsp1_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_sdio_rsp2_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_hyper_sdio_rsp3_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_RX_DEST_DEST_BIT                                  0
        #define UDMA_HYPER_RX_DEST_DEST_WIDTH                                8
        #define UDMA_HYPER_RX_DEST_DEST_MASK                                 0xff
        #define UDMA_HYPER_RX_DEST_DEST_RESET                                0xff
        
        // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_BIT                           8
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_WIDTH                         8
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_MASK                          0xff00
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_RESET                         0xff
        
        // Stream ID for the TX 2D uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_TX_DEST_DEST_BIT                                  0
        #define UDMA_HYPER_TX_DEST_DEST_WIDTH                                8
        #define UDMA_HYPER_TX_DEST_DEST_MASK                                 0xff
        #define UDMA_HYPER_TX_DEST_DEST_RESET                                0xff
        
        // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_BIT                           8
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_WIDTH                         8
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_MASK                          0xff00
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_RESET                         0xff
        
        // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_BIT                           0
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_WIDTH                         1
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_MASK                          0x1
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_RESET                         0x0
        
        // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_BIT                             1
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_WIDTH                           1
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_MASK                            0x2
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_RESET                           0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_BIT                          2
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_WIDTH                        2
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_MASK                         0xc
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_RESET                        0x0
        
        // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_BIT                          4
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_MASK                         0x10
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_RESET                        0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_BIT                      5
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_WIDTH                    1
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_MASK                     0x20
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_RESET                    0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_BIT                 6
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_WIDTH               1
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_MASK                0x40
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_RESET               0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_BIT                          7
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_MASK                         0x80
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_RESET                        0x0
        
        // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_BIT                      8
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_WIDTH                    1
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_MASK                     0x100
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_RESET                    0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_BIT                  9
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_WIDTH                1
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_MASK                 0x200
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_RESET                0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_BIT             10
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_WIDTH           1
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_MASK            0x400
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_RESET           0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_BIT                          11
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_MASK                         0x800
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_RESET                        0x0
        
        // Halted XIP refill when in XIP, XIP refill will wait SW unlock this bit.  - 1'b0: XIP_RUNNING - 1'b1: XIP_HALTED (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_BIT                         12
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_WIDTH                       1
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_MASK                        0x1000
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_RESET                       0x0
        
        // Transfer addr, only when MODE is in AUTO (access: R/W)
        #define UDMA_HYPER_TRANS_ADDR_ADDR_BIT                               0
        #define UDMA_HYPER_TRANS_ADDR_ADDR_WIDTH                             32
        #define UDMA_HYPER_TRANS_ADDR_ADDR_MASK                              0xffffffff
        #define UDMA_HYPER_TRANS_ADDR_ADDR_RESET                             0x0
        
        // Transfer Size (access: R/W)
        #define UDMA_HYPER_TRANS_SIZE_SIZE_BIT                               0
        #define UDMA_HYPER_TRANS_SIZE_SIZE_WIDTH                             32
        #define UDMA_HYPER_TRANS_SIZE_SIZE_MASK                              0xffffffff
        #define UDMA_HYPER_TRANS_SIZE_SIZE_RESET                             0x0
        
        // Transfer type - 1'b0: TX - 1'b1: RX (access: R/W)
        #define UDMA_HYPER_TRANS_CFG_RXTX_BIT                                0
        #define UDMA_HYPER_TRANS_CFG_RXTX_WIDTH                              1
        #define UDMA_HYPER_TRANS_CFG_RXTX_MASK                               0x1
        #define UDMA_HYPER_TRANS_CFG_RXTX_RESET                              0x0
        
        // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start (access: W)
        #define UDMA_HYPER_TRANS_CFG_VALID_BIT                               1
        #define UDMA_HYPER_TRANS_CFG_VALID_WIDTH                             1
        #define UDMA_HYPER_TRANS_CFG_VALID_MASK                              0x2
        #define UDMA_HYPER_TRANS_CFG_VALID_RESET                             0x0
        
        // 2 Bytes SPI command (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_BIT                              0
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_WIDTH                            16
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_MASK                             0xffff
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_RESET                            0x0
        
        // SDIO CMD response type (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_BIT                20
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_WIDTH              3
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_MASK               0x700000
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_RESET              0x0
        
        // SDIO CMD operation code (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_BIT                      24
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_WIDTH                    6
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_MASK                     0x3f000000
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_RESET                    0x0
        
        // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_BIT                         0
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_WIDTH                       2
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_MASK                        0x3
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_RESET                       0x0
        
        // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_BIT                        4
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_WIDTH                      3
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_MASK                       0x70
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_RESET                      0x0
        
        // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_BIT                             8
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_WIDTH                           2
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_MASK                            0x300
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_RESET                           0x0
        
        // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_BIT                      12
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_WIDTH                    1
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_MASK                     0x1000
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_RESET                    0x0
        
        // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_BIT                     13
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_MASK                    0x2000
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_RESET                   0x0
        
        // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_BIT                     14
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_MASK                    0x4000
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_RESET                   0x0
        
        // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_BIT                     15
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_MASK                    0x8000
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_RESET                   0x0
        
        // Memory access address bitfield. (access: R/W)
        #define UDMA_HYPER_EXT_ADDR_SADDR_BIT                                0
        #define UDMA_HYPER_EXT_ADDR_SADDR_WIDTH                              31
        #define UDMA_HYPER_EXT_ADDR_SADDR_MASK                               0x7fffffff
        #define UDMA_HYPER_EXT_ADDR_SADDR_RESET                              0x0
        
        // Register access flag bitfield. (access: R/W)
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_BIT                           31
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_WIDTH                         1
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_MASK                          0x80000000
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_RESET                         0x0
        
        // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_BIT                           0
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_WIDTH                         5
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_MASK                          0x1f
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_RESET                         0x0
        
        // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_BIT                           5
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_WIDTH                         5
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_MASK                          0x3e0
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_RESET                         0x0
        
        // Some HyperBus devices may require a minimum time between the end of a prior transaction and the start of a new access. This time is referred to as Read-Write-Recovery time (tRWR). The master interface must start driving CS# Low only at a time when the CA1 transfer will complete after tRWR is satisfied. - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_BIT                        10
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_WIDTH                      4
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_MASK                       0x3c00
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_RESET                      0x0
        
        // Delay of RWDS for center aligned read: - 3'b000: 0 logic delay - 3'b001: 1 logic delay - 3'b010: 2 logic delay  - 3'b111: 7 logic delay (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_BIT                         14
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_WIDTH                       3
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_MASK                        0x1c000
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_RESET                       0x0
        
        // Auto check for RWDS high or low for additional latency : - 1'b0: additional latency no autocheck - 1'b1: additional latency autocheck (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_BIT    17
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_WIDTH  1
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK   0x20000
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_RESET  0x0
        
        // Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer : - 14'h0000: 1 CK - 14'h0001: 2 CK - 14'h0011: 3 CK -  - 14'h3FFF: 16383 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_BIT                             18
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_WIDTH                           14
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_MASK                            0xfffc0000
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_RESET                           0x100
        
        // - (access: R/W)
        #define UDMA_HYPER_MBA0_RESERVED_BIT                                 0
        #define UDMA_HYPER_MBA0_RESERVED_WIDTH                               24
        #define UDMA_HYPER_MBA0_RESERVED_MASK                                0xffffff
        #define UDMA_HYPER_MBA0_RESERVED_RESET                               0x0
        
        // Memory Base Address 0 for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CS1 = 0;  else CS0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CS0 = 0;  else CS1 = 0; (access: R/W)
        #define UDMA_HYPER_MBA0_MBA0_BIT                                     24
        #define UDMA_HYPER_MBA0_MBA0_WIDTH                                   7
        #define UDMA_HYPER_MBA0_MBA0_MASK                                    0x7f000000
        #define UDMA_HYPER_MBA0_MBA0_RESET                                   0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_MBA1_RESERVED_BIT                                 0
        #define UDMA_HYPER_MBA1_RESERVED_WIDTH                               24
        #define UDMA_HYPER_MBA1_RESERVED_MASK                                0xffffff
        #define UDMA_HYPER_MBA1_RESERVED_RESET                               0x0
        
        // Memory Base Address  for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CSn1 = 0;  else CSn0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CSn0 = 0;  else CSn1 = 0; (access: R/W)
        #define UDMA_HYPER_MBA1_MBA1_BIT                                     24
        #define UDMA_HYPER_MBA1_MBA1_WIDTH                                   7
        #define UDMA_HYPER_MBA1_MBA1_MASK                                    0x7f000000
        #define UDMA_HYPER_MBA1_MBA1_RESET                                   0x0
        
        // Device type : - 1'b00: Octo/ Single SPI - 1'b01: HYPERBUS (access: R/W)
        #define UDMA_HYPER_DEVICE_TYPE_BIT                                   0
        #define UDMA_HYPER_DEVICE_TYPE_WIDTH                                 1
        #define UDMA_HYPER_DEVICE_TYPE_MASK                                  0x1
        #define UDMA_HYPER_DEVICE_TYPE_RESET                                 0x0
        
        // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH (access: R/W)
        #define UDMA_HYPER_DEVICE_DT0_BIT                                    1
        #define UDMA_HYPER_DEVICE_DT0_WIDTH                                  1
        #define UDMA_HYPER_DEVICE_DT0_MASK                                   0x2
        #define UDMA_HYPER_DEVICE_DT0_RESET                                  0x0
        
        // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH (access: R/W)
        #define UDMA_HYPER_DEVICE_DT1_BIT                                    2
        #define UDMA_HYPER_DEVICE_DT1_WIDTH                                  1
        #define UDMA_HYPER_DEVICE_DT1_MASK                                   0x4
        #define UDMA_HYPER_DEVICE_DT1_RESET                                  0x0
        
        // SDIO mode (access: R/W)
        #define UDMA_HYPER_DEVICE_SDIO_BIT                                   3
        #define UDMA_HYPER_DEVICE_SDIO_WIDTH                                 1
        #define UDMA_HYPER_DEVICE_SDIO_MASK                                  0x8
        #define UDMA_HYPER_DEVICE_SDIO_RESET                                 0x0
        
        // 2 Bytes SPI command (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_CMD_BIT                                  0
        #define UDMA_HYPER_OSPI_CMD_CMD_WIDTH                                16
        #define UDMA_HYPER_OSPI_CMD_CMD_MASK                                 0xffff
        #define UDMA_HYPER_OSPI_CMD_CMD_RESET                                0x0
        
        // SDIO CMD response type (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_BIT                    20
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_WIDTH                  3
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_MASK                   0x700000
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_RESET                  0x0
        
        // SDIO CMD operation code (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_BIT                          24
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_WIDTH                        6
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_MASK                         0x3f000000
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_RESET                        0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_MODE0_BIT                              0
        #define UDMA_HYPER_OSPI_ALTER_MODE0_WIDTH                            16
        #define UDMA_HYPER_OSPI_ALTER_MODE0_MASK                             0xffff
        #define UDMA_HYPER_OSPI_ALTER_MODE0_RESET                            0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_MODE1_BIT                              16
        #define UDMA_HYPER_OSPI_ALTER_MODE1_WIDTH                            16
        #define UDMA_HYPER_OSPI_ALTER_MODE1_MASK                             0xffff0000
        #define UDMA_HYPER_OSPI_ALTER_MODE1_RESET                            0x0
        
        // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_BIT                             0
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_WIDTH                           2
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_MASK                            0x3
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_RESET                           0x0
        
        // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_BIT                            4
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_WIDTH                          3
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_MASK                           0x70
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_RESET                          0x0
        
        // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_LINE_BIT                                 8
        #define UDMA_HYPER_OSPI_CFG_LINE_WIDTH                               2
        #define UDMA_HYPER_OSPI_CFG_LINE_MASK                                0x300
        #define UDMA_HYPER_OSPI_CFG_LINE_RESET                               0x0
        
        // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_BIT                          12
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_WIDTH                        1
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_MASK                         0x1000
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_RESET                        0x0
        
        // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_BIT                         13
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_MASK                        0x2000
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_RESET                       0x0
        
        // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_BIT                         14
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_MASK                        0x4000
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_RESET                       0x0
        
        // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_BIT                         15
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_MASK                        0x8000
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_RESET                       0x0
        
        // Octo SPI chip select index controlled by user  :  - 1'b0: CSN0 -  1'b1: CSN1 (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_INDEX_BIT                                0
        #define UDMA_HYPER_OSPI_CSN_INDEX_WIDTH                              1
        #define UDMA_HYPER_OSPI_CSN_INDEX_MASK                               0x1
        #define UDMA_HYPER_OSPI_CSN_INDEX_RESET                              0x0
        
        // Octo SPI chip select controlled by IP automatically  :  - 1'b0: IP control CSN according to index -  1'b1: : IP control CSN according to address range automatically (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_BIT                              1
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_WIDTH                            1
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_MASK                             0x2
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_RESET                            0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_RESERVED_BIT                             2
        #define UDMA_HYPER_OSPI_CSN_RESERVED_WIDTH                           2
        #define UDMA_HYPER_OSPI_CSN_RESERVED_MASK                            0xc
        #define UDMA_HYPER_OSPI_CSN_RESERVED_RESET                           0x0
        
        // Octo SPI chip select controlled by user enable GPIO mode :  - 1'b0: IP control CSN according to index -  1'b1: USER control CSN in GPIO mode (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_BIT                          4
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_WIDTH                        1
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_MASK                         0x10
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_RESET                        0x0
        
        // Octo SPI chip select value controlled by user  :  - 1'b0: HIGH -  1'b1: : Low (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_VALUE_BIT                                5
        #define UDMA_HYPER_OSPI_CSN_VALUE_WIDTH                              1
        #define UDMA_HYPER_OSPI_CSN_VALUE_MASK                               0x20
        #define UDMA_HYPER_OSPI_CSN_VALUE_RESET                              0x0
        
        // SDIO data quad enable  :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_BIT                       6
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_WIDTH                     1
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_MASK                      0x40
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_RESET                     0x0
        
        // SDIO data quad ddr enable  :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_BIT                   7
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_WIDTH                 1
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_MASK                  0x80
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_RESET                 0x0
        
        // SDIO data block number (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_BIT                       8
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_WIDTH                     8
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_MASK                      0xff00
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_RESET                     0x0
        
        // SDIO data block size (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_BIT                      16
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_WIDTH                    10
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_MASK                     0x3ff0000
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_RESET                    0x0
        
        // SDIO enable HW auto stop  after multiple read and write :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_BIT                       26
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_WIDTH                     1
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_MASK                      0x4000000
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_RESET                     0x1
        
        // Octo SPI chip in JEDEC reset mode enable :  - 1'b0: JEDEC reset disable -  1'b1: USER control sdo0 (access: R/W)
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_BIT            0
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_WIDTH          1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_MASK           0x1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_RESET          0x0
        
        // Octo SPI chip in JEDEC reset mode, sdo0 value (access: R/W)
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_BIT         1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_WIDTH       1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_MASK        0x2
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_RESET       0x0
        
        // Octo SPI RAM optimisation read enable for CS 0, special when no accross boundary rwds latency for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_BIT                   0
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_WIDTH                 2
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_MASK                  0x3
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_RESET                 0x0
        
        // Octo SPI or single SPI which use real address instead of address/2 for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_BIT                     2
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_WIDTH                   2
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_MASK                    0xc
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_RESET                   0x0
        
        // PSRAM CMD automatically reform - CMD bit[47] R/W# - Read is 1 or 0 for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_BIT                   4
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_WIDTH                 2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_MASK                  0x30
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_RESET                 0x0
        
        // PSRAM CMD automatically reform enable for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_BIT                     6
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_WIDTH                   2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_MASK                    0xc0
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_RESET                   0x0
        
        // PSRAM even address reform for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_BIT                  8
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_WIDTH                2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_MASK                 0x300
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_RESET                0x0
        
        // PSRAM cross boundary access optimisation enable for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_BIT          10
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_WIDTH        2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_MASK         0xc00
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_RESET        0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_BIT                          0
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_WIDTH                        16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_MASK                         0xffff
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_RESET                        0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_BIT                          16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_WIDTH                        16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_MASK                         0xffff0000
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_RESET                        0x0
        
        // XIP latency0 for cs0 (access: -)
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_BIT                     0
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_WIDTH                   5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_MASK                    0x1f
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_RESET                   0x0
        
        // XIP latency1 for cs1 (access: -)
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_BIT                     5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_WIDTH                   5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_MASK                    0x3e0
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_RESET                   0x0
        
        // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_HYPER_LINE_2D_LINE_BIT                                  0
        #define UDMA_HYPER_LINE_2D_LINE_WIDTH                                32
        #define UDMA_HYPER_LINE_2D_LINE_MASK                                 0xffffffff
        #define UDMA_HYPER_LINE_2D_LINE_RESET                                0x0
        
        // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_HYPER_STRIDE_2D_STRIDE_BIT                              0
        #define UDMA_HYPER_STRIDE_2D_STRIDE_WIDTH                            32
        #define UDMA_HYPER_STRIDE_2D_STRIDE_MASK                             0xffffffff
        #define UDMA_HYPER_STRIDE_2D_STRIDE_RESET                            0x0
        
        // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 0 : - 1'b0 disable    - 1'b1 Enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_BIT            0
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_WIDTH          1
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_MASK           0x1
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_RESET          0x0
        
        // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 1 : - 1'b0 disable    - 1'b1 Enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_BIT            1
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_WIDTH          1
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_MASK           0x2
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_RESET          0x0
        
        // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 0:  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_BIT         2
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_WIDTH       1
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_MASK        0x4
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_RESET       0x0
        
        // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 1 :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_BIT         3
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_WIDTH       1
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_MASK        0x8
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_RESET       0x0
        
        // OSPI burst 2D mode enable for normal mode and XIP :  - 1'b0: BURST 2D mode disable -  1'b1: BURST 2D mode disable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_BIT                        4
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_WIDTH                      2
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_MASK                       0x30
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_RESET                      0x0
        
        // 2D tansfer mode from L2 to external memory config :  - 2'b00: 1D_TO_1D - 2'b01: 1D_TO_2D - 2'b10: 2D_TO_1D - 2'b11: 2D_TO_2D (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_BIT                          6
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_WIDTH                        2
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_MASK                         0xc0
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_RESET                        0x0
        
        // Octo SPI interrupt enable control :  - 1'b0: interrupt disable -  1'b1: Interrupt enable (access: R/W)
        #define UDMA_HYPER_IRQ_EN_EN_BIT                                     0
        #define UDMA_HYPER_IRQ_EN_EN_WIDTH                                   1
        #define UDMA_HYPER_IRQ_EN_EN_MASK                                    0x1
        #define UDMA_HYPER_IRQ_EN_EN_RESET                                   0x0
        
        // Octo SPI interrupt enable control for XIP :  - 1'b0: interrupt disable -  1'b1: Interrupt enable (access: R/W)
        #define UDMA_HYPER_IRQ_EN_XIP_EN_BIT                                 1
        #define UDMA_HYPER_IRQ_EN_XIP_EN_WIDTH                               1
        #define UDMA_HYPER_IRQ_EN_XIP_EN_MASK                                0x2
        #define UDMA_HYPER_IRQ_EN_XIP_EN_RESET                               0x0
        
        // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4  (access: R/W)
        #define UDMA_HYPER_CLK_DIV_DATA_BIT                                  0
        #define UDMA_HYPER_CLK_DIV_DATA_WIDTH                                8
        #define UDMA_HYPER_CLK_DIV_DATA_MASK                                 0xff
        #define UDMA_HYPER_CLK_DIV_DATA_RESET                                0x0
        
        // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0. (access: W)
        #define UDMA_HYPER_CLK_DIV_VALID_BIT                                 8
        #define UDMA_HYPER_CLK_DIV_VALID_WIDTH                               1
        #define UDMA_HYPER_CLK_DIV_VALID_MASK                                0x100
        #define UDMA_HYPER_CLK_DIV_VALID_RESET                               0x0
        
        // TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_TX_ERROR_BIT                               0
        #define UDMA_HYPER_STATUS_TX_ERROR_WIDTH                             1
        #define UDMA_HYPER_STATUS_TX_ERROR_MASK                              0x1
        #define UDMA_HYPER_STATUS_TX_ERROR_RESET                             0x0
        
        // RX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_RX_ERROR_BIT                               1
        #define UDMA_HYPER_STATUS_RX_ERROR_WIDTH                             1
        #define UDMA_HYPER_STATUS_RX_ERROR_MASK                              0x2
        #define UDMA_HYPER_STATUS_RX_ERROR_RESET                             0x0
        
        // RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end (access: R/W)
        #define UDMA_HYPER_STATUS_RX_TX_END_BIT                              2
        #define UDMA_HYPER_STATUS_RX_TX_END_WIDTH                            1
        #define UDMA_HYPER_STATUS_RX_TX_END_MASK                             0x4
        #define UDMA_HYPER_STATUS_RX_TX_END_RESET                            0x0
        
        // SDIO RX TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_BIT                       3
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_WIDTH                     1
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_MASK                      0x8
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_RESET                     0x0
        
        // SDIO RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_BIT                         4
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_WIDTH                       1
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_MASK                        0x10
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_RESET                       0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_STATUS_RESERVED_BIT                               5
        #define UDMA_HYPER_STATUS_RESERVED_WIDTH                             11
        #define UDMA_HYPER_STATUS_RESERVED_MASK                              0xffe0
        #define UDMA_HYPER_STATUS_RESERVED_RESET                             0x0
        
        // SDIO error status flag, indicate the error type (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_BIT                      16
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_WIDTH                    16
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_MASK                     0xffff0000
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_RESET                    0x0
        
        // SDIO command argument (access: R/W)
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_BIT                              0
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_WIDTH                            32
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_MASK                             0xffffffff
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_RESET                            0x0
        
        // SDIO response 0 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP0_RSP0_BIT                                0
        #define UDMA_HYPER_SDIO_RSP0_RSP0_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP0_RSP0_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP0_RSP0_RESET                              0x0
        
        // SDIO response 1 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP1_RSP1_BIT                                0
        #define UDMA_HYPER_SDIO_RSP1_RSP1_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP1_RSP1_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP1_RSP1_RESET                              0x0
        
        // SDIO response 2 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP2_RSP2_BIT                                0
        #define UDMA_HYPER_SDIO_RSP2_RSP2_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP2_RSP2_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP2_RSP2_RESET                              0x0
        
        // SDIO response 3 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP3_RSP3_BIT                                0
        #define UDMA_HYPER_SDIO_RSP3_RSP3_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP3_RSP3_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP3_RSP3_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_RX_DEST_DEST_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_RX_DEST_DEST_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_RX_DEST_DEST_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_RX_DEST_DEST(val)                       ((val) << 0)
        
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_GET(value)          (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_GETS(value)         (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_SET(value,field)    (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM(val)                ((val) << 8)
        
        #define UDMA_HYPER_TX_DEST_DEST_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_TX_DEST_DEST_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_TX_DEST_DEST_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_TX_DEST_DEST(val)                       ((val) << 0)
        
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_GET(value)          (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_GETS(value)         (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_SET(value,field)    (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM(val)                ((val) << 8)
        
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA(val)                ((val) << 0)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN(val)                  ((val) << 1)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_GET(value)         (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_GETS(value)        (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_SET(value,field)   (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0(val)               ((val) << 2)
        
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_GET(value)         (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_GETS(value)        (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_SET(value,field)   (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN(val)               ((val) << 4)
        
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_GET(value)     (GAP_BEXTRACTU((value),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_GETS(value)    (GAP_BEXTRACT((value),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN(val)           ((val) << 5)
        
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256(val)      ((val) << 6)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_GET(value)         (GAP_BEXTRACTU((value),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_GETS(value)        (GAP_BEXTRACT((value),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_SET(value,field)   (GAP_BINSERT((value),(field),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1(val)               ((val) << 7)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_GET(value)     (GAP_BEXTRACTU((value),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_GETS(value)    (GAP_BEXTRACT((value),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN(val)           ((val) << 8)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_GET(value) (GAP_BEXTRACTU((value),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_GETS(value) (GAP_BEXTRACT((value),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN(val)       ((val) << 9)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_GET(value) (GAP_BEXTRACTU((value),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_GETS(value) (GAP_BEXTRACT((value),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256(val)  ((val) << 10)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_GET(value)         (GAP_BEXTRACTU((value),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_GETS(value)        (GAP_BEXTRACT((value),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_SET(value,field)   (GAP_BINSERT((value),(field),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2(val)               ((val) << 11)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_GET(value)        (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_GETS(value)       (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_SET(value,field)  (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED(val)              ((val) << 12)
        
        #define UDMA_HYPER_TRANS_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR(val)                    ((val) << 0)
        
        #define UDMA_HYPER_TRANS_SIZE_SIZE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE(val)                    ((val) << 0)
        
        #define UDMA_HYPER_TRANS_CFG_RXTX_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX(val)                     ((val) << 0)
        
        #define UDMA_HYPER_TRANS_CFG_VALID_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID(val)                    ((val) << 1)
        
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD(val)                   ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_GET(value) (GAP_BEXTRACTU((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_GETS(value) (GAP_BEXTRACT((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE(val)     ((val) << 20)
        
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_GET(value)     (GAP_BEXTRACTU((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_GETS(value)    (GAP_BEXTRACT((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP(val)           ((val) << 24)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_GET(value)        (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_GETS(value)       (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_SET(value,field)  (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE(val)              ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_GET(value)       (GAP_BEXTRACTU((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_GETS(value)      (GAP_BEXTRACT((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_SET(value,field) (GAP_BINSERT((value),(field),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE(val)             ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_GET(value)            (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_GETS(value)           (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_SET(value,field)      (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE(val)                  ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_GET(value)     (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_GETS(value)    (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR(val)           ((val) << 12)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_GET(value)    (GAP_BEXTRACTU((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_GETS(value)   (GAP_BEXTRACT((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR(val)          ((val) << 13)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_GET(value)    (GAP_BEXTRACTU((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_GETS(value)   (GAP_BEXTRACT((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR(val)          ((val) << 14)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_GET(value)    (GAP_BEXTRACTU((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_GETS(value)   (GAP_BEXTRACT((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB(val)          ((val) << 15)
        
        #define UDMA_HYPER_EXT_ADDR_SADDR_GET(value)               (GAP_BEXTRACTU((value),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR_GETS(value)              (GAP_BEXTRACT((value),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR_SET(value,field)         (GAP_BINSERT((value),(field),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR(val)                     ((val) << 0)
        
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_GET(value)          (GAP_BEXTRACTU((value),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_GETS(value)         (GAP_BEXTRACT((value),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_SET(value,field)    (GAP_BINSERT((value),(field),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS(val)                ((val) << 31)
        
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_GET(value)          (GAP_BEXTRACTU((value),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_GETS(value)         (GAP_BEXTRACT((value),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_SET(value,field)    (GAP_BINSERT((value),(field),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0(val)                ((val) << 0)
        
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_GET(value)          (GAP_BEXTRACTU((value),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_GETS(value)         (GAP_BEXTRACT((value),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_SET(value,field)    (GAP_BINSERT((value),(field),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1(val)                ((val) << 5)
        
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_GET(value)       (GAP_BEXTRACTU((value),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_GETS(value)      (GAP_BEXTRACT((value),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_SET(value,field) (GAP_BINSERT((value),(field),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY(val)             ((val) << 10)
        
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_GET(value)        (GAP_BEXTRACTU((value),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_GETS(value)       (GAP_BEXTRACT((value),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_SET(value,field)  (GAP_BINSERT((value),(field),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY(val)              ((val) << 14)
        
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_GET(value) (GAP_BEXTRACTU((value),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_GETS(value) (GAP_BEXTRACT((value),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(val) ((val) << 17)
        
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_GET(value)            (GAP_BEXTRACTU((value),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_GETS(value)           (GAP_BEXTRACT((value),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_SET(value,field)      (GAP_BINSERT((value),(field),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX(val)                  ((val) << 18)
        
        #define UDMA_HYPER_MBA0_RESERVED_GET(value)                (GAP_BEXTRACTU((value),24,0))
        #define UDMA_HYPER_MBA0_RESERVED_GETS(value)               (GAP_BEXTRACT((value),24,0))
        #define UDMA_HYPER_MBA0_RESERVED_SET(value,field)          (GAP_BINSERT((value),(field),24,0))
        #define UDMA_HYPER_MBA0_RESERVED(val)                      ((val) << 0)
        
        #define UDMA_HYPER_MBA0_MBA0_GET(value)                    (GAP_BEXTRACTU((value),7,24))
        #define UDMA_HYPER_MBA0_MBA0_GETS(value)                   (GAP_BEXTRACT((value),7,24))
        #define UDMA_HYPER_MBA0_MBA0_SET(value,field)              (GAP_BINSERT((value),(field),7,24))
        #define UDMA_HYPER_MBA0_MBA0(val)                          ((val) << 24)
        
        #define UDMA_HYPER_MBA1_RESERVED_GET(value)                (GAP_BEXTRACTU((value),24,0))
        #define UDMA_HYPER_MBA1_RESERVED_GETS(value)               (GAP_BEXTRACT((value),24,0))
        #define UDMA_HYPER_MBA1_RESERVED_SET(value,field)          (GAP_BINSERT((value),(field),24,0))
        #define UDMA_HYPER_MBA1_RESERVED(val)                      ((val) << 0)
        
        #define UDMA_HYPER_MBA1_MBA1_GET(value)                    (GAP_BEXTRACTU((value),7,24))
        #define UDMA_HYPER_MBA1_MBA1_GETS(value)                   (GAP_BEXTRACT((value),7,24))
        #define UDMA_HYPER_MBA1_MBA1_SET(value,field)              (GAP_BINSERT((value),(field),7,24))
        #define UDMA_HYPER_MBA1_MBA1(val)                          ((val) << 24)
        
        #define UDMA_HYPER_DEVICE_TYPE_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_DEVICE_TYPE_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_DEVICE_TYPE_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_DEVICE_TYPE(val)                        ((val) << 0)
        
        #define UDMA_HYPER_DEVICE_DT0_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_DEVICE_DT0_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_DEVICE_DT0_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_DEVICE_DT0(val)                         ((val) << 1)
        
        #define UDMA_HYPER_DEVICE_DT1_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_DEVICE_DT1_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_DEVICE_DT1_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_DEVICE_DT1(val)                         ((val) << 2)
        
        #define UDMA_HYPER_DEVICE_SDIO_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_DEVICE_SDIO_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_DEVICE_SDIO_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_DEVICE_SDIO(val)                        ((val) << 3)
        
        #define UDMA_HYPER_OSPI_CMD_CMD_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD(val)                       ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_GET(value)   (GAP_BEXTRACTU((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_GETS(value)  (GAP_BEXTRACT((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE(val)         ((val) << 20)
        
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_GET(value)         (GAP_BEXTRACTU((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_GETS(value)        (GAP_BEXTRACT((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_SET(value,field)   (GAP_BINSERT((value),(field),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP(val)               ((val) << 24)
        
        #define UDMA_HYPER_OSPI_ALTER_MODE0_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0(val)                   ((val) << 0)
        
        #define UDMA_HYPER_OSPI_ALTER_MODE1_GET(value)             (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1_GETS(value)            (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1_SET(value,field)       (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1(val)                   ((val) << 16)
        
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_GET(value)            (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_GETS(value)           (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE(val)                  ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_GET(value)           (GAP_BEXTRACTU((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_GETS(value)          (GAP_BEXTRACT((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_SET(value,field)     (GAP_BINSERT((value),(field),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE(val)                 ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CFG_LINE_GET(value)                (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE_GETS(value)               (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE_SET(value,field)          (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE(val)                      ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_GET(value)         (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_GETS(value)        (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_SET(value,field)   (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR(val)               ((val) << 12)
        
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_GET(value)        (GAP_BEXTRACTU((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_GETS(value)       (GAP_BEXTRACT((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_SET(value,field)  (GAP_BINSERT((value),(field),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR(val)              ((val) << 13)
        
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_GET(value)        (GAP_BEXTRACTU((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_GETS(value)       (GAP_BEXTRACT((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_SET(value,field)  (GAP_BINSERT((value),(field),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR(val)              ((val) << 14)
        
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_GET(value)        (GAP_BEXTRACTU((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_GETS(value)       (GAP_BEXTRACT((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_SET(value,field)  (GAP_BINSERT((value),(field),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB(val)              ((val) << 15)
        
        #define UDMA_HYPER_OSPI_CSN_INDEX_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX(val)                     ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN(val)                   ((val) << 1)
        
        #define UDMA_HYPER_OSPI_CSN_RESERVED_GET(value)            (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED_GETS(value)           (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED_SET(value,field)      (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED(val)                  ((val) << 2)
        
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_GET(value)         (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_GETS(value)        (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_SET(value,field)   (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL(val)               ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CSN_VALUE_GET(value)               (GAP_BEXTRACTU((value),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE_GETS(value)              (GAP_BEXTRACT((value),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE_SET(value,field)         (GAP_BINSERT((value),(field),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE(val)                     ((val) << 5)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD(val)            ((val) << 6)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_GET(value)  (GAP_BEXTRACTU((value),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR(val)        ((val) << 7)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM(val)            ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_GET(value)     (GAP_BEXTRACTU((value),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_GETS(value)    (GAP_BEXTRACT((value),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_SET(value,field) (GAP_BINSERT((value),(field),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE(val)           ((val) << 16)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP(val)            ((val) << 26)
        
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN(val) ((val) << 0)
        
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE(val) ((val) << 1)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_GET(value)  (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_GETS(value) (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS(val)        ((val) << 0)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_GET(value)    (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_GETS(value)   (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_SET(value,field) (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN(val)          ((val) << 2)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_GET(value)  (GAP_BEXTRACTU((value),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_GETS(value) (GAP_BEXTRACT((value),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_SET(value,field) (GAP_BINSERT((value),(field),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT(val)        ((val) << 4)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_GET(value)    (GAP_BEXTRACTU((value),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_GETS(value)   (GAP_BEXTRACT((value),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_SET(value,field) (GAP_BINSERT((value),(field),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN(val)          ((val) << 6)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_GET(value) (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_GETS(value) (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_SET(value,field) (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN(val)       ((val) << 8)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_GET(value) (GAP_BEXTRACTU((value),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_GETS(value) (GAP_BEXTRACT((value),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_SET(value,field) (GAP_BINSERT((value),(field),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN(val) ((val) << 10)
        
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_GET(value)         (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_GETS(value)        (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_SET(value,field)   (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0(val)               ((val) << 0)
        
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_GET(value)         (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_GETS(value)        (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_SET(value,field)   (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1(val)               ((val) << 16)
        
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0(val)          ((val) << 0)
        
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_GET(value)    (GAP_BEXTRACTU((value),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_GETS(value)   (GAP_BEXTRACT((value),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_SET(value,field) (GAP_BINSERT((value),(field),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1(val)          ((val) << 5)
        
        #define UDMA_HYPER_LINE_2D_LINE_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_LINE_2D_LINE_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_LINE_2D_LINE_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_LINE_2D_LINE(val)                       ((val) << 0)
        
        #define UDMA_HYPER_STRIDE_2D_STRIDE_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE(val)                   ((val) << 0)
        
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE(val) ((val) << 0)
        
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE(val) ((val) << 1)
        
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE(val) ((val) << 2)
        
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE(val) ((val) << 3)
        
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_GET(value)       (GAP_BEXTRACTU((value),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_GETS(value)      (GAP_BEXTRACT((value),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE(val)             ((val) << 4)
        
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_GET(value)         (GAP_BEXTRACTU((value),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_GETS(value)        (GAP_BEXTRACT((value),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE(val)               ((val) << 6)
        
        #define UDMA_HYPER_IRQ_EN_EN_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_IRQ_EN_EN_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_IRQ_EN_EN_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_IRQ_EN_EN(val)                          ((val) << 0)
        
        #define UDMA_HYPER_IRQ_EN_XIP_EN_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN(val)                      ((val) << 1)
        
        #define UDMA_HYPER_CLK_DIV_DATA_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA(val)                       ((val) << 0)
        
        #define UDMA_HYPER_CLK_DIV_VALID_GET(value)                (GAP_BEXTRACTU((value),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID_GETS(value)               (GAP_BEXTRACT((value),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID_SET(value,field)          (GAP_BINSERT((value),(field),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID(val)                      ((val) << 8)
        
        #define UDMA_HYPER_STATUS_TX_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR(val)                    ((val) << 0)
        
        #define UDMA_HYPER_STATUS_RX_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR(val)                    ((val) << 1)
        
        #define UDMA_HYPER_STATUS_RX_TX_END_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END(val)                   ((val) << 2)
        
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR(val)            ((val) << 3)
        
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END(val)              ((val) << 4)
        
        #define UDMA_HYPER_STATUS_RESERVED_GET(value)              (GAP_BEXTRACTU((value),11,5))
        #define UDMA_HYPER_STATUS_RESERVED_GETS(value)             (GAP_BEXTRACT((value),11,5))
        #define UDMA_HYPER_STATUS_RESERVED_SET(value,field)        (GAP_BINSERT((value),(field),11,5))
        #define UDMA_HYPER_STATUS_RESERVED(val)                    ((val) << 5)
        
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_GET(value)     (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_GETS(value)    (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS(val)           ((val) << 16)
        
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG(val)                   ((val) << 0)
        
        #define UDMA_HYPER_SDIO_RSP0_RSP0_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0(val)                     ((val) << 0)
        
        #define UDMA_HYPER_SDIO_RSP1_RSP1_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1(val)                     ((val) << 0)
        
        #define UDMA_HYPER_SDIO_RSP2_RSP2_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2(val)                     ((val) << 0)
        
        #define UDMA_HYPER_SDIO_RSP3_RSP3_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_HYPER_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t rx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t tx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t trans_mode;  // Configure transaction mode
            volatile uint32_t trans_addr;  // Configure each transaction addr
            volatile uint32_t trans_size;  // Configure each transaction size
            volatile uint32_t trans_cfg;  // Start  each transaction rx/tx
            volatile uint32_t ospi_cmd_xip;  // OSPI XIP command and psram command config
            volatile uint32_t ospi_cfg_xip;  // OSPI XIP configuration
            volatile uint32_t ext_addr;  // Memory access address register.
            volatile uint32_t timing_cfg;  // HYPERBUS and Octo SPI Memory Timing configuration register.
            volatile uint32_t mba0;  // Device start address register.
            volatile uint32_t mba1;  // Device start address register.
            volatile uint32_t device;  // Device type register(RAM or FLASH).
            volatile uint32_t ospi_cmd;  // OSPI command and psram command config
            volatile uint32_t ospi_alter;  // OSPI alternative 2 bytes
            volatile uint32_t ospi_cfg;  // OSPI configuration
            volatile uint32_t ospi_csn;  // OSPI chip select configuration
            volatile uint32_t ospi_jedec_reset;  // OSPI JEDEC Hardware Reset, user can control sdo0 manually
            volatile uint32_t ospi_ram_opt;  // OSPI RAM DATA transfer optimisation, only in auto mode
            volatile uint32_t ospi_alter_xip;  // OSPI XIP alternative 2 bytes
            volatile uint32_t ospi_reg_xip;  // OSPI XIP other configuration
            volatile uint32_t line_2d;  // OSPI 2D line.
            volatile uint32_t stride_2d;  // OSPI 2D stride.
            volatile uint32_t burst_enable;  // OSPI burst mode/2D mode enable.
            volatile uint32_t irq_en;  // OSPI interrupt enable register
            volatile uint32_t clk_div;  // Clock divide.
            volatile uint32_t status;  // Transfer status for error.
            volatile uint32_t sdio_cmd_arg;  // SDIO command argument.
            volatile uint32_t sdio_rsp0;  // SDIO response 0.
            volatile uint32_t sdio_rsp1;  // SDIO response 1.
            volatile uint32_t sdio_rsp2;  // SDIO response 2.
            volatile uint32_t sdio_rsp3;  // SDIO response 3.
        } __attribute__((packed)) udma_hyper_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)
            unsigned int dest_stream     :8 ; // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_rx_dest_t;
        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the TX 2D uDMA channel. Default is 0xFF(channel disabled)
            unsigned int dest_stream     :8 ; // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_tx_dest_t;
        
        typedef union {
          struct {
            unsigned int auto_ena        :1 ; // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN
            unsigned int xip_en          :1 ; // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN
            unsigned int reserved0       :2 ; // -
            unsigned int stream_en       :1 ; // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN
            unsigned int aes_stream_en   :1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int aes_stream_128_256:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int reserved1       :1 ; // -
            unsigned int xip_stream_en   :1 ; // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN
            unsigned int xip_aes_stream_en:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int xip_aes_stream_128_256:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int reserved2       :1 ; // -
            unsigned int xip_halted      :1 ; // Halted XIP refill when in XIP, XIP refill will wait SW unlock this bit.  - 1'b0: XIP_RUNNING - 1'b1: XIP_HALTED
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_mode_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Transfer addr, only when MODE is in AUTO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_addr_t;
        
        typedef union {
          struct {
            unsigned int size            :32; // Transfer Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_size_t;
        
        typedef union {
          struct {
            unsigned int rxtx            :1 ; // Transfer type - 1'b0: TX - 1'b1: RX
            unsigned int valid           :1 ; // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_cfg_t;
        
        typedef union {
          struct {
            unsigned int cmd             :16; // 2 Bytes SPI command
            unsigned int padding0:4 ;
            unsigned int sdio_cmd_rsp_type:3 ; // SDIO CMD response type
            unsigned int padding1:1 ;
            unsigned int sdio_cmd_op     :6 ; // SDIO CMD operation code
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cmd_xip_t;
        
        typedef union {
          struct {
            unsigned int cmd_size        :2 ; // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte
            unsigned int padding0:2 ;
            unsigned int addr_size       :3 ; // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte
            unsigned int padding1:1 ;
            unsigned int line            :2 ; // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])
            unsigned int padding2:2 ;
            unsigned int cmd_dtr_str     :1 ; // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int addr_dtr_str    :1 ; // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_str    :1 ; // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_msb    :1 ; // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cfg_xip_t;
        
        typedef union {
          struct {
            unsigned int saddr           :31; // Memory access address bitfield.
            unsigned int reg_access      :1 ; // Register access flag bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ext_addr_t;
        
        typedef union {
          struct {
            unsigned int latency0        :5 ; // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int latency1        :5 ; // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int rw_recovery     :4 ; // Some HyperBus devices may require a minimum time between the end of a prior transaction and the start of a new access. This time is referred to as Read-Write-Recovery time (tRWR). The master interface must start driving CS# Low only at a time when the CA1 transfer will complete after tRWR is satisfied. - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int rwds_delay      :3 ; // Delay of RWDS for center aligned read: - 3'b000: 0 logic delay - 3'b001: 1 logic delay - 3'b010: 2 logic delay  - 3'b111: 7 logic delay
            unsigned int additional_latency_autocheck_en:1 ; // Auto check for RWDS high or low for additional latency : - 1'b0: additional latency no autocheck - 1'b1: additional latency autocheck
            unsigned int cs_max          :14; // Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer : - 14'h0000: 1 CK - 14'h0001: 2 CK - 14'h0011: 3 CK -  - 14'h3FFF: 16383 CK
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_timing_cfg_t;
        
        typedef union {
          struct {
            unsigned int reserved        :24; // -
            unsigned int mba0            :7 ; // Memory Base Address 0 for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CS1 = 0;  else CS0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CS0 = 0;  else CS1 = 0;
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_mba0_t;
        
        typedef union {
          struct {
            unsigned int reserved        :24; // -
            unsigned int mba1            :7 ; // Memory Base Address  for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CSn1 = 0;  else CSn0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CSn0 = 0;  else CSn1 = 0;
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_mba1_t;
        
        typedef union {
          struct {
            unsigned int type            :1 ; // Device type : - 1'b00: Octo/ Single SPI - 1'b01: HYPERBUS
            unsigned int dt0             :1 ; // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH
            unsigned int dt1             :1 ; // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH
            unsigned int sdio            :1 ; // SDIO mode
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_device_t;
        
        typedef union {
          struct {
            unsigned int cmd             :16; // 2 Bytes SPI command
            unsigned int padding0:4 ;
            unsigned int sdio_cmd_rsp_type:3 ; // SDIO CMD response type
            unsigned int padding1:1 ;
            unsigned int sdio_cmd_op     :6 ; // SDIO CMD operation code
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cmd_t;
        
        typedef union {
          struct {
            unsigned int mode0           :16; // 2 Bytes SPI alternative
            unsigned int mode1           :16; // 2 Bytes SPI alternative
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_alter_t;
        
        typedef union {
          struct {
            unsigned int cmd_size        :2 ; // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte
            unsigned int padding0:2 ;
            unsigned int addr_size       :3 ; // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte
            unsigned int padding1:1 ;
            unsigned int line            :2 ; // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])
            unsigned int padding2:2 ;
            unsigned int cmd_dtr_str     :1 ; // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int addr_dtr_str    :1 ; // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_str    :1 ; // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_msb    :1 ; // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cfg_t;
        
        typedef union {
          struct {
            unsigned int index           :1 ; // Octo SPI chip select index controlled by user  :  - 1'b0: CSN0 -  1'b1: CSN1
            unsigned int auto_en         :1 ; // Octo SPI chip select controlled by IP automatically  :  - 1'b0: IP control CSN according to index -  1'b1: : IP control CSN according to address range automatically
            unsigned int reserved        :2 ; // -
            unsigned int direct_ctrl     :1 ; // Octo SPI chip select controlled by user enable GPIO mode :  - 1'b0: IP control CSN according to index -  1'b1: USER control CSN in GPIO mode
            unsigned int value           :1 ; // Octo SPI chip select value controlled by user  :  - 1'b0: HIGH -  1'b1: : Low
            unsigned int sdio_data_quad  :1 ; // SDIO data quad enable  :  - 1'b0: Disable -  1'b1: : Enable
            unsigned int sdio_data_quad_ddr:1 ; // SDIO data quad ddr enable  :  - 1'b0: Disable -  1'b1: : Enable
            unsigned int sdio_block_num  :8 ; // SDIO data block number
            unsigned int sdio_block_size :10; // SDIO data block size
            unsigned int sdio_auto_stop  :1 ; // SDIO enable HW auto stop  after multiple read and write :  - 1'b0: Disable -  1'b1: : Enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_csn_t;
        
        typedef union {
          struct {
            unsigned int user_ctrl_sdo0_en:1 ; // Octo SPI chip in JEDEC reset mode enable :  - 1'b0: JEDEC reset disable -  1'b1: USER control sdo0
            unsigned int user_ctrl_sdo0_value:1 ; // Octo SPI chip in JEDEC reset mode, sdo0 value
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_jedec_reset_t;
        
        typedef union {
          struct {
            unsigned int opt_read_en_cs  :2 ; // Octo SPI RAM optimisation read enable for CS 0, special when no accross boundary rwds latency for each channel :  - 1'b0: disable -  1'b1: enable
            unsigned int real_addr_en    :2 ; // Octo SPI or single SPI which use real address instead of address/2 for each channel :  - 1'b0: disable -  1'b1: enable
            unsigned int psram_read_bit  :2 ; // PSRAM CMD automatically reform - CMD bit[47] R/W# - Read is 1 or 0 for each channel
            unsigned int psram_cmd_en    :2 ; // PSRAM CMD automatically reform enable for each channel
            unsigned int psram_addr_even :2 ; // PSRAM even address reform for each channel
            unsigned int psram_cross_boundary_en:2 ; // PSRAM cross boundary access optimisation enable for each channel :  - 1'b0: disable -  1'b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_ram_opt_t;
        
        typedef union {
          struct {
            unsigned int mode0           :16; // 2 Bytes SPI alternative
            unsigned int mode1           :16; // 2 Bytes SPI alternative
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_alter_xip_t;
        
        typedef union {
          struct {
            unsigned int xip_latency0    :5 ; // XIP latency0 for cs0
            unsigned int xip_latency1    :5 ; // XIP latency1 for cs1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_reg_xip_t;
        
        typedef union {
          struct {
            unsigned int line            :32; // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_line_2d_t;
        
        typedef union {
          struct {
            unsigned int stride          :32; // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_stride_2d_t;
        
        typedef union {
          struct {
            unsigned int cs0_auto_burst_enable:1 ; // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 0 : - 1'b0 disable    - 1'b1 Enable
            unsigned int cs1_auto_burst_enable:1 ; // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 1 : - 1'b0 disable    - 1'b1 Enable
            unsigned int cs0_maximum_check_enable:1 ; // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 0:  - 1'b0: disable -  1'b1: enable
            unsigned int cs1_maximum_check_enable:1 ; // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 1 :  - 1'b0: disable -  1'b1: enable
            unsigned int _2d_enable      :2 ; // OSPI burst 2D mode enable for normal mode and XIP :  - 1'b0: BURST 2D mode disable -  1'b1: BURST 2D mode disable
            unsigned int _2d_mode        :2 ; // 2D tansfer mode from L2 to external memory config :  - 2'b00: 1D_TO_1D - 2'b01: 1D_TO_2D - 2'b10: 2D_TO_1D - 2'b11: 2D_TO_2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_burst_enable_t;
        
        typedef union {
          struct {
            unsigned int en              :1 ; // Octo SPI interrupt enable control :  - 1'b0: interrupt disable -  1'b1: Interrupt enable
            unsigned int xip_en          :1 ; // Octo SPI interrupt enable control for XIP :  - 1'b0: interrupt disable -  1'b1: Interrupt enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_irq_en_t;
        
        typedef union {
          struct {
            unsigned int data            :8 ; // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4 
            unsigned int valid           :1 ; // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_clk_div_t;
        
        typedef union {
          struct {
            unsigned int tx_error        :1 ; // TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int rx_error        :1 ; // RX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int rx_tx_end       :1 ; // RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end
            unsigned int sdio_rx_tx_error:1 ; // SDIO RX TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int sdio_rx_tx_end  :1 ; // SDIO RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end
            unsigned int reserved        :11; // -
            unsigned int sdio_error_status:16; // SDIO error status flag, indicate the error type
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_status_t;
        
        typedef union {
          struct {
            unsigned int arg             :32; // SDIO command argument
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_cmd_arg_t;
        
        typedef union {
          struct {
            unsigned int rsp0            :32; // SDIO response 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp0_t;
        
        typedef union {
          struct {
            unsigned int rsp1            :32; // SDIO response 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp1_t;
        
        typedef union {
          struct {
            unsigned int rsp2            :32; // SDIO response 2
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp2_t;
        
        typedef union {
          struct {
            unsigned int rsp3            :32; // SDIO response 3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_hyper : public vp::regmap
        {
        public:
            vp_udma_hyper_rx_dest rx_dest;
            vp_udma_hyper_tx_dest tx_dest;
            vp_udma_hyper_trans_mode trans_mode;
            vp_udma_hyper_trans_addr trans_addr;
            vp_udma_hyper_trans_size trans_size;
            vp_udma_hyper_trans_cfg trans_cfg;
            vp_udma_hyper_ospi_cmd_xip ospi_cmd_xip;
            vp_udma_hyper_ospi_cfg_xip ospi_cfg_xip;
            vp_udma_hyper_ext_addr ext_addr;
            vp_udma_hyper_timing_cfg timing_cfg;
            vp_udma_hyper_mba0 mba0;
            vp_udma_hyper_mba1 mba1;
            vp_udma_hyper_device device;
            vp_udma_hyper_ospi_cmd ospi_cmd;
            vp_udma_hyper_ospi_alter ospi_alter;
            vp_udma_hyper_ospi_cfg ospi_cfg;
            vp_udma_hyper_ospi_csn ospi_csn;
            vp_udma_hyper_ospi_jedec_reset ospi_jedec_reset;
            vp_udma_hyper_ospi_ram_opt ospi_ram_opt;
            vp_udma_hyper_ospi_alter_xip ospi_alter_xip;
            vp_udma_hyper_ospi_reg_xip ospi_reg_xip;
            vp_udma_hyper_line_2d line_2d;
            vp_udma_hyper_stride_2d stride_2d;
            vp_udma_hyper_burst_enable burst_enable;
            vp_udma_hyper_irq_en irq_en;
            vp_udma_hyper_clk_div clk_div;
            vp_udma_hyper_status status;
            vp_udma_hyper_sdio_cmd_arg sdio_cmd_arg;
            vp_udma_hyper_sdio_rsp0 sdio_rsp0;
            vp_udma_hyper_sdio_rsp1 sdio_rsp1;
            vp_udma_hyper_sdio_rsp2 sdio_rsp2;
            vp_udma_hyper_sdio_rsp3 sdio_rsp3;
        };

|

.. _udma_hyper_RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+-----------+------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                              Description                               |
    +=====+===+===========+========================================================================+
    |7:0  |R/W|DEST       |Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)    |
    +-----+---+-----------+------------------------------------------------------------------------+
    |15:8 |R/W|DEST_STREAM|Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+-----------+------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_HYPER_RX_DEST_OFFSET                0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_rx_dest_get(uint32_t base);
        static inline void udma_hyper_rx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_RX_DEST_DEST_BIT                                  0
        #define UDMA_HYPER_RX_DEST_DEST_WIDTH                                8
        #define UDMA_HYPER_RX_DEST_DEST_MASK                                 0xff
        #define UDMA_HYPER_RX_DEST_DEST_RESET                                0xff
        
        // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_BIT                           8
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_WIDTH                         8
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_MASK                          0xff00
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_RESET                         0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_RX_DEST_DEST_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_RX_DEST_DEST_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_RX_DEST_DEST_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_RX_DEST_DEST(val)                       ((val) << 0)
        
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_GET(value)          (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_GETS(value)         (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM_SET(value,field)    (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_RX_DEST_DEST_STREAM(val)                ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the RX uDMA channel. Default is 0xFF(channel disabled)
            unsigned int dest_stream     :8 ; // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_rx_dest : public vp::reg_32
        {
        public:
            inline void dest_set(uint32_t value);
            inline uint32_t dest_get();
            inline void dest_stream_set(uint32_t value);
            inline uint32_t dest_stream_get();
        };

|

.. _udma_hyper_TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+-----------+------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                              Description                               |
    +=====+===+===========+========================================================================+
    |7:0  |R/W|DEST       |Stream ID for the TX 2D uDMA channel. Default is 0xFF(channel disabled) |
    +-----+---+-----------+------------------------------------------------------------------------+
    |15:8 |R/W|DEST_STREAM|Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+-----------+------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_HYPER_TX_DEST_OFFSET                0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_tx_dest_get(uint32_t base);
        static inline void udma_hyper_tx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the TX 2D uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_TX_DEST_DEST_BIT                                  0
        #define UDMA_HYPER_TX_DEST_DEST_WIDTH                                8
        #define UDMA_HYPER_TX_DEST_DEST_MASK                                 0xff
        #define UDMA_HYPER_TX_DEST_DEST_RESET                                0xff
        
        // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_BIT                           8
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_WIDTH                         8
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_MASK                          0xff00
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_RESET                         0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TX_DEST_DEST_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_TX_DEST_DEST_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_TX_DEST_DEST_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_TX_DEST_DEST(val)                       ((val) << 0)
        
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_GET(value)          (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_GETS(value)         (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM_SET(value,field)    (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_TX_DEST_DEST_STREAM(val)                ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the TX 2D uDMA channel. Default is 0xFF(channel disabled)
            unsigned int dest_stream     :8 ; // Stream ID for the STREAM uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_tx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_tx_dest : public vp::reg_32
        {
        public:
            inline void dest_set(uint32_t value);
            inline uint32_t dest_get();
            inline void dest_stream_set(uint32_t value);
            inline uint32_t dest_stream_get();
        };

|

.. _udma_hyper_TRANS_MODE:

TRANS_MODE
""""""""""

Configure transaction mode

.. table:: 

    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|         Name         |                                                                                    Description                                                                                    |
    +=====+===+======================+===================================================================================================================================================================================+
    |    0|R/W|AUTO_ENA              |Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN         |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|XIP_EN                |Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN                |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |3:2  |R/W|RESERVED0             |-                                                                                                                                                                                  |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|STREAM_EN             |Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN|
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|AES_STREAM_EN         |Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN                                 |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|AES_STREAM_128_256    |Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN                                 |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|RESERVED1             |-                                                                                                                                                                                  |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    8|R/W|XIP_STREAM_EN         |Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN|
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    9|R/W|XIP_AES_STREAM_EN     |Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN                                 |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   10|R/W|XIP_AES_STREAM_128_256|Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN                                 |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   11|R/W|RESERVED2             |-                                                                                                                                                                                  |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   12|R/W|XIP_HALTED            |Halted XIP refill when in XIP, XIP refill will wait SW unlock this bit.  - 1'b0: XIP_RUNNING - 1'b1: XIP_HALTED                                                                    |
    +-----+---+----------------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure transaction mode
                #define UDMA_HYPER_TRANS_MODE_OFFSET             0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_trans_mode_get(uint32_t base);
        static inline void udma_hyper_trans_mode_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_BIT                           0
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_WIDTH                         1
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_MASK                          0x1
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_RESET                         0x0
        
        // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_BIT                             1
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_WIDTH                           1
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_MASK                            0x2
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_RESET                           0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_BIT                          2
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_WIDTH                        2
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_MASK                         0xc
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_RESET                        0x0
        
        // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_BIT                          4
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_MASK                         0x10
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_RESET                        0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_BIT                      5
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_WIDTH                    1
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_MASK                     0x20
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_RESET                    0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_BIT                 6
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_WIDTH               1
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_MASK                0x40
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_RESET               0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_BIT                          7
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_MASK                         0x80
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_RESET                        0x0
        
        // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_BIT                      8
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_WIDTH                    1
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_MASK                     0x100
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_RESET                    0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_BIT                  9
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_WIDTH                1
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_MASK                 0x200
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_RESET                0x0
        
        // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_BIT             10
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_WIDTH           1
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_MASK            0x400
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_RESET           0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_BIT                          11
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_WIDTH                        1
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_MASK                         0x800
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_RESET                        0x0
        
        // Halted XIP refill when in XIP, XIP refill will wait SW unlock this bit.  - 1'b0: XIP_RUNNING - 1'b1: XIP_HALTED (access: R/W)
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_BIT                         12
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_WIDTH                       1
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_MASK                        0x1000
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_TRANS_MODE_AUTO_ENA(val)                ((val) << 0)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_TRANS_MODE_XIP_EN(val)                  ((val) << 1)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_GET(value)         (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_GETS(value)        (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0_SET(value,field)   (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_TRANS_MODE_RESERVED0(val)               ((val) << 2)
        
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_GET(value)         (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_GETS(value)        (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN_SET(value,field)   (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_TRANS_MODE_STREAM_EN(val)               ((val) << 4)
        
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_GET(value)     (GAP_BEXTRACTU((value),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_GETS(value)    (GAP_BEXTRACT((value),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_EN(val)           ((val) << 5)
        
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define UDMA_HYPER_TRANS_MODE_AES_STREAM_128_256(val)      ((val) << 6)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_GET(value)         (GAP_BEXTRACTU((value),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_GETS(value)        (GAP_BEXTRACT((value),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1_SET(value,field)   (GAP_BINSERT((value),(field),1,7))
        #define UDMA_HYPER_TRANS_MODE_RESERVED1(val)               ((val) << 7)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_GET(value)     (GAP_BEXTRACTU((value),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_GETS(value)    (GAP_BEXTRACT((value),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define UDMA_HYPER_TRANS_MODE_XIP_STREAM_EN(val)           ((val) << 8)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_GET(value) (GAP_BEXTRACTU((value),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_GETS(value) (GAP_BEXTRACT((value),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_EN(val)       ((val) << 9)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_GET(value) (GAP_BEXTRACTU((value),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_GETS(value) (GAP_BEXTRACT((value),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define UDMA_HYPER_TRANS_MODE_XIP_AES_STREAM_128_256(val)  ((val) << 10)
        
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_GET(value)         (GAP_BEXTRACTU((value),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_GETS(value)        (GAP_BEXTRACT((value),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2_SET(value,field)   (GAP_BINSERT((value),(field),1,11))
        #define UDMA_HYPER_TRANS_MODE_RESERVED2(val)               ((val) << 11)
        
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_GET(value)        (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_GETS(value)       (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED_SET(value,field)  (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_TRANS_MODE_XIP_HALTED(val)              ((val) << 12)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int auto_ena        :1 ; // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN
            unsigned int xip_en          :1 ; // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN
            unsigned int reserved0       :2 ; // -
            unsigned int stream_en       :1 ; // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN
            unsigned int aes_stream_en   :1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int aes_stream_128_256:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int reserved1       :1 ; // -
            unsigned int xip_stream_en   :1 ; // Transfer mode in noraml mode use STREAM or not, IP will configure the STREAM UDMA transfer automatically to read / write data from / to memory - 1'b0: STREAM_DIS - 1'b1: STREAM_EN
            unsigned int xip_aes_stream_en:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int xip_aes_stream_128_256:1 ; // Transfer mode in noraml mode use AES STREAM or not, to avoid Read synchronous issue when in AUTO mode - 1'b0: AES_STREAM_DIS - 1'b1: AES_STREAM_EN
            unsigned int reserved2       :1 ; // -
            unsigned int xip_halted      :1 ; // Halted XIP refill when in XIP, XIP refill will wait SW unlock this bit.  - 1'b0: XIP_RUNNING - 1'b1: XIP_HALTED
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_mode_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_trans_mode : public vp::reg_32
        {
        public:
            inline void auto_ena_set(uint32_t value);
            inline uint32_t auto_ena_get();
            inline void xip_en_set(uint32_t value);
            inline uint32_t xip_en_get();
            inline void reserved0_set(uint32_t value);
            inline uint32_t reserved0_get();
            inline void stream_en_set(uint32_t value);
            inline uint32_t stream_en_get();
            inline void aes_stream_en_set(uint32_t value);
            inline uint32_t aes_stream_en_get();
            inline void aes_stream_128_256_set(uint32_t value);
            inline uint32_t aes_stream_128_256_get();
            inline void reserved1_set(uint32_t value);
            inline uint32_t reserved1_get();
            inline void xip_stream_en_set(uint32_t value);
            inline uint32_t xip_stream_en_get();
            inline void xip_aes_stream_en_set(uint32_t value);
            inline uint32_t xip_aes_stream_en_get();
            inline void xip_aes_stream_128_256_set(uint32_t value);
            inline uint32_t xip_aes_stream_128_256_get();
            inline void reserved2_set(uint32_t value);
            inline uint32_t reserved2_get();
            inline void xip_halted_set(uint32_t value);
            inline uint32_t xip_halted_get();
        };

|

.. _udma_hyper_TRANS_ADDR:

TRANS_ADDR
""""""""""

Configure each transaction addr

.. table:: 

    +-----+---+----+----------------------------------------+
    |Bit #|R/W|Name|              Description               |
    +=====+===+====+========================================+
    |31:0 |R/W|ADDR|Transfer addr, only when MODE is in AUTO|
    +-----+---+----+----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure each transaction addr
                #define UDMA_HYPER_TRANS_ADDR_OFFSET             0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_trans_addr_get(uint32_t base);
        static inline void udma_hyper_trans_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer addr, only when MODE is in AUTO (access: R/W)
        #define UDMA_HYPER_TRANS_ADDR_ADDR_BIT                               0
        #define UDMA_HYPER_TRANS_ADDR_ADDR_WIDTH                             32
        #define UDMA_HYPER_TRANS_ADDR_ADDR_MASK                              0xffffffff
        #define UDMA_HYPER_TRANS_ADDR_ADDR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TRANS_ADDR_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_TRANS_ADDR_ADDR(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Transfer addr, only when MODE is in AUTO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_trans_addr : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _udma_hyper_TRANS_SIZE:

TRANS_SIZE
""""""""""

Configure each transaction size

.. table:: 

    +-----+---+----+-------------+
    |Bit #|R/W|Name| Description |
    +=====+===+====+=============+
    |31:0 |R/W|SIZE|Transfer Size|
    +-----+---+----+-------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure each transaction size
                #define UDMA_HYPER_TRANS_SIZE_OFFSET             0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_trans_size_get(uint32_t base);
        static inline void udma_hyper_trans_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer Size (access: R/W)
        #define UDMA_HYPER_TRANS_SIZE_SIZE_BIT                               0
        #define UDMA_HYPER_TRANS_SIZE_SIZE_WIDTH                             32
        #define UDMA_HYPER_TRANS_SIZE_SIZE_MASK                              0xffffffff
        #define UDMA_HYPER_TRANS_SIZE_SIZE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TRANS_SIZE_SIZE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_TRANS_SIZE_SIZE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :32; // Transfer Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_trans_size : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _udma_hyper_TRANS_CFG:

TRANS_CFG
"""""""""

Start  each transaction rx/tx

.. table:: 

    +-----+---+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                Description                                |
    +=====+===+=====+===========================================================================+
    |    0|R/W|RXTX |Transfer type - 1'b0: TX - 1'b1: RX                                        |
    +-----+---+-----+---------------------------------------------------------------------------+
    |    1|W  |VALID|Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start|
    +-----+---+-----+---------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start  each transaction rx/tx
                #define UDMA_HYPER_TRANS_CFG_OFFSET              0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_trans_cfg_get(uint32_t base);
        static inline void udma_hyper_trans_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer type - 1'b0: TX - 1'b1: RX (access: R/W)
        #define UDMA_HYPER_TRANS_CFG_RXTX_BIT                                0
        #define UDMA_HYPER_TRANS_CFG_RXTX_WIDTH                              1
        #define UDMA_HYPER_TRANS_CFG_RXTX_MASK                               0x1
        #define UDMA_HYPER_TRANS_CFG_RXTX_RESET                              0x0
        
        // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start (access: W)
        #define UDMA_HYPER_TRANS_CFG_VALID_BIT                               1
        #define UDMA_HYPER_TRANS_CFG_VALID_WIDTH                             1
        #define UDMA_HYPER_TRANS_CFG_VALID_MASK                              0x2
        #define UDMA_HYPER_TRANS_CFG_VALID_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TRANS_CFG_RXTX_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_TRANS_CFG_RXTX(val)                     ((val) << 0)
        
        #define UDMA_HYPER_TRANS_CFG_VALID_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_TRANS_CFG_VALID(val)                    ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rxtx            :1 ; // Transfer type - 1'b0: TX - 1'b1: RX
            unsigned int valid           :1 ; // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_trans_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_trans_cfg : public vp::reg_32
        {
        public:
            inline void rxtx_set(uint32_t value);
            inline uint32_t rxtx_get();
            inline void valid_set(uint32_t value);
            inline uint32_t valid_get();
        };

|

.. _udma_hyper_OSPI_CMD_XIP:

OSPI_CMD_XIP
""""""""""""

OSPI XIP command and psram command config

.. table:: 

    +-----+---+-----------------+-----------------------+
    |Bit #|R/W|      Name       |      Description      |
    +=====+===+=================+=======================+
    |15:0 |R/W|CMD              |2 Bytes SPI command    |
    +-----+---+-----------------+-----------------------+
    |22:20|R/W|SDIO_CMD_RSP_TYPE|SDIO CMD response type |
    +-----+---+-----------------+-----------------------+
    |29:24|R/W|SDIO_CMD_OP      |SDIO CMD operation code|
    +-----+---+-----------------+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI XIP command and psram command config
                #define UDMA_HYPER_OSPI_CMD_XIP_OFFSET           0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_cmd_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_cmd_xip_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 2 Bytes SPI command (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_BIT                              0
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_WIDTH                            16
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_MASK                             0xffff
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_RESET                            0x0
        
        // SDIO CMD response type (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_BIT                20
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_WIDTH              3
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_MASK               0x700000
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_RESET              0x0
        
        // SDIO CMD operation code (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_BIT                      24
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_WIDTH                    6
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_MASK                     0x3f000000
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_CMD_XIP_CMD(val)                   ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_GET(value) (GAP_BEXTRACTU((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_GETS(value) (GAP_BEXTRACT((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),3,20))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_RSP_TYPE(val)     ((val) << 20)
        
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_GET(value)     (GAP_BEXTRACTU((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_GETS(value)    (GAP_BEXTRACT((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define UDMA_HYPER_OSPI_CMD_XIP_SDIO_CMD_OP(val)           ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd             :16; // 2 Bytes SPI command
            unsigned int padding0:4 ;
            unsigned int sdio_cmd_rsp_type:3 ; // SDIO CMD response type
            unsigned int padding1:1 ;
            unsigned int sdio_cmd_op     :6 ; // SDIO CMD operation code
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cmd_xip_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_cmd_xip : public vp::reg_32
        {
        public:
            inline void cmd_set(uint32_t value);
            inline uint32_t cmd_get();
            inline void sdio_cmd_rsp_type_set(uint32_t value);
            inline uint32_t sdio_cmd_rsp_type_get();
            inline void sdio_cmd_op_set(uint32_t value);
            inline uint32_t sdio_cmd_op_get();
        };

|

.. _udma_hyper_OSPI_CFG_XIP:

OSPI_CFG_XIP
""""""""""""

OSPI XIP configuration

.. table:: 

    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                                                                  Description                                                                                  |
    +=====+===+============+===============================================================================================================================================================================+
    |1:0  |R/W|CMD_SIZE    |Octo SPI command size :  - 2’b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte                                                                                                        |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |6:4  |R/W|ADDR_SIZE   |Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte                                         |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|LINE        |Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])|
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|CMD_DTR_STR |Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                 |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13   |R/W|ADDR_DTR_STR|Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                 |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14   |R/W|DATA_DTR_STR|Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                    |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15   |R/W|DATA_DTR_MSB|Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB                                                                                                                     |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI XIP configuration
                #define UDMA_HYPER_OSPI_CFG_XIP_OFFSET           0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_cfg_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_cfg_xip_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_BIT                         0
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_WIDTH                       2
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_MASK                        0x3
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_RESET                       0x0
        
        // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_BIT                        4
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_WIDTH                      3
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_MASK                       0x70
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_RESET                      0x0
        
        // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_BIT                             8
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_WIDTH                           2
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_MASK                            0x300
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_RESET                           0x0
        
        // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_BIT                      12
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_WIDTH                    1
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_MASK                     0x1000
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_RESET                    0x0
        
        // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_BIT                     13
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_MASK                    0x2000
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_RESET                   0x0
        
        // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_BIT                     14
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_MASK                    0x4000
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_RESET                   0x0
        
        // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_BIT                     15
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_WIDTH                   1
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_MASK                    0x8000
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_GET(value)        (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_GETS(value)       (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE_SET(value,field)  (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_SIZE(val)              ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_GET(value)       (GAP_BEXTRACTU((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_GETS(value)      (GAP_BEXTRACT((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE_SET(value,field) (GAP_BINSERT((value),(field),3,4))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_SIZE(val)             ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_GET(value)            (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_GETS(value)           (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE_SET(value,field)      (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_CFG_XIP_LINE(val)                  ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_GET(value)     (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_GETS(value)    (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_OSPI_CFG_XIP_CMD_DTR_STR(val)           ((val) << 12)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_GET(value)    (GAP_BEXTRACTU((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_GETS(value)   (GAP_BEXTRACT((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,13))
        #define UDMA_HYPER_OSPI_CFG_XIP_ADDR_DTR_STR(val)          ((val) << 13)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_GET(value)    (GAP_BEXTRACTU((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_GETS(value)   (GAP_BEXTRACT((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR_SET(value,field) (GAP_BINSERT((value),(field),1,14))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_STR(val)          ((val) << 14)
        
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_GET(value)    (GAP_BEXTRACTU((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_GETS(value)   (GAP_BEXTRACT((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define UDMA_HYPER_OSPI_CFG_XIP_DATA_DTR_MSB(val)          ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd_size        :2 ; // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte
            unsigned int padding0:2 ;
            unsigned int addr_size       :3 ; // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte
            unsigned int padding1:1 ;
            unsigned int line            :2 ; // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])
            unsigned int padding2:2 ;
            unsigned int cmd_dtr_str     :1 ; // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int addr_dtr_str    :1 ; // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_str    :1 ; // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_msb    :1 ; // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cfg_xip_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_cfg_xip : public vp::reg_32
        {
        public:
            inline void cmd_size_set(uint32_t value);
            inline uint32_t cmd_size_get();
            inline void addr_size_set(uint32_t value);
            inline uint32_t addr_size_get();
            inline void line_set(uint32_t value);
            inline uint32_t line_get();
            inline void cmd_dtr_str_set(uint32_t value);
            inline uint32_t cmd_dtr_str_get();
            inline void addr_dtr_str_set(uint32_t value);
            inline uint32_t addr_dtr_str_get();
            inline void data_dtr_str_set(uint32_t value);
            inline uint32_t data_dtr_str_get();
            inline void data_dtr_msb_set(uint32_t value);
            inline uint32_t data_dtr_msb_get();
        };

|

.. _udma_hyper_EXT_ADDR:

EXT_ADDR
""""""""

Memory access address register.

.. table:: 

    +-----+---+----------+-------------------------------+
    |Bit #|R/W|   Name   |          Description          |
    +=====+===+==========+===============================+
    |30:0 |R/W|SADDR     |Memory access address bitfield.|
    +-----+---+----------+-------------------------------+
    |31   |R/W|REG_ACCESS|Register access flag bitfield. |
    +-----+---+----------+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Memory access address register.
                #define UDMA_HYPER_EXT_ADDR_OFFSET               0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ext_addr_get(uint32_t base);
        static inline void udma_hyper_ext_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Memory access address bitfield. (access: R/W)
        #define UDMA_HYPER_EXT_ADDR_SADDR_BIT                                0
        #define UDMA_HYPER_EXT_ADDR_SADDR_WIDTH                              31
        #define UDMA_HYPER_EXT_ADDR_SADDR_MASK                               0x7fffffff
        #define UDMA_HYPER_EXT_ADDR_SADDR_RESET                              0x0
        
        // Register access flag bitfield. (access: R/W)
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_BIT                           31
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_WIDTH                         1
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_MASK                          0x80000000
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_EXT_ADDR_SADDR_GET(value)               (GAP_BEXTRACTU((value),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR_GETS(value)              (GAP_BEXTRACT((value),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR_SET(value,field)         (GAP_BINSERT((value),(field),31,0))
        #define UDMA_HYPER_EXT_ADDR_SADDR(val)                     ((val) << 0)
        
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_GET(value)          (GAP_BEXTRACTU((value),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_GETS(value)         (GAP_BEXTRACT((value),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS_SET(value,field)    (GAP_BINSERT((value),(field),1,31))
        #define UDMA_HYPER_EXT_ADDR_REG_ACCESS(val)                ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int saddr           :31; // Memory access address bitfield.
            unsigned int reg_access      :1 ; // Register access flag bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ext_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ext_addr : public vp::reg_32
        {
        public:
            inline void saddr_set(uint32_t value);
            inline uint32_t saddr_get();
            inline void reg_access_set(uint32_t value);
            inline uint32_t reg_access_get();
        };

|

.. _udma_hyper_TIMING_CFG:

TIMING_CFG
""""""""""

HYPERBUS and Octo SPI Memory Timing configuration register.

.. table:: 

    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|             Name              |                                                                                                                                                                                              Description                                                                                                                                                                                               |
    +=====+===+===============================+========================================================================================================================================================================================================================================================================================================================================================================================================+
    |4:0  |R/W|LATENCY0                       |Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK|
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:5  |R/W|LATENCY1                       |Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK|
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13:10|R/W|RW_RECOVERY                    |Some HyperBus devices may require a minimum time between the end of a prior transaction and the start of a new access. This time is referred to as Read-Write-Recovery time (tRWR). The master interface must start driving CS# Low only at a time when the CA1 transfer will complete after tRWR is satisfied. - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK               |
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |16:14|R/W|RWDS_DELAY                     |Delay of RWDS for center aligned read: - 3'b000: 0 logic delay - 3'b001: 1 logic delay - 3'b010: 2 logic delay … - 3'b111: 7 logic delay                                                                                                                                                                                                                                                                |
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |17   |R/W|ADDITIONAL_LATENCY_AUTOCHECK_EN|Auto check for RWDS high or low for additional latency : - 1'b0: additional latency no autocheck - 1'b1: additional latency autocheck                                                                                                                                                                                                                                                                   |
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:18|R/W|CS_MAX                         |Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer : - 14'h0000: 1 CK - 14'h0001: 2 CK - 14'h0011: 3 CK - … - 14'h3FFF: 16383 CK                                                                                                                                                                                                                                         |
    +-----+---+-------------------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // HYPERBUS and Octo SPI Memory Timing configuration register.
                #define UDMA_HYPER_TIMING_CFG_OFFSET             0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_timing_cfg_get(uint32_t base);
        static inline void udma_hyper_timing_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_BIT                           0
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_WIDTH                         5
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_MASK                          0x1f
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_RESET                         0x0
        
        // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_BIT                           5
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_WIDTH                         5
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_MASK                          0x3e0
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_RESET                         0x0
        
        // Some HyperBus devices may require a minimum time between the end of a prior transaction and the start of a new access. This time is referred to as Read-Write-Recovery time (tRWR). The master interface must start driving CS# Low only at a time when the CA1 transfer will complete after tRWR is satisfied. - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_BIT                        10
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_WIDTH                      4
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_MASK                       0x3c00
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_RESET                      0x0
        
        // Delay of RWDS for center aligned read: - 3'b000: 0 logic delay - 3'b001: 1 logic delay - 3'b010: 2 logic delay  - 3'b111: 7 logic delay (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_BIT                         14
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_WIDTH                       3
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_MASK                        0x1c000
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_RESET                       0x0
        
        // Auto check for RWDS high or low for additional latency : - 1'b0: additional latency no autocheck - 1'b1: additional latency autocheck (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_BIT    17
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_WIDTH  1
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_MASK   0x20000
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_RESET  0x0
        
        // Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer : - 14'h0000: 1 CK - 14'h0001: 2 CK - 14'h0011: 3 CK -  - 14'h3FFF: 16383 CK (access: R/W)
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_BIT                             18
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_WIDTH                           14
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_MASK                            0xfffc0000
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_RESET                           0x100

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_GET(value)          (GAP_BEXTRACTU((value),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_GETS(value)         (GAP_BEXTRACT((value),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0_SET(value,field)    (GAP_BINSERT((value),(field),5,0))
        #define UDMA_HYPER_TIMING_CFG_LATENCY0(val)                ((val) << 0)
        
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_GET(value)          (GAP_BEXTRACTU((value),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_GETS(value)         (GAP_BEXTRACT((value),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1_SET(value,field)    (GAP_BINSERT((value),(field),5,5))
        #define UDMA_HYPER_TIMING_CFG_LATENCY1(val)                ((val) << 5)
        
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_GET(value)       (GAP_BEXTRACTU((value),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_GETS(value)      (GAP_BEXTRACT((value),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY_SET(value,field) (GAP_BINSERT((value),(field),4,10))
        #define UDMA_HYPER_TIMING_CFG_RW_RECOVERY(val)             ((val) << 10)
        
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_GET(value)        (GAP_BEXTRACTU((value),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_GETS(value)       (GAP_BEXTRACT((value),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY_SET(value,field)  (GAP_BINSERT((value),(field),3,14))
        #define UDMA_HYPER_TIMING_CFG_RWDS_DELAY(val)              ((val) << 14)
        
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_GET(value) (GAP_BEXTRACTU((value),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_GETS(value) (GAP_BEXTRACT((value),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_HYPER_TIMING_CFG_ADDITIONAL_LATENCY_AUTOCHECK_EN(val) ((val) << 17)
        
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_GET(value)            (GAP_BEXTRACTU((value),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_GETS(value)           (GAP_BEXTRACT((value),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX_SET(value,field)      (GAP_BINSERT((value),(field),14,18))
        #define UDMA_HYPER_TIMING_CFG_CS_MAX(val)                  ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int latency0        :5 ; // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 0. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int latency1        :5 ; // Latency Cycle value for both HyperRAM and HyperFLASH for chip select 1. When using HyperRAM memory, this bit should be set to the same value as the read latency in configuration register of HyperRAM memory the read latency in configuration register of HyperRAM memory. For SPI, is the dummy cycle after ADDRESS stage : - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int rw_recovery     :4 ; // Some HyperBus devices may require a minimum time between the end of a prior transaction and the start of a new access. This time is referred to as Read-Write-Recovery time (tRWR). The master interface must start driving CS# Low only at a time when the CA1 transfer will complete after tRWR is satisfied. - 5'b00000: 0 CK - 5'b00001: 1 CK - 5'b000001: 2 CK ... - 5'b11111: 31 CK
            unsigned int rwds_delay      :3 ; // Delay of RWDS for center aligned read: - 3'b000: 0 logic delay - 3'b001: 1 logic delay - 3'b010: 2 logic delay  - 3'b111: 7 logic delay
            unsigned int additional_latency_autocheck_en:1 ; // Auto check for RWDS high or low for additional latency : - 1'b0: additional latency no autocheck - 1'b1: additional latency autocheck
            unsigned int cs_max          :14; // Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer : - 14'h0000: 1 CK - 14'h0001: 2 CK - 14'h0011: 3 CK -  - 14'h3FFF: 16383 CK
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_timing_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_timing_cfg : public vp::reg_32
        {
        public:
            inline void latency0_set(uint32_t value);
            inline uint32_t latency0_get();
            inline void latency1_set(uint32_t value);
            inline uint32_t latency1_get();
            inline void rw_recovery_set(uint32_t value);
            inline uint32_t rw_recovery_get();
            inline void rwds_delay_set(uint32_t value);
            inline uint32_t rwds_delay_get();
            inline void additional_latency_autocheck_en_set(uint32_t value);
            inline uint32_t additional_latency_autocheck_en_get();
            inline void cs_max_set(uint32_t value);
            inline uint32_t cs_max_get();
        };

|

.. _udma_hyper_MBA0:

MBA0
""""

Device start address register.

.. table:: 

    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                                                                                                                                                                        Description                                                                                                                                                                                                                         |
    +=====+===+========+============================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |23:0 |R/W|reserved|-                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |30:24|R/W|MBA0    |Memory Base Address 0 for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CS1 = 0;  else CS0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CS0 = 0;  else CS1 = 0;|
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Device start address register.
                #define UDMA_HYPER_MBA0_OFFSET                   0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_mba0_get(uint32_t base);
        static inline void udma_hyper_mba0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // - (access: R/W)
        #define UDMA_HYPER_MBA0_RESERVED_BIT                                 0
        #define UDMA_HYPER_MBA0_RESERVED_WIDTH                               24
        #define UDMA_HYPER_MBA0_RESERVED_MASK                                0xffffff
        #define UDMA_HYPER_MBA0_RESERVED_RESET                               0x0
        
        // Memory Base Address 0 for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CS1 = 0;  else CS0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CS0 = 0;  else CS1 = 0; (access: R/W)
        #define UDMA_HYPER_MBA0_MBA0_BIT                                     24
        #define UDMA_HYPER_MBA0_MBA0_WIDTH                                   7
        #define UDMA_HYPER_MBA0_MBA0_MASK                                    0x7f000000
        #define UDMA_HYPER_MBA0_MBA0_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_MBA0_RESERVED_GET(value)                (GAP_BEXTRACTU((value),24,0))
        #define UDMA_HYPER_MBA0_RESERVED_GETS(value)               (GAP_BEXTRACT((value),24,0))
        #define UDMA_HYPER_MBA0_RESERVED_SET(value,field)          (GAP_BINSERT((value),(field),24,0))
        #define UDMA_HYPER_MBA0_RESERVED(val)                      ((val) << 0)
        
        #define UDMA_HYPER_MBA0_MBA0_GET(value)                    (GAP_BEXTRACTU((value),7,24))
        #define UDMA_HYPER_MBA0_MBA0_GETS(value)                   (GAP_BEXTRACT((value),7,24))
        #define UDMA_HYPER_MBA0_MBA0_SET(value,field)              (GAP_BINSERT((value),(field),7,24))
        #define UDMA_HYPER_MBA0_MBA0(val)                          ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int reserved        :24; // -
            unsigned int mba0            :7 ; // Memory Base Address 0 for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CS1 = 0;  else CS0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CS0 = 0;  else CS1 = 0;
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_mba0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_mba0 : public vp::reg_32
        {
        public:
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void mba0_set(uint32_t value);
            inline uint32_t mba0_get();
        };

|

.. _udma_hyper_MBA1:

MBA1
""""

Device start address register.

.. table:: 

    +-----+---+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                                                                                                                                                                          Description                                                                                                                                                                                                                          |
    +=====+===+========+===============================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |23:0 |R/W|reserved|-                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
    +-----+---+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |30:24|R/W|MBA1    |Memory Base Address  for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CSn1 = 0;  else CSn0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CSn0 = 0;  else CSn1 = 0;|
    +-----+---+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Device start address register.
                #define UDMA_HYPER_MBA1_OFFSET                   0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_mba1_get(uint32_t base);
        static inline void udma_hyper_mba1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // - (access: R/W)
        #define UDMA_HYPER_MBA1_RESERVED_BIT                                 0
        #define UDMA_HYPER_MBA1_RESERVED_WIDTH                               24
        #define UDMA_HYPER_MBA1_RESERVED_MASK                                0xffffff
        #define UDMA_HYPER_MBA1_RESERVED_RESET                               0x0
        
        // Memory Base Address  for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CSn1 = 0;  else CSn0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CSn0 = 0;  else CSn1 = 0; (access: R/W)
        #define UDMA_HYPER_MBA1_MBA1_BIT                                     24
        #define UDMA_HYPER_MBA1_MBA1_WIDTH                                   7
        #define UDMA_HYPER_MBA1_MBA1_MASK                                    0x7f000000
        #define UDMA_HYPER_MBA1_MBA1_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_MBA1_RESERVED_GET(value)                (GAP_BEXTRACTU((value),24,0))
        #define UDMA_HYPER_MBA1_RESERVED_GETS(value)               (GAP_BEXTRACT((value),24,0))
        #define UDMA_HYPER_MBA1_RESERVED_SET(value,field)          (GAP_BINSERT((value),(field),24,0))
        #define UDMA_HYPER_MBA1_RESERVED(val)                      ((val) << 0)
        
        #define UDMA_HYPER_MBA1_MBA1_GET(value)                    (GAP_BEXTRACTU((value),7,24))
        #define UDMA_HYPER_MBA1_MBA1_GETS(value)                   (GAP_BEXTRACT((value),7,24))
        #define UDMA_HYPER_MBA1_MBA1_SET(value,field)              (GAP_BINSERT((value),(field),7,24))
        #define UDMA_HYPER_MBA1_MBA1(val)                          ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int reserved        :24; // -
            unsigned int mba1            :7 ; // Memory Base Address  for both RAM and FLASH bitfield. The base address of addressable region to each memory is set up. Since register can be set in 16M bytes boundary, lower 24 bit is fixed to 0. MBA0 can be greater than MBA1, the chip select which decided by the relationship among MBA0, MBA1, and EXT_ADDR.  - MBA0 &lt; MBA1, if (MBA1 &lt;= EXT_ADDR) CSn1 = 0;  else CSn0 = 0; - MBA0 &gt; MBA1, if (MBA0 &lt;= EXT_ADDR) CSn0 = 0;  else CSn1 = 0;
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_mba1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_mba1 : public vp::reg_32
        {
        public:
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void mba1_set(uint32_t value);
            inline uint32_t mba1_get();
        };

|

.. _udma_hyper_DEVICE:

DEVICE
""""""

Device type register(RAM or FLASH).

.. table:: 

    +-----+---+----+------------------------------------------------------------+
    |Bit #|R/W|Name|                        Description                         |
    +=====+===+====+============================================================+
    |    0|R/W|TYPE|Device type : - 1'b00: Octo/ Single SPI - 1'b01: HYPERBUS   |
    +-----+---+----+------------------------------------------------------------+
    |    1|R/W|DT0 |When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH|
    +-----+---+----+------------------------------------------------------------+
    |    2|R/W|DT1 |When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH|
    +-----+---+----+------------------------------------------------------------+
    |    3|R/W|SDIO|SDIO mode                                                   |
    +-----+---+----+------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Device type register(RAM or FLASH).
                #define UDMA_HYPER_DEVICE_OFFSET                 0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_device_get(uint32_t base);
        static inline void udma_hyper_device_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Device type : - 1'b00: Octo/ Single SPI - 1'b01: HYPERBUS (access: R/W)
        #define UDMA_HYPER_DEVICE_TYPE_BIT                                   0
        #define UDMA_HYPER_DEVICE_TYPE_WIDTH                                 1
        #define UDMA_HYPER_DEVICE_TYPE_MASK                                  0x1
        #define UDMA_HYPER_DEVICE_TYPE_RESET                                 0x0
        
        // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH (access: R/W)
        #define UDMA_HYPER_DEVICE_DT0_BIT                                    1
        #define UDMA_HYPER_DEVICE_DT0_WIDTH                                  1
        #define UDMA_HYPER_DEVICE_DT0_MASK                                   0x2
        #define UDMA_HYPER_DEVICE_DT0_RESET                                  0x0
        
        // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH (access: R/W)
        #define UDMA_HYPER_DEVICE_DT1_BIT                                    2
        #define UDMA_HYPER_DEVICE_DT1_WIDTH                                  1
        #define UDMA_HYPER_DEVICE_DT1_MASK                                   0x4
        #define UDMA_HYPER_DEVICE_DT1_RESET                                  0x0
        
        // SDIO mode (access: R/W)
        #define UDMA_HYPER_DEVICE_SDIO_BIT                                   3
        #define UDMA_HYPER_DEVICE_SDIO_WIDTH                                 1
        #define UDMA_HYPER_DEVICE_SDIO_MASK                                  0x8
        #define UDMA_HYPER_DEVICE_SDIO_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_DEVICE_TYPE_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_DEVICE_TYPE_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_DEVICE_TYPE_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_DEVICE_TYPE(val)                        ((val) << 0)
        
        #define UDMA_HYPER_DEVICE_DT0_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_DEVICE_DT0_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_DEVICE_DT0_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_DEVICE_DT0(val)                         ((val) << 1)
        
        #define UDMA_HYPER_DEVICE_DT1_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_DEVICE_DT1_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_DEVICE_DT1_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_DEVICE_DT1(val)                         ((val) << 2)
        
        #define UDMA_HYPER_DEVICE_SDIO_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_DEVICE_SDIO_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_DEVICE_SDIO_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_DEVICE_SDIO(val)                        ((val) << 3)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int type            :1 ; // Device type : - 1'b00: Octo/ Single SPI - 1'b01: HYPERBUS
            unsigned int dt0             :1 ; // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH
            unsigned int dt1             :1 ; // When in HYPERBUS mode :  - 1'b0: HYPERRAM -  1'b1: HYPERLASH
            unsigned int sdio            :1 ; // SDIO mode
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_device_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_device : public vp::reg_32
        {
        public:
            inline void type_set(uint32_t value);
            inline uint32_t type_get();
            inline void dt0_set(uint32_t value);
            inline uint32_t dt0_get();
            inline void dt1_set(uint32_t value);
            inline uint32_t dt1_get();
            inline void sdio_set(uint32_t value);
            inline uint32_t sdio_get();
        };

|

.. _udma_hyper_OSPI_CMD:

OSPI_CMD
""""""""

OSPI command and psram command config

.. table:: 

    +-----+---+-----------------+-----------------------+
    |Bit #|R/W|      Name       |      Description      |
    +=====+===+=================+=======================+
    |15:0 |R/W|CMD              |2 Bytes SPI command    |
    +-----+---+-----------------+-----------------------+
    |22:20|R/W|SDIO_CMD_RSP_TYPE|SDIO CMD response type |
    +-----+---+-----------------+-----------------------+
    |29:24|R/W|SDIO_CMD_OP      |SDIO CMD operation code|
    +-----+---+-----------------+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI command and psram command config
                #define UDMA_HYPER_OSPI_CMD_OFFSET               0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_cmd_get(uint32_t base);
        static inline void udma_hyper_ospi_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 2 Bytes SPI command (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_CMD_BIT                                  0
        #define UDMA_HYPER_OSPI_CMD_CMD_WIDTH                                16
        #define UDMA_HYPER_OSPI_CMD_CMD_MASK                                 0xffff
        #define UDMA_HYPER_OSPI_CMD_CMD_RESET                                0x0
        
        // SDIO CMD response type (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_BIT                    20
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_WIDTH                  3
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_MASK                   0x700000
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_RESET                  0x0
        
        // SDIO CMD operation code (access: R/W)
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_BIT                          24
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_WIDTH                        6
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_MASK                         0x3f000000
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_CMD_CMD_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_CMD_CMD(val)                       ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_GET(value)   (GAP_BEXTRACTU((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_GETS(value)  (GAP_BEXTRACT((value),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),3,20))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_RSP_TYPE(val)         ((val) << 20)
        
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_GET(value)         (GAP_BEXTRACTU((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_GETS(value)        (GAP_BEXTRACT((value),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP_SET(value,field)   (GAP_BINSERT((value),(field),6,24))
        #define UDMA_HYPER_OSPI_CMD_SDIO_CMD_OP(val)               ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd             :16; // 2 Bytes SPI command
            unsigned int padding0:4 ;
            unsigned int sdio_cmd_rsp_type:3 ; // SDIO CMD response type
            unsigned int padding1:1 ;
            unsigned int sdio_cmd_op     :6 ; // SDIO CMD operation code
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_cmd : public vp::reg_32
        {
        public:
            inline void cmd_set(uint32_t value);
            inline uint32_t cmd_get();
            inline void sdio_cmd_rsp_type_set(uint32_t value);
            inline uint32_t sdio_cmd_rsp_type_get();
            inline void sdio_cmd_op_set(uint32_t value);
            inline uint32_t sdio_cmd_op_get();
        };

|

.. _udma_hyper_OSPI_ALTER:

OSPI_ALTER
""""""""""

OSPI alternative 2 bytes

.. table:: 

    +-----+---+-----+-----------------------+
    |Bit #|R/W|Name |      Description      |
    +=====+===+=====+=======================+
    |15:0 |R/W|MODE0|2 Bytes SPI alternative|
    +-----+---+-----+-----------------------+
    |31:16|R/W|MODE1|2 Bytes SPI alternative|
    +-----+---+-----+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI alternative 2 bytes
                #define UDMA_HYPER_OSPI_ALTER_OFFSET             0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_alter_get(uint32_t base);
        static inline void udma_hyper_ospi_alter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_MODE0_BIT                              0
        #define UDMA_HYPER_OSPI_ALTER_MODE0_WIDTH                            16
        #define UDMA_HYPER_OSPI_ALTER_MODE0_MASK                             0xffff
        #define UDMA_HYPER_OSPI_ALTER_MODE0_RESET                            0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_MODE1_BIT                              16
        #define UDMA_HYPER_OSPI_ALTER_MODE1_WIDTH                            16
        #define UDMA_HYPER_OSPI_ALTER_MODE1_MASK                             0xffff0000
        #define UDMA_HYPER_OSPI_ALTER_MODE1_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_ALTER_MODE0_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_ALTER_MODE0(val)                   ((val) << 0)
        
        #define UDMA_HYPER_OSPI_ALTER_MODE1_GET(value)             (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1_GETS(value)            (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1_SET(value,field)       (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_OSPI_ALTER_MODE1(val)                   ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mode0           :16; // 2 Bytes SPI alternative
            unsigned int mode1           :16; // 2 Bytes SPI alternative
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_alter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_alter : public vp::reg_32
        {
        public:
            inline void mode0_set(uint32_t value);
            inline uint32_t mode0_get();
            inline void mode1_set(uint32_t value);
            inline uint32_t mode1_get();
        };

|

.. _udma_hyper_OSPI_CFG:

OSPI_CFG
""""""""

OSPI configuration

.. table:: 

    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                                                                  Description                                                                                  |
    +=====+===+============+===============================================================================================================================================================================+
    |1:0  |R/W|CMD_SIZE    |Octo SPI command size :  - 2’b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte                                                                                                        |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |6:4  |R/W|ADDR_SIZE   |Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte                                         |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|LINE        |Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])|
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|CMD_DTR_STR |Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                 |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13   |R/W|ADDR_DTR_STR|Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                 |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14   |R/W|DATA_DTR_STR|Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode                                                                                                    |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15   |R/W|DATA_DTR_MSB|Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB                                                                                                                     |
    +-----+---+------------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI configuration
                #define UDMA_HYPER_OSPI_CFG_OFFSET               0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_cfg_get(uint32_t base);
        static inline void udma_hyper_ospi_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_BIT                             0
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_WIDTH                           2
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_MASK                            0x3
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_RESET                           0x0
        
        // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_BIT                            4
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_WIDTH                          3
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_MASK                           0x70
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_RESET                          0x0
        
        // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1]) (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_LINE_BIT                                 8
        #define UDMA_HYPER_OSPI_CFG_LINE_WIDTH                               2
        #define UDMA_HYPER_OSPI_CFG_LINE_MASK                                0x300
        #define UDMA_HYPER_OSPI_CFG_LINE_RESET                               0x0
        
        // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_BIT                          12
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_WIDTH                        1
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_MASK                         0x1000
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_RESET                        0x0
        
        // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_BIT                         13
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_MASK                        0x2000
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_RESET                       0x0
        
        // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_BIT                         14
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_MASK                        0x4000
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_RESET                       0x0
        
        // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB (access: R/W)
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_BIT                         15
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_WIDTH                       1
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_MASK                        0x8000
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_GET(value)            (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_GETS(value)           (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_CFG_CMD_SIZE(val)                  ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_GET(value)           (GAP_BEXTRACTU((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_GETS(value)          (GAP_BEXTRACT((value),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE_SET(value,field)     (GAP_BINSERT((value),(field),3,4))
        #define UDMA_HYPER_OSPI_CFG_ADDR_SIZE(val)                 ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CFG_LINE_GET(value)                (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE_GETS(value)               (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE_SET(value,field)          (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_CFG_LINE(val)                      ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_GET(value)         (GAP_BEXTRACTU((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_GETS(value)        (GAP_BEXTRACT((value),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR_SET(value,field)   (GAP_BINSERT((value),(field),1,12))
        #define UDMA_HYPER_OSPI_CFG_CMD_DTR_STR(val)               ((val) << 12)
        
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_GET(value)        (GAP_BEXTRACTU((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_GETS(value)       (GAP_BEXTRACT((value),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR_SET(value,field)  (GAP_BINSERT((value),(field),1,13))
        #define UDMA_HYPER_OSPI_CFG_ADDR_DTR_STR(val)              ((val) << 13)
        
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_GET(value)        (GAP_BEXTRACTU((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_GETS(value)       (GAP_BEXTRACT((value),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR_SET(value,field)  (GAP_BINSERT((value),(field),1,14))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_STR(val)              ((val) << 14)
        
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_GET(value)        (GAP_BEXTRACTU((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_GETS(value)       (GAP_BEXTRACT((value),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB_SET(value,field)  (GAP_BINSERT((value),(field),1,15))
        #define UDMA_HYPER_OSPI_CFG_DATA_DTR_MSB(val)              ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd_size        :2 ; // Octo SPI command size :  - 2b0: 0 byte -  2'b1: 1 byte -  2'b2: 2 byte
            unsigned int padding0:2 ;
            unsigned int addr_size       :3 ; // Octo SPI address size :  - 3'b000: 0 byte (Jump ADDRESS stage) -  3'b001: 1 byte -  3'b010: 2 byte -  3'b011: 3 byte -  3'b100: 4 byte
            unsigned int padding1:1 ;
            unsigned int line            :2 ; // Octo SPI line number for Single SPI or Octo SPI:  - 2'b00: 8 line for Octo SPI -  2'b01: 4 line for QUAD SPI (sdio{3:0]) -  2'b10: 1 line for Single SPI (SI : dq[0] SO: dq[1])
            unsigned int padding2:2 ;
            unsigned int cmd_dtr_str     :1 ; // Octo SPI command ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int addr_dtr_str    :1 ; // Octo SPI address ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_str    :1 ; // Octo SPI data ddr mode or single mode :  - 1'b0: DTR mode -  1'b1: STR mode
            unsigned int data_dtr_msb    :1 ; // Octo SPI data ddr mode data MSB:  - 1'b0: LSB -  1'b1: MSB
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_cfg : public vp::reg_32
        {
        public:
            inline void cmd_size_set(uint32_t value);
            inline uint32_t cmd_size_get();
            inline void addr_size_set(uint32_t value);
            inline uint32_t addr_size_get();
            inline void line_set(uint32_t value);
            inline uint32_t line_get();
            inline void cmd_dtr_str_set(uint32_t value);
            inline uint32_t cmd_dtr_str_get();
            inline void addr_dtr_str_set(uint32_t value);
            inline uint32_t addr_dtr_str_get();
            inline void data_dtr_str_set(uint32_t value);
            inline uint32_t data_dtr_str_get();
            inline void data_dtr_msb_set(uint32_t value);
            inline uint32_t data_dtr_msb_get();
        };

|

.. _udma_hyper_OSPI_CSN:

OSPI_CSN
""""""""

OSPI chip select configuration

.. table:: 

    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|       Name       |                                                                            Description                                                                             |
    +=====+===+==================+====================================================================================================================================================================+
    |    0|R/W|INDEX             |Octo SPI chip select index controlled by user  :  - 1'b0: CSN0 -  1'b1: CSN1                                                                                        |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|AUTO_EN           |Octo SPI chip select controlled by IP automatically  :  - 1'b0: IP control CSN according to index -  1'b1: : IP control CSN according to address range automatically|
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |3:2  |R/W|reserved          |-                                                                                                                                                                   |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|DIRECT_CTRL       |Octo SPI chip select controlled by user enable GPIO mode :  - 1'b0: IP control CSN according to index -  1'b1: USER control CSN in GPIO mode                        |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|VALUE             |Octo SPI chip select value controlled by user  :  - 1'b0: HIGH -  1'b1: : Low                                                                                       |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|SDIO_DATA_QUAD    |SDIO data quad enable  :  - 1'b0: Disable -  1'b1: : Enable                                                                                                         |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|SDIO_DATA_QUAD_DDR|SDIO data quad ddr enable  :  - 1'b0: Disable -  1'b1: : Enable                                                                                                     |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|SDIO_BLOCK_NUM    |SDIO data block number                                                                                                                                              |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |25:16|R/W|SDIO_BLOCK_SIZE   |SDIO data block size                                                                                                                                                |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   26|R/W|SDIO_AUTO_STOP    |SDIO enable HW auto stop  after multiple read and write :  - 1'b0: Disable -  1'b1: : Enable                                                                        |
    +-----+---+------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI chip select configuration
                #define UDMA_HYPER_OSPI_CSN_OFFSET               0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_csn_get(uint32_t base);
        static inline void udma_hyper_ospi_csn_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI chip select index controlled by user  :  - 1'b0: CSN0 -  1'b1: CSN1 (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_INDEX_BIT                                0
        #define UDMA_HYPER_OSPI_CSN_INDEX_WIDTH                              1
        #define UDMA_HYPER_OSPI_CSN_INDEX_MASK                               0x1
        #define UDMA_HYPER_OSPI_CSN_INDEX_RESET                              0x0
        
        // Octo SPI chip select controlled by IP automatically  :  - 1'b0: IP control CSN according to index -  1'b1: : IP control CSN according to address range automatically (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_BIT                              1
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_WIDTH                            1
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_MASK                             0x2
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_RESET                            0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_RESERVED_BIT                             2
        #define UDMA_HYPER_OSPI_CSN_RESERVED_WIDTH                           2
        #define UDMA_HYPER_OSPI_CSN_RESERVED_MASK                            0xc
        #define UDMA_HYPER_OSPI_CSN_RESERVED_RESET                           0x0
        
        // Octo SPI chip select controlled by user enable GPIO mode :  - 1'b0: IP control CSN according to index -  1'b1: USER control CSN in GPIO mode (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_BIT                          4
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_WIDTH                        1
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_MASK                         0x10
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_RESET                        0x0
        
        // Octo SPI chip select value controlled by user  :  - 1'b0: HIGH -  1'b1: : Low (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_VALUE_BIT                                5
        #define UDMA_HYPER_OSPI_CSN_VALUE_WIDTH                              1
        #define UDMA_HYPER_OSPI_CSN_VALUE_MASK                               0x20
        #define UDMA_HYPER_OSPI_CSN_VALUE_RESET                              0x0
        
        // SDIO data quad enable  :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_BIT                       6
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_WIDTH                     1
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_MASK                      0x40
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_RESET                     0x0
        
        // SDIO data quad ddr enable  :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_BIT                   7
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_WIDTH                 1
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_MASK                  0x80
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_RESET                 0x0
        
        // SDIO data block number (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_BIT                       8
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_WIDTH                     8
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_MASK                      0xff00
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_RESET                     0x0
        
        // SDIO data block size (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_BIT                      16
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_WIDTH                    10
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_MASK                     0x3ff0000
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_RESET                    0x0
        
        // SDIO enable HW auto stop  after multiple read and write :  - 1'b0: Disable -  1'b1: : Enable (access: R/W)
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_BIT                       26
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_WIDTH                     1
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_MASK                      0x4000000
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_RESET                     0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_CSN_INDEX_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_OSPI_CSN_INDEX(val)                     ((val) << 0)
        
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_OSPI_CSN_AUTO_EN(val)                   ((val) << 1)
        
        #define UDMA_HYPER_OSPI_CSN_RESERVED_GET(value)            (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED_GETS(value)           (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED_SET(value,field)      (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_OSPI_CSN_RESERVED(val)                  ((val) << 2)
        
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_GET(value)         (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_GETS(value)        (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL_SET(value,field)   (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_OSPI_CSN_DIRECT_CTRL(val)               ((val) << 4)
        
        #define UDMA_HYPER_OSPI_CSN_VALUE_GET(value)               (GAP_BEXTRACTU((value),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE_GETS(value)              (GAP_BEXTRACT((value),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE_SET(value,field)         (GAP_BINSERT((value),(field),1,5))
        #define UDMA_HYPER_OSPI_CSN_VALUE(val)                     ((val) << 5)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD(val)            ((val) << 6)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_GET(value)  (GAP_BEXTRACTU((value),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define UDMA_HYPER_OSPI_CSN_SDIO_DATA_QUAD_DDR(val)        ((val) << 7)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_NUM(val)            ((val) << 8)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_GET(value)     (GAP_BEXTRACTU((value),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_GETS(value)    (GAP_BEXTRACT((value),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE_SET(value,field) (GAP_BINSERT((value),(field),10,16))
        #define UDMA_HYPER_OSPI_CSN_SDIO_BLOCK_SIZE(val)           ((val) << 16)
        
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_GET(value)      (GAP_BEXTRACTU((value),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_GETS(value)     (GAP_BEXTRACT((value),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,26))
        #define UDMA_HYPER_OSPI_CSN_SDIO_AUTO_STOP(val)            ((val) << 26)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int index           :1 ; // Octo SPI chip select index controlled by user  :  - 1'b0: CSN0 -  1'b1: CSN1
            unsigned int auto_en         :1 ; // Octo SPI chip select controlled by IP automatically  :  - 1'b0: IP control CSN according to index -  1'b1: : IP control CSN according to address range automatically
            unsigned int reserved        :2 ; // -
            unsigned int direct_ctrl     :1 ; // Octo SPI chip select controlled by user enable GPIO mode :  - 1'b0: IP control CSN according to index -  1'b1: USER control CSN in GPIO mode
            unsigned int value           :1 ; // Octo SPI chip select value controlled by user  :  - 1'b0: HIGH -  1'b1: : Low
            unsigned int sdio_data_quad  :1 ; // SDIO data quad enable  :  - 1'b0: Disable -  1'b1: : Enable
            unsigned int sdio_data_quad_ddr:1 ; // SDIO data quad ddr enable  :  - 1'b0: Disable -  1'b1: : Enable
            unsigned int sdio_block_num  :8 ; // SDIO data block number
            unsigned int sdio_block_size :10; // SDIO data block size
            unsigned int sdio_auto_stop  :1 ; // SDIO enable HW auto stop  after multiple read and write :  - 1'b0: Disable -  1'b1: : Enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_csn_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_csn : public vp::reg_32
        {
        public:
            inline void index_set(uint32_t value);
            inline uint32_t index_get();
            inline void auto_en_set(uint32_t value);
            inline uint32_t auto_en_get();
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void direct_ctrl_set(uint32_t value);
            inline uint32_t direct_ctrl_get();
            inline void value_set(uint32_t value);
            inline uint32_t value_get();
            inline void sdio_data_quad_set(uint32_t value);
            inline uint32_t sdio_data_quad_get();
            inline void sdio_data_quad_ddr_set(uint32_t value);
            inline uint32_t sdio_data_quad_ddr_get();
            inline void sdio_block_num_set(uint32_t value);
            inline uint32_t sdio_block_num_get();
            inline void sdio_block_size_set(uint32_t value);
            inline uint32_t sdio_block_size_get();
            inline void sdio_auto_stop_set(uint32_t value);
            inline uint32_t sdio_auto_stop_get();
        };

|

.. _udma_hyper_OSPI_JEDEC_RESET:

OSPI_JEDEC_RESET
""""""""""""""""

OSPI JEDEC Hardware Reset, user can control sdo0 manually

.. table:: 

    +-----+---+--------------------+--------------------------------------------------------------------------------------------------+
    |Bit #|R/W|        Name        |                                           Description                                            |
    +=====+===+====================+==================================================================================================+
    |    0|R/W|USER_CTRL_SDO0_EN   |Octo SPI chip in JEDEC reset mode enable :  - 1'b0: JEDEC reset disable -  1'b1: USER control sdo0|
    +-----+---+--------------------+--------------------------------------------------------------------------------------------------+
    |    1|R/W|USER_CTRL_SDO0_VALUE|Octo SPI chip in JEDEC reset mode, sdo0 value                                                     |
    +-----+---+--------------------+--------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI JEDEC Hardware Reset, user can control sdo0 manually
                #define UDMA_HYPER_OSPI_JEDEC_RESET_OFFSET       0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_jedec_reset_get(uint32_t base);
        static inline void udma_hyper_ospi_jedec_reset_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI chip in JEDEC reset mode enable :  - 1'b0: JEDEC reset disable -  1'b1: USER control sdo0 (access: R/W)
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_BIT            0
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_WIDTH          1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_MASK           0x1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_RESET          0x0
        
        // Octo SPI chip in JEDEC reset mode, sdo0 value (access: R/W)
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_BIT         1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_WIDTH       1
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_MASK        0x2
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_RESET       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_EN(val) ((val) << 0)
        
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_OSPI_JEDEC_RESET_USER_CTRL_SDO0_VALUE(val) ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int user_ctrl_sdo0_en:1 ; // Octo SPI chip in JEDEC reset mode enable :  - 1'b0: JEDEC reset disable -  1'b1: USER control sdo0
            unsigned int user_ctrl_sdo0_value:1 ; // Octo SPI chip in JEDEC reset mode, sdo0 value
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_jedec_reset_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_jedec_reset : public vp::reg_32
        {
        public:
            inline void user_ctrl_sdo0_en_set(uint32_t value);
            inline uint32_t user_ctrl_sdo0_en_get();
            inline void user_ctrl_sdo0_value_set(uint32_t value);
            inline uint32_t user_ctrl_sdo0_value_get();
        };

|

.. _udma_hyper_OSPI_RAM_OPT:

OSPI_RAM_OPT
""""""""""""

OSPI RAM DATA transfer optimisation, only in auto mode

.. table:: 

    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|         Name          |                                                                   Description                                                                   |
    +=====+===+=======================+=================================================================================================================================================+
    |1:0  |R/W|OPT_READ_EN_CS         |Octo SPI RAM optimisation read enable for CS 0, special when no accross boundary rwds latency for each channel :  - 1'b0: disable -  1'b1: enable|
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |3:2  |R/W|REAL_ADDR_EN           |Octo SPI or single SPI which use real address instead of address/2 for each channel :  - 1'b0: disable -  1'b1: enable                           |
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |5:4  |R/W|PSRAM_READ_BIT         |PSRAM CMD automatically reform - CMD bit[47] R/W# - Read is 1 or 0 for each channel                                                              |
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:6  |R/W|PSRAM_CMD_EN           |PSRAM CMD automatically reform enable for each channel                                                                                           |
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|PSRAM_ADDR_EVEN        |PSRAM even address reform for each channel                                                                                                       |
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+
    |11:10|R/W|PSRAM_CROSS_BOUNDARY_EN|PSRAM cross boundary access optimisation enable for each channel :  - 1'b0: disable -  1'b1: enable                                              |
    +-----+---+-----------------------+-------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI RAM DATA transfer optimisation, only in auto mode
                #define UDMA_HYPER_OSPI_RAM_OPT_OFFSET           0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_ram_opt_get(uint32_t base);
        static inline void udma_hyper_ospi_ram_opt_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI RAM optimisation read enable for CS 0, special when no accross boundary rwds latency for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_BIT                   0
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_WIDTH                 2
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_MASK                  0x3
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_RESET                 0x0
        
        // Octo SPI or single SPI which use real address instead of address/2 for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_BIT                     2
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_WIDTH                   2
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_MASK                    0xc
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_RESET                   0x0
        
        // PSRAM CMD automatically reform - CMD bit[47] R/W# - Read is 1 or 0 for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_BIT                   4
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_WIDTH                 2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_MASK                  0x30
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_RESET                 0x0
        
        // PSRAM CMD automatically reform enable for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_BIT                     6
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_WIDTH                   2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_MASK                    0xc0
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_RESET                   0x0
        
        // PSRAM even address reform for each channel (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_BIT                  8
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_WIDTH                2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_MASK                 0x300
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_RESET                0x0
        
        // PSRAM cross boundary access optimisation enable for each channel :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_BIT          10
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_WIDTH        2
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_MASK         0xc00
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_RESET        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_GET(value)  (GAP_BEXTRACTU((value),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_GETS(value) (GAP_BEXTRACT((value),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define UDMA_HYPER_OSPI_RAM_OPT_OPT_READ_EN_CS(val)        ((val) << 0)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_GET(value)    (GAP_BEXTRACTU((value),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_GETS(value)   (GAP_BEXTRACT((value),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN_SET(value,field) (GAP_BINSERT((value),(field),2,2))
        #define UDMA_HYPER_OSPI_RAM_OPT_REAL_ADDR_EN(val)          ((val) << 2)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_GET(value)  (GAP_BEXTRACTU((value),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_GETS(value) (GAP_BEXTRACT((value),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT_SET(value,field) (GAP_BINSERT((value),(field),2,4))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_READ_BIT(val)        ((val) << 4)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_GET(value)    (GAP_BEXTRACTU((value),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_GETS(value)   (GAP_BEXTRACT((value),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN_SET(value,field) (GAP_BINSERT((value),(field),2,6))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CMD_EN(val)          ((val) << 6)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_GET(value) (GAP_BEXTRACTU((value),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_GETS(value) (GAP_BEXTRACT((value),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN_SET(value,field) (GAP_BINSERT((value),(field),2,8))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_ADDR_EVEN(val)       ((val) << 8)
        
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_GET(value) (GAP_BEXTRACTU((value),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_GETS(value) (GAP_BEXTRACT((value),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN_SET(value,field) (GAP_BINSERT((value),(field),2,10))
        #define UDMA_HYPER_OSPI_RAM_OPT_PSRAM_CROSS_BOUNDARY_EN(val) ((val) << 10)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int opt_read_en_cs  :2 ; // Octo SPI RAM optimisation read enable for CS 0, special when no accross boundary rwds latency for each channel :  - 1'b0: disable -  1'b1: enable
            unsigned int real_addr_en    :2 ; // Octo SPI or single SPI which use real address instead of address/2 for each channel :  - 1'b0: disable -  1'b1: enable
            unsigned int psram_read_bit  :2 ; // PSRAM CMD automatically reform - CMD bit[47] R/W# - Read is 1 or 0 for each channel
            unsigned int psram_cmd_en    :2 ; // PSRAM CMD automatically reform enable for each channel
            unsigned int psram_addr_even :2 ; // PSRAM even address reform for each channel
            unsigned int psram_cross_boundary_en:2 ; // PSRAM cross boundary access optimisation enable for each channel :  - 1'b0: disable -  1'b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_ram_opt_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_ram_opt : public vp::reg_32
        {
        public:
            inline void opt_read_en_cs_set(uint32_t value);
            inline uint32_t opt_read_en_cs_get();
            inline void real_addr_en_set(uint32_t value);
            inline uint32_t real_addr_en_get();
            inline void psram_read_bit_set(uint32_t value);
            inline uint32_t psram_read_bit_get();
            inline void psram_cmd_en_set(uint32_t value);
            inline uint32_t psram_cmd_en_get();
            inline void psram_addr_even_set(uint32_t value);
            inline uint32_t psram_addr_even_get();
            inline void psram_cross_boundary_en_set(uint32_t value);
            inline uint32_t psram_cross_boundary_en_get();
        };

|

.. _udma_hyper_OSPI_ALTER_XIP:

OSPI_ALTER_XIP
""""""""""""""

OSPI XIP alternative 2 bytes

.. table:: 

    +-----+---+-----+-----------------------+
    |Bit #|R/W|Name |      Description      |
    +=====+===+=====+=======================+
    |15:0 |R/W|MODE0|2 Bytes SPI alternative|
    +-----+---+-----+-----------------------+
    |31:16|R/W|MODE1|2 Bytes SPI alternative|
    +-----+---+-----+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI XIP alternative 2 bytes
                #define UDMA_HYPER_OSPI_ALTER_XIP_OFFSET         0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_alter_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_alter_xip_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_BIT                          0
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_WIDTH                        16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_MASK                         0xffff
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_RESET                        0x0
        
        // 2 Bytes SPI alternative (access: R/W)
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_BIT                          16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_WIDTH                        16
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_MASK                         0xffff0000
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_GET(value)         (GAP_BEXTRACTU((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_GETS(value)        (GAP_BEXTRACT((value),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0_SET(value,field)   (GAP_BINSERT((value),(field),16,0))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE0(val)               ((val) << 0)
        
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_GET(value)         (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_GETS(value)        (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1_SET(value,field)   (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_OSPI_ALTER_XIP_MODE1(val)               ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mode0           :16; // 2 Bytes SPI alternative
            unsigned int mode1           :16; // 2 Bytes SPI alternative
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_alter_xip_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_alter_xip : public vp::reg_32
        {
        public:
            inline void mode0_set(uint32_t value);
            inline uint32_t mode0_get();
            inline void mode1_set(uint32_t value);
            inline uint32_t mode1_get();
        };

|

.. _udma_hyper_OSPI_REG_XIP:

OSPI_REG_XIP
""""""""""""

OSPI XIP other configuration

.. table:: 

    +-----+---+------------+--------------------+
    |Bit #|R/W|    Name    |    Description     |
    +=====+===+============+====================+
    |4:0  |-  |XIP_LATENCY0|XIP latency0 for cs0|
    +-----+---+------------+--------------------+
    |9:5  |-  |XIP_LATENCY1|XIP latency1 for cs1|
    +-----+---+------------+--------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI XIP other configuration
                #define UDMA_HYPER_OSPI_REG_XIP_OFFSET           0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_ospi_reg_xip_get(uint32_t base);
        static inline void udma_hyper_ospi_reg_xip_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // XIP latency0 for cs0 (access: -)
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_BIT                     0
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_WIDTH                   5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_MASK                    0x1f
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_RESET                   0x0
        
        // XIP latency1 for cs1 (access: -)
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_BIT                     5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_WIDTH                   5
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_MASK                    0x3e0
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY0(val)          ((val) << 0)
        
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_GET(value)    (GAP_BEXTRACTU((value),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_GETS(value)   (GAP_BEXTRACT((value),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1_SET(value,field) (GAP_BINSERT((value),(field),5,5))
        #define UDMA_HYPER_OSPI_REG_XIP_XIP_LATENCY1(val)          ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int xip_latency0    :5 ; // XIP latency0 for cs0
            unsigned int xip_latency1    :5 ; // XIP latency1 for cs1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_ospi_reg_xip_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_ospi_reg_xip : public vp::reg_32
        {
        public:
            inline void xip_latency0_set(uint32_t value);
            inline uint32_t xip_latency0_get();
            inline void xip_latency1_set(uint32_t value);
            inline uint32_t xip_latency1_get();
        };

|

.. _udma_hyper_LINE_2D:

LINE_2D
"""""""

OSPI 2D line.

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                 Description                                                  |
    +=====+===+====+==============================================================================================================+
    |31:0 |R/W|LINE|OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE.|
    +-----+---+----+--------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI 2D line.
                #define UDMA_HYPER_LINE_2D_OFFSET                0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_line_2d_get(uint32_t base);
        static inline void udma_hyper_line_2d_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_HYPER_LINE_2D_LINE_BIT                                  0
        #define UDMA_HYPER_LINE_2D_LINE_WIDTH                                32
        #define UDMA_HYPER_LINE_2D_LINE_MASK                                 0xffffffff
        #define UDMA_HYPER_LINE_2D_LINE_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_LINE_2D_LINE_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_LINE_2D_LINE_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_LINE_2D_LINE_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_LINE_2D_LINE(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int line            :32; // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_line_2d_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_line_2d : public vp::reg_32
        {
        public:
            inline void line_set(uint32_t value);
            inline uint32_t line_get();
        };

|

.. _udma_hyper_STRIDE_2D:

STRIDE_2D
"""""""""

OSPI 2D stride.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                   Description                                                    |
    +=====+===+======+==================================================================================================================+
    |31:0 |R/W|STRIDE|OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE.|
    +-----+---+------+------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI 2D stride.
                #define UDMA_HYPER_STRIDE_2D_OFFSET              0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_stride_2d_get(uint32_t base);
        static inline void udma_hyper_stride_2d_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_HYPER_STRIDE_2D_STRIDE_BIT                              0
        #define UDMA_HYPER_STRIDE_2D_STRIDE_WIDTH                            32
        #define UDMA_HYPER_STRIDE_2D_STRIDE_MASK                             0xffffffff
        #define UDMA_HYPER_STRIDE_2D_STRIDE_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_STRIDE_2D_STRIDE_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_STRIDE_2D_STRIDE(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int stride          :32; // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_stride_2d_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_stride_2d : public vp::reg_32
        {
        public:
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _udma_hyper_BURST_ENABLE:

BURST_ENABLE
""""""""""""

OSPI burst mode/2D mode enable.

.. table:: 

    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|          Name          |                                                                      Description                                                                      |
    +=====+===+========================+=======================================================================================================================================================+
    |    0|R/W|CS0_AUTO_BURST_ENABLE   |Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 0 : - 1'b0 disable    - 1'b1 Enable|
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|CS1_AUTO_BURST_ENABLE   |Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 1 : - 1'b0 disable    - 1'b1 Enable|
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|CS0_MAXIMUM_CHECK_ENABLE|Enable Maximum chip select low time for self-refresh HYPERRAM for channel 0:  - 1'b0: disable -  1'b1: enable                                          |
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|CS1_MAXIMUM_CHECK_ENABLE|Enable Maximum chip select low time for self-refresh HYPERRAM for channel 1 :  - 1'b0: disable -  1'b1: enable                                         |
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |5:4  |R/W|2D_ENABLE               |OSPI burst 2D mode enable for normal mode and XIP :  - 1'b0: BURST 2D mode disable -  1'b1: BURST 2D mode disable                                      |
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:6  |R/W|2D_MODE                 |2D tansfer mode from L2 to external memory config :  - 2'b00: 1D_TO_1D - 2'b01: 1D_TO_2D - 2'b10: 2D_TO_1D - 2'b11: 2D_TO_2D                           |
    +-----+---+------------------------+-------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI burst mode/2D mode enable.
                #define UDMA_HYPER_BURST_ENABLE_OFFSET           0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_burst_enable_get(uint32_t base);
        static inline void udma_hyper_burst_enable_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 0 : - 1'b0 disable    - 1'b1 Enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_BIT            0
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_WIDTH          1
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_MASK           0x1
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_RESET          0x0
        
        // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 1 : - 1'b0 disable    - 1'b1 Enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_BIT            1
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_WIDTH          1
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_MASK           0x2
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_RESET          0x0
        
        // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 0:  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_BIT         2
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_WIDTH       1
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_MASK        0x4
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_RESET       0x0
        
        // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 1 :  - 1'b0: disable -  1'b1: enable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_BIT         3
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_WIDTH       1
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_MASK        0x8
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_RESET       0x0
        
        // OSPI burst 2D mode enable for normal mode and XIP :  - 1'b0: BURST 2D mode disable -  1'b1: BURST 2D mode disable (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_BIT                        4
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_WIDTH                      2
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_MASK                       0x30
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_RESET                      0x0
        
        // 2D tansfer mode from L2 to external memory config :  - 2'b00: 1D_TO_1D - 2'b01: 1D_TO_2D - 2'b10: 2D_TO_1D - 2'b11: 2D_TO_2D (access: R/W)
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_BIT                          6
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_WIDTH                        2
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_MASK                         0xc0
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_BURST_ENABLE_CS0_AUTO_BURST_ENABLE(val) ((val) << 0)
        
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_BURST_ENABLE_CS1_AUTO_BURST_ENABLE(val) ((val) << 1)
        
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_BURST_ENABLE_CS0_MAXIMUM_CHECK_ENABLE(val) ((val) << 2)
        
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_BURST_ENABLE_CS1_MAXIMUM_CHECK_ENABLE(val) ((val) << 3)
        
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_GET(value)       (GAP_BEXTRACTU((value),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_GETS(value)      (GAP_BEXTRACT((value),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),2,4))
        #define UDMA_HYPER_BURST_ENABLE_2D_ENABLE(val)             ((val) << 4)
        
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_GET(value)         (GAP_BEXTRACTU((value),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_GETS(value)        (GAP_BEXTRACT((value),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE_SET(value,field)   (GAP_BINSERT((value),(field),2,6))
        #define UDMA_HYPER_BURST_ENABLE_2D_MODE(val)               ((val) << 6)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cs0_auto_burst_enable:1 ; // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 0 : - 1'b0 disable    - 1'b1 Enable
            unsigned int cs1_auto_burst_enable:1 ; // Automatically control Maximum chip select low time for self-refresh HYPERRAM to valid the data transfer for channel 1 : - 1'b0 disable    - 1'b1 Enable
            unsigned int cs0_maximum_check_enable:1 ; // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 0:  - 1'b0: disable -  1'b1: enable
            unsigned int cs1_maximum_check_enable:1 ; // Enable Maximum chip select low time for self-refresh HYPERRAM for channel 1 :  - 1'b0: disable -  1'b1: enable
            unsigned int _2d_enable      :2 ; // OSPI burst 2D mode enable for normal mode and XIP :  - 1'b0: BURST 2D mode disable -  1'b1: BURST 2D mode disable
            unsigned int _2d_mode        :2 ; // 2D tansfer mode from L2 to external memory config :  - 2'b00: 1D_TO_1D - 2'b01: 1D_TO_2D - 2'b10: 2D_TO_1D - 2'b11: 2D_TO_2D
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_burst_enable_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_burst_enable : public vp::reg_32
        {
        public:
            inline void cs0_auto_burst_enable_set(uint32_t value);
            inline uint32_t cs0_auto_burst_enable_get();
            inline void cs1_auto_burst_enable_set(uint32_t value);
            inline uint32_t cs1_auto_burst_enable_get();
            inline void cs0_maximum_check_enable_set(uint32_t value);
            inline uint32_t cs0_maximum_check_enable_get();
            inline void cs1_maximum_check_enable_set(uint32_t value);
            inline uint32_t cs1_maximum_check_enable_get();
            inline void _2d_enable_set(uint32_t value);
            inline uint32_t _2d_enable_get();
            inline void _2d_mode_set(uint32_t value);
            inline uint32_t _2d_mode_get();
        };

|

.. _udma_hyper_IRQ_EN:

IRQ_EN
""""""

OSPI interrupt enable register

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                          Description                                           |
    +=====+===+======+================================================================================================+
    |    0|R/W|EN    |Octo SPI interrupt enable control :  - 1'b0: interrupt disable -  1'b1: Interrupt enable        |
    +-----+---+------+------------------------------------------------------------------------------------------------+
    |    1|R/W|XIP_EN|Octo SPI interrupt enable control for XIP :  - 1'b0: interrupt disable -  1'b1: Interrupt enable|
    +-----+---+------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // OSPI interrupt enable register
                #define UDMA_HYPER_IRQ_EN_OFFSET                 0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_irq_en_get(uint32_t base);
        static inline void udma_hyper_irq_en_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Octo SPI interrupt enable control :  - 1'b0: interrupt disable -  1'b1: Interrupt enable (access: R/W)
        #define UDMA_HYPER_IRQ_EN_EN_BIT                                     0
        #define UDMA_HYPER_IRQ_EN_EN_WIDTH                                   1
        #define UDMA_HYPER_IRQ_EN_EN_MASK                                    0x1
        #define UDMA_HYPER_IRQ_EN_EN_RESET                                   0x0
        
        // Octo SPI interrupt enable control for XIP :  - 1'b0: interrupt disable -  1'b1: Interrupt enable (access: R/W)
        #define UDMA_HYPER_IRQ_EN_XIP_EN_BIT                                 1
        #define UDMA_HYPER_IRQ_EN_XIP_EN_WIDTH                               1
        #define UDMA_HYPER_IRQ_EN_XIP_EN_MASK                                0x2
        #define UDMA_HYPER_IRQ_EN_XIP_EN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_IRQ_EN_EN_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_IRQ_EN_EN_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_IRQ_EN_EN_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_IRQ_EN_EN(val)                          ((val) << 0)
        
        #define UDMA_HYPER_IRQ_EN_XIP_EN_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_IRQ_EN_XIP_EN(val)                      ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // Octo SPI interrupt enable control :  - 1'b0: interrupt disable -  1'b1: Interrupt enable
            unsigned int xip_en          :1 ; // Octo SPI interrupt enable control for XIP :  - 1'b0: interrupt disable -  1'b1: Interrupt enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_irq_en_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_irq_en : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void xip_en_set(uint32_t value);
            inline uint32_t xip_en_get();
        };

|

.. _udma_hyper_CLK_DIV:

CLK_DIV
"""""""

Clock divide.

.. table:: 

    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                     Description                                                                     |
    +=====+===+=====+=====================================================================================================================================================+
    |7:0  |R/W|DATA |Clock divide data, form 0 – 255, frequency divide table is : -8’h0 – IO_FREQUENCY / 1 -8’h1 – IO_FREQUENCY / 2 -8’h2 – IO_FREQUENCY / 4 …            |
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------+
    |8    |W  |VALID|Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.|
    +-----+---+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock divide.
                #define UDMA_HYPER_CLK_DIV_OFFSET                0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_clk_div_get(uint32_t base);
        static inline void udma_hyper_clk_div_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4  (access: R/W)
        #define UDMA_HYPER_CLK_DIV_DATA_BIT                                  0
        #define UDMA_HYPER_CLK_DIV_DATA_WIDTH                                8
        #define UDMA_HYPER_CLK_DIV_DATA_MASK                                 0xff
        #define UDMA_HYPER_CLK_DIV_DATA_RESET                                0x0
        
        // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0. (access: W)
        #define UDMA_HYPER_CLK_DIV_VALID_BIT                                 8
        #define UDMA_HYPER_CLK_DIV_VALID_WIDTH                               1
        #define UDMA_HYPER_CLK_DIV_VALID_MASK                                0x100
        #define UDMA_HYPER_CLK_DIV_VALID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_CLK_DIV_DATA_GET(value)                 (GAP_BEXTRACTU((value),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA_GETS(value)                (GAP_BEXTRACT((value),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA_SET(value,field)           (GAP_BINSERT((value),(field),8,0))
        #define UDMA_HYPER_CLK_DIV_DATA(val)                       ((val) << 0)
        
        #define UDMA_HYPER_CLK_DIV_VALID_GET(value)                (GAP_BEXTRACTU((value),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID_GETS(value)               (GAP_BEXTRACT((value),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID_SET(value,field)          (GAP_BINSERT((value),(field),1,8))
        #define UDMA_HYPER_CLK_DIV_VALID(val)                      ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :8 ; // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4 
            unsigned int valid           :1 ; // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_clk_div_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_clk_div : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
            inline void valid_set(uint32_t value);
            inline uint32_t valid_get();
        };

|

.. _udma_hyper_STATUS:

STATUS
""""""

Transfer status for error.

.. table:: 

    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |                                            Description                                            |
    +=====+===+=================+===================================================================================================+
    |    0|R/W|TX_ERROR         |TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error                |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |    1|R/W|RX_ERROR         |RX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error                |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |    2|R/W|RX_TX_END        |RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end     |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |    3|R/W|SDIO_RX_TX_ERROR |SDIO RX TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error        |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |    4|R/W|SDIO_RX_TX_END   |SDIO RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end|
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |15:5 |R/W|reserved         |-                                                                                                  |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+
    |31:16|R/W|SDIO_ERROR_STATUS|SDIO error status flag, indicate the error type                                                    |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Transfer status for error.
                #define UDMA_HYPER_STATUS_OFFSET                 0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_status_get(uint32_t base);
        static inline void udma_hyper_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_TX_ERROR_BIT                               0
        #define UDMA_HYPER_STATUS_TX_ERROR_WIDTH                             1
        #define UDMA_HYPER_STATUS_TX_ERROR_MASK                              0x1
        #define UDMA_HYPER_STATUS_TX_ERROR_RESET                             0x0
        
        // RX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_RX_ERROR_BIT                               1
        #define UDMA_HYPER_STATUS_RX_ERROR_WIDTH                             1
        #define UDMA_HYPER_STATUS_RX_ERROR_MASK                              0x2
        #define UDMA_HYPER_STATUS_RX_ERROR_RESET                             0x0
        
        // RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end (access: R/W)
        #define UDMA_HYPER_STATUS_RX_TX_END_BIT                              2
        #define UDMA_HYPER_STATUS_RX_TX_END_WIDTH                            1
        #define UDMA_HYPER_STATUS_RX_TX_END_MASK                             0x4
        #define UDMA_HYPER_STATUS_RX_TX_END_RESET                            0x0
        
        // SDIO RX TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_BIT                       3
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_WIDTH                     1
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_MASK                      0x8
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_RESET                     0x0
        
        // SDIO RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_BIT                         4
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_WIDTH                       1
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_MASK                        0x10
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_RESET                       0x0
        
        // - (access: R/W)
        #define UDMA_HYPER_STATUS_RESERVED_BIT                               5
        #define UDMA_HYPER_STATUS_RESERVED_WIDTH                             11
        #define UDMA_HYPER_STATUS_RESERVED_MASK                              0xffe0
        #define UDMA_HYPER_STATUS_RESERVED_RESET                             0x0
        
        // SDIO error status flag, indicate the error type (access: R/W)
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_BIT                      16
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_WIDTH                    16
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_MASK                     0xffff0000
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_STATUS_TX_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_HYPER_STATUS_TX_ERROR(val)                    ((val) << 0)
        
        #define UDMA_HYPER_STATUS_RX_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_HYPER_STATUS_RX_ERROR(val)                    ((val) << 1)
        
        #define UDMA_HYPER_STATUS_RX_TX_END_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define UDMA_HYPER_STATUS_RX_TX_END(val)                   ((val) << 2)
        
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_ERROR(val)            ((val) << 3)
        
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define UDMA_HYPER_STATUS_SDIO_RX_TX_END(val)              ((val) << 4)
        
        #define UDMA_HYPER_STATUS_RESERVED_GET(value)              (GAP_BEXTRACTU((value),11,5))
        #define UDMA_HYPER_STATUS_RESERVED_GETS(value)             (GAP_BEXTRACT((value),11,5))
        #define UDMA_HYPER_STATUS_RESERVED_SET(value,field)        (GAP_BINSERT((value),(field),11,5))
        #define UDMA_HYPER_STATUS_RESERVED(val)                    ((val) << 5)
        
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_GET(value)     (GAP_BEXTRACTU((value),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_GETS(value)    (GAP_BEXTRACT((value),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_HYPER_STATUS_SDIO_ERROR_STATUS(val)           ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tx_error        :1 ; // TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int rx_error        :1 ; // RX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int rx_tx_end       :1 ; // RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end
            unsigned int sdio_rx_tx_error:1 ; // SDIO RX TX transfer error because of tcsm, write 1 to clear: - 1'b0: no error - 1'b1: error
            unsigned int sdio_rx_tx_end  :1 ; // SDIO RX TX transfer end flag, can be polling by user, write 1 to clear: - 1'b0: not end - 1'b1: end
            unsigned int reserved        :11; // -
            unsigned int sdio_error_status:16; // SDIO error status flag, indicate the error type
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_status : public vp::reg_32
        {
        public:
            inline void tx_error_set(uint32_t value);
            inline uint32_t tx_error_get();
            inline void rx_error_set(uint32_t value);
            inline uint32_t rx_error_get();
            inline void rx_tx_end_set(uint32_t value);
            inline uint32_t rx_tx_end_get();
            inline void sdio_rx_tx_error_set(uint32_t value);
            inline uint32_t sdio_rx_tx_error_get();
            inline void sdio_rx_tx_end_set(uint32_t value);
            inline uint32_t sdio_rx_tx_end_get();
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void sdio_error_status_set(uint32_t value);
            inline uint32_t sdio_error_status_get();
        };

|

.. _udma_hyper_SDIO_CMD_ARG:

SDIO_CMD_ARG
""""""""""""

SDIO command argument.

.. table:: 

    +-----+---+----+---------------------+
    |Bit #|R/W|Name|     Description     |
    +=====+===+====+=====================+
    |31:0 |R/W|ARG |SDIO command argument|
    +-----+---+----+---------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SDIO command argument.
                #define UDMA_HYPER_SDIO_CMD_ARG_OFFSET           0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_sdio_cmd_arg_get(uint32_t base);
        static inline void udma_hyper_sdio_cmd_arg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SDIO command argument (access: R/W)
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_BIT                              0
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_WIDTH                            32
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_MASK                             0xffffffff
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_CMD_ARG_ARG(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int arg             :32; // SDIO command argument
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_cmd_arg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_sdio_cmd_arg : public vp::reg_32
        {
        public:
            inline void arg_set(uint32_t value);
            inline uint32_t arg_get();
        };

|

.. _udma_hyper_SDIO_RSP0:

SDIO_RSP0
"""""""""

SDIO response 0.

.. table:: 

    +-----+---+----+---------------+
    |Bit #|R/W|Name|  Description  |
    +=====+===+====+===============+
    |31:0 |R/W|RSP0|SDIO response 0|
    +-----+---+----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SDIO response 0.
                #define UDMA_HYPER_SDIO_RSP0_OFFSET              0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_sdio_rsp0_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SDIO response 0 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP0_RSP0_BIT                                0
        #define UDMA_HYPER_SDIO_RSP0_RSP0_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP0_RSP0_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP0_RSP0_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_SDIO_RSP0_RSP0_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP0_RSP0(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rsp0            :32; // SDIO response 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_sdio_rsp0 : public vp::reg_32
        {
        public:
            inline void rsp0_set(uint32_t value);
            inline uint32_t rsp0_get();
        };

|

.. _udma_hyper_SDIO_RSP1:

SDIO_RSP1
"""""""""

SDIO response 1.

.. table:: 

    +-----+---+----+---------------+
    |Bit #|R/W|Name|  Description  |
    +=====+===+====+===============+
    |31:0 |R/W|RSP1|SDIO response 1|
    +-----+---+----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SDIO response 1.
                #define UDMA_HYPER_SDIO_RSP1_OFFSET              0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_sdio_rsp1_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SDIO response 1 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP1_RSP1_BIT                                0
        #define UDMA_HYPER_SDIO_RSP1_RSP1_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP1_RSP1_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP1_RSP1_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_SDIO_RSP1_RSP1_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP1_RSP1(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rsp1            :32; // SDIO response 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_sdio_rsp1 : public vp::reg_32
        {
        public:
            inline void rsp1_set(uint32_t value);
            inline uint32_t rsp1_get();
        };

|

.. _udma_hyper_SDIO_RSP2:

SDIO_RSP2
"""""""""

SDIO response 2.

.. table:: 

    +-----+---+----+---------------+
    |Bit #|R/W|Name|  Description  |
    +=====+===+====+===============+
    |31:0 |R/W|RSP2|SDIO response 2|
    +-----+---+----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SDIO response 2.
                #define UDMA_HYPER_SDIO_RSP2_OFFSET              0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_sdio_rsp2_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SDIO response 2 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP2_RSP2_BIT                                0
        #define UDMA_HYPER_SDIO_RSP2_RSP2_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP2_RSP2_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP2_RSP2_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_SDIO_RSP2_RSP2_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP2_RSP2(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rsp2            :32; // SDIO response 2
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_sdio_rsp2 : public vp::reg_32
        {
        public:
            inline void rsp2_set(uint32_t value);
            inline uint32_t rsp2_get();
        };

|

.. _udma_hyper_SDIO_RSP3:

SDIO_RSP3
"""""""""

SDIO response 3.

.. table:: 

    +-----+---+----+---------------+
    |Bit #|R/W|Name|  Description  |
    +=====+===+====+===============+
    |31:0 |R/W|RSP3|SDIO response 3|
    +-----+---+----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // SDIO response 3.
                #define UDMA_HYPER_SDIO_RSP3_OFFSET              0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_hyper_sdio_rsp3_get(uint32_t base);
        static inline void udma_hyper_sdio_rsp3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SDIO response 3 (access: R/W)
        #define UDMA_HYPER_SDIO_RSP3_RSP3_BIT                                0
        #define UDMA_HYPER_SDIO_RSP3_RSP3_WIDTH                              32
        #define UDMA_HYPER_SDIO_RSP3_RSP3_MASK                               0xffffffff
        #define UDMA_HYPER_SDIO_RSP3_RSP3_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_HYPER_SDIO_RSP3_RSP3_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_HYPER_SDIO_RSP3_RSP3(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rsp3            :32; // SDIO response 3
          };
          unsigned int raw;
        } __attribute__((packed)) udma_hyper_sdio_rsp3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_hyper_sdio_rsp3 : public vp::reg_32
        {
        public:
            inline void rsp3_set(uint32_t value);
            inline uint32_t rsp3_get();
        };

|
