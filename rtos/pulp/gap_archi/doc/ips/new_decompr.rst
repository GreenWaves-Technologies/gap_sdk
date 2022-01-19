.. 
   Input file: fe/ips/mchan/doc/CL_DMA_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |                           Name                           |Offset|Width|                                               Description                                                |
    +==========================================================+======+=====+==========================================================================================================+
    |:ref:`TCDM_ADDR_REG<new_decompr__TCDM_ADDR_REG>`          |     0|   32|Start address in TCDM (L1)                                                                                |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`L2_ADDR_REG<new_decompr__L2_ADDR_REG>`              |     4|   32|Start address in L2                                                                                       |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`CONF_REG<new_decompr__CONF_REG>`                    |     8|   32|Setup of the different parameters to configure compression/decompression                                  |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`STATUS_REG<new_decompr__STATUS_REG>`                |    12|   32|Busy status for decompressor engines T1, T2 and T3, and compressor engine T1                              |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`LUT_WRITE_REG<new_decompr__LUT_WRITE_REG>`          |    16|   32|Write LUT content (for T2 and T3)                                                                         |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`SPECIAL_SYMBOL_REG<new_decompr__SPECIAL_SYMBOL_REG>`|    20|   32|For T3 decompression only, decompressed value of the special symbol compressed into a single '0' bit in L2|
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`BIT_READ_REG<new_decompr__BIT_READ_REG>`            |    24|   32|Number of bits read during the decompression/compression                                                  |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`MODE_REG<new_decompr__MODE_REG>`                    |    28|   32|Transfer mode (linear or 2D) for source and destination                                                   |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`SOFT_RESET_REG<new_decompr__SOFT_RESET_REG>`        |    32|   32|Reset of compression/decompression engines and FIFOs (configuration registers are preserved)              |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`CLOCK_ENABLE_REG<new_decompr__CLOCK_ENABLE_REG>`    |    36|   32|Clock gating control: enable/disable the clock of the compressor/decompressor                             |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`PUSH_CMD_REG<new_decompr__PUSH_CMD_REG>`            |    40|   32|Trigger TX or RX transaction according to CONF_REG configuration                                          |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`L2_COUNT_REG<new_decompr__L2_COUNT_REG>`            |    48|   32|Number of items to decompress after each stride from L2 to TCDM (Used only for 2D decompression)          |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`L2_STRIDE_REG<new_decompr__L2_STRIDE_REG>`          |    52|   32|Number of items to jump for every L2_COUNT_REG in the L2 (Used only for 2D decompression)                 |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`TCDM_COUNT_REG<new_decompr__TCDM_COUNT_REG>`        |    56|   32|Number of items to decompress after each stride from TCDM to L2 (Used only for 2D decompression)          |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+
    |:ref:`TCDM_STRIDE_REG<new_decompr__TCDM_STRIDE_REG>`      |    60|   32|Number of items to jump for every TCDM_COUNT_REG in the L2 (Used only for 2D decompression)               |
    +----------------------------------------------------------+------+-----+----------------------------------------------------------------------------------------------------------+

.. _new_decompr__TCDM_ADDR_REG:

TCDM_ADDR_REG
"""""""""""""

Start address in TCDM (L1)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+----------+-------------------------------------------+
    |Bit #|R/W|     Name      |  Reset   |                Description                |
    +=====+===+===============+==========+===========================================+
    |31:0 |R/W|TCDM_START_ADDR|0x00000000|Start address for TCDM transfers (TX or RX)|
    +-----+---+---------------+----------+-------------------------------------------+

.. _new_decompr__L2_ADDR_REG:

