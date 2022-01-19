.. 
   Input file: fe/ips/quiddikey/doc/quiddikey_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +------------------------------------------+------+-----+-----------------------------------------+
    |                   Name                   |Offset|Width|               Description               |
    +==========================================+======+=====+=========================================+
    |:ref:`CR<quiddikey__CR>`                  |     0|   32|Control register                         |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`SR<quiddikey__SR>`                  |     8|   32|Status register                          |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`AR<quiddikey__AR>`                  |    12|   32|Allow register                           |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IER<quiddikey__IER>`                |    16|   32|Interrupt Enable register                |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IMR<quiddikey__IMR>`                |    20|   32|Interrupt Mask register                  |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`ISR<quiddikey__ISR>`                |    24|   32|Interrupt Status register                |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DATA_DEST<quiddikey__DATA_DEST>`    |    32|   32|Destination Data register                |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DIR<quiddikey__DIR>`                |   160|   32|Data Input register                      |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DOR<quiddikey__DOR>`                |   168|   32|Data Output register                     |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`MISC<quiddikey__MISC>`              |   192|   32|Miscellaneous register                   |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IF_SR<quiddikey__IF_SR>`            |   208|   32|Interface Status register                |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`TEST<quiddikey__TEST>`              |   216|   32|Test register                            |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`PSR<quiddikey__PSR>`                |   220|   32|PUF Score register                       |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_RUC0<quiddikey__HW_RUC0>`        |   224|   32|Hardware Restrict User Context 0 register|
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_RUC1<quiddikey__HW_RUC1>`        |   228|   32|Hardware Restrict User Context 1 register|
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_SETTINGS<quiddikey__HW_SETTINGS>`|   240|   32|Hardware Settings register               |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_INFO<quiddikey__HW_INFO>`        |   244|   32|Hardware Information register            |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_ID<quiddikey__HW_ID>`            |   248|   32|Hardware Identifier register             |
    +------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_VER<quiddikey__HW_VER>`          |   252|   32|Hardware Version register                |
    +------------------------------------------+------+-----+-----------------------------------------+

.. _quiddikey__CR:

CR
""

Control register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------------+-----+--------------------------------------+
    |Bit #|R/W|        Name         |Reset|             Description              |
    +=====+===+=====================+=====+======================================+
    |    0|R/W|ZEROIZE              |0x00 |Begin Zeroize operation               |
    +-----+---+---------------------+-----+--------------------------------------+
    |    1|R/W|ENROLL               |0x00 |Begin Enroll operation                |
    +-----+---+---------------------+-----+--------------------------------------+
    |    2|R/W|START                |0x00 |Begin Start operation                 |
    +-----+---+---------------------+-----+--------------------------------------+
    |    5|R/W|STOP                 |0x00 |Begin Stop operation                  |
    +-----+---+---------------------+-----+--------------------------------------+
    |    6|R/W|GET_KEY              |0x00 |Begin Get Key operation               |
    +-----+---+---------------------+-----+--------------------------------------+
    |    7|R/W|UNWRAP               |0x00 |Begin Unwrap operation                |
    +-----+---+---------------------+-----+--------------------------------------+
    |    8|R/W|WRAP_GENERATED_RANDOM|0x00 |Begin Wrap Generation Random operation|
    +-----+---+---------------------+-----+--------------------------------------+
    |    9|R/W|WRAP                 |0x00 |Begin Wrap operation                  |
    +-----+---+---------------------+-----+--------------------------------------+
    |   15|R/W|GENERATE_RANDOM      |0x00 |Begin Generate Random operation       |
    +-----+---+---------------------+-----+--------------------------------------+
    |   16|R/W|RESEED               |0x00 |Begin Reseed operation                |
    +-----+---+---------------------+-----+--------------------------------------+
    |   31|R/W|TEST_PUF             |0x00 |Begin Test PUF operation              |
    +-----+---+---------------------+-----+--------------------------------------+

