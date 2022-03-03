.. 
   Input file: fe/ips/lnt_csi2_rx/README_PHY.md

Register map for MIPI Digital PHY
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-----------------------------+------+-----+--------------------------------------------------------------+
    |            Name             |Offset|Width|                         Description                          |
    +=============================+======+=====+==============================================================+
    |:ref:`REG00<csi2_phy__REG00>`|     0|   32|CSI2 DPHY Data lane and clock lane enable register            |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG0D<csi2_phy__REG0D>`|    52|   32|CSI2 DPHY Digital clock sample manual phase selection register|
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG0E<csi2_phy__REG0E>`|    56|   32|CSI2 DPHY Clock lane manual phase selection register          |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG0F<csi2_phy__REG0F>`|    60|   32|CSI2 DPHY Data lanes manual phase selection register          |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG12<csi2_phy__REG12>`|    72|   32|CSI2 DPHY Reverse digital sample clock register               |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG20<csi2_phy__REG20>`|   128|   32|CSI2 DPHY Digital reset register                              |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG4A<csi2_phy__REG4A>`|   296|   32|CSI2 DPHY Clock lane continuous mode register                 |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG58<csi2_phy__REG58>`|   352|   32|CSI2 DPHY Clock lane TSH_SETTLE register                      |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG5A<csi2_phy__REG5A>`|   360|   32|CSI2 DPHY Clock lane calibration reception register           |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG78<csi2_phy__REG78>`|   480|   32|CSI2 DPHY Data lane 0 TSH_SETTLE register                     |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG7A<csi2_phy__REG7A>`|   488|   32|CSI2 DPHY Data lane 0 calibration reception register          |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG98<csi2_phy__REG98>`|   608|   32|CSI2 DPHY Data lane 1 TSH_SETTLE register                     |
    +-----------------------------+------+-----+--------------------------------------------------------------+
    |:ref:`REG9A<csi2_phy__REG9A>`|   616|   32|CSI2 DPHY Data lane 1 calibration reception register          |
    +-----------------------------+------+-----+--------------------------------------------------------------+

.. _csi2_phy__REG00:

REG00
"""""

CSI2 DPHY Data lane and clock lane enable register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+------------------------------------+
    |Bit #|R/W|   Name    |Reset|            Description             |
    +=====+===+===========+=====+====================================+
    |    2|R/W|LANE_EN0   |0x0  |Enable D-PHY lane 0: active high    |
    +-----+---+-----------+-----+------------------------------------+
    |    3|R/W|LANE_EN1   |0x0  |Enable D-PHY lane 1: active high    |
    +-----+---+-----------+-----+------------------------------------+
    |    6|R/W|LANE_CLK_EN|0x0  |Enable D-PHY clock lane: active high|
    +-----+---+-----------+-----+------------------------------------+

.. _csi2_phy__REG0D:

REG0D
"""""

CSI2 DPHY Digital clock sample manual phase selection register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------+-----+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |Reset|                                               Description                                               |
    +=====+===+============+=====+=========================================================================================================+
    |2:0  |R/W|SAMPLE_PHASE|0x0  |Manual phase selection for digital clock sample: 0 to 7, 0 is earliest, 7 is the latest, phase step ~40ps|
    +-----+---+------------+-----+---------------------------------------------------------------------------------------------------------+

.. _csi2_phy__REG0E:

REG0E
"""""

CSI2 DPHY Clock lane manual phase selection register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+-----------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                                          Description                                          |
    +=====+===+===========+=====+===============================================================================================+
    |6:4  |R/W|CLOCK_PHASE|0x3  |Manual phase selection for clock lane: 0 to 7, 0 is earliest, 7 is the latest, phase step ~40ps|
    +-----+---+-----------+-----+-----------------------------------------------------------------------------------------------+

.. _csi2_phy__REG0F:

REG0F
"""""

CSI2 DPHY Data lanes manual phase selection register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                                          Description                                           |
    +=====+===+===========+=====+================================================================================================+
    |2:0  |R/W|DATA0_PHASE|0x3  |Manual phase selection for data lane 0: 0 to 7, 0 is earliest, 7 is the latest, phase step ~40ps|
    +-----+---+-----------+-----+------------------------------------------------------------------------------------------------+
    |5:3  |R/W|DATA1_PHASE|0x3  |Manual phase selection for data lane 1: 0 to 7, 0 is earliest, 7 is the latest, phase step ~40ps|
    +-----+---+-----------+-----+------------------------------------------------------------------------------------------------+

.. _csi2_phy__REG12:

REG12
"""""

CSI2 DPHY Reverse digital sample clock register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+-----+--------------------------------------------+
    |Bit #|R/W|     Name     |Reset|                Description                 |
    +=====+===+==============+=====+============================================+
    |    7|R/W|SAMPLE_REVERSE|0x0  |Set to 1 to reverse the digital sample clock|
    +-----+---+--------------+-----+--------------------------------------------+

.. _csi2_phy__REG20:

REG20
"""""

CSI2 DPHY Digital reset register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+-----------------------------+
    |Bit #|R/W|  Name  |Reset|         Description         |
    +=====+===+========+=====+=============================+
    |    0|R/W|DIG_RSTN|0x1  |Set to 0 to reset digital PHY|
    +-----+---+--------+-----+-----------------------------+

.. _csi2_phy__REG4A:

REG4A
"""""

CSI2 DPHY Clock lane continuous mode register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+--------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                   Description                    |
    +=====+===+=============+=====+==================================================+
    |5:4  |R/W|CLK_CONTINOUS|0x0  |Continuous clock mode: b00: disabled, b11: enabled|
    +-----+---+-------------+-----+--------------------------------------------------+

.. _csi2_phy__REG58:

REG58
"""""

CSI2 DPHY Clock lane TSH_SETTLE register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                                                              Description                                                                              |
    +=====+===+==========+=====+=======================================================================================================================================================================+
    |7:0  |R/W|THS_SETTLE|0x1B |Configure the count time of the THS_SETTLE by protocol. After count done, D-PHY will begin to receive the high speed data. See the note below for configuration values.|
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _csi2_phy__REG5A:

REG5A
"""""

CSI2 DPHY Clock lane calibration reception register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                          Description                          |
    +=====+===+=========+=====+===============================================================+
    |    7|R/W|CALIBRATE|0x0  |Calibration reception enable - 1'b0 : disable ; - 1'b1 : enable|
    +-----+---+---------+-----+---------------------------------------------------------------+

.. _csi2_phy__REG78:

REG78
"""""

CSI2 DPHY Data lane 0 TSH_SETTLE register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                                                              Description                                                                              |
    +=====+===+==========+=====+=======================================================================================================================================================================+
    |7:0  |R/W|THS_SETTLE|0x1B |Configure the count time of the THS_SETTLE by protocol. After count done, D-PHY will begin to receive the high speed data. See the note below for configuration values.|
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _csi2_phy__REG7A:

REG7A
"""""

CSI2 DPHY Data lane 0 calibration reception register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                          Description                          |
    +=====+===+=========+=====+===============================================================+
    |    7|R/W|CALIBRATE|0x0  |Calibration reception enable - 1'b0 : disable ; - 1'b1 : enable|
    +-----+---+---------+-----+---------------------------------------------------------------+

.. _csi2_phy__REG98:

REG98
"""""

CSI2 DPHY Data lane 1 TSH_SETTLE register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                                                              Description                                                                              |
    +=====+===+==========+=====+=======================================================================================================================================================================+
    |7:0  |R/W|THS_SETTLE|0x1B |Configure the count time of the THS_SETTLE by protocol. After count done, D-PHY will begin to receive the high speed data. See the note below for configuration values.|
    +-----+---+----------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _csi2_phy__REG9A:

REG9A
"""""

CSI2 DPHY Data lane 1 calibration reception register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                          Description                          |
    +=====+===+=========+=====+===============================================================+
    |    7|R/W|CALIBRATE|0x0  |Calibration reception enable - 1'b0 : disable ; - 1'b1 : enable|
    +-----+---+---------+-----+---------------------------------------------------------------+