L2_ADDR_REG
"""""""""""

Start address in L2

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+----------+-----------------------------------------+
    |Bit #|R/W|    Name     |  Reset   |               Description               |
    +=====+===+=============+==========+=========================================+
    |31:0 |R/W|L2_START_ADDR|0x00000000|Start address for L2 transfers (TX or RX)|
    +-----+---+-------------+----------+-----------------------------------------+

.. _new_decompr__CONF_REG:

CONF_REG
""""""""

Setup of the different parameters to configure compression/decompression

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |Reset |                                                                                  Description                                                                                  |
    +=====+===+=================+======+===============================================================================================================================================================================+
    |1:0  |R/W|decompr_mode     |0x0   |Decompression Mode: b00: T1; b01: T2; b11:T3; b10: reserved.                                                                                                                   |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |3:2  |R/W|extension_type   |0x3   |Extension Type: b00: 8-bit; b01: 16-bit; b11: 32-bit; b10: reserved.                                                                                                           |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |8:4  |R/W|item_bit_width   |0x04  |Size of compressed data item in L2. Allowed sizes are 1 to 31 for T1, 1 to 15 for T2 and T3. Warning: An illegal configuration will be handled like a 1-bit width configuration|
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |9    |R/W|sign_extension   |0x0   |Sign extension during decompression: 0: Unsigned; 1: Signed.                                                                                                                   |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12:10|R/W|start_bit        |0x0   |Bit offset (in the byte) for L2 start address during decompression or compression.                                                                                             |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13   |R/W|decompr_direction|0x0   |Transfer direction: 0: TX (decompression); 1: RX (compression T1).                                                                                                             |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:14|R/W|start_byte       |0x0   |Byte offset (in a 32-bit word) in L2 start address during decompression or compression.                                                                                        |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|items_to_transfer|0x0000|Total items to compress/decompress during a transfer.                                                                                                                          |
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _new_decompr__STATUS_REG:

STATUS_REG
""""""""""

Busy status for decompressor engines T1, T2 and T3, and compressor engine T1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+-----+------------------------------------------------------------+
    |Bit #|R/W|     Name      |Reset|                        Description                         |
    +=====+===+===============+=====+============================================================+
    |    0|R  |T1_DECOMPR_BUSY|0x0  |Status of the T1 decompression engine: if 1, engine is busy.|
    +-----+---+---------------+-----+------------------------------------------------------------+
    |    1|R  |T2_DECOMPR_BUSY|0x0  |Status of the T2 decompression engine: if 1, engine is busy.|
    +-----+---+---------------+-----+------------------------------------------------------------+
    |    2|R  |T3_DECOMPR_BUSY|0x0  |Status of the T3 decompression engine: if 1, engine is busy.|
    +-----+---+---------------+-----+------------------------------------------------------------+
    |    3|R  |T1_COMPR_BUSY  |0x0  |Status of the T1 compression engine: if 1, engine is busy.  |
    +-----+---+---------------+-----+------------------------------------------------------------+

.. _new_decompr__LUT_WRITE_REG:

LUT_WRITE_REG
"""""""""""""

Write LUT content (for T2 and T3)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+------+--------------------------------+
    |Bit #|R/W|  Name  |Reset |          Description           |
    +=====+===+========+======+================================+
    |15:0 |W  |LUT_DATA|0x0000|Data to write in the LUT        |
    +-----+---+--------+------+--------------------------------+
    |23:16|W  |LUT_ADDR|0x00  |Address of the LUT to be written|
    +-----+---+--------+------+--------------------------------+

.. _new_decompr__SPECIAL_SYMBOL_REG:

SPECIAL_SYMBOL_REG
""""""""""""""""""

For T3 decompression only, decompressed value of the special symbol compressed into a single '0' bit in L2

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------+----------+------------------------------------------------------------------+
    |Bit #|R/W|     Name     |  Reset   |                           Description                            |
    +=====+===+==============+==========+==================================================================+
    |31:0 |R/W|SPECIAL_SYMBOL|0xABBAABBA|Special symbol compressed as a single '0' bit for T3 decompression|
    +-----+---+--------------+----------+------------------------------------------------------------------+

.. _new_decompr__BIT_READ_REG:

BIT_READ_REG
""""""""""""

Number of bits read during the decompression/compression

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-----------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                  Description                  |
    +=====+===+========+==========+===============================================+
    |31:0 |R  |BIT_READ|0x00000000|Number of bits read at the end of decompression|
    +-----+---+--------+----------+-----------------------------------------------+

.. _new_decompr__MODE_REG:

MODE_REG
""""""""

