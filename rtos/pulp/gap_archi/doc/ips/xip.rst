Input file: fe/ips/XIP/doc/XIP_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |                      Name                       |Offset|Width|                    Description                     |
    +=================================================+======+=====+====================================================+
    |:ref:`XIP_CFG_VIRT_ADDR0<xip_XIP_CFG_VIRT_ADDR0>`|    16|   32|Start of Virtual Address for external peripheral 0  |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_VIRT_ADDR1<xip_XIP_CFG_VIRT_ADDR1>`|    20|   32|Start of Virtual Address for external peripheral 1  |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_VIRT_ADDR2<xip_XIP_CFG_VIRT_ADDR2>`|    24|   32|Start of Virtual Address for external peripheral 2  |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_EXT_ADDR0<xip_XIP_CFG_EXT_ADDR0>`  |    28|   32|Start of External Address for external peripheral 0 |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_EXT_ADDR1<xip_XIP_CFG_EXT_ADDR1>`  |    32|   32|Start of External Address for external peripheral 1 |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_EXT_ADDR2<xip_XIP_CFG_EXT_ADDR2>`  |    36|   32|Start of External Address for external peripheral 2 |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_MNT_SIZE0<xip_XIP_CFG_MNT_SIZE0>`  |    40|   12|Size of the mounted region in pages for peripheral 0|
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_MNT_SIZE1<xip_XIP_CFG_MNT_SIZE1>`  |    44|   12|Size of the mounted region in pages for peripheral 1|
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_MNT_SIZE2<xip_XIP_CFG_MNT_SIZE2>`  |    48|   12|Size of the mounted region in pages for peripheral 2|
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE_SIZE0<xip_XIP_CFG_PAGE_SIZE0>`|    52|    3|Size of pages for peripheral 0                      |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE_SIZE1<xip_XIP_CFG_PAGE_SIZE1>`|    56|    3|Size of pages for peripheral 1                      |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE_SIZE2<xip_XIP_CFG_PAGE_SIZE2>`|    60|    3|Size of pages for peripheral 2                      |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE0<xip_XIP_CFG_PAGE0>`          |    64|   32|Configuration of page0                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE1<xip_XIP_CFG_PAGE1>`          |    68|   32|Configuration of page1                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE2<xip_XIP_CFG_PAGE2>`          |    72|   32|Configuration of page2                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE3<xip_XIP_CFG_PAGE3>`          |    76|   32|Configuration of page3                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE4<xip_XIP_CFG_PAGE4>`          |    80|   32|Configuration of page4                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE5<xip_XIP_CFG_PAGE5>`          |    84|   32|Configuration of page5                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE6<xip_XIP_CFG_PAGE6>`          |    88|   32|Configuration of page6                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE7<xip_XIP_CFG_PAGE7>`          |    92|   32|Configuration of page7                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE8<xip_XIP_CFG_PAGE8>`          |    96|   32|Configuration of page8                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE9<xip_XIP_CFG_PAGE9>`          |   100|   32|Configuration of page9                              |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE10<xip_XIP_CFG_PAGE10>`        |   104|   32|Configuration of page10                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE11<xip_XIP_CFG_PAGE11>`        |   108|   32|Configuration of page11                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE12<xip_XIP_CFG_PAGE12>`        |   112|   32|Configuration of page12                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE13<xip_XIP_CFG_PAGE13>`        |   116|   32|Configuration of page13                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE14<xip_XIP_CFG_PAGE14>`        |   120|   32|Configuration of page14                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+
    |:ref:`XIP_CFG_PAGE15<xip_XIP_CFG_PAGE15>`        |   124|   32|Configuration of page15                             |
    +-------------------------------------------------+------+-----+----------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of Virtual Address for external peripheral 0
                #define XIP_XIP_CFG_VIRT_ADDR0_OFFSET            0x10
        
                // Start of Virtual Address for external peripheral 1
                #define XIP_XIP_CFG_VIRT_ADDR1_OFFSET            0x14
        
                // Start of Virtual Address for external peripheral 2
                #define XIP_XIP_CFG_VIRT_ADDR2_OFFSET            0x18
        
                // Start of External Address for external peripheral 0
                #define XIP_XIP_CFG_EXT_ADDR0_OFFSET             0x1c
        
                // Start of External Address for external peripheral 1
                #define XIP_XIP_CFG_EXT_ADDR1_OFFSET             0x20
        
                // Start of External Address for external peripheral 2
                #define XIP_XIP_CFG_EXT_ADDR2_OFFSET             0x24
        
                // Size of the mounted region in pages for peripheral 0
                #define XIP_XIP_CFG_MNT_SIZE0_OFFSET             0x28
        
                // Size of the mounted region in pages for peripheral 1
                #define XIP_XIP_CFG_MNT_SIZE1_OFFSET             0x2c
        
                // Size of the mounted region in pages for peripheral 2
                #define XIP_XIP_CFG_MNT_SIZE2_OFFSET             0x30
        
                // Size of pages for peripheral 0
                #define XIP_XIP_CFG_PAGE_SIZE0_OFFSET            0x34
        
                // Size of pages for peripheral 1
                #define XIP_XIP_CFG_PAGE_SIZE1_OFFSET            0x38
        
                // Size of pages for peripheral 2
                #define XIP_XIP_CFG_PAGE_SIZE2_OFFSET            0x3c
        
                // Configuration of page0
                #define XIP_XIP_CFG_PAGE0_OFFSET                 0x40
        
                // Configuration of page1
                #define XIP_XIP_CFG_PAGE1_OFFSET                 0x44
        
                // Configuration of page2
                #define XIP_XIP_CFG_PAGE2_OFFSET                 0x48
        
                // Configuration of page3
                #define XIP_XIP_CFG_PAGE3_OFFSET                 0x4c
        
                // Configuration of page4
                #define XIP_XIP_CFG_PAGE4_OFFSET                 0x50
        
                // Configuration of page5
                #define XIP_XIP_CFG_PAGE5_OFFSET                 0x54
        
                // Configuration of page6
                #define XIP_XIP_CFG_PAGE6_OFFSET                 0x58
        
                // Configuration of page7
                #define XIP_XIP_CFG_PAGE7_OFFSET                 0x5c
        
                // Configuration of page8
                #define XIP_XIP_CFG_PAGE8_OFFSET                 0x60
        
                // Configuration of page9
                #define XIP_XIP_CFG_PAGE9_OFFSET                 0x64
        
                // Configuration of page10
                #define XIP_XIP_CFG_PAGE10_OFFSET                0x68
        
                // Configuration of page11
                #define XIP_XIP_CFG_PAGE11_OFFSET                0x6c
        
                // Configuration of page12
                #define XIP_XIP_CFG_PAGE12_OFFSET                0x70
        
                // Configuration of page13
                #define XIP_XIP_CFG_PAGE13_OFFSET                0x74
        
                // Configuration of page14
                #define XIP_XIP_CFG_PAGE14_OFFSET                0x78
        
                // Configuration of page15
                #define XIP_XIP_CFG_PAGE15_OFFSET                0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_virt_addr0_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_virt_addr1_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_virt_addr2_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_ext_addr0_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_ext_addr1_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_ext_addr2_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_mnt_size0_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_mnt_size1_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_mnt_size2_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page_size0_get(uint32_t base);
        static inline void xip_xip_cfg_page_size0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page_size1_get(uint32_t base);
        static inline void xip_xip_cfg_page_size1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page_size2_get(uint32_t base);
        static inline void xip_xip_cfg_page_size2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page0_get(uint32_t base);
        static inline void xip_xip_cfg_page0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page1_get(uint32_t base);
        static inline void xip_xip_cfg_page1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page2_get(uint32_t base);
        static inline void xip_xip_cfg_page2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page3_get(uint32_t base);
        static inline void xip_xip_cfg_page3_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page4_get(uint32_t base);
        static inline void xip_xip_cfg_page4_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page5_get(uint32_t base);
        static inline void xip_xip_cfg_page5_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page6_get(uint32_t base);
        static inline void xip_xip_cfg_page6_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page7_get(uint32_t base);
        static inline void xip_xip_cfg_page7_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page8_get(uint32_t base);
        static inline void xip_xip_cfg_page8_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page9_get(uint32_t base);
        static inline void xip_xip_cfg_page9_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page10_get(uint32_t base);
        static inline void xip_xip_cfg_page10_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page11_get(uint32_t base);
        static inline void xip_xip_cfg_page11_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page12_get(uint32_t base);
        static inline void xip_xip_cfg_page12_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page13_get(uint32_t base);
        static inline void xip_xip_cfg_page13_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page14_get(uint32_t base);
        static inline void xip_xip_cfg_page14_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_xip_cfg_page15_get(uint32_t base);
        static inline void xip_xip_cfg_page15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_RESET                      0x20000000
        
        // Start of Virtual Address for external peripheral 1 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_RESET                      0x20000000
        
        // Start of Virtual Address for external peripheral 2 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_RESET                      0x20000000
        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_RESET                        0x0
        
        // Start of External Address for external peripheral 1 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_RESET                        0x0
        
        // Start of External Address for external peripheral 2 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_RESET                        0x0
        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_RESET                        0x0
        
        // Size of the mounted region in pages for peripheral 1 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_RESET                        0x0
        
        // Size of the mounted region in pages for peripheral 2 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_RESET                        0x0
        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_RESET                      0x0
        
        // Size of pages for peripheral 1 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_RESET                      0x0
        
        // Size of pages for peripheral 2 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_RESET                      0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE0_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE0_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE0_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE0_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE1_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE1_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE1_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE1_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE2_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE2_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE2_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE2_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE3_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE3_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE3_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE3_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE4_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE4_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE4_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE4_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE5_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE5_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE5_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE5_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE6_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE6_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE6_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE6_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE7_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE7_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE7_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE7_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE8_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE8_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE8_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE8_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE9_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE9_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE9_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE9_PER_ID_RESET                               0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE10_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE10_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE10_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE10_PER_ID_RESET                              0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE11_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE11_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE11_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE11_PER_ID_RESET                              0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE12_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE12_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE12_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE12_PER_ID_RESET                              0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE13_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE13_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE13_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE13_PER_ID_RESET                              0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE14_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE14_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE14_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE14_PER_ID_RESET                              0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE15_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE15_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE15_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE15_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2(val)               ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2(val)             ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE0_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE1_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE2_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE3_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE4_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE5_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE6_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE7_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE8_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE9_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID(val)                      ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE10_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID(val)                     ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE11_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID(val)                     ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE12_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID(val)                     ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE13_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID(val)                     ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE14_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID(val)                     ((val) << 30)
        
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE15_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** XIP_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t reserved_0[3];  // Reserved/Not used.
            volatile uint32_t xip_cfg_virt_addr0;  // Start of Virtual Address for external peripheral 0
            volatile uint32_t xip_cfg_virt_addr1;  // Start of Virtual Address for external peripheral 1
            volatile uint32_t xip_cfg_virt_addr2;  // Start of Virtual Address for external peripheral 2
            volatile uint32_t xip_cfg_ext_addr0;  // Start of External Address for external peripheral 0
            volatile uint32_t xip_cfg_ext_addr1;  // Start of External Address for external peripheral 1
            volatile uint32_t xip_cfg_ext_addr2;  // Start of External Address for external peripheral 2
            volatile uint32_t xip_cfg_mnt_size0;  // Size of the mounted region in pages for peripheral 0
            volatile uint32_t xip_cfg_mnt_size1;  // Size of the mounted region in pages for peripheral 1
            volatile uint32_t xip_cfg_mnt_size2;  // Size of the mounted region in pages for peripheral 2
            volatile uint32_t xip_cfg_page_size0;  // Size of pages for peripheral 0
            volatile uint32_t xip_cfg_page_size1;  // Size of pages for peripheral 1
            volatile uint32_t xip_cfg_page_size2;  // Size of pages for peripheral 2
            volatile uint32_t xip_cfg_page0;  // Configuration of page0
            volatile uint32_t xip_cfg_page1;  // Configuration of page1
            volatile uint32_t xip_cfg_page2;  // Configuration of page2
            volatile uint32_t xip_cfg_page3;  // Configuration of page3
            volatile uint32_t xip_cfg_page4;  // Configuration of page4
            volatile uint32_t xip_cfg_page5;  // Configuration of page5
            volatile uint32_t xip_cfg_page6;  // Configuration of page6
            volatile uint32_t xip_cfg_page7;  // Configuration of page7
            volatile uint32_t xip_cfg_page8;  // Configuration of page8
            volatile uint32_t xip_cfg_page9;  // Configuration of page9
            volatile uint32_t xip_cfg_page10;  // Configuration of page10
            volatile uint32_t xip_cfg_page11;  // Configuration of page11
            volatile uint32_t xip_cfg_page12;  // Configuration of page12
            volatile uint32_t xip_cfg_page13;  // Configuration of page13
            volatile uint32_t xip_cfg_page14;  // Configuration of page14
            volatile uint32_t xip_cfg_page15;  // Configuration of page15
        } __attribute__((packed)) xip_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr0      :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr0_t;
        
        typedef union {
          struct {
            unsigned int virt_addr1      :32; // Start of Virtual Address for external peripheral 1 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr1_t;
        
        typedef union {
          struct {
            unsigned int virt_addr2      :32; // Start of Virtual Address for external peripheral 2 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr2_t;
        
        typedef union {
          struct {
            unsigned int ext_addr0       :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr0_t;
        
        typedef union {
          struct {
            unsigned int ext_addr1       :32; // Start of External Address for external peripheral 1
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr1_t;
        
        typedef union {
          struct {
            unsigned int ext_addr2       :32; // Start of External Address for external peripheral 2
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr2_t;
        
        typedef union {
          struct {
            unsigned int mnt_size0       :12; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size0_t;
        
        typedef union {
          struct {
            unsigned int mnt_size1       :12; // Size of the mounted region in pages for peripheral 1 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size1_t;
        
        typedef union {
          struct {
            unsigned int mnt_size2       :12; // Size of the mounted region in pages for peripheral 2 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size2_t;
        
        typedef union {
          struct {
            unsigned int page_size0      :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size0_t;
        
        typedef union {
          struct {
            unsigned int page_size1      :3 ; // Size of pages for peripheral 1 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size1_t;
        
        typedef union {
          struct {
            unsigned int page_size2      :3 ; // Size of pages for peripheral 2 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size2_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page0_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page1_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page2_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page3_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page4_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page5_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page6_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page7_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page8_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page9_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page10_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page11_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page12_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page13_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page14_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_xip : public vp::regmap
        {
        public:
            vp_xip_xip_cfg_virt_addr0 xip_cfg_virt_addr0;
            vp_xip_xip_cfg_virt_addr1 xip_cfg_virt_addr1;
            vp_xip_xip_cfg_virt_addr2 xip_cfg_virt_addr2;
            vp_xip_xip_cfg_ext_addr0 xip_cfg_ext_addr0;
            vp_xip_xip_cfg_ext_addr1 xip_cfg_ext_addr1;
            vp_xip_xip_cfg_ext_addr2 xip_cfg_ext_addr2;
            vp_xip_xip_cfg_mnt_size0 xip_cfg_mnt_size0;
            vp_xip_xip_cfg_mnt_size1 xip_cfg_mnt_size1;
            vp_xip_xip_cfg_mnt_size2 xip_cfg_mnt_size2;
            vp_xip_xip_cfg_page_size0 xip_cfg_page_size0;
            vp_xip_xip_cfg_page_size1 xip_cfg_page_size1;
            vp_xip_xip_cfg_page_size2 xip_cfg_page_size2;
            vp_xip_xip_cfg_page0 xip_cfg_page0;
            vp_xip_xip_cfg_page1 xip_cfg_page1;
            vp_xip_xip_cfg_page2 xip_cfg_page2;
            vp_xip_xip_cfg_page3 xip_cfg_page3;
            vp_xip_xip_cfg_page4 xip_cfg_page4;
            vp_xip_xip_cfg_page5 xip_cfg_page5;
            vp_xip_xip_cfg_page6 xip_cfg_page6;
            vp_xip_xip_cfg_page7 xip_cfg_page7;
            vp_xip_xip_cfg_page8 xip_cfg_page8;
            vp_xip_xip_cfg_page9 xip_cfg_page9;
            vp_xip_xip_cfg_page10 xip_cfg_page10;
            vp_xip_xip_cfg_page11 xip_cfg_page11;
            vp_xip_xip_cfg_page12 xip_cfg_page12;
            vp_xip_xip_cfg_page13 xip_cfg_page13;
            vp_xip_xip_cfg_page14 xip_cfg_page14;
            vp_xip_xip_cfg_page15 xip_cfg_page15;
        };

|

.. _xip_XIP_CFG_VIRT_ADDR0:

XIP_CFG_VIRT_ADDR0
""""""""""""""""""

Start of Virtual Address for external peripheral 0

.. table:: 

    +-----+---+----------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                          Description                                           |
    +=====+===+==========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR0|Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+----------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of Virtual Address for external peripheral 0
                #define XIP_XIP_CFG_VIRT_ADDR0_OFFSET            0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_virt_addr0_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_RESET                      0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR0_VIRT_ADDR0(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr0      :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_virt_addr0 : public vp::reg_32
        {
        public:
            inline void virt_addr0_set(uint32_t value);
            inline uint32_t virt_addr0_get();
        };

|

.. _xip_XIP_CFG_VIRT_ADDR1:

XIP_CFG_VIRT_ADDR1
""""""""""""""""""

Start of Virtual Address for external peripheral 1

.. table:: 

    +-----+---+----------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                          Description                                           |
    +=====+===+==========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR1|Start of Virtual Address for external peripheral 1 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+----------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of Virtual Address for external peripheral 1
                #define XIP_XIP_CFG_VIRT_ADDR1_OFFSET            0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_virt_addr1_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 1 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_RESET                      0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR1_VIRT_ADDR1(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr1      :32; // Start of Virtual Address for external peripheral 1 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_virt_addr1 : public vp::reg_32
        {
        public:
            inline void virt_addr1_set(uint32_t value);
            inline uint32_t virt_addr1_get();
        };

|

.. _xip_XIP_CFG_VIRT_ADDR2:

XIP_CFG_VIRT_ADDR2
""""""""""""""""""

Start of Virtual Address for external peripheral 2

.. table:: 

    +-----+---+----------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                          Description                                           |
    +=====+===+==========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR2|Start of Virtual Address for external peripheral 2 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+----------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of Virtual Address for external peripheral 2
                #define XIP_XIP_CFG_VIRT_ADDR2_OFFSET            0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_virt_addr2_get(uint32_t base);
        static inline void xip_xip_cfg_virt_addr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 2 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_BIT                        0
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_WIDTH                      32
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_MASK                       0xffffffff
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_RESET                      0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_VIRT_ADDR2_VIRT_ADDR2(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr2      :32; // Start of Virtual Address for external peripheral 2 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_virt_addr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_virt_addr2 : public vp::reg_32
        {
        public:
            inline void virt_addr2_set(uint32_t value);
            inline uint32_t virt_addr2_get();
        };

|

.. _xip_XIP_CFG_EXT_ADDR0:

XIP_CFG_EXT_ADDR0
"""""""""""""""""

Start of External Address for external peripheral 0

.. table:: 

    +-----+---+---------+---------------------------------------------------+
    |Bit #|R/W|  Name   |                    Description                    |
    +=====+===+=========+===================================================+
    |31:0 |R/W|EXT_ADDR0|Start of External Address for external peripheral 0|
    +-----+---+---------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of External Address for external peripheral 0
                #define XIP_XIP_CFG_EXT_ADDR0_OFFSET             0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_ext_addr0_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR0_EXT_ADDR0(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr0       :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_ext_addr0 : public vp::reg_32
        {
        public:
            inline void ext_addr0_set(uint32_t value);
            inline uint32_t ext_addr0_get();
        };

|

.. _xip_XIP_CFG_EXT_ADDR1:

XIP_CFG_EXT_ADDR1
"""""""""""""""""

Start of External Address for external peripheral 1

.. table:: 

    +-----+---+---------+---------------------------------------------------+
    |Bit #|R/W|  Name   |                    Description                    |
    +=====+===+=========+===================================================+
    |31:0 |R/W|EXT_ADDR1|Start of External Address for external peripheral 1|
    +-----+---+---------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of External Address for external peripheral 1
                #define XIP_XIP_CFG_EXT_ADDR1_OFFSET             0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_ext_addr1_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 1 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR1_EXT_ADDR1(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr1       :32; // Start of External Address for external peripheral 1
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_ext_addr1 : public vp::reg_32
        {
        public:
            inline void ext_addr1_set(uint32_t value);
            inline uint32_t ext_addr1_get();
        };

|

.. _xip_XIP_CFG_EXT_ADDR2:

XIP_CFG_EXT_ADDR2
"""""""""""""""""

Start of External Address for external peripheral 2

.. table:: 

    +-----+---+---------+---------------------------------------------------+
    |Bit #|R/W|  Name   |                    Description                    |
    +=====+===+=========+===================================================+
    |31:0 |R/W|EXT_ADDR2|Start of External Address for external peripheral 2|
    +-----+---+---------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Start of External Address for external peripheral 2
                #define XIP_XIP_CFG_EXT_ADDR2_OFFSET             0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_ext_addr2_get(uint32_t base);
        static inline void xip_xip_cfg_ext_addr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 2 (access: R/W)
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_BIT                          0
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_WIDTH                        32
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_MASK                         0xffffffff
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define XIP_XIP_CFG_EXT_ADDR2_EXT_ADDR2(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr2       :32; // Start of External Address for external peripheral 2
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_ext_addr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_ext_addr2 : public vp::reg_32
        {
        public:
            inline void ext_addr2_set(uint32_t value);
            inline uint32_t ext_addr2_get();
        };

|

.. _xip_XIP_CFG_MNT_SIZE0:

XIP_CFG_MNT_SIZE0
"""""""""""""""""

Size of the mounted region in pages for peripheral 0

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |11:0 |R/W|MNT_SIZE0|Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of the mounted region in pages for peripheral 0
                #define XIP_XIP_CFG_MNT_SIZE0_OFFSET             0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_mnt_size0_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE0_MNT_SIZE0(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size0       :12; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_mnt_size0 : public vp::reg_16
        {
        public:
            inline void mnt_size0_set(uint16_t value);
            inline uint16_t mnt_size0_get();
        };

|

.. _xip_XIP_CFG_MNT_SIZE1:

XIP_CFG_MNT_SIZE1
"""""""""""""""""

Size of the mounted region in pages for peripheral 1

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |11:0 |R/W|MNT_SIZE1|Size of the mounted region in pages for peripheral 1 (total size of region = this reg*page size)|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of the mounted region in pages for peripheral 1
                #define XIP_XIP_CFG_MNT_SIZE1_OFFSET             0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_mnt_size1_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 1 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE1_MNT_SIZE1(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size1       :12; // Size of the mounted region in pages for peripheral 1 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_mnt_size1 : public vp::reg_16
        {
        public:
            inline void mnt_size1_set(uint16_t value);
            inline uint16_t mnt_size1_get();
        };

|

.. _xip_XIP_CFG_MNT_SIZE2:

XIP_CFG_MNT_SIZE2
"""""""""""""""""

Size of the mounted region in pages for peripheral 2

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |11:0 |R/W|MNT_SIZE2|Size of the mounted region in pages for peripheral 2 (total size of region = this reg*page size)|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of the mounted region in pages for peripheral 2
                #define XIP_XIP_CFG_MNT_SIZE2_OFFSET             0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_mnt_size2_get(uint32_t base);
        static inline void xip_xip_cfg_mnt_size2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 2 (total size of region = this reg*page size) (access: R/W)
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_BIT                          0
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_WIDTH                        12
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_MASK                         0xfff
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_GET(value)         (GAP_BEXTRACTU((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_GETS(value)        (GAP_BEXTRACT((value),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2_SET(value,field)   (GAP_BINSERT((value),(field),12,0))
        #define XIP_XIP_CFG_MNT_SIZE2_MNT_SIZE2(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size2       :12; // Size of the mounted region in pages for peripheral 2 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_mnt_size2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_mnt_size2 : public vp::reg_16
        {
        public:
            inline void mnt_size2_set(uint16_t value);
            inline uint16_t mnt_size2_get();
        };

|

.. _xip_XIP_CFG_PAGE_SIZE0:

XIP_CFG_PAGE_SIZE0
""""""""""""""""""

Size of pages for peripheral 0

.. table:: 

    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                       Description                                                        |
    +=====+===+==========+==========================================================================================================================+
    |2:0  |R/W|PAGE_SIZE0|Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes|
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of pages for peripheral 0
                #define XIP_XIP_CFG_PAGE_SIZE0_OFFSET            0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page_size0_get(uint32_t base);
        static inline void xip_xip_cfg_page_size0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE0_PAGE_SIZE0(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size0      :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page_size0 : public vp::reg_8
        {
        public:
            inline void page_size0_set(uint8_t value);
            inline uint8_t page_size0_get();
        };

|

.. _xip_XIP_CFG_PAGE_SIZE1:

XIP_CFG_PAGE_SIZE1
""""""""""""""""""

Size of pages for peripheral 1

.. table:: 

    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                       Description                                                        |
    +=====+===+==========+==========================================================================================================================+
    |2:0  |R/W|PAGE_SIZE1|Size of pages for peripheral 1 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes|
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of pages for peripheral 1
                #define XIP_XIP_CFG_PAGE_SIZE1_OFFSET            0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page_size1_get(uint32_t base);
        static inline void xip_xip_cfg_page_size1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 1 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE1_PAGE_SIZE1(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size1      :3 ; // Size of pages for peripheral 1 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page_size1 : public vp::reg_8
        {
        public:
            inline void page_size1_set(uint8_t value);
            inline uint8_t page_size1_get();
        };

|

.. _xip_XIP_CFG_PAGE_SIZE2:

XIP_CFG_PAGE_SIZE2
""""""""""""""""""

Size of pages for peripheral 2

.. table:: 

    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                       Description                                                        |
    +=====+===+==========+==========================================================================================================================+
    |2:0  |R/W|PAGE_SIZE2|Size of pages for peripheral 2 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes|
    +-----+---+----------+--------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Size of pages for peripheral 2
                #define XIP_XIP_CFG_PAGE_SIZE2_OFFSET            0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page_size2_get(uint32_t base);
        static inline void xip_xip_cfg_page_size2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 2 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes (access: R/W)
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_BIT                        0
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_WIDTH                      3
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_MASK                       0x7
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_GET(value)       (GAP_BEXTRACTU((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_GETS(value)      (GAP_BEXTRACT((value),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2_SET(value,field) (GAP_BINSERT((value),(field),3,0))
        #define XIP_XIP_CFG_PAGE_SIZE2_PAGE_SIZE2(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size2      :3 ; // Size of pages for peripheral 2 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes 4: 8KBytes 5: 16KBytes 6: 32KBytes 7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page_size2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page_size2 : public vp::reg_8
        {
        public:
            inline void page_size2_set(uint8_t value);
            inline uint8_t page_size2_get();
        };

|

.. _xip_XIP_CFG_PAGE0:

XIP_CFG_PAGE0
"""""""""""""

Configuration of page0

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page0
                #define XIP_XIP_CFG_PAGE0_OFFSET                 0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page0_get(uint32_t base);
        static inline void xip_xip_cfg_page0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE0_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE0_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE0_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE0_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE0_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE0_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE0_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page0 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE1:

XIP_CFG_PAGE1
"""""""""""""

Configuration of page1

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page1
                #define XIP_XIP_CFG_PAGE1_OFFSET                 0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page1_get(uint32_t base);
        static inline void xip_xip_cfg_page1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE1_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE1_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE1_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE1_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE1_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE1_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE1_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page1 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE2:

XIP_CFG_PAGE2
"""""""""""""

Configuration of page2

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page2
                #define XIP_XIP_CFG_PAGE2_OFFSET                 0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page2_get(uint32_t base);
        static inline void xip_xip_cfg_page2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE2_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE2_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE2_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE2_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE2_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE2_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE2_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page2 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE3:

XIP_CFG_PAGE3
"""""""""""""

Configuration of page3

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page3
                #define XIP_XIP_CFG_PAGE3_OFFSET                 0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page3_get(uint32_t base);
        static inline void xip_xip_cfg_page3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE3_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE3_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE3_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE3_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE3_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE3_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE3_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page3 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE4:

XIP_CFG_PAGE4
"""""""""""""

Configuration of page4

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page4
                #define XIP_XIP_CFG_PAGE4_OFFSET                 0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page4_get(uint32_t base);
        static inline void xip_xip_cfg_page4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE4_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE4_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE4_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE4_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE4_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE4_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE4_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page4 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE5:

XIP_CFG_PAGE5
"""""""""""""

Configuration of page5

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page5
                #define XIP_XIP_CFG_PAGE5_OFFSET                 0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page5_get(uint32_t base);
        static inline void xip_xip_cfg_page5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE5_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE5_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE5_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE5_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE5_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE5_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE5_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page5 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE6:

XIP_CFG_PAGE6
"""""""""""""

Configuration of page6

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page6
                #define XIP_XIP_CFG_PAGE6_OFFSET                 0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page6_get(uint32_t base);
        static inline void xip_xip_cfg_page6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE6_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE6_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE6_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE6_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE6_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE6_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE6_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page6 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE7:

XIP_CFG_PAGE7
"""""""""""""

Configuration of page7

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page7
                #define XIP_XIP_CFG_PAGE7_OFFSET                 0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page7_get(uint32_t base);
        static inline void xip_xip_cfg_page7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE7_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE7_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE7_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE7_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE7_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE7_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE7_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page7 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE8:

XIP_CFG_PAGE8
"""""""""""""

Configuration of page8

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page8
                #define XIP_XIP_CFG_PAGE8_OFFSET                 0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page8_get(uint32_t base);
        static inline void xip_xip_cfg_page8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE8_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE8_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE8_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE8_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE8_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE8_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE8_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page8 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE9:

XIP_CFG_PAGE9
"""""""""""""

Configuration of page9

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page9
                #define XIP_XIP_CFG_PAGE9_OFFSET                 0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page9_get(uint32_t base);
        static inline void xip_xip_cfg_page9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_BIT                               0
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_WIDTH                             21
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_MASK                              0x1fffff
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_RESET                             0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE9_PER_ID_BIT                                 30
        #define XIP_XIP_CFG_PAGE9_PER_ID_WIDTH                               2
        #define XIP_XIP_CFG_PAGE9_PER_ID_MASK                                0xc0000000
        #define XIP_XIP_CFG_PAGE9_PER_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_GET(value)              (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_GETS(value)             (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE9_INT_ADDR(val)                    ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE9_PER_ID_GET(value)                (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID_GETS(value)               (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID_SET(value,field)          (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE9_PER_ID(val)                      ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page9 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE10:

XIP_CFG_PAGE10
""""""""""""""

Configuration of page10

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page10
                #define XIP_XIP_CFG_PAGE10_OFFSET                0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page10_get(uint32_t base);
        static inline void xip_xip_cfg_page10_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE10_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE10_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE10_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE10_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE10_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE10_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE10_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page10_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page10 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE11:

XIP_CFG_PAGE11
""""""""""""""

Configuration of page11

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page11
                #define XIP_XIP_CFG_PAGE11_OFFSET                0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page11_get(uint32_t base);
        static inline void xip_xip_cfg_page11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE11_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE11_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE11_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE11_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE11_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE11_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE11_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page11 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE12:

XIP_CFG_PAGE12
""""""""""""""

Configuration of page12

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page12
                #define XIP_XIP_CFG_PAGE12_OFFSET                0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page12_get(uint32_t base);
        static inline void xip_xip_cfg_page12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE12_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE12_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE12_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE12_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE12_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE12_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE12_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page12 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE13:

XIP_CFG_PAGE13
""""""""""""""

Configuration of page13

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page13
                #define XIP_XIP_CFG_PAGE13_OFFSET                0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page13_get(uint32_t base);
        static inline void xip_xip_cfg_page13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE13_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE13_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE13_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE13_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE13_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE13_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE13_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page13 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE14:

XIP_CFG_PAGE14
""""""""""""""

Configuration of page14

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page14
                #define XIP_XIP_CFG_PAGE14_OFFSET                0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page14_get(uint32_t base);
        static inline void xip_xip_cfg_page14_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE14_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE14_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE14_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE14_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE14_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE14_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE14_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page14_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page14 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_XIP_CFG_PAGE15:

XIP_CFG_PAGE15
""""""""""""""

Configuration of page15

.. table:: 

    +-----+---+--------+--------------------------------+
    |Bit #|R/W|  Name  |          Description           |
    +=====+===+========+================================+
    |20:0 |R/W|INT_ADDR|21 LSB of L2 Address of the page|
    +-----+---+--------+--------------------------------+
    |31:30|R/W|PER_ID  |Peripheral ID                   |
    +-----+---+--------+--------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Configuration of page15
                #define XIP_XIP_CFG_PAGE15_OFFSET                0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_xip_cfg_page15_get(uint32_t base);
        static inline void xip_xip_cfg_page15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_BIT                              0
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_WIDTH                            21
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_MASK                             0x1fffff
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_RESET                            0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_XIP_CFG_PAGE15_PER_ID_BIT                                30
        #define XIP_XIP_CFG_PAGE15_PER_ID_WIDTH                              2
        #define XIP_XIP_CFG_PAGE15_PER_ID_MASK                               0xc0000000
        #define XIP_XIP_CFG_PAGE15_PER_ID_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_GET(value)             (GAP_BEXTRACTU((value),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_GETS(value)            (GAP_BEXTRACT((value),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR_SET(value,field)       (GAP_BINSERT((value),(field),21,0))
        #define XIP_XIP_CFG_PAGE15_INT_ADDR(val)                   ((val) << 0)
        
        #define XIP_XIP_CFG_PAGE15_PER_ID_GET(value)               (GAP_BEXTRACTU((value),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID_GETS(value)              (GAP_BEXTRACT((value),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID_SET(value,field)         (GAP_BINSERT((value),(field),2,30))
        #define XIP_XIP_CFG_PAGE15_PER_ID(val)                     ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:9 ;
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_xip_cfg_page15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_xip_cfg_page15 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|
