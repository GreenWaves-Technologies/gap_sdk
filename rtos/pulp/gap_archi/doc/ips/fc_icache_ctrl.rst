Input file: docs/IP_REFERENCES/FC_ICACHE_CTRL_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------+------+-----+-----------------------------------------------------------+
    |                   Name                   |Offset|Width|                        Description                        |
    +==========================================+======+=====+===========================================================+
    |:ref:`ENABLE<fc_icache_ctrl_ENABLE>`      |     0|   32|FC instruction cache unit enable configuration register.   |
    +------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`FLUSH<fc_icache_ctrl_FLUSH>`        |     4|   32|FC instruction cache unit flush command register.          |
    +------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`SEL_FLUSH<fc_icache_ctrl_SEL_FLUSH>`|     8|   32|FC instruction cache unit selective flush command register.|
    +------------------------------------------+------+-----+-----------------------------------------------------------+
    |:ref:`STATUS<fc_icache_ctrl_STATUS>`      |    12|   32|FC instruction cache unit status register.                 |
    +------------------------------------------+------+-----+-----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC instruction cache unit enable configuration register.
                #define FC_ICACHE_CTRL_ENABLE_OFFSET             0x0
        
                // FC instruction cache unit flush command register.
                #define FC_ICACHE_CTRL_FLUSH_OFFSET              0x4
        
                // FC instruction cache unit selective flush command register.
                #define FC_ICACHE_CTRL_SEL_FLUSH_OFFSET          0x8
        
                // FC instruction cache unit status register.
                #define FC_ICACHE_CTRL_STATUS_OFFSET             0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_icache_ctrl_enable_get(uint32_t base);
        static inline void fc_icache_ctrl_enable_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_icache_ctrl_flush_get(uint32_t base);
        static inline void fc_icache_ctrl_flush_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_icache_ctrl_sel_flush_get(uint32_t base);
        static inline void fc_icache_ctrl_sel_flush_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_icache_ctrl_status_get(uint32_t base);
        static inline void fc_icache_ctrl_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define FC_ICACHE_CTRL_ENABLE_EN_BIT                                 0
        #define FC_ICACHE_CTRL_ENABLE_EN_WIDTH                               1
        #define FC_ICACHE_CTRL_ENABLE_EN_MASK                                0x1
        #define FC_ICACHE_CTRL_ENABLE_EN_RESET                               0x0
        
        // FC instruction cache fully flush command. (access: W)
        #define FC_ICACHE_CTRL_FLUSH_FL_BIT                                  0
        #define FC_ICACHE_CTRL_FLUSH_FL_WIDTH                                1
        #define FC_ICACHE_CTRL_FLUSH_FL_MASK                                 0x1
        #define FC_ICACHE_CTRL_FLUSH_FL_RESET                                0x0
        
        // FC instruction cache selective flush address configuration bitfield. (access: W)
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT                            0
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH                          32
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_MASK                           0xffffffff
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_RESET                          0x0
        
        // FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action (access: R)
        #define FC_ICACHE_CTRL_STATUS_STATUS_BIT                             0
        #define FC_ICACHE_CTRL_STATUS_STATUS_WIDTH                           1
        #define FC_ICACHE_CTRL_STATUS_STATUS_MASK                            0x1
        #define FC_ICACHE_CTRL_STATUS_STATUS_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_ICACHE_CTRL_ENABLE_EN_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN(val)                      ((val) << 0)
        
        #define FC_ICACHE_CTRL_FLUSH_FL_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL(val)                       ((val) << 0)
        
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR(val)                 ((val) << 0)
        
        #define FC_ICACHE_CTRL_STATUS_STATUS_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** FC_ICACHE_CTRL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t enable;  // FC instruction cache unit enable configuration register.
            volatile uint32_t flush;  // FC instruction cache unit flush command register.
            volatile uint32_t sel_flush;  // FC instruction cache unit selective flush command register.
            volatile uint32_t status;  // FC instruction cache unit status register.
        } __attribute__((packed)) fc_icache_ctrl_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_enable_t;
        
        typedef union {
          struct {
            unsigned int fl              :1 ; // FC instruction cache fully flush command.
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_flush_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // FC instruction cache selective flush address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_sel_flush_t;
        
        typedef union {
          struct {
            unsigned int status          :1 ; // FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_fc_icache_ctrl : public vp::regmap
        {
        public:
            vp_fc_icache_ctrl_enable enable;
            vp_fc_icache_ctrl_flush flush;
            vp_fc_icache_ctrl_sel_flush sel_flush;
            vp_fc_icache_ctrl_status status;
        };

|

.. _fc_icache_ctrl_ENABLE:

ENABLE
""""""

FC instruction cache unit enable configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                    Description                                     |
    +=====+===+====+====================================================================================+
    |    0|W  |EN  |FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+----+------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC instruction cache unit enable configuration register.
                #define FC_ICACHE_CTRL_ENABLE_OFFSET             0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_icache_ctrl_enable_get(uint32_t base);
        static inline void fc_icache_ctrl_enable_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define FC_ICACHE_CTRL_ENABLE_EN_BIT                                 0
        #define FC_ICACHE_CTRL_ENABLE_EN_WIDTH                               1
        #define FC_ICACHE_CTRL_ENABLE_EN_MASK                                0x1
        #define FC_ICACHE_CTRL_ENABLE_EN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_ICACHE_CTRL_ENABLE_EN_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_ENABLE_EN(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en              :1 ; // FC instruction cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_enable_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_icache_ctrl_enable : public vp::reg_32
        {
        public:
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _fc_icache_ctrl_FLUSH:

FLUSH
"""""

FC instruction cache unit flush command register.

.. table:: 

    +-----+---+----+-----------------------------------------+
    |Bit #|R/W|Name|               Description               |
    +=====+===+====+=========================================+
    |    0|W  |FL  |FC instruction cache fully flush command.|
    +-----+---+----+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC instruction cache unit flush command register.
                #define FC_ICACHE_CTRL_FLUSH_OFFSET              0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_icache_ctrl_flush_get(uint32_t base);
        static inline void fc_icache_ctrl_flush_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC instruction cache fully flush command. (access: W)
        #define FC_ICACHE_CTRL_FLUSH_FL_BIT                                  0
        #define FC_ICACHE_CTRL_FLUSH_FL_WIDTH                                1
        #define FC_ICACHE_CTRL_FLUSH_FL_MASK                                 0x1
        #define FC_ICACHE_CTRL_FLUSH_FL_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_ICACHE_CTRL_FLUSH_FL_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_FLUSH_FL(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int fl              :1 ; // FC instruction cache fully flush command.
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_flush_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_icache_ctrl_flush : public vp::reg_32
        {
        public:
            inline void fl_set(uint32_t value);
            inline uint32_t fl_get();
        };

|

.. _fc_icache_ctrl_SEL_FLUSH:

SEL_FLUSH
"""""""""

FC instruction cache unit selective flush command register.

.. table:: 

    +-----+---+----+--------------------------------------------------------------------+
    |Bit #|R/W|Name|                            Description                             |
    +=====+===+====+====================================================================+
    |31:0 |W  |ADDR|FC instruction cache selective flush address configuration bitfield.|
    +-----+---+----+--------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC instruction cache unit selective flush command register.
                #define FC_ICACHE_CTRL_SEL_FLUSH_OFFSET          0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_icache_ctrl_sel_flush_get(uint32_t base);
        static inline void fc_icache_ctrl_sel_flush_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC instruction cache selective flush address configuration bitfield. (access: W)
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT                            0
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH                          32
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_MASK                           0xffffffff
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define FC_ICACHE_CTRL_SEL_FLUSH_ADDR(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // FC instruction cache selective flush address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_sel_flush_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_icache_ctrl_sel_flush : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _fc_icache_ctrl_STATUS:

STATUS
""""""

FC instruction cache unit status register.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                   Description                                                   |
    +=====+===+======+=================================================================================================================+
    |    0|R  |STATUS|FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action|
    +-----+---+------+-----------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC instruction cache unit status register.
                #define FC_ICACHE_CTRL_STATUS_OFFSET             0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_icache_ctrl_status_get(uint32_t base);
        static inline void fc_icache_ctrl_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action (access: R)
        #define FC_ICACHE_CTRL_STATUS_STATUS_BIT                             0
        #define FC_ICACHE_CTRL_STATUS_STATUS_WIDTH                           1
        #define FC_ICACHE_CTRL_STATUS_STATUS_MASK                            0x1
        #define FC_ICACHE_CTRL_STATUS_STATUS_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define FC_ICACHE_CTRL_STATUS_STATUS_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define FC_ICACHE_CTRL_STATUS_STATUS(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int status          :1 ; // FC instruction cache pending action status flag: - 1'b0: no pending caching action - 1'b1: pending caching action
          };
          unsigned int raw;
        } __attribute__((packed)) fc_icache_ctrl_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_icache_ctrl_status : public vp::reg_32
        {
        public:
            inline void status_set(uint32_t value);
            inline uint32_t status_get();
        };

|
