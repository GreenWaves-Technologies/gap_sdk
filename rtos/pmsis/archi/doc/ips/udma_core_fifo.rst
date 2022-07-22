.. 
   Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_FIFO_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +---------------------------------------------------+------+-----+--------------------------+
    |                       Name                        |Offset|Width|       Description        |
    +===================================================+======+=====+==========================+
    |:ref:`CFG_SA_BUFFER<udma_core_fifo__CFG_SA_BUFFER>`|0x0   |   32|Start address for the FIFO|
    +---------------------------------------------------+------+-----+--------------------------+
    |:ref:`CFG_SIZE<udma_core_fifo__CFG_SIZE>`          |0x8   |   32|Sets the size of the FIFO |
    +---------------------------------------------------+------+-----+--------------------------+
    |:ref:`CFG_EVT<udma_core_fifo__CFG_EVT>`            |0xC   |   32|Controls event generation |
    +---------------------------------------------------+------+-----+--------------------------+
    |:ref:`CFG_FIFO_FILL<udma_core_fifo__CFG_FIFO_FILL>`|0x18  |   32|Filling status of the FIFO|
    +---------------------------------------------------+------+-----+--------------------------+
    |:ref:`CFG_CTRL<udma_core_fifo__CFG_CTRL>`          |0x1C  |   32|FIFO control              |
    +---------------------------------------------------+------+-----+--------------------------+

.. _udma_core_fifo__CFG_SA_BUFFER:

CFG_SA_BUFFER
"""""""""""""

Start address for the FIFO

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                          Description                           |
    +=====+===+=======+=====+================================================================+
    |31:0 |R/W|ADDRESS|0x0  |Start address for the FIFO (the 8 upper bits are forced to 0x1C)|
    +-----+---+-------+-----+----------------------------------------------------------------+

.. _udma_core_fifo__CFG_SIZE:

CFG_SIZE
""""""""

Sets the size of the FIFO

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------+
    |Bit #|R/W|Name|Reset|                 Description                 |
    +=====+===+====+=====+=============================================+
    |19:0 |R/W|SIZE|0x10 |FIFO size in bytes (minimum is 16, i.e. 0x10)|
    +-----+---+----+-----+---------------------------------------------+

.. _udma_core_fifo__CFG_EVT:

CFG_EVT
"""""""

Controls event generation

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                    Description                    |
    +=====+===+=========+=====+===================================================+
    |20:0 |R/W|NUM_BYTES|0x0  |Number of bytes to be received to trigger an event |
    +-----+---+---------+-----+---------------------------------------------------+
    |31   |R/W|EN       |0x0  |Enables event generation: b0: disabled; b1: enabled|
    +-----+---+---------+-----+---------------------------------------------------+

.. _udma_core_fifo__CFG_FIFO_FILL:

CFG_FIFO_FILL
"""""""""""""

Filling status of the FIFO

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------------------+
    |Bit #|R/W|  Name   |Reset|        Description        |
    +=====+===+=========+=====+===========================+
    |20:0 |R  |NUM_BYTES|0x0  |Number of bytes in the FIFO|
    +-----+---+---------+-----+---------------------------+

.. _udma_core_fifo__CFG_CTRL:

CFG_CTRL
""""""""

FIFO control

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                                Description                                |
    +=====+===+===========+=====+===========================================================================+
    |    1|R/W|EN         |0x0  |Enable FIFO: b0: disabled; b1: enabled. If enabled, starts the FIFO on read|
    +-----+---+-----------+-----+---------------------------------------------------------------------------+
    |    4|W  |STOP       |0x0  |Disable FIFO: write b1 to stop the FIFO                                    |
    +-----+---+-----------+-----+---------------------------------------------------------------------------+
    |    8|R/W|TIMEOUT_MON|0x0  |Select the port to monitor: b0: monitor RX port; b1: Monitor TX port       |
    +-----+---+-----------+-----+---------------------------------------------------------------------------+
