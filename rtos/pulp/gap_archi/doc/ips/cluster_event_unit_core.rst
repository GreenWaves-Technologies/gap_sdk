.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_core.md

Register map for core events
^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |                                         Name                                         |Offset|Width|                             Description                              |
    +======================================================================================+======+=====+======================================================================+
    |:ref:`EU_CORE_MASK<cluster_event_unit_core__EU_CORE_MASK>`                            |     0|   32|Core event mask register                                              |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_MASK_AND<cluster_event_unit_core__EU_CORE_MASK_AND>`                    |     4|   32|Clear bits of core event mask register                                |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_MASK_OR<cluster_event_unit_core__EU_CORE_MASK_OR>`                      |     8|   32|Set bits of core event mask register                                  |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_MASK_IRQ<cluster_event_unit_core__EU_CORE_MASK_IRQ>`                    |    12|   32|Core intrerrupt mask register                                         |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_MASK_IRQ_AND<cluster_event_unit_core__EU_CORE_MASK_IRQ_AND>`            |    16|   32|Clear bits of core intrerrupt mask register                           |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_MASK_IRQ_OR<cluster_event_unit_core__EU_CORE_MASK_IRQ_OR>`              |    20|   32|Set bits of core intrerrupt mask register                             |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_STATUS<cluster_event_unit_core__EU_CORE_STATUS>`                        |    24|   32|Core status register                                                  |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_BUFFER<cluster_event_unit_core__EU_CORE_BUFFER>`                        |    28|   32|Event status register                                                 |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_BUFFER_MASKED<cluster_event_unit_core__EU_CORE_BUFFER_MASKED>`          |    32|   32|Event status register with event mask applied                         |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_BUFFER_IRQ_MASKED<cluster_event_unit_core__EU_CORE_BUFFER_IRQ_MASKED>`  |    36|   32|Event status register with interrupt mask applied                     |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_BUFFER_CLEAR<cluster_event_unit_core__EU_CORE_BUFFER_CLEAR>`            |    40|   32|Clear bits of event status register                                   |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_SW_EVENTS_MASK<cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK>`        |    44|   32|Target core mask for SW wait events                                   |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_SW_EVENTS_MASK_AND<cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK_AND>`|    48|   32|Clear bits of target core mask for SW wait events                     |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_SW_EVENTS_MASK_OR<cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK_OR>`  |    52|   32|Set bits of target core mask for SW wait events                       |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_EVENT_WAIT<cluster_event_unit_core__EU_CORE_EVENT_WAIT>`                |    56|   32|Wait for event (only accessible from CL_EVENT_UNIT_DEMUX)             |
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`EU_CORE_EVENT_WAIT_CLEAR<cluster_event_unit_core__EU_CORE_EVENT_WAIT_CLEAR>`    |    60|   32|Wait for event and clear it (only accessible from CL_EVENT_UNIT_DEMUX)|
    +--------------------------------------------------------------------------------------+------+-----+----------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK:

EU_CORE_MASK
""""""""""""

Core event mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+----------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                                            Description                                             |
    +=====+===+==========+==========+====================================================================================================+
    |31:0 |R/W|EVENT_MASK|0x00000000|Mask bits for events: set a bit to b1 to enable corresponding event as a wake-up source for the core|
    +-----+---+----------+----------+----------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK_AND:

EU_CORE_MASK_AND
""""""""""""""""

Clear bits of core event mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+--------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                               Description                                |
    +=====+===+========+==========+==========================================================================+
    |31:0 |W  |MASK_CLR|0x00000000|Write b1 to 1 or more bits to clear the corresponding bits of EU_CORE_MASK|
    +-----+---+--------+----------+--------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK_OR:

EU_CORE_MASK_OR
"""""""""""""""

Set bits of core event mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                              Description                               |
    +=====+===+========+==========+========================================================================+
    |31:0 |W  |MASK_SET|0x00000000|Write b1 to 1 or more bits to set the corresponding bits of EU_CORE_MASK|
    +-----+---+--------+----------+------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK_IRQ:

EU_CORE_MASK_IRQ
""""""""""""""""

Core intrerrupt mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+-------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                                              Description                                              |
    +=====+===+==========+==========+=======================================================================================================+
    |31:0 |R/W|EVENT_MASK|0x00000000|Mask bits for events: set a bit to b1 to enable corresponding event as an interrupt source for the core|
    +-----+---+----------+----------+-------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK_IRQ_AND:

EU_CORE_MASK_IRQ_AND
""""""""""""""""""""

Clear bits of core intrerrupt mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                                 Description                                  |
    +=====+===+========+==========+==============================================================================+
    |31:0 |W  |MASK_CLR|0x00000000|Write b1 to 1 or more bits to clear the corresponding bits of EU_CORE_MASK_IRQ|
    +-----+---+--------+----------+------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_MASK_IRQ_OR:

EU_CORE_MASK_IRQ_OR
"""""""""""""""""""

Set bits of core intrerrupt mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+----------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                                Description                                 |
    +=====+===+========+==========+============================================================================+
    |31:0 |W  |MASK_SET|0x00000000|Write b1 to 1 or more bits to set the corresponding bits of EU_CORE_MASK_IRQ|
    +-----+---+--------+----------+----------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_STATUS:

EU_CORE_STATUS
""""""""""""""

Core status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+----------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                          Description                           |
    +=====+===+==========+=====+================================================================+
    |    0|R  |CLK_STATUS|0x0  |Status of core clock: b0: clock is stopped; b1: clock is running|
    +-----+---+----------+-----+----------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_BUFFER:

EU_CORE_BUFFER
""""""""""""""

Event status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+----------+----------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |  Reset   |                                                        Description                                                         |
    +=====+===+======+==========+============================================================================================================================+
    |31:0 |R  |EVENTS|0x00000000|Status bits for events: a bit reads as b1 when the corresponding event has occurred at least once since this bit was cleared|
    +-----+---+------+----------+----------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_BUFFER_MASKED:

EU_CORE_BUFFER_MASKED
"""""""""""""""""""""

Event status register with event mask applied

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |R  |MASKED_EVT|0x00000000|Holds the value of EU_CORE_BUFFER masked by EU_CORE_MASK value|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_BUFFER_IRQ_MASKED:

EU_CORE_BUFFER_IRQ_MASKED
"""""""""""""""""""""""""

Event status register with interrupt mask applied

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+------------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                           Description                            |
    +=====+===+==========+==========+==================================================================+
    |31:0 |R  |MASKED_EVT|0x00000000|Holds the value of EU_CORE_BUFFER masked by EU_CORE_MASK_IRQ value|
    +-----+---+----------+----------+------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_BUFFER_CLEAR:

EU_CORE_BUFFER_CLEAR
""""""""""""""""""""

Clear bits of event status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+----------+------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |  Reset   |                                                               Description                                                                |
    +=====+===+=======+==========+==========================================================================================================================================+
    |31:0 |W  |EVT_CLR|0x00000000|Write b1 to one or more bits to clear the corresponding bits in EU_CORE_BUFFER. Note that an event occuring at the same time may be missed|
    +-----+---+-------+----------+------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK:

EU_CORE_SW_EVENTS_MASK
""""""""""""""""""""""

Target core mask for SW wait events

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset |                                                                                                                   Description                                                                                                                    |
    +=====+===+=============+======+==================================================================================================================================================================================================================================================+
    |7:0  |R/W|CL_INTERN_EVT|0x0000|Mask bits used when a SW event is triggered from EU_CORE_TRIGG_SW_EVENT_WAIT or EU_CORE_TRIGG_SW_EVENT_WAIT_CLEAR. Note that for no-wait SW event triggering, the used mask is set independently when writting the EU_CORE_TRIGG_SW_EVENT register|
    +-----+---+-------------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK_AND:

EU_CORE_SW_EVENTS_MASK_AND
""""""""""""""""""""""""""

Clear bits of target core mask for SW wait events

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                                    Description                                     |
    +=====+===+========+==========+====================================================================================+
    |31:0 |W  |MASK_CLR|0x00000000|Write b1 to 1 or more bits to clear the corresponding bits of EU_CORE_SW_EVENTS_MASK|
    +-----+---+--------+----------+------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_SW_EVENTS_MASK_OR:

EU_CORE_SW_EVENTS_MASK_OR
"""""""""""""""""""""""""

Set bits of target core mask for SW wait events

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+----------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                                   Description                                    |
    +=====+===+========+==========+==================================================================================+
    |31:0 |W  |MASK_SET|0x00000000|Write b1 to 1 or more bits to set the corresponding bits of EU_CORE_SW_EVENTS_MASK|
    +-----+---+--------+----------+----------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_EVENT_WAIT:

EU_CORE_EVENT_WAIT
""""""""""""""""""

Wait for event (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |  Reset   |                                                                                        Description                                                                                         |
    +=====+===+=====+==========+============================================================================================================================================================================================+
    |31:0 |R  |EVENT|0x00000000|Reading this register stops the clock of the core until at least one event with the corresponding mask bit set to 1 occurs. The returned value is identical to that of EU_CORE_BUFFER_MASKED|
    +-----+---+-----+----------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_core__EU_CORE_EVENT_WAIT_CLEAR:

EU_CORE_EVENT_WAIT_CLEAR
""""""""""""""""""""""""

Wait for event and clear it (only accessible from CL_EVENT_UNIT_DEMUX)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |  Reset   |                                                                                  Description                                                                                  |
    +=====+===+=====+==========+===============================================================================================================================================================================+
    |31:0 |R  |EVENT|0x00000000|Reading this register has the same behavior as reading EU_CORE_EVENT_WAIT, except that bits of EU_CORE_BUFFER that are set to 1 in EU_CORE_MASK are cleared to 0 after the read|
    +-----+---+-----+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
