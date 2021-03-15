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
    |:ref:`MODE<udma_ffc_MODE>`                |    24|   32|Direction and IO mode configuration                                               |
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

.. _udma_ffc_RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_FL_FORMAT:

FL_FORMAT
"""""""""

Floating point format - 2'b00 : FP16 - 2'b01 : BFP16- 2'b10, 2'b11 : FP32

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_FP_FORMAT:

FP_FORMAT
"""""""""

Fix point format - 2'b00 : 8bits - 2'b01 :16bits - 2'b10 : 24bits - 2'b11 : 32bits

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_FP_PREC:

FP_PREC
"""""""

Fix point precision, must less than fix point size

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_FP_SCALE:

FP_SCALE
""""""""

Fix point scale, signed number, FL * 2^scale

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_MODE:

MODE
""""

Direction and IO mode configuration

.. table:: 

    +-----+---+---------+--------------------------------------------------+
    |Bit #|R/W|  Name   |                   Description                    |
    +=====+===+=========+==================================================+
    |    0|R/W|DIRECTION|0: Float to fix, 1:Fix to float                   |
    +-----+---+---------+--------------------------------------------------+
    |2:1  |R/W|IO_MODE  |2'b00: MIMO, 2'b01: SIMO, 2'b10: MISO, 2'b11: SISO|
    +-----+---+---------+--------------------------------------------------+

.. _udma_ffc_TRANS_MODE:

TRANS_MODE
""""""""""

Use auto tranfer or not

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_CONV_NUM:

CONV_NUM
""""""""

Convertion data number

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_CONV_TX_ADDR:

CONV_TX_ADDR
""""""""""""

Only use when auto mode is set, convertion data tx channel read from l2 address

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_CONV_RX_ADDR:

CONV_RX_ADDR
""""""""""""

Only use when auto mode is set, convertion data tx channel read from l2 address

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_IRQ_EN:

IRQ_EN
""""""

Enable or disable transfer interrupt

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_STATUS:

STATUS
""""""

Show if the transfer is finished or not

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_ffc_START:

START
"""""

Start transition

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
