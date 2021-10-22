.. 
   Input file: fe/ips/lnt_csi2_rx/README.md

Register map for CSI2 UDMA interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |                   Name                    |Offset|Width|                             Description                              |
    +===========================================+======+=====+======================================================================+
    |:ref:`RX_CH0_DEST<udma_csi2_RX_CH0_DEST>`  |     0|   32|Stream ID for the uDMA channel 0                                      |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`RX_CH1_DEST<udma_csi2_RX_CH1_DEST>`  |     4|   32|Stream ID for the uDMA channel 1                                      |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`RX_ENABLE<udma_csi2_RX_ENABLE>`      |    32|   32|Enable receiving                                                      |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`CLK_SETUP<udma_csi2_CLK_SETUP>`      |    36|   32|Configure clock divider                                               |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE_ENABLE<udma_csi2_SLICE_ENABLE>`|    40|   32|Enable slice mode                                                     |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE_ROWLEN<udma_csi2_SLICE_ROWLEN>`|    44|   32|Configure image row length when in slice mode                         |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE0_ULXY<udma_csi2_SLICE0_ULXY>`  |    48|   32|Configure upper left pixel position when in slice mode for channel 0  |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE0_BRXY<udma_csi2_SLICE0_BRXY>`  |    52|   32|Configure bottom right pixel position when in slice mode for channel 0|
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE1_ULXY<udma_csi2_SLICE1_ULXY>`  |    56|   32|Configure upper left pixel position when in slice mode for channel 1  |
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE1_BRXY<udma_csi2_SLICE1_BRXY>`  |    60|   32|Configure bottom right pixel position when in slice mode for channel 1|
    +-------------------------------------------+------+-----+----------------------------------------------------------------------+

.. _udma_csi2_RX_CH0_DEST:

RX_CH0_DEST
"""""""""""

Stream ID for the uDMA channel 0

.. table:: 

    +-----+---+----+------------------------------------------------------------------------+
    |Bit #|R/W|Name|                              Description                               |
    +=====+===+====+========================================================================+
    |7:0  |R/W|DEST|Stream ID for the CMD uDMA channel 0. Default is 0xFF (channel disabled)|
    +-----+---+----+------------------------------------------------------------------------+

.. _udma_csi2_RX_CH1_DEST:

RX_CH1_DEST
"""""""""""

Stream ID for the uDMA channel 1

.. table:: 

    +-----+---+----+------------------------------------------------------------------------+
    |Bit #|R/W|Name|                              Description                               |
    +=====+===+====+========================================================================+
    |7:0  |R/W|DEST|Stream ID for the CMD uDMA channel 1. Default is 0xFF (channel disabled)|
    +-----+---+----+------------------------------------------------------------------------+

.. _udma_csi2_RX_ENABLE:

RX_ENABLE
"""""""""

Enable receiving

.. table:: 

    +-----+---+----+--------------------------------------------------------------+
    |Bit #|R/W|Name|                         Description                          |
    +=====+===+====+==============================================================+
    |    0|R/W|EN0 |Enable channel 0 pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+--------------------------------------------------------------+
    |    1|R/W|EN1 |Enable channel 1 pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+--------------------------------------------------------------+

.. _udma_csi2_CLK_SETUP:

CLK_SETUP
"""""""""

Configure clock divider

.. table:: 

    +-----+---+-------------+---------------------------+
    |Bit #|R/W|    Name     |        Description        |
    +=====+===+=============+===========================+
    |7:0  |R/W|CCI_CLK_DIV  |Clock diver for CCI clock  |
    +-----+---+-------------+---------------------------+
    |15:8 |R/W|PIXEL_CLK_DIV|Clock diver for pixel clock|
    +-----+---+-------------+---------------------------+
    |23:16|R/W|APB_CLK_DIV  |Clock diver for APB clock  |
    +-----+---+-------------+---------------------------+

.. _udma_csi2_SLICE_ENABLE:

SLICE_ENABLE
""""""""""""

Enable slice mode

.. table:: 

    +-----+---+----+-----------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                 Description                                 |
    +=====+===+====+=============================================================================+
    |    0|R/W|EN0 |Enable channel 0 slice mode for pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----------------------------------------------------------------------------+
    |    1|R/W|EN1 |Enable channel 1 slice mode for pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----------------------------------------------------------------------------+

.. _udma_csi2_SLICE_ROWLEN:

SLICE_ROWLEN
""""""""""""

Configure image row length when in slice mode

.. table:: 

    +-----+---+-------+-----------------------------------------+
    |Bit #|R/W| Name  |               Description               |
    +=====+===+=======+=========================================+
    |15:0 |R/W|ROWLEN0|Slice mode image row length for channel 0|
    +-----+---+-------+-----------------------------------------+
    |31:16|R/W|ROWLEN1|Slice mode image row length for channel 1|
    +-----+---+-------+-----------------------------------------+

.. _udma_csi2_SLICE0_ULXY:

SLICE0_ULXY
"""""""""""

Configure upper left pixel position when in slice mode for channel 0

.. table:: 

    +-----+---+----+-----------------------------------------+
    |Bit #|R/W|Name|               Description               |
    +=====+===+====+=========================================+
    |15:0 |R/W|ULY |Upper left pixel position Y for channel 0|
    +-----+---+----+-----------------------------------------+
    |31:16|R/W|ULX |Upper left pixel position X for channel 0|
    +-----+---+----+-----------------------------------------+

.. _udma_csi2_SLICE0_BRXY:

SLICE0_BRXY
"""""""""""

Configure bottom right pixel position when in slice mode for channel 0

.. table:: 

    +-----+---+----+-------------------------------------------+
    |Bit #|R/W|Name|                Description                |
    +=====+===+====+===========================================+
    |15:0 |R/W|BRY |Bottom right pixel position Y for channel 0|
    +-----+---+----+-------------------------------------------+
    |31:16|R/W|BRX |Bottom right pixel position X for channel 0|
    +-----+---+----+-------------------------------------------+

.. _udma_csi2_SLICE1_ULXY:

SLICE1_ULXY
"""""""""""

Configure upper left pixel position when in slice mode for channel 1

.. table:: 

    +-----+---+----+-----------------------------------------+
    |Bit #|R/W|Name|               Description               |
    +=====+===+====+=========================================+
    |15:0 |R/W|ULY |Upper left pixel position Y for channel 1|
    +-----+---+----+-----------------------------------------+
    |31:16|R/W|ULX |Upper left pixel position X for channel 1|
    +-----+---+----+-----------------------------------------+

.. _udma_csi2_SLICE1_BRXY:

SLICE1_BRXY
"""""""""""

Configure bottom right pixel position when in slice mode for channel 1

.. table:: 

    +-----+---+----+-------------------------------------------+
    |Bit #|R/W|Name|                Description                |
    +=====+===+====+===========================================+
    |15:0 |R/W|BRY |Bottom right pixel position Y for channel 1|
    +-----+---+----+-------------------------------------------+
    |31:16|R/W|BRX |Bottom right pixel position X for channel 1|
    +-----+---+----+-------------------------------------------+
