.. 
   Input file: fe/ips/udma/udma_ffc/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-------------------------------------------+------+-----+----------------------------------------------+
    |                   Name                    |Offset|Width|                 Description                  |
    +===========================================+======+=====+==============================================+
    |:ref:`RX_DEST<udma_ffc__RX_DEST>`          |0x0   |   32|Stream ID for the uDMA RX channel             |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`TX_DEST<udma_ffc__TX_DEST>`          |0x4   |   32|Stream ID for the uDMA TX channel             |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`FL_FORMAT<udma_ffc__FL_FORMAT>`      |0x8   |   32|Floating point format                         |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`FP_FORMAT<udma_ffc__FP_FORMAT>`      |0xC   |   32|Fixed point format                            |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`FP_PREC<udma_ffc__FP_PREC>`          |0x10  |   32|Fixed point precision                         |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`FP_SCALE<udma_ffc__FP_SCALE>`        |0x14  |   32|Fixed point scale                             |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`MODE<udma_ffc__MODE>`                |0x18  |   32|Direction and I/O mode configuration          |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`TRANS_MODE<udma_ffc__TRANS_MODE>`    |0x1C  |   32|Use auto transfer or not                      |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`CONV_NUM<udma_ffc__CONV_NUM>`        |0x20  |   32|Number of conversions to be done              |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`CONV_TX_ADDR<udma_ffc__CONV_TX_ADDR>`|0x24  |   32|TX channel address in L2 when auto mode is set|
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`CONV_RX_ADDR<udma_ffc__CONV_RX_ADDR>`|0x28  |   32|RX channel address in L2 when auto mode is set|
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`IRQ_EN<udma_ffc__IRQ_EN>`            |0x2C  |   32|Enable or disable transfer interrupt          |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`STATUS<udma_ffc__STATUS>`            |0x30  |   32|Show if the transfer is finished or not       |
    +-------------------------------------------+------+-----+----------------------------------------------+
    |:ref:`START<udma_ffc__START>`              |0x34  |   32|Start processing                              |
    +-------------------------------------------+------+-----+----------------------------------------------+

.. _udma_ffc__RX_DEST:

RX_DEST
"""""""

Stream ID for the uDMA RX channel

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-----------------------------+
    |Bit #|R/W|Name |Reset|         Description         |
    +=====+===+=====+=====+=============================+
    |7:0  |R/W|ID_RX|0xFF |UDMA stream ID for RX channel|
    +-----+---+-----+-----+-----------------------------+

.. _udma_ffc__TX_DEST:

TX_DEST
"""""""

Stream ID for the uDMA TX channel

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-----------------------------+
    |Bit #|R/W|Name |Reset|         Description         |
    +=====+===+=====+=====+=============================+
    |7:0  |R/W|ID_TX|0xFF |UDMA stream ID for TX channel|
    +-----+---+-----+-----+-----------------------------+

.. _udma_ffc__FL_FORMAT:

FL_FORMAT
"""""""""

Floating point format

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-----------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                        Description                        |
    +=====+===+======+=====+===========================================================+
    |1:0  |R/W|FORMAT|0x0  |Floating point format: b00: FP16; b01: BFP16; b10-b11: FP32|
    +-----+---+------+-----+-----------------------------------------------------------+

.. _udma_ffc__FP_FORMAT:

FP_FORMAT
"""""""""

Fixed point format

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                              Description                               |
    +=====+===+======+=====+========================================================================+
    |1:0  |R/W|FORMAT|0x0  |Fixed point format: b00: 8 bits; b01: 16 bits; b10: 24 bits; b11: 32bits|
    +-----+---+------+-----+------------------------------------------------------------------------+

.. _udma_ffc__FP_PREC:

FP_PREC
"""""""

Fixed point precision

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+----------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                          Description                           |
    +=====+===+=========+=====+================================================================+
    |4:0  |R/W|PRECISION|0x0  |Precision of fixed point format (cannot exceed fixed point size)|
    +-----+---+---------+-----+----------------------------------------------------------------+

.. _udma_ffc__FP_SCALE:

FP_SCALE
""""""""

Fixed point scale

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+--------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                              Description                                               |
    +=====+===+=====+=====+========================================================================================================+
    |15:0 |R/W|SCALE|0x0  |Fixed point scale (signed number): data are multiplied by :math:`2^{scale}̀` to compute their real value|
    +-----+---+-----+-----+--------------------------------------------------------------------------------------------------------+

.. _udma_ffc__MODE:

MODE
""""

Direction and I/O mode configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+------------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                           Description                            |
    +=====+===+=========+=====+==================================================================+
    |    0|R/W|DIRECTION|0x0  |0: Floating point to fixed point; 1: Fixed point to floating point|
    +-----+---+---------+-----+------------------------------------------------------------------+
    |2:1  |R/W|IO_MODE  |0x0  |b00: MIMO; b01: SIMO; b10: MISO; b11: SISO                        |
    +-----+---+---------+-----+------------------------------------------------------------------+

.. _udma_ffc__TRANS_MODE:

TRANS_MODE
""""""""""

Use auto transfer or not

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+--------------------------------+
    |Bit #|R/W| Name  |Reset|          Description           |
    +=====+===+=======+=====+================================+
    |    0|R/W|AUTO_EN|0x0  |Set to 1 to enable auto transfer|
    +-----+---+-------+-----+--------------------------------+

.. _udma_ffc__CONV_NUM:

CONV_NUM
""""""""

Number of conversions to be done

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------------------------+
    |Bit #|R/W|Name|Reset|          Description           |
    +=====+===+====+=====+================================+
    |31:0 |R/W|NUM |0x0  |Number of conversions to be done|
    +-----+---+----+-----+--------------------------------+

.. _udma_ffc__CONV_TX_ADDR:

CONV_TX_ADDR
""""""""""""

TX channel address in L2 when auto mode is set

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                    Description                     |
    +=====+===+=======+=====+====================================================+
    |31:0 |R/W|ADDRESS|0x0  |In auto mode, gives the address in L2 for TX channel|
    +-----+---+-------+-----+----------------------------------------------------+

.. _udma_ffc__CONV_RX_ADDR:

CONV_RX_ADDR
""""""""""""

RX channel address in L2 when auto mode is set

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                    Description                     |
    +=====+===+=======+=====+====================================================+
    |31:0 |R/W|ADDRESS|0x0  |In auto mode, gives the address in L2 for RX channel|
    +-----+---+-------+-----+----------------------------------------------------+

.. _udma_ffc__IRQ_EN:

IRQ_EN
""""""

Enable or disable transfer interrupt

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------------------+
    |Bit #|R/W| Name |Reset|             Description             |
    +=====+===+======+=====+=====================================+
    |    0|R/W|ENABLE|0x0  |Set to 1 to enable transfer interrupt|
    +-----+---+------+-----+-------------------------------------+

.. _udma_ffc__STATUS:

STATUS
""""""

Show if the transfer is finished or not

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                               Description                               |
    +=====+===+====+=====+=========================================================================+
    |    0|R  |DONE|0x0  |Is set to 1 when the configured number of conversions have been completed|
    +-----+---+----+-----+-------------------------------------------------------------------------+
    |    1|R  |BUSY|0x0  |Is set to 1 while conversions are ongoing                                |
    +-----+---+----+-----+-------------------------------------------------------------------------+

.. _udma_ffc__START:

START
"""""

Start processing

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |    0|W  |START|0x0  |Write any value to start the processing of conversions according to configuration|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+
