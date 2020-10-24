Input file: fe/ips/udma/udma_i2c/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |                                Name                                |Offset|Width|          Description           |
    +====================================================================+======+=====+================================+
    |:ref:`FOLL_UDMA_RX_DEST_REG_IDX<udma_i2c_FOLL_UDMA_RX_DEST_REG_IDX>`|     0|    8|i2c follower rx udma channel    |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`FOLL_UDMA_TX_DEST_REG_IDX<udma_i2c_FOLL_UDMA_TX_DEST_REG_IDX>`|     4|    8|i2c follower tx udma channel    |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`UDMA_CMD_DEST_REG_IDX<udma_i2c_UDMA_CMD_DEST_REG_IDX>`        |     8|    8|command tx udma channel         |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`LEAD_UDMA_RX_DEST_REG_IDX<udma_i2c_LEAD_UDMA_RX_DEST_REG_IDX>`|    12|    8|i2c leader rx udma channel      |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`LEAD_UDMA_TX_DEST_REG_IDX<udma_i2c_LEAD_UDMA_TX_DEST_REG_IDX>`|    16|    8|i2c leader tx udma channel      |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`STATUS_REG_IDX<udma_i2c_STATUS_REG_IDX>`                      |    20|   26|incoming event flag / emit event|
    +--------------------------------------------------------------------+------+-----+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // i2c follower rx udma channel
                #define UDMA_I2C_FOLL_UDMA_RX_DEST_REG_IDX_OFFSET 0x0
        
                // i2c follower tx udma channel
                #define UDMA_I2C_FOLL_UDMA_TX_DEST_REG_IDX_OFFSET 0x4
        
                // command tx udma channel
                #define UDMA_I2C_UDMA_CMD_DEST_REG_IDX_OFFSET    0x8
        
                // i2c leader rx udma channel
                #define UDMA_I2C_LEAD_UDMA_RX_DEST_REG_IDX_OFFSET 0xc
        
                // i2c leader tx udma channel
                #define UDMA_I2C_LEAD_UDMA_TX_DEST_REG_IDX_OFFSET 0x10
        
                // incoming event flag / emit event
                #define UDMA_I2C_STATUS_REG_IDX_OFFSET           0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_foll_udma_rx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_foll_udma_rx_dest_reg_idx_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2c_foll_udma_tx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_foll_udma_tx_dest_reg_idx_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2c_udma_cmd_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_udma_cmd_dest_reg_idx_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2c_lead_udma_rx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_lead_udma_rx_dest_reg_idx_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2c_lead_udma_tx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_lead_udma_tx_dest_reg_idx_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_i2c_status_reg_idx_get(uint32_t base);
        static inline void udma_i2c_status_reg_idx_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // RW1C a leader read addressed this follower. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_BIT  0
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_MASK 0x1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader write addressed this follower. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_BIT  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_MASK 0x2
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader read addressed this follower and closed transfer. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_BIT  2
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_MASK 0x4
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader write addressed this follower and closed transfer. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_BIT  3
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_MASK 0x8
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_RESET 0x0
        
        // RW1C follower is addressed and received an arbitration loss. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_BIT 4
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_MASK 0x10
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_RESET 0x0
        
        // RW1C follower is addressed and noticed incorrect framing. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_BIT 5
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_MASK 0x20
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_RESET 0x0
        
        // W1 unlock follower tx rx channels after a sof/eof/error event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_BIT   14
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_MASK  0x4000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_RESET 0x0
        
        // W1 purge tx rx channels fifos or any remaining data (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_BIT    15
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_WIDTH  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_MASK   0x8000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_RESET  0x0
        
        // RW1C leader encountered an unexpected nack. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_BIT 16
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_MASK 0x10000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader encountered an arbitration loss. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_BIT 17
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_MASK 0x20000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader encountered an incorrect framing. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_BIT 18
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_MASK 0x40000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader emitted a command event. does not lock. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_BIT  19
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_MASK 0x80000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_RESET 0x0
        
        // W1 unlock leader tx rx channels after a sof/eof/error event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_BIT   22
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_MASK  0x400000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_RESET 0x0
        
        // W1 purge cmd tx rx leader fifos or any remaining data (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_BIT    23
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_WIDTH  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_MASK   0x800000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_RESET  0x0
        
        // W1 soft reset i2c, if it is deadlocked due to bus error (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_BIT 24
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_MASK 0x1000000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_RESET 0x0
        
        // W1 by default, prescaler divides by 100. Divide by 10 using this event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_BIT 25
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_MASK 0x2000000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_RESET 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX(val) ((val) << 0)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX(val) ((val) << 1)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX(val) ((val) << 2)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX(val) ((val) << 3)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX(val) ((val) << 4)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX(val) ((val) << 5)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX(val) ((val) << 14)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX(val) ((val) << 15)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX(val) ((val) << 16)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX(val) ((val) << 17)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX(val) ((val) << 18)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX(val) ((val) << 19)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX(val) ((val) << 22)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX(val) ((val) << 23)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX(val) ((val) << 24)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX(val) ((val) << 25)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_I2C_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t foll_udma_rx_dest_reg_idx;  // i2c follower rx udma channel
            volatile uint32_t foll_udma_tx_dest_reg_idx;  // i2c follower tx udma channel
            volatile uint32_t udma_cmd_dest_reg_idx;  // command tx udma channel
            volatile uint32_t lead_udma_rx_dest_reg_idx;  // i2c leader rx udma channel
            volatile uint32_t lead_udma_tx_dest_reg_idx;  // i2c leader tx udma channel
            volatile uint32_t status_reg_idx;  // incoming event flag / emit event
        } __attribute__((packed)) udma_i2c_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_foll_udma_rx_dest_reg_idx_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_foll_udma_tx_dest_reg_idx_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_udma_cmd_dest_reg_idx_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_lead_udma_rx_dest_reg_idx_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_lead_udma_tx_dest_reg_idx_t;
        
        typedef union {
          struct {
            unsigned int status_foll_sof_rcv_event_i_idx:1 ; // RW1C a leader read addressed this follower.
            unsigned int status_foll_sof_snd_event_i_idx:1 ; // RW1C a leader write addressed this follower.
            unsigned int status_foll_eof_rcv_event_i_idx:1 ; // RW1C a leader read addressed this follower and closed transfer.
            unsigned int status_foll_eof_snd_event_i_idx:1 ; // RW1C a leader write addressed this follower and closed transfer.
            unsigned int status_foll_error_arlo_event_i_idx:1 ; // RW1C follower is addressed and received an arbitration loss.
            unsigned int status_foll_error_framing_event_i_idx:1 ; // RW1C follower is addressed and noticed incorrect framing.
            unsigned int padding0:8 ;
            unsigned int status_foll_unlock_event_o_idx:1 ; // W1 unlock follower tx rx channels after a sof/eof/error event
            unsigned int status_foll_purge_event_o_idx:1 ; // W1 purge tx rx channels fifos or any remaining data
            unsigned int status_lead_error_nack_event_i_idx:1 ; // RW1C leader encountered an unexpected nack.
            unsigned int status_lead_error_arlo_event_i_idx:1 ; // RW1C leader encountered an arbitration loss.
            unsigned int status_lead_error_framing_event_i_idx:1 ; // RW1C leader encountered an incorrect framing.
            unsigned int status_lead_command_event_i_idx:1 ; // RW1C leader emitted a command event. does not lock.
            unsigned int padding1:2 ;
            unsigned int status_lead_unlock_event_o_idx:1 ; // W1 unlock leader tx rx channels after a sof/eof/error event
            unsigned int status_lead_purge_event_o_idx:1 ; // W1 purge cmd tx rx leader fifos or any remaining data
            unsigned int status_i2c_soft_reset_event_o_idx:1 ; // W1 soft reset i2c, if it is deadlocked due to bus error
            unsigned int status_i2c_prescaler_set_div10_event_o_idx:1 ; // W1 by default, prescaler divides by 100. Divide by 10 using this event
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_status_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_i2c : public vp::regmap
        {
        public:
            vp_udma_i2c_foll_udma_rx_dest_reg_idx foll_udma_rx_dest_reg_idx;
            vp_udma_i2c_foll_udma_tx_dest_reg_idx foll_udma_tx_dest_reg_idx;
            vp_udma_i2c_udma_cmd_dest_reg_idx udma_cmd_dest_reg_idx;
            vp_udma_i2c_lead_udma_rx_dest_reg_idx lead_udma_rx_dest_reg_idx;
            vp_udma_i2c_lead_udma_tx_dest_reg_idx lead_udma_tx_dest_reg_idx;
            vp_udma_i2c_status_reg_idx status_reg_idx;
        };

|

.. _udma_i2c_FOLL_UDMA_RX_DEST_REG_IDX:

FOLL_UDMA_RX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c follower rx udma channel

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

        
                // i2c follower rx udma channel
                #define UDMA_I2C_FOLL_UDMA_RX_DEST_REG_IDX_OFFSET 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_foll_udma_rx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_foll_udma_rx_dest_reg_idx_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_i2c_foll_udma_rx_dest_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_foll_udma_rx_dest_reg_idx : public vp::reg_8
        {
        public:
        };

|

.. _udma_i2c_FOLL_UDMA_TX_DEST_REG_IDX:

FOLL_UDMA_TX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c follower tx udma channel

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

        
                // i2c follower tx udma channel
                #define UDMA_I2C_FOLL_UDMA_TX_DEST_REG_IDX_OFFSET 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_foll_udma_tx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_foll_udma_tx_dest_reg_idx_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_i2c_foll_udma_tx_dest_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_foll_udma_tx_dest_reg_idx : public vp::reg_8
        {
        public:
        };

|

.. _udma_i2c_UDMA_CMD_DEST_REG_IDX:

UDMA_CMD_DEST_REG_IDX
"""""""""""""""""""""

command tx udma channel

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

        
                // command tx udma channel
                #define UDMA_I2C_UDMA_CMD_DEST_REG_IDX_OFFSET    0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_udma_cmd_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_udma_cmd_dest_reg_idx_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_i2c_udma_cmd_dest_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_udma_cmd_dest_reg_idx : public vp::reg_8
        {
        public:
        };

|

.. _udma_i2c_LEAD_UDMA_RX_DEST_REG_IDX:

LEAD_UDMA_RX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c leader rx udma channel

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

        
                // i2c leader rx udma channel
                #define UDMA_I2C_LEAD_UDMA_RX_DEST_REG_IDX_OFFSET 0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_lead_udma_rx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_lead_udma_rx_dest_reg_idx_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_i2c_lead_udma_rx_dest_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_lead_udma_rx_dest_reg_idx : public vp::reg_8
        {
        public:
        };

|

.. _udma_i2c_LEAD_UDMA_TX_DEST_REG_IDX:

LEAD_UDMA_TX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c leader tx udma channel

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

        
                // i2c leader tx udma channel
                #define UDMA_I2C_LEAD_UDMA_TX_DEST_REG_IDX_OFFSET 0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_lead_udma_tx_dest_reg_idx_get(uint32_t base);
        static inline void udma_i2c_lead_udma_tx_dest_reg_idx_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_i2c_lead_udma_tx_dest_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_lead_udma_tx_dest_reg_idx : public vp::reg_8
        {
        public:
        };

|

.. _udma_i2c_STATUS_REG_IDX:

STATUS_REG_IDX
""""""""""""""

incoming event flag / emit event

.. table:: 

    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |Bit #|R/W|                   Name                   |                             Description                              |
    +=====+===+==========================================+======================================================================+
    |    0|R/W|STATUS_FOLL_SOF_RCV_EVENT_I_IDX           |RW1C a leader read addressed this follower.                           |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |    1|R/W|STATUS_FOLL_SOF_SND_EVENT_I_IDX           |RW1C a leader write addressed this follower.                          |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |    2|R/W|STATUS_FOLL_EOF_RCV_EVENT_I_IDX           |RW1C a leader read addressed this follower and closed transfer.       |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |    3|R/W|STATUS_FOLL_EOF_SND_EVENT_I_IDX           |RW1C a leader write addressed this follower and closed transfer.      |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |    4|R/W|STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX        |RW1C follower is addressed and received an arbitration loss.          |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |    5|R/W|STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX     |RW1C follower is addressed and noticed incorrect framing.             |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   14|W  |STATUS_FOLL_UNLOCK_EVENT_O_IDX            |W1 unlock follower tx rx channels after a sof/eof/error event         |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   15|W  |STATUS_FOLL_PURGE_EVENT_O_IDX             |W1 purge tx rx channels fifos or any remaining data                   |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   16|R/W|STATUS_LEAD_ERROR_NACK_EVENT_I_IDX        |RW1C leader encountered an unexpected nack.                           |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   17|R/W|STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX        |RW1C leader encountered an arbitration loss.                          |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   18|R/W|STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX     |RW1C leader encountered an incorrect framing.                         |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   19|R/W|STATUS_LEAD_COMMAND_EVENT_I_IDX           |RW1C leader emitted a command event. does not lock.                   |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   22|W  |STATUS_LEAD_UNLOCK_EVENT_O_IDX            |W1 unlock leader tx rx channels after a sof/eof/error event           |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   23|W  |STATUS_LEAD_PURGE_EVENT_O_IDX             |W1 purge cmd tx rx leader fifos or any remaining data                 |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   24|W  |STATUS_I2C_SOFT_RESET_EVENT_O_IDX         |W1 soft reset i2c, if it is deadlocked due to bus error               |
    +-----+---+------------------------------------------+----------------------------------------------------------------------+
    |   25|W  |STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX|W1 by default, prescaler divides by 100. Divide by 10 using this event|
    +-----+---+------------------------------------------+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // incoming event flag / emit event
                #define UDMA_I2C_STATUS_REG_IDX_OFFSET           0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_i2c_status_reg_idx_get(uint32_t base);
        static inline void udma_i2c_status_reg_idx_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // RW1C a leader read addressed this follower. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_BIT  0
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_MASK 0x1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader write addressed this follower. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_BIT  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_MASK 0x2
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader read addressed this follower and closed transfer. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_BIT  2
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_MASK 0x4
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_RESET 0x0
        
        // RW1C a leader write addressed this follower and closed transfer. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_BIT  3
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_MASK 0x8
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_RESET 0x0
        
        // RW1C follower is addressed and received an arbitration loss. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_BIT 4
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_MASK 0x10
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_RESET 0x0
        
        // RW1C follower is addressed and noticed incorrect framing. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_BIT 5
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_MASK 0x20
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_RESET 0x0
        
        // W1 unlock follower tx rx channels after a sof/eof/error event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_BIT   14
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_MASK  0x4000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_RESET 0x0
        
        // W1 purge tx rx channels fifos or any remaining data (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_BIT    15
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_WIDTH  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_MASK   0x8000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_RESET  0x0
        
        // RW1C leader encountered an unexpected nack. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_BIT 16
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_MASK 0x10000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader encountered an arbitration loss. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_BIT 17
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_MASK 0x20000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader encountered an incorrect framing. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_BIT 18
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_MASK 0x40000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_RESET 0x0
        
        // RW1C leader emitted a command event. does not lock. (access: R/W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_BIT  19
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_MASK 0x80000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_RESET 0x0
        
        // W1 unlock leader tx rx channels after a sof/eof/error event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_BIT   22
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_MASK  0x400000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_RESET 0x0
        
        // W1 purge cmd tx rx leader fifos or any remaining data (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_BIT    23
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_WIDTH  1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_MASK   0x800000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_RESET  0x0
        
        // W1 soft reset i2c, if it is deadlocked due to bus error (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_BIT 24
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_MASK 0x1000000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_RESET 0x0
        
        // W1 by default, prescaler divides by 100. Divide by 10 using this event (access: W)
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_BIT 25
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_WIDTH 1
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_MASK 0x2000000
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_RESET 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_RCV_EVENT_I_IDX(val) ((val) << 0)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_SOF_SND_EVENT_I_IDX(val) ((val) << 1)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_RCV_EVENT_I_IDX(val) ((val) << 2)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_EOF_SND_EVENT_I_IDX(val) ((val) << 3)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_ARLO_EVENT_I_IDX(val) ((val) << 4)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_ERROR_FRAMING_EVENT_I_IDX(val) ((val) << 5)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,14))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_UNLOCK_EVENT_O_IDX(val) ((val) << 14)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_FOLL_PURGE_EVENT_O_IDX(val) ((val) << 15)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_NACK_EVENT_I_IDX(val) ((val) << 16)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,17))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_ARLO_EVENT_I_IDX(val) ((val) << 17)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,18))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_ERROR_FRAMING_EVENT_I_IDX(val) ((val) << 18)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_GET(value) (GAP_BEXTRACTU((value),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_GETS(value) (GAP_BEXTRACT((value),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,19))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_COMMAND_EVENT_I_IDX(val) ((val) << 19)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,22))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_UNLOCK_EVENT_O_IDX(val) ((val) << 22)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,23))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_LEAD_PURGE_EVENT_O_IDX(val) ((val) << 23)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,24))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_SOFT_RESET_EVENT_O_IDX(val) ((val) << 24)
        
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_GET(value) (GAP_BEXTRACTU((value),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_GETS(value) (GAP_BEXTRACT((value),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define UDMA_I2C_STATUS_REG_IDX_STATUS_I2C_PRESCALER_SET_DIV10_EVENT_O_IDX(val) ((val) << 25)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int status_foll_sof_rcv_event_i_idx:1 ; // RW1C a leader read addressed this follower.
            unsigned int status_foll_sof_snd_event_i_idx:1 ; // RW1C a leader write addressed this follower.
            unsigned int status_foll_eof_rcv_event_i_idx:1 ; // RW1C a leader read addressed this follower and closed transfer.
            unsigned int status_foll_eof_snd_event_i_idx:1 ; // RW1C a leader write addressed this follower and closed transfer.
            unsigned int status_foll_error_arlo_event_i_idx:1 ; // RW1C follower is addressed and received an arbitration loss.
            unsigned int status_foll_error_framing_event_i_idx:1 ; // RW1C follower is addressed and noticed incorrect framing.
            unsigned int padding0:8 ;
            unsigned int status_foll_unlock_event_o_idx:1 ; // W1 unlock follower tx rx channels after a sof/eof/error event
            unsigned int status_foll_purge_event_o_idx:1 ; // W1 purge tx rx channels fifos or any remaining data
            unsigned int status_lead_error_nack_event_i_idx:1 ; // RW1C leader encountered an unexpected nack.
            unsigned int status_lead_error_arlo_event_i_idx:1 ; // RW1C leader encountered an arbitration loss.
            unsigned int status_lead_error_framing_event_i_idx:1 ; // RW1C leader encountered an incorrect framing.
            unsigned int status_lead_command_event_i_idx:1 ; // RW1C leader emitted a command event. does not lock.
            unsigned int padding1:2 ;
            unsigned int status_lead_unlock_event_o_idx:1 ; // W1 unlock leader tx rx channels after a sof/eof/error event
            unsigned int status_lead_purge_event_o_idx:1 ; // W1 purge cmd tx rx leader fifos or any remaining data
            unsigned int status_i2c_soft_reset_event_o_idx:1 ; // W1 soft reset i2c, if it is deadlocked due to bus error
            unsigned int status_i2c_prescaler_set_div10_event_o_idx:1 ; // W1 by default, prescaler divides by 100. Divide by 10 using this event
          };
          unsigned int raw;
        } __attribute__((packed)) udma_i2c_status_reg_idx_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_i2c_status_reg_idx : public vp::reg_32
        {
        public:
            inline void status_foll_sof_rcv_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_sof_rcv_event_i_idx_get();
            inline void status_foll_sof_snd_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_sof_snd_event_i_idx_get();
            inline void status_foll_eof_rcv_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_eof_rcv_event_i_idx_get();
            inline void status_foll_eof_snd_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_eof_snd_event_i_idx_get();
            inline void status_foll_error_arlo_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_error_arlo_event_i_idx_get();
            inline void status_foll_error_framing_event_i_idx_set(uint32_t value);
            inline uint32_t status_foll_error_framing_event_i_idx_get();
            inline void status_foll_unlock_event_o_idx_set(uint32_t value);
            inline uint32_t status_foll_unlock_event_o_idx_get();
            inline void status_foll_purge_event_o_idx_set(uint32_t value);
            inline uint32_t status_foll_purge_event_o_idx_get();
            inline void status_lead_error_nack_event_i_idx_set(uint32_t value);
            inline uint32_t status_lead_error_nack_event_i_idx_get();
            inline void status_lead_error_arlo_event_i_idx_set(uint32_t value);
            inline uint32_t status_lead_error_arlo_event_i_idx_get();
            inline void status_lead_error_framing_event_i_idx_set(uint32_t value);
            inline uint32_t status_lead_error_framing_event_i_idx_get();
            inline void status_lead_command_event_i_idx_set(uint32_t value);
            inline uint32_t status_lead_command_event_i_idx_get();
            inline void status_lead_unlock_event_o_idx_set(uint32_t value);
            inline uint32_t status_lead_unlock_event_o_idx_get();
            inline void status_lead_purge_event_o_idx_set(uint32_t value);
            inline uint32_t status_lead_purge_event_o_idx_get();
            inline void status_i2c_soft_reset_event_o_idx_set(uint32_t value);
            inline uint32_t status_i2c_soft_reset_event_o_idx_get();
            inline void status_i2c_prescaler_set_div10_event_o_idx_set(uint32_t value);
            inline uint32_t status_i2c_prescaler_set_div10_event_o_idx_get();
        };

|

I2C micro-code
^^^^^^^^^^^^^^

.. table:: 

    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |                                       Name                                       |Offset|Width|                             Description                             |
    +==================================================================================+======+=====+=====================================================================+
    |:ref:`CMD_MISC_NOP<I2C micro-code_CMD_MISC_NOP>`                                  |    32|0x00 |does nothing                                                         |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_MISC_WAIT<I2C micro-code_CMD_MISC_WAIT>`                                |    32|0x01 |wait one i2c clock cycle                                             |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_MISC_REPEAT<I2C micro-code_CMD_MISC_REPEAT>`                            |    32|0x02 |reload the 16 bit repeat downcounter at cmd[15:0]                    |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_MISC_WAIT_I2C_PERIOD_END<I2C micro-code_CMD_MISC_WAIT_I2C_PERIOD_END>`  |    32|0x03 |wait one i2c scl period cycle                                        |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_TIMING<I2C micro-code_CMD_TIMING>`                                      |    32|0x10 |setup i2c_clk period and i2c scl low/high delays                     |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_FOLL_ADDR<I2C micro-code_CMD_FOLL_ADDR>`                                |    32|0x20 |setup follower adressing &amp; events                                |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_START<I2C micro-code_CMD_LEAD_START>`                              |    32|0x30 |send a start condition, the first start                              |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND<I2C micro-code_CMD_LEAD_SEND>`                                |    32|0x31 |send a byte from tx stream, check ack                                |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND_IMM<I2C micro-code_CMD_LEAD_SEND_IMM>`                        |    32|0x32 |send a byte from cmd[7:0], check ack                                 |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND_IMM_ADDR_7BITS<I2C micro-code_CMD_LEAD_SEND_IMM_ADDR_7BITS>`  |    32|0x37 |send a addr byte from cmd[7:0], check ack                            |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND_IMM_ADDR_10BITS<I2C micro-code_CMD_LEAD_SEND_IMM_ADDR_10BITS>`|    32|0x38 |send a addr10H+L bytes from cmd[15:8] (h) and cmd[7:0] (l), check ack|
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_RECV<I2C micro-code_CMD_LEAD_RECV>`                                |    32|0x33 |receives a byte that is not the last byte, send ack                  |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_RECV_LAST<I2C micro-code_CMD_LEAD_RECV_LAST>`                      |    32|0x34 |receives a byte that is the last byte, send nack                     |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_RESTART<I2C micro-code_CMD_LEAD_RESTART>`                          |    32|0x35 |send a restart condition                                             |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_LEAD_STOP<I2C micro-code_CMD_LEAD_STOP>`                                |    32|0x36 |generates a stop condition                                           |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_EVENT_RECV<I2C micro-code_CMD_EVENT_RECV>`                              |    32|0x40 |wait for an external event of index cmd[1:0]                         |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_EVENT_SEND<I2C micro-code_CMD_EVENT_SEND>`                              |    32|0x41 |send an event, named command_event                                   |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_UDMA_TX_CHAN_CFG<I2C micro-code_CMD_UDMA_TX_CHAN_CFG>`                  |    32|0x50 |send a configuration command to udma tx channel                      |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CMD_UDMA_RX_CHAN_CFG<I2C micro-code_CMD_UDMA_RX_CHAN_CFG>`                  |    32|0x51 |send a configuration command to udma rx channel                      |
    +----------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+

.. _I2C micro-code_CMD_MISC_NOP:

CMD_MISC_NOP
""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x00 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_MISC_WAIT:

CMD_MISC_WAIT
"""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x01 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_MISC_REPEAT:

CMD_MISC_REPEAT
"""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x02 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_MISC_WAIT_I2C_PERIOD_END:

CMD_MISC_WAIT_I2C_PERIOD_END
""""""""""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x03 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_TIMING:

CMD_TIMING
""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x10 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_FOLL_ADDR:

CMD_FOLL_ADDR
"""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x20 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_START:

CMD_LEAD_START
""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x30 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_SEND:

CMD_LEAD_SEND
"""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x31 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_SEND_IMM:

CMD_LEAD_SEND_IMM
"""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x32 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_SEND_IMM_ADDR_7BITS:

CMD_LEAD_SEND_IMM_ADDR_7BITS
""""""""""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x37 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_SEND_IMM_ADDR_10BITS:

CMD_LEAD_SEND_IMM_ADDR_10BITS
"""""""""""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x38 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_RECV:

CMD_LEAD_RECV
"""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x33 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_RECV_LAST:

CMD_LEAD_RECV_LAST
""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x34 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_RESTART:

CMD_LEAD_RESTART
""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x35 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_LEAD_STOP:

CMD_LEAD_STOP
"""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x36 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_EVENT_RECV:

CMD_EVENT_RECV
""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x40 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_EVENT_SEND:

CMD_EVENT_SEND
""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x41 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_UDMA_TX_CHAN_CFG:

CMD_UDMA_TX_CHAN_CFG
""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x50 |           |
    +-----+-----+-----------+

.. _I2C micro-code_CMD_UDMA_RX_CHAN_CFG:

CMD_UDMA_RX_CHAN_CFG
""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x51 |           |
    +-----+-----+-----------+

CMD_MISC_WAIT_I2C_PERIOD_END
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_REPEAT
^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_TIMING
^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_FOLL_ADDR
^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_LEAD
^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_UDMA_TX_CHAN_CFG
^^^^^^^^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

CMD_UDMA_RX_CHAN_CFG
^^^^^^^^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

Follower mode
^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

Leader mode
^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+
