.. 
   Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_LIN_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |                            Name                            |Offset|Width|                                     Description                                      |
    +============================================================+======+=====+======================================================================================+
    |:ref:`CFG_SA_BUF0<udma_core_lin_addrgen__CFG_SA_BUF0>`      |0x0   |   32|Start address of 1st buffer used in legacy mode and continuous mode (HW double buffer)|
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_SA_BUF1<udma_core_lin_addrgen__CFG_SA_BUF1>`      |0x4   |   32|Start address of 2nd buffer used in continuous mode                                   |
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_SIZE<udma_core_lin_addrgen__CFG_SIZE>`            |0x8   |   32|Sets the size of the buffers                                                          |
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_CURR_ADDR<udma_core_lin_addrgen__CFG_CURR_ADDR>`  |0x14  |   32|Current address                                                                       |
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_BYTES_LEFT<udma_core_lin_addrgen__CFG_BYTES_LEFT>`|0x18  |   32|Number of bytes left                                                                  |
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+
    |:ref:`CFG_CTRL<udma_core_lin_addrgen__CFG_CTRL>`            |0x1C  |   32|Linear generator control                                                              |
    +------------------------------------------------------------+------+-----+--------------------------------------------------------------------------------------+

.. _udma_core_lin_addrgen__CFG_SA_BUF0:

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

.. _udma_core_lin_addrgen__CFG_SA_BUF1:

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

.. _udma_core_lin_addrgen__CFG_SIZE:

CFG_SIZE
""""""""

Sets the size of the buffers

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-----------------------------------------+
    |Bit #|R/W|Name|Reset|               Description               |
    +=====+===+====+=====+=========================================+
    |19:0 |R/W|SIZE|0x0  |Buffer size (i.e. transfer size) in bytes|
    +-----+---+----+-----+-----------------------------------------+

.. _udma_core_lin_addrgen__CFG_CURR_ADDR:

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

.. _udma_core_lin_addrgen__CFG_BYTES_LEFT:

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

.. _udma_core_lin_addrgen__CFG_CTRL:

CFG_CTRL
""""""""

Linear generator control

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
