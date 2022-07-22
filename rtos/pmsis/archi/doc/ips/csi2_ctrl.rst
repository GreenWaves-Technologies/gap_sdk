.. 
   Input file: fe/ips/lnt_csi2_rx/README_CTRL.md

Register map for MIPI CSI-2 Controller
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |                          Name                          |Offset|Width|                   Description                    |
    +========================================================+======+=====+==================================================+
    |:ref:`CONFIG<csi2_ctrl__CONFIG>`                        |0x180 |   32|Configuration of the number of active lanes       |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`ERR_MSB1<csi2_ctrl__ERR_MSB1>`                    |0x184 |   32|Error status MSB1                                 |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`ERR_MSB<csi2_ctrl__ERR_MSB>`                      |0x188 |   32|Error status MSB                                  |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`ERR_LSB<csi2_ctrl__ERR_LSB>`                      |0x18C |   32|Error status LSB                                  |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`HS_RX_TIMEOUT_MSB2<csi2_ctrl__HS_RX_TIMEOUT_MSB2>`|0x190 |   32|Configuration of timeout in high speed mode (MSB2)|
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`HS_RX_TIMEOUT_MSB1<csi2_ctrl__HS_RX_TIMEOUT_MSB1>`|0x194 |   32|Configuration of timeout in high speed mode (MSB1)|
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`HS_RX_TIMEOUT_LSB<csi2_ctrl__HS_RX_TIMEOUT_LSB>`  |0x198 |   32|Configuration of timeout in high speed mode (LSB) |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`VCCFG<csi2_ctrl__VCCFG>`                          |0x19C |   32|Virtual Channel configuration                     |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`POLARITY<csi2_ctrl__POLARITY>`                    |0x1A0 |   32|Vsync and Hsync polarity                          |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_ADDRESS<csi2_ctrl__CCI_ADDRESS>`              |0x1A4 |   32|CCI address                                       |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_WRITE_DATA<csi2_ctrl__CCI_WRITE_DATA>`        |0x1A8 |   32|CCI write data                                    |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_READ_DATA<csi2_ctrl__CCI_READ_DATA>`          |0x1AC |   32|CCI read data                                     |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_READ_WRITE<csi2_ctrl__CCI_READ_WRITE>`        |0x1B0 |   32|CCI R/W                                           |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_STATUS<csi2_ctrl__CCI_STATUS>`                |0x1B4 |   32|CCI status                                        |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CCI_DEV_ADDR<csi2_ctrl__CCI_DEV_ADDR>`            |0x1B8 |   32|CCI device ID                                     |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`ULPS_STATUS<csi2_ctrl__ULPS_STATUS>`              |0x1BC |   32|CSI-2 ULPS status                                 |
    +--------------------------------------------------------+------+-----+--------------------------------------------------+

.. _csi2_ctrl__CONFIG:

CONFIG
""""""

Configuration of the number of active lanes

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                               Description                               |
    +=====+===+==========+=====+=========================================================================+
    |1:0  |R/W|CSI_CONFIG|0x00 |Number of active lanes: b00: single lane, b01: two lanes, b11: four lanes|
    +-----+---+----------+-----+-------------------------------------------------------------------------+

.. _csi2_ctrl__ERR_MSB1:

ERR_MSB1
""""""""

Error status MSB1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+--------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |Reset|                                        Description                                         |
    +=====+===+==============+=====+============================================================================================+
    |    0|R  |CRC_ERROR_VC_3|0x0  |Set to 1 if there is a checksum error on virtual channel 3 (Checksum Error Long packet only)|
    +-----+---+--------------+-----+--------------------------------------------------------------------------------------------+
    |    1|R  |ERR_ESC       |0x0  |Set to 1 if there is an error in escape entry command                                       |
    +-----+---+--------------+-----+--------------------------------------------------------------------------------------------+

.. _csi2_ctrl__ERR_MSB:

ERR_MSB
"""""""

