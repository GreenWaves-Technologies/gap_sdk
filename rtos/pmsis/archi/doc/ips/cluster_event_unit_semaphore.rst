.. 
   Input file: docs/IP_REFERENCES/CLUSTER_EVENT_UNIT_semaphore.md

Register map for semaphores
^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-------------------------------------------------------+------+-----+----------------------------------------------------------+
    |                         Name                          |Offset|Width|                       Description                        |
    +=======================================================+======+=====+==========================================================+
    |:ref:`VALUE<cluster_event_unit_semaphore__VALUE>`      |0x0   |   32|Set or get the value of the semaphore                     |
    +-------------------------------------------------------+------+-----+----------------------------------------------------------+
    |:ref:`COUNTER<cluster_event_unit_semaphore__COUNTER>`  |0x4   |   32|Atomically increase or decrease the value of the semaphore|
    +-------------------------------------------------------+------+-----+----------------------------------------------------------+
    |:ref:`COUNTER2<cluster_event_unit_semaphore__COUNTER2>`|0x8   |   32|Read and increase the value of the semaphore              |
    +-------------------------------------------------------+------+-----+----------------------------------------------------------+

.. _cluster_event_unit_semaphore__VALUE:

VALUE
"""""

Set or get the value of the semaphore

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+----------------------------------+
    |Bit #|R/W|Name |Reset|           Description            |
    +=====+===+=====+=====+==================================+
    |11:0 |R/W|COUNT|0x000|Current value of semaphore counter|
    +-----+---+-----+-----+----------------------------------+

.. _cluster_event_unit_semaphore__COUNTER:

COUNTER
"""""""

Atomically increase or decrease the value of the semaphore

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                                                                         Description                                                                         |
    +=====+===+=======+=====+=============================================================================================================================================================+
    |11:0 |R/W|INC_DEC|0x000|Writing to this register increases the value of the semaphore by the value written; reading this register returns current value and decrements it, atomically|
    +-----+---+-------+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _cluster_event_unit_semaphore__COUNTER2:

COUNTER2
""""""""

Read and increase the value of the semaphore

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                                       Description                                        |
    +=====+===+=========+=====+==========================================================================================+
    |11:0 |R  |COUNT_INC|0x000|Reading this register returns current value of the semaphore and increments it, atomically|
    +-----+---+---------+-----+------------------------------------------------------------------------------------------+
