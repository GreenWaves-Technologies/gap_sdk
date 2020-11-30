Input file: doc/UDMA_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |                      Name                       |Offset|Width|                                                                                                                   Description                                                                                                                    |
    +=================================================+======+=====+==================================================================================================================================================================================================================================================+
    |:ref:`CFG_CG<udma_ctrl_CFG_CG>`                  |     0|   32|uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.|
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_CG_SET<udma_ctrl_CFG_CG_SET>`          |     4|   32|Each bit set to 1 sets the corresponding bit in the CFG_CG register                                                                                                                                                                               |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_CG_CLR<udma_ctrl_CFG_CG_CLR>`          |     8|   32|Each bit set to 1 clears the corresponding bit in the CFG_CG register                                                                                                                                                                             |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN<udma_ctrl_CFG_RSTN>`              |    12|   32|uDMA peripherals reset configuration. At RESET all periphs under reset.                                                                                                                                                                           |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN_SET<udma_ctrl_CFG_RSTN_SET>`      |    16|   32|Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register                                                                                                                                                                    |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN_CLR<udma_ctrl_CFG_RSTN_CLR>`      |    20|   32|Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register                                                                                                                                                                    |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_EVENT<udma_ctrl_CFG_EVENT>`            |    24|   32|uDMA peripherals external event configuration                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`FIFO_CFG<udma_ctrl_FIFO_CFG>`              |    28|   32|Sets the FIFO ID for push and pop                                                                                                                                                                                                                 |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`FIFO_PUSHPOP_8<udma_ctrl_FIFO_PUSHPOP_8>`  |    32|   32|Pushes(write) and pops(read) 8bit to the fifo                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`FIFO_PUSHPOP_16<udma_ctrl_FIFO_PUSHPOP_16>`|    36|   32|Pushes(write) and pops(read) 16bit to the fifo                                                                                                                                                                                                    |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`FIFO_PUSHPOP_24<udma_ctrl_FIFO_PUSHPOP_24>`|    40|   32|Pushes(write) and pops(read) 24bit to the fifo                                                                                                                                                                                                    |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`FIFO_PUSHPOP_32<udma_ctrl_FIFO_PUSHPOP_32>`|    44|   32|Pushes(write) and pops(read) 32bit to the fifo                                                                                                                                                                                                    |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`DATAMOVE_CFG<udma_ctrl_DATAMOVE_CFG>`      |    48|   32|Configure of data movement                                                                                                                                                                                                                        |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`DATAMOVE0_SIZE<udma_ctrl_DATAMOVE0_SIZE>`  |    52|   32|Write sets the number of bytes to be moved. Read returns the number of bytes remaining                                                                                                                                                            |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`DATAMOVE1_SIZE<udma_ctrl_DATAMOVE1_SIZE>`  |    56|   32|Write sets the number of bytes to be moved. Read returns the number of bytes remaining                                                                                                                                                            |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`STREAM_CFG<udma_ctrl_STREAM_CFG>`          |    60|   32|Configure blocking behaviour of streams 1'b0 sets the stream as non blocking 1'b1 sets the stream to be blocking(giving ready only when data available)                                                                                           |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE0<udma_ctrl_TIMEOUT_PRE0>`      |    64|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH0<udma_ctrl_TIMEOUT_CH0>`        |    68|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE1<udma_ctrl_TIMEOUT_PRE1>`      |    72|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH1<udma_ctrl_TIMEOUT_CH1>`        |    76|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE2<udma_ctrl_TIMEOUT_PRE2>`      |    80|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH2<udma_ctrl_TIMEOUT_CH2>`        |    84|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE3<udma_ctrl_TIMEOUT_PRE3>`      |    88|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH3<udma_ctrl_TIMEOUT_CH3>`        |    92|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE4<udma_ctrl_TIMEOUT_PRE4>`      |    96|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH4<udma_ctrl_TIMEOUT_CH4>`        |   100|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE5<udma_ctrl_TIMEOUT_PRE5>`      |   104|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH5<udma_ctrl_TIMEOUT_CH5>`        |   108|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE6<udma_ctrl_TIMEOUT_PRE6>`      |   112|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH6<udma_ctrl_TIMEOUT_CH6>`        |   116|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_PRE7<udma_ctrl_TIMEOUT_PRE7>`      |   120|   32|Configuration of the frequency prescaler for timeout                                                                                                                                                                                              |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`TIMEOUT_CH7<udma_ctrl_TIMEOUT_CH7>`        |   124|   32|Configuration for timeout ch0                                                                                                                                                                                                                     |
    +-------------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
                #define UDMA_CTRL_CFG_CG_OFFSET                  0x0
        
                // Each bit set to 1 sets the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_SET_OFFSET              0x4
        
                // Each bit set to 1 clears the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_CLR_OFFSET              0x8
        
                // uDMA peripherals reset configuration. At RESET all periphs under reset.
                #define UDMA_CTRL_CFG_RSTN_OFFSET                0xc
        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_SET_OFFSET            0x10
        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_CLR_OFFSET            0x14
        
                // uDMA peripherals external event configuration
                #define UDMA_CTRL_CFG_EVENT_OFFSET               0x18
        
                // Sets the FIFO ID for push and pop
                #define UDMA_CTRL_FIFO_CFG_OFFSET                0x1c
        
                // Pushes(write) and pops(read) 8bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_8_OFFSET          0x20
        
                // Pushes(write) and pops(read) 16bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_16_OFFSET         0x24
        
                // Pushes(write) and pops(read) 24bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_24_OFFSET         0x28
        
                // Pushes(write) and pops(read) 32bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_32_OFFSET         0x2c
        
                // Configure of data movement
                #define UDMA_CTRL_DATAMOVE_CFG_OFFSET            0x30
        
                // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
                #define UDMA_CTRL_DATAMOVE0_SIZE_OFFSET          0x34
        
                // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
                #define UDMA_CTRL_DATAMOVE1_SIZE_OFFSET          0x38
        
                // Configure blocking behaviour of streams 1'b0 sets the stream as non blocking 1'b1 sets the stream to be blocking(giving ready only when data available)
                #define UDMA_CTRL_STREAM_CFG_OFFSET              0x3c
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE0_OFFSET            0x40
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH0_OFFSET             0x44
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE1_OFFSET            0x48
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH1_OFFSET             0x4c
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE2_OFFSET            0x50
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH2_OFFSET             0x54
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE3_OFFSET            0x58
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH3_OFFSET             0x5c
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE4_OFFSET            0x60
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH4_OFFSET             0x64
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE5_OFFSET            0x68
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH5_OFFSET             0x6c
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE6_OFFSET            0x70
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH6_OFFSET             0x74
        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE7_OFFSET            0x78
        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH7_OFFSET             0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_cg_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_cg_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_clr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_clr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_event_get(uint32_t base);
        static inline void udma_ctrl_cfg_event_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_fifo_cfg_get(uint32_t base);
        static inline void udma_ctrl_fifo_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_fifo_pushpop_8_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_8_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_fifo_pushpop_16_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_16_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_fifo_pushpop_24_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_24_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_fifo_pushpop_32_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_32_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_datamove_cfg_get(uint32_t base);
        static inline void udma_ctrl_datamove_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_datamove0_size_get(uint32_t base);
        static inline void udma_ctrl_datamove0_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_datamove1_size_get(uint32_t base);
        static inline void udma_ctrl_datamove1_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_stream_cfg_get(uint32_t base);
        static inline void udma_ctrl_stream_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre0_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch0_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch0_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre1_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch1_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch1_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre2_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch2_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch2_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre3_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch3_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch3_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre4_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre4_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch4_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch4_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre5_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch5_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch5_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre6_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre6_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch6_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch6_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_pre7_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre7_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_timeout_ch7_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_BIT                             0
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_MASK                            0xff
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_BIT                             8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_MASK                            0xff00
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_BIT                             16
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_MASK                            0xff0000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_BIT                             24
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_MASK                            0xff000000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_RESET                           0x0
        
        // Sets the FIFO ID used for pushing (access: R/W)
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_BIT                               0
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_WIDTH                             8
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_MASK                              0xff
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_RESET                             0xff
        
        // Sets the FIFO ID used for popping (access: R/W)
        #define UDMA_CTRL_FIFO_CFG_POP_ID_BIT                                8
        #define UDMA_CTRL_FIFO_CFG_POP_ID_WIDTH                              8
        #define UDMA_CTRL_FIFO_CFG_POP_ID_MASK                               0xff00
        #define UDMA_CTRL_FIFO_CFG_POP_ID_RESET                              0xff
        
        // Write sets the number of bytes to be moved. Read returns the number of bytes remaining (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_BIT                              0
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_WIDTH                            21
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_MASK                             0x1fffff
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_RESET                            0x0
        
        // When written to 1 stops the data mover. When read return the enable status of the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_BIT                              30
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_WIDTH                            1
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_MASK                             0x40000000
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_RESET                            0x0
        
        // When written to 1 enables the data mover. When read return the enable status of the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_EN_BIT                                31
        #define UDMA_CTRL_DATAMOVE_CFG_EN_WIDTH                              1
        #define UDMA_CTRL_DATAMOVE_CFG_EN_MASK                               0x80000000
        #define UDMA_CTRL_DATAMOVE_CFG_EN_RESET                              0x0
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_BIT                     0
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_MASK                    0xff
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_BIT                       8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_MASK                      0xff00
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_RESET                     0xff
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_BIT                     16
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_MASK                    0xff0000
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_BIT                       24
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_MASK                      0xff000000
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_RESET                     0xff
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_BIT                     0
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_MASK                    0xff
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_BIT                       8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_MASK                      0xff00
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_RESET                     0xff
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_BIT                     16
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_MASK                    0xff0000
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_BIT                       24
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_MASK                      0xff000000
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_RESET                     0xff
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH0_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH0_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH0_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH1_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH1_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH1_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH2_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH2_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH2_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH3_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH3_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH3_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH4_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH4_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH4_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH5_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH5_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH5_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH6_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH6_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH6_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_RESET                              0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_RESET                             0x0
        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH7_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH7_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH7_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0(val)                  ((val) << 0)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1(val)                  ((val) << 8)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2(val)                  ((val) << 16)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3(val)                  ((val) << 24)
        
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID(val)                    ((val) << 0)
        
        #define UDMA_CTRL_FIFO_CFG_POP_ID_GET(value)               (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID_GETS(value)              (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID(val)                     ((val) << 8)
        
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE(val)                   ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP(val)                   ((val) << 30)
        
        #define UDMA_CTRL_DATAMOVE_CFG_EN_GET(value)               (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN_GETS(value)              (GAP_BEXTRACT((value),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN(val)                     ((val) << 31)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0(val)          ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0(val)            ((val) << 8)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_GET(value)    (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_GETS(value)   (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1(val)          ((val) << 16)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_GET(value)      (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_GETS(value)     (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1(val)            ((val) << 24)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0(val)          ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0(val)            ((val) << 8)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_GET(value)    (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_GETS(value)   (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1(val)          ((val) << 16)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_GET(value)      (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_GETS(value)     (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1(val)            ((val) << 24)
        
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH0_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH1_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH2_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH3_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH4_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH5_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH6_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR(val)                    ((val) << 17)
        
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH7_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_CTRL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cfg_cg;  // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
            volatile uint32_t cfg_cg_set;  // Each bit set to 1 sets the corresponding bit in the CFG_CG register
            volatile uint32_t cfg_cg_clr;  // Each bit set to 1 clears the corresponding bit in the CFG_CG register
            volatile uint32_t cfg_rstn;  // uDMA peripherals reset configuration. At RESET all periphs under reset.
            volatile uint32_t cfg_rstn_set;  // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
            volatile uint32_t cfg_rstn_clr;  // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
            volatile uint32_t cfg_event;  // uDMA peripherals external event configuration
            volatile uint32_t fifo_cfg;  // Sets the FIFO ID for push and pop
            volatile uint32_t fifo_pushpop_8;  // Pushes(write) and pops(read) 8bit to the fifo
            volatile uint32_t fifo_pushpop_16;  // Pushes(write) and pops(read) 16bit to the fifo
            volatile uint32_t fifo_pushpop_24;  // Pushes(write) and pops(read) 24bit to the fifo
            volatile uint32_t fifo_pushpop_32;  // Pushes(write) and pops(read) 32bit to the fifo
            volatile uint32_t datamove_cfg;  // Configure of data movement
            volatile uint32_t datamove0_size;  // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
            volatile uint32_t datamove1_size;  // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
            volatile uint32_t stream_cfg;  // Configure blocking behaviour of streams 1'b0 sets the stream as non blocking 1'b1 sets the stream to be blocking(giving ready only when data available)
            volatile uint32_t timeout_pre0;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch0;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre1;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch1;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre2;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch2;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre3;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch3;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre4;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch4;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre5;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch5;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre6;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch6;  // Configuration for timeout ch0
            volatile uint32_t timeout_pre7;  // Configuration of the frequency prescaler for timeout
            volatile uint32_t timeout_ch7;  // Configuration for timeout ch0
        } __attribute__((packed)) udma_ctrl_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_clr_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_clr_t;
        
        typedef union {
          struct {
            unsigned int cmp_evt0        :8 ; // Compare event
            unsigned int cmp_evt1        :8 ; // Compare event
            unsigned int cmp_evt2        :8 ; // Compare event
            unsigned int cmp_evt3        :8 ; // Compare event
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_event_t;
        
        typedef union {
          struct {
            unsigned int push_id         :8 ; // Sets the FIFO ID used for pushing
            unsigned int pop_id          :8 ; // Sets the FIFO ID used for popping
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_cfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_8_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_16_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_24_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_32_t;
        
        typedef union {
          struct {
            unsigned int size            :21; // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
            unsigned int padding0:9 ;
            unsigned int stop            :1 ; // When written to 1 stops the data mover. When read return the enable status of the data mover
            unsigned int en              :1 ; // When written to 1 enables the data mover. When read return the enable status of the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove_cfg_t;
        
        typedef union {
          struct {
            unsigned int source_id_0     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_0       :8 ; // Sets the destination ID used by the data mover
            unsigned int source_id_1     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_1       :8 ; // Sets the destination ID used by the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove0_size_t;
        
        typedef union {
          struct {
            unsigned int source_id_0     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_0       :8 ; // Sets the destination ID used by the data mover
            unsigned int source_id_1     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_1       :8 ; // Sets the destination ID used by the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove1_size_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_stream_cfg_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre0_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch0_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre1_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch1_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre2_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch2_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre3_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch3_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre4_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch4_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre5_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch5_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre6_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch6_t;
        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre7_t;
        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_ctrl : public vp::regmap
        {
        public:
            vp_udma_ctrl_cfg_cg cfg_cg;
            vp_udma_ctrl_cfg_cg_set cfg_cg_set;
            vp_udma_ctrl_cfg_cg_clr cfg_cg_clr;
            vp_udma_ctrl_cfg_rstn cfg_rstn;
            vp_udma_ctrl_cfg_rstn_set cfg_rstn_set;
            vp_udma_ctrl_cfg_rstn_clr cfg_rstn_clr;
            vp_udma_ctrl_cfg_event cfg_event;
            vp_udma_ctrl_fifo_cfg fifo_cfg;
            vp_udma_ctrl_fifo_pushpop_8 fifo_pushpop_8;
            vp_udma_ctrl_fifo_pushpop_16 fifo_pushpop_16;
            vp_udma_ctrl_fifo_pushpop_24 fifo_pushpop_24;
            vp_udma_ctrl_fifo_pushpop_32 fifo_pushpop_32;
            vp_udma_ctrl_datamove_cfg datamove_cfg;
            vp_udma_ctrl_datamove0_size datamove0_size;
            vp_udma_ctrl_datamove1_size datamove1_size;
            vp_udma_ctrl_stream_cfg stream_cfg;
            vp_udma_ctrl_timeout_pre0 timeout_pre0;
            vp_udma_ctrl_timeout_ch0 timeout_ch0;
            vp_udma_ctrl_timeout_pre1 timeout_pre1;
            vp_udma_ctrl_timeout_ch1 timeout_ch1;
            vp_udma_ctrl_timeout_pre2 timeout_pre2;
            vp_udma_ctrl_timeout_ch2 timeout_ch2;
            vp_udma_ctrl_timeout_pre3 timeout_pre3;
            vp_udma_ctrl_timeout_ch3 timeout_ch3;
            vp_udma_ctrl_timeout_pre4 timeout_pre4;
            vp_udma_ctrl_timeout_ch4 timeout_ch4;
            vp_udma_ctrl_timeout_pre5 timeout_pre5;
            vp_udma_ctrl_timeout_ch5 timeout_ch5;
            vp_udma_ctrl_timeout_pre6 timeout_pre6;
            vp_udma_ctrl_timeout_ch6 timeout_ch6;
            vp_udma_ctrl_timeout_pre7 timeout_pre7;
            vp_udma_ctrl_timeout_ch7 timeout_ch7;
        };

|

.. _udma_ctrl_CFG_CG:

CFG_CG
""""""

uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.

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

        
                // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
                #define UDMA_CTRL_CFG_CG_OFFSET                  0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_CG_SET:

CFG_CG_SET
""""""""""

Each bit set to 1 sets the corresponding bit in the CFG_CG register

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

        
                // Each bit set to 1 sets the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_SET_OFFSET              0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg_set : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_CG_CLR:

CFG_CG_CLR
""""""""""

Each bit set to 1 clears the corresponding bit in the CFG_CG register

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

        
                // Each bit set to 1 clears the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_CLR_OFFSET              0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_clr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_clr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg_clr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN:

CFG_RSTN
""""""""

uDMA peripherals reset configuration. At RESET all periphs under reset.

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

        
                // uDMA peripherals reset configuration. At RESET all periphs under reset.
                #define UDMA_CTRL_CFG_RSTN_OFFSET                0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN_SET:

CFG_RSTN_SET
""""""""""""

Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register

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

        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_SET_OFFSET            0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn_set : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN_CLR:

CFG_RSTN_CLR
""""""""""""

Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register

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

        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_CLR_OFFSET            0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_clr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_clr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn_clr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_EVENT:

CFG_EVENT
"""""""""

uDMA peripherals external event configuration

.. table:: 

    +-----+---+--------+-------------+
    |Bit #|R/W|  Name  | Description |
    +=====+===+========+=============+
    |7:0  |R/W|CMP_EVT0|Compare event|
    +-----+---+--------+-------------+
    |15:8 |R/W|CMP_EVT1|Compare event|
    +-----+---+--------+-------------+
    |23:16|R/W|CMP_EVT2|Compare event|
    +-----+---+--------+-------------+
    |31:24|R/W|CMP_EVT3|Compare event|
    +-----+---+--------+-------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // uDMA peripherals external event configuration
                #define UDMA_CTRL_CFG_EVENT_OFFSET               0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_event_get(uint32_t base);
        static inline void udma_ctrl_cfg_event_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_BIT                             0
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_MASK                            0xff
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_BIT                             8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_MASK                            0xff00
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_BIT                             16
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_MASK                            0xff0000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_RESET                           0x0
        
        // Compare event (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_BIT                             24
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_MASK                            0xff000000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0(val)                  ((val) << 0)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1(val)                  ((val) << 8)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2(val)                  ((val) << 16)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmp_evt0        :8 ; // Compare event
            unsigned int cmp_evt1        :8 ; // Compare event
            unsigned int cmp_evt2        :8 ; // Compare event
            unsigned int cmp_evt3        :8 ; // Compare event
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_event_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_event : public vp::reg_32
        {
        public:
            inline void cmp_evt0_set(uint32_t value);
            inline uint32_t cmp_evt0_get();
            inline void cmp_evt1_set(uint32_t value);
            inline uint32_t cmp_evt1_get();
            inline void cmp_evt2_set(uint32_t value);
            inline uint32_t cmp_evt2_get();
            inline void cmp_evt3_set(uint32_t value);
            inline uint32_t cmp_evt3_get();
        };

|

.. _udma_ctrl_FIFO_CFG:

FIFO_CFG
""""""""

Sets the FIFO ID for push and pop

.. table:: 

    +-----+---+-------+---------------------------------+
    |Bit #|R/W| Name  |           Description           |
    +=====+===+=======+=================================+
    |7:0  |R/W|PUSH_ID|Sets the FIFO ID used for pushing|
    +-----+---+-------+---------------------------------+
    |15:8 |R/W|POP_ID |Sets the FIFO ID used for popping|
    +-----+---+-------+---------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sets the FIFO ID for push and pop
                #define UDMA_CTRL_FIFO_CFG_OFFSET                0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_fifo_cfg_get(uint32_t base);
        static inline void udma_ctrl_fifo_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the FIFO ID used for pushing (access: R/W)
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_BIT                               0
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_WIDTH                             8
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_MASK                              0xff
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_RESET                             0xff
        
        // Sets the FIFO ID used for popping (access: R/W)
        #define UDMA_CTRL_FIFO_CFG_POP_ID_BIT                                8
        #define UDMA_CTRL_FIFO_CFG_POP_ID_WIDTH                              8
        #define UDMA_CTRL_FIFO_CFG_POP_ID_MASK                               0xff00
        #define UDMA_CTRL_FIFO_CFG_POP_ID_RESET                              0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_GET(value)              (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_GETS(value)             (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID_SET(value,field)        (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_FIFO_CFG_PUSH_ID(val)                    ((val) << 0)
        
        #define UDMA_CTRL_FIFO_CFG_POP_ID_GET(value)               (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID_GETS(value)              (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID_SET(value,field)         (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_FIFO_CFG_POP_ID(val)                     ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int push_id         :8 ; // Sets the FIFO ID used for pushing
            unsigned int pop_id          :8 ; // Sets the FIFO ID used for popping
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_fifo_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_fifo_cfg : public vp::reg_32
        {
        public:
            inline void push_id_set(uint32_t value);
            inline uint32_t push_id_get();
            inline void pop_id_set(uint32_t value);
            inline uint32_t pop_id_get();
        };

|

.. _udma_ctrl_FIFO_PUSHPOP_8:

FIFO_PUSHPOP_8
""""""""""""""

Pushes(write) and pops(read) 8bit to the fifo

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

        
                // Pushes(write) and pops(read) 8bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_8_OFFSET          0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_fifo_pushpop_8_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_8_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_fifo_pushpop_8 : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_FIFO_PUSHPOP_16:

FIFO_PUSHPOP_16
"""""""""""""""

Pushes(write) and pops(read) 16bit to the fifo

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

        
                // Pushes(write) and pops(read) 16bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_16_OFFSET         0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_fifo_pushpop_16_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_16_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_16_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_fifo_pushpop_16 : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_FIFO_PUSHPOP_24:

FIFO_PUSHPOP_24
"""""""""""""""

Pushes(write) and pops(read) 24bit to the fifo

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

        
                // Pushes(write) and pops(read) 24bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_24_OFFSET         0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_fifo_pushpop_24_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_24_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_24_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_fifo_pushpop_24 : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_FIFO_PUSHPOP_32:

FIFO_PUSHPOP_32
"""""""""""""""

Pushes(write) and pops(read) 32bit to the fifo

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

        
                // Pushes(write) and pops(read) 32bit to the fifo
                #define UDMA_CTRL_FIFO_PUSHPOP_32_OFFSET         0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_fifo_pushpop_32_get(uint32_t base);
        static inline void udma_ctrl_fifo_pushpop_32_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_fifo_pushpop_32_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_fifo_pushpop_32 : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_DATAMOVE_CFG:

DATAMOVE_CFG
""""""""""""

Configure of data movement

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                         Description                                          |
    +=====+===+====+==============================================================================================+
    |20:0 |R/W|SIZE|Write sets the number of bytes to be moved. Read returns the number of bytes remaining        |
    +-----+---+----+----------------------------------------------------------------------------------------------+
    |30   |R/W|STOP|When written to 1 stops the data mover. When read return the enable status of the data mover  |
    +-----+---+----+----------------------------------------------------------------------------------------------+
    |31   |R/W|EN  |When written to 1 enables the data mover. When read return the enable status of the data mover|
    +-----+---+----+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configure of data movement
                #define UDMA_CTRL_DATAMOVE_CFG_OFFSET            0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_datamove_cfg_get(uint32_t base);
        static inline void udma_ctrl_datamove_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Write sets the number of bytes to be moved. Read returns the number of bytes remaining (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_BIT                              0
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_WIDTH                            21
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_MASK                             0x1fffff
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_RESET                            0x0
        
        // When written to 1 stops the data mover. When read return the enable status of the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_BIT                              30
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_WIDTH                            1
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_MASK                             0x40000000
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_RESET                            0x0
        
        // When written to 1 enables the data mover. When read return the enable status of the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE_CFG_EN_BIT                                31
        #define UDMA_CTRL_DATAMOVE_CFG_EN_WIDTH                              1
        #define UDMA_CTRL_DATAMOVE_CFG_EN_MASK                               0x80000000
        #define UDMA_CTRL_DATAMOVE_CFG_EN_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define UDMA_CTRL_DATAMOVE_CFG_SIZE(val)                   ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_GET(value)             (GAP_BEXTRACTU((value),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_GETS(value)            (GAP_BEXTRACT((value),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP_SET(value,field)       (GAP_BINSERT((value),(field),1,30))
        #define UDMA_CTRL_DATAMOVE_CFG_STOP(val)                   ((val) << 30)
        
        #define UDMA_CTRL_DATAMOVE_CFG_EN_GET(value)               (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN_GETS(value)              (GAP_BEXTRACT((value),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CTRL_DATAMOVE_CFG_EN(val)                     ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :21; // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
            unsigned int padding0:9 ;
            unsigned int stop            :1 ; // When written to 1 stops the data mover. When read return the enable status of the data mover
            unsigned int en              :1 ; // When written to 1 enables the data mover. When read return the enable status of the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_datamove_cfg : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _udma_ctrl_DATAMOVE0_SIZE:

DATAMOVE0_SIZE
""""""""""""""

Write sets the number of bytes to be moved. Read returns the number of bytes remaining

.. table:: 

    +-----+---+-----------+----------------------------------------------+
    |Bit #|R/W|   Name    |                 Description                  |
    +=====+===+===========+==============================================+
    |7:0  |R/W|SOURCE_ID_0|Sets the source ID used by the data mover     |
    +-----+---+-----------+----------------------------------------------+
    |15:8 |R/W|DEST_ID_0  |Sets the destination ID used by the data mover|
    +-----+---+-----------+----------------------------------------------+
    |23:16|R/W|SOURCE_ID_1|Sets the source ID used by the data mover     |
    +-----+---+-----------+----------------------------------------------+
    |31:24|R/W|DEST_ID_1  |Sets the destination ID used by the data mover|
    +-----+---+-----------+----------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
                #define UDMA_CTRL_DATAMOVE0_SIZE_OFFSET          0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_datamove0_size_get(uint32_t base);
        static inline void udma_ctrl_datamove0_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_BIT                     0
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_MASK                    0xff
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_BIT                       8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_MASK                      0xff00
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_RESET                     0xff
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_BIT                     16
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_MASK                    0xff0000
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_BIT                       24
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_MASK                      0xff000000
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_RESET                     0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_0(val)          ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_0(val)            ((val) << 8)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_GET(value)    (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_GETS(value)   (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_DATAMOVE0_SIZE_SOURCE_ID_1(val)          ((val) << 16)
        
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_GET(value)      (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_GETS(value)     (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_DATAMOVE0_SIZE_DEST_ID_1(val)            ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id_0     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_0       :8 ; // Sets the destination ID used by the data mover
            unsigned int source_id_1     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_1       :8 ; // Sets the destination ID used by the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove0_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_datamove0_size : public vp::reg_32
        {
        public:
            inline void source_id_0_set(uint32_t value);
            inline uint32_t source_id_0_get();
            inline void dest_id_0_set(uint32_t value);
            inline uint32_t dest_id_0_get();
            inline void source_id_1_set(uint32_t value);
            inline uint32_t source_id_1_get();
            inline void dest_id_1_set(uint32_t value);
            inline uint32_t dest_id_1_get();
        };

|

.. _udma_ctrl_DATAMOVE1_SIZE:

DATAMOVE1_SIZE
""""""""""""""

Write sets the number of bytes to be moved. Read returns the number of bytes remaining

.. table:: 

    +-----+---+-----------+----------------------------------------------+
    |Bit #|R/W|   Name    |                 Description                  |
    +=====+===+===========+==============================================+
    |7:0  |R/W|SOURCE_ID_0|Sets the source ID used by the data mover     |
    +-----+---+-----------+----------------------------------------------+
    |15:8 |R/W|DEST_ID_0  |Sets the destination ID used by the data mover|
    +-----+---+-----------+----------------------------------------------+
    |23:16|R/W|SOURCE_ID_1|Sets the source ID used by the data mover     |
    +-----+---+-----------+----------------------------------------------+
    |31:24|R/W|DEST_ID_1  |Sets the destination ID used by the data mover|
    +-----+---+-----------+----------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Write sets the number of bytes to be moved. Read returns the number of bytes remaining
                #define UDMA_CTRL_DATAMOVE1_SIZE_OFFSET          0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_datamove1_size_get(uint32_t base);
        static inline void udma_ctrl_datamove1_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_BIT                     0
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_MASK                    0xff
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_BIT                       8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_MASK                      0xff00
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_RESET                     0xff
        
        // Sets the source ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_BIT                     16
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_WIDTH                   8
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_MASK                    0xff0000
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_RESET                   0xff
        
        // Sets the destination ID used by the data mover (access: R/W)
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_BIT                       24
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_WIDTH                     8
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_MASK                      0xff000000
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_RESET                     0xff

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_0(val)          ((val) << 0)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_GET(value)      (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_GETS(value)     (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_0(val)            ((val) << 8)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_GET(value)    (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_GETS(value)   (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_DATAMOVE1_SIZE_SOURCE_ID_1(val)          ((val) << 16)
        
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_GET(value)      (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_GETS(value)     (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_DATAMOVE1_SIZE_DEST_ID_1(val)            ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id_0     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_0       :8 ; // Sets the destination ID used by the data mover
            unsigned int source_id_1     :8 ; // Sets the source ID used by the data mover
            unsigned int dest_id_1       :8 ; // Sets the destination ID used by the data mover
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_datamove1_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_datamove1_size : public vp::reg_32
        {
        public:
            inline void source_id_0_set(uint32_t value);
            inline uint32_t source_id_0_get();
            inline void dest_id_0_set(uint32_t value);
            inline uint32_t dest_id_0_get();
            inline void source_id_1_set(uint32_t value);
            inline uint32_t source_id_1_get();
            inline void dest_id_1_set(uint32_t value);
            inline uint32_t dest_id_1_get();
        };

|

.. _udma_ctrl_STREAM_CFG:

STREAM_CFG
""""""""""

Configure blocking behaviour of streams 1'b0 sets the stream as non blocking 1'b1 sets the stream to be blocking(giving ready only when data available)

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

        
                // Configure blocking behaviour of streams 1'b0 sets the stream as non blocking 1'b1 sets the stream to be blocking(giving ready only when data available)
                #define UDMA_CTRL_STREAM_CFG_OFFSET              0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_stream_cfg_get(uint32_t base);
        static inline void udma_ctrl_stream_cfg_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_stream_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_stream_cfg : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_TIMEOUT_PRE0:

TIMEOUT_PRE0
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE0_OFFSET            0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre0_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE0_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE0_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE0_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre0 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH0:

TIMEOUT_CH0
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH0_OFFSET             0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch0_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH0_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH0_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH0_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH0_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH0_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH0_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH0_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH0_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch0 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE1:

TIMEOUT_PRE1
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE1_OFFSET            0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre1_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE1_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE1_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE1_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre1 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH1:

TIMEOUT_CH1
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH1_OFFSET             0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch1_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH1_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH1_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH1_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH1_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH1_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH1_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH1_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH1_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch1 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE2:

TIMEOUT_PRE2
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE2_OFFSET            0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre2_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE2_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE2_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE2_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre2 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH2:

TIMEOUT_CH2
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH2_OFFSET             0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch2_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH2_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH2_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH2_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH2_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH2_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH2_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH2_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH2_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch2 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE3:

TIMEOUT_PRE3
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE3_OFFSET            0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre3_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE3_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE3_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE3_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre3 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH3:

TIMEOUT_CH3
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH3_OFFSET             0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch3_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH3_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH3_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH3_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH3_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH3_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH3_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH3_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH3_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch3 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE4:

TIMEOUT_PRE4
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE4_OFFSET            0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre4_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE4_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE4_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE4_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre4 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH4:

TIMEOUT_CH4
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH4_OFFSET             0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch4_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH4_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH4_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH4_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH4_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH4_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH4_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH4_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH4_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch4 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE5:

TIMEOUT_PRE5
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE5_OFFSET            0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre5_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE5_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE5_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE5_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre5 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH5:

TIMEOUT_CH5
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH5_OFFSET             0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch5_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH5_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH5_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH5_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH5_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH5_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH5_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH5_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH5_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch5 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE6:

TIMEOUT_PRE6
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE6_OFFSET            0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre6_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE6_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE6_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE6_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre6 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH6:

TIMEOUT_CH6
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH6_OFFSET             0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch6_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH6_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH6_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH6_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH6_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH6_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH6_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH6_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH6_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch6 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _udma_ctrl_TIMEOUT_PRE7:

TIMEOUT_PRE7
""""""""""""

Configuration of the frequency prescaler for timeout

.. table:: 

    +-----+---+----+------------------------------------------------------------------+
    |Bit #|R/W|Name|                           Description                            |
    +=====+===+====+==================================================================+
    |15:0 |R/W|CNT |Sets the target for the timeout counter                           |
    +-----+---+----+------------------------------------------------------------------+
    |16   |R/W|EN  |Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable|
    +-----+---+----+------------------------------------------------------------------+
    |17   |W  |CLR |Resets the timeout prescaler to 0                                 |
    +-----+---+----+------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of the frequency prescaler for timeout
                #define UDMA_CTRL_TIMEOUT_PRE7_OFFSET            0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_pre7_get(uint32_t base);
        static inline void udma_ctrl_timeout_pre7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_BIT                               0
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_WIDTH                             16
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_MASK                              0xffff
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_RESET                             0x0
        
        // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_BIT                                16
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_WIDTH                              1
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_MASK                               0x10000
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_RESET                              0x0
        
        // Resets the timeout prescaler to 0 (access: W)
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_BIT                               17
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_WIDTH                             1
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_MASK                              0x20000
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CTRL_TIMEOUT_PRE7_CNT(val)                    ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define UDMA_CTRL_TIMEOUT_PRE7_EN(val)                     ((val) << 16)
        
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_GET(value)              (GAP_BEXTRACTU((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_GETS(value)             (GAP_BEXTRACT((value),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR_SET(value,field)        (GAP_BINSERT((value),(field),1,17))
        #define UDMA_CTRL_TIMEOUT_PRE7_CLR(val)                    ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :16; // Sets the target for the timeout counter
            unsigned int en              :1 ; // Enables/disable the timeout prescaler 1'b0: disable - 1'b1: enable
            unsigned int clr             :1 ; // Resets the timeout prescaler to 0
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_pre7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_pre7 : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void clr_set(uint32_t value);
            inline uint32_t clr_get();
        };

|

.. _udma_ctrl_TIMEOUT_CH7:

TIMEOUT_CH7
"""""""""""

Configuration for timeout ch0

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                Description                                                                                 |
    +=====+===+=========+============================================================================================================================================================================+
    |7:0  |R/W|SOURCE_ID|Sets the uDMA source ID used by the timeout                                                                                                                                 |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|MODE     |Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx|
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10   |R/W|EN       |Enables/disable the timeout 1'b0: disable - 1'b1: enable                                                                                                                    |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|CNT      |Sets the target for the timeout counter                                                                                                                                     |
    +-----+---+---------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration for timeout ch0
                #define UDMA_CTRL_TIMEOUT_CH7_OFFSET             0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_timeout_ch7_get(uint32_t base);
        static inline void udma_ctrl_timeout_ch7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Sets the uDMA source ID used by the timeout (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_BIT                          0
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_WIDTH                        8
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_MASK                         0xff
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_RESET                        0x0
        
        // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_BIT                               8
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_WIDTH                             2
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_MASK                              0x300
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_RESET                             0x0
        
        // Enables/disable the timeout 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_EN_BIT                                 10
        #define UDMA_CTRL_TIMEOUT_CH7_EN_WIDTH                               1
        #define UDMA_CTRL_TIMEOUT_CH7_EN_MASK                                0x400
        #define UDMA_CTRL_TIMEOUT_CH7_EN_RESET                               0x0
        
        // Sets the target for the timeout counter (access: R/W)
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_BIT                                16
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_WIDTH                              16
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_MASK                               0xffff0000
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_TIMEOUT_CH7_SOURCE_ID(val)               ((val) << 0)
        
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_GET(value)              (GAP_BEXTRACTU((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_GETS(value)             (GAP_BEXTRACT((value),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE_SET(value,field)        (GAP_BINSERT((value),(field),2,8))
        #define UDMA_CTRL_TIMEOUT_CH7_MODE(val)                    ((val) << 8)
        
        #define UDMA_CTRL_TIMEOUT_CH7_EN_GET(value)                (GAP_BEXTRACTU((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN_GETS(value)               (GAP_BEXTRACT((value),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,10))
        #define UDMA_CTRL_TIMEOUT_CH7_EN(val)                      ((val) << 10)
        
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_GET(value)               (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_GETS(value)              (GAP_BEXTRACT((value),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT_SET(value,field)         (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CTRL_TIMEOUT_CH7_CNT(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int source_id       :8 ; // Sets the uDMA source ID used by the timeout
            unsigned int mode            :2 ; // Sets the mode to start/stop the timeout 2'b00: sw triggered - 2'b01: started by start of transfer and stopped by end of transfer - 2'b10: counter cleared at each data rx/tx
            unsigned int en              :1 ; // Enables/disable the timeout 1'b0: disable - 1'b1: enable
            unsigned int padding0:5 ;
            unsigned int cnt             :16; // Sets the target for the timeout counter
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_timeout_ch7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_timeout_ch7 : public vp::reg_32
        {
        public:
            inline void source_id_set(uint32_t value);
            inline uint32_t source_id_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|