.. _quiddikey__SR:

SR
""

Status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+-----+----------------------------------------------------+
    |Bit #|R/W|     Name      |Reset|                    Description                     |
    +=====+===+===============+=====+====================================================+
    |    0|R/W|BUSY           |0x00 |Operation is in progress                            |
    +-----+---+---------------+-----+----------------------------------------------------+
    |    1|R/W|OK             |0x00 |Last operation was successful                       |
    +-----+---+---------------+-----+----------------------------------------------------+
    |    2|R/W|ERROR          |0x00 |Last operation failed                               |
    +-----+---+---------------+-----+----------------------------------------------------+
    |    3|R/W|ZEROIZED       |0x00 |Quiddikey is in Zeroized or in Locked state         |
    +-----+---+---------------+-----+----------------------------------------------------+
    |    4|R/W|REJECTED       |0x00 |Read: last command rejected, Write 1: Clear this bit|
    +-----+---+---------------+-----+----------------------------------------------------+
    |    5|R/W|DI_REQUEST     |0x00 |Request for Data in transfer via DIR register       |
    +-----+---+---------------+-----+----------------------------------------------------+
    |    6|R/W|DO_REQUEST     |0x00 |Request for Data out transfer via DOR register      |
    +-----+---+---------------+-----+----------------------------------------------------+
    |   29|R/W|RESEED_WARNING |0x00 |Reseed warning (see ORR RESEED_WARNING)             |
    +-----+---+---------------+-----+----------------------------------------------------+
    |   30|R/W|RESEED_REQUIRED|0x00 |Reseed required (see ORR RESEED_REQUIRED)           |
    +-----+---+---------------+-----+----------------------------------------------------+
    |   31|R/W|LAB_TEST_MODE  |0x00 |Quiddikey is in state Lab Test Mode                 |
    +-----+---+---------------+-----+----------------------------------------------------+

.. _quiddikey__AR:

AR
""

Allow register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------------+-----+------------------------+
    |Bit #|R/W|       Name       |Reset|      Description       |
    +=====+===+==================+=====+========================+
    |    1|R/W|ALLOW_ENROLL      |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    2|R/W|ALLOW_START       |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    5|R/W|ALLOW_STOP        |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    6|R/W|ALLOW_GET_KEY     |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    7|R/W|ALLOW_UNWRAP      |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    8|R/W|ALLOW_WRAP_GEN_RND|0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |    9|R/W|ALLOW_WRAP        |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |   15|R/W|ALLOW_GEN_RND     |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |   16|R/W|ALLOW_RESEED      |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+
    |   31|R/W|ALLOW_TEST_PUF    |0x00 |Operation allowed status|
    +-----+---+------------------+-----+------------------------+

.. _quiddikey__IER:

IER
"""

Interrupt Enable register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+-------------------------+
    |Bit #|R/W| Name |Reset|       Description       |
    +=====+===+======+=====+=========================+
    |    0|R/W|INT_EN|0x00 |Interrupt enable register|
    +-----+---+------+-----+-------------------------+

.. _quiddikey__IMR:

IMR
"""

