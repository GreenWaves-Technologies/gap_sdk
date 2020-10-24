Input file: fe/ips/hier-icache/DOC/CLUSTER_ICACHE_CTRL_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |                                     Name                                      |Offset|Width|                                                  Description                                                  |
    +===============================================================================+======+=====+===============================================================================================================+
    |:ref:`ENABLE<cluster_icache_ctrl_ENABLE>`                                      |     0|   32|Cluster instruction cache unit enable configuration register.                                                  |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`FLUSH<cluster_icache_ctrl_FLUSH>`                                        |     4|   32|Cluster instruction cache unit flush command register.                                                         |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`L0_FLUSH<cluster_icache_ctrl_L0_FLUSH>`                                  |     8|   32|Cluster level 0 instruction cache unit flush command register.                                                 |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`SEL_FLUSH<cluster_icache_ctrl_SEL_FLUSH>`                                |    12|   32|Cluster instruction cache unit selective flush command register.                                               |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_SPECIAL_CORE_CACHE<cluster_icache_ctrl_ENABLE_SPECIAL_CORE_CACHE>`|    24|   32|When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.|
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+
    |:ref:`ENABLE_L1_L15_PREFETCH<cluster_icache_ctrl_ENABLE_L1_L15_PREFETCH>`      |    28|   32|For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch                           |
    +-------------------------------------------------------------------------------+------+-----+---------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster instruction cache unit enable configuration register.
                #define CLUSTER_ICACHE_CTRL_ENABLE_OFFSET        0x0
        
                // Cluster instruction cache unit flush command register.
                #define CLUSTER_ICACHE_CTRL_FLUSH_OFFSET         0x4
        
                // Cluster level 0 instruction cache unit flush command register.
                #define CLUSTER_ICACHE_CTRL_L0_FLUSH_OFFSET      0x8
        
                // Cluster instruction cache unit selective flush command register.
                #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_OFFSET     0xc
        
                // When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.
                #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_OFFSET 0x18
        
                // For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch 
                #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_OFFSET 0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_enable_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_icache_ctrl_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_flush_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_icache_ctrl_l0_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_l0_flush_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_icache_ctrl_sel_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_sel_flush_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_icache_ctrl_enable_special_core_cache_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_special_core_cache_set(uint32_t base, uint32_t value);

        static inline uint32_t cluster_icache_ctrl_enable_l1_l15_prefetch_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_l1_l15_prefetch_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster instruction private cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_BIT                        0
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_WIDTH                      9
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_MASK                       0x1ff
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_RESET                      0x0
        
        // Cluster instruction shared cache banks enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_BIT                         9
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_WIDTH                       2
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_MASK                        0x600
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_RESET                       0x0
        
        // Cluster instruction private cache full flush command. (access: W)
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_BIT                         0
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_WIDTH                       9
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_MASK                        0x1ff
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_RESET                       0x0
        
        // Cluster instruction shared cache banks full flush command. (access: W)
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_BIT                          9
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_WIDTH                        2
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_MASK                         0x600
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_RESET                        0x0
        
        // Cluster level 0 instruction cache full flush command.  (access: W)
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_BIT                       0
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_WIDTH                     9
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_MASK                      0x1ff
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_RESET                     0x0
        
        // Cluster instruction cache selective flush address configuration bitfield. (access: W)
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT                       0
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH                     32
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_MASK                      0xffffffff
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_RESET                     0x0
        
        // Enable or disable 9th core seperated icache: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_BIT     0
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_WIDTH   1
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_MASK    0x1
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_RESET   0x0
        
        // Enable or disable all 9  private icache prefetch feature, for each bitfield: - 9'b000000000: disabled all private icache prefetch - 9'b000000001: enabled private icache 0 prefetch - 9'b000000011: enabled private icache 0, 1  prefetch ... - 9'b111111111: enabled all private icache prefetch (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_BIT        0
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_WIDTH      9
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_MASK       0x1ff
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_RESET      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_GET(value)       (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_GETS(value)      (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI(val)             ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_GET(value)        (GAP_BEXTRACTU((value),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_GETS(value)       (GAP_BEXTRACT((value),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_SET(value,field)  (GAP_BINSERT((value),(field),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH(val)              ((val) << 9)
        
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_GET(value)        (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_GETS(value)       (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_SET(value,field)  (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI(val)              ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_GET(value)         (GAP_BEXTRACTU((value),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_GETS(value)        (GAP_BEXTRACT((value),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_SET(value,field)   (GAP_BINSERT((value),(field),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH(val)               ((val) << 9)
        
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_GET(value)      (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_GETS(value)     (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL(val)            ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR(val)            ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE(val) ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_GET(value) (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_GETS(value) (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE(val) ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** CLUSTER_ICACHE_CTRL_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t enable;  // Cluster instruction cache unit enable configuration register.
            volatile uint32_t flush;  // Cluster instruction cache unit flush command register.
            volatile uint32_t l0_flush;  // Cluster level 0 instruction cache unit flush command register.
            volatile uint32_t sel_flush;  // Cluster instruction cache unit selective flush command register.
            volatile uint32_t reserved_0[2];  // Reserved/Not used.
            volatile uint32_t enable_special_core_cache;  // When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.
            volatile uint32_t enable_l1_l15_prefetch;  // For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch 
        } __attribute__((packed)) cluster_icache_ctrl_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en_pri          :9 ; // Cluster instruction private cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int en_sh           :2 ; // Cluster instruction shared cache banks enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_t;
        
        typedef union {
          struct {
            unsigned int fl_pri          :9 ; // Cluster instruction private cache full flush command.
            unsigned int fl_sh           :2 ; // Cluster instruction shared cache banks full flush command.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_flush_t;
        
        typedef union {
          struct {
            unsigned int l0_fl           :9 ; // Cluster level 0 instruction cache full flush command. 
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_l0_flush_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Cluster instruction cache selective flush address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_sel_flush_t;
        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Enable or disable 9th core seperated icache: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_special_core_cache_t;
        
        typedef union {
          struct {
            unsigned int enable          :9 ; // Enable or disable all 9  private icache prefetch feature, for each bitfield: - 9'b000000000: disabled all private icache prefetch - 9'b000000001: enabled private icache 0 prefetch - 9'b000000011: enabled private icache 0, 1  prefetch ... - 9'b111111111: enabled all private icache prefetch
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_l1_l15_prefetch_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_cluster_icache_ctrl : public vp::regmap
        {
        public:
            vp_cluster_icache_ctrl_enable enable;
            vp_cluster_icache_ctrl_flush flush;
            vp_cluster_icache_ctrl_l0_flush l0_flush;
            vp_cluster_icache_ctrl_sel_flush sel_flush;
            vp_cluster_icache_ctrl_enable_special_core_cache enable_special_core_cache;
            vp_cluster_icache_ctrl_enable_l1_l15_prefetch enable_l1_l15_prefetch;
        };

|

.. _cluster_icache_ctrl_ENABLE:

ENABLE
""""""

Cluster instruction cache unit enable configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                             Description                                              |
    +=====+===+======+======================================================================================================+
    |8:0  |W  |EN_PRI|Cluster instruction private cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled     |
    +-----+---+------+------------------------------------------------------------------------------------------------------+
    |10:9 |W  |EN_SH |Cluster instruction shared cache banks enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+------+------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster instruction cache unit enable configuration register.
                #define CLUSTER_ICACHE_CTRL_ENABLE_OFFSET        0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_enable_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster instruction private cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_BIT                        0
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_WIDTH                      9
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_MASK                       0x1ff
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_RESET                      0x0
        
        // Cluster instruction shared cache banks enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_BIT                         9
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_WIDTH                       2
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_MASK                        0x600
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_GET(value)       (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_GETS(value)      (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_PRI(val)             ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_GET(value)        (GAP_BEXTRACTU((value),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_GETS(value)       (GAP_BEXTRACT((value),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH_SET(value,field)  (GAP_BINSERT((value),(field),2,9))
        #define CLUSTER_ICACHE_CTRL_ENABLE_EN_SH(val)              ((val) << 9)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int en_pri          :9 ; // Cluster instruction private cache enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int en_sh           :2 ; // Cluster instruction shared cache banks enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_enable : public vp::reg_32
        {
        public:
            inline void en_pri_set(uint32_t value);
            inline uint32_t en_pri_get();
            inline void en_sh_set(uint32_t value);
            inline uint32_t en_sh_get();
        };

|

.. _cluster_icache_ctrl_FLUSH:

FLUSH
"""""

Cluster instruction cache unit flush command register.

.. table:: 

    +-----+---+------+----------------------------------------------------------+
    |Bit #|R/W| Name |                       Description                        |
    +=====+===+======+==========================================================+
    |8:0  |W  |FL_PRI|Cluster instruction private cache full flush command.     |
    +-----+---+------+----------------------------------------------------------+
    |10:9 |W  |FL_SH |Cluster instruction shared cache banks full flush command.|
    +-----+---+------+----------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster instruction cache unit flush command register.
                #define CLUSTER_ICACHE_CTRL_FLUSH_OFFSET         0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_flush_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster instruction private cache full flush command. (access: W)
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_BIT                         0
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_WIDTH                       9
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_MASK                        0x1ff
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_RESET                       0x0
        
        // Cluster instruction shared cache banks full flush command. (access: W)
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_BIT                          9
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_WIDTH                        2
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_MASK                         0x600
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_GET(value)        (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_GETS(value)       (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI_SET(value,field)  (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_PRI(val)              ((val) << 0)
        
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_GET(value)         (GAP_BEXTRACTU((value),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_GETS(value)        (GAP_BEXTRACT((value),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH_SET(value,field)   (GAP_BINSERT((value),(field),2,9))
        #define CLUSTER_ICACHE_CTRL_FLUSH_FL_SH(val)               ((val) << 9)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int fl_pri          :9 ; // Cluster instruction private cache full flush command.
            unsigned int fl_sh           :2 ; // Cluster instruction shared cache banks full flush command.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_flush_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_flush : public vp::reg_32
        {
        public:
            inline void fl_pri_set(uint32_t value);
            inline uint32_t fl_pri_get();
            inline void fl_sh_set(uint32_t value);
            inline uint32_t fl_sh_get();
        };

|

.. _cluster_icache_ctrl_L0_FLUSH:

L0_FLUSH
""""""""

Cluster level 0 instruction cache unit flush command register.

.. table:: 

    +-----+---+-----+------------------------------------------------------+
    |Bit #|R/W|Name |                     Description                      |
    +=====+===+=====+======================================================+
    |8:0  |W  |L0_FL|Cluster level 0 instruction cache full flush command. |
    +-----+---+-----+------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster level 0 instruction cache unit flush command register.
                #define CLUSTER_ICACHE_CTRL_L0_FLUSH_OFFSET      0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_l0_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_l0_flush_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster level 0 instruction cache full flush command.  (access: W)
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_BIT                       0
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_WIDTH                     9
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_MASK                      0x1ff
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_GET(value)      (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_GETS(value)     (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_L0_FLUSH_L0_FL(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int l0_fl           :9 ; // Cluster level 0 instruction cache full flush command. 
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_l0_flush_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_l0_flush : public vp::reg_32
        {
        public:
            inline void l0_fl_set(uint32_t value);
            inline uint32_t l0_fl_get();
        };

|

.. _cluster_icache_ctrl_SEL_FLUSH:

SEL_FLUSH
"""""""""

Cluster instruction cache unit selective flush command register.

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                               |
    +=====+===+====+=========================================================================+
    |31:0 |W  |ADDR|Cluster instruction cache selective flush address configuration bitfield.|
    +-----+---+----+-------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster instruction cache unit selective flush command register.
                #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_OFFSET     0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_sel_flush_get(uint32_t base);
        static inline void cluster_icache_ctrl_sel_flush_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Cluster instruction cache selective flush address configuration bitfield. (access: W)
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_BIT                       0
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_WIDTH                     32
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_MASK                      0xffffffff
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define CLUSTER_ICACHE_CTRL_SEL_FLUSH_ADDR(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Cluster instruction cache selective flush address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_sel_flush_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_sel_flush : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _cluster_icache_ctrl_ENABLE_SPECIAL_CORE_CACHE:

ENABLE_SPECIAL_CORE_CACHE
"""""""""""""""""""""""""

When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                 Description                                 |
    +=====+===+======+=============================================================================+
    |    0|W  |ENABLE|Enable or disable 9th core seperated icache: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+------+-----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // When use 9th core, the number 8 core can have seperated icache or its own icache. Use this register to control.
                #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_OFFSET 0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_enable_special_core_cache_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_special_core_cache_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable or disable 9th core seperated icache: - 1'b0: disabled - 1'b1: enabled (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_BIT     0
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_WIDTH   1
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_MASK    0x1
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_RESET   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_SPECIAL_CORE_CACHE_ENABLE(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :1 ; // Enable or disable 9th core seperated icache: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_special_core_cache_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_enable_special_core_cache : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|

.. _cluster_icache_ctrl_ENABLE_L1_L15_PREFETCH:

ENABLE_L1_L15_PREFETCH
""""""""""""""""""""""

For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch 

.. table:: 

    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                           Description                                                                                                                                           |
    +=====+===+======+=================================================================================================================================================================================================================================================================================================+
    |8:0  |W  |ENABLE|Enable or disable all 9  private icache prefetch feature, for each bitfield: - 9'b000000000: disabled all private icache prefetch - 9'b000000001: enabled private icache 0 prefetch - 9'b000000011: enabled private icache 0, 1  prefetch ... - 9'b111111111: enabled all private icache prefetch|
    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // For each Level 0 private cache, enable L0 to L0.5, (also called L1 to L1.5) prefetch 
                #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_OFFSET 0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cluster_icache_ctrl_enable_l1_l15_prefetch_get(uint32_t base);
        static inline void cluster_icache_ctrl_enable_l1_l15_prefetch_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable or disable all 9  private icache prefetch feature, for each bitfield: - 9'b000000000: disabled all private icache prefetch - 9'b000000001: enabled private icache 0 prefetch - 9'b000000011: enabled private icache 0, 1  prefetch ... - 9'b111111111: enabled all private icache prefetch (access: W)
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_BIT        0
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_WIDTH      9
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_MASK       0x1ff
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_RESET      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_GET(value) (GAP_BEXTRACTU((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_GETS(value) (GAP_BEXTRACT((value),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE_SET(value,field) (GAP_BINSERT((value),(field),9,0))
        #define CLUSTER_ICACHE_CTRL_ENABLE_L1_L15_PREFETCH_ENABLE(val) ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int enable          :9 ; // Enable or disable all 9  private icache prefetch feature, for each bitfield: - 9'b000000000: disabled all private icache prefetch - 9'b000000001: enabled private icache 0 prefetch - 9'b000000011: enabled private icache 0, 1  prefetch ... - 9'b111111111: enabled all private icache prefetch
          };
          unsigned int raw;
        } __attribute__((packed)) cluster_icache_ctrl_enable_l1_l15_prefetch_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cluster_icache_ctrl_enable_l1_l15_prefetch : public vp::reg_32
        {
        public:
            inline void enable_set(uint32_t value);
            inline uint32_t enable_get();
        };

|
