Input file: fe/ips/rtc_dolphin/docs/RTC_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-----------------------------+------+-----+--------------------------------------+
    |            Name             |Offset|Width|             Description              |
    +=============================+======+=====+======================================+
    |:ref:`APB_SR<rtc_APB_SR>`    |     0|   32|RTC APB status register.              |
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`APB_CR<rtc_APB_CR>`    |     4|   32|RTC APB control register.             |
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`APB_DR<rtc_APB_DR>`    |     8|   32|RTC APB data register.                |
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`RESERVED<rtc_RESERVED>`|    12|   32|Reserved/Not used.                    |
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`APB_ICR<rtc_APB_ICR>`  |    16|   32|RTC APB interruption control register.|
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`APB_IMR<rtc_APB_IMR>`  |    20|   32|RTC APB interruption mask register.   |
    +-----------------------------+------+-----+--------------------------------------+
    |:ref:`APB_IFR<rtc_APB_IFR>`  |    24|   32|RTC APB interruption flag register.   |
    +-----------------------------+------+-----+--------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB status register.
                #define RTC_APB_SR_OFFSET                        0x0
        
                // RTC APB control register.
                #define RTC_APB_CR_OFFSET                        0x4
        
                // RTC APB data register.
                #define RTC_APB_DR_OFFSET                        0x8
        
                // Reserved/Not used.
                #define RTC_RESERVED_OFFSET                      0xc
        
                // RTC APB interruption control register.
                #define RTC_APB_ICR_OFFSET                       0x10
        
                // RTC APB interruption mask register.
                #define RTC_APB_IMR_OFFSET                       0x14
        
                // RTC APB interruption flag register.
                #define RTC_APB_IFR_OFFSET                       0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_sr_get(uint32_t base);
        static inline void rtc_apb_sr_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_apb_cr_get(uint32_t base);
        static inline void rtc_apb_cr_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_apb_dr_get(uint32_t base);
        static inline void rtc_apb_dr_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_reserved_get(uint32_t base);
        static inline void rtc_reserved_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_apb_icr_get(uint32_t base);
        static inline void rtc_apb_icr_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_apb_imr_get(uint32_t base);
        static inline void rtc_apb_imr_set(uint32_t base, uint32_t value);

        static inline uint32_t rtc_apb_ifr_get(uint32_t base);
        static inline void rtc_apb_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested (access: R)
        #define RTC_APB_SR_APB_INT_BIT                                       0
        #define RTC_APB_SR_APB_INT_WIDTH                                     2
        #define RTC_APB_SR_APB_INT_MASK                                      0x3
        #define RTC_APB_SR_APB_INT_RESET                                     0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_SR_RESERVED_0_BIT                                    2
        #define RTC_APB_SR_RESERVED_0_WIDTH                                  30
        #define RTC_APB_SR_RESERVED_0_MASK                                   0xfffffffc
        #define RTC_APB_SR_RESERVED_0_RESET                                  0x0
        
        // Indirect access address configuration. (access: R/W)
        #define RTC_APB_CR_APB_ADDR_BIT                                      0
        #define RTC_APB_CR_APB_ADDR_WIDTH                                    6
        #define RTC_APB_CR_APB_ADDR_MASK                                     0x3f
        #define RTC_APB_CR_APB_ADDR_RESET                                    0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_CR_RESERVED_0_BIT                                    6
        #define RTC_APB_CR_RESERVED_0_WIDTH                                  10
        #define RTC_APB_CR_RESERVED_0_MASK                                   0xffc0
        #define RTC_APB_CR_RESERVED_0_RESET                                  0x0
        
        // Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation (access: R/W)
        #define RTC_APB_CR_APB_OP_BIT                                        16
        #define RTC_APB_CR_APB_OP_WIDTH                                      1
        #define RTC_APB_CR_APB_OP_MASK                                       0x10000
        #define RTC_APB_CR_APB_OP_RESET                                      0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_CR_RESERVED_1_BIT                                    17
        #define RTC_APB_CR_RESERVED_1_WIDTH                                  15
        #define RTC_APB_CR_RESERVED_1_MASK                                   0xfffe0000
        #define RTC_APB_CR_RESERVED_1_RESET                                  0x0
        
        // Indirect access data register. (access: R/W)
        #define RTC_APB_DR_APB_DATA_BIT                                      0
        #define RTC_APB_DR_APB_DATA_WIDTH                                    32
        #define RTC_APB_DR_APB_DATA_MASK                                     0xffffffff
        #define RTC_APB_DR_APB_DATA_RESET                                    0x0
        
        // APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration (access: R/W)
        #define RTC_APB_ICR_MODE_BIT                                         0
        #define RTC_APB_ICR_MODE_WIDTH                                       2
        #define RTC_APB_ICR_MODE_MASK                                        0x3
        #define RTC_APB_ICR_MODE_RESET                                       0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_ICR_RESERVED_0_BIT                                   2
        #define RTC_APB_ICR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_ICR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_ICR_RESERVED_0_RESET                                 0x0
        
        // APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
        #define RTC_APB_IMR_READ_MASK_BIT                                    0
        #define RTC_APB_IMR_READ_MASK_WIDTH                                  1
        #define RTC_APB_IMR_READ_MASK_MASK                                   0x1
        #define RTC_APB_IMR_READ_MASK_RESET                                  0x0
        
        // APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
        #define RTC_APB_IMR_WRITE_MASK_BIT                                   1
        #define RTC_APB_IMR_WRITE_MASK_WIDTH                                 1
        #define RTC_APB_IMR_WRITE_MASK_MASK                                  0x2
        #define RTC_APB_IMR_WRITE_MASK_RESET                                 0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_IMR_RESERVED_0_BIT                                   2
        #define RTC_APB_IMR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_IMR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_IMR_RESERVED_0_RESET                                 0x0
        
        // APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available (access: R/W)
        #define RTC_APB_IFR_READ_FLAG_BIT                                    0
        #define RTC_APB_IFR_READ_FLAG_WIDTH                                  1
        #define RTC_APB_IFR_READ_FLAG_MASK                                   0x1
        #define RTC_APB_IFR_READ_FLAG_RESET                                  0x0
        
        // APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done (access: R/W)
        #define RTC_APB_IFR_WRITE_FLAG_BIT                                   1
        #define RTC_APB_IFR_WRITE_FLAG_WIDTH                                 1
        #define RTC_APB_IFR_WRITE_FLAG_MASK                                  0x2
        #define RTC_APB_IFR_WRITE_FLAG_RESET                                 0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_IFR_RESERVED_0_BIT                                   2
        #define RTC_APB_IFR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_IFR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_IFR_RESERVED_0_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_SR_APB_INT_GET(value)                      (GAP_BEXTRACTU((value),2,0))
        #define RTC_APB_SR_APB_INT_GETS(value)                     (GAP_BEXTRACT((value),2,0))
        #define RTC_APB_SR_APB_INT_SET(value,field)                (GAP_BINSERT((value),(field),2,0))
        #define RTC_APB_SR_APB_INT(val)                            ((val) << 0)
        
        #define RTC_APB_SR_RESERVED_0_GET(value)                   (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_SR_RESERVED_0_GETS(value)                  (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_SR_RESERVED_0_SET(value,field)             (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_SR_RESERVED_0(val)                         ((val) << 2)
        
        #define RTC_APB_CR_APB_ADDR_GET(value)                     (GAP_BEXTRACTU((value),6,0))
        #define RTC_APB_CR_APB_ADDR_GETS(value)                    (GAP_BEXTRACT((value),6,0))
        #define RTC_APB_CR_APB_ADDR_SET(value,field)               (GAP_BINSERT((value),(field),6,0))
        #define RTC_APB_CR_APB_ADDR(val)                           ((val) << 0)
        
        #define RTC_APB_CR_RESERVED_0_GET(value)                   (GAP_BEXTRACTU((value),10,6))
        #define RTC_APB_CR_RESERVED_0_GETS(value)                  (GAP_BEXTRACT((value),10,6))
        #define RTC_APB_CR_RESERVED_0_SET(value,field)             (GAP_BINSERT((value),(field),10,6))
        #define RTC_APB_CR_RESERVED_0(val)                         ((val) << 6)
        
        #define RTC_APB_CR_APB_OP_GET(value)                       (GAP_BEXTRACTU((value),1,16))
        #define RTC_APB_CR_APB_OP_GETS(value)                      (GAP_BEXTRACT((value),1,16))
        #define RTC_APB_CR_APB_OP_SET(value,field)                 (GAP_BINSERT((value),(field),1,16))
        #define RTC_APB_CR_APB_OP(val)                             ((val) << 16)
        
        #define RTC_APB_CR_RESERVED_1_GET(value)                   (GAP_BEXTRACTU((value),15,17))
        #define RTC_APB_CR_RESERVED_1_GETS(value)                  (GAP_BEXTRACT((value),15,17))
        #define RTC_APB_CR_RESERVED_1_SET(value,field)             (GAP_BINSERT((value),(field),15,17))
        #define RTC_APB_CR_RESERVED_1(val)                         ((val) << 17)
        
        #define RTC_APB_DR_APB_DATA_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define RTC_APB_DR_APB_DATA_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define RTC_APB_DR_APB_DATA_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define RTC_APB_DR_APB_DATA(val)                           ((val) << 0)
        
        #define RTC_APB_ICR_MODE_GET(value)                        (GAP_BEXTRACTU((value),2,0))
        #define RTC_APB_ICR_MODE_GETS(value)                       (GAP_BEXTRACT((value),2,0))
        #define RTC_APB_ICR_MODE_SET(value,field)                  (GAP_BINSERT((value),(field),2,0))
        #define RTC_APB_ICR_MODE(val)                              ((val) << 0)
        
        #define RTC_APB_ICR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_ICR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_ICR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_ICR_RESERVED_0(val)                        ((val) << 2)
        
        #define RTC_APB_IMR_READ_MASK_GET(value)                   (GAP_BEXTRACTU((value),1,0))
        #define RTC_APB_IMR_READ_MASK_GETS(value)                  (GAP_BEXTRACT((value),1,0))
        #define RTC_APB_IMR_READ_MASK_SET(value,field)             (GAP_BINSERT((value),(field),1,0))
        #define RTC_APB_IMR_READ_MASK(val)                         ((val) << 0)
        
        #define RTC_APB_IMR_WRITE_MASK_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define RTC_APB_IMR_WRITE_MASK_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define RTC_APB_IMR_WRITE_MASK_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define RTC_APB_IMR_WRITE_MASK(val)                        ((val) << 1)
        
        #define RTC_APB_IMR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_IMR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_IMR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_IMR_RESERVED_0(val)                        ((val) << 2)
        
        #define RTC_APB_IFR_READ_FLAG_GET(value)                   (GAP_BEXTRACTU((value),1,0))
        #define RTC_APB_IFR_READ_FLAG_GETS(value)                  (GAP_BEXTRACT((value),1,0))
        #define RTC_APB_IFR_READ_FLAG_SET(value,field)             (GAP_BINSERT((value),(field),1,0))
        #define RTC_APB_IFR_READ_FLAG(val)                         ((val) << 0)
        
        #define RTC_APB_IFR_WRITE_FLAG_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define RTC_APB_IFR_WRITE_FLAG_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define RTC_APB_IFR_WRITE_FLAG_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define RTC_APB_IFR_WRITE_FLAG(val)                        ((val) << 1)
        
        #define RTC_APB_IFR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_IFR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_IFR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_IFR_RESERVED_0(val)                        ((val) << 2)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** RTC_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t apb_sr;  // RTC APB status register.
            volatile uint32_t apb_cr;  // RTC APB control register.
            volatile uint32_t apb_dr;  // RTC APB data register.
            volatile uint32_t reserved;  // Reserved/Not used.
            volatile uint32_t apb_icr;  // RTC APB interruption control register.
            volatile uint32_t apb_imr;  // RTC APB interruption mask register.
            volatile uint32_t apb_ifr;  // RTC APB interruption flag register.
        } __attribute__((packed)) rtc_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int apb_int         :2 ; // APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_sr_t;
        
        typedef union {
          struct {
            unsigned int apb_addr        :6 ; // Indirect access address configuration.
            unsigned int reserved_0      :10; // Reserved/Not used.
            unsigned int apb_op          :1 ; // Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation
            unsigned int reserved_1      :15; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_cr_t;
        
        typedef union {
          struct {
            unsigned int apb_data        :32; // Indirect access data register.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_dr_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_reserved_t;
        
        typedef union {
          struct {
            unsigned int mode            :2 ; // APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_icr_t;
        
        typedef union {
          struct {
            unsigned int read_mask       :1 ; // APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
            unsigned int write_mask      :1 ; // APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_imr_t;
        
        typedef union {
          struct {
            unsigned int read_flag       :1 ; // APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available
            unsigned int write_flag      :1 ; // APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_rtc : public vp::regmap
        {
        public:
            vp_rtc_apb_sr apb_sr;
            vp_rtc_apb_cr apb_cr;
            vp_rtc_apb_dr apb_dr;
            vp_rtc_reserved reserved;
            vp_rtc_apb_icr apb_icr;
            vp_rtc_apb_imr apb_imr;
            vp_rtc_apb_ifr apb_ifr;
        };

|

.. _rtc_APB_SR:

APB_SR
""""""

RTC APB status register.

.. table:: 

    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                               Description                                               |
    +=====+===+==========+=========================================================================================================+
    |1:0  |R  |APB_INT   |APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested|
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |31:2 |R/W|RESERVED_0|Reserved/Not used.                                                                                       |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB status register.
                #define RTC_APB_SR_OFFSET                        0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_sr_get(uint32_t base);
        static inline void rtc_apb_sr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested (access: R)
        #define RTC_APB_SR_APB_INT_BIT                                       0
        #define RTC_APB_SR_APB_INT_WIDTH                                     2
        #define RTC_APB_SR_APB_INT_MASK                                      0x3
        #define RTC_APB_SR_APB_INT_RESET                                     0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_SR_RESERVED_0_BIT                                    2
        #define RTC_APB_SR_RESERVED_0_WIDTH                                  30
        #define RTC_APB_SR_RESERVED_0_MASK                                   0xfffffffc
        #define RTC_APB_SR_RESERVED_0_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_SR_APB_INT_GET(value)                      (GAP_BEXTRACTU((value),2,0))
        #define RTC_APB_SR_APB_INT_GETS(value)                     (GAP_BEXTRACT((value),2,0))
        #define RTC_APB_SR_APB_INT_SET(value,field)                (GAP_BINSERT((value),(field),2,0))
        #define RTC_APB_SR_APB_INT(val)                            ((val) << 0)
        
        #define RTC_APB_SR_RESERVED_0_GET(value)                   (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_SR_RESERVED_0_GETS(value)                  (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_SR_RESERVED_0_SET(value,field)             (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_SR_RESERVED_0(val)                         ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int apb_int         :2 ; // APB interrupt status bitfield : - 1'b0: No interruption has been requested - 1'b1: Interruption requested
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_sr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_sr : public vp::reg_32
        {
        public:
            inline void apb_int_set(uint32_t value);
            inline uint32_t apb_int_get();
            inline void reserved_0_set(uint32_t value);
            inline uint32_t reserved_0_get();
        };

|

.. _rtc_APB_CR:

APB_CR
""""""

RTC APB control register.

.. table:: 

    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                               Description                                               |
    +=====+===+==========+=========================================================================================================+
    |5:0  |R/W|APB_ADDR  |Indirect access address configuration.                                                                   |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |15:6 |R/W|RESERVED_0|Reserved/Not used.                                                                                       |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |16   |R/W|APB_OP    |Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation|
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+
    |31:17|R/W|RESERVED_1|Reserved/Not used.                                                                                       |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB control register.
                #define RTC_APB_CR_OFFSET                        0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_cr_get(uint32_t base);
        static inline void rtc_apb_cr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Indirect access address configuration. (access: R/W)
        #define RTC_APB_CR_APB_ADDR_BIT                                      0
        #define RTC_APB_CR_APB_ADDR_WIDTH                                    6
        #define RTC_APB_CR_APB_ADDR_MASK                                     0x3f
        #define RTC_APB_CR_APB_ADDR_RESET                                    0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_CR_RESERVED_0_BIT                                    6
        #define RTC_APB_CR_RESERVED_0_WIDTH                                  10
        #define RTC_APB_CR_RESERVED_0_MASK                                   0xffc0
        #define RTC_APB_CR_RESERVED_0_RESET                                  0x0
        
        // Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation (access: R/W)
        #define RTC_APB_CR_APB_OP_BIT                                        16
        #define RTC_APB_CR_APB_OP_WIDTH                                      1
        #define RTC_APB_CR_APB_OP_MASK                                       0x10000
        #define RTC_APB_CR_APB_OP_RESET                                      0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_CR_RESERVED_1_BIT                                    17
        #define RTC_APB_CR_RESERVED_1_WIDTH                                  15
        #define RTC_APB_CR_RESERVED_1_MASK                                   0xfffe0000
        #define RTC_APB_CR_RESERVED_1_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_CR_APB_ADDR_GET(value)                     (GAP_BEXTRACTU((value),6,0))
        #define RTC_APB_CR_APB_ADDR_GETS(value)                    (GAP_BEXTRACT((value),6,0))
        #define RTC_APB_CR_APB_ADDR_SET(value,field)               (GAP_BINSERT((value),(field),6,0))
        #define RTC_APB_CR_APB_ADDR(val)                           ((val) << 0)
        
        #define RTC_APB_CR_RESERVED_0_GET(value)                   (GAP_BEXTRACTU((value),10,6))
        #define RTC_APB_CR_RESERVED_0_GETS(value)                  (GAP_BEXTRACT((value),10,6))
        #define RTC_APB_CR_RESERVED_0_SET(value,field)             (GAP_BINSERT((value),(field),10,6))
        #define RTC_APB_CR_RESERVED_0(val)                         ((val) << 6)
        
        #define RTC_APB_CR_APB_OP_GET(value)                       (GAP_BEXTRACTU((value),1,16))
        #define RTC_APB_CR_APB_OP_GETS(value)                      (GAP_BEXTRACT((value),1,16))
        #define RTC_APB_CR_APB_OP_SET(value,field)                 (GAP_BINSERT((value),(field),1,16))
        #define RTC_APB_CR_APB_OP(val)                             ((val) << 16)
        
        #define RTC_APB_CR_RESERVED_1_GET(value)                   (GAP_BEXTRACTU((value),15,17))
        #define RTC_APB_CR_RESERVED_1_GETS(value)                  (GAP_BEXTRACT((value),15,17))
        #define RTC_APB_CR_RESERVED_1_SET(value,field)             (GAP_BINSERT((value),(field),15,17))
        #define RTC_APB_CR_RESERVED_1(val)                         ((val) << 17)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int apb_addr        :6 ; // Indirect access address configuration.
            unsigned int reserved_0      :10; // Reserved/Not used.
            unsigned int apb_op          :1 ; // Indirect access operation configuration bitfield : - 1'b0: APB read operation - 1'b1: APB write operation
            unsigned int reserved_1      :15; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_cr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_cr : public vp::reg_32
        {
        public:
            inline void apb_addr_set(uint32_t value);
            inline uint32_t apb_addr_get();
            inline void reserved_0_set(uint32_t value);
            inline uint32_t reserved_0_get();
            inline void apb_op_set(uint32_t value);
            inline uint32_t apb_op_get();
            inline void reserved_1_set(uint32_t value);
            inline uint32_t reserved_1_get();
        };

|

.. _rtc_APB_DR:

APB_DR
""""""

RTC APB data register.

.. table:: 

    +-----+---+--------+------------------------------+
    |Bit #|R/W|  Name  |         Description          |
    +=====+===+========+==============================+
    |31:0 |R/W|APB_DATA|Indirect access data register.|
    +-----+---+--------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB data register.
                #define RTC_APB_DR_OFFSET                        0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_dr_get(uint32_t base);
        static inline void rtc_apb_dr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Indirect access data register. (access: R/W)
        #define RTC_APB_DR_APB_DATA_BIT                                      0
        #define RTC_APB_DR_APB_DATA_WIDTH                                    32
        #define RTC_APB_DR_APB_DATA_MASK                                     0xffffffff
        #define RTC_APB_DR_APB_DATA_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_DR_APB_DATA_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define RTC_APB_DR_APB_DATA_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define RTC_APB_DR_APB_DATA_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define RTC_APB_DR_APB_DATA(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int apb_data        :32; // Indirect access data register.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_dr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_dr : public vp::reg_32
        {
        public:
            inline void apb_data_set(uint32_t value);
            inline uint32_t apb_data_get();
        };

|

.. _rtc_RESERVED:

RESERVED
""""""""

Reserved/Not used.

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

        
                // Reserved/Not used.
                #define RTC_RESERVED_OFFSET                      0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_reserved_get(uint32_t base);
        static inline void rtc_reserved_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) rtc_reserved_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_reserved : public vp::reg_32
        {
        public:
        };

|

.. _rtc_APB_ICR:

APB_ICR
"""""""

RTC APB interruption control register.

.. table:: 

    +-----+---+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                                                                                                Description                                                                                                                                 |
    +=====+===+==========+============================================================================================================================================================================================================================================================================+
    |1:0  |R/W|MODE      |APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration|
    +-----+---+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:2 |R/W|RESERVED_0|Reserved/Not used.                                                                                                                                                                                                                                                          |
    +-----+---+----------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB interruption control register.
                #define RTC_APB_ICR_OFFSET                       0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_icr_get(uint32_t base);
        static inline void rtc_apb_icr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration (access: R/W)
        #define RTC_APB_ICR_MODE_BIT                                         0
        #define RTC_APB_ICR_MODE_WIDTH                                       2
        #define RTC_APB_ICR_MODE_MASK                                        0x3
        #define RTC_APB_ICR_MODE_RESET                                       0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_ICR_RESERVED_0_BIT                                   2
        #define RTC_APB_ICR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_ICR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_ICR_RESERVED_0_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_ICR_MODE_GET(value)                        (GAP_BEXTRACTU((value),2,0))
        #define RTC_APB_ICR_MODE_GETS(value)                       (GAP_BEXTRACT((value),2,0))
        #define RTC_APB_ICR_MODE_SET(value,field)                  (GAP_BINSERT((value),(field),2,0))
        #define RTC_APB_ICR_MODE(val)                              ((val) << 0)
        
        #define RTC_APB_ICR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_ICR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_ICR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_ICR_RESERVED_0(val)                        ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mode            :2 ; // APB interrupt signal mode configuration bitfield : - 2'b00: APB interrupt is a high level signal - 2'b01: APB interrupt is a low level signal - 2'b10: APB interrupt is a high level pulse of 1 PCLK duration - 2'b11: APB interrupt is a low level pulse of 1 PCLK duration
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_icr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_icr : public vp::reg_32
        {
        public:
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void reserved_0_set(uint32_t value);
            inline uint32_t reserved_0_get();
        };

|

.. _rtc_APB_IMR:

APB_IMR
"""""""

RTC APB interruption mask register.

.. table:: 

    +-----+---+----------+---------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                   Description                                   |
    +=====+===+==========+=================================================================================+
    |    0|R/W|READ_MASK |APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled |
    +-----+---+----------+---------------------------------------------------------------------------------+
    |    1|R/W|WRITE_MASK|APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled|
    +-----+---+----------+---------------------------------------------------------------------------------+
    |31:2 |R/W|RESERVED_0|Reserved/Not used.                                                               |
    +-----+---+----------+---------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB interruption mask register.
                #define RTC_APB_IMR_OFFSET                       0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_imr_get(uint32_t base);
        static inline void rtc_apb_imr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
        #define RTC_APB_IMR_READ_MASK_BIT                                    0
        #define RTC_APB_IMR_READ_MASK_WIDTH                                  1
        #define RTC_APB_IMR_READ_MASK_MASK                                   0x1
        #define RTC_APB_IMR_READ_MASK_RESET                                  0x0
        
        // APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled (access: R/W)
        #define RTC_APB_IMR_WRITE_MASK_BIT                                   1
        #define RTC_APB_IMR_WRITE_MASK_WIDTH                                 1
        #define RTC_APB_IMR_WRITE_MASK_MASK                                  0x2
        #define RTC_APB_IMR_WRITE_MASK_RESET                                 0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_IMR_RESERVED_0_BIT                                   2
        #define RTC_APB_IMR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_IMR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_IMR_RESERVED_0_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_IMR_READ_MASK_GET(value)                   (GAP_BEXTRACTU((value),1,0))
        #define RTC_APB_IMR_READ_MASK_GETS(value)                  (GAP_BEXTRACT((value),1,0))
        #define RTC_APB_IMR_READ_MASK_SET(value,field)             (GAP_BINSERT((value),(field),1,0))
        #define RTC_APB_IMR_READ_MASK(val)                         ((val) << 0)
        
        #define RTC_APB_IMR_WRITE_MASK_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define RTC_APB_IMR_WRITE_MASK_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define RTC_APB_IMR_WRITE_MASK_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define RTC_APB_IMR_WRITE_MASK(val)                        ((val) << 1)
        
        #define RTC_APB_IMR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_IMR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_IMR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_IMR_RESERVED_0(val)                        ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int read_mask       :1 ; // APB read operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
            unsigned int write_mask      :1 ; // APB write operation interruption mask bitfield : - 1'b0: enabled - 1'b1: disabled
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_imr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_imr : public vp::reg_32
        {
        public:
            inline void read_mask_set(uint32_t value);
            inline uint32_t read_mask_get();
            inline void write_mask_set(uint32_t value);
            inline uint32_t write_mask_get();
            inline void reserved_0_set(uint32_t value);
            inline uint32_t reserved_0_get();
        };

|

.. _rtc_APB_IFR:

APB_IFR
"""""""

RTC APB interruption flag register.

.. table:: 

    +-----+---+----------+------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                         Description                                                          |
    +=====+===+==========+==============================================================================================================================+
    |    0|R/W|READ_FLAG |APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available|
    +-----+---+----------+------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|WRITE_FLAG|APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done                                       |
    +-----+---+----------+------------------------------------------------------------------------------------------------------------------------------+
    |31:2 |R/W|RESERVED_0|Reserved/Not used.                                                                                                            |
    +-----+---+----------+------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RTC APB interruption flag register.
                #define RTC_APB_IFR_OFFSET                       0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t rtc_apb_ifr_get(uint32_t base);
        static inline void rtc_apb_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available (access: R/W)
        #define RTC_APB_IFR_READ_FLAG_BIT                                    0
        #define RTC_APB_IFR_READ_FLAG_WIDTH                                  1
        #define RTC_APB_IFR_READ_FLAG_MASK                                   0x1
        #define RTC_APB_IFR_READ_FLAG_RESET                                  0x0
        
        // APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done (access: R/W)
        #define RTC_APB_IFR_WRITE_FLAG_BIT                                   1
        #define RTC_APB_IFR_WRITE_FLAG_WIDTH                                 1
        #define RTC_APB_IFR_WRITE_FLAG_MASK                                  0x2
        #define RTC_APB_IFR_WRITE_FLAG_RESET                                 0x0
        
        // Reserved/Not used. (access: R/W)
        #define RTC_APB_IFR_RESERVED_0_BIT                                   2
        #define RTC_APB_IFR_RESERVED_0_WIDTH                                 30
        #define RTC_APB_IFR_RESERVED_0_MASK                                  0xfffffffc
        #define RTC_APB_IFR_RESERVED_0_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define RTC_APB_IFR_READ_FLAG_GET(value)                   (GAP_BEXTRACTU((value),1,0))
        #define RTC_APB_IFR_READ_FLAG_GETS(value)                  (GAP_BEXTRACT((value),1,0))
        #define RTC_APB_IFR_READ_FLAG_SET(value,field)             (GAP_BINSERT((value),(field),1,0))
        #define RTC_APB_IFR_READ_FLAG(val)                         ((val) << 0)
        
        #define RTC_APB_IFR_WRITE_FLAG_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define RTC_APB_IFR_WRITE_FLAG_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define RTC_APB_IFR_WRITE_FLAG_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define RTC_APB_IFR_WRITE_FLAG(val)                        ((val) << 1)
        
        #define RTC_APB_IFR_RESERVED_0_GET(value)                  (GAP_BEXTRACTU((value),30,2))
        #define RTC_APB_IFR_RESERVED_0_GETS(value)                 (GAP_BEXTRACT((value),30,2))
        #define RTC_APB_IFR_RESERVED_0_SET(value,field)            (GAP_BINSERT((value),(field),30,2))
        #define RTC_APB_IFR_RESERVED_0(val)                        ((val) << 2)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int read_flag       :1 ; // APB read operation status flag bitfield : - 1'b0: nothing - 1'b1: read operation done and requested indirect data is available
            unsigned int write_flag      :1 ; // APB write operation status flag bitfield : - 1'b0: nothing - 1'b1: write operation done
            unsigned int reserved_0      :30; // Reserved/Not used.
          };
          unsigned int raw;
        } __attribute__((packed)) rtc_apb_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_rtc_apb_ifr : public vp::reg_32
        {
        public:
            inline void read_flag_set(uint32_t value);
            inline uint32_t read_flag_get();
            inline void write_flag_set(uint32_t value);
            inline uint32_t write_flag_get();
            inline void reserved_0_set(uint32_t value);
            inline uint32_t reserved_0_get();
        };

|
