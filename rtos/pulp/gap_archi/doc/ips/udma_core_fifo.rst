.. 
   Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_FIFO_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+
    |                       Name                       |Offset|Width|                              Description                              |
    +==================================================+======+=====+=======================================================================+
    |:ref:`CFG_SA_BUFFER<udma_core_fifo_CFG_SA_BUFFER>`|     0|   32|Start address for the FIFO                                             |
    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+
    |:ref:`CFG_SIZE<udma_core_fifo_CFG_SIZE>`          |     8|   32|Sets the size of the FIFO (minimum size is 16bytes 0x10)               |
    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+
    |:ref:`CFG_EVT<udma_core_fifo_CFG_EVT>`            |    12|   32|Controls event generation                                              |
    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+
    |:ref:`CFG_FIFO_FILL<udma_core_fifo_CFG_FIFO_FILL>`|    24|   32|FILL status of the FIFO                                                |
    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+
    |:ref:`CFG_CTRL<udma_core_fifo_CFG_CTRL>`          |    28|   32|uDMA peripherals reset configuration. At RESET all periphs under reset.|
    +--------------------------------------------------+------+-----+-----------------------------------------------------------------------+

.. _udma_core_fifo_CFG_SA_BUFFER:

CFG_SA_BUFFER
"""""""""""""

Start address for the FIFO

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_fifo_CFG_SIZE:

CFG_SIZE
""""""""

Sets the size of the FIFO (minimum size is 16bytes 0x10)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_fifo_CFG_EVT:

CFG_EVT
"""""""

Controls event generation

.. table:: 

    +-----+---+---------+--------------------------------------------+
    |Bit #|R/W|  Name   |                Description                 |
    +=====+===+=========+============================================+
    |20:0 |R/W|NUM_BYTES|Number of bytes received that trigger event.|
    +-----+---+---------+--------------------------------------------+
    |31   |R/W|EN       |Enables event generation                    |
    +-----+---+---------+--------------------------------------------+

.. _udma_core_fifo_CFG_FIFO_FILL:

CFG_FIFO_FILL
"""""""""""""

FILL status of the FIFO

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_fifo_CFG_CTRL:

CFG_CTRL
""""""""

uDMA peripherals reset configuration. At RESET all periphs under reset.

.. table:: 

    +-----+---+-----------+-----------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |                                    Description                                    |
    +=====+===+===========+===================================================================================+
    |    1|R/W|EN         |FIFO enable - 1’b1 : Starts the FIFO on read -1'b1 FIFO enabled -1'b0 FIFO disabled|
    +-----+---+-----------+-----------------------------------------------------------------------------------+
    |    4|W  |STOP       |FIFO disable - 1’b1 : Stops the FIFO                                               |
    +-----+---+-----------+-----------------------------------------------------------------------------------+
    |    8|R/W|TIMEOUT_MON|Selects the port to monitor - 1'b0 : Monitor RX port -1’b1 : Monitor TX port       |
    +-----+---+-----------+-----------------------------------------------------------------------------------+
