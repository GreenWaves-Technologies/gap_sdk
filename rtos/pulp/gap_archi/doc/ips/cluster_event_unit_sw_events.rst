.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_sw_events.md

Register map for software events
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |                                                   Name                                                    |Offset|Width|                    Description                     |
    +===========================================================================================================+======+=====+====================================================+
    |:ref:`EU_CORE_TRIGG_SW_EVENT0<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0>`                      |     0|   32|Trigger SW event 0                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT1<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1>`                      |     4|   32|Trigger SW event 1                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT2<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2>`                      |     8|   32|Trigger SW event 2                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT3<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3>`                      |    12|   32|Trigger SW event 3                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT4<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4>`                      |    16|   32|Trigger SW event 4                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT5<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5>`                      |    20|   32|Trigger SW event 5                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT6<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6>`                      |    24|   32|Trigger SW event 6                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT7<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7>`                      |    28|   32|Trigger SW event 7                                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT0_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0_WAIT>`            |    64|   32|Trigger SW event 0 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT1_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1_WAIT>`            |    68|   32|Trigger SW event 1 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT2_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2_WAIT>`            |    72|   32|Trigger SW event 2 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT3_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3_WAIT>`            |    76|   32|Trigger SW event 3 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT4_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4_WAIT>`            |    64|   32|Trigger SW event 4 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT5_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5_WAIT>`            |    68|   32|Trigger SW event 5 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT6_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6_WAIT>`            |    72|   32|Trigger SW event 6 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT7_WAIT<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7_WAIT>`            |    76|   32|Trigger SW event 7 and go to sleep                  |
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR>`|   128|   32|Trigger SW event 0, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT1_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1_WAIT_CLEAR>`|   132|   32|Trigger SW event 1, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT2_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2_WAIT_CLEAR>`|   136|   32|Trigger SW event 2, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT3_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3_WAIT_CLEAR>`|   140|   32|Trigger SW event 3, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT4_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4_WAIT_CLEAR>`|   128|   32|Trigger SW event 4, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT5_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5_WAIT_CLEAR>`|   132|   32|Trigger SW event 5, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT6_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6_WAIT_CLEAR>`|   136|   32|Trigger SW event 6, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`EU_CORE_TRIGG_SW_EVENT7_WAIT_CLEAR<cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7_WAIT_CLEAR>`|   140|   32|Trigger SW event 7, go to sleep and clear on wake-up|
    +-----------------------------------------------------------------------------------------------------------+------+-----+----------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0:

EU_CORE_TRIGG_SW_EVENT0
"""""""""""""""""""""""

Trigger SW event 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                                                 Description                                                 |
    +=====+===+========+=====+=============================================================================================================+
    |8:0  |W  |EVT_MASK|0x000|Writing to this register triggers software event with ID 0 for all cores with corresponding bit written as b1|
    +-----+---+--------+-----+-------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1:

EU_CORE_TRIGG_SW_EVENT1
"""""""""""""""""""""""

Trigger SW event 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2:

EU_CORE_TRIGG_SW_EVENT2
"""""""""""""""""""""""

Trigger SW event 2

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3:

EU_CORE_TRIGG_SW_EVENT3
"""""""""""""""""""""""

Trigger SW event 3

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4:

EU_CORE_TRIGG_SW_EVENT4
"""""""""""""""""""""""

Trigger SW event 4

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5:

EU_CORE_TRIGG_SW_EVENT5
"""""""""""""""""""""""

Trigger SW event 5

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6:

EU_CORE_TRIGG_SW_EVENT6
"""""""""""""""""""""""

Trigger SW event 6

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7:

EU_CORE_TRIGG_SW_EVENT7
"""""""""""""""""""""""

Trigger SW event 7

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                             Description                              |
    +=====+===+========+=====+======================================================================+
    |8:0  |W  |EVT_MASK|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0, except for the event ID used|
    +-----+---+--------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0_WAIT:

EU_CORE_TRIGG_SW_EVENT0_WAIT
""""""""""""""""""""""""""""

Trigger SW event 0 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                                                                                                 Description                                                                                                                 |
    +=====+===+=====+=====+=============================================================================================================================================================================================================================================+
    |8:0  |W  |EVENT|0x000|Reading from this register triggers software event with ID 0 for all cores with corresponding bit written as b1 in EU_CORE_SW_EVENTS_MASK register. Current core then goes to sleep with the same behavior as when reading EU_CORE_EVENT_WAIT|
    +-----+---+-----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1_WAIT:

EU_CORE_TRIGG_SW_EVENT1_WAIT
""""""""""""""""""""""""""""

Trigger SW event 1 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2_WAIT:

EU_CORE_TRIGG_SW_EVENT2_WAIT
""""""""""""""""""""""""""""

Trigger SW event 2 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3_WAIT:

EU_CORE_TRIGG_SW_EVENT3_WAIT
""""""""""""""""""""""""""""

Trigger SW event 3 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4_WAIT:

EU_CORE_TRIGG_SW_EVENT4_WAIT
""""""""""""""""""""""""""""

Trigger SW event 4 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5_WAIT:

EU_CORE_TRIGG_SW_EVENT5_WAIT
""""""""""""""""""""""""""""

Trigger SW event 5 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6_WAIT:

EU_CORE_TRIGG_SW_EVENT6_WAIT
""""""""""""""""""""""""""""

Trigger SW event 6 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7_WAIT:

EU_CORE_TRIGG_SW_EVENT7_WAIT
""""""""""""""""""""""""""""

Trigger SW event 7 and go to sleep

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 0, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                                                                                                    Description                                                                                                                    |
    +=====+===+=====+=====+===================================================================================================================================================================================================================================================+
    |8:0  |W  |EVENT|0x000|Reading from this register triggers software event with ID 0 for all cores with corresponding bit written as b1 in EU_CORE_SW_EVENTS_MASK register. Current core then goes to sleep with the same behavior as when reading EU_CORE_EVENT_WAIT_CLEAR|
    +-----+---+-----+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT1_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT1_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 1, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT2_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT2_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 2, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT3_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT3_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 3, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT4_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT4_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 4, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT5_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT5_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 5, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT6_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT6_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 6, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+

.. _cluster_event_unit_sw_events__EU_CORE_TRIGG_SW_EVENT7_WAIT_CLEAR:

EU_CORE_TRIGG_SW_EVENT7_WAIT_CLEAR
""""""""""""""""""""""""""""""""""

Trigger SW event 7, go to sleep and clear on wake-up

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                   Description                                   |
    +=====+===+=====+=====+=================================================================================+
    |8:0  |W  |EVENT|0x000|Same behavior as EU_CORE_TRIGG_SW_EVENT0_WAIT_CLEAR, except for the event ID used|
    +-----+---+-----+-----+---------------------------------------------------------------------------------+
