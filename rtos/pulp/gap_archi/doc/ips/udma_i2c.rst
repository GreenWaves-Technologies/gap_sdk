Input file: fe/ips/udma/udma_i2c/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |                                Name                                |Offset|Width|          Description           |
    +====================================================================+======+=====+================================+
    |:ref:`FOLL_UDMA_RX_DEST_REG_IDX<udma_i2c_FOLL_UDMA_RX_DEST_REG_IDX>`|     0|   32|i2c follower rx udma channel    |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`FOLL_UDMA_TX_DEST_REG_IDX<udma_i2c_FOLL_UDMA_TX_DEST_REG_IDX>`|     4|   32|i2c follower tx udma channel    |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`UDMA_CMD_DEST_REG_IDX<udma_i2c_UDMA_CMD_DEST_REG_IDX>`        |     8|   32|command tx udma channel         |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`LEAD_UDMA_RX_DEST_REG_IDX<udma_i2c_LEAD_UDMA_RX_DEST_REG_IDX>`|    12|   32|i2c leader rx udma channel      |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`LEAD_UDMA_TX_DEST_REG_IDX<udma_i2c_LEAD_UDMA_TX_DEST_REG_IDX>`|    16|   32|i2c leader tx udma channel      |
    +--------------------------------------------------------------------+------+-----+--------------------------------+
    |:ref:`STATUS_REG_IDX<udma_i2c_STATUS_REG_IDX>`                      |    20|   32|incoming event flag / emit event|
    +--------------------------------------------------------------------+------+-----+--------------------------------+

.. _udma_i2c_FOLL_UDMA_RX_DEST_REG_IDX:

FOLL_UDMA_RX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c follower rx udma channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_i2c_FOLL_UDMA_TX_DEST_REG_IDX:

FOLL_UDMA_TX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c follower tx udma channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_i2c_UDMA_CMD_DEST_REG_IDX:

UDMA_CMD_DEST_REG_IDX
"""""""""""""""""""""

command tx udma channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_i2c_LEAD_UDMA_RX_DEST_REG_IDX:

LEAD_UDMA_RX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c leader rx udma channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_i2c_LEAD_UDMA_TX_DEST_REG_IDX:

LEAD_UDMA_TX_DEST_REG_IDX
"""""""""""""""""""""""""

i2c leader tx udma channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

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

I2C micro-code
^^^^^^^^^^^^^^

.. table:: 

    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |                                      Name                                      |Offset|Width|                         Description                          |
    +================================================================================+======+=====+==============================================================+
    |:ref:`CMD_MISC_NOP<I2C micro-code_CMD_MISC_NOP>`                                |    32|0x00 |does nothing                                                  |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_MISC_WAIT<I2C micro-code_CMD_MISC_WAIT>`                              |    32|0x01 |wait one i2c clock cycle                                      |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_MISC_REPEAT<I2C micro-code_CMD_MISC_REPEAT>`                          |    32|0x02 |reload the 16 bit repeat downcounter at cmd[15:0]             |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_MISC_WAIT_I2C_PERIOD_END<I2C micro-code_CMD_MISC_WAIT_I2C_PERIOD_END>`|    32|0x03 |wait one i2c scl period cycle                                 |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_TIMING<I2C micro-code_CMD_TIMING>`                                    |    32|0x10 |setup i2c_clk period and i2c scl low/high delays              |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_FOLL_ADDR<I2C micro-code_CMD_FOLL_ADDR>`                              |    32|0x20 |setup follower adressing &amp; events                         |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_START<I2C micro-code_CMD_LEAD_START>`                            |    32|0x30 |send a (re)start condition                                    |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND<I2C micro-code_CMD_LEAD_SEND>`                              |    32|0x31 |send a byte from tx stream, check ack                         |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND_IMM<I2C micro-code_CMD_LEAD_SEND_IMM>`                      |    32|0x32 |send a byte from cmd[7:0], check ack                          |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_SEND_IMM_ADDR<I2C micro-code_CMD_LEAD_SEND_IMM_ADDR>`            |    32|0x37 |send a addr7 from cmd[7:0] or addr10 from cmd[15:0], check ack|
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_RECV<I2C micro-code_CMD_LEAD_RECV>`                              |    32|0x33 |receives a byte that is not the last byte, send ack           |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_RECV_LAST<I2C micro-code_CMD_LEAD_RECV_LAST>`                    |    32|0x34 |receives a byte that is the last byte, send nack              |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_LEAD_STOP<I2C micro-code_CMD_LEAD_STOP>`                              |    32|0x36 |generates a stop condition                                    |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_EVENT_RECV<I2C micro-code_CMD_EVENT_RECV>`                            |    32|0x40 |wait for an external event of index cmd[1:0]                  |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_EVENT_SEND<I2C micro-code_CMD_EVENT_SEND>`                            |    32|0x41 |send an event, named command_event                            |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_UDMA_TX_CHAN_CFG<I2C micro-code_CMD_UDMA_TX_CHAN_CFG>`                |    32|0x50 |send a configuration command to udma tx channel               |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`CMD_UDMA_RX_CHAN_CFG<I2C micro-code_CMD_UDMA_RX_CHAN_CFG>`                |    32|0x51 |send a configuration command to udma rx channel               |
    +--------------------------------------------------------------------------------+------+-----+--------------------------------------------------------------+

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

.. _I2C micro-code_CMD_LEAD_SEND_IMM_ADDR:

CMD_LEAD_SEND_IMM_ADDR
""""""""""""""""""""""

.. table:: 

    +-----+-----+-----------+
    |Bit #|Value|Description|
    +=====+=====+===========+
    |31:0 |0x37 |           |
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

Handling undefined multimaster conditions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

Leader
^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

Follower
^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+

Leader &amp; Follower
^^^^^^^^^^^^^^^^^^^^^

.. table:: 

    +----+------+-----+-----------+
    |Name|Offset|Width|Description|
    +====+======+=====+===========+
    +----+------+-----+-----------+