Error status MSB

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name     |Reset|                                            Description                                             |
    +=====+===+==============+=====+====================================================================================================+
    |    0|R  |INVLD_PKT_LEN |0x0  |Set to 1 if there is an invalid packet length (invalid transmission length)                         |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    1|R  |FRAME_SYNC_ERR|0x0  |Set to 1 if a frame end is received but not paired with a frame start in the same virtual channel   |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    2|R  |ECC_NO_ERR    |0x0  |Set to 1 when ECC check shows no error (either no error or more than 2 bits of error)               |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    3|R  |ECC_BIT_ERROR |0x0  |Set to 1 if there is an error in the ECC field                                                      |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    4|R  |ERR_FRAME_DATA|0x0  |If a CRC error is present in the data packet, then this error is set to 1 when vsync end is received|
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    5|R  |HS_RX_TO_ERR  |0x0  |Set to 1 in case of HS RX timeout                                                                   |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    6|R  |CRC_ERROR_VC1 |0x0  |Set to 1 if there is a checksum error on virtual channel 1 (Checksum Error Long packet only)        |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+
    |    7|R  |CRC_ERROR_VC2 |0x0  |Set to 1 if there is a checksum error on virtual channel 2 (Checksum Error Long packet only)        |
    +-----+---+--------------+-----+----------------------------------------------------------------------------------------------------+

.. _csi2_ctrl__ERR_LSB:

ERR_LSB
"""""""

Error status LSB

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |Reset|                                                        Description                                                        |
    +=====+===+===============+=====+===========================================================================================================================+
    |    0|R  |SOT_ERR        |0x0  |Set to 1 if there is an error with start of frame (SoT Error)                                                              |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    1|R  |SOT_SYNC_ERR   |0x0  |Set to 1 if there is an error in synchronization of Start of Transfer (SoT Sync Error)                                     |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    2|R  |FALSE_CTRL     |0x0  |Set to 1 if there is a False Control Error                                                                                 |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    3|R  |ECC_ERR_SINGLE |0x0  |Set to 1 if there is a single bit error, even when it is corrected using ECC (ECC Error, single-bit detected and corrected)|
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    4|R  |ECC_ERR_MULT   |0x0  |Set to 1 if there is a two-bit error in the packet (ECC Error, multi-bit detected not corrected)                           |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    5|R  |CRC_ERROR_VC0  |0x0  |Set to 1 if there is a checksum error on virtual channel 0 (Checksum Error Long packet only)                               |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    6|R  |INVLD_DATA_TYPE|0x0  |Set to 1 if the received data is invalid (CSI Data Type Not Recognized)                                                    |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+
    |    7|R  |INVLD_VC_ID    |0x0  |Set to 1 in case of invalid virtual channel ID (CSI VC ID Invalid)                                                         |
    +-----+---+---------------+-----+---------------------------------------------------------------------------------------------------------------------------+

.. _csi2_ctrl__HS_RX_TIMEOUT_MSB2:

HS_RX_TIMEOUT_MSB2
""""""""""""""""""

Configuration of timeout in high speed mode (MSB2)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+--------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                      Description                       |
    +=====+===+========+=====+========================================================+
    |7:0  |R/W|TIME_OUT|0xFF |High speed request timeout configuration (bits 16 to 23)|
    +-----+---+--------+-----+--------------------------------------------------------+

.. _csi2_ctrl__HS_RX_TIMEOUT_MSB1:

HS_RX_TIMEOUT_MSB1
""""""""""""""""""

Configuration of timeout in high speed mode (MSB1)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+-------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                      Description                      |
    +=====+===+========+=====+=======================================================+
    |7:0  |R/W|TIME_OUT|0xFF |High speed request timeout configuration (bits 8 to 15)|
    +-----+---+--------+-----+-------------------------------------------------------+

.. _csi2_ctrl__HS_RX_TIMEOUT_LSB:

HS_RX_TIMEOUT_LSB
"""""""""""""""""

Configuration of timeout in high speed mode (LSB)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+------------------------------------------------------+
    |Bit #|R/W|  Name  |Reset|                     Description                      |
    +=====+===+========+=====+======================================================+
    |7:0  |R/W|TIME_OUT|0xFF |High speed request timeout configuration (bits 0 to 7)|
    +-----+---+--------+-----+------------------------------------------------------+

.. _csi2_ctrl__VCCFG:

VCCFG
"""""

