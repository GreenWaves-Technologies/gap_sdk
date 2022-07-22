.. 
   Input file: fe/ips/power_manager_vega/docs/DLC_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------------+------+-----+-------------------------------------------+
    |                       Name                       |Offset|Width|                Description                |
    +==================================================+======+=====+===========================================+
    |:ref:`DLCPD_MSR<power_manager__DLCPD_MSR>`        |0x0   |   32|Maestro Status Register                    |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_MPACR<power_manager__DLCPD_MPACR>`    |0x4   |   32|Maestro PICL Access Control Register       |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_MPADR<power_manager__DLCPD_MPADR>`    |0x8   |   32|Maestro PICL Access Data Register          |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IMR<power_manager__DLCPD_IMR>`        |0xC   |   32|Maestro Interrupt Mask Register            |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IFR<power_manager__DLCPD_IFR>`        |0x10  |   32|Maestro Interrupt Flag Register            |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IOK_IFR<power_manager__DLCPD_IOK_IFR>`|0x14  |   32|Maestro ICU_OK Interrupt Flag Register     |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IDL_IFR<power_manager__DLCPD_IDL_IFR>`|0x18  |   32|Maestro ICU_DELAYED Interrupt Flag Register|
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IDN_IFR<power_manager__DLCPD_IDN_IFR>`|0x1C  |   32|Maestro ICU_DENIED Interrupt Flag Register |
    +--------------------------------------------------+------+-----+-------------------------------------------+
    |:ref:`DLCPD_IUP_IFR<power_manager__DLCPD_IUP_IFR>`|0x20  |   32|Maestro ICU_UPDATED Interrupt Flag Register|
    +--------------------------------------------------+------+-----+-------------------------------------------+

.. _power_manager__DLCPD_MSR:

DLCPD_MSR
"""""""""

Maestro Status Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                                                Description                                                |
    +=====+===+=========+=====+===========================================================================================================+
    |    0|W  |PICL_BUSY|    0|PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer.|
    +-----+---+---------+-----+-----------------------------------------------------------------------------------------------------------+
    |    1|W  |SCU_BUSY |    0|SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends.                    |
    +-----+---+---------+-----+-----------------------------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_MPACR:

DLCPD_MPACR
"""""""""""

Maestro PICL Access Control Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                                    Description                                     |
    +=====+===+=======+=====+====================================================================================+
    |15:0 |R  |PAADDR |    0|PICL Access Address.                                                                |
    +-----+---+-------+-----+------------------------------------------------------------------------------------+
    |24   |R  |PADIR  |    0|PICL Access Direction: b0: write; b1: read.                                         |
    +-----+---+-------+-----+------------------------------------------------------------------------------------+
    |28   |R  |PASTART|    0|PICL Access Start. This field is automatically cleared when PICL access is finished.|
    +-----+---+-------+-----+------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_MPADR:

DLCPD_MPADR
"""""""""""

Maestro PICL Access Data Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-----------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                                          Description                                          |
    +=====+===+=======+=====+===============================================================================================+
    |15:0 |W  |PRWDATA|    0|PICL Read/Write Data. This field is automatically updated after a PICL read access is finished.|
    +-----+---+-------+-----+-----------------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_IMR:

DLCPD_IMR
"""""""""

Maestro Interrupt Mask Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+------------------------------+
    |Bit #|R/W|  Name   |Reset|         Description          |
    +=====+===+=========+=====+==============================+
    |    0|W  |ICU_OK_M |    0|Mask of ICU_OK interrupt.     |
    +-----+---+---------+-----+------------------------------+
    |    1|W  |ICU_DLY_M|    0|Mask of ICU_DELAYED interrupt.|
    +-----+---+---------+-----+------------------------------+
    |    2|W  |ICU_DEN_M|    0|Mask of ICU_DENIED interrupt. |
    +-----+---+---------+-----+------------------------------+
    |    3|W  |ICU_UPD_M|    0|Mask of ICU_UPDATED interrupt.|
    +-----+---+---------+-----+------------------------------+
    |    6|W  |PICL_OK_M|    0|Mask of PICL_OK interrupt.    |
    +-----+---+---------+-----+------------------------------+
    |    7|W  |SCU_OK_M |    0|Mask of SCU_OK interrupt.     |
    +-----+---+---------+-----+------------------------------+
    |    8|W  |SCU_FL_M |    0|Mask of SCU_FL interrupt.     |
    +-----+---+---------+-----+------------------------------+

.. _power_manager__DLCPD_IFR:

DLCPD_IFR
"""""""""

Maestro Interrupt Flag Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                                     Description                                      |
    +=====+===+=========+=====+======================================================================================+
    |    0|W  |ICU_OK_F |    0|Set when at least one of the bit of the DLCPD_IOK_IFR register is set.                |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    1|W  |ICU_DLY_F|    0|Set when at least one of the bit of the DLCPD_IDL_IFR register is set.                |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    2|W  |ICU_DEN_F|    0|Set when at least one of the bit of the DLCPD_IDN_IFR register is set.                |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    3|W  |ICU_UPD_F|    0|Set when at least one of the bit of the DLCPD_IUP_IFR register is set.                |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    6|W  |PICL_OK_F|    0|Set when PICL transfer is finished. Cleared when writing 1 in this field.             |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    7|W  |SCU_OK_F |    0|Set when SCU sequence is finished without error. Cleared when writing 1 in this field.|
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+
    |    8|W  |SCU_FL_F |    0|Set when SCU sequence is finished with error. Cleared when writing 1 in this field.   |
    +-----+---+---------+-----+--------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_IOK_IFR:

DLCPD_IOK_IFR
"""""""""""""

Maestro ICU_OK Interrupt Flag Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |Reset|                                                                                       Description                                                                                       |
    +=====+===+============+=====+=========================================================================================================================================================================================+
    |31:0 |W  |ICU_OK_FLAGS|    0|Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1.|
    +-----+---+------------+-----+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_IDL_IFR:

DLCPD_IDL_IFR
"""""""""""""

Maestro ICU_DELAYED Interrupt Flag Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                                                                                        Description                                                                                         |
    +=====+===+=============+=====+============================================================================================================================================================================================+
    |31:0 |W  |ICU_DLY_FLAGS|    0|Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_IDN_IFR:

DLCPD_IDN_IFR
"""""""""""""

Maestro ICU_DENIED Interrupt Flag Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                                                                                       Description                                                                                        |
    +=====+===+=============+=====+==========================================================================================================================================================================================+
    |31:0 |W  |ICU_DEN_FLAGS|    0|Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+-----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _power_manager__DLCPD_IUP_IFR:

DLCPD_IUP_IFR
"""""""""""""

Maestro ICU_UPDATED Interrupt Flag Register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                                                                    Description                                                                     |
    +=====+===+=============+=====+====================================================================================================================================================+
    |31:0 |W  |ICU_UPD_FLAGS|    0|Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------+
