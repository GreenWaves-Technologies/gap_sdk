.. 
   Input file: fe/ips/udma/udma_anc/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""


Refer to :ref:`GAP9 address map<REF_MEMORY_MAP_DETAIL>` for the base address to be used.

.. table:: 
    :align: center
    :widths: 40 12 12 90

    +--------------------------------------------+------+-----+-----------------------------------+
    |                    Name                    |Offset|Width|            Description            |
    +============================================+======+=====+===================================+
    |:ref:`GRAPH_PTR<sfu__GRAPH_PTR>`            |     0|   32|Pointer to graph configuration     |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`GRAPH_CMD<sfu__GRAPH_CMD>`            |     4|   32|Graph command register             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLOCK_PTR<sfu__CLOCK_PTR>`            |     8|   32|Pointer to clock configuration     |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLOCK_CMD<sfu__CLOCK_CMD>`            |    12|   32|Clock command register             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`SFU_STATUS<sfu__SFU_STATUS>`          |    16|   32|Status of graph and clocks commands|
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_STATUS<sfu__MEM_IN_STATUS>`    |    20|   32|Status of memory IN interfaces     |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`LIMITER_MUTE<sfu__LIMITER_MUTE>`      |    24|   32|Limiter mute/unmute control        |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`VOLUME_INDEX<sfu__VOLUME_INDEX>`      |    28|   32|Control of mute/unmute             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`VOLUME_VALUE<sfu__VOLUME_VALUE>`      |    32|   32|Control of linear volume           |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLK_MONITOR_0<sfu__CLK_MONITOR_0>`    |    36|   32|Control of clock monitors 0 to 3   |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLK_MONITOR_1<sfu__CLK_MONITOR_1>`    |    40|   32|Control of clock monitors 4 to 7   |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`OUT_MUTE<sfu__OUT_MUTE>`              |    44|   32|Control of output channel mute     |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_0<sfu__AUDIO_CLK_CFG_0>`|    48|   32|Control audio clock generator 0    |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_1<sfu__AUDIO_CLK_CFG_1>`|    52|   32|Control audio clock generator 1    |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_2<sfu__AUDIO_CLK_CFG_2>`|    56|   32|Control audio clock generator 2    |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_3<sfu__AUDIO_CLK_CFG_3>`|    60|   32|Control audio clock generator 3    |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_0<sfu__ASRC_RATIO_0>`      |    64|   32|ASRC0 conversion ratio             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_1<sfu__ASRC_RATIO_1>`      |    68|   32|ASRC1 conversion ratio             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_2<sfu__ASRC_RATIO_2>`      |    72|   32|ASRC2 conversion ratio             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_0_CNT<sfu__MEM_IN_0_CNT>`      |    88|   32|Memory input counter 0             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_1_CNT<sfu__MEM_IN_1_CNT>`      |    92|   32|Memory input counter 1             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_2_CNT<sfu__MEM_IN_2_CNT>`      |    96|   32|Memory input counter 2             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_3_CNT<sfu__MEM_IN_3_CNT>`      |   100|   32|Memory input counter 3             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_4_CNT<sfu__MEM_IN_4_CNT>`      |   104|   32|Memory input counter 4             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_5_CNT<sfu__MEM_IN_5_CNT>`      |   108|   32|Memory input counter 5             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_6_CNT<sfu__MEM_IN_6_CNT>`      |   112|   32|Memory input counter 6             |
    +--------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_7_CNT<sfu__MEM_IN_7_CNT>`      |   116|   32|Memory input counter 7             |
    +--------------------------------------------+------+-----+-----------------------------------+

.. _sfu__GRAPH_PTR:

GRAPH_PTR
"""""""""

Pointer to graph configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------+
    |Bit #|R/W| Name  |Reset|              Description               |
    +=====+===+=======+=====+========================================+
    |31:0 |R/W|ADDRESS|0x0  |Address of graph configuration in memory|
    +-----+---+-------+-----+----------------------------------------+

.. _sfu__GRAPH_CMD:

GRAPH_CMD
"""""""""

