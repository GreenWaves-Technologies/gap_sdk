.. 
   Input file: fe/ips/hier-icache/DOC/CLUSTER_ICACHE_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |                                      Name                                      |Offset|Width|                                                  Description                                                  |
    +================================================================================+======+=====+===============================================================================================================+
    |:ref:`ENABLE<cluster_icache_ctrl__ENABLE>`                                      |     0|   32|Cluster instruction cache unit enable configuration register.                                                  |
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`FLUSH<cluster_icache_ctrl__FLUSH>`                                        |     4|   32|Cluster instruction cache unit flush command register.                                                         |
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`L0_FLUSH<cluster_icache_ctrl__L0_FLUSH>`                                  |     8|   32|Cluster level 0 instruction cache unit flush command register.                                                 |
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`SEL_FLUSH<cluster_icache_ctrl__SEL_FLUSH>`                                |    12|   32|Cluster instruction cache unit selective flush command register.                                               |
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_SPECIAL_CORE_CACHE<cluster_icache_ctrl__ENABLE_SPECIAL_CORE_CACHE>`|    24|   32|When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.|
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_L1_L15_PREFETCH<cluster_icache_ctrl__ENABLE_L1_L15_PREFETCH>`      |    28|   32|For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch                           |
    +--------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+

.. _cluster_icache_ctrl__ENABLE:

ENABLE
""""""

Cluster instruction cache unit enable configuration register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+---------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                         Description                                         |
    +=====+===+======+=====+=============================================================================================+
    |8:0  |W  |EN_PRI|0x0  |Enable private banks of cluster instruction cache (1 bit per core): b0: disabled; b1: enabled|
    +-----+---+------+-----+---------------------------------------------------------------------------------------------+
    |10:9 |W  |EN_SH |0x0  |Enable shared banks of cluster instruction cache (1 bit per bank): b0: disabled; b1: enabled |
    +-----+---+------+-----+---------------------------------------------------------------------------------------------+

.. _cluster_icache_ctrl__FLUSH:

FLUSH
"""""

Cluster instruction cache unit flush command register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+---------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                Description                                |
    +=====+===+======+=====+===========================================================================+
    |8:0  |W  |FL_PRI|0x0  |Write 1 to bit i to fully flush private instruction cache of cluster core i|
    +-----+---+------+-----+---------------------------------------------------------------------------+
    |10:9 |W  |FL_SH |0x0  |Write 1 to bit i to fully flush shared bank i of cluster instruction cache |
    +-----+---+------+-----+---------------------------------------------------------------------------+

.. _cluster_icache_ctrl__L0_FLUSH:

L0_FLUSH
""""""""

Cluster level 0 instruction cache unit flush command register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-----------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                              Description                              |
    +=====+===+=====+=====+=======================================================================+
    |8:0  |W  |L0_FL|0x0  |Write 1 to bit i to fully flush level 0 of instruction cache for core i|
    +-----+---+-----+-----+-----------------------------------------------------------------------+

.. _cluster_icache_ctrl__SEL_FLUSH:

SEL_FLUSH
"""""""""

Cluster instruction cache unit selective flush command register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------------------------+
    |Bit #|R/W|Name|Reset|              Description               |
    +=====+===+====+=====+========================================+
    |31:0 |W  |ADDR|0x0  |Write an address to selectively flush it|
    +-----+---+----+-----+----------------------------------------+

.. _cluster_icache_ctrl__ENABLE_SPECIAL_CORE_CACHE:

ENABLE_SPECIAL_CORE_CACHE
"""""""""""""""""""""""""

When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+---------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                             Description                             |
    +=====+===+======+=====+=====================================================================+
    |    0|W  |ENABLE|0x0  |Enable or disable core 9 seperated i-cache: b0: disabled; b1: enabled|
    +-----+---+------+-----+---------------------------------------------------------------------+

.. _cluster_icache_ctrl__ENABLE_L1_L15_PREFETCH:

ENABLE_L1_L15_PREFETCH
""""""""""""""""""""""

For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                           Description                                           |
    +=====+===+======+=====+=================================================================================================+
    |8:0  |W  |ENABLE|0x0  |Enable or disable the prefetchers for private i-cache (1 bit per core): b0: disabled; b1: enabled|
    +-----+---+------+-----+-------------------------------------------------------------------------------------------------+
