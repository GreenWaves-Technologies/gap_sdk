.. 
   Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_2D_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |                       Name                        |Offset|Width|                                     Description                                      |
    +===================================================+======+=====+======================================================================================+
    |:ref:`CFG_SA_BUF0<udma_core_2d__CFG_SA_BUF0>`      |     0|   32|Start address of 1st buffer used in legacy mode and continuous mode (HW double buffer)|
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_SA_BUF1<udma_core_2d__CFG_SA_BUF1>`      |     4|   32|Start address of 2nd buffer used in continuous mode                                   |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_SIZE<udma_core_2d__CFG_SIZE>`            |     8|   32|Sets the size of the buffers                                                          |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_STRIDE<udma_core_2d__CFG_STRIDE>`        |    12|   32|Distance in byte between the end of a row and the beginning of the following          |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_ROW_LEN<udma_core_2d__CFG_ROW_LEN>`      |    16|   32|Length of the row in bytes                                                            |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_CURR_ADDR<udma_core_2d__CFG_CURR_ADDR>`  |    20|   32|Current address                                                                       |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_BYTES_LEFT<udma_core_2d__CFG_BYTES_LEFT>`|    24|   32|Number of bytes left                                                                  |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_CTRL<udma_core_2d__CFG_CTRL>`            |    28|   32|2D generator control                                                                  |
    +---------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+

.. _udma_core_2d__CFG_SA_BUF0:

CFG_SA_BUF0
"""""""""""

Start address of 1st buffer used in legacy mode and continuous mode (HW double buffer)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+---------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                             Description                             |
    +=====+===+=======+=====+=====================================================================+
    |31:0 |R/W|ADDRESS|0x0  |Start address of the 1st buffer (the 8 upper bits are forced to 0x1C)|
    +-----+---+-------+-----+---------------------------------------------------------------------+

.. _udma_core_2d__CFG_SA_BUF1:

CFG_SA_BUF1
"""""""""""

Start address of 2nd buffer used in continuous mode

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                                         Description                                          |
    +=====+===+=======+=====+==============================================================================================+
    |31:0 |R/W|ADDRESS|0x0  |Start address of the 2nd buffer, when in continuous mode (the 8 upper bits are forced to 0x1C)|
    +-----+---+-------+-----+----------------------------------------------------------------------------------------------+

.. _udma_core_2d__CFG_SIZE:

CFG_SIZE
""""""""

Sets the size of the buffers

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------+
    |Bit #|R/W|Name|Reset|     Description      |
    +=====+===+====+=====+======================+
    |19:0 |R/W|SIZE|0x0  |Transfer size in bytes|
    +-----+---+----+-----+----------------------+

.. _udma_core_2d__CFG_STRIDE:

CFG_STRIDE
""""""""""

Distance in byte between the end of a row and the beginning of the following

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+----------------------+
    |Bit #|R/W| Name |Reset|     Description      |
    +=====+===+======+=====+======================+
    |19:0 |R/W|STRIDE|0x0  |Stride length in bytes|
    +-----+---+------+-----+----------------------+

.. _udma_core_2d__CFG_ROW_LEN:

CFG_ROW_LEN
"""""""""""

Length of the row in bytes

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------+
    |Bit #|R/W| Name  |Reset|     Description      |
    +=====+===+=======+=====+======================+
    |19:0 |R/W|ROW_LEN|0x0  |Stride length in bytes|
    +-----+---+-------+-----+----------------------+

.. _udma_core_2d__CFG_CURR_ADDR:

CFG_CURR_ADDR
"""""""""""""

Current address

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                               Description                               |
    +=====+===+=======+=====+=========================================================================+
    |31:0 |R  |ADDRESS|0x0  |Value of current address in the buffer (the 8 upper bits are always 0x1C)|
    +-----+---+-------+-----+-------------------------------------------------------------------------+

.. _udma_core_2d__CFG_BYTES_LEFT:

CFG_BYTES_LEFT
""""""""""""""

Number of bytes left

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-------------------------------------+
    |Bit #|R/W|  Name   |Reset|             Description             |
    +=====+===+=========+=====+=====================================+
    |20:0 |R  |NUM_BYTES|0x0  |Number of remaining bytes to transfer|
    +-----+---+---------+-----+-------------------------------------+

.. _udma_core_2d__CFG_CTRL:

CFG_CTRL
""""""""

2D generator control

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                                              Description                                                               |
    +=====+===+====+=====+========================================================================================================================================+
    |    0|R/W|CONT|0x0  |Enable hardware double buffer support (continuous mode): b0: legacy mode; b1: continuous mode                                           |
    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|EN  |0x0  |Write b1 to start a transfer or enqueue a new transfer if one is already running. On read: b0: no ongoing transfer; b1: ongoing transfer|
    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------+
    |    4|W  |STOP|0x0  |Write b1 to stop the current transfer and reset the address generator                                                                   |
    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------+
