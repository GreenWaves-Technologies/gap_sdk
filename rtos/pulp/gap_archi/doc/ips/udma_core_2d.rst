Input file: fe/ips/udma/udma_core/doc/UDMA_CORE_2D_ADDRGEN_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |                       Name                       |Offset|Width|                                     Description                                     |
    +==================================================+======+=====+=====================================================================================+
    |:ref:`CFG_SA_BUF0<udma_core_2d_CFG_SA_BUF0>`      |     0|   32|Start address of 1st buffer used in legacy mode and continuous mode(hw double buffer)|
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_SA_BUF1<udma_core_2d_CFG_SA_BUF1>`      |     4|   32|Start address of 2nd buffer used in continuous mode                                  |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_SIZE<udma_core_2d_CFG_SIZE>`            |     8|   32|Sets the size of the buffer                                                          |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_STRIDE<udma_core_2d_CFG_STRIDE>`        |    12|   32|Distance in byte between the end of a row and the beginning of the following         |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_ROW_LEN<udma_core_2d_CFG_ROW_LEN>`      |    16|   32|Length of the row in bytes                                                           |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_CURR_ADDR<udma_core_2d_CFG_CURR_ADDR>`  |    20|   32|Current Address                                                                      |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_BYTES_LEFT<udma_core_2d_CFG_BYTES_LEFT>`|    24|   32|Number of bytes left                                                                 |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+
    |:ref:`CFG_CTRL<udma_core_2d_CFG_CTRL>`            |    28|   32|uDMA peripherals reset configuration. At RESET all periphs under reset.              |
    +--------------------------------------------------+------+-----+-------------------------------------------------------------------------------------+

.. _udma_core_2d_CFG_SA_BUF0:

CFG_SA_BUF0
"""""""""""

Start address of 1st buffer used in legacy mode and continuous mode(hw double buffer)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_SA_BUF1:

CFG_SA_BUF1
"""""""""""

Start address of 2nd buffer used in continuous mode

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_SIZE:

CFG_SIZE
""""""""

Sets the size of the buffer

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_STRIDE:

CFG_STRIDE
""""""""""

Distance in byte between the end of a row and the beginning of the following

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_ROW_LEN:

CFG_ROW_LEN
"""""""""""

Length of the row in bytes

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_CURR_ADDR:

CFG_CURR_ADDR
"""""""""""""

Current Address

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_BYTES_LEFT:

CFG_BYTES_LEFT
""""""""""""""

Number of bytes left

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_core_2d_CFG_CTRL:

CFG_CTRL
""""""""

uDMA peripherals reset configuration. At RESET all periphs under reset.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                       Description                                                                       |
    +=====+===+====+=========================================================================================================================================================+
    |    0|R/W|CONT|Enable hardware double buffer support - 1’b1 : HW Doble Buffer -1'b0 Legacy mode                                                                         |
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|EN  |Transfer enable - 1’b1 : Starts the transfer or enque a new transfer is a transfer is allready running. On read -1'b1 transfer on going -1'b0 no transfer|
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|W  |STOP|When set stops the current transfer and resets the address generator                                                                                     |
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
