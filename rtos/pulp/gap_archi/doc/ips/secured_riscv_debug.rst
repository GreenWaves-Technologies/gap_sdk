.. 
   Input file: fe/ips/riscv_gwt/doc/SECURED_RI5CY_DEBUG_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |                       Name                        |Offset|Width|                            Description                            |
    +===================================================+======+=====+===================================================================+
    |:ref:`CTRL<secured_riscv_debug_CTRL>`              |     0|   32|Debug control configuration register.                              |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`HIT<secured_riscv_debug_HIT>`                |     4|   32|Debug hit status register.                                         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`IE<secured_riscv_debug_IE>`                  |     8|   32|Debug exception trap enable configuration register.                |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CAUSE<secured_riscv_debug_CAUSE>`            |    12|   32|Debug trap cause status register.                                  |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`NPC<secured_riscv_debug_NPC>`                |  8192|   32|Debug next program counter value register.                         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`PPC<secured_riscv_debug_PPC>`                |  8196|   32|Debug previous program counter value register.                     |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR0<secured_riscv_debug_GPR0>`              |  1024|   32|Core general purpose register 0 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR1<secured_riscv_debug_GPR1>`              |  1028|   32|Core general purpose register 1 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR2<secured_riscv_debug_GPR2>`              |  1032|   32|Core general purpose register 2 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR3<secured_riscv_debug_GPR3>`              |  1036|   32|Core general purpose register 3 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR4<secured_riscv_debug_GPR4>`              |  1040|   32|Core general purpose register 4 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR5<secured_riscv_debug_GPR5>`              |  1044|   32|Core general purpose register 5 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR6<secured_riscv_debug_GPR6>`              |  1048|   32|Core general purpose register 6 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR7<secured_riscv_debug_GPR7>`              |  1052|   32|Core general purpose register 7 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR8<secured_riscv_debug_GPR8>`              |  1056|   32|Core general purpose register 8 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR9<secured_riscv_debug_GPR9>`              |  1060|   32|Core general purpose register 9 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR10<secured_riscv_debug_GPR10>`            |  1064|   32|Core general purpose register 10 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR11<secured_riscv_debug_GPR11>`            |  1068|   32|Core general purpose register 11 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR12<secured_riscv_debug_GPR12>`            |  1072|   32|Core general purpose register 12 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR13<secured_riscv_debug_GPR13>`            |  1076|   32|Core general purpose register 13 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR14<secured_riscv_debug_GPR14>`            |  1080|   32|Core general purpose register 14 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR15<secured_riscv_debug_GPR15>`            |  1084|   32|Core general purpose register 15 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR16<secured_riscv_debug_GPR16>`            |  1088|   32|Core general purpose register 16 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR17<secured_riscv_debug_GPR17>`            |  1092|   32|Core general purpose register 17 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR18<secured_riscv_debug_GPR18>`            |  1096|   32|Core general purpose register 18 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR19<secured_riscv_debug_GPR19>`            |  1100|   32|Core general purpose register 19 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR20<secured_riscv_debug_GPR20>`            |  1104|   32|Core general purpose register 20 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR21<secured_riscv_debug_GPR21>`            |  1108|   32|Core general purpose register 21 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR22<secured_riscv_debug_GPR22>`            |  1112|   32|Core general purpose register 22 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR23<secured_riscv_debug_GPR23>`            |  1116|   32|Core general purpose register 23 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR24<secured_riscv_debug_GPR24>`            |  1120|   32|Core general purpose register 24 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR25<secured_riscv_debug_GPR25>`            |  1124|   32|Core general purpose register 25 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR26<secured_riscv_debug_GPR26>`            |  1128|   32|Core general purpose register 26 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR27<secured_riscv_debug_GPR27>`            |  1132|   32|Core general purpose register 27 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR28<secured_riscv_debug_GPR28>`            |  1136|   32|Core general purpose register 28 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR29<secured_riscv_debug_GPR29>`            |  1140|   32|Core general purpose register 29 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR30<secured_riscv_debug_GPR30>`            |  1144|   32|Core general purpose register 30 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR31<secured_riscv_debug_GPR31>`            |  1148|   32|Core general purpose register 31 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MSTATUS<secured_riscv_debug_CSR_MSTATUS>`| 19456|   32|Core CSR machine status value register.                            |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MTVEC<secured_riscv_debug_CSR_MTVEC>`    | 19476|   32|Core CSR machine vector-trap base address value register.          |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MEPC<secured_riscv_debug_CSR_MEPC>`      | 19716|   32|Core CSR machine exception program counter value register.         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MCAUSE<secured_riscv_debug_CSR_MCAUSE>`  | 19720|   32|Core CSR machine trap cause value register.                        |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCCR<secured_riscv_debug_CSR_PCCR>`      | 24064|   32|Core CSR performance counter counter register.                     |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCER<secured_riscv_debug_CSR_PCER>`      | 24192|   32|Core CSR performance counter enable configuration register.        |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCMR<secured_riscv_debug_CSR_PCMR>`      | 24196|   32|Core CSR performance counter mode configuration register.          |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0S<secured_riscv_debug_CSR_HWLP0S>`  | 24256|   32|Core CSR hardware loop 0 start configuration register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0E<secured_riscv_debug_CSR_HWLP0E>`  | 24260|   32|Core CSR hardware loop 0 end configuration register.               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0C<secured_riscv_debug_CSR_HWLP0C>`  | 24264|   32|Core CSR hardware loop 0 counter configuration register.           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1S<secured_riscv_debug_CSR_HWLP1S>`  | 24272|   32|Core CSR hardware loop 1 start configuration register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1E<secured_riscv_debug_CSR_HWLP1E>`  | 24276|   32|Core CSR hardware loop 1 end configuration register.               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1C<secured_riscv_debug_CSR_HWLP1C>`  | 24280|   32|Core CSR hardware loop 1 counter configuration register.           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PRIVLV<secured_riscv_debug_CSR_PRIVLV>`  | 28736|   32|Cose CSR current privilege level status register.                  |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UHARTID<secured_riscv_debug_CSR_UHARTID>`| 16464|   32|Core CSR user privilege mode hardware thread ID status register.   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MHARTID<secured_riscv_debug_CSR_MHARTID>`| 31824|   32|Core CSR machine privilege mode hardware thread ID status register.|
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_USTATUS<secured_riscv_debug_CSR_USTATUS>`| 16404|   32|Core CSR user status value register.                               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UTVEC<secured_riscv_debug_CSR_UTVEC>`    | 16464|   32|Core CSR user vector-trap base address value register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UEPC<secured_riscv_debug_CSR_UEPC>`      | 16644|   32|Core CSR user exception program counter value register.            |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UCAUSE<secured_riscv_debug_CSR_UCAUSE>`  | 16648|   32|Core CSR user trap cause value register.                           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+

.. _secured_riscv_debug_CTRL:

CTRL
""""

Debug control configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_HIT:

HIT
"""

Debug hit status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_IE:

IE
""

Debug exception trap enable configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CAUSE:

CAUSE
"""""

Debug trap cause status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_NPC:

NPC
"""

Debug next program counter value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_PPC:

PPC
"""

Debug previous program counter value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR0:

GPR0
""""

Core general purpose register 0 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR1:

GPR1
""""

Core general purpose register 1 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR2:

GPR2
""""

Core general purpose register 2 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR3:

GPR3
""""

Core general purpose register 3 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR4:

GPR4
""""

Core general purpose register 4 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR5:

GPR5
""""

Core general purpose register 5 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR6:

GPR6
""""

Core general purpose register 6 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR7:

GPR7
""""

Core general purpose register 7 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR8:

GPR8
""""

Core general purpose register 8 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR9:

GPR9
""""

Core general purpose register 9 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR10:

GPR10
"""""

Core general purpose register 10 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR11:

GPR11
"""""

Core general purpose register 11 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR12:

GPR12
"""""

Core general purpose register 12 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR13:

GPR13
"""""

Core general purpose register 13 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR14:

GPR14
"""""

Core general purpose register 14 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR15:

GPR15
"""""

Core general purpose register 15 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR16:

GPR16
"""""

Core general purpose register 16 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR17:

GPR17
"""""

Core general purpose register 17 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR18:

GPR18
"""""

Core general purpose register 18 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR19:

GPR19
"""""

Core general purpose register 19 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR20:

GPR20
"""""

Core general purpose register 20 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR21:

GPR21
"""""

Core general purpose register 21 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR22:

GPR22
"""""

Core general purpose register 22 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR23:

GPR23
"""""

Core general purpose register 23 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR24:

GPR24
"""""

Core general purpose register 24 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR25:

GPR25
"""""

Core general purpose register 25 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR26:

GPR26
"""""

Core general purpose register 26 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR27:

GPR27
"""""

Core general purpose register 27 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR28:

GPR28
"""""

Core general purpose register 28 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR29:

GPR29
"""""

Core general purpose register 29 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR30:

GPR30
"""""

Core general purpose register 30 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_GPR31:

GPR31
"""""

Core general purpose register 31 value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_MSTATUS:

CSR_MSTATUS
"""""""""""

Core CSR machine status value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_MTVEC:

CSR_MTVEC
"""""""""

Core CSR machine vector-trap base address value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_MEPC:

CSR_MEPC
""""""""

Core CSR machine exception program counter value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_MCAUSE:

CSR_MCAUSE
""""""""""

Core CSR machine trap cause value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_PCCR:

CSR_PCCR
""""""""

Core CSR performance counter counter register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_PCER:

CSR_PCER
""""""""

Core CSR performance counter enable configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_PCMR:

CSR_PCMR
""""""""

Core CSR performance counter mode configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP0S:

CSR_HWLP0S
""""""""""

Core CSR hardware loop 0 start configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP0E:

CSR_HWLP0E
""""""""""

Core CSR hardware loop 0 end configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP0C:

CSR_HWLP0C
""""""""""

Core CSR hardware loop 0 counter configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP1S:

CSR_HWLP1S
""""""""""

Core CSR hardware loop 1 start configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP1E:

CSR_HWLP1E
""""""""""

Core CSR hardware loop 1 end configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_HWLP1C:

CSR_HWLP1C
""""""""""

Core CSR hardware loop 1 counter configuration register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_PRIVLV:

CSR_PRIVLV
""""""""""

Cose CSR current privilege level status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_UHARTID:

CSR_UHARTID
"""""""""""

Core CSR user privilege mode hardware thread ID status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_MHARTID:

CSR_MHARTID
"""""""""""

Core CSR machine privilege mode hardware thread ID status register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_USTATUS:

CSR_USTATUS
"""""""""""

Core CSR user status value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_UTVEC:

CSR_UTVEC
"""""""""

Core CSR user vector-trap base address value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_UEPC:

CSR_UEPC
""""""""

Core CSR user exception program counter value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _secured_riscv_debug_CSR_UCAUSE:

CSR_UCAUSE
""""""""""

Core CSR user trap cause value register.

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+
