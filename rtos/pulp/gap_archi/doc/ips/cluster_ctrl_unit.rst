.. 
   Input file: docs/IP_REFERENCES/CLUSTER_CRTL_UNIT_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |                                  Name                                   |Offset|Width|                             Description                             |
    +=========================================================================+======+=====+=====================================================================+
    |:ref:`EOC<cluster_ctrl_unit_EOC>`                                        |     0|   32|End Of Computation status register.                                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`FETCH_EN<cluster_ctrl_unit_FETCH_EN>`                              |     8|   32|Cluster cores fetch enable configuration register.                   |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`CLOCK_GATE<cluster_ctrl_unit_CLOCK_GATE>`                          |    32|   32|Cluster clock gate configuration register.                           |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_RESUME<cluster_ctrl_unit_DBG_RESUME>`                          |    40|   32|Cluster cores debug resume register.                                 |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_HALT_STATUS<cluster_ctrl_unit_DBG_HALT_STATUS>`                |    40|   32|Cluster cores debug halt status register.                            |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`DBG_HALT_MASK<cluster_ctrl_unit_DBG_HALT_MASK>`                    |    56|   32|Cluster cores debug halt mask configuration register.                |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR0<cluster_ctrl_unit_BOOT_ADDR0>`                          |    64|   32|Cluster core 0 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR1<cluster_ctrl_unit_BOOT_ADDR1>`                          |    68|   32|Cluster core 1 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR2<cluster_ctrl_unit_BOOT_ADDR2>`                          |    72|   32|Cluster core 2 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR3<cluster_ctrl_unit_BOOT_ADDR3>`                          |    76|   32|Cluster core 3 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR4<cluster_ctrl_unit_BOOT_ADDR4>`                          |    80|   32|Cluster core 4 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR5<cluster_ctrl_unit_BOOT_ADDR5>`                          |    84|   32|Cluster core 5 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR6<cluster_ctrl_unit_BOOT_ADDR6>`                          |    88|   32|Cluster core 6 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`BOOT_ADDR7<cluster_ctrl_unit_BOOT_ADDR7>`                          |    92|   32|Cluster core 7 boot address configuration register.                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH0<cluster_ctrl_unit_TCDM_ARB_POLICY_CH0>`        |   128|   32|TCDM arbitration policy ch0 for cluster cores configuration register.|
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH1<cluster_ctrl_unit_TCDM_ARB_POLICY_CH1>`        |   136|   32|TCDM arbitration policy ch1 for DMA/HWCE configuration register.     |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH0_REP<cluster_ctrl_unit_TCDM_ARB_POLICY_CH0_REP>`|   192|   32|Read only duplicate of TCDM_ARB_POLICY_CH0 register                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+
    |:ref:`TCDM_ARB_POLICY_CH1_REP<cluster_ctrl_unit_TCDM_ARB_POLICY_CH1_REP>`|   200|   32|Read only duplicate of TCDM_ARB_POLICY_CH1 register                  |
    +-------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------+

.. _cluster_ctrl_unit_EOC:

EOC
"""

End Of Computation status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_FETCH_EN:

FETCH_EN
""""""""

Cluster cores fetch enable configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_CLOCK_GATE:

CLOCK_GATE
""""""""""

Cluster clock gate configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_DBG_RESUME:

DBG_RESUME
""""""""""

Cluster cores debug resume register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_DBG_HALT_STATUS:

DBG_HALT_STATUS
"""""""""""""""

Cluster cores debug halt status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_DBG_HALT_MASK:

DBG_HALT_MASK
"""""""""""""

Cluster cores debug halt mask configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR0:

BOOT_ADDR0
""""""""""

Cluster core 0 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR1:

BOOT_ADDR1
""""""""""

Cluster core 1 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR2:

BOOT_ADDR2
""""""""""

Cluster core 2 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR3:

BOOT_ADDR3
""""""""""

Cluster core 3 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR4:

BOOT_ADDR4
""""""""""

Cluster core 4 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR5:

BOOT_ADDR5
""""""""""

Cluster core 5 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR6:

BOOT_ADDR6
""""""""""

Cluster core 6 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_BOOT_ADDR7:

BOOT_ADDR7
""""""""""

Cluster core 7 boot address configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH0:

TCDM_ARB_POLICY_CH0
"""""""""""""""""""

TCDM arbitration policy ch0 for cluster cores configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH1:

TCDM_ARB_POLICY_CH1
"""""""""""""""""""

TCDM arbitration policy ch1 for DMA/HWCE configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH0_REP:

TCDM_ARB_POLICY_CH0_REP
"""""""""""""""""""""""

Read only duplicate of TCDM_ARB_POLICY_CH0 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _cluster_ctrl_unit_TCDM_ARB_POLICY_CH1_REP:

TCDM_ARB_POLICY_CH1_REP
"""""""""""""""""""""""

Read only duplicate of TCDM_ARB_POLICY_CH1 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
