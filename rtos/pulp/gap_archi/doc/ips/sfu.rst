Input file: fe/ips/udma/udma_anc/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------+------+-----+-----------------------------------+
    |                   Name                    |Offset|Width|            Description            |
    +===========================================+======+=====+===================================+
    |:ref:`GRAPH_PTR<sfu_GRAPH_PTR>`            |     0|   32|Pointer to GRAPH configuration     |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`GRAPH_CMD<sfu_GRAPH_CMD>`            |     4|   32|GRAPH command register             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLOCK_PTR<sfu_CLOCK_PTR>`            |     8|   32|Pointer to CLOCK configuration     |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLOCK_CMD<sfu_CLOCK_CMD>`            |    12|   32|CLOCK command register             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`SFU_STATUS<sfu_SFU_STATUS>`          |    16|   32|Status of graph and clocks commands|
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_STATUS<sfu_MEM_IN_STATUS>`    |    20|   32|Status of memory IN interfaces     |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`LIMITER_MUTE<sfu_LIMITER_MUTE>`      |    24|   32|Limiter mute/unmute control        |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`VOLUME_INDEX<sfu_VOLUME_INDEX>`      |    28|   32|Control of Mute/Unmute             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`VOLUME_VALUE<sfu_VOLUME_VALUE>`      |    32|   32|Control of linear Volume           |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLK_MONITOR_0<sfu_CLK_MONITOR_0>`    |    36|   32|Control of clock monitors 0..3     |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`CLK_MONITOR_1<sfu_CLK_MONITOR_1>`    |    40|   32|Control of clock monitors 4..7     |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`OUT_MUTE<sfu_OUT_MUTE>`              |    44|   32|Mutes corresponding output channel |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_0<sfu_AUDIO_CLK_CFG_0>`|    48|   32|Control Audio clock generator 0    |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_1<sfu_AUDIO_CLK_CFG_1>`|    52|   32|Control Audio clock generator 1    |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_2<sfu_AUDIO_CLK_CFG_2>`|    56|   32|Control Audio clock generator 2    |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`AUDIO_CLK_CFG_3<sfu_AUDIO_CLK_CFG_3>`|    60|   32|Control Audio clock generator 3    |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_0<sfu_ASRC_RATIO_0>`      |    64|   32|ASRC0 conversion ratio             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_1<sfu_ASRC_RATIO_1>`      |    68|   32|ASRC1 conversion ratio             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`ASRC_RATIO_2<sfu_ASRC_RATIO_2>`      |    72|   32|ASRC2 conversion ratio             |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_0_CNT<sfu_MEM_IN_0_CNT>`      |    88|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_1_CNT<sfu_MEM_IN_1_CNT>`      |    92|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_2_CNT<sfu_MEM_IN_2_CNT>`      |    96|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_3_CNT<sfu_MEM_IN_3_CNT>`      |   100|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_4_CNT<sfu_MEM_IN_4_CNT>`      |   104|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_5_CNT<sfu_MEM_IN_5_CNT>`      |   108|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_6_CNT<sfu_MEM_IN_6_CNT>`      |   112|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+
    |:ref:`MEM_IN_7_CNT<sfu_MEM_IN_7_CNT>`      |   116|   32|Memory input counter               |
    +-------------------------------------------+------+-----+-----------------------------------+

.. _sfu_GRAPH_PTR:

GRAPH_PTR
"""""""""

Pointer to GRAPH configuration

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _sfu_GRAPH_CMD:

GRAPH_CMD
"""""""""

GRAPH command register

.. table:: 

    +-----+---+-----------+--------------------------------------------+
    |Bit #|R/W|   Name    |                Description                 |
    +=====+===+===========+============================================+
    |    0|W  |LOAD       |Starts GRAPH load                           |
    +-----+---+-----------+--------------------------------------------+
    |    1|W  |RECONF     |Starts GRAPH reconf                         |
    +-----+---+-----------+--------------------------------------------+
    |    2|W  |UNLOAD     |Starts GRAPH unload                         |
    +-----+---+-----------+--------------------------------------------+
    |    3|W  |SAVE       |Starts GRAPH save                           |
    +-----+---+-----------+--------------------------------------------+
    |    4|W  |SET_CURRENT|Sets the current graph(used for status read)|
    +-----+---+-----------+--------------------------------------------+

.. _sfu_CLOCK_PTR:

CLOCK_PTR
"""""""""

Pointer to CLOCK configuration

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

.. _sfu_CLOCK_CMD:

CLOCK_CMD
"""""""""

CLOCK command register

.. table:: 

    +-----+---+------+-------------------+
    |Bit #|R/W| Name |    Description    |
    +=====+===+======+===================+
    |    0|W  |LOAD  |Starts CLOCK load  |
    +-----+---+------+-------------------+
    |    1|W  |UNLOAD|Starts CLOCK unload|
    +-----+---+------+-------------------+