Virtual Channel configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-------------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                         Description                         |
    +=====+===+=====+=====+=============================================================+
    |    0|R/W|VCCFG|0x1  |Set bit to 1 to enable virtual channel (default: VC0 enabled)|
    +-----+---+-----+-----+-------------------------------------------------------------+

.. _csi2_ctrl__POLARITY:

POLARITY
""""""""

Vsync and Hsync polarity

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------+
    |Bit #|R/W|Name |Reset|                 Description                 |
    +=====+===+=====+=====+=============================================+
    |    0|R/W|VSYNC|0x0  |VSYNC polarity: 0: active high, 1: active low|
    +-----+---+-----+-----+---------------------------------------------+
    |    1|R/W|HSYNC|0x0  |HSYNC polarity: 0: active high, 1: active low|
    +-----+---+-----+-----+---------------------------------------------+

.. _csi2_ctrl__CCI_ADDRESS:

CCI_ADDRESS
"""""""""""

CCI address

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+---------------------+
    |Bit #|R/W| Name  |Reset|     Description     |
    +=====+===+=======+=====+=====================+
    |7:0  |R/W|ADDRESS|0x00 |CCI interface address|
    +-----+---+-------+-----+---------------------+

.. _csi2_ctrl__CCI_WRITE_DATA:

CCI_WRITE_DATA
""""""""""""""

CCI write data

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+------------------------+
    |Bit #|R/W| Name  |Reset|      Description       |
    +=====+===+=======+=====+========================+
    |7:0  |R/W|WR_DATA|0x00 |CCI interface write data|
    +-----+---+-------+-----+------------------------+

.. _csi2_ctrl__CCI_READ_DATA:

CCI_READ_DATA
"""""""""""""

CCI read data

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-----------------------+
    |Bit #|R/W| Name  |Reset|      Description      |
    +=====+===+=======+=====+=======================+
    |7:0  |R  |RD_DATA|0x00 |CCI interface read data|
    +-----+---+-------+-----+-----------------------+

.. _csi2_ctrl__CCI_READ_WRITE:

CCI_READ_WRITE
""""""""""""""

CCI R/W

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-----------+
    |Bit #|R/W|  Name   |Reset|Description|
    +=====+===+=========+=====+===========+
    |6:0  |R/W|CCI_BYTE |0x00 |CCI byte   |
    +-----+---+---------+-----+-----------+
    |7    |W  |CCI_WRITE|0x0  |CCI write  |
    +-----+---+---------+-----+-----------+

.. _csi2_ctrl__CCI_STATUS:

CCI_STATUS
""""""""""

CCI status

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                Description                |
    +=====+===+=========+=====+===========================================+
    |    0|R/W|RW_DONE  |0x0  |Read/write transfer done (write 1 to clear)|
    +-----+---+---------+-----+-------------------------------------------+
    |    1|R  |READ_READ|0x0  |Is set to 1 if CCI read data available     |
    +-----+---+---------+-----+-------------------------------------------+

.. _csi2_ctrl__CCI_DEV_ADDR:

CCI_DEV_ADDR
""""""""""""

CCI device ID

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------+
    |Bit #|R/W|Name|Reset|   Description    |
    +=====+===+====+=====+==================+
    |6:0  |R/W|ADDR|0x3C |CCI device address|
    +-----+---+----+-----+------------------+

.. _csi2_ctrl__ULPS_STATUS:

ULPS_STATUS
"""""""""""

CSI-2 ULPS status

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------+-----+-----------------------------------------------------------+
    |Bit #|R/W|      Name       |Reset|                        Description                        |
    +=====+===+=================+=====+===========================================================+
    |    0|R  |ULPS_ACTIVE_LANE0|0x0  |Set to 1 if ultra low power state is active for data lane 0|
    +-----+---+-----------------+-----+-----------------------------------------------------------+
    |    1|R  |ULPS_ACTIVE_LANE1|0x0  |Set to 1 if ultra low power state is active for data lane 1|
    +-----+---+-----------------+-----+-----------------------------------------------------------+
    |    4|R  |ULPS_ACTIVE_CLK  |0x0  |Set to 1 if ultra low power state is active for clock lane |
    +-----+---+-----------------+-----+-----------------------------------------------------------+
