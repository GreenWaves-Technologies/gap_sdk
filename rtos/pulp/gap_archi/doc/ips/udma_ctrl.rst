Input file: doc/UDMA_CTRL_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |                   Name                    |Offset|Width|                                                                                                                   Description                                                                                                                    |
    +===========================================+======+=====+==================================================================================================================================================================================================================================================+
    |:ref:`CFG_CG<udma_ctrl_CFG_CG>`            |     0|   32|uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.|
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_CG_SET<udma_ctrl_CFG_CG_SET>`    |     4|   32|Each bit set to 1 sets the corresponding bit in the CFG_CG register                                                                                                                                                                               |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_CG_CLR<udma_ctrl_CFG_CG_CLR>`    |     8|   32|Each bit set to 1 clears the corresponding bit in the CFG_CG register                                                                                                                                                                             |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN<udma_ctrl_CFG_RSTN>`        |    12|   32|uDMA peripherals reset configuration. At RESET all periphs under reset.                                                                                                                                                                           |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN_SET<udma_ctrl_CFG_RSTN_SET>`|    16|   32|Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register                                                                                                                                                                    |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_RSTN_CLR<udma_ctrl_CFG_RSTN_CLR>`|    20|   32|Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register                                                                                                                                                                    |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |:ref:`CFG_EVENT<udma_ctrl_CFG_EVENT>`      |    24|   32|uDMA peripherals external event configuration                                                                                                                                                                                                     |
    +-------------------------------------------+------+-----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
                #define UDMA_CTRL_CFG_CG_OFFSET                  0x0
        
                // Each bit set to 1 sets the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_SET_OFFSET              0x4
        
                // Each bit set to 1 clears the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_CLR_OFFSET              0x8
        
                // uDMA peripherals reset configuration. At RESET all periphs under reset.
                #define UDMA_CTRL_CFG_RSTN_OFFSET                0xc
        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_SET_OFFSET            0x10
        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_CLR_OFFSET            0x14
        
                // uDMA peripherals external event configuration
                #define UDMA_CTRL_CFG_EVENT_OFFSET               0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_cg_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_cg_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_clr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_rstn_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_clr_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_ctrl_cfg_event_get(uint32_t base);
        static inline void udma_ctrl_cfg_event_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_BIT                             0
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_MASK                            0xff
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_BIT                             8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_MASK                            0xff00
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_BIT                             16
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_MASK                            0xff0000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_BIT                             24
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_MASK                            0xff000000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0(val)                  ((val) << 0)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1(val)                  ((val) << 8)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2(val)                  ((val) << 16)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_CTRL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cfg_cg;  // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
            volatile uint32_t cfg_cg_set;  // Each bit set to 1 sets the corresponding bit in the CFG_CG register
            volatile uint32_t cfg_cg_clr;  // Each bit set to 1 clears the corresponding bit in the CFG_CG register
            volatile uint32_t cfg_rstn;  // uDMA peripherals reset configuration. At RESET all periphs under reset.
            volatile uint32_t cfg_rstn_set;  // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
            volatile uint32_t cfg_rstn_clr;  // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
            volatile uint32_t cfg_event;  // uDMA peripherals external event configuration
        } __attribute__((packed)) udma_ctrl_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_cg_clr_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_rstn_clr_t;
        
        typedef union {
          struct {
            unsigned int cmp_evt0        :8 ; // 
            unsigned int cmp_evt1        :8 ; // 
            unsigned int cmp_evt2        :8 ; // 
            unsigned int cmp_evt3        :8 ; // 
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_event_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_ctrl : public vp::regmap
        {
        public:
            vp_udma_ctrl_cfg_cg cfg_cg;
            vp_udma_ctrl_cfg_cg_set cfg_cg_set;
            vp_udma_ctrl_cfg_cg_clr cfg_cg_clr;
            vp_udma_ctrl_cfg_rstn cfg_rstn;
            vp_udma_ctrl_cfg_rstn_set cfg_rstn_set;
            vp_udma_ctrl_cfg_rstn_clr cfg_rstn_clr;
            vp_udma_ctrl_cfg_event cfg_event;
        };

|

.. _udma_ctrl_CFG_CG:

CFG_CG
""""""

uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.

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

        
                // uDMA peripherals clock gate configuration. This controls the individual clock-gating of each uDMA peripheral. There is one bit per peripheral, and the clock-gating is active low, i.e the peripheral is inactive when its corresponding bit is 0.
                #define UDMA_CTRL_CFG_CG_OFFSET                  0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_CG_SET:

CFG_CG_SET
""""""""""

Each bit set to 1 sets the corresponding bit in the CFG_CG register

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

        
                // Each bit set to 1 sets the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_SET_OFFSET              0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg_set : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_CG_CLR:

CFG_CG_CLR
""""""""""

Each bit set to 1 clears the corresponding bit in the CFG_CG register

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

        
                // Each bit set to 1 clears the corresponding bit in the CFG_CG register
                #define UDMA_CTRL_CFG_CG_CLR_OFFSET              0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_cg_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_cg_clr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_cg_clr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_cg_clr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN:

CFG_RSTN
""""""""

uDMA peripherals reset configuration. At RESET all periphs under reset.

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

        
                // uDMA peripherals reset configuration. At RESET all periphs under reset.
                #define UDMA_CTRL_CFG_RSTN_OFFSET                0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN_SET:

CFG_RSTN_SET
""""""""""""

Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register

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

        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_SET_OFFSET            0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_set_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn_set : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_RSTN_CLR:

CFG_RSTN_CLR
""""""""""""

Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register

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

        
                // Each bit set to 1 sets the corresponding bit in the UDMACTRL_CFG_RSTN register
                #define UDMA_CTRL_CFG_RSTN_CLR_OFFSET            0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_rstn_clr_get(uint32_t base);
        static inline void udma_ctrl_cfg_rstn_clr_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) udma_ctrl_cfg_rstn_clr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_rstn_clr : public vp::reg_32
        {
        public:
        };

|

.. _udma_ctrl_CFG_EVENT:

CFG_EVENT
"""""""""

uDMA peripherals external event configuration

.. table:: 

    +-----+---+--------+-----------+
    |Bit #|R/W|  Name  |Description|
    +=====+===+========+===========+
    |7:0  |R/W|CMP_EVT0|           |
    +-----+---+--------+-----------+
    |15:8 |R/W|CMP_EVT1|           |
    +-----+---+--------+-----------+
    |23:16|R/W|CMP_EVT2|           |
    +-----+---+--------+-----------+
    |31:24|R/W|CMP_EVT3|           |
    +-----+---+--------+-----------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // uDMA peripherals external event configuration
                #define UDMA_CTRL_CFG_EVENT_OFFSET               0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_ctrl_cfg_event_get(uint32_t base);
        static inline void udma_ctrl_cfg_event_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_BIT                             0
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_MASK                            0xff
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_BIT                             8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_MASK                            0xff00
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_BIT                             16
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_MASK                            0xff0000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_RESET                           0x0
        
        //  (access: R/W)
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_BIT                             24
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_WIDTH                           8
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_MASK                            0xff000000
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GET(value)            (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_GETS(value)           (GAP_BEXTRACT((value),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0_SET(value,field)      (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT0(val)                  ((val) << 0)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GET(value)            (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_GETS(value)           (GAP_BEXTRACT((value),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1_SET(value,field)      (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT1(val)                  ((val) << 8)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GET(value)            (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_GETS(value)           (GAP_BEXTRACT((value),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2_SET(value,field)      (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT2(val)                  ((val) << 16)
        
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define UDMA_CTRL_CFG_EVENT_CMP_EVT3(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmp_evt0        :8 ; // 
            unsigned int cmp_evt1        :8 ; // 
            unsigned int cmp_evt2        :8 ; // 
            unsigned int cmp_evt3        :8 ; // 
          };
          unsigned int raw;
        } __attribute__((packed)) udma_ctrl_cfg_event_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_ctrl_cfg_event : public vp::reg_32
        {
        public:
            inline void cmp_evt0_set(uint32_t value);
            inline uint32_t cmp_evt0_get();
            inline void cmp_evt1_set(uint32_t value);
            inline uint32_t cmp_evt1_get();
            inline void cmp_evt2_set(uint32_t value);
            inline uint32_t cmp_evt2_get();
            inline void cmp_evt3_set(uint32_t value);
            inline uint32_t cmp_evt3_get();
        };

|
