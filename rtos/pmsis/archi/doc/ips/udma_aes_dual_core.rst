.. 
   Input file: fe/ips/udma/udma_aes/README_dual_core.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-----------------------------------------+------+-----+--------------------------------------------------+
    |                  Name                   |Offset|Width|                   Description                    |
    +=========================================+======+=====+==================================================+
    |:ref:`KEY0_0<udma_aes_dual_core__KEY0_0>`|0x0   |   32|Word 0 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_1<udma_aes_dual_core__KEY0_1>`|0x4   |   32|Word 1 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_2<udma_aes_dual_core__KEY0_2>`|0x8   |   32|Word 2 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_3<udma_aes_dual_core__KEY0_3>`|0xC   |   32|Word 3 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_4<udma_aes_dual_core__KEY0_4>`|0x10  |   32|Word 4 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_5<udma_aes_dual_core__KEY0_5>`|0x14  |   32|Word 5 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_6<udma_aes_dual_core__KEY0_6>`|0x18  |   32|Word 6 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY0_7<udma_aes_dual_core__KEY0_7>`|0x1C  |   32|Word 7 of encryption key for core 0               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV0_0<udma_aes_dual_core__IV0_0>`  |0x20  |   32|Word 0 of encrypted block initial value for core 0|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV0_1<udma_aes_dual_core__IV0_1>`  |0x24  |   32|Word 1 of encrypted block initial value for core 0|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV0_2<udma_aes_dual_core__IV0_2>`  |0x28  |   32|Word 2 of encrypted block initial value for core 0|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV0_3<udma_aes_dual_core__IV0_3>`  |0x2C  |   32|Word 3 of encrypted block initial value for core 0|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_0<udma_aes_dual_core__KEY1_0>`|0x30  |   32|Word 0 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_1<udma_aes_dual_core__KEY1_1>`|0x34  |   32|Word 1 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_2<udma_aes_dual_core__KEY1_2>`|0x38  |   32|Word 2 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_3<udma_aes_dual_core__KEY1_3>`|0x3C  |   32|Word 3 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_4<udma_aes_dual_core__KEY1_4>`|0x40  |   32|Word 4 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_5<udma_aes_dual_core__KEY1_5>`|0x44  |   32|Word 5 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_6<udma_aes_dual_core__KEY1_6>`|0x48  |   32|Word 6 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`KEY1_7<udma_aes_dual_core__KEY1_7>`|0x4C  |   32|Word 7 of encryption key for core 1               |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV1_0<udma_aes_dual_core__IV1_0>`  |0x50  |   32|Word 0 of encrypted block initial value for core 1|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV1_1<udma_aes_dual_core__IV1_1>`  |0x54  |   32|Word 1 of encrypted block initial value for core 1|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV1_2<udma_aes_dual_core__IV1_2>`  |0x58  |   32|Word 2 of encrypted block initial value for core 1|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`IV1_3<udma_aes_dual_core__IV1_3>`  |0x5C  |   32|Word 3 of encrypted block initial value for core 1|
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`DEST<udma_aes_dual_core__DEST>`    |0x60  |   32|RX and TX destination channels                    |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`SETUP0<udma_aes_dual_core__SETUP0>`|0x64  |   32|Core 0 setup                                      |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`SETUP1<udma_aes_dual_core__SETUP1>`|0x68  |   32|Core 1 setup                                      |
    +-----------------------------------------+------+-----+--------------------------------------------------+
    |:ref:`CFG<udma_aes_dual_core__CFG>`      |0x6C  |   32|AES data flow configuration                       |
    +-----------------------------------------+------+-----+--------------------------------------------------+

.. _udma_aes_dual_core__KEY0_0:

KEY0_0
""""""

Word 0 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_1:

KEY0_1
""""""

Word 1 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_2:

KEY0_2
""""""

Word 2 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_3:

KEY0_3
""""""

Word 3 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_4:

KEY0_4
""""""

Word 4 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_5:

KEY0_5
""""""

Word 5 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_6:

KEY0_6
""""""

Word 6 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY0_7:

KEY0_7
""""""

Word 7 of encryption key for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__IV0_0:

IV0_0
"""""

Word 0 of encrypted block initial value for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV0_1:

IV0_1
"""""

Word 1 of encrypted block initial value for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV0_2:

IV0_2
"""""

Word 2 of encrypted block initial value for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV0_3:

IV0_3
"""""

Word 3 of encrypted block initial value for core 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__KEY1_0:

KEY1_0
""""""

Word 0 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_1:

KEY1_1
""""""

Word 1 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_2:

KEY1_2
""""""

Word 2 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_3:

KEY1_3
""""""

Word 3 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_4:

KEY1_4
""""""

Word 4 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_5:

KEY1_5
""""""

Word 5 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_6:

KEY1_6
""""""

Word 6 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__KEY1_7:

KEY1_7
""""""

Word 7 of encryption key for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+----------+-------------------------------------------------+
    |Bit #|R/W|  Name  |  Reset   |                   Description                   |
    +=====+===+========+==========+=================================================+
    |31:0 |W  |KEY_WORD|0x00000000|Value of the corresponding word of encryption key|
    +-----+---+--------+----------+-------------------------------------------------+

.. _udma_aes_dual_core__IV1_0:

IV1_0
"""""

Word 0 of encrypted block initial value for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV1_1:

IV1_1
"""""

Word 1 of encrypted block initial value for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV1_2:

IV1_2
"""""

Word 2 of encrypted block initial value for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__IV1_3:

IV1_3
"""""

Word 3 of encrypted block initial value for core 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+----------+--------------------------------------------------------------+
    |Bit #|R/W|   Name   |  Reset   |                         Description                          |
    +=====+===+==========+==========+==============================================================+
    |31:0 |W  |BLOCK_WORD|0x00000000|Value of the corresponding word of the initial encrypted block|
    +-----+---+----------+----------+--------------------------------------------------------------+

.. _udma_aes_dual_core__DEST:

DEST
""""

RX and TX destination channels

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+---------------------------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                             Description                             |
    +=====+===+=======+=====+=====================================================================+
    |7:0  |R/W|RX_DEST|0xFF |Stream ID for the RX uDMA channel. Default is 0xFF (channel disabled)|
    +-----+---+-------+-----+---------------------------------------------------------------------+
    |15:8 |R/W|TX_DEST|0xFF |Stream ID for the TX uDMA channel. Default is 0xFF (channel disabled)|
    +-----+---+-------+-----+---------------------------------------------------------------------+

.. _udma_aes_dual_core__SETUP0:

SETUP0
""""""

Core 0 setup

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+--------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                   Description                    |
    +=====+===+=========+=====+==================================================+
    |    0|R  |KEY_INIT |0x0  |Is set to 1 when the key configuration is finished|
    +-----+---+---------+-----+--------------------------------------------------+
    |    1|R/W|KEY_TYPE |0x0  |Key type: b0: 128 bits; b1: 256 bits              |
    +-----+---+---------+-----+--------------------------------------------------+
    |    2|R/W|ENC_DEC  |0x0  |Operation type: b0: decryption; b1: encryption    |
    +-----+---+---------+-----+--------------------------------------------------+
    |    3|R/W|ECB_CBC  |0x0  |Encryption type: b0: ECB; b1: CBC                 |
    +-----+---+---------+-----+--------------------------------------------------+
    |    4|W  |BLOCK_RST|0x0  |Write b1 to reset AES core 0                      |
    +-----+---+---------+-----+--------------------------------------------------+
    |    5|R/W|QK_KEY_EN|0x0  |Use quiddikey key generation                      |
    +-----+---+---------+-----+--------------------------------------------------+
    |    8|W  |FIFO_CLR |0x0  |Write b1 to clear data FIFO                       |
    +-----+---+---------+-----+--------------------------------------------------+

.. _udma_aes_dual_core__SETUP1:

SETUP1
""""""

Core 1 setup

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+--------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                   Description                    |
    +=====+===+=========+=====+==================================================+
    |    0|R  |KEY_INIT |0x0  |Is set to 1 when the key configuration is finished|
    +-----+---+---------+-----+--------------------------------------------------+
    |    1|R/W|KEY_TYPE |0x0  |Key type: b0: 128 bits; b1: 256 bits              |
    +-----+---+---------+-----+--------------------------------------------------+
    |    2|R/W|ENC_DEC  |0x0  |Operation type: b0: decryption; b1: encryption    |
    +-----+---+---------+-----+--------------------------------------------------+
    |    3|R/W|ECB_CBC  |0x0  |Encryption type: b0: ECB; b1: CBC                 |
    +-----+---+---------+-----+--------------------------------------------------+
    |    4|W  |BLOCK_RST|0x0  |Write b1 to reset AES core 1                      |
    +-----+---+---------+-----+--------------------------------------------------+
    |    5|R/W|QK_KEY_EN|0x0  |Use quiddikey key generation                      |
    +-----+---+---------+-----+--------------------------------------------------+
    |    8|W  |FIFO_CLR |0x0  |Write b1 to clear data FIFO                       |
    +-----+---+---------+-----+--------------------------------------------------+

.. _udma_aes_dual_core__CFG:

CFG
"""

AES data flow configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                                                                                                                               Description                                                                                                                                                |
    +=====+===+====+=====+==========================================================================================================================================================================================================================================================================================================+
    |1:0  |R/W|MODE|0x0  |Transfer mode for core 0: b00: memory to memory; b01: stream to memory; b10: memory to stream; b11: stream to stream --- reverse configuration is used for core 1. When used with memory controller, must be set to b10 (core 0: memory to stream for encryption; core 1: stream to memory for decryption)|
    +-----+---+----+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