Interrupt Mask register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------------------+-----+---------------------------------+
    |Bit #|R/W|         Name         |Reset|           Description           |
    +=====+===+======================+=====+=================================+
    |    0|R/W|INT_EN_BUSY           |0x00 |Enable Busy interrupt            |
    +-----+---+----------------------+-----+---------------------------------+
    |    1|R/W|INT_EN_OK             |0x00 |Enable Ok interrupt              |
    +-----+---+----------------------+-----+---------------------------------+
    |    2|R/W|INT_EN_ERROR          |0x00 |Enable Error interrupt           |
    +-----+---+----------------------+-----+---------------------------------+
    |    3|R/W|INT_EN_ZEROIZED       |0x00 |Enable Zeroized interrupt        |
    +-----+---+----------------------+-----+---------------------------------+
    |    4|R/W|INT_EN_REJECTED       |0x00 |Enable Rejected interrupt        |
    +-----+---+----------------------+-----+---------------------------------+
    |    5|R/W|INT_EN_DI_REQUEST     |0x00 |Enable Data In Request interrupt |
    +-----+---+----------------------+-----+---------------------------------+
    |    6|R/W|INT_EN_DO_REQUEST     |0x00 |Enable Data Out Request interrupt|
    +-----+---+----------------------+-----+---------------------------------+
    |   29|R/W|INT_EN_RESEED_WARNING |0x00 |Enable Reseed Warning interrupt  |
    +-----+---+----------------------+-----+---------------------------------+
    |   30|R/W|INT_EN_RESEED_REQUIRED|0x00 |Enable Reseed Required interrupt |
    +-----+---+----------------------+-----+---------------------------------+
    |   31|R/W|INT_EN_LAB_TEST_MODE  |0x00 |Enable Lab Test Mode interrupt   |
    +-----+---+----------------------+-----+---------------------------------+

.. _quiddikey__ISR:

ISR
"""

Interrupt Status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------------+-----+------------------------------------------+
    |Bit #|R/W|       Name        |Reset|               Description                |
    +=====+===+===================+=====+==========================================+
    |    0|R/W|INT_BUSY           |0x00 |Busy interrupt status register            |
    +-----+---+-------------------+-----+------------------------------------------+
    |    1|R/W|INT_OK             |0x00 |Ok interrupt status register              |
    +-----+---+-------------------+-----+------------------------------------------+
    |    2|R/W|INT_ERROR          |0x00 |Error interrupt status register           |
    +-----+---+-------------------+-----+------------------------------------------+
    |    3|R/W|INT_ZEROIZED       |0x00 |Zeroized interrupt status register        |
    +-----+---+-------------------+-----+------------------------------------------+
    |    4|R/W|INT_REJECTED       |0x00 |Rejected interrupt status register        |
    +-----+---+-------------------+-----+------------------------------------------+
    |    5|R/W|INT_DI_REQUEST     |0x00 |Data In Request interrupt status register |
    +-----+---+-------------------+-----+------------------------------------------+
    |    6|R/W|INT_DO_REQUEST     |0x00 |Data Out Request interrupt status register|
    +-----+---+-------------------+-----+------------------------------------------+
    |   29|R/W|INT_RESEED_WARNING |0x00 |Reseed Warning interrupt status register  |
    +-----+---+-------------------+-----+------------------------------------------+
    |   30|R/W|INT_RESEED_REQUIRED|0x00 |Reseed Required interrupt status register |
    +-----+---+-------------------+-----+------------------------------------------+
    |   31|R/W|INT_LAB_TEST_MODE  |0x00 |Lab Test Mode interrupt status register   |
    +-----+---+-------------------+-----+------------------------------------------+

.. _quiddikey__DATA_DEST:

DATA_DEST
"""""""""

Destination Data register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------+-----+-----------------------------------+
    |Bit #|R/W|  Name  |Reset|            Description            |
    +=====+===+========+=====+===================================+
    |    0|R/W|DEST_DOR|0x00 |Data out register destination      |
    +-----+---+--------+-----+-----------------------------------+
    |    1|R/W|DEST_SO |0x00 |Secure Output interface destination|
    +-----+---+--------+-----+-----------------------------------+

.. _quiddikey__DIR:

DIR
"""

Data Input register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------+
    |Bit #|R/W|Name|Reset| Description |
    +=====+===+====+=====+=============+
    |31:0 |R/W|DI  |0x00 |Data In field|
    +-----+---+----+-----+-------------+

.. _quiddikey__DOR:

DOR
"""

