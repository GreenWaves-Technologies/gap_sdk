.. 
   Input file: fe/ips/udma/udma_filter/docs/FILTER_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------------+------+-----+---------------------------------------------+
    |                       Name                        |Offset|Width|                 Description                 |
    +===================================================+======+=====+=============================================+
    |:ref:`REG_TX_CH0_ADD<udma_filter_REG_TX_CH0_ADD>`  |     0|   32|FILTER tx channel 0 address register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_CFG<udma_filter_REG_TX_CH0_CFG>`  |     4|   32|FILTER tx channel 0 configuration register   |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN0<udma_filter_REG_TX_CH0_LEN0>`|     8|   32|FILTER tx channel 0 length0 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN1<udma_filter_REG_TX_CH0_LEN1>`|    12|   32|FILTER tx channel 0 length1 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH0_LEN2<udma_filter_REG_TX_CH0_LEN2>`|    16|   32|FILTER tx channel 0 length2 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_ADD<udma_filter_REG_TX_CH1_ADD>`  |    20|   32|FILTER tx channel 1 address register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_CFG<udma_filter_REG_TX_CH1_CFG>`  |    24|   32|FILTER tx channel 1 configuration register   |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN0<udma_filter_REG_TX_CH1_LEN0>`|    28|   32|FILTER tx channel 1 length0 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN1<udma_filter_REG_TX_CH1_LEN1>`|    32|   32|FILTER tx channel 1 length1 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_TX_CH1_LEN2<udma_filter_REG_TX_CH1_LEN2>`|    36|   32|FILTER tx channel 1 length2 register         |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_ADD<udma_filter_REG_RX_CH_ADD>`    |    40|   32|FILTER RX channel address register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_CFG<udma_filter_REG_RX_CH_CFG>`    |    44|   32|FILTER RX channel configuration register     |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN0<udma_filter_REG_RX_CH_LEN0>`  |    48|   32|FILTER RX channel length0 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN1<udma_filter_REG_RX_CH_LEN1>`  |    52|   32|FILTER RX channel length1 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_RX_CH_LEN2<udma_filter_REG_RX_CH_LEN2>`  |    56|   32|FILTER RX channel length2 register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_CFG<udma_filter_REG_AU_CFG>`          |    60|   32|FILTER arithmetic unit configuration register|
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_REG0<udma_filter_REG_AU_REG0>`        |    64|   32|FILTER arithmetic unit 0 register            |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_AU_REG1<udma_filter_REG_AU_REG1>`        |    68|   32|FILTER arithmetic unit 1 register            |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_TH<udma_filter_REG_BINCU_TH>`      |    72|   32|FILTER binarization threshold register       |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_CNT<udma_filter_REG_BINCU_CNT>`    |    76|   32|FILTER binarization count register           |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_SETUP<udma_filter_REG_BINCU_SETUP>`|    80|   32|FILTER binarization datasize format register |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_BINCU_VAL<udma_filter_REG_BINCU_VAL>`    |    84|   32|FILTER binarization result count register    |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_FILT<udma_filter_REG_FILT>`              |    88|   32|FILTER control mode register                 |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_FILT_CMD<udma_filter_REG_FILT_CMD>`      |    92|   32|FILTER start register                        |
    +---------------------------------------------------+------+-----+---------------------------------------------+
    |:ref:`REG_STATUS<udma_filter_REG_STATUS>`          |    96|   32|FILTER status register                       |
    +---------------------------------------------------+------+-----+---------------------------------------------+

.. _udma_filter_REG_TX_CH0_ADD:

REG_TX_CH0_ADD
""""""""""""""

FILTER tx channel 0 address register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH0_CFG:

REG_TX_CH0_CFG
""""""""""""""

FILTER tx channel 0 configuration register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH0_LEN0:

REG_TX_CH0_LEN0
"""""""""""""""

FILTER tx channel 0 length0 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH0_LEN1:

REG_TX_CH0_LEN1
"""""""""""""""

FILTER tx channel 0 length1 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH0_LEN2:

REG_TX_CH0_LEN2
"""""""""""""""

FILTER tx channel 0 length2 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH1_ADD:

REG_TX_CH1_ADD
""""""""""""""

FILTER tx channel 1 address register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH1_CFG:

REG_TX_CH1_CFG
""""""""""""""

FILTER tx channel 1 configuration register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH1_LEN0:

REG_TX_CH1_LEN0
"""""""""""""""

FILTER tx channel 1 length0 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH1_LEN1:

REG_TX_CH1_LEN1
"""""""""""""""

FILTER tx channel 1 length1 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_TX_CH1_LEN2:

REG_TX_CH1_LEN2
"""""""""""""""

FILTER tx channel 1 length2 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_RX_CH_ADD:

REG_RX_CH_ADD
"""""""""""""

FILTER RX channel address register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_RX_CH_CFG:

REG_RX_CH_CFG
"""""""""""""

FILTER RX channel configuration register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_RX_CH_LEN0:

REG_RX_CH_LEN0
""""""""""""""

FILTER RX channel length0 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_RX_CH_LEN1:

REG_RX_CH_LEN1
""""""""""""""

FILTER RX channel length1 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_RX_CH_LEN2:

REG_RX_CH_LEN2
""""""""""""""

FILTER RX channel length2 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_AU_CFG:

REG_AU_CFG
""""""""""

FILTER arithmetic unit configuration register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_AU_REG0:

REG_AU_REG0
"""""""""""

FILTER arithmetic unit 0 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_AU_REG1:

REG_AU_REG1
"""""""""""

FILTER arithmetic unit 1 register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_BINCU_TH:

REG_BINCU_TH
""""""""""""

FILTER binarization threshold register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_BINCU_CNT:

REG_BINCU_CNT
"""""""""""""

FILTER binarization count register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_BINCU_SETUP:

REG_BINCU_SETUP
"""""""""""""""

FILTER binarization datasize format register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_BINCU_VAL:

REG_BINCU_VAL
"""""""""""""

FILTER binarization result count register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_FILT:

REG_FILT
""""""""

FILTER control mode register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_FILT_CMD:

REG_FILT_CMD
""""""""""""

FILTER start register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _udma_filter_REG_STATUS:

REG_STATUS
""""""""""

FILTER status register

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
