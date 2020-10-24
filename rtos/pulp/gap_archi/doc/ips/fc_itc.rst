Input file: fe/ips/apb_interrupt_cntrl/docs/FC_ITC_reference.xlsx

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

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // FC_ITC Mask status register.
                #define FC_ITC_MASK_OFFSET                       0x0
        
                // FC_ITC Mask set register.
                #define FC_ITC_MASK_SET_OFFSET                   0x4
        
                // FC_ITC Mask clear register.
                #define FC_ITC_MASK_CLEAR_OFFSET                 0x8
        
                // FC_ITC interrupt status register.
                #define FC_ITC_STATUS_OFFSET                     0xc
        
                // FC_ITC interrupt enable register.
                #define FC_ITC_STATUS_SET_OFFSET                 0x10
        
                // FC_ITC interrupt clear register.
                #define FC_ITC_STATUS_CLEAR_OFFSET               0x14
        
                // FC_ITC interrupt ack status register.
                #define FC_ITC_ACK_OFFSET                        0x18
        
                // FC_ITC interrupt ack set register.
                #define FC_ITC_ACK_SET_OFFSET                    0x1c
        
                // FC_ITC interrupt ack clear register.
                #define FC_ITC_ACK_CLEAR_OFFSET                  0x20
        
                // FC_ITC event number fifo register.
                #define FC_ITC_FIFO_OFFSET                       0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_mask_get(uint32_t base);
        static inline void fc_itc_mask_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_mask_set_get(uint32_t base);
        static inline void fc_itc_mask_set_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_mask_clear_get(uint32_t base);
        static inline void fc_itc_mask_clear_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_status_get(uint32_t base);
        static inline void fc_itc_status_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_status_set_get(uint32_t base);
        static inline void fc_itc_status_set_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_status_clear_get(uint32_t base);
        static inline void fc_itc_status_clear_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_ack_get(uint32_t base);
        static inline void fc_itc_ack_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_ack_set_get(uint32_t base);
        static inline void fc_itc_ack_set_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_ack_clear_get(uint32_t base);
        static inline void fc_itc_ack_clear_set(uint32_t base, uint32_t value);

        static inline uint32_t fc_itc_fifo_get(uint32_t base);
        static inline void fc_itc_fifo_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c


.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c


.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** FC_ITC_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t mask;  // FC_ITC Mask status register.
            volatile uint32_t mask_set;  // FC_ITC Mask set register.
            volatile uint32_t mask_clear;  // FC_ITC Mask clear register.
            volatile uint32_t status;  // FC_ITC interrupt status register.
            volatile uint32_t status_set;  // FC_ITC interrupt enable register.
            volatile uint32_t status_clear;  // FC_ITC interrupt clear register.
            volatile uint32_t ack;  // FC_ITC interrupt ack status register.
            volatile uint32_t ack_set;  // FC_ITC interrupt ack set register.
            volatile uint32_t ack_clear;  // FC_ITC interrupt ack clear register.
            volatile uint32_t fifo;  // FC_ITC event number fifo register.
        } __attribute__((packed)) fc_itc_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_mask_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_mask_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_mask_clear_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_status_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_status_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_status_clear_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_ack_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_ack_set_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_ack_clear_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) fc_itc_fifo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_fc_itc : public vp::regmap
        {
        public:
            vp_fc_itc_mask mask;
            vp_fc_itc_mask_set mask_set;
            vp_fc_itc_mask_clear mask_clear;
            vp_fc_itc_status status;
            vp_fc_itc_status_set status_set;
            vp_fc_itc_status_clear status_clear;
            vp_fc_itc_ack ack;
            vp_fc_itc_ack_set ack_set;
            vp_fc_itc_ack_clear ack_clear;
            vp_fc_itc_fifo fifo;
        };

|

.. _fc_itc_MASK:

MASK
""""

FC_ITC Mask status register.

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

        
                // FC_ITC Mask status register.
                #define FC_ITC_MASK_OFFSET                       0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_mask_get(uint32_t base);
        static inline void fc_itc_mask_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_mask_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_mask : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_MASK_SET:

MASK_SET
""""""""

FC_ITC Mask set register.

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

        
                // FC_ITC Mask set register.
                #define FC_ITC_MASK_SET_OFFSET                   0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_mask_set_get(uint32_t base);
        static inline void fc_itc_mask_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_mask_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_mask_set : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_MASK_CLEAR:

MASK_CLEAR
""""""""""

FC_ITC Mask clear register.

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

        
                // FC_ITC Mask clear register.
                #define FC_ITC_MASK_CLEAR_OFFSET                 0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_mask_clear_get(uint32_t base);
        static inline void fc_itc_mask_clear_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_mask_clear_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_mask_clear : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_STATUS:

STATUS
""""""

FC_ITC interrupt status register.

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

        
                // FC_ITC interrupt status register.
                #define FC_ITC_STATUS_OFFSET                     0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_status_get(uint32_t base);
        static inline void fc_itc_status_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_status : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_STATUS_SET:

STATUS_SET
""""""""""

FC_ITC interrupt enable register.

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

        
                // FC_ITC interrupt enable register.
                #define FC_ITC_STATUS_SET_OFFSET                 0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_status_set_get(uint32_t base);
        static inline void fc_itc_status_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_status_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_status_set : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_STATUS_CLEAR:

STATUS_CLEAR
""""""""""""

FC_ITC interrupt clear register.

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

        
                // FC_ITC interrupt clear register.
                #define FC_ITC_STATUS_CLEAR_OFFSET               0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_status_clear_get(uint32_t base);
        static inline void fc_itc_status_clear_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_status_clear_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_status_clear : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_ACK:

ACK
"""

FC_ITC interrupt ack status register.

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

        
                // FC_ITC interrupt ack status register.
                #define FC_ITC_ACK_OFFSET                        0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_ack_get(uint32_t base);
        static inline void fc_itc_ack_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_ack_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_ack : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_ACK_SET:

ACK_SET
"""""""

FC_ITC interrupt ack set register.

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

        
                // FC_ITC interrupt ack set register.
                #define FC_ITC_ACK_SET_OFFSET                    0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_ack_set_get(uint32_t base);
        static inline void fc_itc_ack_set_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_ack_set_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_ack_set : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_ACK_CLEAR:

ACK_CLEAR
"""""""""

FC_ITC interrupt ack clear register.

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

        
                // FC_ITC interrupt ack clear register.
                #define FC_ITC_ACK_CLEAR_OFFSET                  0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_ack_clear_get(uint32_t base);
        static inline void fc_itc_ack_clear_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_ack_clear_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_ack_clear : public vp::reg_32
        {
        public:
        };

|

.. _fc_itc_FIFO:

FIFO
""""

FC_ITC event number fifo register.

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

        
                // FC_ITC event number fifo register.
                #define FC_ITC_FIFO_OFFSET                       0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t fc_itc_fifo_get(uint32_t base);
        static inline void fc_itc_fifo_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) fc_itc_fifo_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_fc_itc_fifo : public vp::reg_32
        {
        public:
        };

|
