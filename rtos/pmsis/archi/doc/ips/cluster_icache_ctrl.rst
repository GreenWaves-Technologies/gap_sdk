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

    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |                                      Name                                      |Offset|Width|                Description                |
    +================================================================================+======+=====+===========================================+
    |:ref:`ENABLE<cluster_icache_ctrl__ENABLE>`                                      |0x0   |   32|Cluster instruction cache enablement       |
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`FLUSH<cluster_icache_ctrl__FLUSH>`                                        |0x4   |   32|Cluster instruction cache flush            |
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`L1_FLUSH<cluster_icache_ctrl__L1_FLUSH>`                                  |0x8   |   32|L1 cache flush                             |
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`SEL_FLUSH<cluster_icache_ctrl__SEL_FLUSH>`                                |0xC   |   32|Selective instruction flush                |
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`ENABLE_SPECIAL_CORE_CACHE<cluster_icache_ctrl__ENABLE_SPECIAL_CORE_CACHE>`|0x18  |   32|Core 8 cache sharing configuration to share|
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`ENABLE_L1_L15_PREFETCH<cluster_icache_ctrl__ENABLE_L1_L15_PREFETCH>`      |0x1C  |   32|L1 prefetch enablement                     |
    +--------------------------------------------------------------------------------+------+-----+-------------------------------------------+

.. _cluster_icache_ctrl__ENABLE:

ENABLE
""""""

Cluster instruction cache enablement

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+----------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                            Description                                             |
    +=====+===+======+=====+====================================================================================================+
    |8:0  |W  |EN_PRI|0x0  |Enable L1 private instruction cache (bit *i* configures L1 of core *i*\ ): b0: disabled; b1: enabled|
    +-----+---+------+-----+----------------------------------------------------------------------------------------------------+
    |10:9 |W  |EN_SH |0x0  |Enable banks of shared L1.5 cache (bit 9+\ *i* configures bank *i*\ ): b0: disabled; b1: enabled    |
    +-----+---+------+-----+----------------------------------------------------------------------------------------------------+

.. _cluster_icache_ctrl__FLUSH:

FLUSH
"""""

Cluster instruction cache flush

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+--------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                               Description                                |
    +=====+===+======+=====+==========================================================================+
    |8:0  |W  |FL_PRI|0x0  |Write 1 to bit *i* to fully flush L1 private instruction cache of core *i*|
    +-----+---+------+-----+--------------------------------------------------------------------------+
    |10:9 |W  |FL_SH |0x0  |Write 1 to bit 9+\ *i* to fully flush bank *i* of the shared L1.5 cache   |
    +-----+---+------+-----+--------------------------------------------------------------------------+

.. _cluster_icache_ctrl__L1_FLUSH:

L1_FLUSH
""""""""

L1 cache flush

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                                Description                                |
    +=====+===+=====+=====+===========================================================================+
    |8:0  |W  |L1_FL|0x0  |Write 1 to bit *i* to fully flush L1 private instruction cache for core *i*|
    +-----+---+-----+-----+---------------------------------------------------------------------------+

.. _cluster_icache_ctrl__SEL_FLUSH:

SEL_FLUSH
"""""""""

Selective instruction flush

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

Core 8 cache sharing configuration to share

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+---------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                            Description                                            |
    +=====+===+======+=====+===================================================================================================+
    |    0|R/W|ENABLE|0x0  |If b0, core 8 shares L1.5 with other cores; if b1, core 8 cache is independent from the other cores|
    +-----+---+------+-----+---------------------------------------------------------------------------------------------------+

.. _cluster_icache_ctrl__ENABLE_L1_L15_PREFETCH:

ENABLE_L1_L15_PREFETCH
""""""""""""""""""""""

L1 prefetch enablement

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                                 Description                                                 |
    +=====+===+======+=====+=============================================================================================================+
    |8:0  |R/W|ENABLE|0x0  |Enable the prefetchers for L1 private caches (bit *i* configures L1 of core *i*\ ): b0: disabled; b1: enabled|
    +-----+---+------+-----+-------------------------------------------------------------------------------------------------------------+
