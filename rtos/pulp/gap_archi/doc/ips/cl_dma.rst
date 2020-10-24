Input file: fe/ips/mchan/doc/CL_DMA_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +----------------------------+------+-----+-----------------------------------+
    |            Name            |Offset|Width|            Description            |
    +============================+======+=====+===================================+
    |:ref:`CMD<cl_dma_CMD>`      |     0|   32|Cluster DMA configuration register.|
    +----------------------------+------+-----+-----------------------------------+
    |:ref:`STATUS<cl_dma_STATUS>`|     4|   32|Cluster DMA status register.       |
    +----------------------------+------+-----+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster DMA configuration register.
                #define CL_DMA_CMD_OFFSET                        0x0
        
                // Cluster DMA status register.
                #define CL_DMA_STATUS_OFFSET                     0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cl_dma_cmd_get(uint32_t base);
        static inline void cl_dma_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t cl_dma_status_get(uint32_t base);
        static inline void cl_dma_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Format is operation dependent. See below. (access: R/W)
        #define CL_DMA_CMD_CMD_BIT                                           0
        #define CL_DMA_CMD_CMD_WIDTH                                         32
        #define CL_DMA_CMD_CMD_MASK                                          0xffffffff
        #define CL_DMA_CMD_CMD_RESET                                         0x0
        
        // Format is operation dependent. See below. (access: R/W)
        #define CL_DMA_STATUS_STATUS_BIT                                     0
        #define CL_DMA_STATUS_STATUS_WIDTH                                   32
        #define CL_DMA_STATUS_STATUS_MASK                                    0xffffffff
        #define CL_DMA_STATUS_STATUS_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CL_DMA_CMD_CMD_GET(value)                          (GAP_BEXTRACTU((value),32,0))
        #define CL_DMA_CMD_CMD_GETS(value)                         (GAP_BEXTRACT((value),32,0))
        #define CL_DMA_CMD_CMD_SET(value,field)                    (GAP_BINSERT((value),(field),32,0))
        #define CL_DMA_CMD_CMD(val)                                ((val) << 0)
        
        #define CL_DMA_STATUS_STATUS_GET(value)                    (GAP_BEXTRACTU((value),32,0))
        #define CL_DMA_STATUS_STATUS_GETS(value)                   (GAP_BEXTRACT((value),32,0))
        #define CL_DMA_STATUS_STATUS_SET(value,field)              (GAP_BINSERT((value),(field),32,0))
        #define CL_DMA_STATUS_STATUS(val)                          ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** CL_DMA_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cmd;  // Cluster DMA configuration register.
            volatile uint32_t status;  // Cluster DMA status register.
        } __attribute__((packed)) cl_dma_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd             :32; // Format is operation dependent. See below.
          };
          unsigned int raw;
        } __attribute__((packed)) cl_dma_cmd_t;
        
        typedef union {
          struct {
            unsigned int status          :32; // Format is operation dependent. See below.
          };
          unsigned int raw;
        } __attribute__((packed)) cl_dma_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_cl_dma : public vp::regmap
        {
        public:
            vp_cl_dma_cmd cmd;
            vp_cl_dma_status status;
        };

|

.. _cl_dma_CMD:

CMD
"""

Cluster DMA configuration register.

.. table:: 

    +-----+---+----+-----------------------------------------+
    |Bit #|R/W|Name|               Description               |
    +=====+===+====+=========================================+
    |31:0 |R/W|CMD |Format is operation dependent. See below.|
    +-----+---+----+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster DMA configuration register.
                #define CL_DMA_CMD_OFFSET                        0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cl_dma_cmd_get(uint32_t base);
        static inline void cl_dma_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Format is operation dependent. See below. (access: R/W)
        #define CL_DMA_CMD_CMD_BIT                                           0
        #define CL_DMA_CMD_CMD_WIDTH                                         32
        #define CL_DMA_CMD_CMD_MASK                                          0xffffffff
        #define CL_DMA_CMD_CMD_RESET                                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CL_DMA_CMD_CMD_GET(value)                          (GAP_BEXTRACTU((value),32,0))
        #define CL_DMA_CMD_CMD_GETS(value)                         (GAP_BEXTRACT((value),32,0))
        #define CL_DMA_CMD_CMD_SET(value,field)                    (GAP_BINSERT((value),(field),32,0))
        #define CL_DMA_CMD_CMD(val)                                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cmd             :32; // Format is operation dependent. See below.
          };
          unsigned int raw;
        } __attribute__((packed)) cl_dma_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cl_dma_cmd : public vp::reg_32
        {
        public:
            inline void cmd_set(uint32_t value);
            inline uint32_t cmd_get();
        };

|

.. _cl_dma_STATUS:

STATUS
""""""

Cluster DMA status register.

.. table:: 

    +-----+---+------+-----------------------------------------+
    |Bit #|R/W| Name |               Description               |
    +=====+===+======+=========================================+
    |31:0 |R/W|STATUS|Format is operation dependent. See below.|
    +-----+---+------+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cluster DMA status register.
                #define CL_DMA_STATUS_OFFSET                     0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t cl_dma_status_get(uint32_t base);
        static inline void cl_dma_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Format is operation dependent. See below. (access: R/W)
        #define CL_DMA_STATUS_STATUS_BIT                                     0
        #define CL_DMA_STATUS_STATUS_WIDTH                                   32
        #define CL_DMA_STATUS_STATUS_MASK                                    0xffffffff
        #define CL_DMA_STATUS_STATUS_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define CL_DMA_STATUS_STATUS_GET(value)                    (GAP_BEXTRACTU((value),32,0))
        #define CL_DMA_STATUS_STATUS_GETS(value)                   (GAP_BEXTRACT((value),32,0))
        #define CL_DMA_STATUS_STATUS_SET(value,field)              (GAP_BINSERT((value),(field),32,0))
        #define CL_DMA_STATUS_STATUS(val)                          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int status          :32; // Format is operation dependent. See below.
          };
          unsigned int raw;
        } __attribute__((packed)) cl_dma_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_cl_dma_status : public vp::reg_32
        {
        public:
            inline void status_set(uint32_t value);
            inline uint32_t status_get();
        };

|
