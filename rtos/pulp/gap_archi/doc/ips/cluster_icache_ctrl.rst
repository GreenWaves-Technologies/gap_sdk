Input file: fe/ips/hier-icache/DOC/CLUSTER_ICACHE_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |                                     Name                                      |Offset|Width|                                                  Description                                                  |
    +===============================================================================+======+=====+===============================================================================================================+
    |:ref:`ENABLE<cluster_icache_ctrl_ENABLE>`                                      |     0|   32|Cluster instruction cache unit enable configuration register.                                                  |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`FLUSH<cluster_icache_ctrl_FLUSH>`                                        |     4|   32|Cluster instruction cache unit flush command register.                                                         |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`L0_FLUSH<cluster_icache_ctrl_L0_FLUSH>`                                  |     8|   32|Cluster level 0 instruction cache unit flush command register.                                                 |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`SEL_FLUSH<cluster_icache_ctrl_SEL_FLUSH>`                                |    12|   32|Cluster instruction cache unit selective flush command register.                                               |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_SPECIAL_CORE_CACHE<cluster_icache_ctrl_ENABLE_SPECIAL_CORE_CACHE>`|    24|   32|When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.|
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_L1_L15_PREFETCH<cluster_icache_ctrl_ENABLE_L1_L15_PREFETCH>`      |    28|   32|For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch                           |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+

.. _cluster_icache_ctrl_ENABLE:

ENABLE
""""""

Cluster instruction cache unit enable configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_icache_ctrl_FLUSH:

FLUSH
"""""

Cluster instruction cache unit flush command register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_icache_ctrl_L0_FLUSH:

L0_FLUSH
""""""""

Cluster level 0 instruction cache unit flush command register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_icache_ctrl_SEL_FLUSH:

SEL_FLUSH
"""""""""

Cluster instruction cache unit selective flush command register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_icache_ctrl_ENABLE_SPECIAL_CORE_CACHE:

ENABLE_SPECIAL_CORE_CACHE
"""""""""""""""""""""""""

When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_icache_ctrl_ENABLE_L1_L15_PREFETCH:

ENABLE_L1_L15_PREFETCH
""""""""""""""""""""""

For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
