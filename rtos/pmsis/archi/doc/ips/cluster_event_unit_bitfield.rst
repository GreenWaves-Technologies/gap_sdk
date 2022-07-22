.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_bitfield.md

Register map for bitfields
^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +------------------------------------------------+------+-----+------------------------------------------------+
    |                      Name                      |Offset|Width|                  Description                   |
    +================================================+======+=====+================================================+
    |:ref:`VALUE<cluster_event_unit_bitfield__VALUE>`|0x0   |   32|Set or get the value of the bitfield            |
    +------------------------------------------------+------+-----+------------------------------------------------+
    |:ref:`SET<cluster_event_unit_bitfield__SET>`    |0x4   |   32|Selectively set bitfield bits                   |
    +------------------------------------------------+------+-----+------------------------------------------------+
    |:ref:`CLEAR<cluster_event_unit_bitfield__CLEAR>`|0x8   |   32|Selectively clear bitfield bits                 |
    +------------------------------------------------+------+-----+------------------------------------------------+
    |:ref:`ALLOC<cluster_event_unit_bitfield__ALLOC>`|0xC   |   32|Get position and clear least significant set bit|
    +------------------------------------------------+------+-----+------------------------------------------------+

.. _cluster_event_unit_bitfield__VALUE:

VALUE
"""""

Set or get the value of the bitfield

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+----------+-----------------------------+
    |Bit #|R/W|Name|  Reset   |         Description         |
    +=====+===+====+==========+=============================+
    |31:0 |R/W|BITS|0x00000000|Current value of the bitfield|
    +-----+---+----+----------+-----------------------------+

.. _cluster_event_unit_bitfield__SET:

SET
"""

Selectively set bitfield bits

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                              Description                               |
    +=====+===+========+==========+========================================================================+
    |31:0 |W  |SET_MASK|0x00000000|Write b1 to 1 or more bits to set the corresponding bits of the bitfield|
    +-----+---+--------+----------+------------------------------------------------------------------------+

.. _cluster_event_unit_bitfield__CLEAR:

CLEAR
"""""

Selectively clear bitfield bits

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                               Description                                |
    +=====+===+==========+==========+==========================================================================+
    |31:0 |W  |CLEAR_MASK|0x00000000|Write b1 to 1 or more bits to clear the corresponding bits of the bitfield|
    +-----+---+----------+----------+--------------------------------------------------------------------------+

.. _cluster_event_unit_bitfield__ALLOC:

ALLOC
"""""

Get position and clear least significant set bit

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |  Reset   |                                                                      Description                                                                       |
    +=====+===+=========+==========+========================================================================================================================================================+
    |31:0 |R  |FIRST_ONE|0x00000000|Reading this field returns the position (from 0 to 31) of the least significant bit set at 1, and clears it to 0. If no bit is set, the read value is 32|
    +-----+---+---------+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------+