Graph command register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----------+-----+---------------------------------------------+
    |Bit #|R/W|   Name    |Reset|                 Description                 |
    +=====+===+===========+=====+=============================================+
    |    0|W  |LOAD       |0x0  |Write b1 to start graph load                 |
    +-----+---+-----------+-----+---------------------------------------------+
    |    1|W  |RECONF     |0x0  |Write b1 to start graph reconfiguration      |
    +-----+---+-----------+-----+---------------------------------------------+
    |    2|W  |UNLOAD     |0x0  |Write b1 to start graph unload               |
    +-----+---+-----------+-----+---------------------------------------------+
    |    3|W  |SAVE       |0x0  |Write b1 to start graph save                 |
    +-----+---+-----------+-----+---------------------------------------------+
    |    4|W  |SET_CURRENT|0x0  |Sets the current graph (used for status read)|
    +-----+---+-----------+-----+---------------------------------------------+

.. _sfu__CLOCK_PTR:

CLOCK_PTR
"""""""""

Pointer to clock configuration

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------+-----+----------------------------------------+
    |Bit #|R/W| Name  |Reset|              Description               |
    +=====+===+=======+=====+========================================+
    |31:0 |R/W|ADDRESS|0x0  |Address of clock configuration in memory|
    +-----+---+-------+-----+----------------------------------------+

.. _sfu__CLOCK_CMD:

CLOCK_CMD
"""""""""

Clock command register

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+------------------------------+
    |Bit #|R/W| Name |Reset|         Description          |
    +=====+===+======+=====+==============================+
    |    0|W  |LOAD  |0x0  |Write b1 to start clock load  |
    +-----+---+------+-----+------------------------------+
    |    1|W  |UNLOAD|0x0  |Write b1 to start clock unload|
    +-----+---+------+-----+------------------------------+

.. _sfu__SFU_STATUS:

SFU_STATUS
""""""""""

Status of graph and clocks commands

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |Reset|                                         Description                                          |
    +=====+===+=============+=====+==============================================================================================+
    |    0|R  |CLOCK_LOAD   |0x0  |Bit is set to 1 when clock load is ongoing                                                    |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    1|R  |GRAPH_LOAD   |0x0  |Bit is set to 1 when graph load is ongoing                                                    |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    2|R  |GRAPH_UNLOAD |0x0  |Bit is set to 1 when graph unload is ongoing                                                  |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    3|R  |GRAPH_RECONF |0x0  |Bit is set to 1 when graph reconfiguration is ongoing                                         |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    4|R  |GRAPH_SAVE   |0x0  |Bit is set to 1 when graph save is ongoing                                                    |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    5|R  |GRAPH_SET_CUR|0x0  |Bit is set to 1 when current graph is being set                                               |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |8:6  |R  |ASRC_LOCK    |0x0  |Lock status of the 3 ASRCs: bit *i* is set to 1 when frequency tracking of ASRC\ *i* is locked|
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+
    |    9|R  |GRAPH_BUSY   |0x0  |Bit is set to 1 when current graph is busy                                                    |
    +-----+---+-------------+-----+----------------------------------------------------------------------------------------------+

.. _sfu__MEM_IN_STATUS:

MEM_IN_STATUS
"""""""""""""

Status of memory IN interfaces

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |Reset|                                                                                      Description                                                                                       |
    +=====+===+======+=====+========================================================================================================================================================================================+
    |7:0  |R/W|STATUS|0x0  |When reading, bit *i* give the status of MemIn interface *i*: b0: interface OK; b1: buffer has ended. Writing b1 to bit *i* restarts the MemIn interface *i* (e.g. after buffer restart)|
    +-----+---+------+-----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

.. _sfu__LIMITER_MUTE:

LIMITER_MUTE
""""""""""""

Limiter mute/unmute control

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+--------------------------------------------------+
    |Bit #|R/W|   Name   |Reset|                   Description                    |
    +=====+===+==========+=====+==================================================+
    |    0|R/W|MUTE_LIM_0|0x0  |Enable mute 0: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+
    |    1|R/W|MUTE_LIM_1|0x0  |Enable mute 1: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+
    |    2|R/W|MUTE_LIM_2|0x0  |Enable mute 2: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+
    |    3|R/W|MUTE_LIM_3|0x0  |Enable mute 3: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+
    |    4|R/W|MUTE_LIM_4|0x0  |Enable mute 4: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+
    |    5|R/W|MUTE_LIM_5|0x0  |Enable mute 5: b0: mute disabled; b1: mute enabled|
    +-----+---+----------+-----+--------------------------------------------------+

.. _sfu__VOLUME_INDEX:

VOLUME_INDEX
""""""""""""

Control of mute/unmute

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+---------------------------------------------------------+
    |Bit #|R/W|Name |Reset|                       Description                       |
    +=====+===+=====+=====+=========================================================+
    |4:0  |R/W|INDEX|0x0  |Index of volume setting accessed by VOLUME_VALUE register|
    +-----+---+-----+-----+---------------------------------------------------------+

.. _sfu__VOLUME_VALUE:

VOLUME_VALUE
""""""""""""

Control of linear volume

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+---------+-----+------------------------------------------------------+
    |Bit #|R/W|  Name   |Reset|                     Description                      |
    +=====+===+=========+=====+======================================================+
    |25:0 |R/W|VOLUME   |0x0  |Value of volume (linear)                              |
    +-----+---+---------+-----+------------------------------------------------------+
    |31:26|R/W|SCALING_V|0x0  |Value in bits for the scaling (bit 5 is the direction)|
    +-----+---+---------+-----+------------------------------------------------------+

.. _sfu__CLK_MONITOR_0:

CLK_MONITOR_0
"""""""""""""

Control of clock monitors 0 to 3

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                      Description                      |
    +=====+===+====+=====+=======================================================+
    |4:0  |R/W|SEL0|0x0  |Monitored clock selector (see Clock select table below)|
    +-----+---+----+-----+-------------------------------------------------------+
    |7    |R/W|EN0 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |12:8 |R/W|SEL1|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |15   |R/W|EN1 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |20:16|R/W|SEL2|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |23   |R/W|EN2 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |28:24|R/W|SEL3|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |31   |R/W|EN3 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+

.. _sfu__CLK_MONITOR_1:

CLK_MONITOR_1
"""""""""""""

Control of clock monitors 4 to 7

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+-------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                      Description                      |
    +=====+===+====+=====+=======================================================+
    |4:0  |R/W|SEL0|0x0  |Monitored clock selector (see Clock select table below)|
    +-----+---+----+-----+-------------------------------------------------------+
    |7    |R/W|EN0 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |12:8 |R/W|SEL1|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |15   |R/W|EN1 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |20:16|R/W|SEL2|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |23   |R/W|EN2 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+
    |28:24|R/W|SEL3|0x0  |Monitored clock selector (see Clock select table below |
    +-----+---+----+-----+-------------------------------------------------------+
    |31   |R/W|EN3 |0x0  |Set to b1 to enable monitoring                         |
    +-----+---+----+-----+-------------------------------------------------------+

.. _sfu__OUT_MUTE:

OUT_MUTE
""""""""

Control of output channel mute

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----------+-----+-------------------------------------+
    |Bit #|R/W|   Name   |Reset|             Description             |
    +=====+===+==========+=====+=====================================+
    |7:0  |R/W|MEM_OUT   |0x0  |Mutes corresponding MemOut channel   |
    +-----+---+----------+-----+-------------------------------------+
    |15:8 |R/W|STREAM_OUT|0x0  |Mutes corresponding StreamOut channel|
    +-----+---+----------+-----+-------------------------------------+
    |18:16|R/W|PDM_OUT   |0x0  |Mutes corresponding PDMOut channel   |
    +-----+---+----------+-----+-------------------------------------+

.. _sfu__AUDIO_CLK_CFG_0:

AUDIO_CLK_CFG_0
"""""""""""""""

Control audio clock generator 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                       Description                       |
    +=====+===+====+=====+=========================================================+
    |15:0 |R/W|DIV |0x0  |Division factor for audio clock                          |
    +-----+---+----+-----+---------------------------------------------------------+
    |16   |R/W|EN  |0x0  |Enable: b0: audio clock disabled; b1: audio clock enabled|
    +-----+---+----+-----+---------------------------------------------------------+

