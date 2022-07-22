.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_mutex.md

Register map for mutex events
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |                                Name                                 |Offset|Width|                          Description                          |
    +=====================================================================+======+=====+===============================================================+
    |:ref:`EU_CORE_HW_MUTEX0<cluster_event_unit_mutex__EU_CORE_HW_MUTEX0>`|0x0   |   32|Access to HW mutex 0 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX1<cluster_event_unit_mutex__EU_CORE_HW_MUTEX1>`|0x4   |   32|Access to HW mutex 1 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX2<cluster_event_unit_mutex__EU_CORE_HW_MUTEX2>`|0x8   |   32|Access to HW mutex 2 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX3<cluster_event_unit_mutex__EU_CORE_HW_MUTEX3>`|0xC   |   32|Access to HW mutex 3 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX4<cluster_event_unit_mutex__EU_CORE_HW_MUTEX4>`|0x10  |   32|Access to HW mutex 4 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX5<cluster_event_unit_mutex__EU_CORE_HW_MUTEX5>`|0x14  |   32|Access to HW mutex 5 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX6<cluster_event_unit_mutex__EU_CORE_HW_MUTEX6>`|0x18  |   32|Access to HW mutex 6 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+
    |:ref:`EU_CORE_HW_MUTEX7<cluster_event_unit_mutex__EU_CORE_HW_MUTEX7>`|0x1C  |   32|Access to HW mutex 7 (only accessible from CL_EVENT_UNIT_DEMUX)|
    +---------------------------------------------------------------------+------+-----+---------------------------------------------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX0:

EU_CORE_HW_MUTEX0
"""""""""""""""""

Access to HW mutex 0 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |  Reset   |                                                                                                                                                                            Description                                                                                                                                                                            |
    +=====+===+=====+==========+===================================================================================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|VALUE|0x00000000|Reading tries to lock the corresponding mutex, writing unlocks it. The value written during an unlock is transmitted to the core which locks the mutex next. Note that there is no protection against illegal unlocks in hardware, i.e. the runtime SW is responsible for making sure that only the core that has locked a mutex performs a write on this register.|
    +-----+---+-----+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX1:

EU_CORE_HW_MUTEX1
"""""""""""""""""

Access to HW mutex 1 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX2:

EU_CORE_HW_MUTEX2
"""""""""""""""""

Access to HW mutex 2 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX3:

EU_CORE_HW_MUTEX3
"""""""""""""""""

Access to HW mutex 3 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX4:

EU_CORE_HW_MUTEX4
"""""""""""""""""

Access to HW mutex 4 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX5:

EU_CORE_HW_MUTEX5
"""""""""""""""""

Access to HW mutex 5 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX6:

EU_CORE_HW_MUTEX6
"""""""""""""""""

Access to HW mutex 6 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+

.. _cluster_event_unit_mutex__EU_CORE_HW_MUTEX7:

EU_CORE_HW_MUTEX7
"""""""""""""""""

Access to HW mutex 7 (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+----------------------------+
    |Bit #|R/W|Name |  Reset   |        Description         |
    +=====+===+=====+==========+============================+
    |31:0 |R/W|VALUE|0x00000000|Same behavior as HW mutex 0.|
    +-----+---+-----+----------+----------------------------+
