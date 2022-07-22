.. 
   Input file: fe/ips/apb_interrupt_cntrl/docs/FC_ITC_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +-----------------------------------------+------+-----+-------------------------------+
    |                  Name                   |Offset|Width|          Description          |
    +=========================================+======+=====+===============================+
    |:ref:`MASK<fc_itc__MASK>`                |0x0   |   32|Mask status register.          |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`MASK_SET<fc_itc__MASK_SET>`        |0x4   |   32|Mask set register.             |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`MASK_CLEAR<fc_itc__MASK_CLEAR>`    |0x8   |   32|Mask clear register.           |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`STATUS<fc_itc__STATUS>`            |0xC   |   32|Interrupt status register.     |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`STATUS_SET<fc_itc__STATUS_SET>`    |0x10  |   32|Interrupt enable register.     |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`STATUS_CLEAR<fc_itc__STATUS_CLEAR>`|0x14  |   32|Interrupt clear register.      |
    +-----------------------------------------+------+-----+-------------------------------+
    |:ref:`FIFO<fc_itc__FIFO>`                |0x24  |   32|Read access to SoC events FIFO.|
    +-----------------------------------------+------+-----+-------------------------------+

.. _fc_itc__MASK:

MASK
""""

Mask status register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------------------+
    |Bit #|R/W| Name  |Reset|                    Description                     |
    +=====+===+=======+=====+====================================================+
    |31:0 |R/W|IT_MASK|0x0  |Interrupt mask: set bit i to 1 to enable interrupt i|
    +-----+---+-------+-----+----------------------------------------------------+

.. _fc_itc__MASK_SET:

MASK_SET
""""""""

Mask set register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+----------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                 Description                  |
    +=====+===+===========+=====+==============================================+
    |31:0 |W  |IT_MASK_SET|0x0  |Write 1 to bit i to set bit i of MASK register|
    +-----+---+-----------+-----+----------------------------------------------+

.. _fc_itc__MASK_CLEAR:

MASK_CLEAR
""""""""""

Mask clear register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                  Description                   |
    +=====+===+=============+=====+================================================+
    |31:0 |W  |IT_MASK_CLEAR|0x0  |Write 1 to bit i to clear bit i of MASK register|
    +-----+---+-------------+-----+------------------------------------------------+

.. _fc_itc__STATUS:

STATUS
""""""

Interrupt status register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                      Description                      |
    +=====+===+=========+=====+=======================================================+
    |31:0 |R/W|IT_STATUS|0x0  |Interrupt status: bit i is 1 when interrupt i is active|
    +-----+---+---------+-----+-------------------------------------------------------+

.. _fc_itc__STATUS_SET:

STATUS_SET
""""""""""

Interrupt enable register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                  Description                   |
    +=====+===+=============+=====+================================================+
    |31:0 |W  |IT_STATUS_SET|0x0  |Write 1 to bit i to set bit i of STATUS register|
    +-----+---+-------------+-----+------------------------------------------------+

.. _fc_itc__STATUS_CLEAR:

STATUS_CLEAR
""""""""""""

Interrupt clear register.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------------+-----+--------------------------------------------------+
    |Bit #|R/W|     Name      |Reset|                   Description                    |
    +=====+===+===============+=====+==================================================+
    |31:0 |W  |IT_STATUS_CLEAR|0x0  |Write 1 to bit i to clear bit i of STATUS register|
    +-----+---+---------------+-----+--------------------------------------------------+

.. _fc_itc__FIFO:

FIFO
""""

Read access to SoC events FIFO.

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+-----------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                                 Description                                 |
    +=====+===+=========+=====+=============================================================================+
    |31:0 |R  |EVENT_NUM|0x0  |Reading this field pops an event from SoC event FIFO and returns the event ID|
    +-----+---+---------+-----+-----------------------------------------------------------------------------+
