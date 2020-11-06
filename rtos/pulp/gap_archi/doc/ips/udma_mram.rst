Input file: fe/ips/udma/udma_mram/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------+------+-----+-------------------------------+
    |                 Name                  |Offset|Width|          Description          |
    +=======================================+======+=====+===============================+
    |:ref:`RX_DEST<udma_mram_RX_DEST>`      |     0|   32|Stream ID for the uDMA channel |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TX_DEST<udma_mram_TX_DEST>`      |     4|   32|Stream ID for the uDMA channel |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TRANS_MODE<udma_mram_TRANS_MODE>`|     8|   32|Configure transaction mode     |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TRANS_ADDR<udma_mram_TRANS_ADDR>`|    12|   32|Configure each transaction addr|
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TRANS_SIZE<udma_mram_TRANS_SIZE>`|    16|   32|Configure each transaction size|
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TRANS_CFG<udma_mram_TRANS_CFG>`  |    20|   32|Start  each transaction rx/tx  |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`EXT_ADDR<udma_mram_EXT_ADDR>`    |    24|   32|Memory access address register.|
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`STATUS<udma_mram_STATUS>`        |    28|   32|MRAM status register.          |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`MODE<udma_mram_MODE>`            |    32|   32|MRAM mode configure register.  |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`ERASE_ADDR<udma_mram_ERASE_ADDR>`|    36|   32|Erase address register.        |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`ERASE_SIZE<udma_mram_ERASE_SIZE>`|    40|   32|Erase size register.           |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`CLK_DIV<udma_mram_CLK_DIV>`      |    44|   32|Clock divide.                  |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`ISR<udma_mram_ISR>`              |    48|   32|IRQ status register.           |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`IER<udma_mram_IER>`              |    52|   32|IRQ enable register.           |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`LINE_2D<udma_mram_LINE_2D>`      |    60|   32|OSPI 2D line.                  |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`STRIDE_2D<udma_mram_STRIDE_2D>`  |    64|   32|OSPI 2D stride.                |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`ENABLE_2D<udma_mram_ENABLE_2D>`  |    68|   32|2D mode enable.                |
    +---------------------------------------+------+-----+-------------------------------+
    |:ref:`TIMING_CFG<udma_mram_TIMING_CFG>`|    72|   32|Timing configuration.          |
    +---------------------------------------+------+-----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_MRAM_RX_DEST_OFFSET                 0x0
        
                // Stream ID for the uDMA channel
                #define UDMA_MRAM_TX_DEST_OFFSET                 0x4
        
                // Configure transaction mode
                #define UDMA_MRAM_TRANS_MODE_OFFSET              0x8
        
                // Configure each transaction addr
                #define UDMA_MRAM_TRANS_ADDR_OFFSET              0xc
        
                // Configure each transaction size
                #define UDMA_MRAM_TRANS_SIZE_OFFSET              0x10
        
                // Start  each transaction rx/tx
                #define UDMA_MRAM_TRANS_CFG_OFFSET               0x14
        
                // Memory access address register.
                #define UDMA_MRAM_EXT_ADDR_OFFSET                0x18
        
                // MRAM status register.
                #define UDMA_MRAM_STATUS_OFFSET                  0x1c
        
                // MRAM mode configure register.
                #define UDMA_MRAM_MODE_OFFSET                    0x20
        
                // Erase address register.
                #define UDMA_MRAM_ERASE_ADDR_OFFSET              0x24
        
                // Erase size register.
                #define UDMA_MRAM_ERASE_SIZE_OFFSET              0x28
        
                // Clock divide.
                #define UDMA_MRAM_CLK_DIV_OFFSET                 0x2c
        
                // IRQ status register.
                #define UDMA_MRAM_ISR_OFFSET                     0x30
        
                // IRQ enable register.
                #define UDMA_MRAM_IER_OFFSET                     0x34
        
                // OSPI 2D line.
                #define UDMA_MRAM_LINE_2D_OFFSET                 0x3c
        
                // OSPI 2D stride.
                #define UDMA_MRAM_STRIDE_2D_OFFSET               0x40
        
                // 2D mode enable.
                #define UDMA_MRAM_ENABLE_2D_OFFSET               0x44
        
                // Timing configuration.
                #define UDMA_MRAM_TIMING_CFG_OFFSET              0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_rx_dest_get(uint32_t base);
        static inline void udma_mram_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_tx_dest_get(uint32_t base);
        static inline void udma_mram_tx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_trans_mode_get(uint32_t base);
        static inline void udma_mram_trans_mode_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_trans_addr_get(uint32_t base);
        static inline void udma_mram_trans_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_trans_size_get(uint32_t base);
        static inline void udma_mram_trans_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_trans_cfg_get(uint32_t base);
        static inline void udma_mram_trans_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_ext_addr_get(uint32_t base);
        static inline void udma_mram_ext_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_status_get(uint32_t base);
        static inline void udma_mram_status_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_mode_get(uint32_t base);
        static inline void udma_mram_mode_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_erase_addr_get(uint32_t base);
        static inline void udma_mram_erase_addr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_erase_size_get(uint32_t base);
        static inline void udma_mram_erase_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_clk_div_get(uint32_t base);
        static inline void udma_mram_clk_div_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_isr_get(uint32_t base);
        static inline void udma_mram_isr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_ier_get(uint32_t base);
        static inline void udma_mram_ier_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_line_2d_get(uint32_t base);
        static inline void udma_mram_line_2d_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_stride_2d_get(uint32_t base);
        static inline void udma_mram_stride_2d_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_enable_2d_get(uint32_t base);
        static inline void udma_mram_enable_2d_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_mram_timing_cfg_get(uint32_t base);
        static inline void udma_mram_timing_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_MRAM_RX_DEST_DEST_BIT                                   0
        #define UDMA_MRAM_RX_DEST_DEST_WIDTH                                 8
        #define UDMA_MRAM_RX_DEST_DEST_MASK                                  0xff
        #define UDMA_MRAM_RX_DEST_DEST_RESET                                 0xff
        
        // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_MRAM_TX_DEST_DEST_BIT                                   0
        #define UDMA_MRAM_TX_DEST_DEST_WIDTH                                 8
        #define UDMA_MRAM_TX_DEST_DEST_MASK                                  0xff
        #define UDMA_MRAM_TX_DEST_DEST_RESET                                 0xff
        
        // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN (access: R/W)
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_BIT                            0
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_WIDTH                          1
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_MASK                           0x1
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_RESET                          0x0
        
        // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN (access: R/W)
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_BIT                              1
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_WIDTH                            1
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_MASK                             0x2
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_RESET                            0x0
        
        // Transfer addr, only when MODE is in AUTO (access: R/W)
        #define UDMA_MRAM_TRANS_ADDR_ADDR_BIT                                0
        #define UDMA_MRAM_TRANS_ADDR_ADDR_WIDTH                              32
        #define UDMA_MRAM_TRANS_ADDR_ADDR_MASK                               0xffffffff
        #define UDMA_MRAM_TRANS_ADDR_ADDR_RESET                              0x0
        
        // Transfer Size (access: R/W)
        #define UDMA_MRAM_TRANS_SIZE_SIZE_BIT                                0
        #define UDMA_MRAM_TRANS_SIZE_SIZE_WIDTH                              21
        #define UDMA_MRAM_TRANS_SIZE_SIZE_MASK                               0x1fffff
        #define UDMA_MRAM_TRANS_SIZE_SIZE_RESET                              0x0
        
        // Transfer type - 1'b0: TX - 1'b1: RX (access: R/W)
        #define UDMA_MRAM_TRANS_CFG_RXTX_BIT                                 0
        #define UDMA_MRAM_TRANS_CFG_RXTX_WIDTH                               1
        #define UDMA_MRAM_TRANS_CFG_RXTX_MASK                                0x1
        #define UDMA_MRAM_TRANS_CFG_RXTX_RESET                               0x0
        
        // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start (access: R/W)
        #define UDMA_MRAM_TRANS_CFG_VALID_BIT                                1
        #define UDMA_MRAM_TRANS_CFG_VALID_WIDTH                              1
        #define UDMA_MRAM_TRANS_CFG_VALID_MASK                               0x2
        #define UDMA_MRAM_TRANS_CFG_VALID_RESET                              0x0
        
        // Memory access address bitfield. (access: R/W)
        #define UDMA_MRAM_EXT_ADDR_ADDR_BIT                                  0
        #define UDMA_MRAM_EXT_ADDR_ADDR_WIDTH                                32
        #define UDMA_MRAM_EXT_ADDR_ADDR_MASK                                 0xffffffff
        #define UDMA_MRAM_EXT_ADDR_ADDR_RESET                                0x0
        
        // Erase busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_ERASE_BUSY_BIT                              0
        #define UDMA_MRAM_STATUS_ERASE_BUSY_WIDTH                            1
        #define UDMA_MRAM_STATUS_ERASE_BUSY_MASK                             0x1
        #define UDMA_MRAM_STATUS_ERASE_BUSY_RESET                            0x0
        
        // TX busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_TX_BUSY_BIT                                 1
        #define UDMA_MRAM_STATUS_TX_BUSY_WIDTH                               1
        #define UDMA_MRAM_STATUS_TX_BUSY_MASK                                0x2
        #define UDMA_MRAM_STATUS_TX_BUSY_RESET                               0x0
        
        // RX busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_RX_BUSY_BIT                                 2
        #define UDMA_MRAM_STATUS_RX_BUSY_WIDTH                               1
        #define UDMA_MRAM_STATUS_RX_BUSY_MASK                                0x4
        #define UDMA_MRAM_STATUS_RX_BUSY_RESET                               0x0
        
        // ECC Error Correction, Low indicates no ECC error detected. High indicates 1 or 2 bit errors corrected. (access: R)
        #define UDMA_MRAM_STATUS_EC_ERR_BIT                                  4
        #define UDMA_MRAM_STATUS_EC_ERR_WIDTH                                1
        #define UDMA_MRAM_STATUS_EC_ERR_MASK                                 0x10
        #define UDMA_MRAM_STATUS_EC_ERR_RESET                                0x0
        
        // Unrecoverable Error, High indicates unrecoverable errors detected. Not 100% accurate for 3 or more bit errors. In the case of 3-bit errors,correct detection is 84% (access: R)
        #define UDMA_MRAM_STATUS_UE_ERR_BIT                                  5
        #define UDMA_MRAM_STATUS_UE_ERR_WIDTH                                1
        #define UDMA_MRAM_STATUS_UE_ERR_MASK                                 0x20
        #define UDMA_MRAM_STATUS_UE_ERR_RESET                                0x0
        
        // Signal ECC bypass (access: R/W)
        #define UDMA_MRAM_MODE_ECCBYPS_BIT                                   0
        #define UDMA_MRAM_MODE_ECCBYPS_WIDTH                                 1
        #define UDMA_MRAM_MODE_ECCBYPS_MASK                                  0x1
        #define UDMA_MRAM_MODE_ECCBYPS_RESET                                 0x0
        
        // Signal Deep power down (access: R/W)
        #define UDMA_MRAM_MODE_DPD_BIT                                       1
        #define UDMA_MRAM_MODE_DPD_WIDTH                                     1
        #define UDMA_MRAM_MODE_DPD_MASK                                      0x2
        #define UDMA_MRAM_MODE_DPD_RESET                                     0x0
        
        // Signal TMEN (access: R/W)
        #define UDMA_MRAM_MODE_TMEN_BIT                                      3
        #define UDMA_MRAM_MODE_TMEN_WIDTH                                    1
        #define UDMA_MRAM_MODE_TMEN_MASK                                     0x8
        #define UDMA_MRAM_MODE_TMEN_RESET                                    0x0
        
        // Signal NVR (access: R/W)
        #define UDMA_MRAM_MODE_NVR_BIT                                       4
        #define UDMA_MRAM_MODE_NVR_WIDTH                                     1
        #define UDMA_MRAM_MODE_NVR_MASK                                      0x10
        #define UDMA_MRAM_MODE_NVR_RESET                                     0x0
        
        // Signal RSTb (access: R/W)
        #define UDMA_MRAM_MODE_RSTB_BIT                                      5
        #define UDMA_MRAM_MODE_RSTB_WIDTH                                    1
        #define UDMA_MRAM_MODE_RSTB_MASK                                     0x20
        #define UDMA_MRAM_MODE_RSTB_RESET                                    0x0
        
        // Signal RETb (access: R/W)
        #define UDMA_MRAM_MODE_RETB_BIT                                      6
        #define UDMA_MRAM_MODE_RETB_WIDTH                                    1
        #define UDMA_MRAM_MODE_RETB_MASK                                     0x40
        #define UDMA_MRAM_MODE_RETB_RESET                                    0x0
        
        // Signal PORb (access: R/W)
        #define UDMA_MRAM_MODE_PORB_BIT                                      7
        #define UDMA_MRAM_MODE_PORB_WIDTH                                    1
        #define UDMA_MRAM_MODE_PORB_MASK                                     0x80
        #define UDMA_MRAM_MODE_PORB_RESET                                    0x0
        
        // MRAM operation configure, -8'h00 power up -8'h01 trim configure -8'h02 program -8'h04 chip erase -8'h08 sector erase -8'h10 word erase -8'h20 power down -8'h40 read (access: R/W)
        #define UDMA_MRAM_MODE_OPERATION_BIT                                 8
        #define UDMA_MRAM_MODE_OPERATION_WIDTH                               8
        #define UDMA_MRAM_MODE_OPERATION_MASK                                0xff00
        #define UDMA_MRAM_MODE_OPERATION_RESET                               0x0
        
        // Erase addr, lower 4 bit is useless (access: R/W)
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_BIT                            0
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_WIDTH                          4
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_MASK                           0xf
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_RESET                          0x0
        
        // Erase addr (access: R/W)
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_BIT                            4
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_WIDTH                          17
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_MASK                           0x1ffff0
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_RESET                          0x0
        
        // Erase Size (access: R/W)
        #define UDMA_MRAM_ERASE_SIZE_SIZE_BIT                                0
        #define UDMA_MRAM_ERASE_SIZE_SIZE_WIDTH                              7
        #define UDMA_MRAM_ERASE_SIZE_SIZE_MASK                               0x7f
        #define UDMA_MRAM_ERASE_SIZE_SIZE_RESET                              0x0
        
        // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4  (access: R/W)
        #define UDMA_MRAM_CLK_DIV_DATA_BIT                                   0
        #define UDMA_MRAM_CLK_DIV_DATA_WIDTH                                 8
        #define UDMA_MRAM_CLK_DIV_DATA_MASK                                  0xff
        #define UDMA_MRAM_CLK_DIV_DATA_RESET                                 0x0
        
        // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0. (access: W)
        #define UDMA_MRAM_CLK_DIV_VALID_BIT                                  8
        #define UDMA_MRAM_CLK_DIV_VALID_WIDTH                                1
        #define UDMA_MRAM_CLK_DIV_VALID_MASK                                 0x100
        #define UDMA_MRAM_CLK_DIV_VALID_RESET                                0x0
        
        // MRAM Clock enable. (access: W)
        #define UDMA_MRAM_CLK_DIV_ENABLE_BIT                                 9
        #define UDMA_MRAM_CLK_DIV_ENABLE_WIDTH                               1
        #define UDMA_MRAM_CLK_DIV_ENABLE_MASK                                0x200
        #define UDMA_MRAM_CLK_DIV_ENABLE_RESET                               0x0
        
        // MRAM erase finish (access: R)
        #define UDMA_MRAM_ISR_ERASE_DONE_BIT                                 0
        #define UDMA_MRAM_ISR_ERASE_DONE_WIDTH                               1
        #define UDMA_MRAM_ISR_ERASE_DONE_MASK                                0x1
        #define UDMA_MRAM_ISR_ERASE_DONE_RESET                               0x0
        
        // MRAM program finish (access: R)
        #define UDMA_MRAM_ISR_PROGRAM_DONE_BIT                               1
        #define UDMA_MRAM_ISR_PROGRAM_DONE_WIDTH                             1
        #define UDMA_MRAM_ISR_PROGRAM_DONE_MASK                              0x2
        #define UDMA_MRAM_ISR_PROGRAM_DONE_RESET                             0x0
        
        // MRAM trim config finish (access: R)
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_BIT                           2
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_WIDTH                         1
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_MASK                          0x4
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_RESET                         0x0
        
        // MRAM read finish. (access: R)
        #define UDMA_MRAM_ISR_RX_DONE_BIT                                    3
        #define UDMA_MRAM_ISR_RX_DONE_WIDTH                                  1
        #define UDMA_MRAM_ISR_RX_DONE_MASK                                   0x8
        #define UDMA_MRAM_ISR_RX_DONE_RESET                                  0x0
        
        // MRAM erase IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_ERASE_EN_BIT                                   0
        #define UDMA_MRAM_IER_ERASE_EN_WIDTH                                 1
        #define UDMA_MRAM_IER_ERASE_EN_MASK                                  0x1
        #define UDMA_MRAM_IER_ERASE_EN_RESET                                 0x0
        
        // MRAM program IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_PROGRAM_EN_BIT                                 1
        #define UDMA_MRAM_IER_PROGRAM_EN_WIDTH                               1
        #define UDMA_MRAM_IER_PROGRAM_EN_MASK                                0x2
        #define UDMA_MRAM_IER_PROGRAM_EN_RESET                               0x0
        
        // MRAM trim config IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_BIT                             2
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_WIDTH                           1
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_MASK                            0x4
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_RESET                           0x0
        
        // MRAM read IRQ enable. (access: R/W)
        #define UDMA_MRAM_IER_RX_DONE_EN_BIT                                 3
        #define UDMA_MRAM_IER_RX_DONE_EN_WIDTH                               1
        #define UDMA_MRAM_IER_RX_DONE_EN_MASK                                0x8
        #define UDMA_MRAM_IER_RX_DONE_EN_RESET                               0x0
        
        // MRAM xip erase IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_ERASE_EN_BIT                               0
        #define UDMA_MRAM_IER_XIP_ERASE_EN_WIDTH                             1
        #define UDMA_MRAM_IER_XIP_ERASE_EN_MASK                              0x1
        #define UDMA_MRAM_IER_XIP_ERASE_EN_RESET                             0x0
        
        // MRAM xip program IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_BIT                             1
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_WIDTH                           1
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_MASK                            0x2
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_RESET                           0x0
        
        // MRAM xip trim config IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_BIT                         2
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_WIDTH                       1
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_MASK                        0x4
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_RESET                       0x0
        
        // MRAM xip read IRQ enable. (access: R/W)
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_BIT                             3
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_WIDTH                           1
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_MASK                            0x8
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_RESET                           0x0
        
        // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_MRAM_LINE_2D_LINE_BIT                                   0
        #define UDMA_MRAM_LINE_2D_LINE_WIDTH                                 32
        #define UDMA_MRAM_LINE_2D_LINE_MASK                                  0xffffffff
        #define UDMA_MRAM_LINE_2D_LINE_RESET                                 0x0
        
        // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_MRAM_STRIDE_2D_STRIDE_BIT                               0
        #define UDMA_MRAM_STRIDE_2D_STRIDE_WIDTH                             32
        #define UDMA_MRAM_STRIDE_2D_STRIDE_MASK                              0xffffffff
        #define UDMA_MRAM_STRIDE_2D_STRIDE_RESET                             0x0
        
        // MRAM 2D mode enable :  - 1'b0: 2D mode disable -  1'b1: 2D mode disable (access: R/W)
        #define UDMA_MRAM_ENABLE_2D_ENABLE_BIT                               0
        #define UDMA_MRAM_ENABLE_2D_ENABLE_WIDTH                             1
        #define UDMA_MRAM_ENABLE_2D_ENABLE_MASK                              0x1
        #define UDMA_MRAM_ENABLE_2D_ENABLE_RESET                             0x0
        
        // Strobe timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_BIT                     0
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_WIDTH                   3
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_MASK                    0x7
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_RESET                   0x2
        
        // Power Supply timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_BIT                     3
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_WIDTH                   3
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_MASK                    0x38
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_RESET                   0x4
        
        // MRAM EN timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_BIT                        6
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_WIDTH                      3
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_MASK                       0x1c0
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_RESET                      0x4
        
        // Latency from write to read timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_BIT                         9
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_WIDTH                       7
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_MASK                        0xfe00
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_RESET                       0x78
        
        // Address setup time couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_BIT                        16
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_WIDTH                      3
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_MASK                       0x70000
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_RESET                      0x4
        
        // Program Setup time couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_BIT                        19
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_WIDTH                      10
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_MASK                       0x1ff80000
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_RESET                      0x320
        
        // Program Min Pulse Width timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_BIT                       29
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_WIDTH                     3
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_MASK                      0xe0000000
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_RESET                     0x8

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_RX_DEST_DEST_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_RX_DEST_DEST_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_RX_DEST_DEST_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_RX_DEST_DEST(val)                        ((val) << 0)
        
        #define UDMA_MRAM_TX_DEST_DEST_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_TX_DEST_DEST_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_TX_DEST_DEST_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_TX_DEST_DEST(val)                        ((val) << 0)
        
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA(val)                 ((val) << 0)
        
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN(val)                   ((val) << 1)
        
        #define UDMA_MRAM_TRANS_ADDR_ADDR_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR(val)                     ((val) << 0)
        
        #define UDMA_MRAM_TRANS_SIZE_SIZE_GET(value)               (GAP_BEXTRACTU((value),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE_GETS(value)              (GAP_BEXTRACT((value),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE(val)                     ((val) << 0)
        
        #define UDMA_MRAM_TRANS_CFG_RXTX_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX(val)                      ((val) << 0)
        
        #define UDMA_MRAM_TRANS_CFG_VALID_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID(val)                     ((val) << 1)
        
        #define UDMA_MRAM_EXT_ADDR_ADDR_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR(val)                       ((val) << 0)
        
        #define UDMA_MRAM_STATUS_ERASE_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY(val)                   ((val) << 0)
        
        #define UDMA_MRAM_STATUS_TX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY(val)                      ((val) << 1)
        
        #define UDMA_MRAM_STATUS_RX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY(val)                      ((val) << 2)
        
        #define UDMA_MRAM_STATUS_EC_ERR_GET(value)                 (GAP_BEXTRACTU((value),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR_GETS(value)                (GAP_BEXTRACT((value),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR_SET(value,field)           (GAP_BINSERT((value),(field),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR(val)                       ((val) << 4)
        
        #define UDMA_MRAM_STATUS_UE_ERR_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR(val)                       ((val) << 5)
        
        #define UDMA_MRAM_MODE_ECCBYPS_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS(val)                        ((val) << 0)
        
        #define UDMA_MRAM_MODE_DPD_GET(value)                      (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_MODE_DPD_GETS(value)                     (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_MODE_DPD_SET(value,field)                (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_MODE_DPD(val)                            ((val) << 1)
        
        #define UDMA_MRAM_MODE_TMEN_GET(value)                     (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_MODE_TMEN_GETS(value)                    (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_MODE_TMEN_SET(value,field)               (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_MODE_TMEN(val)                           ((val) << 3)
        
        #define UDMA_MRAM_MODE_NVR_GET(value)                      (GAP_BEXTRACTU((value),1,4))
        #define UDMA_MRAM_MODE_NVR_GETS(value)                     (GAP_BEXTRACT((value),1,4))
        #define UDMA_MRAM_MODE_NVR_SET(value,field)                (GAP_BINSERT((value),(field),1,4))
        #define UDMA_MRAM_MODE_NVR(val)                            ((val) << 4)
        
        #define UDMA_MRAM_MODE_RSTB_GET(value)                     (GAP_BEXTRACTU((value),1,5))
        #define UDMA_MRAM_MODE_RSTB_GETS(value)                    (GAP_BEXTRACT((value),1,5))
        #define UDMA_MRAM_MODE_RSTB_SET(value,field)               (GAP_BINSERT((value),(field),1,5))
        #define UDMA_MRAM_MODE_RSTB(val)                           ((val) << 5)
        
        #define UDMA_MRAM_MODE_RETB_GET(value)                     (GAP_BEXTRACTU((value),1,6))
        #define UDMA_MRAM_MODE_RETB_GETS(value)                    (GAP_BEXTRACT((value),1,6))
        #define UDMA_MRAM_MODE_RETB_SET(value,field)               (GAP_BINSERT((value),(field),1,6))
        #define UDMA_MRAM_MODE_RETB(val)                           ((val) << 6)
        
        #define UDMA_MRAM_MODE_PORB_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define UDMA_MRAM_MODE_PORB_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define UDMA_MRAM_MODE_PORB_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define UDMA_MRAM_MODE_PORB(val)                           ((val) << 7)
        
        #define UDMA_MRAM_MODE_OPERATION_GET(value)                (GAP_BEXTRACTU((value),8,8))
        #define UDMA_MRAM_MODE_OPERATION_GETS(value)               (GAP_BEXTRACT((value),8,8))
        #define UDMA_MRAM_MODE_OPERATION_SET(value,field)          (GAP_BINSERT((value),(field),8,8))
        #define UDMA_MRAM_MODE_OPERATION(val)                      ((val) << 8)
        
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB(val)                 ((val) << 0)
        
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_GET(value)           (GAP_BEXTRACTU((value),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_GETS(value)          (GAP_BEXTRACT((value),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_SET(value,field)     (GAP_BINSERT((value),(field),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB(val)                 ((val) << 4)
        
        #define UDMA_MRAM_ERASE_SIZE_SIZE_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE(val)                     ((val) << 0)
        
        #define UDMA_MRAM_CLK_DIV_DATA_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA(val)                        ((val) << 0)
        
        #define UDMA_MRAM_CLK_DIV_VALID_GET(value)                 (GAP_BEXTRACTU((value),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID_GETS(value)                (GAP_BEXTRACT((value),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID_SET(value,field)           (GAP_BINSERT((value),(field),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID(val)                       ((val) << 8)
        
        #define UDMA_MRAM_CLK_DIV_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE(val)                      ((val) << 9)
        
        #define UDMA_MRAM_ISR_ERASE_DONE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE(val)                      ((val) << 0)
        
        #define UDMA_MRAM_ISR_PROGRAM_DONE_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE(val)                    ((val) << 1)
        
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_GET(value)          (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_GETS(value)         (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_SET(value,field)    (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE(val)                ((val) << 2)
        
        #define UDMA_MRAM_ISR_RX_DONE_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_ISR_RX_DONE_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_ISR_RX_DONE_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_ISR_RX_DONE(val)                         ((val) << 3)
        
        #define UDMA_MRAM_IER_ERASE_EN_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_IER_ERASE_EN_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_IER_ERASE_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_IER_ERASE_EN(val)                        ((val) << 0)
        
        #define UDMA_MRAM_IER_PROGRAM_EN_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN(val)                      ((val) << 1)
        
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_GET(value)            (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_GETS(value)           (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN(val)                  ((val) << 2)
        
        #define UDMA_MRAM_IER_RX_DONE_EN_GET(value)                (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN_GETS(value)               (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN(val)                      ((val) << 3)
        
        #define UDMA_MRAM_IER_XIP_ERASE_EN_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN(val)                    ((val) << 0)
        
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN(val)                  ((val) << 1)
        
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_GET(value)        (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_GETS(value)       (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_SET(value,field)  (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN(val)              ((val) << 2)
        
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_GET(value)            (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_GETS(value)           (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN(val)                  ((val) << 3)
        
        #define UDMA_MRAM_LINE_2D_LINE_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_LINE_2D_LINE_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_LINE_2D_LINE_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_LINE_2D_LINE(val)                        ((val) << 0)
        
        #define UDMA_MRAM_STRIDE_2D_STRIDE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE(val)                    ((val) << 0)
        
        #define UDMA_MRAM_ENABLE_2D_ENABLE_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE(val)                    ((val) << 0)
        
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_GET(value)    (GAP_BEXTRACTU((value),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_GETS(value)   (GAP_BEXTRACT((value),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT(val)          ((val) << 0)
        
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_GET(value)    (GAP_BEXTRACTU((value),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_GETS(value)   (GAP_BEXTRACT((value),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT(val)          ((val) << 3)
        
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT(val)             ((val) << 6)
        
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_GET(value)        (GAP_BEXTRACTU((value),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_GETS(value)       (GAP_BEXTRACT((value),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_SET(value,field)  (GAP_BINSERT((value),(field),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT(val)              ((val) << 9)
        
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT(val)             ((val) << 16)
        
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT(val)             ((val) << 19)
        
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_GET(value)      (GAP_BEXTRACTU((value),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_GETS(value)     (GAP_BEXTRACT((value),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT(val)            ((val) << 29)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_MRAM_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t rx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t tx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t trans_mode;  // Configure transaction mode
            volatile uint32_t trans_addr;  // Configure each transaction addr
            volatile uint32_t trans_size;  // Configure each transaction size
            volatile uint32_t trans_cfg;  // Start  each transaction rx/tx
            volatile uint32_t ext_addr;  // Memory access address register.
            volatile uint32_t status;  // MRAM status register.
            volatile uint32_t mode;  // MRAM mode configure register.
            volatile uint32_t erase_addr;  // Erase address register.
            volatile uint32_t erase_size;  // Erase size register.
            volatile uint32_t clk_div;  // Clock divide.
            volatile uint32_t isr;  // IRQ status register.
            volatile uint32_t ier;  // IRQ enable register.
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t line_2d;  // OSPI 2D line.
            volatile uint32_t stride_2d;  // OSPI 2D stride.
            volatile uint32_t enable_2d;  // 2D mode enable.
            volatile uint32_t timing_cfg;  // Timing configuration.
        } __attribute__((packed)) udma_mram_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_rx_dest_t;
        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_tx_dest_t;
        
        typedef union {
          struct {
            unsigned int auto_ena        :1 ; // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN
            unsigned int xip_en          :1 ; // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_mode_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Transfer addr, only when MODE is in AUTO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_addr_t;
        
        typedef union {
          struct {
            unsigned int size            :21; // Transfer Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_size_t;
        
        typedef union {
          struct {
            unsigned int rxtx            :1 ; // Transfer type - 1'b0: TX - 1'b1: RX
            unsigned int valid           :1 ; // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_cfg_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Memory access address bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_ext_addr_t;
        
        typedef union {
          struct {
            unsigned int erase_busy      :1 ; // Erase busy: - 1'b0: no busy - 1'b1: busy
            unsigned int tx_busy         :1 ; // TX busy: - 1'b0: no busy - 1'b1: busy
            unsigned int rx_busy         :1 ; // RX busy: - 1'b0: no busy - 1'b1: busy
            unsigned int padding0:1 ;
            unsigned int ec_err          :1 ; // ECC Error Correction, Low indicates no ECC error detected. High indicates 1 or 2 bit errors corrected.
            unsigned int ue_err          :1 ; // Unrecoverable Error, High indicates unrecoverable errors detected. Not 100% accurate for 3 or more bit errors. In the case of 3-bit errors,correct detection is 84%
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_status_t;
        
        typedef union {
          struct {
            unsigned int eccbyps         :1 ; // Signal ECC bypass
            unsigned int dpd             :1 ; // Signal Deep power down
            unsigned int padding0:1 ;
            unsigned int tmen            :1 ; // Signal TMEN
            unsigned int nvr             :1 ; // Signal NVR
            unsigned int rstb            :1 ; // Signal RSTb
            unsigned int retb            :1 ; // Signal RETb
            unsigned int porb            :1 ; // Signal PORb
            unsigned int operation       :8 ; // MRAM operation configure, -8'h00 power up -8'h01 trim configure -8'h02 program -8'h04 chip erase -8'h08 sector erase -8'h10 word erase -8'h20 power down -8'h40 read
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_mode_t;
        
        typedef union {
          struct {
            unsigned int addr_lsb        :4 ; // Erase addr, lower 4 bit is useless
            unsigned int addr_msb        :17; // Erase addr
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_erase_addr_t;
        
        typedef union {
          struct {
            unsigned int size            :7 ; // Erase Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_erase_size_t;
        
        typedef union {
          struct {
            unsigned int data            :8 ; // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4 
            unsigned int valid           :1 ; // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
            unsigned int enable          :1 ; // MRAM Clock enable.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_clk_div_t;
        
        typedef union {
          struct {
            unsigned int erase_done      :1 ; // MRAM erase finish
            unsigned int program_done    :1 ; // MRAM program finish
            unsigned int trim_config_done:1 ; // MRAM trim config finish
            unsigned int rx_done         :1 ; // MRAM read finish.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_isr_t;
        
        typedef union {
          struct {
            unsigned int erase_en        :1 ; // MRAM erase IRQ enable
            unsigned int program_en      :1 ; // MRAM program IRQ enable
            unsigned int trim_config_en  :1 ; // MRAM trim config IRQ enable
            unsigned int rx_done_en      :1 ; // MRAM read IRQ enable.
            unsigned int xip_erase_en    :1 ; // MRAM xip erase IRQ enable
            unsigned int xip_program_en  :1 ; // MRAM xip program IRQ enable
            unsigned int xip_trim_config_en:1 ; // MRAM xip trim config IRQ enable
            unsigned int rx_xip_done_en  :1 ; // MRAM xip read IRQ enable.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_ier_t;
        
        typedef union {
          struct {
            unsigned int line            :32; // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_line_2d_t;
        
        typedef union {
          struct {
            unsigned int stride          :32; // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_stride_2d_t;
        
        typedef union {
          struct {
            unsigned int enable          :1 ; // MRAM 2D mode enable :  - 1'b0: 2D mode disable -  1'b1: 2D mode disable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_enable_2d_t;
        
        typedef union {
          struct {
            unsigned int strobe_time_cnt :3 ; // Strobe timing couner
            unsigned int go_sup_time_cnt :3 ; // Power Supply timing couner
            unsigned int men_time_cnt    :3 ; // MRAM EN timing couner
            unsigned int rw_time_cnt     :7 ; // Latency from write to read timing couner
            unsigned int ads_time_cnt    :3 ; // Address setup time couner
            unsigned int pgs_time_cnt    :10; // Program Setup time couner
            unsigned int prog_time_cnt   :3 ; // Program Min Pulse Width timing couner
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_timing_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_mram : public vp::regmap
        {
        public:
            vp_udma_mram_rx_dest rx_dest;
            vp_udma_mram_tx_dest tx_dest;
            vp_udma_mram_trans_mode trans_mode;
            vp_udma_mram_trans_addr trans_addr;
            vp_udma_mram_trans_size trans_size;
            vp_udma_mram_trans_cfg trans_cfg;
            vp_udma_mram_ext_addr ext_addr;
            vp_udma_mram_status status;
            vp_udma_mram_mode mode;
            vp_udma_mram_erase_addr erase_addr;
            vp_udma_mram_erase_size erase_size;
            vp_udma_mram_clk_div clk_div;
            vp_udma_mram_isr isr;
            vp_udma_mram_ier ier;
            vp_udma_mram_line_2d line_2d;
            vp_udma_mram_stride_2d stride_2d;
            vp_udma_mram_enable_2d enable_2d;
            vp_udma_mram_timing_cfg timing_cfg;
        };

|

.. _udma_mram_RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+----+---------------------------------------------------------------------+
    |Bit #|R/W|Name|                             Description                             |
    +=====+===+====+=====================================================================+
    |7:0  |R/W|DEST|Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+----+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_MRAM_RX_DEST_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_rx_dest_get(uint32_t base);
        static inline void udma_mram_rx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_MRAM_RX_DEST_DEST_BIT                                   0
        #define UDMA_MRAM_RX_DEST_DEST_WIDTH                                 8
        #define UDMA_MRAM_RX_DEST_DEST_MASK                                  0xff
        #define UDMA_MRAM_RX_DEST_DEST_RESET                                 0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_RX_DEST_DEST_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_RX_DEST_DEST_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_RX_DEST_DEST_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_RX_DEST_DEST(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_rx_dest : public vp::reg_32
        {
        public:
            inline void dest_set(uint32_t value);
            inline uint32_t dest_get();
        };

|

.. _udma_mram_TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+----+---------------------------------------------------------------------+
    |Bit #|R/W|Name|                             Description                             |
    +=====+===+====+=====================================================================+
    |7:0  |R/W|DEST|Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)|
    +-----+---+----+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_MRAM_TX_DEST_OFFSET                 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_tx_dest_get(uint32_t base);
        static inline void udma_mram_tx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled) (access: R/W)
        #define UDMA_MRAM_TX_DEST_DEST_BIT                                   0
        #define UDMA_MRAM_TX_DEST_DEST_WIDTH                                 8
        #define UDMA_MRAM_TX_DEST_DEST_MASK                                  0xff
        #define UDMA_MRAM_TX_DEST_DEST_RESET                                 0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TX_DEST_DEST_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_TX_DEST_DEST_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_TX_DEST_DEST_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_TX_DEST_DEST(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest            :8 ; // Stream ID for the CMD uDMA channel. Default is 0xFF(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_tx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_tx_dest : public vp::reg_32
        {
        public:
            inline void dest_set(uint32_t value);
            inline uint32_t dest_get();
        };

|

.. _udma_mram_TRANS_MODE:

TRANS_MODE
""""""""""

Configure transaction mode

.. table:: 

    +-----+---+--------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                               Description                                                                                |
    +=====+===+========+==========================================================================================================================================================================+
    |    0|R/W|AUTO_ENA|Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN|
    +-----+---+--------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|XIP_EN  |Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN       |
    +-----+---+--------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure transaction mode
                #define UDMA_MRAM_TRANS_MODE_OFFSET              0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_trans_mode_get(uint32_t base);
        static inline void udma_mram_trans_mode_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN (access: R/W)
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_BIT                            0
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_WIDTH                          1
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_MASK                           0x1
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_RESET                          0x0
        
        // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN (access: R/W)
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_BIT                              1
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_WIDTH                            1
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_MASK                             0x2
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_TRANS_MODE_AUTO_ENA(val)                 ((val) << 0)
        
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_TRANS_MODE_XIP_EN(val)                   ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int auto_ena        :1 ; // Transfer mode in AUTO, IP will configure the UDMA transfer automatically using register parameters instead using SW configuration in UDMA - 1'b0: AUTO_DIS - 1'b1: AUTO_EN
            unsigned int xip_en          :1 ; // Transfer mode in XIP, IP will configure the UDMA transfer automatically using XIP parameters  instead using SW configuration in UDMA - 1'b0: XIP_DIS - 1'b1: XIP_EN
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_mode_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_trans_mode : public vp::reg_32
        {
        public:
            inline void auto_ena_set(uint32_t value);
            inline uint32_t auto_ena_get();
            inline void xip_en_set(uint32_t value);
            inline uint32_t xip_en_get();
        };

|

.. _udma_mram_TRANS_ADDR:

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
                #define UDMA_MRAM_TRANS_ADDR_OFFSET              0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_trans_addr_get(uint32_t base);
        static inline void udma_mram_trans_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer addr, only when MODE is in AUTO (access: R/W)
        #define UDMA_MRAM_TRANS_ADDR_ADDR_BIT                                0
        #define UDMA_MRAM_TRANS_ADDR_ADDR_WIDTH                              32
        #define UDMA_MRAM_TRANS_ADDR_ADDR_MASK                               0xffffffff
        #define UDMA_MRAM_TRANS_ADDR_ADDR_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TRANS_ADDR_ADDR_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_TRANS_ADDR_ADDR(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Transfer addr, only when MODE is in AUTO
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_trans_addr : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _udma_mram_TRANS_SIZE:

TRANS_SIZE
""""""""""

Configure each transaction size

.. table:: 

    +-----+---+----+-------------+
    |Bit #|R/W|Name| Description |
    +=====+===+====+=============+
    |20:0 |R/W|SIZE|Transfer Size|
    +-----+---+----+-------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure each transaction size
                #define UDMA_MRAM_TRANS_SIZE_OFFSET              0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_trans_size_get(uint32_t base);
        static inline void udma_mram_trans_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer Size (access: R/W)
        #define UDMA_MRAM_TRANS_SIZE_SIZE_BIT                                0
        #define UDMA_MRAM_TRANS_SIZE_SIZE_WIDTH                              21
        #define UDMA_MRAM_TRANS_SIZE_SIZE_MASK                               0x1fffff
        #define UDMA_MRAM_TRANS_SIZE_SIZE_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TRANS_SIZE_SIZE_GET(value)               (GAP_BEXTRACTU((value),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE_GETS(value)              (GAP_BEXTRACT((value),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),21,0))
        #define UDMA_MRAM_TRANS_SIZE_SIZE(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :21; // Transfer Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_trans_size : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _udma_mram_TRANS_CFG:

TRANS_CFG
"""""""""

Start  each transaction rx/tx

.. table:: 

    +-----+---+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                Description                                |
    +=====+===+=====+===========================================================================+
    |    0|R/W|RXTX |Transfer type - 1'b0: TX - 1'b1: RX                                        |
    +-----+---+-----+---------------------------------------------------------------------------+
    |    1|R/W|VALID|Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start|
    +-----+---+-----+---------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start  each transaction rx/tx
                #define UDMA_MRAM_TRANS_CFG_OFFSET               0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_trans_cfg_get(uint32_t base);
        static inline void udma_mram_trans_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer type - 1'b0: TX - 1'b1: RX (access: R/W)
        #define UDMA_MRAM_TRANS_CFG_RXTX_BIT                                 0
        #define UDMA_MRAM_TRANS_CFG_RXTX_WIDTH                               1
        #define UDMA_MRAM_TRANS_CFG_RXTX_MASK                                0x1
        #define UDMA_MRAM_TRANS_CFG_RXTX_RESET                               0x0
        
        // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start (access: R/W)
        #define UDMA_MRAM_TRANS_CFG_VALID_BIT                                1
        #define UDMA_MRAM_TRANS_CFG_VALID_WIDTH                              1
        #define UDMA_MRAM_TRANS_CFG_VALID_MASK                               0x2
        #define UDMA_MRAM_TRANS_CFG_VALID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TRANS_CFG_RXTX_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_TRANS_CFG_RXTX(val)                      ((val) << 0)
        
        #define UDMA_MRAM_TRANS_CFG_VALID_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_TRANS_CFG_VALID(val)                     ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rxtx            :1 ; // Transfer type - 1'b0: TX - 1'b1: RX
            unsigned int valid           :1 ; // Transfer valid to start, always read 0 - 1'b0: clear transfer - 1'b1: Start
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_trans_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_trans_cfg : public vp::reg_32
        {
        public:
            inline void rxtx_set(uint32_t value);
            inline uint32_t rxtx_get();
            inline void valid_set(uint32_t value);
            inline uint32_t valid_get();
        };

|

.. _udma_mram_EXT_ADDR:

EXT_ADDR
""""""""

Memory access address register.

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |31:0 |R/W|ADDR|Memory access address bitfield.|
    +-----+---+----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Memory access address register.
                #define UDMA_MRAM_EXT_ADDR_OFFSET                0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_ext_addr_get(uint32_t base);
        static inline void udma_mram_ext_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Memory access address bitfield. (access: R/W)
        #define UDMA_MRAM_EXT_ADDR_ADDR_BIT                                  0
        #define UDMA_MRAM_EXT_ADDR_ADDR_WIDTH                                32
        #define UDMA_MRAM_EXT_ADDR_ADDR_MASK                                 0xffffffff
        #define UDMA_MRAM_EXT_ADDR_ADDR_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_EXT_ADDR_ADDR_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_EXT_ADDR_ADDR(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Memory access address bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_ext_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_ext_addr : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _udma_mram_STATUS:

STATUS
""""""

MRAM status register.

.. table:: 

    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                                            Description                                                                            |
    +=====+===+==========+===================================================================================================================================================================+
    |    0|R  |ERASE_BUSY|Erase busy: - 1'b0: no busy - 1'b1: busy                                                                                                                           |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R  |TX_BUSY   |TX busy: - 1'b0: no busy - 1'b1: busy                                                                                                                              |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R  |RX_BUSY   |RX busy: - 1'b0: no busy - 1'b1: busy                                                                                                                              |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R  |EC_ERR    |ECC Error Correction, Low indicates no ECC error detected. High indicates 1 or 2 bit errors corrected.                                                             |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R  |UE_ERR    |Unrecoverable Error, High indicates unrecoverable errors detected. Not 100% accurate for 3 or more bit errors. In the case of 3-bit errors,correct detection is 84%|
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MRAM status register.
                #define UDMA_MRAM_STATUS_OFFSET                  0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_status_get(uint32_t base);
        static inline void udma_mram_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Erase busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_ERASE_BUSY_BIT                              0
        #define UDMA_MRAM_STATUS_ERASE_BUSY_WIDTH                            1
        #define UDMA_MRAM_STATUS_ERASE_BUSY_MASK                             0x1
        #define UDMA_MRAM_STATUS_ERASE_BUSY_RESET                            0x0
        
        // TX busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_TX_BUSY_BIT                                 1
        #define UDMA_MRAM_STATUS_TX_BUSY_WIDTH                               1
        #define UDMA_MRAM_STATUS_TX_BUSY_MASK                                0x2
        #define UDMA_MRAM_STATUS_TX_BUSY_RESET                               0x0
        
        // RX busy: - 1'b0: no busy - 1'b1: busy (access: R)
        #define UDMA_MRAM_STATUS_RX_BUSY_BIT                                 2
        #define UDMA_MRAM_STATUS_RX_BUSY_WIDTH                               1
        #define UDMA_MRAM_STATUS_RX_BUSY_MASK                                0x4
        #define UDMA_MRAM_STATUS_RX_BUSY_RESET                               0x0
        
        // ECC Error Correction, Low indicates no ECC error detected. High indicates 1 or 2 bit errors corrected. (access: R)
        #define UDMA_MRAM_STATUS_EC_ERR_BIT                                  4
        #define UDMA_MRAM_STATUS_EC_ERR_WIDTH                                1
        #define UDMA_MRAM_STATUS_EC_ERR_MASK                                 0x10
        #define UDMA_MRAM_STATUS_EC_ERR_RESET                                0x0
        
        // Unrecoverable Error, High indicates unrecoverable errors detected. Not 100% accurate for 3 or more bit errors. In the case of 3-bit errors,correct detection is 84% (access: R)
        #define UDMA_MRAM_STATUS_UE_ERR_BIT                                  5
        #define UDMA_MRAM_STATUS_UE_ERR_WIDTH                                1
        #define UDMA_MRAM_STATUS_UE_ERR_MASK                                 0x20
        #define UDMA_MRAM_STATUS_UE_ERR_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_STATUS_ERASE_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_STATUS_ERASE_BUSY(val)                   ((val) << 0)
        
        #define UDMA_MRAM_STATUS_TX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_STATUS_TX_BUSY(val)                      ((val) << 1)
        
        #define UDMA_MRAM_STATUS_RX_BUSY_GET(value)                (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY_GETS(value)               (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY_SET(value,field)          (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_STATUS_RX_BUSY(val)                      ((val) << 2)
        
        #define UDMA_MRAM_STATUS_EC_ERR_GET(value)                 (GAP_BEXTRACTU((value),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR_GETS(value)                (GAP_BEXTRACT((value),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR_SET(value,field)           (GAP_BINSERT((value),(field),1,4))
        #define UDMA_MRAM_STATUS_EC_ERR(val)                       ((val) << 4)
        
        #define UDMA_MRAM_STATUS_UE_ERR_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define UDMA_MRAM_STATUS_UE_ERR(val)                       ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int erase_busy      :1 ; // Erase busy: - 1'b0: no busy - 1'b1: busy
            unsigned int tx_busy         :1 ; // TX busy: - 1'b0: no busy - 1'b1: busy
            unsigned int rx_busy         :1 ; // RX busy: - 1'b0: no busy - 1'b1: busy
            unsigned int padding0:1 ;
            unsigned int ec_err          :1 ; // ECC Error Correction, Low indicates no ECC error detected. High indicates 1 or 2 bit errors corrected.
            unsigned int ue_err          :1 ; // Unrecoverable Error, High indicates unrecoverable errors detected. Not 100% accurate for 3 or more bit errors. In the case of 3-bit errors,correct detection is 84%
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_status : public vp::reg_32
        {
        public:
            inline void erase_busy_set(uint32_t value);
            inline uint32_t erase_busy_get();
            inline void tx_busy_set(uint32_t value);
            inline uint32_t tx_busy_get();
            inline void rx_busy_set(uint32_t value);
            inline uint32_t rx_busy_get();
            inline void ec_err_set(uint32_t value);
            inline uint32_t ec_err_get();
            inline void ue_err_set(uint32_t value);
            inline uint32_t ue_err_get();
        };

|

.. _udma_mram_MODE:

MODE
""""

MRAM mode configure register.

.. table:: 

    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                            Description                                                                             |
    +=====+===+=========+====================================================================================================================================================================+
    |    0|R/W|ECCBYPS  |Signal ECC bypass                                                                                                                                                   |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|DPD      |Signal Deep power down                                                                                                                                              |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|TMEN     |Signal TMEN                                                                                                                                                         |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|NVR      |Signal NVR                                                                                                                                                          |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|RSTb     |Signal RSTb                                                                                                                                                         |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|RETb     |Signal RETb                                                                                                                                                         |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|PORb     |Signal PORb                                                                                                                                                         |
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|OPERATION|MRAM operation configure, -8'h00 power up -8'h01 trim configure -8'h02 program -8'h04 chip erase -8'h08 sector erase -8'h10 word erase -8'h20 power down -8'h40 read|
    +-----+---+---------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // MRAM mode configure register.
                #define UDMA_MRAM_MODE_OFFSET                    0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_mode_get(uint32_t base);
        static inline void udma_mram_mode_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Signal ECC bypass (access: R/W)
        #define UDMA_MRAM_MODE_ECCBYPS_BIT                                   0
        #define UDMA_MRAM_MODE_ECCBYPS_WIDTH                                 1
        #define UDMA_MRAM_MODE_ECCBYPS_MASK                                  0x1
        #define UDMA_MRAM_MODE_ECCBYPS_RESET                                 0x0
        
        // Signal Deep power down (access: R/W)
        #define UDMA_MRAM_MODE_DPD_BIT                                       1
        #define UDMA_MRAM_MODE_DPD_WIDTH                                     1
        #define UDMA_MRAM_MODE_DPD_MASK                                      0x2
        #define UDMA_MRAM_MODE_DPD_RESET                                     0x0
        
        // Signal TMEN (access: R/W)
        #define UDMA_MRAM_MODE_TMEN_BIT                                      3
        #define UDMA_MRAM_MODE_TMEN_WIDTH                                    1
        #define UDMA_MRAM_MODE_TMEN_MASK                                     0x8
        #define UDMA_MRAM_MODE_TMEN_RESET                                    0x0
        
        // Signal NVR (access: R/W)
        #define UDMA_MRAM_MODE_NVR_BIT                                       4
        #define UDMA_MRAM_MODE_NVR_WIDTH                                     1
        #define UDMA_MRAM_MODE_NVR_MASK                                      0x10
        #define UDMA_MRAM_MODE_NVR_RESET                                     0x0
        
        // Signal RSTb (access: R/W)
        #define UDMA_MRAM_MODE_RSTB_BIT                                      5
        #define UDMA_MRAM_MODE_RSTB_WIDTH                                    1
        #define UDMA_MRAM_MODE_RSTB_MASK                                     0x20
        #define UDMA_MRAM_MODE_RSTB_RESET                                    0x0
        
        // Signal RETb (access: R/W)
        #define UDMA_MRAM_MODE_RETB_BIT                                      6
        #define UDMA_MRAM_MODE_RETB_WIDTH                                    1
        #define UDMA_MRAM_MODE_RETB_MASK                                     0x40
        #define UDMA_MRAM_MODE_RETB_RESET                                    0x0
        
        // Signal PORb (access: R/W)
        #define UDMA_MRAM_MODE_PORB_BIT                                      7
        #define UDMA_MRAM_MODE_PORB_WIDTH                                    1
        #define UDMA_MRAM_MODE_PORB_MASK                                     0x80
        #define UDMA_MRAM_MODE_PORB_RESET                                    0x0
        
        // MRAM operation configure, -8'h00 power up -8'h01 trim configure -8'h02 program -8'h04 chip erase -8'h08 sector erase -8'h10 word erase -8'h20 power down -8'h40 read (access: R/W)
        #define UDMA_MRAM_MODE_OPERATION_BIT                                 8
        #define UDMA_MRAM_MODE_OPERATION_WIDTH                               8
        #define UDMA_MRAM_MODE_OPERATION_MASK                                0xff00
        #define UDMA_MRAM_MODE_OPERATION_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_MODE_ECCBYPS_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_MODE_ECCBYPS(val)                        ((val) << 0)
        
        #define UDMA_MRAM_MODE_DPD_GET(value)                      (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_MODE_DPD_GETS(value)                     (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_MODE_DPD_SET(value,field)                (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_MODE_DPD(val)                            ((val) << 1)
        
        #define UDMA_MRAM_MODE_TMEN_GET(value)                     (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_MODE_TMEN_GETS(value)                    (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_MODE_TMEN_SET(value,field)               (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_MODE_TMEN(val)                           ((val) << 3)
        
        #define UDMA_MRAM_MODE_NVR_GET(value)                      (GAP_BEXTRACTU((value),1,4))
        #define UDMA_MRAM_MODE_NVR_GETS(value)                     (GAP_BEXTRACT((value),1,4))
        #define UDMA_MRAM_MODE_NVR_SET(value,field)                (GAP_BINSERT((value),(field),1,4))
        #define UDMA_MRAM_MODE_NVR(val)                            ((val) << 4)
        
        #define UDMA_MRAM_MODE_RSTB_GET(value)                     (GAP_BEXTRACTU((value),1,5))
        #define UDMA_MRAM_MODE_RSTB_GETS(value)                    (GAP_BEXTRACT((value),1,5))
        #define UDMA_MRAM_MODE_RSTB_SET(value,field)               (GAP_BINSERT((value),(field),1,5))
        #define UDMA_MRAM_MODE_RSTB(val)                           ((val) << 5)
        
        #define UDMA_MRAM_MODE_RETB_GET(value)                     (GAP_BEXTRACTU((value),1,6))
        #define UDMA_MRAM_MODE_RETB_GETS(value)                    (GAP_BEXTRACT((value),1,6))
        #define UDMA_MRAM_MODE_RETB_SET(value,field)               (GAP_BINSERT((value),(field),1,6))
        #define UDMA_MRAM_MODE_RETB(val)                           ((val) << 6)
        
        #define UDMA_MRAM_MODE_PORB_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define UDMA_MRAM_MODE_PORB_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define UDMA_MRAM_MODE_PORB_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define UDMA_MRAM_MODE_PORB(val)                           ((val) << 7)
        
        #define UDMA_MRAM_MODE_OPERATION_GET(value)                (GAP_BEXTRACTU((value),8,8))
        #define UDMA_MRAM_MODE_OPERATION_GETS(value)               (GAP_BEXTRACT((value),8,8))
        #define UDMA_MRAM_MODE_OPERATION_SET(value,field)          (GAP_BINSERT((value),(field),8,8))
        #define UDMA_MRAM_MODE_OPERATION(val)                      ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int eccbyps         :1 ; // Signal ECC bypass
            unsigned int dpd             :1 ; // Signal Deep power down
            unsigned int padding0:1 ;
            unsigned int tmen            :1 ; // Signal TMEN
            unsigned int nvr             :1 ; // Signal NVR
            unsigned int rstb            :1 ; // Signal RSTb
            unsigned int retb            :1 ; // Signal RETb
            unsigned int porb            :1 ; // Signal PORb
            unsigned int operation       :8 ; // MRAM operation configure, -8'h00 power up -8'h01 trim configure -8'h02 program -8'h04 chip erase -8'h08 sector erase -8'h10 word erase -8'h20 power down -8'h40 read
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_mode_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_mode : public vp::reg_32
        {
        public:
            inline void eccbyps_set(uint32_t value);
            inline uint32_t eccbyps_get();
            inline void dpd_set(uint32_t value);
            inline uint32_t dpd_get();
            inline void tmen_set(uint32_t value);
            inline uint32_t tmen_get();
            inline void nvr_set(uint32_t value);
            inline uint32_t nvr_get();
            inline void rstb_set(uint32_t value);
            inline uint32_t rstb_get();
            inline void retb_set(uint32_t value);
            inline uint32_t retb_get();
            inline void porb_set(uint32_t value);
            inline uint32_t porb_get();
            inline void operation_set(uint32_t value);
            inline uint32_t operation_get();
        };

|

.. _udma_mram_ERASE_ADDR:

ERASE_ADDR
""""""""""

Erase address register.

.. table:: 

    +-----+---+--------+----------------------------------+
    |Bit #|R/W|  Name  |           Description            |
    +=====+===+========+==================================+
    |3:0  |R/W|ADDR_LSB|Erase addr, lower 4 bit is useless|
    +-----+---+--------+----------------------------------+
    |20:4 |R/W|ADDR_MSB|Erase addr                        |
    +-----+---+--------+----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Erase address register.
                #define UDMA_MRAM_ERASE_ADDR_OFFSET              0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_erase_addr_get(uint32_t base);
        static inline void udma_mram_erase_addr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Erase addr, lower 4 bit is useless (access: R/W)
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_BIT                            0
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_WIDTH                          4
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_MASK                           0xf
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_RESET                          0x0
        
        // Erase addr (access: R/W)
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_BIT                            4
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_WIDTH                          17
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_MASK                           0x1ffff0
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_LSB(val)                 ((val) << 0)
        
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_GET(value)           (GAP_BEXTRACTU((value),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_GETS(value)          (GAP_BEXTRACT((value),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB_SET(value,field)     (GAP_BINSERT((value),(field),17,4))
        #define UDMA_MRAM_ERASE_ADDR_ADDR_MSB(val)                 ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr_lsb        :4 ; // Erase addr, lower 4 bit is useless
            unsigned int addr_msb        :17; // Erase addr
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_erase_addr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_erase_addr : public vp::reg_32
        {
        public:
            inline void addr_lsb_set(uint32_t value);
            inline uint32_t addr_lsb_get();
            inline void addr_msb_set(uint32_t value);
            inline uint32_t addr_msb_get();
        };

|

.. _udma_mram_ERASE_SIZE:

ERASE_SIZE
""""""""""

Erase size register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    |6:0  |R/W|SIZE|Erase Size |
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Erase size register.
                #define UDMA_MRAM_ERASE_SIZE_OFFSET              0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_erase_size_get(uint32_t base);
        static inline void udma_mram_erase_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Erase Size (access: R/W)
        #define UDMA_MRAM_ERASE_SIZE_SIZE_BIT                                0
        #define UDMA_MRAM_ERASE_SIZE_SIZE_WIDTH                              7
        #define UDMA_MRAM_ERASE_SIZE_SIZE_MASK                               0x7f
        #define UDMA_MRAM_ERASE_SIZE_SIZE_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_ERASE_SIZE_SIZE_GET(value)               (GAP_BEXTRACTU((value),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE_GETS(value)              (GAP_BEXTRACT((value),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE_SET(value,field)         (GAP_BINSERT((value),(field),7,0))
        #define UDMA_MRAM_ERASE_SIZE_SIZE(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :7 ; // Erase Size
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_erase_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_erase_size : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _udma_mram_CLK_DIV:

CLK_DIV
"""""""

Clock divide.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                     Description                                                                     |
    +=====+===+======+=====================================================================================================================================================+
    |7:0  |R/W|DATA  |Clock divide data, form 0 – 255, frequency divide table is : -8’h0 – IO_FREQUENCY / 1 -8’h1 – IO_FREQUENCY / 2 -8’h2 – IO_FREQUENCY / 4 …            |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------+
    |8    |W  |VALID |Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.|
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------+
    |9    |W  |ENABLE|MRAM Clock enable.                                                                                                                                   |
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock divide.
                #define UDMA_MRAM_CLK_DIV_OFFSET                 0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_clk_div_get(uint32_t base);
        static inline void udma_mram_clk_div_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4  (access: R/W)
        #define UDMA_MRAM_CLK_DIV_DATA_BIT                                   0
        #define UDMA_MRAM_CLK_DIV_DATA_WIDTH                                 8
        #define UDMA_MRAM_CLK_DIV_DATA_MASK                                  0xff
        #define UDMA_MRAM_CLK_DIV_DATA_RESET                                 0x0
        
        // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0. (access: W)
        #define UDMA_MRAM_CLK_DIV_VALID_BIT                                  8
        #define UDMA_MRAM_CLK_DIV_VALID_WIDTH                                1
        #define UDMA_MRAM_CLK_DIV_VALID_MASK                                 0x100
        #define UDMA_MRAM_CLK_DIV_VALID_RESET                                0x0
        
        // MRAM Clock enable. (access: W)
        #define UDMA_MRAM_CLK_DIV_ENABLE_BIT                                 9
        #define UDMA_MRAM_CLK_DIV_ENABLE_WIDTH                               1
        #define UDMA_MRAM_CLK_DIV_ENABLE_MASK                                0x200
        #define UDMA_MRAM_CLK_DIV_ENABLE_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_CLK_DIV_DATA_GET(value)                  (GAP_BEXTRACTU((value),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA_GETS(value)                 (GAP_BEXTRACT((value),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA_SET(value,field)            (GAP_BINSERT((value),(field),8,0))
        #define UDMA_MRAM_CLK_DIV_DATA(val)                        ((val) << 0)
        
        #define UDMA_MRAM_CLK_DIV_VALID_GET(value)                 (GAP_BEXTRACTU((value),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID_GETS(value)                (GAP_BEXTRACT((value),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID_SET(value,field)           (GAP_BINSERT((value),(field),1,8))
        #define UDMA_MRAM_CLK_DIV_VALID(val)                       ((val) << 8)
        
        #define UDMA_MRAM_CLK_DIV_ENABLE_GET(value)                (GAP_BEXTRACTU((value),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE_GETS(value)               (GAP_BEXTRACT((value),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,9))
        #define UDMA_MRAM_CLK_DIV_ENABLE(val)                      ((val) << 9)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int data            :8 ; // Clock divide data, form 0  255, frequency divide table is : -8h0  IO_FREQUENCY / 1 -8h1  IO_FREQUENCY / 2 -8h2  IO_FREQUENCY / 4 
            unsigned int valid           :1 ; // Clock divide valid, user can not control. Every time there is clock divide write access, set 1 by default, then when clock divide is finished, set 0.
            unsigned int enable          :1 ; // MRAM Clock enable.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_clk_div_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_clk_div : public vp::reg_32
        {
        public:
            inline void data_set(uint32_t value);
            inline uint32_t data_get();
            inline void valid_set(uint32_t value);
            inline uint32_t valid_get();
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|

.. _udma_mram_ISR:

ISR
"""

IRQ status register.

.. table:: 

    +-----+---+----------------+-----------------------+
    |Bit #|R/W|      Name      |      Description      |
    +=====+===+================+=======================+
    |    0|R  |erase_done      |MRAM erase finish      |
    +-----+---+----------------+-----------------------+
    |    1|R  |program_done    |MRAM program finish    |
    +-----+---+----------------+-----------------------+
    |    2|R  |trim_config_done|MRAM trim config finish|
    +-----+---+----------------+-----------------------+
    |    3|R  |rx_done         |MRAM read finish.      |
    +-----+---+----------------+-----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // IRQ status register.
                #define UDMA_MRAM_ISR_OFFSET                     0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_isr_get(uint32_t base);
        static inline void udma_mram_isr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MRAM erase finish (access: R)
        #define UDMA_MRAM_ISR_ERASE_DONE_BIT                                 0
        #define UDMA_MRAM_ISR_ERASE_DONE_WIDTH                               1
        #define UDMA_MRAM_ISR_ERASE_DONE_MASK                                0x1
        #define UDMA_MRAM_ISR_ERASE_DONE_RESET                               0x0
        
        // MRAM program finish (access: R)
        #define UDMA_MRAM_ISR_PROGRAM_DONE_BIT                               1
        #define UDMA_MRAM_ISR_PROGRAM_DONE_WIDTH                             1
        #define UDMA_MRAM_ISR_PROGRAM_DONE_MASK                              0x2
        #define UDMA_MRAM_ISR_PROGRAM_DONE_RESET                             0x0
        
        // MRAM trim config finish (access: R)
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_BIT                           2
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_WIDTH                         1
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_MASK                          0x4
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_RESET                         0x0
        
        // MRAM read finish. (access: R)
        #define UDMA_MRAM_ISR_RX_DONE_BIT                                    3
        #define UDMA_MRAM_ISR_RX_DONE_WIDTH                                  1
        #define UDMA_MRAM_ISR_RX_DONE_MASK                                   0x8
        #define UDMA_MRAM_ISR_RX_DONE_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_ISR_ERASE_DONE_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_ISR_ERASE_DONE(val)                      ((val) << 0)
        
        #define UDMA_MRAM_ISR_PROGRAM_DONE_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_ISR_PROGRAM_DONE(val)                    ((val) << 1)
        
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_GET(value)          (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_GETS(value)         (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE_SET(value,field)    (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_ISR_TRIM_CONFIG_DONE(val)                ((val) << 2)
        
        #define UDMA_MRAM_ISR_RX_DONE_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_ISR_RX_DONE_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_ISR_RX_DONE_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_ISR_RX_DONE(val)                         ((val) << 3)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int erase_done      :1 ; // MRAM erase finish
            unsigned int program_done    :1 ; // MRAM program finish
            unsigned int trim_config_done:1 ; // MRAM trim config finish
            unsigned int rx_done         :1 ; // MRAM read finish.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_isr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_isr : public vp::reg_32
        {
        public:
            inline void erase_done_set(uint32_t value);
            inline uint32_t erase_done_get();
            inline void program_done_set(uint32_t value);
            inline uint32_t program_done_get();
            inline void trim_config_done_set(uint32_t value);
            inline uint32_t trim_config_done_get();
            inline void rx_done_set(uint32_t value);
            inline uint32_t rx_done_get();
        };

|

.. _udma_mram_IER:

IER
"""

IRQ enable register.

.. table:: 

    +-----+---+------------------+-------------------------------+
    |Bit #|R/W|       Name       |          Description          |
    +=====+===+==================+===============================+
    |    0|R/W|erase_en          |MRAM erase IRQ enable          |
    +-----+---+------------------+-------------------------------+
    |    1|R/W|program_en        |MRAM program IRQ enable        |
    +-----+---+------------------+-------------------------------+
    |    2|R/W|trim_config_en    |MRAM trim config IRQ enable    |
    +-----+---+------------------+-------------------------------+
    |    3|R/W|rx_done_en        |MRAM read IRQ enable.          |
    +-----+---+------------------+-------------------------------+
    |    0|R/W|xip_erase_en      |MRAM xip erase IRQ enable      |
    +-----+---+------------------+-------------------------------+
    |    1|R/W|xip_program_en    |MRAM xip program IRQ enable    |
    +-----+---+------------------+-------------------------------+
    |    2|R/W|xip_trim_config_en|MRAM xip trim config IRQ enable|
    +-----+---+------------------+-------------------------------+
    |    3|R/W|rx_xip_done_en    |MRAM xip read IRQ enable.      |
    +-----+---+------------------+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // IRQ enable register.
                #define UDMA_MRAM_IER_OFFSET                     0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_ier_get(uint32_t base);
        static inline void udma_mram_ier_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MRAM erase IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_ERASE_EN_BIT                                   0
        #define UDMA_MRAM_IER_ERASE_EN_WIDTH                                 1
        #define UDMA_MRAM_IER_ERASE_EN_MASK                                  0x1
        #define UDMA_MRAM_IER_ERASE_EN_RESET                                 0x0
        
        // MRAM program IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_PROGRAM_EN_BIT                                 1
        #define UDMA_MRAM_IER_PROGRAM_EN_WIDTH                               1
        #define UDMA_MRAM_IER_PROGRAM_EN_MASK                                0x2
        #define UDMA_MRAM_IER_PROGRAM_EN_RESET                               0x0
        
        // MRAM trim config IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_BIT                             2
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_WIDTH                           1
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_MASK                            0x4
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_RESET                           0x0
        
        // MRAM read IRQ enable. (access: R/W)
        #define UDMA_MRAM_IER_RX_DONE_EN_BIT                                 3
        #define UDMA_MRAM_IER_RX_DONE_EN_WIDTH                               1
        #define UDMA_MRAM_IER_RX_DONE_EN_MASK                                0x8
        #define UDMA_MRAM_IER_RX_DONE_EN_RESET                               0x0
        
        // MRAM xip erase IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_ERASE_EN_BIT                               0
        #define UDMA_MRAM_IER_XIP_ERASE_EN_WIDTH                             1
        #define UDMA_MRAM_IER_XIP_ERASE_EN_MASK                              0x1
        #define UDMA_MRAM_IER_XIP_ERASE_EN_RESET                             0x0
        
        // MRAM xip program IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_BIT                             1
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_WIDTH                           1
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_MASK                            0x2
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_RESET                           0x0
        
        // MRAM xip trim config IRQ enable (access: R/W)
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_BIT                         2
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_WIDTH                       1
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_MASK                        0x4
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_RESET                       0x0
        
        // MRAM xip read IRQ enable. (access: R/W)
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_BIT                             3
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_WIDTH                           1
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_MASK                            0x8
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_IER_ERASE_EN_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_IER_ERASE_EN_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_IER_ERASE_EN_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_IER_ERASE_EN(val)                        ((val) << 0)
        
        #define UDMA_MRAM_IER_PROGRAM_EN_GET(value)                (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN_GETS(value)               (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_IER_PROGRAM_EN(val)                      ((val) << 1)
        
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_GET(value)            (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_GETS(value)           (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_IER_TRIM_CONFIG_EN(val)                  ((val) << 2)
        
        #define UDMA_MRAM_IER_RX_DONE_EN_GET(value)                (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN_GETS(value)               (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_IER_RX_DONE_EN(val)                      ((val) << 3)
        
        #define UDMA_MRAM_IER_XIP_ERASE_EN_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_IER_XIP_ERASE_EN(val)                    ((val) << 0)
        
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define UDMA_MRAM_IER_XIP_PROGRAM_EN(val)                  ((val) << 1)
        
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_GET(value)        (GAP_BEXTRACTU((value),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_GETS(value)       (GAP_BEXTRACT((value),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN_SET(value,field)  (GAP_BINSERT((value),(field),1,2))
        #define UDMA_MRAM_IER_XIP_TRIM_CONFIG_EN(val)              ((val) << 2)
        
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_GET(value)            (GAP_BEXTRACTU((value),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_GETS(value)           (GAP_BEXTRACT((value),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,3))
        #define UDMA_MRAM_IER_RX_XIP_DONE_EN(val)                  ((val) << 3)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int erase_en        :1 ; // MRAM erase IRQ enable
            unsigned int program_en      :1 ; // MRAM program IRQ enable
            unsigned int trim_config_en  :1 ; // MRAM trim config IRQ enable
            unsigned int rx_done_en      :1 ; // MRAM read IRQ enable.
            unsigned int xip_erase_en    :1 ; // MRAM xip erase IRQ enable
            unsigned int xip_program_en  :1 ; // MRAM xip program IRQ enable
            unsigned int xip_trim_config_en:1 ; // MRAM xip trim config IRQ enable
            unsigned int rx_xip_done_en  :1 ; // MRAM xip read IRQ enable.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_ier_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_ier : public vp::reg_32
        {
        public:
            inline void erase_en_set(uint32_t value);
            inline uint32_t erase_en_get();
            inline void program_en_set(uint32_t value);
            inline uint32_t program_en_get();
            inline void trim_config_en_set(uint32_t value);
            inline uint32_t trim_config_en_get();
            inline void rx_done_en_set(uint32_t value);
            inline uint32_t rx_done_en_get();
            inline void xip_erase_en_set(uint32_t value);
            inline uint32_t xip_erase_en_get();
            inline void xip_program_en_set(uint32_t value);
            inline uint32_t xip_program_en_get();
            inline void xip_trim_config_en_set(uint32_t value);
            inline uint32_t xip_trim_config_en_get();
            inline void rx_xip_done_en_set(uint32_t value);
            inline uint32_t rx_xip_done_en_get();
        };

|

.. _udma_mram_LINE_2D:

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
                #define UDMA_MRAM_LINE_2D_OFFSET                 0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_line_2d_get(uint32_t base);
        static inline void udma_mram_line_2d_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_MRAM_LINE_2D_LINE_BIT                                   0
        #define UDMA_MRAM_LINE_2D_LINE_WIDTH                                 32
        #define UDMA_MRAM_LINE_2D_LINE_MASK                                  0xffffffff
        #define UDMA_MRAM_LINE_2D_LINE_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_LINE_2D_LINE_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_LINE_2D_LINE_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_LINE_2D_LINE_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_LINE_2D_LINE(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int line            :32; // OSPI 2D line with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, LINE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_line_2d_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_line_2d : public vp::reg_32
        {
        public:
            inline void line_set(uint32_t value);
            inline uint32_t line_get();
        };

|

.. _udma_mram_STRIDE_2D:

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
                #define UDMA_MRAM_STRIDE_2D_OFFSET               0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_stride_2d_get(uint32_t base);
        static inline void udma_mram_stride_2d_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE. (access: R/W)
        #define UDMA_MRAM_STRIDE_2D_STRIDE_BIT                               0
        #define UDMA_MRAM_STRIDE_2D_STRIDE_WIDTH                             32
        #define UDMA_MRAM_STRIDE_2D_STRIDE_MASK                              0xffffffff
        #define UDMA_MRAM_STRIDE_2D_STRIDE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_STRIDE_2D_STRIDE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define UDMA_MRAM_STRIDE_2D_STRIDE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int stride          :32; // OSPI 2D stride with 2D mode. For example, ADDR = START_ADDR + i * BURST_STRIDE. Normally, STRIDE &gt;= BURST_SIZE.
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_stride_2d_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_stride_2d : public vp::reg_32
        {
        public:
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _udma_mram_ENABLE_2D:

ENABLE_2D
"""""""""

2D mode enable.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------+
    |Bit #|R/W| Name |                              Description                              |
    +=====+===+======+=======================================================================+
    |    0|R/W|ENABLE|MRAM 2D mode enable :  - 1'b0: 2D mode disable -  1'b1: 2D mode disable|
    +-----+---+------+-----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // 2D mode enable.
                #define UDMA_MRAM_ENABLE_2D_OFFSET               0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_enable_2d_get(uint32_t base);
        static inline void udma_mram_enable_2d_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // MRAM 2D mode enable :  - 1'b0: 2D mode disable -  1'b1: 2D mode disable (access: R/W)
        #define UDMA_MRAM_ENABLE_2D_ENABLE_BIT                               0
        #define UDMA_MRAM_ENABLE_2D_ENABLE_WIDTH                             1
        #define UDMA_MRAM_ENABLE_2D_ENABLE_MASK                              0x1
        #define UDMA_MRAM_ENABLE_2D_ENABLE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_ENABLE_2D_ENABLE_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define UDMA_MRAM_ENABLE_2D_ENABLE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // MRAM 2D mode enable :  - 1'b0: 2D mode disable -  1'b1: 2D mode disable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_enable_2d_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_enable_2d : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|

.. _udma_mram_TIMING_CFG:

TIMING_CFG
""""""""""

Timing configuration.

.. table:: 

    +-----+---+---------------+----------------------------------------+
    |Bit #|R/W|     Name      |              Description               |
    +=====+===+===============+========================================+
    |2:0  |R/W|STROBE_TIME_CNT|Strobe timing couner                    |
    +-----+---+---------------+----------------------------------------+
    |5:3  |R/W|GO_SUP_TIME_CNT|Power Supply timing couner              |
    +-----+---+---------------+----------------------------------------+
    |8:6  |R/W|MEN_TIME_CNT   |MRAM EN timing couner                   |
    +-----+---+---------------+----------------------------------------+
    |15:9 |R/W|RW_TIME_CNT    |Latency from write to read timing couner|
    +-----+---+---------------+----------------------------------------+
    |18:16|R/W|ADS_TIME_CNT   |Address setup time couner               |
    +-----+---+---------------+----------------------------------------+
    |28:19|R/W|PGS_TIME_CNT   |Program Setup time couner               |
    +-----+---+---------------+----------------------------------------+
    |31:29|R/W|PROG_TIME_CNT  |Program Min Pulse Width timing couner   |
    +-----+---+---------------+----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Timing configuration.
                #define UDMA_MRAM_TIMING_CFG_OFFSET              0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_mram_timing_cfg_get(uint32_t base);
        static inline void udma_mram_timing_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Strobe timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_BIT                     0
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_WIDTH                   3
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_MASK                    0x7
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_RESET                   0x2
        
        // Power Supply timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_BIT                     3
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_WIDTH                   3
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_MASK                    0x38
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_RESET                   0x4
        
        // MRAM EN timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_BIT                        6
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_WIDTH                      3
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_MASK                       0x1c0
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_RESET                      0x4
        
        // Latency from write to read timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_BIT                         9
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_WIDTH                       7
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_MASK                        0xfe00
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_RESET                       0x78
        
        // Address setup time couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_BIT                        16
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_WIDTH                      3
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_MASK                       0x70000
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_RESET                      0x4
        
        // Program Setup time couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_BIT                        19
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_WIDTH                      10
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_MASK                       0x1ff80000
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_RESET                      0x320
        
        // Program Min Pulse Width timing couner (access: R/W)
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_BIT                       29
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_WIDTH                     3
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_MASK                      0xe0000000
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_RESET                     0x8

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_GET(value)    (GAP_BEXTRACTU((value),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_GETS(value)   (GAP_BEXTRACT((value),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define UDMA_MRAM_TIMING_CFG_STROBE_TIME_CNT(val)          ((val) << 0)
        
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_GET(value)    (GAP_BEXTRACTU((value),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_GETS(value)   (GAP_BEXTRACT((value),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,3))
        #define UDMA_MRAM_TIMING_CFG_GO_SUP_TIME_CNT(val)          ((val) << 3)
        
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,6))
        #define UDMA_MRAM_TIMING_CFG_MEN_TIME_CNT(val)             ((val) << 6)
        
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_GET(value)        (GAP_BEXTRACTU((value),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_GETS(value)       (GAP_BEXTRACT((value),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT_SET(value,field)  (GAP_BINSERT((value),(field),7,9))
        #define UDMA_MRAM_TIMING_CFG_RW_TIME_CNT(val)              ((val) << 9)
        
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,16))
        #define UDMA_MRAM_TIMING_CFG_ADS_TIME_CNT(val)             ((val) << 16)
        
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_GET(value)       (GAP_BEXTRACTU((value),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_GETS(value)      (GAP_BEXTRACT((value),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),10,19))
        #define UDMA_MRAM_TIMING_CFG_PGS_TIME_CNT(val)             ((val) << 19)
        
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_GET(value)      (GAP_BEXTRACTU((value),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_GETS(value)     (GAP_BEXTRACT((value),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT_SET(value,field) (GAP_BINSERT((value),(field),3,29))
        #define UDMA_MRAM_TIMING_CFG_PROG_TIME_CNT(val)            ((val) << 29)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int strobe_time_cnt :3 ; // Strobe timing couner
            unsigned int go_sup_time_cnt :3 ; // Power Supply timing couner
            unsigned int men_time_cnt    :3 ; // MRAM EN timing couner
            unsigned int rw_time_cnt     :7 ; // Latency from write to read timing couner
            unsigned int ads_time_cnt    :3 ; // Address setup time couner
            unsigned int pgs_time_cnt    :10; // Program Setup time couner
            unsigned int prog_time_cnt   :3 ; // Program Min Pulse Width timing couner
          };
          unsigned int raw;
        } __attribute__((packed)) udma_mram_timing_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_mram_timing_cfg : public vp::reg_32
        {
        public:
            inline void strobe_time_cnt_set(uint32_t value);
            inline uint32_t strobe_time_cnt_get();
            inline void go_sup_time_cnt_set(uint32_t value);
            inline uint32_t go_sup_time_cnt_get();
            inline void men_time_cnt_set(uint32_t value);
            inline uint32_t men_time_cnt_get();
            inline void rw_time_cnt_set(uint32_t value);
            inline uint32_t rw_time_cnt_get();
            inline void ads_time_cnt_set(uint32_t value);
            inline uint32_t ads_time_cnt_get();
            inline void pgs_time_cnt_set(uint32_t value);
            inline uint32_t pgs_time_cnt_get();
            inline void prog_time_cnt_set(uint32_t value);
            inline uint32_t prog_time_cnt_get();
        };

|
