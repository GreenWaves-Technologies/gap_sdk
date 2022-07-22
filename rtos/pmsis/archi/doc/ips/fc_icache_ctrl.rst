.. 
   Input file: docs/IP_REFERENCES/FC_ICACHE_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-------------------------------------------+------+-----+-----------------------------------------------------------+
    |                   Name                    |Offset|Width|                        Description                        |
    +===========================================+======+=====+===========================================================+
    |:ref:`ENABLE<fc_icache_ctrl__ENABLE>`      |0x0   |   32|FC instruction cache unit enable configuration register.   |
    +-------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`FLUSH<fc_icache_ctrl__FLUSH>`        |0x4   |   32|FC instruction cache unit flush command register.          |
    +-------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SEL_FLUSH<fc_icache_ctrl__SEL_FLUSH>`|0x8   |   32|FC instruction cache unit selective flush command register.|
    +-------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`STATUS<fc_icache_ctrl__STATUS>`      |0xC   |   32|FC instruction cache unit status register.                 |
    +-------------------------------------------+------+-----+-----------------------------------------------------------+

.. _fc_icache_ctrl__ENABLE:

ENABLE
""""""

FC instruction cache unit enable configuration register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                     Description                      |
    +=====+===+======+=====+======================================================+
    |    0|W  |ENABLE|0x0  |Enable FC instruction cache: b0: disabled; b1: enabled|
    +-----+---+------+-----+------------------------------------------------------+

.. _fc_icache_ctrl__FLUSH:

FLUSH
"""""

FC instruction cache unit flush command register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-----------------------------------------------+
    |Bit #|R/W|Name |Reset|                  Description                  |
    +=====+===+=====+=====+===============================================+
    |    0|W  |FLUSH|0x0  |Write 1 to fully flush the FC instruction cache|
    +-----+---+-----+-----+-----------------------------------------------+

.. _fc_icache_ctrl__SEL_FLUSH:

SEL_FLUSH
"""""""""

FC instruction cache unit selective flush command register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+----------------------------------------+
    |Bit #|R/W|  Name   |Reset|              Description               |
    +=====+===+=========+=====+========================================+
    |31:0 |W  |SEL_FLUSH|0x0  |Write an address to selectively flush it|
    +-----+---+---------+-----+----------------------------------------+

.. _fc_icache_ctrl__STATUS:

STATUS
""""""

FC instruction cache unit status register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                     Description                                     |
    +=====+===+======+=====+=====================================================================================+
    |    0|R  |STATUS|0x0  |Pending action status flag: b0: no pending caching action; b1: pending caching action|
    +-----+---+------+-----+-------------------------------------------------------------------------------------+
