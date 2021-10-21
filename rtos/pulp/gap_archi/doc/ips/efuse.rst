.. 
   Input file: fe/ips/apb/apb_efuse_if/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------+------+-----+---------------------------------------------+
    |                 Name                  |Offset|Width|                 Description                 |
    +=======================================+======+=====+=============================================+
    |:ref:`CMD<efuse_CMD>`                  |     0|   32|CMD register for efuses                      |
    +---------------------------------------+------+-----+---------------------------------------------+
    |:ref:`CFG<efuse_CFG>`                  |     4|   32|CFG register for timings                     |
    +---------------------------------------+------+-----+---------------------------------------------+
    |:ref:`PAGE_PROTECT<efuse_PAGE_PROTECT>`|     8|   32|Protection (RO mode) register for efuse pages|
    +---------------------------------------+------+-----+---------------------------------------------+

.. _efuse_CMD:

CMD
"""

CMD register for efuses

.. table:: 

    +-----+---+-----+-----------------+
    |Bit #|R/W|Name |   Description   |
    +=====+===+=====+=================+
    |    0|R/W|READ |Enable read mode |
    +-----+---+-----+-----------------+
    |    1|R/W|WRITE|Enable write mode|
    +-----+---+-----+-----------------+
    |    2|R/W|IDLE |Enable idle mode |
    +-----+---+-----+-----------------+

.. _efuse_CFG:

CFG
"""

CFG register for timings

.. table:: 

    +-----+---+-----------+----------------------------------+
    |Bit #|R/W|   Name    |           Description            |
    +=====+===+===========+==================================+
    |5:0  |R/W|CNT_TARGET0|Enable or Disable TLB mode        |
    +-----+---+-----------+----------------------------------+
    |15:6 |R/W|CNT_TARGET1|Flag to check whether device is RO|
    +-----+---+-----------+----------------------------------+
    |29:16|R/W|CNT_TARGET2|Flag to check whether device is RO|
    +-----+---+-----------+----------------------------------+
    |31:30|R/W|MARGIN     |Flag to check whether device is RO|
    +-----+---+-----------+----------------------------------+

.. _efuse_PAGE_PROTECT:

PAGE_PROTECT
""""""""""""

Protection (RO mode) register for efuse pages

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