.. _sfu__AUDIO_CLK_CFG_1:

AUDIO_CLK_CFG_1
"""""""""""""""

Control audio clock generator 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                       Description                       |
    +=====+===+====+=====+=========================================================+
    |15:0 |R/W|DIV |0x0  |Division factor for audio clock                          |
    +-----+---+----+-----+---------------------------------------------------------+
    |16   |R/W|EN  |0x0  |Enable: b0: audio clock disabled; b1: audio clock enabled|
    +-----+---+----+-----+---------------------------------------------------------+

.. _sfu__AUDIO_CLK_CFG_2:

AUDIO_CLK_CFG_2
"""""""""""""""

Control audio clock generator 2

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                       Description                       |
    +=====+===+====+=====+=========================================================+
    |15:0 |R/W|DIV |0x0  |Division factor for audio clock                          |
    +-----+---+----+-----+---------------------------------------------------------+
    |16   |R/W|EN  |0x0  |Enable: b0: audio clock disabled; b1: audio clock enabled|
    +-----+---+----+-----+---------------------------------------------------------+

.. _sfu__AUDIO_CLK_CFG_3:

AUDIO_CLK_CFG_3
"""""""""""""""

Control audio clock generator 3

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+---------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                       Description                       |
    +=====+===+====+=====+=========================================================+
    |15:0 |R/W|DIV |0x0  |Division factor for audio clock                          |
    +-----+---+----+-----+---------------------------------------------------------+
    |16   |R/W|EN  |0x0  |Enable: b0: audio clock disabled; b1: audio clock enabled|
    +-----+---+----+-----+---------------------------------------------------------+

.. _sfu__ASRC_RATIO_0:

ASRC_RATIO_0
""""""""""""

ASRC0 conversion ratio

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+----------------+
    |Bit #|R/W|Name |Reset|  Description   |
    +=====+===+=====+=====+================+
    |25:0 |R/W|RATIO|0x0  |Conversion ratio|
    +-----+---+-----+-----+----------------+

.. _sfu__ASRC_RATIO_1:

ASRC_RATIO_1
""""""""""""

ASRC1 conversion ratio

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+----------------+
    |Bit #|R/W|Name |Reset|  Description   |
    +=====+===+=====+=====+================+
    |25:0 |R/W|RATIO|0x0  |Conversion ratio|
    +-----+---+-----+-----+----------------+

.. _sfu__ASRC_RATIO_2:

ASRC_RATIO_2
""""""""""""

ASRC2 conversion ratio

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+-----+-----+----------------+
    |Bit #|R/W|Name |Reset|  Description   |
    +=====+===+=====+=====+================+
    |25:0 |R/W|RATIO|0x0  |Conversion ratio|
    +-----+---+-----+-----+----------------+

.. _sfu__MEM_IN_0_CNT:

MEM_IN_0_CNT
""""""""""""

Memory input counter 0

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_1_CNT:

MEM_IN_1_CNT
""""""""""""

Memory input counter 1

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_2_CNT:

MEM_IN_2_CNT
""""""""""""

Memory input counter 2

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_3_CNT:

MEM_IN_3_CNT
""""""""""""

Memory input counter 3

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_4_CNT:

MEM_IN_4_CNT
""""""""""""

Memory input counter 4

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_5_CNT:

MEM_IN_5_CNT
""""""""""""

Memory input counter 5

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_6_CNT:

MEM_IN_6_CNT
""""""""""""

Memory input counter 6

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+

.. _sfu__MEM_IN_7_CNT:

MEM_IN_7_CNT
""""""""""""

Memory input counter 7

.. table:: 
    :align: center
    :widths: 13 12 45 24 85

    +-----+---+----+-----+------------------------------------------------------------------------------+
    |Bit #|R/W|Name|Reset|                                 Description                                  |
    +=====+===+====+=====+==============================================================================+
    |20:0 |R  |CNT |0x0  |Reports how many samples have been pushed to the SFU from this MemIn interface|
    +-----+---+----+-----+------------------------------------------------------------------------------+
