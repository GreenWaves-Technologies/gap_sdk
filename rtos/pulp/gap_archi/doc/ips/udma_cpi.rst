.. 
   Input file: fe/ips/udma/udma_camera/doc/CAM_CPI_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |                         Name                          |Offset|Width|                Description                 |
    +=======================================================+======+=====+============================================+
    |:ref:`CAM_RX_DEST<udma_cpi__CAM_RX_DEST>`              |     0|   32|Stream ID for the uDMA channel              |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_RX_DATASIZE<udma_cpi__CAM_RX_DATASIZE>`      |     4|   32|Data size                                   |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_CFG_GLOB<udma_cpi__CAM_CFG_GLOB>`            |    32|   32|Global configuration register               |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_CFG_LL<udma_cpi__CAM_CFG_LL>`                |    36|   32|Lower Left corner configuration register    |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_CFG_UR<udma_cpi__CAM_CFG_UR>`                |    40|   32|Upper Right corner configuration register   |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_CFG_SIZE<udma_cpi__CAM_CFG_SIZE>`            |    44|   32|Horizontal Resolution configuration register|
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_CFG_RGB<udma_cpi__CAM_CFG_RGB>`              |    48|   32|RGB coefficients configuration register     |
    +-------------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`CAM_VSYNC_POLARITY<udma_cpi__CAM_VSYNC_POLARITY>`|    52|   32|VSYNC Polarity register                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------+

.. _udma_cpi__CAM_RX_DEST:

CAM_RX_DEST
"""""""""""

Stream ID for the uDMA channel

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                            Description                            |
    +=====+===+=======+=====+===================================================================+
    |6:0  |R/W|RX_DEST|0x7F |Stream ID for the uDMA channel. Default is 0x7F (channel disabled).|
    +-----+---+-------+-----+-------------------------------------------------------------------+

.. _udma_cpi__CAM_RX_DATASIZE:

CAM_RX_DATASIZE
"""""""""""""""

Data size

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+----------------------------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                          Description                           |
    +=====+===+===========+=====+================================================================+
    |1:0  |R/W|RX_DATASIZE|0x0  |Transfer datasize: b00: 8bit; b01: 16bit; b10: 24bit; b11: 32bit|
    +-----+---+-----------+-----+----------------------------------------------------------------+

.. _udma_cpi__CAM_CFG_GLOB:

CAM_CFG_GLOB
""""""""""""

Global configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                                                      Description                                                       |
    +=====+===+=============+=====+========================================================================================================================+
    |    0|R/W|FRAMEDROP_EN |0x0  |Frame dropping: b0: disabled; b1: enabled                                                                               |
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+
    |6:1  |R/W|FRAMEDROP_VAL|0x0  |Set how many frames should be dropped after each received                                                               |
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|FRAMESLICE_EN|0x0  |Input frame slicing: b0: disabled; b1: enabled                                                                          |
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|FORMAT       |0x0  |Input frame format: b000: RGB565; b001: RGB555; b010: RGB444; b011: RGB888; b100: BYPASS_LITEND; b101: BYPASS_BIGEND    |
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+
    |   31|R/W|EN           |0x0  |Enable data rx from camera interface. The enable/disable happens only at the start of a frame. b0: disabled; b1: enabled|
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------+

.. _udma_cpi__CAM_CFG_LL:

CAM_CFG_LL
""""""""""

Lower Left corner configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+------------------------------------------+
    |Bit #|R/W|     Name     |Reset|               Description                |
    +=====+===+==============+=====+==========================================+
    |15:0 |R/W|FRAMESLICE_LLX|0x0  |X coordinate of lower left corner of slice|
    +-----+---+--------------+-----+------------------------------------------+
    |31:16|R/W|FRAMESLICE_LLY|0x0  |Y coordinate of lower left corner of slice|
    +-----+---+--------------+-----+------------------------------------------+

.. _udma_cpi__CAM_CFG_UR:

CAM_CFG_UR
""""""""""

Upper Right corner configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+-------------------------------------------+
    |Bit #|R/W|     Name     |Reset|                Description                |
    +=====+===+==============+=====+===========================================+
    |15:0 |R/W|FRAMESLICE_URX|0x0  |X coordinate of upper right corner of slice|
    +-----+---+--------------+-----+-------------------------------------------+
    |31:16|R/W|FRAMESLICE_URY|0x0  |Y coordinate of upper right corner of slice|
    +-----+---+--------------+-----+-------------------------------------------+

.. _udma_cpi__CAM_CFG_SIZE:

CAM_CFG_SIZE
""""""""""""

Horizontal Resolution configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                               Description                                                |
    +=====+===+======+=====+==========================================================================================================+
    |15:0 |R/W|ROWLEN|0x0  |Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1).|
    +-----+---+------+-----+----------------------------------------------------------------------------------------------------------+

.. _udma_cpi__CAM_CFG_RGB:

CAM_CFG_RGB
"""""""""""

RGB coefficients configuration register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                           Description                                           |
    +=====+===+======+=====+=================================================================================================+
    |2:0  |R/W|FORMAT|0x0  |Order of RGB coefficients: h0: RGB; h1: RBG; h2: GRB; h3: GBR; h4: BRG; h5: BGR; h6: RGB; h7: RGB|
    +-----+---+------+-----+-------------------------------------------------------------------------------------------------+

.. _udma_cpi__CAM_VSYNC_POLARITY:

CAM_VSYNC_POLARITY
""""""""""""""""""

VSYNC Polarity register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+----------------------------------------------------------+
    |Bit #|R/W|     Name     |Reset|                       Description                        |
    +=====+===+==============+=====+==========================================================+
    |    0|R/W|VSYNC_POLARITY|0x0  |Set vsync polarity of CPI: b0: active low; b1: active high|
    +-----+---+--------------+-----+----------------------------------------------------------+
    |    1|R/W|HSYNC_POLARITY|0x0  |Set hsync polarity of CPI: b0: active high; b1: active low|
    +-----+---+--------------+-----+----------------------------------------------------------+
