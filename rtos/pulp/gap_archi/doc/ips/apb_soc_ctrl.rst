Input file: docs/IP_REFERENCES/APB_SOC_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |                              Name                              |Offset|Width|                    Description                    |
    +================================================================+======+=====+===================================================+
    |:ref:`INFO<apb_soc_ctrl_INFO>`                                  |     0|   32|Core information register.                         |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`FC_BOOT<apb_soc_ctrl_FC_BOOT>`                            |     4|   32|Boot address                                       |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`FC_FETCH<apb_soc_ctrl_FC_FETCH>`                          |     8|   32|FC Fetch enable                                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CL_ISOLATE<apb_soc_ctrl_CL_ISOLATE>`                      |    12|   32|Isolate cluster register                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN0<apb_soc_ctrl_PADFUN0>`                            |    16|   32|Mux config register (pad 0-15)                     |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN1<apb_soc_ctrl_PADFUN1>`                            |    20|   32|Mux config register (pad 16-31)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN2<apb_soc_ctrl_PADFUN2>`                            |    24|   32|Mux config register (pad 32-47)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN3<apb_soc_ctrl_PADFUN3>`                            |    28|   32|Mux config register (pad 48-63)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN4<apb_soc_ctrl_PADFUN4>`                            |    32|   32|Mux config register (pad 64-79)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADFUN5<apb_soc_ctrl_PADFUN5>`                            |    36|   32|Mux config register (pad 80-95)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED0<apb_soc_ctrl_RESERVED0>`                        |    40|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED1<apb_soc_ctrl_RESERVED1>`                        |    44|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG0<apb_soc_ctrl_PADCFG0>`                            |    48|   32|Function register (pad 0 to 3)                     |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG1<apb_soc_ctrl_PADCFG1>`                            |    52|   32|Function register (pad 4 to 7)                     |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG2<apb_soc_ctrl_PADCFG2>`                            |    56|   32|Function register (pad 8 to 11)                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG3<apb_soc_ctrl_PADCFG3>`                            |    60|   32|Function register (pad 12 to 15)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG4<apb_soc_ctrl_PADCFG4>`                            |    64|   32|Function register (pad 16 to 19)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG5<apb_soc_ctrl_PADCFG5>`                            |    68|   32|Function register (pad 20 to 23)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG6<apb_soc_ctrl_PADCFG6>`                            |    72|   32|Function register (pad 24 to 27)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG7<apb_soc_ctrl_PADCFG7>`                            |    76|   32|Function register (pad 28 to 31)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG8<apb_soc_ctrl_PADCFG8>`                            |    80|   32|Function register (pad 32 to 35)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG9<apb_soc_ctrl_PADCFG9>`                            |    84|   32|Function register (pad 36 to 39)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG10<apb_soc_ctrl_PADCFG10>`                          |    88|   32|Function register (pad 40 to 43)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG11<apb_soc_ctrl_PADCFG11>`                          |    92|   32|Function register (pad 44 to 47)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG12<apb_soc_ctrl_PADCFG12>`                          |    96|   32|Function register (pad 48 to 51)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG13<apb_soc_ctrl_PADCFG13>`                          |   100|   32|Function register (pad 52 to 55)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG14<apb_soc_ctrl_PADCFG14>`                          |   104|   32|Function register (pad 56 to 59)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG15<apb_soc_ctrl_PADCFG15>`                          |   108|   32|Function register (pad 60 to 63)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG16<apb_soc_ctrl_PADCFG16>`                          |   112|   32|Function register (pad 64 to 67)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG17<apb_soc_ctrl_PADCFG17>`                          |   116|   32|Function register (pad 68 to 71)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG18<apb_soc_ctrl_PADCFG18>`                          |   120|   32|Function register (pad 72 to 75)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG19<apb_soc_ctrl_PADCFG19>`                          |   124|   32|Function register (pad 76 to 79)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG20<apb_soc_ctrl_PADCFG20>`                          |   128|   32|Function register (pad 80 to 83)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG21<apb_soc_ctrl_PADCFG21>`                          |   132|   32|Function register (pad 84 to 87)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG22<apb_soc_ctrl_PADCFG22>`                          |   136|   32|Function register (pad 88 to 91)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADCFG23<apb_soc_ctrl_PADCFG23>`                          |   140|   32|Function register (pad 92 to 95)                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REG_REPROG_PAD0<apb_soc_ctrl_REG_REPROG_PAD0>`            |   144|   32|Controls reprogrammable pads 27,28,29,30,34        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REG_REPROG_PAD1<apb_soc_ctrl_REG_REPROG_PAD1>`            |   148|   32|Controls reprogrammable pads 35,40,41,42,43        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REG_REPROG_PAD2<apb_soc_ctrl_REG_REPROG_PAD2>`            |   152|   32|Controls reprogrammable pads 44,45,60,61,62        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REG_REPROG_PAD3<apb_soc_ctrl_REG_REPROG_PAD3>`            |   156|   32|Controls reprogrammable pads 63,65,66,67,68        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED2<apb_soc_ctrl_RESERVED2>`                        |   160|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED3<apb_soc_ctrl_RESERVED3>`                        |   164|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED4<apb_soc_ctrl_RESERVED4>`                        |   168|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED5<apb_soc_ctrl_RESERVED5>`                        |   172|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CL_BUSY<apb_soc_ctrl_CL_BUSY>`                            |   176|   32|Cluster busy register                              |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`JTAGREG<apb_soc_ctrl_JTAGREG>`                            |   180|   32|JTAG external register                             |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REF_FAST_CLK_DIV<apb_soc_ctrl_REF_FAST_CLK_DIV>`          |   184|   32|Read only, reference fast clk divided by power of 2|
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SW_RST<apb_soc_ctrl_SW_RST>`                              |   188|   32|Software reset, reboot                             |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CORESTATUS<apb_soc_ctrl_CORESTATUS>`                      |   192|   32|EOC and chip status register                       |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`BOOTSEL<apb_soc_ctrl_BOOTSEL>`                            |   196|   32|Value of pad bootsel                               |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`WD_RST_RST<apb_soc_ctrl_WD_RST_RST>`                      |   200|   32|Clear WD timer                                     |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`WD_RST_SET<apb_soc_ctrl_WD_RST_SET>`                      |   204|   32|Set WD timer                                       |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RWM_PER<apb_soc_ctrl_RWM_PER>`                            |   208|   32|RWM for peripherals                                |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED7<apb_soc_ctrl_RESERVED7>`                        |   212|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED8<apb_soc_ctrl_RESERVED8>`                        |   216|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED9<apb_soc_ctrl_RESERVED9>`                        |   220|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SUPERVISOR_DBG<apb_soc_ctrl_SUPERVISOR_DBG>`              |   224|   32|                                                   |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`DBG_CTRL<apb_soc_ctrl_DBG_CTRL>`                          |   228|   32|Debug access control                               |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED11<apb_soc_ctrl_RESERVED11>`                      |   232|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED12<apb_soc_ctrl_RESERVED12>`                      |   236|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CLK_DIV_I3C<apb_soc_ctrl_CLK_DIV_I3C>`                    |   240|   32|Clock divider for I3C                              |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CLK_EN_QUIDDIKEY<apb_soc_ctrl_CLK_EN_QUIDDIKEY>`          |   244|   32|Clock divider for QUIDDIKEY                        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RESERVED13<apb_soc_ctrl_RESERVED13>`                      |   248|   32|Reserved                                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`VERSION<apb_soc_ctrl_VERSION>`                            |   252|   32|Show chip version                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`WAKEUP_CTRL<apb_soc_ctrl_WAKEUP_CTRL>`                    |   256|   32|Wakeup control                                     |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_CTRL<apb_soc_ctrl_SLEEP_CTRL>`                      |   260|   32|Sleep control                                      |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_GPIO_CTRL<apb_soc_ctrl_SLEEP_GPIO_CTRL>`            |   264|   32|Sleep GPIO control                                 |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REG_OSC_CTRL<apb_soc_ctrl_REG_OSC_CTRL>`                  |   272|   32|Controls fast oscillator                           |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`REBOOT<apb_soc_ctrl_REBOOT>`                              |   276|   32|Check if system is reboot or cold boot             |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`CLK_DIV_REF_FAST_POW2<apb_soc_ctrl_CLK_DIV_REF_FAST_POW2>`|   280|   32|Controls fast oscillator pow2 divider              |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG0<apb_soc_ctrl_SLEEP_PAD_CFG0>`              |   320|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG1<apb_soc_ctrl_SLEEP_PAD_CFG1>`              |   324|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG2<apb_soc_ctrl_SLEEP_PAD_CFG2>`              |   328|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG3<apb_soc_ctrl_SLEEP_PAD_CFG3>`              |   332|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG4<apb_soc_ctrl_SLEEP_PAD_CFG4>`              |   336|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG5<apb_soc_ctrl_SLEEP_PAD_CFG5>`              |   340|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SLEEP_PAD_CFG6<apb_soc_ctrl_SLEEP_PAD_CFG6>`              |   344|   32|Sleep pad control                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`PADSLEEP<apb_soc_ctrl_PADSLEEP>`                          |   352|   32|sets the pad sleep mode on (1= on, 0= off)         |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`NEVACFG<apb_soc_ctrl_NEVACFG>`                            |   356|   32|NEVA config                                        |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`TRCCFG<apb_soc_ctrl_TRCCFG>`                              |   360|   32|TRC config                                         |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RWM_L2_INTL<apb_soc_ctrl_RWM_L2_INTL>`                    |   364|   32|RWM for L2 Interlevead                             |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`RWM_L2_PRI<apb_soc_ctrl_RWM_L2_PRI>`                      |   368|   32|RWM for L2 Private memories 3:0                    |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`L2_PWR<apb_soc_ctrl_L2_PWR>`                              |   372|   32|Controls L2 power                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`L2_CTRL<apb_soc_ctrl_L2_CTRL>`                            |   376|   32|Controls L2 power                                  |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`BORCFG<apb_soc_ctrl_BORCFG>`                              |   384|   32|Controls the brown-out reset                       |
    +----------------------------------------------------------------+------+-----+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core information register.
                #define APB_SOC_CTRL_INFO_OFFSET                 0x0
        
                // Boot address
                #define APB_SOC_CTRL_FC_BOOT_OFFSET              0x4
        
                // FC Fetch enable
                #define APB_SOC_CTRL_FC_FETCH_OFFSET             0x8
        
                // Isolate cluster register
                #define APB_SOC_CTRL_CL_ISOLATE_OFFSET           0xc
        
                // Mux config register (pad 0-15)
                #define APB_SOC_CTRL_PADFUN0_OFFSET              0x10
        
                // Mux config register (pad 16-31)
                #define APB_SOC_CTRL_PADFUN1_OFFSET              0x14
        
                // Mux config register (pad 32-47)
                #define APB_SOC_CTRL_PADFUN2_OFFSET              0x18
        
                // Mux config register (pad 48-63)
                #define APB_SOC_CTRL_PADFUN3_OFFSET              0x1c
        
                // Mux config register (pad 64-79)
                #define APB_SOC_CTRL_PADFUN4_OFFSET              0x20
        
                // Mux config register (pad 80-95)
                #define APB_SOC_CTRL_PADFUN5_OFFSET              0x24
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED0_OFFSET            0x28
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED1_OFFSET            0x2c
        
                // Function register (pad 0 to 3)
                #define APB_SOC_CTRL_PADCFG0_OFFSET              0x30
        
                // Function register (pad 4 to 7)
                #define APB_SOC_CTRL_PADCFG1_OFFSET              0x34
        
                // Function register (pad 8 to 11)
                #define APB_SOC_CTRL_PADCFG2_OFFSET              0x38
        
                // Function register (pad 12 to 15)
                #define APB_SOC_CTRL_PADCFG3_OFFSET              0x3c
        
                // Function register (pad 16 to 19)
                #define APB_SOC_CTRL_PADCFG4_OFFSET              0x40
        
                // Function register (pad 20 to 23)
                #define APB_SOC_CTRL_PADCFG5_OFFSET              0x44
        
                // Function register (pad 24 to 27)
                #define APB_SOC_CTRL_PADCFG6_OFFSET              0x48
        
                // Function register (pad 28 to 31)
                #define APB_SOC_CTRL_PADCFG7_OFFSET              0x4c
        
                // Function register (pad 32 to 35)
                #define APB_SOC_CTRL_PADCFG8_OFFSET              0x50
        
                // Function register (pad 36 to 39)
                #define APB_SOC_CTRL_PADCFG9_OFFSET              0x54
        
                // Function register (pad 40 to 43)
                #define APB_SOC_CTRL_PADCFG10_OFFSET             0x58
        
                // Function register (pad 44 to 47)
                #define APB_SOC_CTRL_PADCFG11_OFFSET             0x5c
        
                // Function register (pad 48 to 51)
                #define APB_SOC_CTRL_PADCFG12_OFFSET             0x60
        
                // Function register (pad 52 to 55)
                #define APB_SOC_CTRL_PADCFG13_OFFSET             0x64
        
                // Function register (pad 56 to 59)
                #define APB_SOC_CTRL_PADCFG14_OFFSET             0x68
        
                // Function register (pad 60 to 63)
                #define APB_SOC_CTRL_PADCFG15_OFFSET             0x6c
        
                // Function register (pad 64 to 67)
                #define APB_SOC_CTRL_PADCFG16_OFFSET             0x70
        
                // Function register (pad 68 to 71)
                #define APB_SOC_CTRL_PADCFG17_OFFSET             0x74
        
                // Function register (pad 72 to 75)
                #define APB_SOC_CTRL_PADCFG18_OFFSET             0x78
        
                // Function register (pad 76 to 79)
                #define APB_SOC_CTRL_PADCFG19_OFFSET             0x7c
        
                // Function register (pad 80 to 83)
                #define APB_SOC_CTRL_PADCFG20_OFFSET             0x80
        
                // Function register (pad 84 to 87)
                #define APB_SOC_CTRL_PADCFG21_OFFSET             0x84
        
                // Function register (pad 88 to 91)
                #define APB_SOC_CTRL_PADCFG22_OFFSET             0x88
        
                // Function register (pad 92 to 95)
                #define APB_SOC_CTRL_PADCFG23_OFFSET             0x8c
        
                // Controls reprogrammable pads 27,28,29,30,34
                #define APB_SOC_CTRL_REG_REPROG_PAD0_OFFSET      0x90
        
                // Controls reprogrammable pads 35,40,41,42,43
                #define APB_SOC_CTRL_REG_REPROG_PAD1_OFFSET      0x94
        
                // Controls reprogrammable pads 44,45,60,61,62
                #define APB_SOC_CTRL_REG_REPROG_PAD2_OFFSET      0x98
        
                // Controls reprogrammable pads 63,65,66,67,68
                #define APB_SOC_CTRL_REG_REPROG_PAD3_OFFSET      0x9c
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED2_OFFSET            0xa0
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED3_OFFSET            0xa4
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED4_OFFSET            0xa8
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED5_OFFSET            0xac
        
                // Cluster busy register
                #define APB_SOC_CTRL_CL_BUSY_OFFSET              0xb0
        
                // JTAG external register
                #define APB_SOC_CTRL_JTAGREG_OFFSET              0xb4
        
                // Read only, reference fast clk divided by power of 2
                #define APB_SOC_CTRL_REF_FAST_CLK_DIV_OFFSET     0xb8
        
                // Software reset, reboot
                #define APB_SOC_CTRL_SW_RST_OFFSET               0xbc
        
                // EOC and chip status register
                #define APB_SOC_CTRL_CORESTATUS_OFFSET           0xc0
        
                // Value of pad bootsel
                #define APB_SOC_CTRL_BOOTSEL_OFFSET              0xc4
        
                // Clear WD timer
                #define APB_SOC_CTRL_WD_RST_RST_OFFSET           0xc8
        
                // Set WD timer
                #define APB_SOC_CTRL_WD_RST_SET_OFFSET           0xcc
        
                // RWM for peripherals
                #define APB_SOC_CTRL_RWM_PER_OFFSET              0xd0
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED7_OFFSET            0xd4
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED8_OFFSET            0xd8
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED9_OFFSET            0xdc
        
                #define APB_SOC_CTRL_SUPERVISOR_DBG_OFFSET       0xe0
        
                // Debug access control
                #define APB_SOC_CTRL_DBG_CTRL_OFFSET             0xe4
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED11_OFFSET           0xe8
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED12_OFFSET           0xec
        
                // Clock divider for I3C
                #define APB_SOC_CTRL_CLK_DIV_I3C_OFFSET          0xf0
        
                // Clock divider for QUIDDIKEY
                #define APB_SOC_CTRL_CLK_EN_QUIDDIKEY_OFFSET     0xf4
        
                // Reserved
                #define APB_SOC_CTRL_RESERVED13_OFFSET           0xf8
        
                // Show chip version
                #define APB_SOC_CTRL_VERSION_OFFSET              0xfc
        
                // Wakeup control
                #define APB_SOC_CTRL_WAKEUP_CTRL_OFFSET          0x100
        
                // Sleep control
                #define APB_SOC_CTRL_SLEEP_CTRL_OFFSET           0x104
        
                // Sleep GPIO control
                #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_OFFSET      0x108
        
                // Controls fast oscillator
                #define APB_SOC_CTRL_REG_OSC_CTRL_OFFSET         0x110
        
                // Check if system is reboot or cold boot
                #define APB_SOC_CTRL_REBOOT_OFFSET               0x114
        
                // Controls fast oscillator pow2 divider
                #define APB_SOC_CTRL_CLK_DIV_REF_FAST_POW2_OFFSET 0x118
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG0_OFFSET       0x140
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG1_OFFSET       0x144
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG2_OFFSET       0x148
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG3_OFFSET       0x14c
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG4_OFFSET       0x150
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG5_OFFSET       0x154
        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG6_OFFSET       0x158
        
                // sets the pad sleep mode on (1= on, 0= off)
                #define APB_SOC_CTRL_PADSLEEP_OFFSET             0x160
        
                // NEVA config
                #define APB_SOC_CTRL_NEVACFG_OFFSET              0x164
        
                // TRC config
                #define APB_SOC_CTRL_TRCCFG_OFFSET               0x168
        
                // RWM for L2 Interlevead
                #define APB_SOC_CTRL_RWM_L2_INTL_OFFSET          0x16c
        
                // RWM for L2 Private memories 3:0
                #define APB_SOC_CTRL_RWM_L2_PRI_OFFSET           0x170
        
                // Controls L2 power
                #define APB_SOC_CTRL_L2_PWR_OFFSET               0x174
        
                // Controls L2 power
                #define APB_SOC_CTRL_L2_CTRL_OFFSET              0x178
        
                // Controls the brown-out reset
                #define APB_SOC_CTRL_BORCFG_OFFSET               0x180

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_info_get(uint32_t base);
        static inline void apb_soc_ctrl_info_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_fc_boot_get(uint32_t base);
        static inline void apb_soc_ctrl_fc_boot_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_fc_fetch_get(uint32_t base);
        static inline void apb_soc_ctrl_fc_fetch_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_cl_isolate_get(uint32_t base);
        static inline void apb_soc_ctrl_cl_isolate_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun0_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun0_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun1_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun1_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun2_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun3_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun3_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun4_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun4_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padfun5_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun5_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved0_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved0_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved1_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved1_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg0_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg0_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg1_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg1_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg2_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg3_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg3_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg4_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg4_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg5_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg5_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg6_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg6_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg7_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg7_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg8_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg8_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg9_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg9_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg10_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg10_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg11_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg11_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg12_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg12_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg13_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg13_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg14_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg14_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg15_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg15_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg16_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg16_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg17_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg17_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg18_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg18_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg19_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg19_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg20_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg20_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg21_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg21_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg22_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg22_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padcfg23_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg23_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reg_reprog_pad0_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad0_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reg_reprog_pad1_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad1_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reg_reprog_pad2_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reg_reprog_pad3_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad3_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved2_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved3_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved3_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved4_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved4_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved5_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved5_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_cl_busy_get(uint32_t base);
        static inline void apb_soc_ctrl_cl_busy_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_jtagreg_get(uint32_t base);
        static inline void apb_soc_ctrl_jtagreg_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_ref_fast_clk_div_get(uint32_t base);
        static inline void apb_soc_ctrl_ref_fast_clk_div_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sw_rst_get(uint32_t base);
        static inline void apb_soc_ctrl_sw_rst_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_corestatus_get(uint32_t base);
        static inline void apb_soc_ctrl_corestatus_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_bootsel_get(uint32_t base);
        static inline void apb_soc_ctrl_bootsel_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_wd_rst_rst_get(uint32_t base);
        static inline void apb_soc_ctrl_wd_rst_rst_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_wd_rst_set_get(uint32_t base);
        static inline void apb_soc_ctrl_wd_rst_set_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_rwm_per_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_per_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved7_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved7_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved8_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved8_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved9_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved9_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_supervisor_dbg_get(uint32_t base);
        static inline void apb_soc_ctrl_supervisor_dbg_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_dbg_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_dbg_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved11_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved11_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved12_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved12_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_clk_div_i3c_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_div_i3c_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_clk_en_quiddikey_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_en_quiddikey_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reserved13_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved13_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_version_get(uint32_t base);
        static inline void apb_soc_ctrl_version_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_wakeup_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_wakeup_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_gpio_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_gpio_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reg_osc_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_osc_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_reboot_get(uint32_t base);
        static inline void apb_soc_ctrl_reboot_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_clk_div_ref_fast_pow2_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_div_ref_fast_pow2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg0_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg0_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg1_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg1_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg2_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg2_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg3_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg3_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg4_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg4_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg5_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg5_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg6_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg6_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_padsleep_get(uint32_t base);
        static inline void apb_soc_ctrl_padsleep_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_nevacfg_get(uint32_t base);
        static inline void apb_soc_ctrl_nevacfg_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_trccfg_get(uint32_t base);
        static inline void apb_soc_ctrl_trccfg_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_rwm_l2_intl_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_l2_intl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_rwm_l2_pri_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_l2_pri_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_l2_pwr_get(uint32_t base);
        static inline void apb_soc_ctrl_l2_pwr_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_l2_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_l2_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t apb_soc_ctrl_borcfg_get(uint32_t base);
        static inline void apb_soc_ctrl_borcfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Number of clusters (access: R)
        #define APB_SOC_CTRL_INFO_NB_CL_BIT                                  0
        #define APB_SOC_CTRL_INFO_NB_CL_WIDTH                                16
        #define APB_SOC_CTRL_INFO_NB_CL_MASK                                 0xffff
        #define APB_SOC_CTRL_INFO_NB_CL_RESET                                0x8
        
        // Number of cores (access: R)
        #define APB_SOC_CTRL_INFO_NB_CORES_BIT                               16
        #define APB_SOC_CTRL_INFO_NB_CORES_WIDTH                             16
        #define APB_SOC_CTRL_INFO_NB_CORES_MASK                              0xffff0000
        #define APB_SOC_CTRL_INFO_NB_CORES_RESET                             0x1
        
        // FC Boot Address (access: R/W)
        #define APB_SOC_CTRL_FC_BOOT_ADDR_BIT                                0
        #define APB_SOC_CTRL_FC_BOOT_ADDR_WIDTH                              32
        #define APB_SOC_CTRL_FC_BOOT_ADDR_MASK                               0xffffffff
        #define APB_SOC_CTRL_FC_BOOT_ADDR_RESET                              0x1a000000
        
        // FC Fetch Enable (access: R/W)
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_BIT                              0
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_WIDTH                            1
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_MASK                             0x1
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_RESET                            0x0
        
        // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable (access: R/W)
        #define APB_SOC_CTRL_CL_ISOLATE_EN_BIT                               0
        #define APB_SOC_CTRL_CL_ISOLATE_EN_WIDTH                             1
        #define APB_SOC_CTRL_CL_ISOLATE_EN_MASK                              0x1
        #define APB_SOC_CTRL_CL_ISOLATE_EN_RESET                             0x1
        
        // Selects between: hyper0_ckn / gpio0 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_BIT                            0
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_MASK                           0x3
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_RESET                          0x0
        
        // Selects between: hyper0_ck / gpio1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_BIT                            2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_MASK                           0xc
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_RESET                          0x0
        
        // Selects between: hyper0_dq0 / gpio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_BIT                            4
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_MASK                           0x30
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_RESET                          0x0
        
        // Selects between: hyper0_dq1 / gpio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_BIT                            6
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_MASK                           0xc0
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_RESET                          0x0
        
        // Selects between: hyper0_dq2 / gpio4 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_BIT                            8
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_MASK                           0x300
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_RESET                          0x0
        
        // Selects between: hyper0_dq3 / gpio5 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_BIT                            10
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_MASK                           0xc00
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_RESET                          0x0
        
        // Selects between: hyper0_dq4 / gpio6 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_BIT                            12
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_MASK                           0x3000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_RESET                          0x0
        
        // Selects between: hyper0_dq5 / gpio7 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_BIT                            14
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_MASK                           0xc000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_RESET                          0x0
        
        // Selects between: hyper0_dq6 / gpio8 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_BIT                            16
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_MASK                           0x30000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_RESET                          0x0
        
        // Selects between: hyper0_dq7 / gpio9 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_BIT                            18
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_MASK                           0xc0000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_RESET                          0x0
        
        // Selects between: hyper0_csn0 / gpio10 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_BIT                           20
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_RESET                         0x0
        
        // Selects between: hyper0_csn1 / gpio11 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_BIT                           22
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_RESET                         0x0
        
        // Selects between: hyper0_rwds / gpio12 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_BIT                           24
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_RESET                         0x0
        
        // Selects between: hyper1_ckn / gpio13 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_BIT                           26
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_RESET                         0x0
        
        // Selects between: hyper1_ck / gpio14 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_BIT                           28
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_RESET                         0x0
        
        // Selects between: hyper1_dq0 / gpio15 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_BIT                           30
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_RESET                         0x0
        
        // Selects between: hyper1_dq1 / gpio16 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_BIT                           0
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_RESET                         0x0
        
        // Selects between: hyper1_dq2 / gpio17 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_BIT                           2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_RESET                         0x0
        
        // Selects between: hyper1_dq3 / gpio18 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_BIT                           4
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_RESET                         0x0
        
        // Selects between: hyper1_dq4 / gpio19 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_BIT                           6
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_RESET                         0x0
        
        // Selects between: hyper1_dq5 / gpio20 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_BIT                           8
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_RESET                         0x0
        
        // Selects between: hyper1_dq6 / gpio21 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_BIT                           10
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_RESET                         0x0
        
        // Selects between: hyper1_dq7 / gpio22 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_BIT                           12
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_RESET                         0x0
        
        // Selects between: hyper1_csn0 / gpio23 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_BIT                           14
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_RESET                         0x0
        
        // Selects between: hyper1_csn1 / gpio24 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_BIT                           16
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_RESET                         0x0
        
        // Selects between: hyper1_rwds / gpio25 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_BIT                           18
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_RESET                         0x0
        
        // Selects between: spi0_sck / gpio26 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_BIT                           20
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs0 / gpio27 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_BIT                           22
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs1 / gpio28 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_BIT                           24
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs2 / gpio29 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_BIT                           26
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs3 / gpio30 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_BIT                           28
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_RESET                         0x0
        
        // Selects between: spi0_sdo / gpio31 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_BIT                           30
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_RESET                         0x0
        
        // Selects between: spi0_sdi / gpio32 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_BIT                           0
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_RESET                         0x0
        
        // Selects between: spi1_sck / gpio33 / uart3_clk (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_BIT                           2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_RESET                         0x0
        
        // Selects between: mux_group_sel_spi1_cs0 / gpio34 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_BIT                           4
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_RESET                         0x0
        
        // Selects between: mux_group_sel_spi1_cs1 / gpio35 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_BIT                           6
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_RESET                         0x0
        
        // Selects between: spi1_cs2 / gpio36 / uart3_cts / spi1_sdio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_BIT                           8
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_RESET                         0x0
        
        // Selects between: spi1_cs3 / gpio37 / uart3_rts / spi1_sdio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_BIT                           10
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_RESET                         0x0
        
        // Selects between: spi1_sdo / gpio38 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_BIT                           12
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_RESET                         0x0
        
        // Selects between: spi1_sdi / gpio39 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_BIT                           14
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c0_sda / gpio40 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_BIT                           16
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c0_scl / gpio41 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_BIT                           18
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c1_sda / gpio42 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_BIT                           20
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c1_scl / gpio43 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_BIT                           22
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c2_sda / gpio44 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_BIT                           24
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c2_scl / gpio45 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_BIT                           26
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_RESET                         0x0
        
        // Selects between: i3c_sda / gpio46 / i2c3_sda / spi0_sdio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_BIT                           28
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_RESET                         0x0
        
        // Selects between: i3c_scl / gpio47 / i2c3_scl / spi0_sdio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_BIT                           30
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_RESET                         0x0
        
        // Selects between: i2s0_sck / gpio48 / uart2_clk (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_BIT                           0
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_RESET                         0x0
        
        // Selects between: i2s0_ws / gpio49 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_BIT                           2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_RESET                         0x0
        
        // Selects between: i2s0_sdi / gpio50 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_BIT                           4
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_RESET                         0x0
        
        // Selects between: i2s0_sdo / gpio51 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_BIT                           6
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_RESET                         0x0
        
        // Selects between: i2s1_sck / gpio52 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_BIT                           8
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_RESET                         0x0
        
        // Selects between: i2s1_ws / gpio53 / spi2_cs1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_BIT                           10
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_RESET                         0x0
        
        // Selects between: i2s1_sdi / gpio54 / spi2_cs2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_BIT                           12
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_RESET                         0x0
        
        // Selects between: i2s1_sdo / gpio55 / spi2_cs3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_BIT                           14
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_RESET                         0x0
        
        // Selects between: i2s2_sck / gpio56 / spi2_sck (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_BIT                           16
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_RESET                         0x0
        
        // Selects between: i2s2_ws / gpio57 / spi2_cs0 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_BIT                           18
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_RESET                         0x0
        
        // Selects between: i2s2_sdi / gpio58 / spi2_sdi (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_BIT                           20
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_RESET                         0x0
        
        // Selects between: i2s2_sdo / gpio59 / spi2_sdo (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_BIT                           22
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_rx / gpio60 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_BIT                           24
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_tx / gpio61 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_BIT                           26
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_cts / gpio62 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_BIT                           28
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_rts / gpio63 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_BIT                           30
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_RESET                         0x0
        
        // Selects between: uart0_clk / gpio64 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_BIT                           0
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_RESET                         0x0
        
        // Selects between: mux_group_sel_uart1_rx / gpio65 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_BIT                           2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_RESET                         0x0
        
        // Selects between: mux_group_sel_uart1_tx / gpio66 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_BIT                           4
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_RESET                         0x0
        
        // Selects between: mux_group_sel_pwm0 / gpio67 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_BIT                           6
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_RESET                         0x0
        
        // Selects between: mux_group_sel_pwm1 / gpio68 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_BIT                           8
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_RESET                         0x0
        
        // Selects between: uart1_clk / gpio69 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_BIT                           10
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_RESET                         0x0
        
        // Selects between: cam_pclk / gpio70 / spi3_sck (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_BIT                           12
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_RESET                         0x0
        
        // Selects between: cam_hsync / gpio71 / spi3_cs0 / csi2_hsync (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_BIT                           14
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_RESET                         0x0
        
        // Selects between: cam_data0 / gpio72 / spi3_cs1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_BIT                           16
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_RESET                         0x0
        
        // Selects between: cam_data1 / gpio73 / spi3_cs2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_BIT                           18
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_RESET                         0x0
        
        // Selects between: cam_data2 / gpio74 / spi3_cs3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_BIT                           20
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_RESET                         0x0
        
        // Selects between: cam_data3 / gpio75 / spi3_sdo (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_BIT                           22
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_RESET                         0x0
        
        // Selects between: cam_data4 / gpio76 / spi3_sdi (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_BIT                           24
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_RESET                         0x0
        
        // Selects between: cam_data5 / gpio77 / observability1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_BIT                           26
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_RESET                         0x0
        
        // Selects between: cam_data6 / gpio78 / observability2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_BIT                           28
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_RESET                         0x0
        
        // Selects between: cam_data7 / gpio79 / observability3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_BIT                           30
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_RESET                         0x0
        
        // Selects between: cam_vsync / gpio80 / observability4 / csi2_vsync (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_BIT                           0
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_RESET                         0x0
        
        // Selects between: jtag_tck / gpio81 / uart4_clk (JTAG has to be disabled to use alternates (gpio81/uart4_clk) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_BIT                           2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_RESET                         0x0
        
        // Selects between: jtag_tdi / gpio82 / uart4_rx (JTAG has to be disabled to use alternates (gpio82/uart4_rx) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_BIT                           4
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_RESET                         0x0
        
        // Selects between: jtag_tdo / gpio83 / uart4_tx (JTAG has to be disabled to use alternates (gpio83/uart4_tx) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_BIT                           6
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_RESET                         0x0
        
        // Selects between: jtag_tms / gpio84 / uart4_cts (JTAG has to be disabled to use alternates (gpio84/uart4_cts) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_BIT                           8
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_RESET                         0x0
        
        // Selects between: jtag_trst / gpio85 / uart4_rts (JTAG has to be disabled to use alternates (gpio85/uart4_rts) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_BIT                           10
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_RESET                         0x0
        
        // Selects between: wakeup_spi2_sck / gpio86 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_BIT                           12
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_RESET                         0x0
        
        // Selects between: wakeup_spi2_sdi / gpio87 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_BIT                           14
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_RESET                         0x0
        
        // Selects between: wakeup_spi2_sdo / gpio88 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_BIT                           16
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_RESET                         0x0
        
        // Selects between: wakeup_spi2_cs0 / gpio89 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_BIT                           18
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_RESET                         0x0
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs0 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_RESET    0x0
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs1 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_RESET    0x1
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs2 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_RESET    0x2
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs3 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_RESET    0x3
        
        // Selects an alternate from the mux group. Default alternate: spi1_cs0 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_BIT      24
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_MASK     0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_RESET    0x4
        
        // Selects an alternate from the mux group. Default alternate: spi1_cs1 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_RESET    0x6
        
        // Selects an alternate from the mux group. Default alternate: i2c0_sda (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_RESET    0x8
        
        // Selects an alternate from the mux group. Default alternate: i2c0_scl (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_RESET    0x9
        
        // Selects an alternate from the mux group. Default alternate: i2c1_sda_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_RESET    0xa
        
        // Selects an alternate from the mux group. Default alternate: i2c1_scl_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_BIT      24
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_MASK     0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_RESET    0xc
        
        // Selects an alternate from the mux group. Default alternate: i2c2_sda_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_RESET    0xe
        
        // Selects an alternate from the mux group. Default alternate: i2c2_scl_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_RESET    0x10
        
        // Selects an alternate from the mux group. Default alternate: uart0_rx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_RESET    0x12
        
        // Selects an alternate from the mux group. Default alternate: uart0_tx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_RESET    0x13
        
        // Selects an alternate from the mux group. Default alternate: uart0_cts_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_BIT     24
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_WIDTH   6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_MASK    0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_RESET   0x14
        
        // Selects an alternate from the mux group. Default alternate: uart0_rts_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_BIT     0
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_WIDTH   6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_MASK    0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_RESET   0x16
        
        // Selects an alternate from the mux group. Default alternate: uart1_rx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_RESET    0x18
        
        // Selects an alternate from the mux group. Default alternate: uart1_tx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_RESET    0x1b
        
        // Selects an alternate from the mux group. Default alternate: pwm0_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_BIT          18
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_WIDTH        6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_MASK         0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_RESET        0x1e
        
        // Selects an alternate from the mux group. Default alternate: pwm1_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_BIT          24
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_WIDTH        6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_MASK         0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_RESET        0x21
        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED2_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED2_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED2_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED2_BUSY_RESET                            0x0
        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED3_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED3_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED3_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED3_BUSY_RESET                            0x0
        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED4_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED4_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED4_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED4_BUSY_RESET                            0x0
        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_CL_BUSY_BUSY_BIT                                0
        #define APB_SOC_CTRL_CL_BUSY_BUSY_WIDTH                              1
        #define APB_SOC_CTRL_CL_BUSY_BUSY_MASK                               0x1
        #define APB_SOC_CTRL_CL_BUSY_BUSY_RESET                              0x0
        
        // JTAG internal register used for synchronisation from external debugger (access: R/W)
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_BIT                            0
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_WIDTH                          4
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_MASK                           0xf
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_RESET                          0x0
        
        // JTAG external register used for synchronisation from external debugger (access: R)
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_BIT                            8
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_WIDTH                          4
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_MASK                           0xf00
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_RESET                          0x0
        
        // Chip status register. The SW can store the exit value value so that the external loader can get it. (access: R/W)
        #define APB_SOC_CTRL_CORESTATUS_STATUS_BIT                           0
        #define APB_SOC_CTRL_CORESTATUS_STATUS_WIDTH                         32
        #define APB_SOC_CTRL_CORESTATUS_STATUS_MASK                          0xffffffff
        #define APB_SOC_CTRL_CORESTATUS_STATUS_RESET                         0x0
        
        // End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished. (access: R/W)
        #define APB_SOC_CTRL_CORESTATUS_EOC_BIT                              31
        #define APB_SOC_CTRL_CORESTATUS_EOC_WIDTH                            1
        #define APB_SOC_CTRL_CORESTATUS_EOC_MASK                             0x80000000
        #define APB_SOC_CTRL_CORESTATUS_EOC_RESET                            0x0
        
        // Boot mode. These bits can be used by the ROM to select the boot mode, see ROM documentation for more details (access: R)
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_BIT                             0
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_WIDTH                           2
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_MASK                            0x3
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_RESET                           0x0
        
        // Clear watch dog (access: R)
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_BIT                            0
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_WIDTH                          1
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_MASK                           0x1
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_RESET                          0x0
        
        // Watch dog Counter, maximum 2^16 * 1 / 32.768KHZ = 2s (access: R)
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_BIT                      0
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_WIDTH                    16
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_MASK                     0xffff
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_RESET                    0xffff
        
        // Enable watch dog (access: R)
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_BIT                           31
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_WIDTH                         1
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_MASK                          0x80000000
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_RESET                         0x0
        
        // Choose wakeup mode, 0 : pin wakeup mode; 1 : spis wakeup mode (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_BIT                             0
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_WIDTH                           1
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_MASK                            0x1
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_RESET                           0x0
        
        // SPI slave controller CPOL and CPHA (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_BIT                       1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_WIDTH                     2
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_MASK                      0x6
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_RESET                     0x0
        
        // Reset the SPI Slave wakeup controller (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_BIT                       3
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_WIDTH                     1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_MASK                      0x8
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_RESET                     0x1
        
        // Notify outside device wakeup done (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_BIT                    4
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_WIDTH                  1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_MASK                   0x10
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_RESET                  0x0
        
        // 0: JTAG is enabled and alternates are disabled, 1: JTAG is disabled and alternates are enabled (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_BIT                    5
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_WIDTH                  1
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_MASK                   0x20
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_RESET                  0x0
        
        // SW config. This field is only interpreted by ROM and runtime to keep information accross deep sleep. (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_BIT                           0
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_WIDTH                         8
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_MASK                          0xff
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_RESET                         0x0
        
        // Enable RTC wakeup (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_BIT                      8
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_WIDTH                    1
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_MASK                     0x100
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_RESET                    0x0
        
        // RTC event (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_BIT                        9
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_WIDTH                      1
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_MASK                       0x200
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_RESET                      0x0
        
        // Enable external wakeup (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_BIT                    10
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_WIDTH                  1
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_MASK                   0x400
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_RESET                  0x0
        
        // External event (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_BIT                        11
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_WIDTH                      8
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_MASK                       0x7f800
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_RESET                      0x0
        
        // ? (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_BIT                       19
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_WIDTH                     2
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_MASK                      0x180000
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_RESET                     0x0
        
        // ? (access: R/W)
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_BIT            0
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_WIDTH          16
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_MASK           0xffff
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_RESET          0x0
        
        // 0 - Power Down, 1 - Enables Oscillation (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_BIT                             0
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_WIDTH                           1
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_MASK                            0x1
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_RESET                           0x1
        
        // 0 - No bypass, 1 - Bypass (CLK follows XI) (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_BIT                         1
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_WIDTH                       1
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_MASK                        0x2
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_RESET                       0x0
        
        // Controls desired oscillator frequency (b'1000 - 12 or 27 MHz, b'1001 - 50Mhz) (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_BIT                         2
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_WIDTH                       4
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_MASK                        0x3c
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_RESET                       0x0
        
        // Max output current when switching on iswitchable I/Os (access: R/W)
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_BIT                     0
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_WIDTH                   5
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_MASK                    0x1f
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_RESET                   0x1f
        
        // Max output current when switching on memory interfaces (access: R/W)
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_BIT                     8
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_WIDTH                   5
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_MASK                    0x1f00
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_RESET                   0x1f
        
        // L2 retention control for private and interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_BIT                           0
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_WIDTH                         14
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_MASK                          0x3fff
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_RESET                         0x0
        
        // L2 power control for interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_BIT                          16
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_WIDTH                        12
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_MASK                         0xfff0000
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_RESET                        0x0
        
        // L2 power gate control for private and interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_BIT                    0
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_WIDTH                  14
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_MASK                   0x3fff
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_RESET                  0x0
        
        // L2 deep sleep for interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_BIT                    16
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_WIDTH                  12
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_MASK                   0xfff0000
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_RESET                  0x0
        
        // Set to 1 to enable BOR monitoring of power supply (access: R/W)
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_BIT                           0
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_WIDTH                         1
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_MASK                          0x1
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_RESET                         0x0
        
        // Threshold of detection of power supply drops (access: R/W)
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_BIT                             1
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_WIDTH                           5
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_MASK                            0x3e
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_RESET                           0x5

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_INFO_NB_CL_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL(val)                       ((val) << 0)
        
        #define APB_SOC_CTRL_INFO_NB_CORES_GET(value)              (GAP_BEXTRACTU((value),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES_GETS(value)             (GAP_BEXTRACT((value),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES_SET(value,field)        (GAP_BINSERT((value),(field),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES(val)                    ((val) << 16)
        
        #define APB_SOC_CTRL_FC_BOOT_ADDR_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR(val)                     ((val) << 0)
        
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE(val)                   ((val) << 0)
        
        #define APB_SOC_CTRL_CL_ISOLATE_EN_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN(val)                    ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_GET(value)           (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_GETS(value)          (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_SET(value,field)     (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0(val)                 ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_GET(value)           (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_GETS(value)          (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_SET(value,field)     (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1(val)                 ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_GET(value)           (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_GETS(value)          (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_SET(value,field)     (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2(val)                 ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_GET(value)           (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_GETS(value)          (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_SET(value,field)     (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3(val)                 ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_GET(value)           (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_GETS(value)          (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_SET(value,field)     (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4(val)                 ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_GET(value)           (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_GETS(value)          (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_SET(value,field)     (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5(val)                 ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6(val)                 ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_GET(value)           (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_GETS(value)          (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_SET(value,field)     (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7(val)                 ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_GET(value)           (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_GETS(value)          (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_SET(value,field)     (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8(val)                 ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_GET(value)           (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_GETS(value)          (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_SET(value,field)     (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9(val)                 ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15(val)                ((val) << 30)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31(val)                ((val) << 30)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47(val)                ((val) << 30)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63(val)                ((val) << 30)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79(val)                ((val) << 30)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0(val) ((val) << 24)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL(val) ((val) << 24)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS(val) ((val) << 24)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1(val) ((val) << 24)
        
        #define APB_SOC_CTRL_RESERVED2_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY(val)                   ((val) << 0)
        
        #define APB_SOC_CTRL_RESERVED3_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY(val)                   ((val) << 0)
        
        #define APB_SOC_CTRL_RESERVED4_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY(val)                   ((val) << 0)
        
        #define APB_SOC_CTRL_CL_BUSY_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY(val)                     ((val) << 0)
        
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL(val)                 ((val) << 0)
        
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_GET(value)           (GAP_BEXTRACTU((value),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_GETS(value)          (GAP_BEXTRACT((value),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_SET(value,field)     (GAP_BINSERT((value),(field),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL(val)                 ((val) << 8)
        
        #define APB_SOC_CTRL_CORESTATUS_STATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_CORESTATUS_EOC_GET(value)             (GAP_BEXTRACTU((value),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC_GETS(value)            (GAP_BEXTRACT((value),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC_SET(value,field)       (GAP_BINSERT((value),(field),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC(val)                   ((val) << 31)
        
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_GET(value)            (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_GETS(value)           (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_SET(value,field)      (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL(val)                  ((val) << 0)
        
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR(val)                 ((val) << 0)
        
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_GET(value)     (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_GETS(value)    (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER(val)           ((val) << 0)
        
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_GET(value)          (GAP_BEXTRACTU((value),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_GETS(value)         (GAP_BEXTRACT((value),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_SET(value,field)    (GAP_BINSERT((value),(field),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE(val)                ((val) << 31)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX(val)                  ((val) << 0)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_GET(value)      (GAP_BEXTRACTU((value),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_GETS(value)     (GAP_BEXTRACT((value),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_SET(value,field) (GAP_BINSERT((value),(field),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE(val)            ((val) << 1)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN(val)            ((val) << 3)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_GET(value)   (GAP_BEXTRACTU((value),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_GETS(value)  (GAP_BEXTRACT((value),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE(val)         ((val) << 4)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_GET(value)   (GAP_BEXTRACTU((value),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_GETS(value)  (GAP_BEXTRACT((value),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE(val)         ((val) << 5)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_GET(value)          (GAP_BEXTRACTU((value),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_GETS(value)         (GAP_BEXTRACT((value),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_SET(value,field)    (GAP_BINSERT((value),(field),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_GET(value)     (GAP_BEXTRACTU((value),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_GETS(value)    (GAP_BEXTRACT((value),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN(val)           ((val) << 8)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT(val)             ((val) << 9)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_GET(value)   (GAP_BEXTRACTU((value),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_GETS(value)  (GAP_BEXTRACT((value),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN(val)         ((val) << 10)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_GET(value)       (GAP_BEXTRACTU((value),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_GETS(value)      (GAP_BEXTRACT((value),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_SET(value,field) (GAP_BINSERT((value),(field),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT(val)             ((val) << 11)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_GET(value)      (GAP_BEXTRACTU((value),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_GETS(value)     (GAP_BEXTRACT((value),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_SET(value,field) (GAP_BINSERT((value),(field),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG(val)            ((val) << 19)
        
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN(val)                  ((val) << 0)
        
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS(val)              ((val) << 1)
        
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_GET(value)        (GAP_BEXTRACTU((value),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_GETS(value)       (GAP_BEXTRACT((value),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_SET(value,field)  (GAP_BINSERT((value),(field),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL(val)              ((val) << 2)
        
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL(val)          ((val) << 0)
        
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_GET(value)    (GAP_BEXTRACTU((value),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_GETS(value)   (GAP_BEXTRACT((value),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL(val)          ((val) << 8)
        
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_GET(value)          (GAP_BEXTRACTU((value),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_GETS(value)         (GAP_BEXTRACT((value),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_SET(value,field)    (GAP_BINSERT((value),(field),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_GET(value)         (GAP_BEXTRACTU((value),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_GETS(value)        (GAP_BEXTRACT((value),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_SET(value,field)   (GAP_BINSERT((value),(field),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN(val)               ((val) << 16)
        
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_GET(value)   (GAP_BEXTRACTU((value),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_GETS(value)  (GAP_BEXTRACT((value),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_SET(value,field) (GAP_BINSERT((value),(field),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE(val)         ((val) << 0)
        
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_GET(value)   (GAP_BEXTRACTU((value),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_GETS(value)  (GAP_BEXTRACT((value),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_SET(value,field) (GAP_BINSERT((value),(field),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP(val)         ((val) << 16)
        
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_GET(value)            (GAP_BEXTRACTU((value),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_GETS(value)           (GAP_BEXTRACT((value),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_SET(value,field)      (GAP_BINSERT((value),(field),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL(val)                  ((val) << 1)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** APB_SOC_CTRL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t info;  // Core information register.
            volatile uint32_t fc_boot;  // Boot address
            volatile uint32_t fc_fetch;  // FC Fetch enable
            volatile uint32_t cl_isolate;  // Isolate cluster register
            volatile uint32_t padfun0;  // Mux config register (pad 0-15)
            volatile uint32_t padfun1;  // Mux config register (pad 16-31)
            volatile uint32_t padfun2;  // Mux config register (pad 32-47)
            volatile uint32_t padfun3;  // Mux config register (pad 48-63)
            volatile uint32_t padfun4;  // Mux config register (pad 64-79)
            volatile uint32_t padfun5;  // Mux config register (pad 80-95)
            volatile uint32_t reserved0;  // Reserved
            volatile uint32_t reserved1;  // Reserved
            volatile uint32_t padcfg0;  // Function register (pad 0 to 3)
            volatile uint32_t padcfg1;  // Function register (pad 4 to 7)
            volatile uint32_t padcfg2;  // Function register (pad 8 to 11)
            volatile uint32_t padcfg3;  // Function register (pad 12 to 15)
            volatile uint32_t padcfg4;  // Function register (pad 16 to 19)
            volatile uint32_t padcfg5;  // Function register (pad 20 to 23)
            volatile uint32_t padcfg6;  // Function register (pad 24 to 27)
            volatile uint32_t padcfg7;  // Function register (pad 28 to 31)
            volatile uint32_t padcfg8;  // Function register (pad 32 to 35)
            volatile uint32_t padcfg9;  // Function register (pad 36 to 39)
            volatile uint32_t padcfg10;  // Function register (pad 40 to 43)
            volatile uint32_t padcfg11;  // Function register (pad 44 to 47)
            volatile uint32_t padcfg12;  // Function register (pad 48 to 51)
            volatile uint32_t padcfg13;  // Function register (pad 52 to 55)
            volatile uint32_t padcfg14;  // Function register (pad 56 to 59)
            volatile uint32_t padcfg15;  // Function register (pad 60 to 63)
            volatile uint32_t padcfg16;  // Function register (pad 64 to 67)
            volatile uint32_t padcfg17;  // Function register (pad 68 to 71)
            volatile uint32_t padcfg18;  // Function register (pad 72 to 75)
            volatile uint32_t padcfg19;  // Function register (pad 76 to 79)
            volatile uint32_t padcfg20;  // Function register (pad 80 to 83)
            volatile uint32_t padcfg21;  // Function register (pad 84 to 87)
            volatile uint32_t padcfg22;  // Function register (pad 88 to 91)
            volatile uint32_t padcfg23;  // Function register (pad 92 to 95)
            volatile uint32_t reg_reprog_pad0;  // Controls reprogrammable pads 27,28,29,30,34
            volatile uint32_t reg_reprog_pad1;  // Controls reprogrammable pads 35,40,41,42,43
            volatile uint32_t reg_reprog_pad2;  // Controls reprogrammable pads 44,45,60,61,62
            volatile uint32_t reg_reprog_pad3;  // Controls reprogrammable pads 63,65,66,67,68
            volatile uint32_t reserved2;  // Reserved
            volatile uint32_t reserved3;  // Reserved
            volatile uint32_t reserved4;  // Reserved
            volatile uint32_t reserved5;  // Reserved
            volatile uint32_t cl_busy;  // Cluster busy register
            volatile uint32_t jtagreg;  // JTAG external register
            volatile uint32_t ref_fast_clk_div;  // Read only, reference fast clk divided by power of 2
            volatile uint32_t sw_rst;  // Software reset, reboot
            volatile uint32_t corestatus;  // EOC and chip status register
            volatile uint32_t bootsel;  // Value of pad bootsel
            volatile uint32_t wd_rst_rst;  // Clear WD timer
            volatile uint32_t wd_rst_set;  // Set WD timer
            volatile uint32_t rwm_per;  // RWM for peripherals
            volatile uint32_t reserved7;  // Reserved
            volatile uint32_t reserved8;  // Reserved
            volatile uint32_t reserved9;  // Reserved
            volatile uint32_t supervisor_dbg; 
            volatile uint32_t dbg_ctrl;  // Debug access control
            volatile uint32_t reserved11;  // Reserved
            volatile uint32_t reserved12;  // Reserved
            volatile uint32_t clk_div_i3c;  // Clock divider for I3C
            volatile uint32_t clk_en_quiddikey;  // Clock divider for QUIDDIKEY
            volatile uint32_t reserved13;  // Reserved
            volatile uint32_t version;  // Show chip version
            volatile uint32_t wakeup_ctrl;  // Wakeup control
            volatile uint32_t sleep_ctrl;  // Sleep control
            volatile uint32_t sleep_gpio_ctrl;  // Sleep GPIO control
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t reg_osc_ctrl;  // Controls fast oscillator
            volatile uint32_t reboot;  // Check if system is reboot or cold boot
            volatile uint32_t clk_div_ref_fast_pow2;  // Controls fast oscillator pow2 divider
            volatile uint32_t reserved_1[9];  // Reserved/Not used.
            volatile uint32_t sleep_pad_cfg0;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg1;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg2;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg3;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg4;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg5;  // Sleep pad control
            volatile uint32_t sleep_pad_cfg6;  // Sleep pad control
            volatile uint32_t reserved_2[1];  // Reserved/Not used.
            volatile uint32_t padsleep;  // sets the pad sleep mode on (1= on, 0= off)
            volatile uint32_t nevacfg;  // NEVA config
            volatile uint32_t trccfg;  // TRC config
            volatile uint32_t rwm_l2_intl;  // RWM for L2 Interlevead
            volatile uint32_t rwm_l2_pri;  // RWM for L2 Private memories 3:0
            volatile uint32_t l2_pwr;  // Controls L2 power
            volatile uint32_t l2_ctrl;  // Controls L2 power
            volatile uint32_t reserved_3[1];  // Reserved/Not used.
            volatile uint32_t borcfg;  // Controls the brown-out reset
        } __attribute__((packed)) apb_soc_ctrl_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int nb_cl           :16; // Number of clusters
            unsigned int nb_cores        :16; // Number of cores
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_info_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // FC Boot Address
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_fc_boot_t;
        
        typedef union {
          struct {
            unsigned int fc_fe           :1 ; // FC Fetch Enable
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_fc_fetch_t;
        
        typedef union {
          struct {
            unsigned int en              :1 ; // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_cl_isolate_t;
        
        typedef union {
          struct {
            unsigned int padmux_0        :2 ; // Selects between: hyper0_ckn / gpio0
            unsigned int padmux_1        :2 ; // Selects between: hyper0_ck / gpio1
            unsigned int padmux_2        :2 ; // Selects between: hyper0_dq0 / gpio2
            unsigned int padmux_3        :2 ; // Selects between: hyper0_dq1 / gpio3
            unsigned int padmux_4        :2 ; // Selects between: hyper0_dq2 / gpio4
            unsigned int padmux_5        :2 ; // Selects between: hyper0_dq3 / gpio5
            unsigned int padmux_6        :2 ; // Selects between: hyper0_dq4 / gpio6
            unsigned int padmux_7        :2 ; // Selects between: hyper0_dq5 / gpio7
            unsigned int padmux_8        :2 ; // Selects between: hyper0_dq6 / gpio8
            unsigned int padmux_9        :2 ; // Selects between: hyper0_dq7 / gpio9
            unsigned int padmux_10       :2 ; // Selects between: hyper0_csn0 / gpio10
            unsigned int padmux_11       :2 ; // Selects between: hyper0_csn1 / gpio11
            unsigned int padmux_12       :2 ; // Selects between: hyper0_rwds / gpio12
            unsigned int padmux_13       :2 ; // Selects between: hyper1_ckn / gpio13
            unsigned int padmux_14       :2 ; // Selects between: hyper1_ck / gpio14
            unsigned int padmux_15       :2 ; // Selects between: hyper1_dq0 / gpio15
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun0_t;
        
        typedef union {
          struct {
            unsigned int padmux_16       :2 ; // Selects between: hyper1_dq1 / gpio16
            unsigned int padmux_17       :2 ; // Selects between: hyper1_dq2 / gpio17
            unsigned int padmux_18       :2 ; // Selects between: hyper1_dq3 / gpio18
            unsigned int padmux_19       :2 ; // Selects between: hyper1_dq4 / gpio19
            unsigned int padmux_20       :2 ; // Selects between: hyper1_dq5 / gpio20
            unsigned int padmux_21       :2 ; // Selects between: hyper1_dq6 / gpio21
            unsigned int padmux_22       :2 ; // Selects between: hyper1_dq7 / gpio22
            unsigned int padmux_23       :2 ; // Selects between: hyper1_csn0 / gpio23
            unsigned int padmux_24       :2 ; // Selects between: hyper1_csn1 / gpio24
            unsigned int padmux_25       :2 ; // Selects between: hyper1_rwds / gpio25
            unsigned int padmux_26       :2 ; // Selects between: spi0_sck / gpio26
            unsigned int padmux_27       :2 ; // Selects between: mux_group_sel_spi0_cs0 / gpio27
            unsigned int padmux_28       :2 ; // Selects between: mux_group_sel_spi0_cs1 / gpio28
            unsigned int padmux_29       :2 ; // Selects between: mux_group_sel_spi0_cs2 / gpio29
            unsigned int padmux_30       :2 ; // Selects between: mux_group_sel_spi0_cs3 / gpio30
            unsigned int padmux_31       :2 ; // Selects between: spi0_sdo / gpio31
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun1_t;
        
        typedef union {
          struct {
            unsigned int padmux_32       :2 ; // Selects between: spi0_sdi / gpio32
            unsigned int padmux_33       :2 ; // Selects between: spi1_sck / gpio33 / uart3_clk
            unsigned int padmux_34       :2 ; // Selects between: mux_group_sel_spi1_cs0 / gpio34
            unsigned int padmux_35       :2 ; // Selects between: mux_group_sel_spi1_cs1 / gpio35
            unsigned int padmux_36       :2 ; // Selects between: spi1_cs2 / gpio36 / uart3_cts / spi1_sdio2
            unsigned int padmux_37       :2 ; // Selects between: spi1_cs3 / gpio37 / uart3_rts / spi1_sdio3
            unsigned int padmux_38       :2 ; // Selects between: spi1_sdo / gpio38
            unsigned int padmux_39       :2 ; // Selects between: spi1_sdi / gpio39
            unsigned int padmux_40       :2 ; // Selects between: mux_group_sel_i2c0_sda / gpio40
            unsigned int padmux_41       :2 ; // Selects between: mux_group_sel_i2c0_scl / gpio41
            unsigned int padmux_42       :2 ; // Selects between: mux_group_sel_i2c1_sda / gpio42
            unsigned int padmux_43       :2 ; // Selects between: mux_group_sel_i2c1_scl / gpio43
            unsigned int padmux_44       :2 ; // Selects between: mux_group_sel_i2c2_sda / gpio44
            unsigned int padmux_45       :2 ; // Selects between: mux_group_sel_i2c2_scl / gpio45
            unsigned int padmux_46       :2 ; // Selects between: i3c_sda / gpio46 / i2c3_sda / spi0_sdio2
            unsigned int padmux_47       :2 ; // Selects between: i3c_scl / gpio47 / i2c3_scl / spi0_sdio3
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun2_t;
        
        typedef union {
          struct {
            unsigned int padmux_48       :2 ; // Selects between: i2s0_sck / gpio48 / uart2_clk
            unsigned int padmux_49       :2 ; // Selects between: i2s0_ws / gpio49
            unsigned int padmux_50       :2 ; // Selects between: i2s0_sdi / gpio50
            unsigned int padmux_51       :2 ; // Selects between: i2s0_sdo / gpio51
            unsigned int padmux_52       :2 ; // Selects between: i2s1_sck / gpio52
            unsigned int padmux_53       :2 ; // Selects between: i2s1_ws / gpio53 / spi2_cs1
            unsigned int padmux_54       :2 ; // Selects between: i2s1_sdi / gpio54 / spi2_cs2
            unsigned int padmux_55       :2 ; // Selects between: i2s1_sdo / gpio55 / spi2_cs3
            unsigned int padmux_56       :2 ; // Selects between: i2s2_sck / gpio56 / spi2_sck
            unsigned int padmux_57       :2 ; // Selects between: i2s2_ws / gpio57 / spi2_cs0
            unsigned int padmux_58       :2 ; // Selects between: i2s2_sdi / gpio58 / spi2_sdi
            unsigned int padmux_59       :2 ; // Selects between: i2s2_sdo / gpio59 / spi2_sdo
            unsigned int padmux_60       :2 ; // Selects between: mux_group_sel_uart0_rx / gpio60
            unsigned int padmux_61       :2 ; // Selects between: mux_group_sel_uart0_tx / gpio61
            unsigned int padmux_62       :2 ; // Selects between: mux_group_sel_uart0_cts / gpio62
            unsigned int padmux_63       :2 ; // Selects between: mux_group_sel_uart0_rts / gpio63
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun3_t;
        
        typedef union {
          struct {
            unsigned int padmux_64       :2 ; // Selects between: uart0_clk / gpio64
            unsigned int padmux_65       :2 ; // Selects between: mux_group_sel_uart1_rx / gpio65
            unsigned int padmux_66       :2 ; // Selects between: mux_group_sel_uart1_tx / gpio66
            unsigned int padmux_67       :2 ; // Selects between: mux_group_sel_pwm0 / gpio67
            unsigned int padmux_68       :2 ; // Selects between: mux_group_sel_pwm1 / gpio68
            unsigned int padmux_69       :2 ; // Selects between: uart1_clk / gpio69
            unsigned int padmux_70       :2 ; // Selects between: cam_pclk / gpio70 / spi3_sck
            unsigned int padmux_71       :2 ; // Selects between: cam_hsync / gpio71 / spi3_cs0 / csi2_hsync
            unsigned int padmux_72       :2 ; // Selects between: cam_data0 / gpio72 / spi3_cs1
            unsigned int padmux_73       :2 ; // Selects between: cam_data1 / gpio73 / spi3_cs2
            unsigned int padmux_74       :2 ; // Selects between: cam_data2 / gpio74 / spi3_cs3
            unsigned int padmux_75       :2 ; // Selects between: cam_data3 / gpio75 / spi3_sdo
            unsigned int padmux_76       :2 ; // Selects between: cam_data4 / gpio76 / spi3_sdi
            unsigned int padmux_77       :2 ; // Selects between: cam_data5 / gpio77 / observability1
            unsigned int padmux_78       :2 ; // Selects between: cam_data6 / gpio78 / observability2
            unsigned int padmux_79       :2 ; // Selects between: cam_data7 / gpio79 / observability3
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun4_t;
        
        typedef union {
          struct {
            unsigned int padmux_80       :2 ; // Selects between: cam_vsync / gpio80 / observability4 / csi2_vsync
            unsigned int padmux_81       :2 ; // Selects between: jtag_tck / gpio81 / uart4_clk (JTAG has to be disabled to use alternates (gpio81/uart4_clk) see JTAG_DISABLE below)
            unsigned int padmux_82       :2 ; // Selects between: jtag_tdi / gpio82 / uart4_rx (JTAG has to be disabled to use alternates (gpio82/uart4_rx) see JTAG_DISABLE below)
            unsigned int padmux_83       :2 ; // Selects between: jtag_tdo / gpio83 / uart4_tx (JTAG has to be disabled to use alternates (gpio83/uart4_tx) see JTAG_DISABLE below)
            unsigned int padmux_84       :2 ; // Selects between: jtag_tms / gpio84 / uart4_cts (JTAG has to be disabled to use alternates (gpio84/uart4_cts) see JTAG_DISABLE below)
            unsigned int padmux_85       :2 ; // Selects between: jtag_trst / gpio85 / uart4_rts (JTAG has to be disabled to use alternates (gpio85/uart4_rts) see JTAG_DISABLE below)
            unsigned int padmux_86       :2 ; // Selects between: wakeup_spi2_sck / gpio86
            unsigned int padmux_87       :2 ; // Selects between: wakeup_spi2_sdi / gpio87
            unsigned int padmux_88       :2 ; // Selects between: wakeup_spi2_sdo / gpio88
            unsigned int padmux_89       :2 ; // Selects between: wakeup_spi2_cs0 / gpio89
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg7_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg8_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg9_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg10_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg11_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg12_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg13_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg14_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg15_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg16_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg17_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg18_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg19_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg20_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg21_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg22_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg23_t;
        
        typedef union {
          struct {
            unsigned int mux_group_sel_spi0_cs0:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs0
            unsigned int mux_group_sel_spi0_cs1:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs1
            unsigned int mux_group_sel_spi0_cs2:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs2
            unsigned int mux_group_sel_spi0_cs3:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs3
            unsigned int mux_group_sel_spi1_cs0:6 ; // Selects an alternate from the mux group. Default alternate: spi1_cs0
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad0_t;
        
        typedef union {
          struct {
            unsigned int mux_group_sel_spi1_cs1:6 ; // Selects an alternate from the mux group. Default alternate: spi1_cs1
            unsigned int mux_group_sel_i2c0_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c0_sda
            unsigned int mux_group_sel_i2c0_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c0_scl
            unsigned int mux_group_sel_i2c1_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c1_sda_o
            unsigned int mux_group_sel_i2c1_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c1_scl_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad1_t;
        
        typedef union {
          struct {
            unsigned int mux_group_sel_i2c2_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c2_sda_o
            unsigned int mux_group_sel_i2c2_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c2_scl_o
            unsigned int mux_group_sel_uart0_rx:6 ; // Selects an alternate from the mux group. Default alternate: uart0_rx_o
            unsigned int mux_group_sel_uart0_tx:6 ; // Selects an alternate from the mux group. Default alternate: uart0_tx_o
            unsigned int mux_group_sel_uart0_cts:6 ; // Selects an alternate from the mux group. Default alternate: uart0_cts_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad2_t;
        
        typedef union {
          struct {
            unsigned int mux_group_sel_uart0_rts:6 ; // Selects an alternate from the mux group. Default alternate: uart0_rts_o
            unsigned int mux_group_sel_uart1_rx:6 ; // Selects an alternate from the mux group. Default alternate: uart1_rx_o
            unsigned int mux_group_sel_uart1_tx:6 ; // Selects an alternate from the mux group. Default alternate: uart1_tx_o
            unsigned int mux_group_sel_pwm0:6 ; // Selects an alternate from the mux group. Default alternate: pwm0_o
            unsigned int mux_group_sel_pwm1:6 ; // Selects an alternate from the mux group. Default alternate: pwm1_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad3_t;
        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved2_t;
        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved3_t;
        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved5_t;
        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_cl_busy_t;
        
        typedef union {
          struct {
            unsigned int internal        :4 ; // JTAG internal register used for synchronisation from external debugger
            unsigned int padding0:4 ;
            unsigned int external        :4 ; // JTAG external register used for synchronisation from external debugger
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_jtagreg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_ref_fast_clk_div_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sw_rst_t;
        
        typedef union {
          struct {
            unsigned int status          :32; // Chip status register. The SW can store the exit value value so that the external loader can get it.
            unsigned int eoc             :1 ; // End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_corestatus_t;
        
        typedef union {
          struct {
            unsigned int bootsel         :2 ; // Boot mode. These bits can be used by the ROM to select the boot mode, see ROM documentation for more details
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_bootsel_t;
        
        typedef union {
          struct {
            unsigned int clear           :1 ; // Clear watch dog
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wd_rst_rst_t;
        
        typedef union {
          struct {
            unsigned int ref_counter     :16; // Watch dog Counter, maximum 2^16 * 1 / 32.768KHZ = 2s
            unsigned int padding0:15;
            unsigned int enable          :1 ; // Enable watch dog
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wd_rst_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_per_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved7_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved8_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved9_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_supervisor_dbg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_dbg_ctrl_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved11_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved12_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_div_i3c_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_en_quiddikey_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved13_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_version_t;
        
        typedef union {
          struct {
            unsigned int mux             :1 ; // Choose wakeup mode, 0 : pin wakeup mode; 1 : spis wakeup mode
            unsigned int spis_mode       :2 ; // SPI slave controller CPOL and CPHA
            unsigned int spis_rstn       :1 ; // Reset the SPI Slave wakeup controller
            unsigned int spis_sw_done    :1 ; // Notify outside device wakeup done
            unsigned int jtag_disable    :1 ; // 0: JTAG is enabled and alternates are disabled, 1: JTAG is disabled and alternates are enabled
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wakeup_ctrl_t;
        
        typedef union {
          struct {
            unsigned int reboot          :8 ; // SW config. This field is only interpreted by ROM and runtime to keep information accross deep sleep.
            unsigned int rtc_wake_en     :1 ; // Enable RTC wakeup
            unsigned int rtc_event       :1 ; // RTC event
            unsigned int ext_wakeup_en   :1 ; // Enable external wakeup
            unsigned int ext_event       :8 ; // External event
            unsigned int wakeup_cfg      :2 ; // ?
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_ctrl_t;
        
        typedef union {
          struct {
            unsigned int gpio_wakeup_type:16; // ?
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_gpio_ctrl_t;
        
        typedef union {
          struct {
            unsigned int en              :1 ; // 0 - Power Down, 1 - Enables Oscillation
            unsigned int bypass          :1 ; // 0 - No bypass, 1 - Bypass (CLK follows XI)
            unsigned int gm_sel          :4 ; // Controls desired oscillator frequency (b'1000 - 12 or 27 MHz, b'1001 - 50Mhz)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_osc_ctrl_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reboot_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_div_ref_fast_pow2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg2_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg4_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg5_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg6_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padsleep_t;
        
        typedef union {
          struct {
            unsigned int neva_io_ls_tsel :5 ; // Max output current when switching on iswitchable I/Os
            unsigned int padding0:3 ;
            unsigned int neva_io_hs_tsel :5 ; // Max output current when switching on memory interfaces
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_nevacfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_trccfg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_l2_intl_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_l2_pri_t;
        
        typedef union {
          struct {
            unsigned int l2_mem_ret      :14; // L2 retention control for private and interleaved banks
            unsigned int padding0:2 ;
            unsigned int l2_mem_pdwn     :12; // L2 power control for interleaved banks
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_l2_pwr_t;
        
        typedef union {
          struct {
            unsigned int l2_mem_powergate:14; // L2 power gate control for private and interleaved banks
            unsigned int padding0:2 ;
            unsigned int l2_mem_deepsleep:12; // L2 deep sleep for interleaved banks
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_l2_ctrl_t;
        
        typedef union {
          struct {
            unsigned int bor_enable      :1 ; // Set to 1 to enable BOR monitoring of power supply
            unsigned int bor_vsel        :5 ; // Threshold of detection of power supply drops
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_borcfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_apb_soc_ctrl : public vp::regmap
        {
        public:
            vp_apb_soc_ctrl_info info;
            vp_apb_soc_ctrl_fc_boot fc_boot;
            vp_apb_soc_ctrl_fc_fetch fc_fetch;
            vp_apb_soc_ctrl_cl_isolate cl_isolate;
            vp_apb_soc_ctrl_padfun0 padfun0;
            vp_apb_soc_ctrl_padfun1 padfun1;
            vp_apb_soc_ctrl_padfun2 padfun2;
            vp_apb_soc_ctrl_padfun3 padfun3;
            vp_apb_soc_ctrl_padfun4 padfun4;
            vp_apb_soc_ctrl_padfun5 padfun5;
            vp_apb_soc_ctrl_reserved0 reserved0;
            vp_apb_soc_ctrl_reserved1 reserved1;
            vp_apb_soc_ctrl_padcfg0 padcfg0;
            vp_apb_soc_ctrl_padcfg1 padcfg1;
            vp_apb_soc_ctrl_padcfg2 padcfg2;
            vp_apb_soc_ctrl_padcfg3 padcfg3;
            vp_apb_soc_ctrl_padcfg4 padcfg4;
            vp_apb_soc_ctrl_padcfg5 padcfg5;
            vp_apb_soc_ctrl_padcfg6 padcfg6;
            vp_apb_soc_ctrl_padcfg7 padcfg7;
            vp_apb_soc_ctrl_padcfg8 padcfg8;
            vp_apb_soc_ctrl_padcfg9 padcfg9;
            vp_apb_soc_ctrl_padcfg10 padcfg10;
            vp_apb_soc_ctrl_padcfg11 padcfg11;
            vp_apb_soc_ctrl_padcfg12 padcfg12;
            vp_apb_soc_ctrl_padcfg13 padcfg13;
            vp_apb_soc_ctrl_padcfg14 padcfg14;
            vp_apb_soc_ctrl_padcfg15 padcfg15;
            vp_apb_soc_ctrl_padcfg16 padcfg16;
            vp_apb_soc_ctrl_padcfg17 padcfg17;
            vp_apb_soc_ctrl_padcfg18 padcfg18;
            vp_apb_soc_ctrl_padcfg19 padcfg19;
            vp_apb_soc_ctrl_padcfg20 padcfg20;
            vp_apb_soc_ctrl_padcfg21 padcfg21;
            vp_apb_soc_ctrl_padcfg22 padcfg22;
            vp_apb_soc_ctrl_padcfg23 padcfg23;
            vp_apb_soc_ctrl_reg_reprog_pad0 reg_reprog_pad0;
            vp_apb_soc_ctrl_reg_reprog_pad1 reg_reprog_pad1;
            vp_apb_soc_ctrl_reg_reprog_pad2 reg_reprog_pad2;
            vp_apb_soc_ctrl_reg_reprog_pad3 reg_reprog_pad3;
            vp_apb_soc_ctrl_reserved2 reserved2;
            vp_apb_soc_ctrl_reserved3 reserved3;
            vp_apb_soc_ctrl_reserved4 reserved4;
            vp_apb_soc_ctrl_reserved5 reserved5;
            vp_apb_soc_ctrl_cl_busy cl_busy;
            vp_apb_soc_ctrl_jtagreg jtagreg;
            vp_apb_soc_ctrl_ref_fast_clk_div ref_fast_clk_div;
            vp_apb_soc_ctrl_sw_rst sw_rst;
            vp_apb_soc_ctrl_corestatus corestatus;
            vp_apb_soc_ctrl_bootsel bootsel;
            vp_apb_soc_ctrl_wd_rst_rst wd_rst_rst;
            vp_apb_soc_ctrl_wd_rst_set wd_rst_set;
            vp_apb_soc_ctrl_rwm_per rwm_per;
            vp_apb_soc_ctrl_reserved7 reserved7;
            vp_apb_soc_ctrl_reserved8 reserved8;
            vp_apb_soc_ctrl_reserved9 reserved9;
            vp_apb_soc_ctrl_supervisor_dbg supervisor_dbg;
            vp_apb_soc_ctrl_dbg_ctrl dbg_ctrl;
            vp_apb_soc_ctrl_reserved11 reserved11;
            vp_apb_soc_ctrl_reserved12 reserved12;
            vp_apb_soc_ctrl_clk_div_i3c clk_div_i3c;
            vp_apb_soc_ctrl_clk_en_quiddikey clk_en_quiddikey;
            vp_apb_soc_ctrl_reserved13 reserved13;
            vp_apb_soc_ctrl_version version;
            vp_apb_soc_ctrl_wakeup_ctrl wakeup_ctrl;
            vp_apb_soc_ctrl_sleep_ctrl sleep_ctrl;
            vp_apb_soc_ctrl_sleep_gpio_ctrl sleep_gpio_ctrl;
            vp_apb_soc_ctrl_reg_osc_ctrl reg_osc_ctrl;
            vp_apb_soc_ctrl_reboot reboot;
            vp_apb_soc_ctrl_clk_div_ref_fast_pow2 clk_div_ref_fast_pow2;
            vp_apb_soc_ctrl_sleep_pad_cfg0 sleep_pad_cfg0;
            vp_apb_soc_ctrl_sleep_pad_cfg1 sleep_pad_cfg1;
            vp_apb_soc_ctrl_sleep_pad_cfg2 sleep_pad_cfg2;
            vp_apb_soc_ctrl_sleep_pad_cfg3 sleep_pad_cfg3;
            vp_apb_soc_ctrl_sleep_pad_cfg4 sleep_pad_cfg4;
            vp_apb_soc_ctrl_sleep_pad_cfg5 sleep_pad_cfg5;
            vp_apb_soc_ctrl_sleep_pad_cfg6 sleep_pad_cfg6;
            vp_apb_soc_ctrl_padsleep padsleep;
            vp_apb_soc_ctrl_nevacfg nevacfg;
            vp_apb_soc_ctrl_trccfg trccfg;
            vp_apb_soc_ctrl_rwm_l2_intl rwm_l2_intl;
            vp_apb_soc_ctrl_rwm_l2_pri rwm_l2_pri;
            vp_apb_soc_ctrl_l2_pwr l2_pwr;
            vp_apb_soc_ctrl_l2_ctrl l2_ctrl;
            vp_apb_soc_ctrl_borcfg borcfg;
        };

|

.. _apb_soc_ctrl_INFO:

INFO
""""

Core information register.

.. table:: 

    +-----+---+--------+------------------+
    |Bit #|R/W|  Name  |   Description    |
    +=====+===+========+==================+
    |15:0 |R  |NB_CL   |Number of clusters|
    +-----+---+--------+------------------+
    |31:16|R  |NB_CORES|Number of cores   |
    +-----+---+--------+------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core information register.
                #define APB_SOC_CTRL_INFO_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_info_get(uint32_t base);
        static inline void apb_soc_ctrl_info_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Number of clusters (access: R)
        #define APB_SOC_CTRL_INFO_NB_CL_BIT                                  0
        #define APB_SOC_CTRL_INFO_NB_CL_WIDTH                                16
        #define APB_SOC_CTRL_INFO_NB_CL_MASK                                 0xffff
        #define APB_SOC_CTRL_INFO_NB_CL_RESET                                0x8
        
        // Number of cores (access: R)
        #define APB_SOC_CTRL_INFO_NB_CORES_BIT                               16
        #define APB_SOC_CTRL_INFO_NB_CORES_WIDTH                             16
        #define APB_SOC_CTRL_INFO_NB_CORES_MASK                              0xffff0000
        #define APB_SOC_CTRL_INFO_NB_CORES_RESET                             0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_INFO_NB_CL_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_INFO_NB_CL(val)                       ((val) << 0)
        
        #define APB_SOC_CTRL_INFO_NB_CORES_GET(value)              (GAP_BEXTRACTU((value),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES_GETS(value)             (GAP_BEXTRACT((value),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES_SET(value,field)        (GAP_BINSERT((value),(field),16,16))
        #define APB_SOC_CTRL_INFO_NB_CORES(val)                    ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int nb_cl           :16; // Number of clusters
            unsigned int nb_cores        :16; // Number of cores
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_info_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_info : public vp::reg_32
        {
        public:
            inline void nb_cl_set(uint32_t value);
            inline uint32_t nb_cl_get();
            inline void nb_cores_set(uint32_t value);
            inline uint32_t nb_cores_get();
        };

|

.. _apb_soc_ctrl_FC_BOOT:

FC_BOOT
"""""""

Boot address

.. table:: 

    +-----+---+----+---------------+
    |Bit #|R/W|Name|  Description  |
    +=====+===+====+===============+
    |31:0 |R/W|ADDR|FC Boot Address|
    +-----+---+----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Boot address
                #define APB_SOC_CTRL_FC_BOOT_OFFSET              0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_fc_boot_get(uint32_t base);
        static inline void apb_soc_ctrl_fc_boot_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC Boot Address (access: R/W)
        #define APB_SOC_CTRL_FC_BOOT_ADDR_BIT                                0
        #define APB_SOC_CTRL_FC_BOOT_ADDR_WIDTH                              32
        #define APB_SOC_CTRL_FC_BOOT_ADDR_MASK                               0xffffffff
        #define APB_SOC_CTRL_FC_BOOT_ADDR_RESET                              0x1a000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_FC_BOOT_ADDR_GET(value)               (GAP_BEXTRACTU((value),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR_GETS(value)              (GAP_BEXTRACT((value),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR_SET(value,field)         (GAP_BINSERT((value),(field),32,0))
        #define APB_SOC_CTRL_FC_BOOT_ADDR(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // FC Boot Address
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_fc_boot_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_fc_boot : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _apb_soc_ctrl_FC_FETCH:

FC_FETCH
""""""""

FC Fetch enable

.. table:: 

    +-----+---+-----+---------------+
    |Bit #|R/W|Name |  Description  |
    +=====+===+=====+===============+
    |    0|R/W|FC_FE|FC Fetch Enable|
    +-----+---+-----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC Fetch enable
                #define APB_SOC_CTRL_FC_FETCH_OFFSET             0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_fc_fetch_get(uint32_t base);
        static inline void apb_soc_ctrl_fc_fetch_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC Fetch Enable (access: R/W)
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_BIT                              0
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_WIDTH                            1
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_MASK                             0x1
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_FC_FETCH_FC_FE(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int fc_fe           :1 ; // FC Fetch Enable
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_fc_fetch_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_fc_fetch : public vp::reg_32
        {
        public:
            inline void fc_fe_set(uint32_t value);
            inline uint32_t fc_fe_get();
        };

|

.. _apb_soc_ctrl_CL_ISOLATE:

CL_ISOLATE
""""""""""

Isolate cluster register

.. table:: 

    +-----+---+----+-----------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                          Description                                          |
    +=====+===+====+===============================================================================================+
    |    0|R/W|EN  |Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable|
    +-----+---+----+-----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Isolate cluster register
                #define APB_SOC_CTRL_CL_ISOLATE_OFFSET           0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_cl_isolate_get(uint32_t base);
        static inline void apb_soc_ctrl_cl_isolate_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable (access: R/W)
        #define APB_SOC_CTRL_CL_ISOLATE_EN_BIT                               0
        #define APB_SOC_CTRL_CL_ISOLATE_EN_WIDTH                             1
        #define APB_SOC_CTRL_CL_ISOLATE_EN_MASK                              0x1
        #define APB_SOC_CTRL_CL_ISOLATE_EN_RESET                             0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_CL_ISOLATE_EN_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_CL_ISOLATE_EN(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // Isolate cluster. Inhibits AXI transactions from cluster to SoC: - 1'b0:  Disable - 1'b1: Enable
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_cl_isolate_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_cl_isolate : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _apb_soc_ctrl_PADFUN0:

PADFUN0
"""""""

Mux config register (pad 0-15)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |1:0  |R/W|Padmux_0 |Selects between: hyper0_ckn / gpio0  |
    +-----+---+---------+-------------------------------------+
    |3:2  |R/W|Padmux_1 |Selects between: hyper0_ck / gpio1   |
    +-----+---+---------+-------------------------------------+
    |5:4  |R/W|Padmux_2 |Selects between: hyper0_dq0 / gpio2  |
    +-----+---+---------+-------------------------------------+
    |7:6  |R/W|Padmux_3 |Selects between: hyper0_dq1 / gpio3  |
    +-----+---+---------+-------------------------------------+
    |9:8  |R/W|Padmux_4 |Selects between: hyper0_dq2 / gpio4  |
    +-----+---+---------+-------------------------------------+
    |11:10|R/W|Padmux_5 |Selects between: hyper0_dq3 / gpio5  |
    +-----+---+---------+-------------------------------------+
    |13:12|R/W|Padmux_6 |Selects between: hyper0_dq4 / gpio6  |
    +-----+---+---------+-------------------------------------+
    |15:14|R/W|Padmux_7 |Selects between: hyper0_dq5 / gpio7  |
    +-----+---+---------+-------------------------------------+
    |17:16|R/W|Padmux_8 |Selects between: hyper0_dq6 / gpio8  |
    +-----+---+---------+-------------------------------------+
    |19:18|R/W|Padmux_9 |Selects between: hyper0_dq7 / gpio9  |
    +-----+---+---------+-------------------------------------+
    |21:20|R/W|Padmux_10|Selects between: hyper0_csn0 / gpio10|
    +-----+---+---------+-------------------------------------+
    |23:22|R/W|Padmux_11|Selects between: hyper0_csn1 / gpio11|
    +-----+---+---------+-------------------------------------+
    |25:24|R/W|Padmux_12|Selects between: hyper0_rwds / gpio12|
    +-----+---+---------+-------------------------------------+
    |27:26|R/W|Padmux_13|Selects between: hyper1_ckn / gpio13 |
    +-----+---+---------+-------------------------------------+
    |29:28|R/W|Padmux_14|Selects between: hyper1_ck / gpio14  |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|Padmux_15|Selects between: hyper1_dq0 / gpio15 |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 0-15)
                #define APB_SOC_CTRL_PADFUN0_OFFSET              0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun0_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: hyper0_ckn / gpio0 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_BIT                            0
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_MASK                           0x3
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_RESET                          0x0
        
        // Selects between: hyper0_ck / gpio1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_BIT                            2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_MASK                           0xc
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_RESET                          0x0
        
        // Selects between: hyper0_dq0 / gpio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_BIT                            4
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_MASK                           0x30
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_RESET                          0x0
        
        // Selects between: hyper0_dq1 / gpio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_BIT                            6
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_MASK                           0xc0
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_RESET                          0x0
        
        // Selects between: hyper0_dq2 / gpio4 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_BIT                            8
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_MASK                           0x300
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_RESET                          0x0
        
        // Selects between: hyper0_dq3 / gpio5 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_BIT                            10
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_MASK                           0xc00
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_RESET                          0x0
        
        // Selects between: hyper0_dq4 / gpio6 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_BIT                            12
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_MASK                           0x3000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_RESET                          0x0
        
        // Selects between: hyper0_dq5 / gpio7 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_BIT                            14
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_MASK                           0xc000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_RESET                          0x0
        
        // Selects between: hyper0_dq6 / gpio8 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_BIT                            16
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_MASK                           0x30000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_RESET                          0x0
        
        // Selects between: hyper0_dq7 / gpio9 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_BIT                            18
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_WIDTH                          2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_MASK                           0xc0000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_RESET                          0x0
        
        // Selects between: hyper0_csn0 / gpio10 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_BIT                           20
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_RESET                         0x0
        
        // Selects between: hyper0_csn1 / gpio11 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_BIT                           22
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_RESET                         0x0
        
        // Selects between: hyper0_rwds / gpio12 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_BIT                           24
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_RESET                         0x0
        
        // Selects between: hyper1_ckn / gpio13 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_BIT                           26
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_RESET                         0x0
        
        // Selects between: hyper1_ck / gpio14 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_BIT                           28
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_RESET                         0x0
        
        // Selects between: hyper1_dq0 / gpio15 (access: R/W)
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_BIT                           30
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_GET(value)           (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_GETS(value)          (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0_SET(value,field)     (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_0(val)                 ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_GET(value)           (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_GETS(value)          (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1_SET(value,field)     (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_1(val)                 ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_GET(value)           (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_GETS(value)          (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2_SET(value,field)     (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_2(val)                 ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_GET(value)           (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_GETS(value)          (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3_SET(value,field)     (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_3(val)                 ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_GET(value)           (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_GETS(value)          (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4_SET(value,field)     (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_4(val)                 ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_GET(value)           (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_GETS(value)          (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5_SET(value,field)     (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_5(val)                 ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_GET(value)           (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_GETS(value)          (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6_SET(value,field)     (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_6(val)                 ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_GET(value)           (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_GETS(value)          (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7_SET(value,field)     (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_7(val)                 ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_GET(value)           (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_GETS(value)          (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8_SET(value,field)     (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_8(val)                 ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_GET(value)           (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_GETS(value)          (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9_SET(value,field)     (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_9(val)                 ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_10(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_11(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_12(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_13(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_14(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN0_PADMUX_15(val)                ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_0        :2 ; // Selects between: hyper0_ckn / gpio0
            unsigned int padmux_1        :2 ; // Selects between: hyper0_ck / gpio1
            unsigned int padmux_2        :2 ; // Selects between: hyper0_dq0 / gpio2
            unsigned int padmux_3        :2 ; // Selects between: hyper0_dq1 / gpio3
            unsigned int padmux_4        :2 ; // Selects between: hyper0_dq2 / gpio4
            unsigned int padmux_5        :2 ; // Selects between: hyper0_dq3 / gpio5
            unsigned int padmux_6        :2 ; // Selects between: hyper0_dq4 / gpio6
            unsigned int padmux_7        :2 ; // Selects between: hyper0_dq5 / gpio7
            unsigned int padmux_8        :2 ; // Selects between: hyper0_dq6 / gpio8
            unsigned int padmux_9        :2 ; // Selects between: hyper0_dq7 / gpio9
            unsigned int padmux_10       :2 ; // Selects between: hyper0_csn0 / gpio10
            unsigned int padmux_11       :2 ; // Selects between: hyper0_csn1 / gpio11
            unsigned int padmux_12       :2 ; // Selects between: hyper0_rwds / gpio12
            unsigned int padmux_13       :2 ; // Selects between: hyper1_ckn / gpio13
            unsigned int padmux_14       :2 ; // Selects between: hyper1_ck / gpio14
            unsigned int padmux_15       :2 ; // Selects between: hyper1_dq0 / gpio15
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun0 : public vp::reg_32
        {
        public:
            inline void padmux_0_set(uint32_t value);
            inline uint32_t padmux_0_get();
            inline void padmux_1_set(uint32_t value);
            inline uint32_t padmux_1_get();
            inline void padmux_2_set(uint32_t value);
            inline uint32_t padmux_2_get();
            inline void padmux_3_set(uint32_t value);
            inline uint32_t padmux_3_get();
            inline void padmux_4_set(uint32_t value);
            inline uint32_t padmux_4_get();
            inline void padmux_5_set(uint32_t value);
            inline uint32_t padmux_5_get();
            inline void padmux_6_set(uint32_t value);
            inline uint32_t padmux_6_get();
            inline void padmux_7_set(uint32_t value);
            inline uint32_t padmux_7_get();
            inline void padmux_8_set(uint32_t value);
            inline uint32_t padmux_8_get();
            inline void padmux_9_set(uint32_t value);
            inline uint32_t padmux_9_get();
            inline void padmux_10_set(uint32_t value);
            inline uint32_t padmux_10_get();
            inline void padmux_11_set(uint32_t value);
            inline uint32_t padmux_11_get();
            inline void padmux_12_set(uint32_t value);
            inline uint32_t padmux_12_get();
            inline void padmux_13_set(uint32_t value);
            inline uint32_t padmux_13_get();
            inline void padmux_14_set(uint32_t value);
            inline uint32_t padmux_14_get();
            inline void padmux_15_set(uint32_t value);
            inline uint32_t padmux_15_get();
        };

|

.. _apb_soc_ctrl_PADFUN1:

PADFUN1
"""""""

Mux config register (pad 16-31)

.. table:: 

    +-----+---+---------+------------------------------------------------+
    |Bit #|R/W|  Name   |                  Description                   |
    +=====+===+=========+================================================+
    |1:0  |R/W|Padmux_16|Selects between: hyper1_dq1 / gpio16            |
    +-----+---+---------+------------------------------------------------+
    |3:2  |R/W|Padmux_17|Selects between: hyper1_dq2 / gpio17            |
    +-----+---+---------+------------------------------------------------+
    |5:4  |R/W|Padmux_18|Selects between: hyper1_dq3 / gpio18            |
    +-----+---+---------+------------------------------------------------+
    |7:6  |R/W|Padmux_19|Selects between: hyper1_dq4 / gpio19            |
    +-----+---+---------+------------------------------------------------+
    |9:8  |R/W|Padmux_20|Selects between: hyper1_dq5 / gpio20            |
    +-----+---+---------+------------------------------------------------+
    |11:10|R/W|Padmux_21|Selects between: hyper1_dq6 / gpio21            |
    +-----+---+---------+------------------------------------------------+
    |13:12|R/W|Padmux_22|Selects between: hyper1_dq7 / gpio22            |
    +-----+---+---------+------------------------------------------------+
    |15:14|R/W|Padmux_23|Selects between: hyper1_csn0 / gpio23           |
    +-----+---+---------+------------------------------------------------+
    |17:16|R/W|Padmux_24|Selects between: hyper1_csn1 / gpio24           |
    +-----+---+---------+------------------------------------------------+
    |19:18|R/W|Padmux_25|Selects between: hyper1_rwds / gpio25           |
    +-----+---+---------+------------------------------------------------+
    |21:20|R/W|Padmux_26|Selects between: spi0_sck / gpio26              |
    +-----+---+---------+------------------------------------------------+
    |23:22|R/W|Padmux_27|Selects between: mux_group_sel_spi0_cs0 / gpio27|
    +-----+---+---------+------------------------------------------------+
    |25:24|R/W|Padmux_28|Selects between: mux_group_sel_spi0_cs1 / gpio28|
    +-----+---+---------+------------------------------------------------+
    |27:26|R/W|Padmux_29|Selects between: mux_group_sel_spi0_cs2 / gpio29|
    +-----+---+---------+------------------------------------------------+
    |29:28|R/W|Padmux_30|Selects between: mux_group_sel_spi0_cs3 / gpio30|
    +-----+---+---------+------------------------------------------------+
    |31:30|R/W|Padmux_31|Selects between: spi0_sdo / gpio31              |
    +-----+---+---------+------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 16-31)
                #define APB_SOC_CTRL_PADFUN1_OFFSET              0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun1_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: hyper1_dq1 / gpio16 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_BIT                           0
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_RESET                         0x0
        
        // Selects between: hyper1_dq2 / gpio17 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_BIT                           2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_RESET                         0x0
        
        // Selects between: hyper1_dq3 / gpio18 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_BIT                           4
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_RESET                         0x0
        
        // Selects between: hyper1_dq4 / gpio19 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_BIT                           6
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_RESET                         0x0
        
        // Selects between: hyper1_dq5 / gpio20 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_BIT                           8
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_RESET                         0x0
        
        // Selects between: hyper1_dq6 / gpio21 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_BIT                           10
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_RESET                         0x0
        
        // Selects between: hyper1_dq7 / gpio22 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_BIT                           12
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_RESET                         0x0
        
        // Selects between: hyper1_csn0 / gpio23 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_BIT                           14
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_RESET                         0x0
        
        // Selects between: hyper1_csn1 / gpio24 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_BIT                           16
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_RESET                         0x0
        
        // Selects between: hyper1_rwds / gpio25 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_BIT                           18
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_RESET                         0x0
        
        // Selects between: spi0_sck / gpio26 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_BIT                           20
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs0 / gpio27 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_BIT                           22
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs1 / gpio28 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_BIT                           24
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs2 / gpio29 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_BIT                           26
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_RESET                         0x0
        
        // Selects between: mux_group_sel_spi0_cs3 / gpio30 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_BIT                           28
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_RESET                         0x0
        
        // Selects between: spi0_sdo / gpio31 (access: R/W)
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_BIT                           30
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_16(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_17(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_18(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_19(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_20(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_21(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_22(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_23(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_24(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_25(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_26(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_27(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_28(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_29(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_30(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN1_PADMUX_31(val)                ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_16       :2 ; // Selects between: hyper1_dq1 / gpio16
            unsigned int padmux_17       :2 ; // Selects between: hyper1_dq2 / gpio17
            unsigned int padmux_18       :2 ; // Selects between: hyper1_dq3 / gpio18
            unsigned int padmux_19       :2 ; // Selects between: hyper1_dq4 / gpio19
            unsigned int padmux_20       :2 ; // Selects between: hyper1_dq5 / gpio20
            unsigned int padmux_21       :2 ; // Selects between: hyper1_dq6 / gpio21
            unsigned int padmux_22       :2 ; // Selects between: hyper1_dq7 / gpio22
            unsigned int padmux_23       :2 ; // Selects between: hyper1_csn0 / gpio23
            unsigned int padmux_24       :2 ; // Selects between: hyper1_csn1 / gpio24
            unsigned int padmux_25       :2 ; // Selects between: hyper1_rwds / gpio25
            unsigned int padmux_26       :2 ; // Selects between: spi0_sck / gpio26
            unsigned int padmux_27       :2 ; // Selects between: mux_group_sel_spi0_cs0 / gpio27
            unsigned int padmux_28       :2 ; // Selects between: mux_group_sel_spi0_cs1 / gpio28
            unsigned int padmux_29       :2 ; // Selects between: mux_group_sel_spi0_cs2 / gpio29
            unsigned int padmux_30       :2 ; // Selects between: mux_group_sel_spi0_cs3 / gpio30
            unsigned int padmux_31       :2 ; // Selects between: spi0_sdo / gpio31
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun1 : public vp::reg_32
        {
        public:
            inline void padmux_16_set(uint32_t value);
            inline uint32_t padmux_16_get();
            inline void padmux_17_set(uint32_t value);
            inline uint32_t padmux_17_get();
            inline void padmux_18_set(uint32_t value);
            inline uint32_t padmux_18_get();
            inline void padmux_19_set(uint32_t value);
            inline uint32_t padmux_19_get();
            inline void padmux_20_set(uint32_t value);
            inline uint32_t padmux_20_get();
            inline void padmux_21_set(uint32_t value);
            inline uint32_t padmux_21_get();
            inline void padmux_22_set(uint32_t value);
            inline uint32_t padmux_22_get();
            inline void padmux_23_set(uint32_t value);
            inline uint32_t padmux_23_get();
            inline void padmux_24_set(uint32_t value);
            inline uint32_t padmux_24_get();
            inline void padmux_25_set(uint32_t value);
            inline uint32_t padmux_25_get();
            inline void padmux_26_set(uint32_t value);
            inline uint32_t padmux_26_get();
            inline void padmux_27_set(uint32_t value);
            inline uint32_t padmux_27_get();
            inline void padmux_28_set(uint32_t value);
            inline uint32_t padmux_28_get();
            inline void padmux_29_set(uint32_t value);
            inline uint32_t padmux_29_get();
            inline void padmux_30_set(uint32_t value);
            inline uint32_t padmux_30_get();
            inline void padmux_31_set(uint32_t value);
            inline uint32_t padmux_31_get();
        };

|

.. _apb_soc_ctrl_PADFUN2:

PADFUN2
"""""""

Mux config register (pad 32-47)

.. table:: 

    +-----+---+---------+-----------------------------------------------------------+
    |Bit #|R/W|  Name   |                        Description                        |
    +=====+===+=========+===========================================================+
    |1:0  |R/W|Padmux_32|Selects between: spi0_sdi / gpio32                         |
    +-----+---+---------+-----------------------------------------------------------+
    |3:2  |R/W|Padmux_33|Selects between: spi1_sck / gpio33 / uart3_clk             |
    +-----+---+---------+-----------------------------------------------------------+
    |5:4  |R/W|Padmux_34|Selects between: mux_group_sel_spi1_cs0 / gpio34           |
    +-----+---+---------+-----------------------------------------------------------+
    |7:6  |R/W|Padmux_35|Selects between: mux_group_sel_spi1_cs1 / gpio35           |
    +-----+---+---------+-----------------------------------------------------------+
    |9:8  |R/W|Padmux_36|Selects between: spi1_cs2 / gpio36 / uart3_cts / spi1_sdio2|
    +-----+---+---------+-----------------------------------------------------------+
    |11:10|R/W|Padmux_37|Selects between: spi1_cs3 / gpio37 / uart3_rts / spi1_sdio3|
    +-----+---+---------+-----------------------------------------------------------+
    |13:12|R/W|Padmux_38|Selects between: spi1_sdo / gpio38                         |
    +-----+---+---------+-----------------------------------------------------------+
    |15:14|R/W|Padmux_39|Selects between: spi1_sdi / gpio39                         |
    +-----+---+---------+-----------------------------------------------------------+
    |17:16|R/W|Padmux_40|Selects between: mux_group_sel_i2c0_sda / gpio40           |
    +-----+---+---------+-----------------------------------------------------------+
    |19:18|R/W|Padmux_41|Selects between: mux_group_sel_i2c0_scl / gpio41           |
    +-----+---+---------+-----------------------------------------------------------+
    |21:20|R/W|Padmux_42|Selects between: mux_group_sel_i2c1_sda / gpio42           |
    +-----+---+---------+-----------------------------------------------------------+
    |23:22|R/W|Padmux_43|Selects between: mux_group_sel_i2c1_scl / gpio43           |
    +-----+---+---------+-----------------------------------------------------------+
    |25:24|R/W|Padmux_44|Selects between: mux_group_sel_i2c2_sda / gpio44           |
    +-----+---+---------+-----------------------------------------------------------+
    |27:26|R/W|Padmux_45|Selects between: mux_group_sel_i2c2_scl / gpio45           |
    +-----+---+---------+-----------------------------------------------------------+
    |29:28|R/W|Padmux_46|Selects between: i3c_sda / gpio46 / i2c3_sda / spi0_sdio2  |
    +-----+---+---------+-----------------------------------------------------------+
    |31:30|R/W|Padmux_47|Selects between: i3c_scl / gpio47 / i2c3_scl / spi0_sdio3  |
    +-----+---+---------+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 32-47)
                #define APB_SOC_CTRL_PADFUN2_OFFSET              0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun2_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: spi0_sdi / gpio32 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_BIT                           0
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_RESET                         0x0
        
        // Selects between: spi1_sck / gpio33 / uart3_clk (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_BIT                           2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_RESET                         0x0
        
        // Selects between: mux_group_sel_spi1_cs0 / gpio34 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_BIT                           4
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_RESET                         0x0
        
        // Selects between: mux_group_sel_spi1_cs1 / gpio35 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_BIT                           6
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_RESET                         0x0
        
        // Selects between: spi1_cs2 / gpio36 / uart3_cts / spi1_sdio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_BIT                           8
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_RESET                         0x0
        
        // Selects between: spi1_cs3 / gpio37 / uart3_rts / spi1_sdio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_BIT                           10
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_RESET                         0x0
        
        // Selects between: spi1_sdo / gpio38 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_BIT                           12
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_RESET                         0x0
        
        // Selects between: spi1_sdi / gpio39 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_BIT                           14
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c0_sda / gpio40 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_BIT                           16
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c0_scl / gpio41 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_BIT                           18
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c1_sda / gpio42 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_BIT                           20
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c1_scl / gpio43 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_BIT                           22
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c2_sda / gpio44 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_BIT                           24
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_RESET                         0x0
        
        // Selects between: mux_group_sel_i2c2_scl / gpio45 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_BIT                           26
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_RESET                         0x0
        
        // Selects between: i3c_sda / gpio46 / i2c3_sda / spi0_sdio2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_BIT                           28
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_RESET                         0x0
        
        // Selects between: i3c_scl / gpio47 / i2c3_scl / spi0_sdio3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_BIT                           30
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_32(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_33(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_34(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_35(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_36(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_37(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_38(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_39(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_40(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_41(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_42(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_43(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_44(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_45(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_46(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN2_PADMUX_47(val)                ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_32       :2 ; // Selects between: spi0_sdi / gpio32
            unsigned int padmux_33       :2 ; // Selects between: spi1_sck / gpio33 / uart3_clk
            unsigned int padmux_34       :2 ; // Selects between: mux_group_sel_spi1_cs0 / gpio34
            unsigned int padmux_35       :2 ; // Selects between: mux_group_sel_spi1_cs1 / gpio35
            unsigned int padmux_36       :2 ; // Selects between: spi1_cs2 / gpio36 / uart3_cts / spi1_sdio2
            unsigned int padmux_37       :2 ; // Selects between: spi1_cs3 / gpio37 / uart3_rts / spi1_sdio3
            unsigned int padmux_38       :2 ; // Selects between: spi1_sdo / gpio38
            unsigned int padmux_39       :2 ; // Selects between: spi1_sdi / gpio39
            unsigned int padmux_40       :2 ; // Selects between: mux_group_sel_i2c0_sda / gpio40
            unsigned int padmux_41       :2 ; // Selects between: mux_group_sel_i2c0_scl / gpio41
            unsigned int padmux_42       :2 ; // Selects between: mux_group_sel_i2c1_sda / gpio42
            unsigned int padmux_43       :2 ; // Selects between: mux_group_sel_i2c1_scl / gpio43
            unsigned int padmux_44       :2 ; // Selects between: mux_group_sel_i2c2_sda / gpio44
            unsigned int padmux_45       :2 ; // Selects between: mux_group_sel_i2c2_scl / gpio45
            unsigned int padmux_46       :2 ; // Selects between: i3c_sda / gpio46 / i2c3_sda / spi0_sdio2
            unsigned int padmux_47       :2 ; // Selects between: i3c_scl / gpio47 / i2c3_scl / spi0_sdio3
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun2 : public vp::reg_32
        {
        public:
            inline void padmux_32_set(uint32_t value);
            inline uint32_t padmux_32_get();
            inline void padmux_33_set(uint32_t value);
            inline uint32_t padmux_33_get();
            inline void padmux_34_set(uint32_t value);
            inline uint32_t padmux_34_get();
            inline void padmux_35_set(uint32_t value);
            inline uint32_t padmux_35_get();
            inline void padmux_36_set(uint32_t value);
            inline uint32_t padmux_36_get();
            inline void padmux_37_set(uint32_t value);
            inline uint32_t padmux_37_get();
            inline void padmux_38_set(uint32_t value);
            inline uint32_t padmux_38_get();
            inline void padmux_39_set(uint32_t value);
            inline uint32_t padmux_39_get();
            inline void padmux_40_set(uint32_t value);
            inline uint32_t padmux_40_get();
            inline void padmux_41_set(uint32_t value);
            inline uint32_t padmux_41_get();
            inline void padmux_42_set(uint32_t value);
            inline uint32_t padmux_42_get();
            inline void padmux_43_set(uint32_t value);
            inline uint32_t padmux_43_get();
            inline void padmux_44_set(uint32_t value);
            inline uint32_t padmux_44_get();
            inline void padmux_45_set(uint32_t value);
            inline uint32_t padmux_45_get();
            inline void padmux_46_set(uint32_t value);
            inline uint32_t padmux_46_get();
            inline void padmux_47_set(uint32_t value);
            inline uint32_t padmux_47_get();
        };

|

.. _apb_soc_ctrl_PADFUN3:

PADFUN3
"""""""

Mux config register (pad 48-63)

.. table:: 

    +-----+---+---------+-------------------------------------------------+
    |Bit #|R/W|  Name   |                   Description                   |
    +=====+===+=========+=================================================+
    |1:0  |R/W|Padmux_48|Selects between: i2s0_sck / gpio48 / uart2_clk   |
    +-----+---+---------+-------------------------------------------------+
    |3:2  |R/W|Padmux_49|Selects between: i2s0_ws / gpio49                |
    +-----+---+---------+-------------------------------------------------+
    |5:4  |R/W|Padmux_50|Selects between: i2s0_sdi / gpio50               |
    +-----+---+---------+-------------------------------------------------+
    |7:6  |R/W|Padmux_51|Selects between: i2s0_sdo / gpio51               |
    +-----+---+---------+-------------------------------------------------+
    |9:8  |R/W|Padmux_52|Selects between: i2s1_sck / gpio52               |
    +-----+---+---------+-------------------------------------------------+
    |11:10|R/W|Padmux_53|Selects between: i2s1_ws / gpio53 / spi2_cs1     |
    +-----+---+---------+-------------------------------------------------+
    |13:12|R/W|Padmux_54|Selects between: i2s1_sdi / gpio54 / spi2_cs2    |
    +-----+---+---------+-------------------------------------------------+
    |15:14|R/W|Padmux_55|Selects between: i2s1_sdo / gpio55 / spi2_cs3    |
    +-----+---+---------+-------------------------------------------------+
    |17:16|R/W|Padmux_56|Selects between: i2s2_sck / gpio56 / spi2_sck    |
    +-----+---+---------+-------------------------------------------------+
    |19:18|R/W|Padmux_57|Selects between: i2s2_ws / gpio57 / spi2_cs0     |
    +-----+---+---------+-------------------------------------------------+
    |21:20|R/W|Padmux_58|Selects between: i2s2_sdi / gpio58 / spi2_sdi    |
    +-----+---+---------+-------------------------------------------------+
    |23:22|R/W|Padmux_59|Selects between: i2s2_sdo / gpio59 / spi2_sdo    |
    +-----+---+---------+-------------------------------------------------+
    |25:24|R/W|Padmux_60|Selects between: mux_group_sel_uart0_rx / gpio60 |
    +-----+---+---------+-------------------------------------------------+
    |27:26|R/W|Padmux_61|Selects between: mux_group_sel_uart0_tx / gpio61 |
    +-----+---+---------+-------------------------------------------------+
    |29:28|R/W|Padmux_62|Selects between: mux_group_sel_uart0_cts / gpio62|
    +-----+---+---------+-------------------------------------------------+
    |31:30|R/W|Padmux_63|Selects between: mux_group_sel_uart0_rts / gpio63|
    +-----+---+---------+-------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 48-63)
                #define APB_SOC_CTRL_PADFUN3_OFFSET              0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun3_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: i2s0_sck / gpio48 / uart2_clk (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_BIT                           0
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_RESET                         0x0
        
        // Selects between: i2s0_ws / gpio49 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_BIT                           2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_RESET                         0x0
        
        // Selects between: i2s0_sdi / gpio50 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_BIT                           4
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_RESET                         0x0
        
        // Selects between: i2s0_sdo / gpio51 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_BIT                           6
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_RESET                         0x0
        
        // Selects between: i2s1_sck / gpio52 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_BIT                           8
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_RESET                         0x0
        
        // Selects between: i2s1_ws / gpio53 / spi2_cs1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_BIT                           10
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_RESET                         0x0
        
        // Selects between: i2s1_sdi / gpio54 / spi2_cs2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_BIT                           12
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_RESET                         0x0
        
        // Selects between: i2s1_sdo / gpio55 / spi2_cs3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_BIT                           14
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_RESET                         0x0
        
        // Selects between: i2s2_sck / gpio56 / spi2_sck (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_BIT                           16
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_RESET                         0x0
        
        // Selects between: i2s2_ws / gpio57 / spi2_cs0 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_BIT                           18
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_RESET                         0x0
        
        // Selects between: i2s2_sdi / gpio58 / spi2_sdi (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_BIT                           20
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_RESET                         0x0
        
        // Selects between: i2s2_sdo / gpio59 / spi2_sdo (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_BIT                           22
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_rx / gpio60 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_BIT                           24
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_tx / gpio61 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_BIT                           26
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_cts / gpio62 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_BIT                           28
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_RESET                         0x0
        
        // Selects between: mux_group_sel_uart0_rts / gpio63 (access: R/W)
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_BIT                           30
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_48(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_49(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_50(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_51(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_52(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_53(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_54(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_55(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_56(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_57(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_58(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_59(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_60(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_61(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_62(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN3_PADMUX_63(val)                ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_48       :2 ; // Selects between: i2s0_sck / gpio48 / uart2_clk
            unsigned int padmux_49       :2 ; // Selects between: i2s0_ws / gpio49
            unsigned int padmux_50       :2 ; // Selects between: i2s0_sdi / gpio50
            unsigned int padmux_51       :2 ; // Selects between: i2s0_sdo / gpio51
            unsigned int padmux_52       :2 ; // Selects between: i2s1_sck / gpio52
            unsigned int padmux_53       :2 ; // Selects between: i2s1_ws / gpio53 / spi2_cs1
            unsigned int padmux_54       :2 ; // Selects between: i2s1_sdi / gpio54 / spi2_cs2
            unsigned int padmux_55       :2 ; // Selects between: i2s1_sdo / gpio55 / spi2_cs3
            unsigned int padmux_56       :2 ; // Selects between: i2s2_sck / gpio56 / spi2_sck
            unsigned int padmux_57       :2 ; // Selects between: i2s2_ws / gpio57 / spi2_cs0
            unsigned int padmux_58       :2 ; // Selects between: i2s2_sdi / gpio58 / spi2_sdi
            unsigned int padmux_59       :2 ; // Selects between: i2s2_sdo / gpio59 / spi2_sdo
            unsigned int padmux_60       :2 ; // Selects between: mux_group_sel_uart0_rx / gpio60
            unsigned int padmux_61       :2 ; // Selects between: mux_group_sel_uart0_tx / gpio61
            unsigned int padmux_62       :2 ; // Selects between: mux_group_sel_uart0_cts / gpio62
            unsigned int padmux_63       :2 ; // Selects between: mux_group_sel_uart0_rts / gpio63
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun3 : public vp::reg_32
        {
        public:
            inline void padmux_48_set(uint32_t value);
            inline uint32_t padmux_48_get();
            inline void padmux_49_set(uint32_t value);
            inline uint32_t padmux_49_get();
            inline void padmux_50_set(uint32_t value);
            inline uint32_t padmux_50_get();
            inline void padmux_51_set(uint32_t value);
            inline uint32_t padmux_51_get();
            inline void padmux_52_set(uint32_t value);
            inline uint32_t padmux_52_get();
            inline void padmux_53_set(uint32_t value);
            inline uint32_t padmux_53_get();
            inline void padmux_54_set(uint32_t value);
            inline uint32_t padmux_54_get();
            inline void padmux_55_set(uint32_t value);
            inline uint32_t padmux_55_get();
            inline void padmux_56_set(uint32_t value);
            inline uint32_t padmux_56_get();
            inline void padmux_57_set(uint32_t value);
            inline uint32_t padmux_57_get();
            inline void padmux_58_set(uint32_t value);
            inline uint32_t padmux_58_get();
            inline void padmux_59_set(uint32_t value);
            inline uint32_t padmux_59_get();
            inline void padmux_60_set(uint32_t value);
            inline uint32_t padmux_60_get();
            inline void padmux_61_set(uint32_t value);
            inline uint32_t padmux_61_get();
            inline void padmux_62_set(uint32_t value);
            inline uint32_t padmux_62_get();
            inline void padmux_63_set(uint32_t value);
            inline uint32_t padmux_63_get();
        };

|

.. _apb_soc_ctrl_PADFUN4:

PADFUN4
"""""""

Mux config register (pad 64-79)

.. table:: 

    +-----+---+---------+-----------------------------------------------------------+
    |Bit #|R/W|  Name   |                        Description                        |
    +=====+===+=========+===========================================================+
    |1:0  |R/W|Padmux_64|Selects between: uart0_clk / gpio64                        |
    +-----+---+---------+-----------------------------------------------------------+
    |3:2  |R/W|Padmux_65|Selects between: mux_group_sel_uart1_rx / gpio65           |
    +-----+---+---------+-----------------------------------------------------------+
    |5:4  |R/W|Padmux_66|Selects between: mux_group_sel_uart1_tx / gpio66           |
    +-----+---+---------+-----------------------------------------------------------+
    |7:6  |R/W|Padmux_67|Selects between: mux_group_sel_pwm0 / gpio67               |
    +-----+---+---------+-----------------------------------------------------------+
    |9:8  |R/W|Padmux_68|Selects between: mux_group_sel_pwm1 / gpio68               |
    +-----+---+---------+-----------------------------------------------------------+
    |11:10|R/W|Padmux_69|Selects between: uart1_clk / gpio69                        |
    +-----+---+---------+-----------------------------------------------------------+
    |13:12|R/W|Padmux_70|Selects between: cam_pclk / gpio70 / spi3_sck              |
    +-----+---+---------+-----------------------------------------------------------+
    |15:14|R/W|Padmux_71|Selects between: cam_hsync / gpio71 / spi3_cs0 / csi2_hsync|
    +-----+---+---------+-----------------------------------------------------------+
    |17:16|R/W|Padmux_72|Selects between: cam_data0 / gpio72 / spi3_cs1             |
    +-----+---+---------+-----------------------------------------------------------+
    |19:18|R/W|Padmux_73|Selects between: cam_data1 / gpio73 / spi3_cs2             |
    +-----+---+---------+-----------------------------------------------------------+
    |21:20|R/W|Padmux_74|Selects between: cam_data2 / gpio74 / spi3_cs3             |
    +-----+---+---------+-----------------------------------------------------------+
    |23:22|R/W|Padmux_75|Selects between: cam_data3 / gpio75 / spi3_sdo             |
    +-----+---+---------+-----------------------------------------------------------+
    |25:24|R/W|Padmux_76|Selects between: cam_data4 / gpio76 / spi3_sdi             |
    +-----+---+---------+-----------------------------------------------------------+
    |27:26|R/W|Padmux_77|Selects between: cam_data5 / gpio77 / observability1       |
    +-----+---+---------+-----------------------------------------------------------+
    |29:28|R/W|Padmux_78|Selects between: cam_data6 / gpio78 / observability2       |
    +-----+---+---------+-----------------------------------------------------------+
    |31:30|R/W|Padmux_79|Selects between: cam_data7 / gpio79 / observability3       |
    +-----+---+---------+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 64-79)
                #define APB_SOC_CTRL_PADFUN4_OFFSET              0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun4_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: uart0_clk / gpio64 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_BIT                           0
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_RESET                         0x0
        
        // Selects between: mux_group_sel_uart1_rx / gpio65 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_BIT                           2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_RESET                         0x0
        
        // Selects between: mux_group_sel_uart1_tx / gpio66 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_BIT                           4
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_RESET                         0x0
        
        // Selects between: mux_group_sel_pwm0 / gpio67 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_BIT                           6
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_RESET                         0x0
        
        // Selects between: mux_group_sel_pwm1 / gpio68 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_BIT                           8
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_RESET                         0x0
        
        // Selects between: uart1_clk / gpio69 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_BIT                           10
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_RESET                         0x0
        
        // Selects between: cam_pclk / gpio70 / spi3_sck (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_BIT                           12
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_RESET                         0x0
        
        // Selects between: cam_hsync / gpio71 / spi3_cs0 / csi2_hsync (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_BIT                           14
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_RESET                         0x0
        
        // Selects between: cam_data0 / gpio72 / spi3_cs1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_BIT                           16
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_RESET                         0x0
        
        // Selects between: cam_data1 / gpio73 / spi3_cs2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_BIT                           18
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_RESET                         0x0
        
        // Selects between: cam_data2 / gpio74 / spi3_cs3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_BIT                           20
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_MASK                          0x300000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_RESET                         0x0
        
        // Selects between: cam_data3 / gpio75 / spi3_sdo (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_BIT                           22
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_MASK                          0xc00000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_RESET                         0x0
        
        // Selects between: cam_data4 / gpio76 / spi3_sdi (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_BIT                           24
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_MASK                          0x3000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_RESET                         0x0
        
        // Selects between: cam_data5 / gpio77 / observability1 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_BIT                           26
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_MASK                          0xc000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_RESET                         0x0
        
        // Selects between: cam_data6 / gpio78 / observability2 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_BIT                           28
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_MASK                          0x30000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_RESET                         0x0
        
        // Selects between: cam_data7 / gpio79 / observability3 (access: R/W)
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_BIT                           30
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_MASK                          0xc0000000
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_64(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_65(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_66(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_67(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_68(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_69(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_70(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_71(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_72(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_73(val)                ((val) << 18)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_74(val)                ((val) << 20)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_GET(value)          (GAP_BEXTRACTU((value),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_GETS(value)         (GAP_BEXTRACT((value),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75_SET(value,field)    (GAP_BINSERT((value),(field),2,22))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_75(val)                ((val) << 22)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_GET(value)          (GAP_BEXTRACTU((value),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_GETS(value)         (GAP_BEXTRACT((value),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76_SET(value,field)    (GAP_BINSERT((value),(field),2,24))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_76(val)                ((val) << 24)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_GET(value)          (GAP_BEXTRACTU((value),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_GETS(value)         (GAP_BEXTRACT((value),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77_SET(value,field)    (GAP_BINSERT((value),(field),2,26))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_77(val)                ((val) << 26)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_GET(value)          (GAP_BEXTRACTU((value),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_GETS(value)         (GAP_BEXTRACT((value),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78_SET(value,field)    (GAP_BINSERT((value),(field),2,28))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_78(val)                ((val) << 28)
        
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_GET(value)          (GAP_BEXTRACTU((value),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_GETS(value)         (GAP_BEXTRACT((value),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79_SET(value,field)    (GAP_BINSERT((value),(field),2,30))
        #define APB_SOC_CTRL_PADFUN4_PADMUX_79(val)                ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_64       :2 ; // Selects between: uart0_clk / gpio64
            unsigned int padmux_65       :2 ; // Selects between: mux_group_sel_uart1_rx / gpio65
            unsigned int padmux_66       :2 ; // Selects between: mux_group_sel_uart1_tx / gpio66
            unsigned int padmux_67       :2 ; // Selects between: mux_group_sel_pwm0 / gpio67
            unsigned int padmux_68       :2 ; // Selects between: mux_group_sel_pwm1 / gpio68
            unsigned int padmux_69       :2 ; // Selects between: uart1_clk / gpio69
            unsigned int padmux_70       :2 ; // Selects between: cam_pclk / gpio70 / spi3_sck
            unsigned int padmux_71       :2 ; // Selects between: cam_hsync / gpio71 / spi3_cs0 / csi2_hsync
            unsigned int padmux_72       :2 ; // Selects between: cam_data0 / gpio72 / spi3_cs1
            unsigned int padmux_73       :2 ; // Selects between: cam_data1 / gpio73 / spi3_cs2
            unsigned int padmux_74       :2 ; // Selects between: cam_data2 / gpio74 / spi3_cs3
            unsigned int padmux_75       :2 ; // Selects between: cam_data3 / gpio75 / spi3_sdo
            unsigned int padmux_76       :2 ; // Selects between: cam_data4 / gpio76 / spi3_sdi
            unsigned int padmux_77       :2 ; // Selects between: cam_data5 / gpio77 / observability1
            unsigned int padmux_78       :2 ; // Selects between: cam_data6 / gpio78 / observability2
            unsigned int padmux_79       :2 ; // Selects between: cam_data7 / gpio79 / observability3
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun4 : public vp::reg_32
        {
        public:
            inline void padmux_64_set(uint32_t value);
            inline uint32_t padmux_64_get();
            inline void padmux_65_set(uint32_t value);
            inline uint32_t padmux_65_get();
            inline void padmux_66_set(uint32_t value);
            inline uint32_t padmux_66_get();
            inline void padmux_67_set(uint32_t value);
            inline uint32_t padmux_67_get();
            inline void padmux_68_set(uint32_t value);
            inline uint32_t padmux_68_get();
            inline void padmux_69_set(uint32_t value);
            inline uint32_t padmux_69_get();
            inline void padmux_70_set(uint32_t value);
            inline uint32_t padmux_70_get();
            inline void padmux_71_set(uint32_t value);
            inline uint32_t padmux_71_get();
            inline void padmux_72_set(uint32_t value);
            inline uint32_t padmux_72_get();
            inline void padmux_73_set(uint32_t value);
            inline uint32_t padmux_73_get();
            inline void padmux_74_set(uint32_t value);
            inline uint32_t padmux_74_get();
            inline void padmux_75_set(uint32_t value);
            inline uint32_t padmux_75_get();
            inline void padmux_76_set(uint32_t value);
            inline uint32_t padmux_76_get();
            inline void padmux_77_set(uint32_t value);
            inline uint32_t padmux_77_get();
            inline void padmux_78_set(uint32_t value);
            inline uint32_t padmux_78_get();
            inline void padmux_79_set(uint32_t value);
            inline uint32_t padmux_79_get();
        };

|

.. _apb_soc_ctrl_PADFUN5:

PADFUN5
"""""""

Mux config register (pad 80-95)

.. table:: 

    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                             Description                                                             |
    +=====+===+=========+=====================================================================================================================================+
    |1:0  |R/W|Padmux_80|Selects between: cam_vsync / gpio80 / observability4 / csi2_vsync                                                                    |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |3:2  |R/W|Padmux_81|Selects between: jtag_tck / gpio81 / uart4_clk (JTAG has to be disabled to use alternates (gpio81/uart4_clk) see JTAG_DISABLE below) |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |5:4  |R/W|Padmux_82|Selects between: jtag_tdi / gpio82 / uart4_rx (JTAG has to be disabled to use alternates (gpio82/uart4_rx) see JTAG_DISABLE below)   |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |7:6  |R/W|Padmux_83|Selects between: jtag_tdo / gpio83 / uart4_tx (JTAG has to be disabled to use alternates (gpio83/uart4_tx) see JTAG_DISABLE below)   |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |9:8  |R/W|Padmux_84|Selects between: jtag_tms / gpio84 / uart4_cts (JTAG has to be disabled to use alternates (gpio84/uart4_cts) see JTAG_DISABLE below) |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |11:10|R/W|Padmux_85|Selects between: jtag_trst / gpio85 / uart4_rts (JTAG has to be disabled to use alternates (gpio85/uart4_rts) see JTAG_DISABLE below)|
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |13:12|R/W|Padmux_86|Selects between: wakeup_spi2_sck / gpio86                                                                                            |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |15:14|R/W|Padmux_87|Selects between: wakeup_spi2_sdi / gpio87                                                                                            |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |17:16|R/W|Padmux_88|Selects between: wakeup_spi2_sdo / gpio88                                                                                            |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+
    |19:18|R/W|Padmux_89|Selects between: wakeup_spi2_cs0 / gpio89                                                                                            |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Mux config register (pad 80-95)
                #define APB_SOC_CTRL_PADFUN5_OFFSET              0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padfun5_get(uint32_t base);
        static inline void apb_soc_ctrl_padfun5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects between: cam_vsync / gpio80 / observability4 / csi2_vsync (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_BIT                           0
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_MASK                          0x3
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_RESET                         0x0
        
        // Selects between: jtag_tck / gpio81 / uart4_clk (JTAG has to be disabled to use alternates (gpio81/uart4_clk) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_BIT                           2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_MASK                          0xc
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_RESET                         0x0
        
        // Selects between: jtag_tdi / gpio82 / uart4_rx (JTAG has to be disabled to use alternates (gpio82/uart4_rx) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_BIT                           4
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_MASK                          0x30
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_RESET                         0x0
        
        // Selects between: jtag_tdo / gpio83 / uart4_tx (JTAG has to be disabled to use alternates (gpio83/uart4_tx) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_BIT                           6
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_MASK                          0xc0
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_RESET                         0x0
        
        // Selects between: jtag_tms / gpio84 / uart4_cts (JTAG has to be disabled to use alternates (gpio84/uart4_cts) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_BIT                           8
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_MASK                          0x300
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_RESET                         0x0
        
        // Selects between: jtag_trst / gpio85 / uart4_rts (JTAG has to be disabled to use alternates (gpio85/uart4_rts) see JTAG_DISABLE below) (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_BIT                           10
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_MASK                          0xc00
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_RESET                         0x0
        
        // Selects between: wakeup_spi2_sck / gpio86 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_BIT                           12
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_MASK                          0x3000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_RESET                         0x0
        
        // Selects between: wakeup_spi2_sdi / gpio87 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_BIT                           14
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_MASK                          0xc000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_RESET                         0x0
        
        // Selects between: wakeup_spi2_sdo / gpio88 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_BIT                           16
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_MASK                          0x30000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_RESET                         0x0
        
        // Selects between: wakeup_spi2_cs0 / gpio89 (access: R/W)
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_BIT                           18
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_WIDTH                         2
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_MASK                          0xc0000
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_GET(value)          (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_GETS(value)         (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80_SET(value,field)    (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_80(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_GET(value)          (GAP_BEXTRACTU((value),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_GETS(value)         (GAP_BEXTRACT((value),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81_SET(value,field)    (GAP_BINSERT((value),(field),2,2))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_81(val)                ((val) << 2)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_GET(value)          (GAP_BEXTRACTU((value),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_GETS(value)         (GAP_BEXTRACT((value),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82_SET(value,field)    (GAP_BINSERT((value),(field),2,4))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_82(val)                ((val) << 4)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_GET(value)          (GAP_BEXTRACTU((value),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_GETS(value)         (GAP_BEXTRACT((value),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83_SET(value,field)    (GAP_BINSERT((value),(field),2,6))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_83(val)                ((val) << 6)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_GET(value)          (GAP_BEXTRACTU((value),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_GETS(value)         (GAP_BEXTRACT((value),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84_SET(value,field)    (GAP_BINSERT((value),(field),2,8))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_84(val)                ((val) << 8)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_GET(value)          (GAP_BEXTRACTU((value),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_GETS(value)         (GAP_BEXTRACT((value),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85_SET(value,field)    (GAP_BINSERT((value),(field),2,10))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_85(val)                ((val) << 10)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_GET(value)          (GAP_BEXTRACTU((value),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_GETS(value)         (GAP_BEXTRACT((value),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86_SET(value,field)    (GAP_BINSERT((value),(field),2,12))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_86(val)                ((val) << 12)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_GET(value)          (GAP_BEXTRACTU((value),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_GETS(value)         (GAP_BEXTRACT((value),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87_SET(value,field)    (GAP_BINSERT((value),(field),2,14))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_87(val)                ((val) << 14)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_GET(value)          (GAP_BEXTRACTU((value),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_GETS(value)         (GAP_BEXTRACT((value),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88_SET(value,field)    (GAP_BINSERT((value),(field),2,16))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_88(val)                ((val) << 16)
        
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define APB_SOC_CTRL_PADFUN5_PADMUX_89(val)                ((val) << 18)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padmux_80       :2 ; // Selects between: cam_vsync / gpio80 / observability4 / csi2_vsync
            unsigned int padmux_81       :2 ; // Selects between: jtag_tck / gpio81 / uart4_clk (JTAG has to be disabled to use alternates (gpio81/uart4_clk) see JTAG_DISABLE below)
            unsigned int padmux_82       :2 ; // Selects between: jtag_tdi / gpio82 / uart4_rx (JTAG has to be disabled to use alternates (gpio82/uart4_rx) see JTAG_DISABLE below)
            unsigned int padmux_83       :2 ; // Selects between: jtag_tdo / gpio83 / uart4_tx (JTAG has to be disabled to use alternates (gpio83/uart4_tx) see JTAG_DISABLE below)
            unsigned int padmux_84       :2 ; // Selects between: jtag_tms / gpio84 / uart4_cts (JTAG has to be disabled to use alternates (gpio84/uart4_cts) see JTAG_DISABLE below)
            unsigned int padmux_85       :2 ; // Selects between: jtag_trst / gpio85 / uart4_rts (JTAG has to be disabled to use alternates (gpio85/uart4_rts) see JTAG_DISABLE below)
            unsigned int padmux_86       :2 ; // Selects between: wakeup_spi2_sck / gpio86
            unsigned int padmux_87       :2 ; // Selects between: wakeup_spi2_sdi / gpio87
            unsigned int padmux_88       :2 ; // Selects between: wakeup_spi2_sdo / gpio88
            unsigned int padmux_89       :2 ; // Selects between: wakeup_spi2_cs0 / gpio89
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padfun5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padfun5 : public vp::reg_32
        {
        public:
            inline void padmux_80_set(uint32_t value);
            inline uint32_t padmux_80_get();
            inline void padmux_81_set(uint32_t value);
            inline uint32_t padmux_81_get();
            inline void padmux_82_set(uint32_t value);
            inline uint32_t padmux_82_get();
            inline void padmux_83_set(uint32_t value);
            inline uint32_t padmux_83_get();
            inline void padmux_84_set(uint32_t value);
            inline uint32_t padmux_84_get();
            inline void padmux_85_set(uint32_t value);
            inline uint32_t padmux_85_get();
            inline void padmux_86_set(uint32_t value);
            inline uint32_t padmux_86_get();
            inline void padmux_87_set(uint32_t value);
            inline uint32_t padmux_87_get();
            inline void padmux_88_set(uint32_t value);
            inline uint32_t padmux_88_get();
            inline void padmux_89_set(uint32_t value);
            inline uint32_t padmux_89_get();
        };

|

.. _apb_soc_ctrl_RESERVED0:

RESERVED0
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED0_OFFSET            0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved0_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved0 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED1:

RESERVED1
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED1_OFFSET            0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved1_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved1 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG0:

PADCFG0
"""""""

Function register (pad 0 to 3)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 0 to 3)
                #define APB_SOC_CTRL_PADCFG0_OFFSET              0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg0_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg0 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG1:

PADCFG1
"""""""

Function register (pad 4 to 7)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 4 to 7)
                #define APB_SOC_CTRL_PADCFG1_OFFSET              0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg1_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg1 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG2:

PADCFG2
"""""""

Function register (pad 8 to 11)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 8 to 11)
                #define APB_SOC_CTRL_PADCFG2_OFFSET              0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg2_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg2 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG3:

PADCFG3
"""""""

Function register (pad 12 to 15)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 12 to 15)
                #define APB_SOC_CTRL_PADCFG3_OFFSET              0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg3_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg3 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG4:

PADCFG4
"""""""

Function register (pad 16 to 19)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 16 to 19)
                #define APB_SOC_CTRL_PADCFG4_OFFSET              0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg4_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg4 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG5:

PADCFG5
"""""""

Function register (pad 20 to 23)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 20 to 23)
                #define APB_SOC_CTRL_PADCFG5_OFFSET              0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg5_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg5 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG6:

PADCFG6
"""""""

Function register (pad 24 to 27)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 24 to 27)
                #define APB_SOC_CTRL_PADCFG6_OFFSET              0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg6_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg6 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG7:

PADCFG7
"""""""

Function register (pad 28 to 31)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 28 to 31)
                #define APB_SOC_CTRL_PADCFG7_OFFSET              0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg7_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg7 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG8:

PADCFG8
"""""""

Function register (pad 32 to 35)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 32 to 35)
                #define APB_SOC_CTRL_PADCFG8_OFFSET              0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg8_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg8 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG9:

PADCFG9
"""""""

Function register (pad 36 to 39)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 36 to 39)
                #define APB_SOC_CTRL_PADCFG9_OFFSET              0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg9_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg9 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG10:

PADCFG10
""""""""

Function register (pad 40 to 43)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 40 to 43)
                #define APB_SOC_CTRL_PADCFG10_OFFSET             0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg10_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg10_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg10_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg10 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG11:

PADCFG11
""""""""

Function register (pad 44 to 47)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 44 to 47)
                #define APB_SOC_CTRL_PADCFG11_OFFSET             0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg11_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg11 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG12:

PADCFG12
""""""""

Function register (pad 48 to 51)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 48 to 51)
                #define APB_SOC_CTRL_PADCFG12_OFFSET             0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg12_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg12 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG13:

PADCFG13
""""""""

Function register (pad 52 to 55)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 52 to 55)
                #define APB_SOC_CTRL_PADCFG13_OFFSET             0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg13_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg13 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG14:

PADCFG14
""""""""

Function register (pad 56 to 59)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 56 to 59)
                #define APB_SOC_CTRL_PADCFG14_OFFSET             0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg14_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg14_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg14_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg14 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG15:

PADCFG15
""""""""

Function register (pad 60 to 63)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 60 to 63)
                #define APB_SOC_CTRL_PADCFG15_OFFSET             0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg15_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg15 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG16:

PADCFG16
""""""""

Function register (pad 64 to 67)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 64 to 67)
                #define APB_SOC_CTRL_PADCFG16_OFFSET             0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg16_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg16_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg16_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg16 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG17:

PADCFG17
""""""""

Function register (pad 68 to 71)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 68 to 71)
                #define APB_SOC_CTRL_PADCFG17_OFFSET             0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg17_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg17_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg17_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg17 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG18:

PADCFG18
""""""""

Function register (pad 72 to 75)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 72 to 75)
                #define APB_SOC_CTRL_PADCFG18_OFFSET             0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg18_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg18_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg18_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg18 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG19:

PADCFG19
""""""""

Function register (pad 76 to 79)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 76 to 79)
                #define APB_SOC_CTRL_PADCFG19_OFFSET             0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg19_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg19_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg19_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg19 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG20:

PADCFG20
""""""""

Function register (pad 80 to 83)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 80 to 83)
                #define APB_SOC_CTRL_PADCFG20_OFFSET             0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg20_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg20_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg20_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg20 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG21:

PADCFG21
""""""""

Function register (pad 84 to 87)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 84 to 87)
                #define APB_SOC_CTRL_PADCFG21_OFFSET             0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg21_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg21_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg21_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg21 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG22:

PADCFG22
""""""""

Function register (pad 88 to 91)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 88 to 91)
                #define APB_SOC_CTRL_PADCFG22_OFFSET             0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg22_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg22_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg22_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg22 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADCFG23:

PADCFG23
""""""""

Function register (pad 92 to 95)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Function register (pad 92 to 95)
                #define APB_SOC_CTRL_PADCFG23_OFFSET             0x8c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padcfg23_get(uint32_t base);
        static inline void apb_soc_ctrl_padcfg23_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padcfg23_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padcfg23 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_REG_REPROG_PAD0:

REG_REPROG_PAD0
"""""""""""""""

Controls reprogrammable pads 27,28,29,30,34

.. table:: 

    +-----+---+----------------------+--------------------------------------------------------------------+
    |Bit #|R/W|         Name         |                            Description                             |
    +=====+===+======================+====================================================================+
    |5:0  |R/W|mux_group_sel_spi0_cs0|Selects an alternate from the mux group. Default alternate: spi0_cs0|
    +-----+---+----------------------+--------------------------------------------------------------------+
    |11:6 |R/W|mux_group_sel_spi0_cs1|Selects an alternate from the mux group. Default alternate: spi0_cs1|
    +-----+---+----------------------+--------------------------------------------------------------------+
    |17:12|R/W|mux_group_sel_spi0_cs2|Selects an alternate from the mux group. Default alternate: spi0_cs2|
    +-----+---+----------------------+--------------------------------------------------------------------+
    |23:18|R/W|mux_group_sel_spi0_cs3|Selects an alternate from the mux group. Default alternate: spi0_cs3|
    +-----+---+----------------------+--------------------------------------------------------------------+
    |29:24|R/W|mux_group_sel_spi1_cs0|Selects an alternate from the mux group. Default alternate: spi1_cs0|
    +-----+---+----------------------+--------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls reprogrammable pads 27,28,29,30,34
                #define APB_SOC_CTRL_REG_REPROG_PAD0_OFFSET      0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reg_reprog_pad0_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects an alternate from the mux group. Default alternate: spi0_cs0 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_RESET    0x0
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs1 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_RESET    0x1
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs2 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_RESET    0x2
        
        // Selects an alternate from the mux group. Default alternate: spi0_cs3 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_RESET    0x3
        
        // Selects an alternate from the mux group. Default alternate: spi1_cs0 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_BIT      24
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_MASK     0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_RESET    0x4

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS0(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS1(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS2(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI0_CS3(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD0_MUX_GROUP_SEL_SPI1_CS0(val) ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mux_group_sel_spi0_cs0:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs0
            unsigned int mux_group_sel_spi0_cs1:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs1
            unsigned int mux_group_sel_spi0_cs2:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs2
            unsigned int mux_group_sel_spi0_cs3:6 ; // Selects an alternate from the mux group. Default alternate: spi0_cs3
            unsigned int mux_group_sel_spi1_cs0:6 ; // Selects an alternate from the mux group. Default alternate: spi1_cs0
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reg_reprog_pad0 : public vp::reg_32
        {
        public:
            inline void mux_group_sel_spi0_cs0_set(uint32_t value);
            inline uint32_t mux_group_sel_spi0_cs0_get();
            inline void mux_group_sel_spi0_cs1_set(uint32_t value);
            inline uint32_t mux_group_sel_spi0_cs1_get();
            inline void mux_group_sel_spi0_cs2_set(uint32_t value);
            inline uint32_t mux_group_sel_spi0_cs2_get();
            inline void mux_group_sel_spi0_cs3_set(uint32_t value);
            inline uint32_t mux_group_sel_spi0_cs3_get();
            inline void mux_group_sel_spi1_cs0_set(uint32_t value);
            inline uint32_t mux_group_sel_spi1_cs0_get();
        };

|

.. _apb_soc_ctrl_REG_REPROG_PAD1:

REG_REPROG_PAD1
"""""""""""""""

Controls reprogrammable pads 35,40,41,42,43

.. table:: 

    +-----+---+----------------------+----------------------------------------------------------------------+
    |Bit #|R/W|         Name         |                             Description                              |
    +=====+===+======================+======================================================================+
    |5:0  |R/W|mux_group_sel_spi1_cs1|Selects an alternate from the mux group. Default alternate: spi1_cs1  |
    +-----+---+----------------------+----------------------------------------------------------------------+
    |11:6 |R/W|mux_group_sel_i2c0_sda|Selects an alternate from the mux group. Default alternate: i2c0_sda  |
    +-----+---+----------------------+----------------------------------------------------------------------+
    |17:12|R/W|mux_group_sel_i2c0_scl|Selects an alternate from the mux group. Default alternate: i2c0_scl  |
    +-----+---+----------------------+----------------------------------------------------------------------+
    |23:18|R/W|mux_group_sel_i2c1_sda|Selects an alternate from the mux group. Default alternate: i2c1_sda_o|
    +-----+---+----------------------+----------------------------------------------------------------------+
    |29:24|R/W|mux_group_sel_i2c1_scl|Selects an alternate from the mux group. Default alternate: i2c1_scl_o|
    +-----+---+----------------------+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls reprogrammable pads 35,40,41,42,43
                #define APB_SOC_CTRL_REG_REPROG_PAD1_OFFSET      0x94

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reg_reprog_pad1_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects an alternate from the mux group. Default alternate: spi1_cs1 (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_RESET    0x6
        
        // Selects an alternate from the mux group. Default alternate: i2c0_sda (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_RESET    0x8
        
        // Selects an alternate from the mux group. Default alternate: i2c0_scl (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_RESET    0x9
        
        // Selects an alternate from the mux group. Default alternate: i2c1_sda_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_RESET    0xa
        
        // Selects an alternate from the mux group. Default alternate: i2c1_scl_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_BIT      24
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_MASK     0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_RESET    0xc

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_SPI1_CS1(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SDA(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C0_SCL(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SDA(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD1_MUX_GROUP_SEL_I2C1_SCL(val) ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mux_group_sel_spi1_cs1:6 ; // Selects an alternate from the mux group. Default alternate: spi1_cs1
            unsigned int mux_group_sel_i2c0_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c0_sda
            unsigned int mux_group_sel_i2c0_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c0_scl
            unsigned int mux_group_sel_i2c1_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c1_sda_o
            unsigned int mux_group_sel_i2c1_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c1_scl_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reg_reprog_pad1 : public vp::reg_32
        {
        public:
            inline void mux_group_sel_spi1_cs1_set(uint32_t value);
            inline uint32_t mux_group_sel_spi1_cs1_get();
            inline void mux_group_sel_i2c0_sda_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c0_sda_get();
            inline void mux_group_sel_i2c0_scl_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c0_scl_get();
            inline void mux_group_sel_i2c1_sda_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c1_sda_get();
            inline void mux_group_sel_i2c1_scl_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c1_scl_get();
        };

|

.. _apb_soc_ctrl_REG_REPROG_PAD2:

REG_REPROG_PAD2
"""""""""""""""

Controls reprogrammable pads 44,45,60,61,62

.. table:: 

    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |Bit #|R/W|         Name          |                              Description                              |
    +=====+===+=======================+=======================================================================+
    |5:0  |R/W|mux_group_sel_i2c2_sda |Selects an alternate from the mux group. Default alternate: i2c2_sda_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |11:6 |R/W|mux_group_sel_i2c2_scl |Selects an alternate from the mux group. Default alternate: i2c2_scl_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |17:12|R/W|mux_group_sel_uart0_rx |Selects an alternate from the mux group. Default alternate: uart0_rx_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |23:18|R/W|mux_group_sel_uart0_tx |Selects an alternate from the mux group. Default alternate: uart0_tx_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |29:24|R/W|mux_group_sel_uart0_cts|Selects an alternate from the mux group. Default alternate: uart0_cts_o|
    +-----+---+-----------------------+-----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls reprogrammable pads 44,45,60,61,62
                #define APB_SOC_CTRL_REG_REPROG_PAD2_OFFSET      0x98

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reg_reprog_pad2_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects an alternate from the mux group. Default alternate: i2c2_sda_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_BIT      0
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_MASK     0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_RESET    0xe
        
        // Selects an alternate from the mux group. Default alternate: i2c2_scl_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_RESET    0x10
        
        // Selects an alternate from the mux group. Default alternate: uart0_rx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_RESET    0x12
        
        // Selects an alternate from the mux group. Default alternate: uart0_tx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_BIT      18
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_MASK     0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_RESET    0x13
        
        // Selects an alternate from the mux group. Default alternate: uart0_cts_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_BIT     24
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_WIDTH   6
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_MASK    0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_RESET   0x14

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SDA(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_I2C2_SCL(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_RX(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_TX(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD2_MUX_GROUP_SEL_UART0_CTS(val) ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mux_group_sel_i2c2_sda:6 ; // Selects an alternate from the mux group. Default alternate: i2c2_sda_o
            unsigned int mux_group_sel_i2c2_scl:6 ; // Selects an alternate from the mux group. Default alternate: i2c2_scl_o
            unsigned int mux_group_sel_uart0_rx:6 ; // Selects an alternate from the mux group. Default alternate: uart0_rx_o
            unsigned int mux_group_sel_uart0_tx:6 ; // Selects an alternate from the mux group. Default alternate: uart0_tx_o
            unsigned int mux_group_sel_uart0_cts:6 ; // Selects an alternate from the mux group. Default alternate: uart0_cts_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reg_reprog_pad2 : public vp::reg_32
        {
        public:
            inline void mux_group_sel_i2c2_sda_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c2_sda_get();
            inline void mux_group_sel_i2c2_scl_set(uint32_t value);
            inline uint32_t mux_group_sel_i2c2_scl_get();
            inline void mux_group_sel_uart0_rx_set(uint32_t value);
            inline uint32_t mux_group_sel_uart0_rx_get();
            inline void mux_group_sel_uart0_tx_set(uint32_t value);
            inline uint32_t mux_group_sel_uart0_tx_get();
            inline void mux_group_sel_uart0_cts_set(uint32_t value);
            inline uint32_t mux_group_sel_uart0_cts_get();
        };

|

.. _apb_soc_ctrl_REG_REPROG_PAD3:

REG_REPROG_PAD3
"""""""""""""""

Controls reprogrammable pads 63,65,66,67,68

.. table:: 

    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |Bit #|R/W|         Name          |                              Description                              |
    +=====+===+=======================+=======================================================================+
    |5:0  |R/W|mux_group_sel_uart0_rts|Selects an alternate from the mux group. Default alternate: uart0_rts_o|
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |11:6 |R/W|mux_group_sel_uart1_rx |Selects an alternate from the mux group. Default alternate: uart1_rx_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |17:12|R/W|mux_group_sel_uart1_tx |Selects an alternate from the mux group. Default alternate: uart1_tx_o |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |23:18|R/W|mux_group_sel_pwm0     |Selects an alternate from the mux group. Default alternate: pwm0_o     |
    +-----+---+-----------------------+-----------------------------------------------------------------------+
    |29:24|R/W|mux_group_sel_pwm1     |Selects an alternate from the mux group. Default alternate: pwm1_o     |
    +-----+---+-----------------------+-----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls reprogrammable pads 63,65,66,67,68
                #define APB_SOC_CTRL_REG_REPROG_PAD3_OFFSET      0x9c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reg_reprog_pad3_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_reprog_pad3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Selects an alternate from the mux group. Default alternate: uart0_rts_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_BIT     0
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_WIDTH   6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_MASK    0x3f
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_RESET   0x16
        
        // Selects an alternate from the mux group. Default alternate: uart1_rx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_BIT      6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_MASK     0xfc0
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_RESET    0x18
        
        // Selects an alternate from the mux group. Default alternate: uart1_tx_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_BIT      12
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_WIDTH    6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_MASK     0x3f000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_RESET    0x1b
        
        // Selects an alternate from the mux group. Default alternate: pwm0_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_BIT          18
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_WIDTH        6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_MASK         0xfc0000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_RESET        0x1e
        
        // Selects an alternate from the mux group. Default alternate: pwm1_o (access: R/W)
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_BIT          24
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_WIDTH        6
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_MASK         0x3f000000
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_RESET        0x21

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_GET(value) (GAP_BEXTRACTU((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_GETS(value) (GAP_BEXTRACT((value),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS_SET(value,field) (GAP_BINSERT((value),(field),6,0))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART0_RTS(val) ((val) << 0)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_GET(value) (GAP_BEXTRACTU((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_GETS(value) (GAP_BEXTRACT((value),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX_SET(value,field) (GAP_BINSERT((value),(field),6,6))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_RX(val) ((val) << 6)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_GET(value) (GAP_BEXTRACTU((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_GETS(value) (GAP_BEXTRACT((value),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX_SET(value,field) (GAP_BINSERT((value),(field),6,12))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_UART1_TX(val) ((val) << 12)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_GET(value) (GAP_BEXTRACTU((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_GETS(value) (GAP_BEXTRACT((value),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0_SET(value,field) (GAP_BINSERT((value),(field),6,18))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM0(val) ((val) << 18)
        
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_GET(value) (GAP_BEXTRACTU((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_GETS(value) (GAP_BEXTRACT((value),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1_SET(value,field) (GAP_BINSERT((value),(field),6,24))
        #define APB_SOC_CTRL_REG_REPROG_PAD3_MUX_GROUP_SEL_PWM1(val) ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mux_group_sel_uart0_rts:6 ; // Selects an alternate from the mux group. Default alternate: uart0_rts_o
            unsigned int mux_group_sel_uart1_rx:6 ; // Selects an alternate from the mux group. Default alternate: uart1_rx_o
            unsigned int mux_group_sel_uart1_tx:6 ; // Selects an alternate from the mux group. Default alternate: uart1_tx_o
            unsigned int mux_group_sel_pwm0:6 ; // Selects an alternate from the mux group. Default alternate: pwm0_o
            unsigned int mux_group_sel_pwm1:6 ; // Selects an alternate from the mux group. Default alternate: pwm1_o
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_reprog_pad3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reg_reprog_pad3 : public vp::reg_32
        {
        public:
            inline void mux_group_sel_uart0_rts_set(uint32_t value);
            inline uint32_t mux_group_sel_uart0_rts_get();
            inline void mux_group_sel_uart1_rx_set(uint32_t value);
            inline uint32_t mux_group_sel_uart1_rx_get();
            inline void mux_group_sel_uart1_tx_set(uint32_t value);
            inline uint32_t mux_group_sel_uart1_tx_get();
            inline void mux_group_sel_pwm0_set(uint32_t value);
            inline uint32_t mux_group_sel_pwm0_get();
            inline void mux_group_sel_pwm1_set(uint32_t value);
            inline uint32_t mux_group_sel_pwm1_get();
        };

|

.. _apb_soc_ctrl_RESERVED2:

RESERVED2
"""""""""

Reserved

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                   Description                                    |
    +=====+===+====+==================================================================================+
    |    0|R  |BUSY|Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)|
    +-----+---+----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED2_OFFSET            0xa0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved2_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED2_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED2_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED2_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED2_BUSY_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_RESERVED2_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED2_BUSY(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved2 : public vp::reg_32
        {
        public:
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
        };

|

.. _apb_soc_ctrl_RESERVED3:

RESERVED3
"""""""""

Reserved

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                   Description                                    |
    +=====+===+====+==================================================================================+
    |    0|R  |BUSY|Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)|
    +-----+---+----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED3_OFFSET            0xa4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved3_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED3_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED3_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED3_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED3_BUSY_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_RESERVED3_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED3_BUSY(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved3 : public vp::reg_32
        {
        public:
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
        };

|

.. _apb_soc_ctrl_RESERVED4:

RESERVED4
"""""""""

Reserved

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                   Description                                    |
    +=====+===+====+==================================================================================+
    |    0|R  |BUSY|Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)|
    +-----+---+----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED4_OFFSET            0xa8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved4_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_RESERVED4_BUSY_BIT                              0
        #define APB_SOC_CTRL_RESERVED4_BUSY_WIDTH                            1
        #define APB_SOC_CTRL_RESERVED4_BUSY_MASK                             0x1
        #define APB_SOC_CTRL_RESERVED4_BUSY_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_RESERVED4_BUSY_GET(value)             (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY_GETS(value)            (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY_SET(value,field)       (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_RESERVED4_BUSY(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved4 : public vp::reg_32
        {
        public:
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
        };

|

.. _apb_soc_ctrl_RESERVED5:

RESERVED5
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED5_OFFSET            0xac

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved5_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved5 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_CL_BUSY:

CL_BUSY
"""""""

Cluster busy register

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                   Description                                    |
    +=====+===+====+==================================================================================+
    |    0|R  |BUSY|Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)|
    +-----+---+----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster busy register
                #define APB_SOC_CTRL_CL_BUSY_OFFSET              0xb0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_cl_busy_get(uint32_t base);
        static inline void apb_soc_ctrl_cl_busy_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster) (access: R)
        #define APB_SOC_CTRL_CL_BUSY_BUSY_BIT                                0
        #define APB_SOC_CTRL_CL_BUSY_BUSY_WIDTH                              1
        #define APB_SOC_CTRL_CL_BUSY_BUSY_MASK                               0x1
        #define APB_SOC_CTRL_CL_BUSY_BUSY_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_CL_BUSY_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_CL_BUSY_BUSY(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Cluster busy flag (i.e. It's 1 if there is at least 1 active block in the cluster)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_cl_busy_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_cl_busy : public vp::reg_32
        {
        public:
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
        };

|

.. _apb_soc_ctrl_JTAGREG:

JTAGREG
"""""""

JTAG external register

.. table:: 

    +-----+---+--------+----------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                             Description                              |
    +=====+===+========+======================================================================+
    |3:0  |R/W|INTERNAL|JTAG internal register used for synchronisation from external debugger|
    +-----+---+--------+----------------------------------------------------------------------+
    |11:8 |R  |EXTERNAL|JTAG external register used for synchronisation from external debugger|
    +-----+---+--------+----------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // JTAG external register
                #define APB_SOC_CTRL_JTAGREG_OFFSET              0xb4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_jtagreg_get(uint32_t base);
        static inline void apb_soc_ctrl_jtagreg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // JTAG internal register used for synchronisation from external debugger (access: R/W)
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_BIT                            0
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_WIDTH                          4
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_MASK                           0xf
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_RESET                          0x0
        
        // JTAG external register used for synchronisation from external debugger (access: R)
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_BIT                            8
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_WIDTH                          4
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_MASK                           0xf00
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define APB_SOC_CTRL_JTAGREG_INTERNAL(val)                 ((val) << 0)
        
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_GET(value)           (GAP_BEXTRACTU((value),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_GETS(value)          (GAP_BEXTRACT((value),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL_SET(value,field)     (GAP_BINSERT((value),(field),4,8))
        #define APB_SOC_CTRL_JTAGREG_EXTERNAL(val)                 ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int internal        :4 ; // JTAG internal register used for synchronisation from external debugger
            unsigned int padding0:4 ;
            unsigned int external        :4 ; // JTAG external register used for synchronisation from external debugger
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_jtagreg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_jtagreg : public vp::reg_32
        {
        public:
            inline void internal_set(uint32_t value);
            inline uint32_t internal_get();
            inline void external_set(uint32_t value);
            inline uint32_t external_get();
        };

|

.. _apb_soc_ctrl_REF_FAST_CLK_DIV:

REF_FAST_CLK_DIV
""""""""""""""""

Read only, reference fast clk divided by power of 2

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Read only, reference fast clk divided by power of 2
                #define APB_SOC_CTRL_REF_FAST_CLK_DIV_OFFSET     0xb8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_ref_fast_clk_div_get(uint32_t base);
        static inline void apb_soc_ctrl_ref_fast_clk_div_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_ref_fast_clk_div_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_ref_fast_clk_div : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SW_RST:

SW_RST
""""""

Software reset, reboot

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Software reset, reboot
                #define APB_SOC_CTRL_SW_RST_OFFSET               0xbc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sw_rst_get(uint32_t base);
        static inline void apb_soc_ctrl_sw_rst_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sw_rst_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sw_rst : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_CORESTATUS:

CORESTATUS
""""""""""

EOC and chip status register

.. table:: 

    +-----+---+------+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                               Description                                               |
    +=====+===+======+=========================================================================================================+
    |31:0 |R/W|STATUS|Chip status register. The SW can store the exit value value so that the external loader can get it.      |
    +-----+---+------+---------------------------------------------------------------------------------------------------------+
    |31   |R/W|EOC   |End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished.|
    +-----+---+------+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // EOC and chip status register
                #define APB_SOC_CTRL_CORESTATUS_OFFSET           0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_corestatus_get(uint32_t base);
        static inline void apb_soc_ctrl_corestatus_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Chip status register. The SW can store the exit value value so that the external loader can get it. (access: R/W)
        #define APB_SOC_CTRL_CORESTATUS_STATUS_BIT                           0
        #define APB_SOC_CTRL_CORESTATUS_STATUS_WIDTH                         32
        #define APB_SOC_CTRL_CORESTATUS_STATUS_MASK                          0xffffffff
        #define APB_SOC_CTRL_CORESTATUS_STATUS_RESET                         0x0
        
        // End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished. (access: R/W)
        #define APB_SOC_CTRL_CORESTATUS_EOC_BIT                              31
        #define APB_SOC_CTRL_CORESTATUS_EOC_WIDTH                            1
        #define APB_SOC_CTRL_CORESTATUS_EOC_MASK                             0x80000000
        #define APB_SOC_CTRL_CORESTATUS_EOC_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_CORESTATUS_STATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define APB_SOC_CTRL_CORESTATUS_STATUS(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_CORESTATUS_EOC_GET(value)             (GAP_BEXTRACTU((value),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC_GETS(value)            (GAP_BEXTRACT((value),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC_SET(value,field)       (GAP_BINSERT((value),(field),1,31))
        #define APB_SOC_CTRL_CORESTATUS_EOC(val)                   ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int status          :32; // Chip status register. The SW can store the exit value value so that the external loader can get it.
            unsigned int eoc             :1 ; // End Of Computation. The SW can store 1 here to notify the external loader that the execution is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_corestatus_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_corestatus : public vp::reg_32
        {
        public:
            inline void status_set(uint32_t value);
            inline uint32_t status_get();
            inline void eoc_set(uint32_t value);
            inline uint32_t eoc_get();
        };

|

.. _apb_soc_ctrl_BOOTSEL:

BOOTSEL
"""""""

Value of pad bootsel

.. table:: 

    +-----+---+-------+------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                Description                                                 |
    +=====+===+=======+============================================================================================================+
    |1:0  |R  |BOOTSEL|Boot mode. These bits can be used by the ROM to select the boot mode, see ROM documentation for more details|
    +-----+---+-------+------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Value of pad bootsel
                #define APB_SOC_CTRL_BOOTSEL_OFFSET              0xc4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_bootsel_get(uint32_t base);
        static inline void apb_soc_ctrl_bootsel_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Boot mode. These bits can be used by the ROM to select the boot mode, see ROM documentation for more details (access: R)
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_BIT                             0
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_WIDTH                           2
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_MASK                            0x3
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_GET(value)            (GAP_BEXTRACTU((value),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_GETS(value)           (GAP_BEXTRACT((value),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL_SET(value,field)      (GAP_BINSERT((value),(field),2,0))
        #define APB_SOC_CTRL_BOOTSEL_BOOTSEL(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int bootsel         :2 ; // Boot mode. These bits can be used by the ROM to select the boot mode, see ROM documentation for more details
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_bootsel_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_bootsel : public vp::reg_32
        {
        public:
            inline void bootsel_set(uint32_t value);
            inline uint32_t bootsel_get();
        };

|

.. _apb_soc_ctrl_WD_RST_RST:

WD_RST_RST
""""""""""

Clear WD timer

.. table:: 

    +-----+---+-----+---------------+
    |Bit #|R/W|Name |  Description  |
    +=====+===+=====+===============+
    |    0|R  |CLEAR|Clear watch dog|
    +-----+---+-----+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clear WD timer
                #define APB_SOC_CTRL_WD_RST_RST_OFFSET           0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_wd_rst_rst_get(uint32_t base);
        static inline void apb_soc_ctrl_wd_rst_rst_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Clear watch dog (access: R)
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_BIT                            0
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_WIDTH                          1
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_MASK                           0x1
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_WD_RST_RST_CLEAR(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int clear           :1 ; // Clear watch dog
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wd_rst_rst_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_wd_rst_rst : public vp::reg_32
        {
        public:
            inline void clear_set(uint32_t value);
            inline uint32_t clear_get();
        };

|

.. _apb_soc_ctrl_WD_RST_SET:

WD_RST_SET
""""""""""

Set WD timer

.. table:: 

    +-----+---+-----------+----------------------------------------------------+
    |Bit #|R/W|   Name    |                    Description                     |
    +=====+===+===========+====================================================+
    |15:0 |R  |REF_COUNTER|Watch dog Counter, maximum 2^16 * 1 / 32.768KHZ = 2s|
    +-----+---+-----------+----------------------------------------------------+
    |31   |R  |ENABLE     |Enable watch dog                                    |
    +-----+---+-----------+----------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Set WD timer
                #define APB_SOC_CTRL_WD_RST_SET_OFFSET           0xcc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_wd_rst_set_get(uint32_t base);
        static inline void apb_soc_ctrl_wd_rst_set_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Watch dog Counter, maximum 2^16 * 1 / 32.768KHZ = 2s (access: R)
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_BIT                      0
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_WIDTH                    16
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_MASK                     0xffff
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_RESET                    0xffff
        
        // Enable watch dog (access: R)
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_BIT                           31
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_WIDTH                         1
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_MASK                          0x80000000
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_GET(value)     (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_GETS(value)    (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_WD_RST_SET_REF_COUNTER(val)           ((val) << 0)
        
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_GET(value)          (GAP_BEXTRACTU((value),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_GETS(value)         (GAP_BEXTRACT((value),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE_SET(value,field)    (GAP_BINSERT((value),(field),1,31))
        #define APB_SOC_CTRL_WD_RST_SET_ENABLE(val)                ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ref_counter     :16; // Watch dog Counter, maximum 2^16 * 1 / 32.768KHZ = 2s
            unsigned int padding0:15;
            unsigned int enable          :1 ; // Enable watch dog
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wd_rst_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_wd_rst_set : public vp::reg_32
        {
        public:
            inline void ref_counter_set(uint32_t value);
            inline uint32_t ref_counter_get();
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|

.. _apb_soc_ctrl_RWM_PER:

RWM_PER
"""""""

RWM for peripherals

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RWM for peripherals
                #define APB_SOC_CTRL_RWM_PER_OFFSET              0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_rwm_per_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_per_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_per_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_rwm_per : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED7:

RESERVED7
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED7_OFFSET            0xd4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved7_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved7 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED8:

RESERVED8
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED8_OFFSET            0xd8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved8_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved8 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED9:

RESERVED9
"""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED9_OFFSET            0xdc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved9_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved9 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SUPERVISOR_DBG:

SUPERVISOR_DBG
""""""""""""""



.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                #define APB_SOC_CTRL_SUPERVISOR_DBG_OFFSET       0xe0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_supervisor_dbg_get(uint32_t base);
        static inline void apb_soc_ctrl_supervisor_dbg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_supervisor_dbg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_supervisor_dbg : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_DBG_CTRL:

DBG_CTRL
""""""""

Debug access control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug access control
                #define APB_SOC_CTRL_DBG_CTRL_OFFSET             0xe4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_dbg_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_dbg_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_dbg_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_dbg_ctrl : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED11:

RESERVED11
""""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED11_OFFSET           0xe8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved11_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved11 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED12:

RESERVED12
""""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED12_OFFSET           0xec

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved12_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved12 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_CLK_DIV_I3C:

CLK_DIV_I3C
"""""""""""

Clock divider for I3C

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock divider for I3C
                #define APB_SOC_CTRL_CLK_DIV_I3C_OFFSET          0xf0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_clk_div_i3c_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_div_i3c_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_div_i3c_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_clk_div_i3c : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_CLK_EN_QUIDDIKEY:

CLK_EN_QUIDDIKEY
""""""""""""""""

Clock divider for QUIDDIKEY

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Clock divider for QUIDDIKEY
                #define APB_SOC_CTRL_CLK_EN_QUIDDIKEY_OFFSET     0xf4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_clk_en_quiddikey_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_en_quiddikey_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_en_quiddikey_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_clk_en_quiddikey : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RESERVED13:

RESERVED13
""""""""""

Reserved

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reserved
                #define APB_SOC_CTRL_RESERVED13_OFFSET           0xf8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reserved13_get(uint32_t base);
        static inline void apb_soc_ctrl_reserved13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reserved13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reserved13 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_VERSION:

VERSION
"""""""

Show chip version

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Show chip version
                #define APB_SOC_CTRL_VERSION_OFFSET              0xfc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_version_get(uint32_t base);
        static inline void apb_soc_ctrl_version_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_version_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_version : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_WAKEUP_CTRL:

WAKEUP_CTRL
"""""""""""

Wakeup control

.. table:: 

    +-----+---+------------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                         Description                                          |
    +=====+===+============+==============================================================================================+
    |    0|R/W|MUX         |Choose wakeup mode, 0 : pin wakeup mode; 1 : spis wakeup mode                                 |
    +-----+---+------------+----------------------------------------------------------------------------------------------+
    |2:1  |R/W|SPIS_MODE   |SPI slave controller CPOL and CPHA                                                            |
    +-----+---+------------+----------------------------------------------------------------------------------------------+
    |    3|R/W|SPIS_RSTN   |Reset the SPI Slave wakeup controller                                                         |
    +-----+---+------------+----------------------------------------------------------------------------------------------+
    |    4|R/W|SPIS_SW_DONE|Notify outside device wakeup done                                                             |
    +-----+---+------------+----------------------------------------------------------------------------------------------+
    |    5|R/W|JTAG_DISABLE|0: JTAG is enabled and alternates are disabled, 1: JTAG is disabled and alternates are enabled|
    +-----+---+------------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Wakeup control
                #define APB_SOC_CTRL_WAKEUP_CTRL_OFFSET          0x100

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_wakeup_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_wakeup_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Choose wakeup mode, 0 : pin wakeup mode; 1 : spis wakeup mode (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_BIT                             0
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_WIDTH                           1
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_MASK                            0x1
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_RESET                           0x0
        
        // SPI slave controller CPOL and CPHA (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_BIT                       1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_WIDTH                     2
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_MASK                      0x6
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_RESET                     0x0
        
        // Reset the SPI Slave wakeup controller (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_BIT                       3
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_WIDTH                     1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_MASK                      0x8
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_RESET                     0x1
        
        // Notify outside device wakeup done (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_BIT                    4
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_WIDTH                  1
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_MASK                   0x10
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_RESET                  0x0
        
        // 0: JTAG is enabled and alternates are disabled, 1: JTAG is disabled and alternates are enabled (access: R/W)
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_BIT                    5
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_WIDTH                  1
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_MASK                   0x20
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_WAKEUP_CTRL_MUX(val)                  ((val) << 0)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_GET(value)      (GAP_BEXTRACTU((value),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_GETS(value)     (GAP_BEXTRACT((value),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE_SET(value,field) (GAP_BINSERT((value),(field),2,1))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_MODE(val)            ((val) << 1)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_GET(value)      (GAP_BEXTRACTU((value),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_GETS(value)     (GAP_BEXTRACT((value),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_RSTN(val)            ((val) << 3)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_GET(value)   (GAP_BEXTRACTU((value),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_GETS(value)  (GAP_BEXTRACT((value),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define APB_SOC_CTRL_WAKEUP_CTRL_SPIS_SW_DONE(val)         ((val) << 4)
        
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_GET(value)   (GAP_BEXTRACTU((value),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_GETS(value)  (GAP_BEXTRACT((value),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define APB_SOC_CTRL_WAKEUP_CTRL_JTAG_DISABLE(val)         ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mux             :1 ; // Choose wakeup mode, 0 : pin wakeup mode; 1 : spis wakeup mode
            unsigned int spis_mode       :2 ; // SPI slave controller CPOL and CPHA
            unsigned int spis_rstn       :1 ; // Reset the SPI Slave wakeup controller
            unsigned int spis_sw_done    :1 ; // Notify outside device wakeup done
            unsigned int jtag_disable    :1 ; // 0: JTAG is enabled and alternates are disabled, 1: JTAG is disabled and alternates are enabled
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_wakeup_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_wakeup_ctrl : public vp::reg_32
        {
        public:
            inline void mux_set(uint32_t value);
            inline uint32_t mux_get();
            inline void spis_mode_set(uint32_t value);
            inline uint32_t spis_mode_get();
            inline void spis_rstn_set(uint32_t value);
            inline uint32_t spis_rstn_get();
            inline void spis_sw_done_set(uint32_t value);
            inline uint32_t spis_sw_done_get();
            inline void jtag_disable_set(uint32_t value);
            inline uint32_t jtag_disable_get();
        };

|

.. _apb_soc_ctrl_SLEEP_CTRL:

SLEEP_CTRL
""""""""""

Sleep control

.. table:: 

    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                            Description                                             |
    +=====+===+=============+====================================================================================================+
    |7:0  |R/W|REBOOT       |SW config. This field is only interpreted by ROM and runtime to keep information accross deep sleep.|
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |8    |R/W|RTC_WAKE_EN  |Enable RTC wakeup                                                                                   |
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |9    |R/W|RTC_EVENT    |RTC event                                                                                           |
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |10   |R/W|EXT_WAKEUP_EN|Enable external wakeup                                                                              |
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |18:11|R/W|EXT_EVENT    |External event                                                                                      |
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+
    |20:19|R/W|WAKEUP_CFG   |?                                                                                                   |
    +-----+---+-------------+----------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep control
                #define APB_SOC_CTRL_SLEEP_CTRL_OFFSET           0x104

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // SW config. This field is only interpreted by ROM and runtime to keep information accross deep sleep. (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_BIT                           0
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_WIDTH                         8
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_MASK                          0xff
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_RESET                         0x0
        
        // Enable RTC wakeup (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_BIT                      8
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_WIDTH                    1
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_MASK                     0x100
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_RESET                    0x0
        
        // RTC event (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_BIT                        9
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_WIDTH                      1
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_MASK                       0x200
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_RESET                      0x0
        
        // Enable external wakeup (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_BIT                    10
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_WIDTH                  1
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_MASK                   0x400
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_RESET                  0x0
        
        // External event (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_BIT                        11
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_WIDTH                      8
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_MASK                       0x7f800
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_RESET                      0x0
        
        // ? (access: R/W)
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_BIT                       19
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_WIDTH                     2
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_MASK                      0x180000
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_GET(value)          (GAP_BEXTRACTU((value),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_GETS(value)         (GAP_BEXTRACT((value),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT_SET(value,field)    (GAP_BINSERT((value),(field),8,0))
        #define APB_SOC_CTRL_SLEEP_CTRL_REBOOT(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_GET(value)     (GAP_BEXTRACTU((value),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_GETS(value)    (GAP_BEXTRACT((value),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_WAKE_EN(val)           ((val) << 8)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_GET(value)       (GAP_BEXTRACTU((value),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_GETS(value)      (GAP_BEXTRACT((value),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define APB_SOC_CTRL_SLEEP_CTRL_RTC_EVENT(val)             ((val) << 9)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_GET(value)   (GAP_BEXTRACTU((value),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_GETS(value)  (GAP_BEXTRACT((value),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN_SET(value,field) (GAP_BINSERT((value),(field),1,10))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_WAKEUP_EN(val)         ((val) << 10)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_GET(value)       (GAP_BEXTRACTU((value),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_GETS(value)      (GAP_BEXTRACT((value),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT_SET(value,field) (GAP_BINSERT((value),(field),8,11))
        #define APB_SOC_CTRL_SLEEP_CTRL_EXT_EVENT(val)             ((val) << 11)
        
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_GET(value)      (GAP_BEXTRACTU((value),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_GETS(value)     (GAP_BEXTRACT((value),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG_SET(value,field) (GAP_BINSERT((value),(field),2,19))
        #define APB_SOC_CTRL_SLEEP_CTRL_WAKEUP_CFG(val)            ((val) << 19)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int reboot          :8 ; // SW config. This field is only interpreted by ROM and runtime to keep information accross deep sleep.
            unsigned int rtc_wake_en     :1 ; // Enable RTC wakeup
            unsigned int rtc_event       :1 ; // RTC event
            unsigned int ext_wakeup_en   :1 ; // Enable external wakeup
            unsigned int ext_event       :8 ; // External event
            unsigned int wakeup_cfg      :2 ; // ?
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_ctrl : public vp::reg_32
        {
        public:
            inline void reboot_set(uint32_t value);
            inline uint32_t reboot_get();
            inline void rtc_wake_en_set(uint32_t value);
            inline uint32_t rtc_wake_en_get();
            inline void rtc_event_set(uint32_t value);
            inline uint32_t rtc_event_get();
            inline void ext_wakeup_en_set(uint32_t value);
            inline uint32_t ext_wakeup_en_get();
            inline void ext_event_set(uint32_t value);
            inline uint32_t ext_event_get();
            inline void wakeup_cfg_set(uint32_t value);
            inline uint32_t wakeup_cfg_get();
        };

|

.. _apb_soc_ctrl_SLEEP_GPIO_CTRL:

SLEEP_GPIO_CTRL
"""""""""""""""

Sleep GPIO control

.. table:: 

    +-----+---+----------------+-----------+
    |Bit #|R/W|      Name      |Description|
    +=====+===+================+===========+
    |15:0 |R/W|GPIO_WAKEUP_TYPE|?          |
    +-----+---+----------------+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep GPIO control
                #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_OFFSET      0x108

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_gpio_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_gpio_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ? (access: R/W)
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_BIT            0
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_WIDTH          16
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_MASK           0xffff
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_RESET          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define APB_SOC_CTRL_SLEEP_GPIO_CTRL_GPIO_WAKEUP_TYPE(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpio_wakeup_type:16; // ?
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_gpio_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_gpio_ctrl : public vp::reg_32
        {
        public:
            inline void gpio_wakeup_type_set(uint32_t value);
            inline uint32_t gpio_wakeup_type_get();
        };

|

.. _apb_soc_ctrl_REG_OSC_CTRL:

REG_OSC_CTRL
""""""""""""

Controls fast oscillator

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                 Description                                 |
    +=====+===+======+=============================================================================+
    |    0|R/W|EN    |0 - Power Down, 1 - Enables Oscillation                                      |
    +-----+---+------+-----------------------------------------------------------------------------+
    |    1|R/W|BYPASS|0 - No bypass, 1 - Bypass (CLK follows XI)                                   |
    +-----+---+------+-----------------------------------------------------------------------------+
    |5:2  |R/W|GM_SEL|Controls desired oscillator frequency (b'1000 - 12 or 27 MHz, b'1001 - 50Mhz)|
    +-----+---+------+-----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls fast oscillator
                #define APB_SOC_CTRL_REG_OSC_CTRL_OFFSET         0x110

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reg_osc_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_reg_osc_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 0 - Power Down, 1 - Enables Oscillation (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_BIT                             0
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_WIDTH                           1
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_MASK                            0x1
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_RESET                           0x1
        
        // 0 - No bypass, 1 - Bypass (CLK follows XI) (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_BIT                         1
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_WIDTH                       1
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_MASK                        0x2
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_RESET                       0x0
        
        // Controls desired oscillator frequency (b'1000 - 12 or 27 MHz, b'1001 - 50Mhz) (access: R/W)
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_BIT                         2
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_WIDTH                       4
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_MASK                        0x3c
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_REG_OSC_CTRL_EN(val)                  ((val) << 0)
        
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define APB_SOC_CTRL_REG_OSC_CTRL_BYPASS(val)              ((val) << 1)
        
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_GET(value)        (GAP_BEXTRACTU((value),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_GETS(value)       (GAP_BEXTRACT((value),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL_SET(value,field)  (GAP_BINSERT((value),(field),4,2))
        #define APB_SOC_CTRL_REG_OSC_CTRL_GM_SEL(val)              ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // 0 - Power Down, 1 - Enables Oscillation
            unsigned int bypass          :1 ; // 0 - No bypass, 1 - Bypass (CLK follows XI)
            unsigned int gm_sel          :4 ; // Controls desired oscillator frequency (b'1000 - 12 or 27 MHz, b'1001 - 50Mhz)
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reg_osc_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reg_osc_ctrl : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
            inline void bypass_set(uint32_t value);
            inline uint32_t bypass_get();
            inline void gm_sel_set(uint32_t value);
            inline uint32_t gm_sel_get();
        };

|

.. _apb_soc_ctrl_REBOOT:

REBOOT
""""""

Check if system is reboot or cold boot

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Check if system is reboot or cold boot
                #define APB_SOC_CTRL_REBOOT_OFFSET               0x114

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_reboot_get(uint32_t base);
        static inline void apb_soc_ctrl_reboot_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_reboot_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_reboot : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_CLK_DIV_REF_FAST_POW2:

CLK_DIV_REF_FAST_POW2
"""""""""""""""""""""

Controls fast oscillator pow2 divider

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls fast oscillator pow2 divider
                #define APB_SOC_CTRL_CLK_DIV_REF_FAST_POW2_OFFSET 0x118

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_clk_div_ref_fast_pow2_get(uint32_t base);
        static inline void apb_soc_ctrl_clk_div_ref_fast_pow2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_clk_div_ref_fast_pow2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_clk_div_ref_fast_pow2 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG0:

SLEEP_PAD_CFG0
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG0_OFFSET       0x140

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg0_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg0 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG1:

SLEEP_PAD_CFG1
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG1_OFFSET       0x144

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg1_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg1 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG2:

SLEEP_PAD_CFG2
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG2_OFFSET       0x148

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg2_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg2 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG3:

SLEEP_PAD_CFG3
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG3_OFFSET       0x14c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg3_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg3 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG4:

SLEEP_PAD_CFG4
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG4_OFFSET       0x150

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg4_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg4 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG5:

SLEEP_PAD_CFG5
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG5_OFFSET       0x154

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg5_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg5 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_SLEEP_PAD_CFG6:

SLEEP_PAD_CFG6
""""""""""""""

Sleep pad control

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Sleep pad control
                #define APB_SOC_CTRL_SLEEP_PAD_CFG6_OFFSET       0x158

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_sleep_pad_cfg6_get(uint32_t base);
        static inline void apb_soc_ctrl_sleep_pad_cfg6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_sleep_pad_cfg6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_sleep_pad_cfg6 : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_PADSLEEP:

PADSLEEP
""""""""

sets the pad sleep mode on (1= on, 0= off)

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // sets the pad sleep mode on (1= on, 0= off)
                #define APB_SOC_CTRL_PADSLEEP_OFFSET             0x160

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_padsleep_get(uint32_t base);
        static inline void apb_soc_ctrl_padsleep_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_padsleep_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_padsleep : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_NEVACFG:

NEVACFG
"""""""

NEVA config

.. table:: 

    +-----+---+---------------+------------------------------------------------------+
    |Bit #|R/W|     Name      |                     Description                      |
    +=====+===+===============+======================================================+
    |4:0  |R/W|NEVA_IO_LS_TSEL|Max output current when switching on iswitchable I/Os |
    +-----+---+---------------+------------------------------------------------------+
    |12:8 |R/W|NEVA_IO_HS_TSEL|Max output current when switching on memory interfaces|
    +-----+---+---------------+------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // NEVA config
                #define APB_SOC_CTRL_NEVACFG_OFFSET              0x164

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_nevacfg_get(uint32_t base);
        static inline void apb_soc_ctrl_nevacfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Max output current when switching on iswitchable I/Os (access: R/W)
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_BIT                     0
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_WIDTH                   5
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_MASK                    0x1f
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_RESET                   0x1f
        
        // Max output current when switching on memory interfaces (access: R/W)
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_BIT                     8
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_WIDTH                   5
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_MASK                    0x1f00
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_RESET                   0x1f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_LS_TSEL(val)          ((val) << 0)
        
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_GET(value)    (GAP_BEXTRACTU((value),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_GETS(value)   (GAP_BEXTRACT((value),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL_SET(value,field) (GAP_BINSERT((value),(field),5,8))
        #define APB_SOC_CTRL_NEVACFG_NEVA_IO_HS_TSEL(val)          ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int neva_io_ls_tsel :5 ; // Max output current when switching on iswitchable I/Os
            unsigned int padding0:3 ;
            unsigned int neva_io_hs_tsel :5 ; // Max output current when switching on memory interfaces
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_nevacfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_nevacfg : public vp::reg_32
        {
        public:
            inline void neva_io_ls_tsel_set(uint32_t value);
            inline uint32_t neva_io_ls_tsel_get();
            inline void neva_io_hs_tsel_set(uint32_t value);
            inline uint32_t neva_io_hs_tsel_get();
        };

|

.. _apb_soc_ctrl_TRCCFG:

TRCCFG
""""""

TRC config

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // TRC config
                #define APB_SOC_CTRL_TRCCFG_OFFSET               0x168

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_trccfg_get(uint32_t base);
        static inline void apb_soc_ctrl_trccfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_trccfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_trccfg : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RWM_L2_INTL:

RWM_L2_INTL
"""""""""""

RWM for L2 Interlevead

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RWM for L2 Interlevead
                #define APB_SOC_CTRL_RWM_L2_INTL_OFFSET          0x16c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_rwm_l2_intl_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_l2_intl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_l2_intl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_rwm_l2_intl : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_RWM_L2_PRI:

RWM_L2_PRI
""""""""""

RWM for L2 Private memories 3:0

.. table:: 

    +-----+---+----+-----------+
    |Bit #|R/W|Name|Description|
    +=====+===+====+===========+
    +-----+---+----+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RWM for L2 Private memories 3:0
                #define APB_SOC_CTRL_RWM_L2_PRI_OFFSET           0x170

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_rwm_l2_pri_get(uint32_t base);
        static inline void apb_soc_ctrl_rwm_l2_pri_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_rwm_l2_pri_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_rwm_l2_pri : public vp::reg_32
        {
        public:
        };

|

.. _apb_soc_ctrl_L2_PWR:

L2_PWR
""""""

Controls L2 power

.. table:: 

    +-----+---+-----------+------------------------------------------------------+
    |Bit #|R/W|   Name    |                     Description                      |
    +=====+===+===========+======================================================+
    |13:0 |R/W|L2_MEM_RET |L2 retention control for private and interleaved banks|
    +-----+---+-----------+------------------------------------------------------+
    |27:16|R/W|L2_MEM_PDWN|L2 power control for interleaved banks                |
    +-----+---+-----------+------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls L2 power
                #define APB_SOC_CTRL_L2_PWR_OFFSET               0x174

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_l2_pwr_get(uint32_t base);
        static inline void apb_soc_ctrl_l2_pwr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // L2 retention control for private and interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_BIT                           0
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_WIDTH                         14
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_MASK                          0x3fff
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_RESET                         0x0
        
        // L2 power control for interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_BIT                          16
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_WIDTH                        12
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_MASK                         0xfff0000
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_GET(value)          (GAP_BEXTRACTU((value),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_GETS(value)         (GAP_BEXTRACT((value),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET_SET(value,field)    (GAP_BINSERT((value),(field),14,0))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_RET(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_GET(value)         (GAP_BEXTRACTU((value),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_GETS(value)        (GAP_BEXTRACT((value),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN_SET(value,field)   (GAP_BINSERT((value),(field),12,16))
        #define APB_SOC_CTRL_L2_PWR_L2_MEM_PDWN(val)               ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int l2_mem_ret      :14; // L2 retention control for private and interleaved banks
            unsigned int padding0:2 ;
            unsigned int l2_mem_pdwn     :12; // L2 power control for interleaved banks
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_l2_pwr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_l2_pwr : public vp::reg_32
        {
        public:
            inline void l2_mem_ret_set(uint32_t value);
            inline uint32_t l2_mem_ret_get();
            inline void l2_mem_pdwn_set(uint32_t value);
            inline uint32_t l2_mem_pdwn_get();
        };

|

.. _apb_soc_ctrl_L2_CTRL:

L2_CTRL
"""""""

Controls L2 power

.. table:: 

    +-----+---+----------------+-------------------------------------------------------+
    |Bit #|R/W|      Name      |                      Description                      |
    +=====+===+================+=======================================================+
    |13:0 |R/W|L2_MEM_POWERGATE|L2 power gate control for private and interleaved banks|
    +-----+---+----------------+-------------------------------------------------------+
    |27:16|R/W|L2_MEM_DEEPSLEEP|L2 deep sleep for interleaved banks                    |
    +-----+---+----------------+-------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls L2 power
                #define APB_SOC_CTRL_L2_CTRL_OFFSET              0x178

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_l2_ctrl_get(uint32_t base);
        static inline void apb_soc_ctrl_l2_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // L2 power gate control for private and interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_BIT                    0
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_WIDTH                  14
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_MASK                   0x3fff
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_RESET                  0x0
        
        // L2 deep sleep for interleaved banks (access: R/W)
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_BIT                    16
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_WIDTH                  12
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_MASK                   0xfff0000
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_GET(value)   (GAP_BEXTRACTU((value),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_GETS(value)  (GAP_BEXTRACT((value),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE_SET(value,field) (GAP_BINSERT((value),(field),14,0))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_POWERGATE(val)         ((val) << 0)
        
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_GET(value)   (GAP_BEXTRACTU((value),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_GETS(value)  (GAP_BEXTRACT((value),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP_SET(value,field) (GAP_BINSERT((value),(field),12,16))
        #define APB_SOC_CTRL_L2_CTRL_L2_MEM_DEEPSLEEP(val)         ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int l2_mem_powergate:14; // L2 power gate control for private and interleaved banks
            unsigned int padding0:2 ;
            unsigned int l2_mem_deepsleep:12; // L2 deep sleep for interleaved banks
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_l2_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_l2_ctrl : public vp::reg_32
        {
        public:
            inline void l2_mem_powergate_set(uint32_t value);
            inline uint32_t l2_mem_powergate_get();
            inline void l2_mem_deepsleep_set(uint32_t value);
            inline uint32_t l2_mem_deepsleep_get();
        };

|

.. _apb_soc_ctrl_BORCFG:

BORCFG
""""""

Controls the brown-out reset

.. table:: 

    +-----+---+----------+-------------------------------------------------+
    |Bit #|R/W|   Name   |                   Description                   |
    +=====+===+==========+=================================================+
    |    0|R/W|BOR_ENABLE|Set to 1 to enable BOR monitoring of power supply|
    +-----+---+----------+-------------------------------------------------+
    |5:1  |R/W|BOR_VSEL  |Threshold of detection of power supply drops     |
    +-----+---+----------+-------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Controls the brown-out reset
                #define APB_SOC_CTRL_BORCFG_OFFSET               0x180

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t apb_soc_ctrl_borcfg_get(uint32_t base);
        static inline void apb_soc_ctrl_borcfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Set to 1 to enable BOR monitoring of power supply (access: R/W)
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_BIT                           0
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_WIDTH                         1
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_MASK                          0x1
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_RESET                         0x0
        
        // Threshold of detection of power supply drops (access: R/W)
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_BIT                             1
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_WIDTH                           5
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_MASK                            0x3e
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_RESET                           0x5

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define APB_SOC_CTRL_BORCFG_BOR_ENABLE(val)                ((val) << 0)
        
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_GET(value)            (GAP_BEXTRACTU((value),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_GETS(value)           (GAP_BEXTRACT((value),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL_SET(value,field)      (GAP_BINSERT((value),(field),5,1))
        #define APB_SOC_CTRL_BORCFG_BOR_VSEL(val)                  ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int bor_enable      :1 ; // Set to 1 to enable BOR monitoring of power supply
            unsigned int bor_vsel        :5 ; // Threshold of detection of power supply drops
          };
          unsigned int raw;
        } __attribute__((packed)) apb_soc_ctrl_borcfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_apb_soc_ctrl_borcfg : public vp::reg_32
        {
        public:
            inline void bor_enable_set(uint32_t value);
            inline uint32_t bor_enable_get();
            inline void bor_vsel_set(uint32_t value);
            inline uint32_t bor_vsel_get();
        };

|