Data Output register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+--------------+
    |Bit #|R/W|Name|Reset| Description  |
    +=====+===+====+=====+==============+
    |31:0 |R  |DOR |0x00 |Data Out field|
    +-----+---+----+-----+--------------+

.. _quiddikey__MISC:

MISC
""""

Miscellaneous register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+--------------+
    |Bit #|R/W|   Name   |Reset| Description  |
    +=====+===+==========+=====+==============+
    |31:0 |R  |ENDIANNESS|0x00 |Set endianness|
    +-----+---+----------+-----+--------------+

.. _quiddikey__IF_SR:

IF_SR
"""""

Interface Status register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+------------------------+
    |Bit #|R/W|  Name   |Reset|      Description       |
    +=====+===+=========+=====+========================+
    |    0|R/W|APB_ERROR|0x00 |An APB error has occured|
    +-----+---+---------+-----+------------------------+

.. _quiddikey__TEST:

TEST
""""

Test register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------+-----+-----------------------------------+
    |Bit #|R/W|    Name    |Reset|            Description            |
    +=====+===+============+=====+===================================+
    |    0|R/W|BIST_ENABLE |0x00 |Isolates Quiddikey and runs BIST   |
    +-----+---+------------+-----+-----------------------------------+
    |    4|R/W|BIST_RUNNING|0x00 |BIST is in progress or finishing up|
    +-----+---+------------+-----+-----------------------------------+
    |    5|R/W|BIST_ACTIVE |0x00 |BIST is in progress                |
    +-----+---+------------+-----+-----------------------------------+
    |    6|R/W|BIST_OK     |0x00 |BIST has passed                    |
    +-----+---+------------+-----+-----------------------------------+
    |    7|R/W|BIST_ERROR  |0x00 |BIST has failed                    |
    +-----+---+------------+-----+-----------------------------------+
    |   31|R/W|ALLOW_BIST  |0x00 |BIST is not allowed                |
    +-----+---+------------+-----+-----------------------------------+

.. _quiddikey__PSR:

PSR
"""

PUF Score register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+---------------+
    |Bit #|R/W|  Name   |Reset|  Description  |
    +=====+===+=========+=====+===============+
    |3:0  |R/W|PUF_SCORE|0x00 |PUF Score field|
    +-----+---+---------+-----+---------------+

.. _quiddikey__HW_RUC0:

HW_RUC0
"""""""

Hardware Restrict User Context 0 register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------------+-----+-----------------------------+
    |Bit #|R/W|         Name          |Reset|         Description         |
    +=====+===+=======================+=====+=============================+
    |31:0 |R/W|RESTRICT_USER_CONTEXT_0|0x00 |Restrict User Context 0 field|
    +-----+---+-----------------------+-----+-----------------------------+

.. _quiddikey__HW_RUC1:

HW_RUC1
"""""""

Hardware Restrict User Context 1 register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------------------+-----+-----------------------------+
    |Bit #|R/W|         Name          |Reset|         Description         |
    +=====+===+=======================+=====+=============================+
    |31:0 |R/W|RESTRICT_USER_CONTEXT_1|0x00 |Restrict User Context 1 field|
    +-----+---+-----------------------+-----+-----------------------------+

.. _quiddikey__HW_SETTINGS:

