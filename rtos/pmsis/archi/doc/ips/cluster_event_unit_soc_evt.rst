.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_soc_evt.md

Register map for SoC events
^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-------------------------------------------------------------------------------+------+-----+-------------------------------------------------------------+
    |                                     Name                                      |Offset|Width|                         Description                         |
    +===============================================================================+======+=====+=============================================================+
    |:ref:`EU_CORE_CURRENT_EVENT<cluster_event_unit_soc_evt__EU_CORE_CURRENT_EVENT>`|0x0   |   32|Access to SoC event FIFO (only accessible from CL_EVENT_UNIT)|
    +-------------------------------------------------------------------------------+------+-----+-------------------------------------------------------------+

.. _cluster_event_unit_soc_evt__EU_CORE_CURRENT_EVENT:

EU_CORE_CURRENT_EVENT
"""""""""""""""""""""

Access to SoC event FIFO (only accessible from CL_EVENT_UNIT)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                                                                      Description                                                                       |
    +=====+===+========+=====+========================================================================================================================================================+
    |7:0  |R  |EVENT_ID|0x00 |Oldest SoC peripheral event ID that was written into the FIFO. After a read of this register, the next event ID (if any) will be read at the next access|
    +-----+---+--------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31   |R  |VALID   |0x0  |Valid bit: if read as b1, the EVENT_ID field is valid                                                                                                   |
    +-----+---+--------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------+
