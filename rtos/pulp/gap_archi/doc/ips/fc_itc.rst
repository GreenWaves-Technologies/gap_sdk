.. 
   Input file: fe/ips/apb_interrupt_cntrl/docs/FC_ITC_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +----------------------------------------+------+-----+-------------------------------------+
    |                  Name                  |Offset|Width|             Description             |
    +========================================+======+=====+=====================================+
    |:ref:`MASK<fc_itc_MASK>`                |     0|   32|FC_ITC Mask status register.         |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`MASK_SET<fc_itc_MASK_SET>`        |     4|   32|FC_ITC Mask set register.            |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`MASK_CLEAR<fc_itc_MASK_CLEAR>`    |     8|   32|FC_ITC Mask clear register.          |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`STATUS<fc_itc_STATUS>`            |    12|   32|FC_ITC interrupt status register.    |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`STATUS_SET<fc_itc_STATUS_SET>`    |    16|   32|FC_ITC interrupt enable register.    |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`STATUS_CLEAR<fc_itc_STATUS_CLEAR>`|    20|   32|FC_ITC interrupt clear register.     |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`ACK<fc_itc_ACK>`                  |    24|   32|FC_ITC interrupt ack status register.|
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`ACK_SET<fc_itc_ACK_SET>`          |    28|   32|FC_ITC interrupt ack set register.   |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`ACK_CLEAR<fc_itc_ACK_CLEAR>`      |    32|   32|FC_ITC interrupt ack clear register. |
    +----------------------------------------+------+-----+-------------------------------------+
    |:ref:`FIFO<fc_itc_FIFO>`                |    36|   32|FC_ITC event number fifo register.   |
    +----------------------------------------+------+-----+-------------------------------------+

.. _fc_itc_MASK:

MASK
""""

FC_ITC Mask status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_MASK_SET:

MASK_SET
""""""""

FC_ITC Mask set register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_MASK_CLEAR:

MASK_CLEAR
""""""""""

FC_ITC Mask clear register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_STATUS:

STATUS
""""""

FC_ITC interrupt status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_STATUS_SET:

STATUS_SET
""""""""""

FC_ITC interrupt enable register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_STATUS_CLEAR:

STATUS_CLEAR
""""""""""""

FC_ITC interrupt clear register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_ACK:

ACK
"""

FC_ITC interrupt ack status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_ACK_SET:

ACK_SET
"""""""

FC_ITC interrupt ack set register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_ACK_CLEAR:

ACK_CLEAR
"""""""""

FC_ITC interrupt ack clear register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _fc_itc_FIFO:

FIFO
""""

FC_ITC event number fifo register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