.. _sfu_SFU_STATUS:

SFU_STATUS
""""""""""

Status of graph and clocks commands

.. table:: 

    +-----+---+-------------+-----------------------------------------+
    |Bit #|R/W|    Name     |               Description               |
    +=====+===+=============+=========================================+
    |    0|R  |CLOCK_LOAD   |When set load is ongoing                 |
    +-----+---+-------------+-----------------------------------------+
    |    1|R  |GRAPH_LOAD   |When set graph load is ongoing           |
    +-----+---+-------------+-----------------------------------------+
    |    2|R  |GRAPH_UNLOAD |When set graph unload is ongoing         |
    +-----+---+-------------+-----------------------------------------+
    |    3|R  |GRAPH_RECONF |When set graph reconfiguration is ongoing|
    +-----+---+-------------+-----------------------------------------+
    |    4|R  |GRAPH_SAVE   |When set graph save is ongoing           |
    +-----+---+-------------+-----------------------------------------+
    |    5|R  |GRAPH_SET_CUR|When set graph set current is ongoing    |
    +-----+---+-------------+-----------------------------------------+
    |8:6  |R  |ASRC_LOCK    |Lock status of the 3 ASRCs               |
    +-----+---+-------------+-----------------------------------------+
    |    9|R  |GRAPH_BUSY   |When high current graph is busy          |
    +-----+---+-------------+-----------------------------------------+

.. _sfu_MEM_IN_STATUS:

MEM_IN_STATUS
"""""""""""""

Status of memory IN interfaces

.. table:: 

    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                        Description                                                        |
    +=====+===+======+===========================================================================================================================+
    |7:0  |R/W|STATUS|When reading 1:b0 mem_in ok 1:b1 buffer is ended. When writing 1'b1: restart the mem in interface(ex. after buffer restart)|
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------+

.. _sfu_LIMITER_MUTE:

LIMITER_MUTE
""""""""""""

Limiter mute/unmute control

.. table:: 

    +-----+---+----------+--------------------------------------+
    |Bit #|R/W|   Name   |             Description              |
    +=====+===+==========+======================================+
    |    0|R/W|MUTE_LIM_0|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+
    |    1|R/W|MUTE_LIM_1|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+
    |    2|R/W|MUTE_LIM_2|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+
    |    3|R/W|MUTE_LIM_3|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+
    |    4|R/W|MUTE_LIM_4|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+
    |    5|R/W|MUTE_LIM_5|1'b1: Enables Mute 1'b0: Disables Mute|
    +-----+---+----------+--------------------------------------+

.. _sfu_VOLUME_INDEX:

VOLUME_INDEX
""""""""""""

Control of Mute/Unmute

.. table:: 

    +-----+---+-----+-------------------------------------------------+
    |Bit #|R/W|Name |                   Description                   |
    +=====+===+=====+=================================================+
    |4:0  |R/W|INDEX|index of volume accessed by VOLUME_VALUE register|
    +-----+---+-----+-------------------------------------------------+

.. _sfu_VOLUME_VALUE:

VOLUME_VALUE
""""""""""""

Control of linear Volume

.. table:: 

    +-----+---+---------+------------------------------------------------------+
    |Bit #|R/W|  Name   |                     Description                      |
    +=====+===+=========+======================================================+
    |25:0 |R/W|VOLUME   |Value of volume(linear)                               |
    +-----+---+---------+------------------------------------------------------+
    |31:26|R/W|SCALING_V|Value in bits for the scaling (bit 5 is the direction)|
    +-----+---+---------+------------------------------------------------------+

.. _sfu_CLK_MONITOR_0:

CLK_MONITOR_0
"""""""""""""

Control of clock monitors 0..3

.. table:: 

    +-----+---+----+------------------------+
    |Bit #|R/W|Name|      Description       |
    +=====+===+====+========================+
    |4:0  |R/W|SEL0|Monitored clock selector|
    +-----+---+----+------------------------+
    |7    |R/W|EN0 |Enable monitor          |
    +-----+---+----+------------------------+
    |12:8 |R/W|SEL1|Monitored clock selector|
    +-----+---+----+------------------------+
    |15   |R/W|EN1 |Enable monitor          |
    +-----+---+----+------------------------+
    |20:16|R/W|SEL2|Monitored clock selector|
    +-----+---+----+------------------------+
    |23   |R/W|EN2 |Enable moniotor         |
    +-----+---+----+------------------------+
    |28:24|R/W|SEL3|Monitored clock selector|
    +-----+---+----+------------------------+
    |31   |R/W|EN3 |Enable monitor          |
    +-----+---+----+------------------------+

.. _sfu_CLK_MONITOR_1:

CLK_MONITOR_1
"""""""""""""

Control of clock monitors 4..7

.. table:: 

    +-----+---+----+------------------------+
    |Bit #|R/W|Name|      Description       |
    +=====+===+====+========================+
    |4:0  |R/W|SEL0|Monitored clock selector|
    +-----+---+----+------------------------+
    |7    |R/W|EN0 |Enable monitor          |
    +-----+---+----+------------------------+
    |12:8 |R/W|SEL1|Monitored clock selector|
    +-----+---+----+------------------------+
    |15   |R/W|EN1 |Enable monitor          |
    +-----+---+----+------------------------+
    |20:16|R/W|SEL2|Monitored clock selector|
    +-----+---+----+------------------------+
    |23   |R/W|EN2 |Enable moniotor         |
    +-----+---+----+------------------------+
    |28:24|R/W|SEL3|Monitored clock selector|
    +-----+---+----+------------------------+
    |31   |R/W|EN3 |Enable monitor          |
    +-----+---+----+------------------------+

.. _sfu_OUT_MUTE:

OUT_MUTE
""""""""

Mutes corresponding output channel

.. table:: 

    +-----+---+----------+--------------------------------------+
    |Bit #|R/W|   Name   |             Description              |
    +=====+===+==========+======================================+
    |7:0  |R/W|MEM_OUT   |Mutes corresponding MEM_OUT channel   |
    +-----+---+----------+--------------------------------------+
    |15:8 |R/W|STREAM_OUT|Mutes corresponding STREAM_OUT channel|
    +-----+---+----------+--------------------------------------+
    |18:16|R/W|PDM_OUT   |Mutes corresponding PDM_OUT channel   |
    +-----+---+----------+--------------------------------------+

.. _sfu_AUDIO_CLK_CFG_0:

AUDIO_CLK_CFG_0
"""""""""""""""