Transfer mode (linear or 2D) for source and destination

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                                                                       Description                                                                        |
    +=====+===+===========+=====+==========================================================================================================================================================+
    |1:0  |R/W|TRANSF_MODE|0x0  |Transfer mode: 0: linear L2/linear TCDM; 1: 2D L2/linear TCDM; 2: linear L2/2D TCDM; 3: reserved. Warning: Only T1 decompression supports a non-0 setting.|
    +-----+---+-----------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _new_decompr__SOFT_RESET_REG:

SOFT_RESET_REG
""""""""""""""

Reset of compression/decompression engines and FIFOs (configuration registers are preserved)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                                               Description                                                |
    +=====+===+==========+=====+==========================================================================================================+
    |    0|W  |SOFT_RESET|0x0  |Writing any value generates a reset to clear all engines and FIFOs. Configuration registers are not reset.|
    +-----+---+----------+-----+----------------------------------------------------------------------------------------------------------+

.. _new_decompr__CLOCK_ENABLE_REG:

CLOCK_ENABLE_REG
""""""""""""""""

Clock gating control: enable/disable the clock of the compressor/decompressor

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------+-----+------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |Reset|                                                                  Description                                                                   |
    +=====+===+============+=====+================================================================================================================================================+
    |    0|W  |CLOCK_ENABLE|0x0  |Set to 1 to enable the clock for the compressor/decompressor block. Access to configuration registers remains active when the clock is disabled.|
    +-----+---+------------+-----+------------------------------------------------------------------------------------------------------------------------------------------------+

.. _new_decompr__PUSH_CMD_REG:

PUSH_CMD_REG
""""""""""""

Trigger TX or RX transaction according to CONF_REG configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+-------------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                               Description                               |
    +=====+===+=======+=====+=========================================================================+
    |    0|W  |TRIGGER|0x0  |Write 1 to trigger a TX or RX transfer, according to configured settings.|
    +-----+---+-------+-----+-------------------------------------------------------------------------+

.. _new_decompr__L2_COUNT_REG:

L2_COUNT_REG
""""""""""""

Number of items to decompress after each stride from L2 to TCDM (Used only for 2D decompression)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+------+----------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |Reset |                                                        Description                                                         |
    +=====+===+===============+======+============================================================================================================================+
    |15:0 |R/W|L2_LINEAR_COUNT|0x0000|For L2 2D transfers, number of items for each linear part of the transfer, minus 1 (e.g. set to 7 for a 8-item linear part).|
    +-----+---+---------------+------+----------------------------------------------------------------------------------------------------------------------------+

.. _new_decompr__L2_STRIDE_REG:

L2_STRIDE_REG
"""""""""""""

Number of items to jump for every L2_COUNT_REG in the L2 (Used only for 2D decompression)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|     Name      |Reset |                                                                                                             Description                                                                                                             |
    +=====+===+===============+======+=====================================================================================================================================================================================================================================+
    |15:0 |R/W|L2_STRIDE_COUNT|0x0000|For L2 2D transfers, number of items in a stride (i.e. from a linear chunck of data to the next), minus 1 (e.g if the first item of the next linear chunk of data is 16 items after the first item of current chunk, then set to 15).|
    +-----+---+---------------+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _new_decompr__TCDM_COUNT_REG:

TCDM_COUNT_REG
""""""""""""""

Number of items to decompress after each stride from TCDM to L2 (Used only for 2D decompression)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------+------+-------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |Reset |                                     Description                                     |
    +=====+===+=================+======+=====================================================================================+
    |15:0 |R/W|TCDM_LINEAR_COUNT|0x0000|For TCDM 2D transfers, number of items for each linear part of the transfer, minus 1.|
    +-----+---+-----------------+------+-------------------------------------------------------------------------------------+

.. _new_decompr__TCDM_STRIDE_REG:

TCDM_STRIDE_REG
"""""""""""""""

Number of items to jump for every TCDM_COUNT_REG in the L2 (Used only for 2D decompression)

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------+------+------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |Reset |                                                Description                                                 |
    +=====+===+=================+======+============================================================================================================+
    |15:0 |R/W|TCDM_STRIDE_COUNT|0x0000|For TCDM 2D transfers, number of items in a stride (i.e. from a linear chunck of data to the next), minus 1.|
    +-----+---+-----------------+------+------------------------------------------------------------------------------------------------------------+
