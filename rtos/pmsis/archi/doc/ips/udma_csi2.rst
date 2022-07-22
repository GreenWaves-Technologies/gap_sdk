.. 
   Input file: fe/ips/lnt_csi2_rx/README.md

Register map for CSI-2 UDMA interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |                    Name                    |Offset|Width|                             Description                              |
    +============================================+======+=====+======================================================================+
    |:ref:`RX_CH0_DEST<udma_csi2__RX_CH0_DEST>`  |0x0   |   32|Stream ID for the uDMA channel 0                                      |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`RX_CH1_DEST<udma_csi2__RX_CH1_DEST>`  |0x4   |   32|Stream ID for the uDMA channel 1                                      |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`RX_ENABLE<udma_csi2__RX_ENABLE>`      |0x20  |   32|Enable receiving                                                      |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`CLK_SETUP<udma_csi2__CLK_SETUP>`      |0x24  |   32|Configure clock divider                                               |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE_ENABLE<udma_csi2__SLICE_ENABLE>`|0x28  |   32|Enable slice mode                                                     |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE_ROWLEN<udma_csi2__SLICE_ROWLEN>`|0x2C  |   32|Configure image row length when in slice mode                         |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE0_ULXY<udma_csi2__SLICE0_ULXY>`  |0x30  |   32|Configure upper left pixel position when in slice mode for channel 0  |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE0_BRXY<udma_csi2__SLICE0_BRXY>`  |0x34  |   32|Configure bottom right pixel position when in slice mode for channel 0|
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE1_ULXY<udma_csi2__SLICE1_ULXY>`  |0x38  |   32|Configure upper left pixel position when in slice mode for channel 1  |
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+
    |:ref:`SLICE1_BRXY<udma_csi2__SLICE1_BRXY>`  |0x3C  |   32|Configure bottom right pixel position when in slice mode for channel 1|
    +--------------------------------------------+------+-----+----------------------------------------------------------------------+

.. _udma_csi2__RX_CH0_DEST:

RX_CH0_DEST
"""""""""""

Stream ID for the uDMA channel 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                              Description                               |
    +=====+===+====+=====+========================================================================+
    |7:0  |R/W|DEST|0xFF |Stream ID for the CMD uDMA channel 0. Default is 0xFF (channel disabled)|
    +-----+---+----+-----+------------------------------------------------------------------------+

.. _udma_csi2__RX_CH1_DEST:

RX_CH1_DEST
"""""""""""

Stream ID for the uDMA channel 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                              Description                               |
    +=====+===+====+=====+========================================================================+
    |7:0  |R/W|DEST|0xFF |Stream ID for the CMD uDMA channel 1. Default is 0xFF (channel disabled)|
    +-----+---+----+-----+------------------------------------------------------------------------+

.. _udma_csi2__RX_ENABLE:

RX_ENABLE
"""""""""

Enable receiving

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                         Description                          |
    +=====+===+====+=====+==============================================================+
    |    0|R/W|EN0 |0x0  |Enable channel 0 pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----+--------------------------------------------------------------+
    |    1|R/W|EN1 |0x0  |Enable channel 1 pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----+--------------------------------------------------------------+

.. _udma_csi2__CLK_SETUP:

CLK_SETUP
"""""""""

Configure clock divider

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+---------------------------+
    |Bit #|R/W|    Name     |Reset|        Description        |
    +=====+===+=============+=====+===========================+
    |7:0  |R/W|CCI_CLK_DIV  |0x0  |Clock diver for CCI clock  |
    +-----+---+-------------+-----+---------------------------+
    |15:8 |R/W|PIXEL_CLK_DIV|0x0  |Clock diver for pixel clock|
    +-----+---+-------------+-----+---------------------------+
    |23:16|R/W|APB_CLK_DIV  |0x0  |Clock diver for APB clock  |
    +-----+---+-------------+-----+---------------------------+

.. _udma_csi2__SLICE_ENABLE:

SLICE_ENABLE
""""""""""""

Enable slice mode

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-----------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                 |
    +=====+===+====+=====+=============================================================================+
    |    0|R/W|EN0 |0x0  |Enable channel 0 slice mode for pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----+-----------------------------------------------------------------------------+
    |    1|R/W|EN1 |0x0  |Enable channel 1 slice mode for pixel stream receiving: 0: Disable, 1: Enable|
    +-----+---+----+-----+-----------------------------------------------------------------------------+

.. _udma_csi2__SLICE_ROWLEN:

SLICE_ROWLEN
""""""""""""

Configure image row length when in slice mode

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-----------------------------------------+
    |Bit #|R/W| Name  |Reset|               Description               |
    +=====+===+=======+=====+=========================================+
    |15:0 |R/W|ROWLEN0|0x0  |Slice mode image row length for channel 0|
    +-----+---+-------+-----+-----------------------------------------+
    |31:16|R/W|ROWLEN1|0x0  |Slice mode image row length for channel 1|
    +-----+---+-------+-----+-----------------------------------------+

.. _udma_csi2__SLICE0_ULXY:

SLICE0_ULXY
"""""""""""

Configure upper left pixel position when in slice mode for channel 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-----------------------------------------+
    |Bit #|R/W|Name|Reset|               Description               |
    +=====+===+====+=====+=========================================+
    |15:0 |R/W|ULY |0x0  |Upper left pixel position Y for channel 0|
    +-----+---+----+-----+-----------------------------------------+
    |31:16|R/W|ULX |0x0  |Upper left pixel position X for channel 0|
    +-----+---+----+-----+-----------------------------------------+

.. _udma_csi2__SLICE0_BRXY:

SLICE0_BRXY
"""""""""""

Configure bottom right pixel position when in slice mode for channel 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------------------------------------+
    |Bit #|R/W|Name|Reset|                Description                |
    +=====+===+====+=====+===========================================+
    |15:0 |R/W|BRY |0x0  |Bottom right pixel position Y for channel 0|
    +-----+---+----+-----+-------------------------------------------+
    |31:16|R/W|BRX |0x0  |Bottom right pixel position X for channel 0|
    +-----+---+----+-----+-------------------------------------------+

.. _udma_csi2__SLICE1_ULXY:

SLICE1_ULXY
"""""""""""

Configure upper left pixel position when in slice mode for channel 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-----------------------------------------+
    |Bit #|R/W|Name|Reset|               Description               |
    +=====+===+====+=====+=========================================+
    |15:0 |R/W|ULY |0x0  |Upper left pixel position Y for channel 1|
    +-----+---+----+-----+-----------------------------------------+
    |31:16|R/W|ULX |0x0  |Upper left pixel position X for channel 1|
    +-----+---+----+-----+-----------------------------------------+

.. _udma_csi2__SLICE1_BRXY:

SLICE1_BRXY
"""""""""""

Configure bottom right pixel position when in slice mode for channel 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------------------------------------+
    |Bit #|R/W|Name|Reset|                Description                |
    +=====+===+====+=====+===========================================+
    |15:0 |R/W|BRY |0x0  |Bottom right pixel position Y for channel 1|
    +-----+---+----+-----+-------------------------------------------+
    |31:16|R/W|BRX |0x0  |Bottom right pixel position X for channel 1|
    +-----+---+----+-----+-------------------------------------------+