Control Audio clock generator 0

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |15:0 |R/W|DIV |DIVision factor for audio clock|
    +-----+---+----+-------------------------------+
    |16   |R/W|EN  |Enable audio clock             |
    +-----+---+----+-------------------------------+

.. _sfu_AUDIO_CLK_CFG_1:

AUDIO_CLK_CFG_1
"""""""""""""""

Control Audio clock generator 1

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |15:0 |R/W|DIV |DIVision factor for audio clock|
    +-----+---+----+-------------------------------+
    |16   |R/W|EN  |Enable audio clock             |
    +-----+---+----+-------------------------------+

.. _sfu_AUDIO_CLK_CFG_2:

AUDIO_CLK_CFG_2
"""""""""""""""

Control Audio clock generator 2

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |15:0 |R/W|DIV |DIVision factor for audio clock|
    +-----+---+----+-------------------------------+
    |16   |R/W|EN  |Enable audio clock             |
    +-----+---+----+-------------------------------+

.. _sfu_AUDIO_CLK_CFG_3:

AUDIO_CLK_CFG_3
"""""""""""""""

Control Audio clock generator 3

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |15:0 |R/W|DIV |DIVision factor for audio clock|
    +-----+---+----+-------------------------------+
    |16   |R/W|EN  |Enable audio clock             |
    +-----+---+----+-------------------------------+

.. _sfu_ASRC_RATIO_0:

ASRC_RATIO_0
""""""""""""

ASRC0 conversion ratio

.. table:: 

    +-----+---+-----+----------------+
    |Bit #|R/W|Name |  Description   |
    +=====+===+=====+================+
    |25:0 |R/W|RATIO|Conversion RATIO|
    +-----+---+-----+----------------+

.. _sfu_ASRC_RATIO_1:

ASRC_RATIO_1
""""""""""""

ASRC1 conversion ratio

.. table:: 

    +-----+---+-----+----------------+
    |Bit #|R/W|Name |  Description   |
    +=====+===+=====+================+
    |25:0 |R/W|RATIO|Conversion RATIO|
    +-----+---+-----+----------------+

.. _sfu_ASRC_RATIO_2:

ASRC_RATIO_2
""""""""""""

ASRC2 conversion ratio

.. table:: 

    +-----+---+-----+----------------+
    |Bit #|R/W|Name |  Description   |
    +=====+===+=====+================+
    |25:0 |R/W|RATIO|Conversion RATIO|
    +-----+---+-----+----------------+

.. _sfu_MEM_IN_0_CNT:

MEM_IN_0_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_1_CNT:

MEM_IN_1_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_2_CNT:

MEM_IN_2_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_3_CNT:

MEM_IN_3_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_4_CNT:

MEM_IN_4_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_5_CNT:

MEM_IN_5_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_6_CNT:

MEM_IN_6_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+

.. _sfu_MEM_IN_7_CNT:

MEM_IN_7_CNT
""""""""""""

Memory input counter

.. table:: 

    +-----+---+----+----------------------------------------------------+
    |Bit #|R/W|Name|                    Description                     |
    +=====+===+====+====================================================+
    |20:0 |R  |CNT |Reports how many samples have been pushed to the SFU|
    +-----+---+----+----------------------------------------------------+
