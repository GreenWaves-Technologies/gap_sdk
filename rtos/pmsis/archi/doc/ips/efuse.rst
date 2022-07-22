.. 
   Input file: fe/ips/apb/apb_efuse_if/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +----------------------------------------+------+-----+---------------------------------------------+
    |                  Name                  |Offset|Width|                 Description                 |
    +========================================+======+=====+=============================================+
    |:ref:`CMD<efuse__CMD>`                  |0x0   |   32|CMD register for eFuses                      |
    +----------------------------------------+------+-----+---------------------------------------------+
    |:ref:`CFG<efuse__CFG>`                  |0x4   |   32|CFG register for timings                     |
    +----------------------------------------+------+-----+---------------------------------------------+
    |:ref:`PAGE_PROTECT<efuse__PAGE_PROTECT>`|0x8   |   32|Protection (RO mode) register for eFuse pages|
    +----------------------------------------+------+-----+---------------------------------------------+

.. _efuse__CMD:

CMD
"""

CMD register for eFuses

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-----------------+
    |Bit #|R/W|Name |Reset|   Description   |
    +=====+===+=====+=====+=================+
    |    0|R/W|READ |0x0  |Enable read mode |
    +-----+---+-----+-----+-----------------+
    |    1|R/W|WRITE|0x0  |Enable write mode|
    +-----+---+-----+-----+-----------------+
    |    2|R/W|IDLE |0x0  |Enable idle mode |
    +-----+---+-----+-----+-----------------+

.. _efuse__CFG:

CFG
"""

CFG register for timings

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+---------------------------------------+
    |Bit #|R/W|   Name    |Reset|              Description              |
    +=====+===+===========+=====+=======================================+
    |5:0  |R/W|CNT_TARGET0|0x2  |Counter value to generate short delays |
    +-----+---+-----------+-----+---------------------------------------+
    |15:6 |R/W|CNT_TARGET1|0x32 |Counter value to generate medium delays|
    +-----+---+-----------+-----+---------------------------------------+
    |29:16|R/W|CNT_TARGET2|0x1F4|Counter value to generate long delays  |
    +-----+---+-----------+-----+---------------------------------------+
    |31:30|R/W|MARGIN     |0x0  |EFuse read margin                      |
    +-----+---+-----------+-----+---------------------------------------+

.. _efuse__PAGE_PROTECT:

PAGE_PROTECT
""""""""""""

Protection (RO mode) register for eFuse pages

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-----------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                           Description                           |
    +=====+===+=========+=====+=================================================================+
    |7:0  |R/W|PROTECTED|0x0  |Protected pages. Bit *i* is 1 means page *i* is in read only mode|
    +-----+---+---------+-----+-----------------------------------------------------------------+
    |31   |R/W|LOCK     |0x0  |If 1, PAGE_PROTECT register cannot be overwritten                |
    +-----+---+---------+-----+-----------------------------------------------------------------+