HW_SETTINGS
"""""""""""

Hardware Settings register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+--------------------------+-----+------------------------------------+
    |Bit #|R/W|           Name           |Reset|            Description             |
    +=====+===+==========================+=====+====================================+
    |    1|R/W|DISABLE_ENROLL            |0x00 |Enroll settings field               |
    +-----+---+--------------------------+-----+------------------------------------+
    |    2|R/W|DISABLE_START             |0x00 |Start settings field                |
    +-----+---+--------------------------+-----+------------------------------------+
    |    5|R/W|DISABLE_STOP              |0x00 |Stop settings field                 |
    +-----+---+--------------------------+-----+------------------------------------+
    |    6|R/W|DISABLE_GET_KEY           |0x00 |Get Key settings field              |
    +-----+---+--------------------------+-----+------------------------------------+
    |    7|R/W|DISABLE_UNWRAP            |0x00 |Unwrap settings field               |
    +-----+---+--------------------------+-----+------------------------------------+
    |    8|R/W|DISABLE_WRAP_GEN_RND      |0x00 |Wrap Generated Random settings field|
    +-----+---+--------------------------+-----+------------------------------------+
    |    9|R/W|DISABLE_WRAP              |0x00 |Wrap settings field                 |
    +-----+---+--------------------------+-----+------------------------------------+
    |   15|R/W|DISABLE_GEN_RND           |0x00 |Generate Random settings field      |
    +-----+---+--------------------------+-----+------------------------------------+
    |   16|R/W|DISABLE_RESEED            |0x00 |Reseed settings field               |
    +-----+---+--------------------------+-----+------------------------------------+
    |   24|R/W|DISABLE_LAB_TEST_MODE     |0x00 |Lab Test Mode settings field        |
    +-----+---+--------------------------+-----+------------------------------------+
    |   25|R/W|SELECT_LAB_TEST_MODE      |0x00 |Lab Test Mode select field          |
    +-----+---+--------------------------+-----+------------------------------------+
    |   27|R/W|REQUIRE_RESEED_SRC_VIA_DIR|0x00 |Reseed via DIR settings field       |
    +-----+---+--------------------------+-----+------------------------------------+
    |   28|R/W|REQUIRE_RESEED_SRC_VIA_SI |0x00 |Reseed via SI settings field        |
    +-----+---+--------------------------+-----+------------------------------------+
    |   31|R/W|DISABLE_TEST_PUF          |0x00 |Test PUF settings field             |
    +-----+---+--------------------------+-----+------------------------------------+

.. _quiddikey__HW_INFO:

HW_INFO
"""""""

Hardware Information register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------------+-----+-----------------------------------------+
    |Bit #|R/W|      Name      |Reset|               Description               |
    +=====+===+================+=====+=========================================+
    |   21|R/W|CONFIG_SP_800_90|0x00 |1: SP 800-90 is included, 0: not included|
    +-----+---+----------------+-----+-----------------------------------------+
    |   22|R/W|CONFIG_BIST     |0x00 |1: BIST is included, 0: not included     |
    +-----+---+----------------+-----+-----------------------------------------+
    |   23|R/W|RESERVED        |0x00 |1: Safe, 0: Plus                         |
    +-----+---+----------------+-----+-----------------------------------------+
    |   24|R/W|CONFIG_WRAP     |0x00 |1: Wrap is included, 0: not included     |
    +-----+---+----------------+-----+-----------------------------------------+
    |31:28|R/W|CONFIG_TYPE     |0x00 |Quiddikey configuration                  |
    +-----+---+----------------+-----+-----------------------------------------+

.. _quiddikey__HW_ID:

HW_ID
"""""

Hardware Identifier register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+-------------------+
    |Bit #|R/W|Name |Reset|    Description    |
    +=====+===+=====+=====+===================+
    |31:0 |R/W|HW_ID|0x00 |Hardware Identifier|
    +-----+---+-----+-----+-------------------+

.. _quiddikey__HW_VER:

HW_VER
""""""

Hardware Version register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------------+-----+-------------------------------+
    |Bit #|R/W|    Name    |Reset|          Description          |
    +=====+===+============+=====+===============================+
    |7:0  |R/W|HW_VER_REV  |0x00 |Hardware version, revision part|
    +-----+---+------------+-----+-------------------------------+
    |15:8 |R/W|HW_VER_MINOR|0x00 |Hardware version, minor part   |
    +-----+---+------------+-----+-------------------------------+
    |23:16|R/W|HW_VER_MAJOR|0x00 |Hardware version, major part   |
    +-----+---+------------+-----+-------------------------------+
