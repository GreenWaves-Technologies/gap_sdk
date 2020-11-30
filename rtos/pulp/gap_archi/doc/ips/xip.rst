Input file: fe/ips/XIP/README.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |                    Name                     |Offset|Width|                                   Description                                    |
    +=============================================+======+=====+==================================================================================+
    |:ref:`CFG_XIP<xip_CFG_XIP>`                  |     0|   32|Main config register for XIP (tlb mode)                                           |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_SOFT_RESET<xip_CFG_SOFT_RESET>`    |     4|   32|Flush mask register for pages, allow keeping page synced with EXT                 |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CLUSTER_POWER_ON<xip_CLUSTER_POWER_ON>`|     8|   32|Inform XIP whether cluster is on or not (for icache flush)                        |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_VIRT_ADDR0<xip_CFG_VIRT_ADDR0>`    |    16|   32|Base virtual address for hyper0                                                   |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_VIRT_ADDR1<xip_CFG_VIRT_ADDR1>`    |    20|   32|Base virtual address for hyper1                                                   |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_VIRT_ADDR2<xip_CFG_VIRT_ADDR2>`    |    24|   32|Base virtual address for mram                                                     |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_EXT_ADDR0<xip_CFG_EXT_ADDR0>`      |    28|   32|Base external address for hyper0                                                  |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_EXT_ADDR1<xip_CFG_EXT_ADDR1>`      |    32|   32|Base external address for hyper1                                                  |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_EXT_ADDR2<xip_CFG_EXT_ADDR2>`      |    36|   32|Base external address for mram                                                    |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_MNT_SIZE0<xip_CFG_MNT_SIZE0>`      |    40|   32|Virt memory size for hyper0 (pages)                                               |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_MNT_SIZE1<xip_CFG_MNT_SIZE1>`      |    44|   32|Virt memory size for hyper1 (pages)                                               |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_MNT_SIZE2<xip_CFG_MNT_SIZE2>`      |    48|   32|Virt memory size for mram   (pages)                                               |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE_SIZE0<xip_CFG_PAGE_SIZE0>`    |    52|   32|Page size for hyper0 (512B - 64KB, power of two)                                  |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE_SIZE1<xip_CFG_PAGE_SIZE1>`    |    56|   32|Page size for hyper1 (512B - 64KB, power of two)                                  |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE_SIZE2<xip_CFG_PAGE_SIZE2>`    |    60|   32|Page size for mram (512B - 64KB, power of two)                                    |
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE0<xip_CFG_PAGE0>`              |    64|   32|page0 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE1<xip_CFG_PAGE1>`              |    68|   32|page1 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE2<xip_CFG_PAGE2>`              |    72|   32|page2 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE3<xip_CFG_PAGE3>`              |    76|   32|page3 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE4<xip_CFG_PAGE4>`              |    80|   32|page4 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE5<xip_CFG_PAGE5>`              |    84|   32|page5 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE6<xip_CFG_PAGE6>`              |    88|   32|page6 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE7<xip_CFG_PAGE7>`              |    92|   32|page7 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE8<xip_CFG_PAGE8>`              |    96|   32|page8 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE9<xip_CFG_PAGE9>`              |   100|   32|page9 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE10<xip_CFG_PAGE10>`            |   104|   32|page10 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE11<xip_CFG_PAGE11>`            |   108|   32|page11 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE12<xip_CFG_PAGE12>`            |   112|   32|page12 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE13<xip_CFG_PAGE13>`            |   116|   32|page13 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE14<xip_CFG_PAGE14>`            |   120|   32|page14 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+
    |:ref:`CFG_PAGE15<xip_CFG_PAGE15>`            |   124|   32|page15 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)|
    +---------------------------------------------+------+-----+----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Main config register for XIP (tlb mode)
                #define XIP_CFG_XIP_OFFSET                       0x0
        
                // Flush mask register for pages, allow keeping page synced with EXT
                #define XIP_CFG_SOFT_RESET_OFFSET                0x4
        
                // Inform XIP whether cluster is on or not (for icache flush)
                #define XIP_CLUSTER_POWER_ON_OFFSET              0x8
        
                // Base virtual address for hyper0
                #define XIP_CFG_VIRT_ADDR0_OFFSET                0x10
        
                // Base virtual address for hyper1
                #define XIP_CFG_VIRT_ADDR1_OFFSET                0x14
        
                // Base virtual address for mram
                #define XIP_CFG_VIRT_ADDR2_OFFSET                0x18
        
                // Base external address for hyper0
                #define XIP_CFG_EXT_ADDR0_OFFSET                 0x1c
        
                // Base external address for hyper1
                #define XIP_CFG_EXT_ADDR1_OFFSET                 0x20
        
                // Base external address for mram
                #define XIP_CFG_EXT_ADDR2_OFFSET                 0x24
        
                // Virt memory size for hyper0 (pages)
                #define XIP_CFG_MNT_SIZE0_OFFSET                 0x28
        
                // Virt memory size for hyper1 (pages)
                #define XIP_CFG_MNT_SIZE1_OFFSET                 0x2c
        
                // Virt memory size for mram   (pages)
                #define XIP_CFG_MNT_SIZE2_OFFSET                 0x30
        
                // Page size for hyper0 (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE0_OFFSET                0x34
        
                // Page size for hyper1 (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE1_OFFSET                0x38
        
                // Page size for mram (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE2_OFFSET                0x3c
        
                // page0 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE0_OFFSET                     0x40
        
                // page1 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE1_OFFSET                     0x44
        
                // page2 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE2_OFFSET                     0x48
        
                // page3 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE3_OFFSET                     0x4c
        
                // page4 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE4_OFFSET                     0x50
        
                // page5 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE5_OFFSET                     0x54
        
                // page6 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE6_OFFSET                     0x58
        
                // page7 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE7_OFFSET                     0x5c
        
                // page8 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE8_OFFSET                     0x60
        
                // page9 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE9_OFFSET                     0x64
        
                // page10 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE10_OFFSET                    0x68
        
                // page11 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE11_OFFSET                    0x6c
        
                // page12 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE12_OFFSET                    0x70
        
                // page13 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE13_OFFSET                    0x74
        
                // page14 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE14_OFFSET                    0x78
        
                // page15 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE15_OFFSET                    0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_xip_get(uint32_t base);
        static inline void xip_cfg_xip_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_soft_reset_get(uint32_t base);
        static inline void xip_cfg_soft_reset_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cluster_power_on_get(uint32_t base);
        static inline void xip_cluster_power_on_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_virt_addr0_get(uint32_t base);
        static inline void xip_cfg_virt_addr0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_virt_addr1_get(uint32_t base);
        static inline void xip_cfg_virt_addr1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_virt_addr2_get(uint32_t base);
        static inline void xip_cfg_virt_addr2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_ext_addr0_get(uint32_t base);
        static inline void xip_cfg_ext_addr0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_ext_addr1_get(uint32_t base);
        static inline void xip_cfg_ext_addr1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_ext_addr2_get(uint32_t base);
        static inline void xip_cfg_ext_addr2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_mnt_size0_get(uint32_t base);
        static inline void xip_cfg_mnt_size0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_mnt_size1_get(uint32_t base);
        static inline void xip_cfg_mnt_size1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_mnt_size2_get(uint32_t base);
        static inline void xip_cfg_mnt_size2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page_size0_get(uint32_t base);
        static inline void xip_cfg_page_size0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page_size1_get(uint32_t base);
        static inline void xip_cfg_page_size1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page_size2_get(uint32_t base);
        static inline void xip_cfg_page_size2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page0_get(uint32_t base);
        static inline void xip_cfg_page0_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page1_get(uint32_t base);
        static inline void xip_cfg_page1_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page2_get(uint32_t base);
        static inline void xip_cfg_page2_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page3_get(uint32_t base);
        static inline void xip_cfg_page3_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page4_get(uint32_t base);
        static inline void xip_cfg_page4_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page5_get(uint32_t base);
        static inline void xip_cfg_page5_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page6_get(uint32_t base);
        static inline void xip_cfg_page6_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page7_get(uint32_t base);
        static inline void xip_cfg_page7_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page8_get(uint32_t base);
        static inline void xip_cfg_page8_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page9_get(uint32_t base);
        static inline void xip_cfg_page9_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page10_get(uint32_t base);
        static inline void xip_cfg_page10_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page11_get(uint32_t base);
        static inline void xip_cfg_page11_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page12_get(uint32_t base);
        static inline void xip_cfg_page12_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page13_get(uint32_t base);
        static inline void xip_cfg_page13_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page14_get(uint32_t base);
        static inline void xip_cfg_page14_set(uint32_t base, uint32_t value);

        static inline uint32_t xip_cfg_page15_get(uint32_t base);
        static inline void xip_cfg_page15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable or Disable TLB mode (access: R/W)
        #define XIP_CFG_XIP_TLB_EN_BIT                                       0
        #define XIP_CFG_XIP_TLB_EN_WIDTH                                     1
        #define XIP_CFG_XIP_TLB_EN_MASK                                      0x1
        #define XIP_CFG_XIP_TLB_EN_RESET                                     0x0
        
        // Flush dirty pages, and reset logic to enable reconfiguration. Falls to 0 when done. (access: R/W)
        #define XIP_CFG_SOFT_RESET_RESET_BIT                                 0
        #define XIP_CFG_SOFT_RESET_RESET_WIDTH                               1
        #define XIP_CFG_SOFT_RESET_RESET_MASK                                0x1
        #define XIP_CFG_SOFT_RESET_RESET_RESET                               0x0
        
        // Inform XIP that cluster is powered on (access: R/W)
        #define XIP_CLUSTER_POWER_ON_POWER_ON_BIT                            0
        #define XIP_CLUSTER_POWER_ON_POWER_ON_WIDTH                          1
        #define XIP_CLUSTER_POWER_ON_POWER_ON_MASK                           0x1
        #define XIP_CLUSTER_POWER_ON_POWER_ON_RESET                          0x0
        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_RESET                           0x20000000
        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_RESET                           0x20000000
        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_RESET                           0x20000000
        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_RESET                             0x0
        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_RESET                             0x0
        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_RESET                             0x0
        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_RESET                             0x0
        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_RESET                             0x0
        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_RESET                             0x0
        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_RESET                           0x0
        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_RESET                           0x0
        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_RESET                           0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE0_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE0_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE0_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE0_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE0_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE0_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE0_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE0_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE0_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE0_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE0_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE0_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE0_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE0_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE0_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE0_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE1_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE1_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE1_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE1_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE1_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE1_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE1_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE1_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE1_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE1_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE1_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE1_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE1_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE1_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE1_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE1_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE2_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE2_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE2_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE2_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE2_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE2_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE2_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE2_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE2_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE2_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE2_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE2_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE2_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE2_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE2_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE2_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE3_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE3_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE3_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE3_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE3_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE3_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE3_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE3_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE3_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE3_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE3_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE3_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE3_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE3_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE3_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE3_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE4_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE4_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE4_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE4_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE4_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE4_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE4_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE4_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE4_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE4_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE4_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE4_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE4_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE4_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE4_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE4_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE5_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE5_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE5_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE5_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE5_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE5_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE5_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE5_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE5_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE5_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE5_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE5_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE5_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE5_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE5_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE5_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE6_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE6_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE6_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE6_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE6_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE6_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE6_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE6_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE6_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE6_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE6_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE6_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE6_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE6_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE6_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE6_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE7_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE7_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE7_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE7_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE7_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE7_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE7_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE7_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE7_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE7_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE7_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE7_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE7_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE7_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE7_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE7_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE8_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE8_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE8_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE8_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE8_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE8_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE8_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE8_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE8_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE8_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE8_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE8_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE8_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE8_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE8_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE8_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE9_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE9_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE9_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE9_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE9_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE9_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE9_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE9_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE9_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE9_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE9_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE9_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE9_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE9_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE9_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE9_PER_ID_RESET                                   0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE10_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE10_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE10_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE10_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE10_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE10_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE10_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE10_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE10_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE10_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE10_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE10_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE10_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE10_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE10_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE10_PER_ID_RESET                                  0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE11_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE11_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE11_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE11_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE11_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE11_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE11_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE11_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE11_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE11_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE11_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE11_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE11_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE11_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE11_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE11_PER_ID_RESET                                  0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE12_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE12_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE12_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE12_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE12_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE12_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE12_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE12_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE12_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE12_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE12_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE12_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE12_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE12_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE12_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE12_PER_ID_RESET                                  0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE13_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE13_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE13_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE13_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE13_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE13_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE13_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE13_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE13_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE13_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE13_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE13_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE13_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE13_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE13_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE13_PER_ID_RESET                                  0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE14_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE14_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE14_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE14_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE14_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE14_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE14_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE14_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE14_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE14_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE14_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE14_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE14_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE14_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE14_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE14_PER_ID_RESET                                  0x0
        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE15_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE15_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE15_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE15_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE15_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE15_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE15_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE15_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE15_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE15_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE15_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE15_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE15_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE15_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE15_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE15_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_XIP_TLB_EN_GET(value)                      (GAP_BEXTRACTU((value),1,0))
        #define XIP_CFG_XIP_TLB_EN_GETS(value)                     (GAP_BEXTRACT((value),1,0))
        #define XIP_CFG_XIP_TLB_EN_SET(value,field)                (GAP_BINSERT((value),(field),1,0))
        #define XIP_CFG_XIP_TLB_EN(val)                            ((val) << 0)
        
        #define XIP_CFG_SOFT_RESET_RESET_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define XIP_CFG_SOFT_RESET_RESET_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define XIP_CFG_SOFT_RESET_RESET_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define XIP_CFG_SOFT_RESET_RESET(val)                      ((val) << 0)
        
        #define XIP_CLUSTER_POWER_ON_POWER_ON_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON(val)                 ((val) << 0)
        
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR(val)                  ((val) << 0)
        
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR(val)                  ((val) << 0)
        
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR(val)                  ((val) << 0)
        
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR(val)                    ((val) << 0)
        
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR(val)                    ((val) << 0)
        
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR(val)                    ((val) << 0)
        
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE(val)                    ((val) << 0)
        
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE(val)                    ((val) << 0)
        
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE(val)                    ((val) << 0)
        
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE(val)                  ((val) << 0)
        
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE(val)                  ((val) << 0)
        
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE(val)                  ((val) << 0)
        
        #define XIP_CFG_PAGE0_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE0_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE0_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE0_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE0_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE0_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE0_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE0_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE0_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE0_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE1_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE1_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE1_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE1_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE1_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE1_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE1_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE1_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE1_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE1_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE2_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE2_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE2_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE2_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE2_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE2_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE2_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE2_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE2_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE2_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE3_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE3_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE3_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE3_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE3_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE3_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE3_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE3_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE3_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE3_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE4_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE4_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE4_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE4_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE4_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE4_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE4_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE4_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE4_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE4_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE5_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE5_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE5_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE5_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE5_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE5_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE5_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE5_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE5_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE5_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE6_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE6_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE6_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE6_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE6_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE6_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE6_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE6_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE6_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE6_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE7_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE7_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE7_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE7_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE7_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE7_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE7_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE7_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE7_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE7_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE8_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE8_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE8_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE8_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE8_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE8_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE8_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE8_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE8_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE8_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE9_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE9_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE9_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE9_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE9_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE9_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE9_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE9_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE9_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE9_PER_ID(val)                          ((val) << 30)
        
        #define XIP_CFG_PAGE10_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE10_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE10_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE10_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE10_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE10_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE10_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE10_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE10_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE10_PER_ID(val)                         ((val) << 30)
        
        #define XIP_CFG_PAGE11_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE11_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE11_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE11_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE11_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE11_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE11_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE11_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE11_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE11_PER_ID(val)                         ((val) << 30)
        
        #define XIP_CFG_PAGE12_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE12_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE12_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE12_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE12_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE12_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE12_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE12_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE12_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE12_PER_ID(val)                         ((val) << 30)
        
        #define XIP_CFG_PAGE13_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE13_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE13_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE13_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE13_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE13_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE13_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE13_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE13_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE13_PER_ID(val)                         ((val) << 30)
        
        #define XIP_CFG_PAGE14_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE14_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE14_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE14_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE14_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE14_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE14_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE14_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE14_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE14_PER_ID(val)                         ((val) << 30)
        
        #define XIP_CFG_PAGE15_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE15_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE15_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE15_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE15_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE15_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE15_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE15_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE15_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE15_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** XIP_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cfg_xip;  // Main config register for XIP (tlb mode)
            volatile uint32_t cfg_soft_reset;  // Flush mask register for pages, allow keeping page synced with EXT
            volatile uint32_t cluster_power_on;  // Inform XIP whether cluster is on or not (for icache flush)
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t cfg_virt_addr0;  // Base virtual address for hyper0
            volatile uint32_t cfg_virt_addr1;  // Base virtual address for hyper1
            volatile uint32_t cfg_virt_addr2;  // Base virtual address for mram
            volatile uint32_t cfg_ext_addr0;  // Base external address for hyper0
            volatile uint32_t cfg_ext_addr1;  // Base external address for hyper1
            volatile uint32_t cfg_ext_addr2;  // Base external address for mram
            volatile uint32_t cfg_mnt_size0;  // Virt memory size for hyper0 (pages)
            volatile uint32_t cfg_mnt_size1;  // Virt memory size for hyper1 (pages)
            volatile uint32_t cfg_mnt_size2;  // Virt memory size for mram   (pages)
            volatile uint32_t cfg_page_size0;  // Page size for hyper0 (512B - 64KB, power of two)
            volatile uint32_t cfg_page_size1;  // Page size for hyper1 (512B - 64KB, power of two)
            volatile uint32_t cfg_page_size2;  // Page size for mram (512B - 64KB, power of two)
            volatile uint32_t cfg_page0;  // page0 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page1;  // page1 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page2;  // page2 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page3;  // page3 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page4;  // page4 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page5;  // page5 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page6;  // page6 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page7;  // page7 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page8;  // page8 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page9;  // page9 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page10;  // page10 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page11;  // page11 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page12;  // page12 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page13;  // page13 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page14;  // page14 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
            volatile uint32_t cfg_page15;  // page15 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
        } __attribute__((packed)) xip_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tlb_en          :1 ; // Enable or Disable TLB mode
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_xip_t;
        
        typedef union {
          struct {
            unsigned int reset           :1 ; // Flush dirty pages, and reset logic to enable reconfiguration. Falls to 0 when done.
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_soft_reset_t;
        
        typedef union {
          struct {
            unsigned int power_on        :1 ; // Inform XIP that cluster is powered on
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cluster_power_on_t;
        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr0_t;
        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr1_t;
        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr2_t;
        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr0_t;
        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr1_t;
        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr2_t;
        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size0_t;
        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size1_t;
        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size2_t;
        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size0_t;
        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size1_t;
        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size2_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page0_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page1_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page2_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page3_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page4_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page5_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page6_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page7_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page8_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page9_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page10_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page11_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page12_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page13_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page14_t;
        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_xip : public vp::regmap
        {
        public:
            vp_xip_cfg_xip cfg_xip;
            vp_xip_cfg_soft_reset cfg_soft_reset;
            vp_xip_cluster_power_on cluster_power_on;
            vp_xip_cfg_virt_addr0 cfg_virt_addr0;
            vp_xip_cfg_virt_addr1 cfg_virt_addr1;
            vp_xip_cfg_virt_addr2 cfg_virt_addr2;
            vp_xip_cfg_ext_addr0 cfg_ext_addr0;
            vp_xip_cfg_ext_addr1 cfg_ext_addr1;
            vp_xip_cfg_ext_addr2 cfg_ext_addr2;
            vp_xip_cfg_mnt_size0 cfg_mnt_size0;
            vp_xip_cfg_mnt_size1 cfg_mnt_size1;
            vp_xip_cfg_mnt_size2 cfg_mnt_size2;
            vp_xip_cfg_page_size0 cfg_page_size0;
            vp_xip_cfg_page_size1 cfg_page_size1;
            vp_xip_cfg_page_size2 cfg_page_size2;
            vp_xip_cfg_page0 cfg_page0;
            vp_xip_cfg_page1 cfg_page1;
            vp_xip_cfg_page2 cfg_page2;
            vp_xip_cfg_page3 cfg_page3;
            vp_xip_cfg_page4 cfg_page4;
            vp_xip_cfg_page5 cfg_page5;
            vp_xip_cfg_page6 cfg_page6;
            vp_xip_cfg_page7 cfg_page7;
            vp_xip_cfg_page8 cfg_page8;
            vp_xip_cfg_page9 cfg_page9;
            vp_xip_cfg_page10 cfg_page10;
            vp_xip_cfg_page11 cfg_page11;
            vp_xip_cfg_page12 cfg_page12;
            vp_xip_cfg_page13 cfg_page13;
            vp_xip_cfg_page14 cfg_page14;
            vp_xip_cfg_page15 cfg_page15;
        };

|

.. _xip_CFG_XIP:

CFG_XIP
"""""""

Main config register for XIP (tlb mode)

.. table:: 

    +-----+---+------+--------------------------+
    |Bit #|R/W| Name |       Description        |
    +=====+===+======+==========================+
    |    0|R/W|TLB_EN|Enable or Disable TLB mode|
    +-----+---+------+--------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Main config register for XIP (tlb mode)
                #define XIP_CFG_XIP_OFFSET                       0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_xip_get(uint32_t base);
        static inline void xip_cfg_xip_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable or Disable TLB mode (access: R/W)
        #define XIP_CFG_XIP_TLB_EN_BIT                                       0
        #define XIP_CFG_XIP_TLB_EN_WIDTH                                     1
        #define XIP_CFG_XIP_TLB_EN_MASK                                      0x1
        #define XIP_CFG_XIP_TLB_EN_RESET                                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_XIP_TLB_EN_GET(value)                      (GAP_BEXTRACTU((value),1,0))
        #define XIP_CFG_XIP_TLB_EN_GETS(value)                     (GAP_BEXTRACT((value),1,0))
        #define XIP_CFG_XIP_TLB_EN_SET(value,field)                (GAP_BINSERT((value),(field),1,0))
        #define XIP_CFG_XIP_TLB_EN(val)                            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int tlb_en          :1 ; // Enable or Disable TLB mode
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_xip_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_xip : public vp::reg_32
        {
        public:
            inline void tlb_en_set(uint32_t value);
            inline uint32_t tlb_en_get();
        };

|

.. _xip_CFG_SOFT_RESET:

CFG_SOFT_RESET
""""""""""""""

Flush mask register for pages, allow keeping page synced with EXT

.. table:: 

    +-----+---+-----+-----------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                    Description                                    |
    +=====+===+=====+===================================================================================+
    |    0|R/W|RESET|Flush dirty pages, and reset logic to enable reconfiguration. Falls to 0 when done.|
    +-----+---+-----+-----------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Flush mask register for pages, allow keeping page synced with EXT
                #define XIP_CFG_SOFT_RESET_OFFSET                0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_soft_reset_get(uint32_t base);
        static inline void xip_cfg_soft_reset_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Flush dirty pages, and reset logic to enable reconfiguration. Falls to 0 when done. (access: R/W)
        #define XIP_CFG_SOFT_RESET_RESET_BIT                                 0
        #define XIP_CFG_SOFT_RESET_RESET_WIDTH                               1
        #define XIP_CFG_SOFT_RESET_RESET_MASK                                0x1
        #define XIP_CFG_SOFT_RESET_RESET_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_SOFT_RESET_RESET_GET(value)                (GAP_BEXTRACTU((value),1,0))
        #define XIP_CFG_SOFT_RESET_RESET_GETS(value)               (GAP_BEXTRACT((value),1,0))
        #define XIP_CFG_SOFT_RESET_RESET_SET(value,field)          (GAP_BINSERT((value),(field),1,0))
        #define XIP_CFG_SOFT_RESET_RESET(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int reset           :1 ; // Flush dirty pages, and reset logic to enable reconfiguration. Falls to 0 when done.
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_soft_reset_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_soft_reset : public vp::reg_32
        {
        public:
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
        };

|

.. _xip_CLUSTER_POWER_ON:

CLUSTER_POWER_ON
""""""""""""""""

Inform XIP whether cluster is on or not (for icache flush)

.. table:: 

    +-----+---+--------+-------------------------------------+
    |Bit #|R/W|  Name  |             Description             |
    +=====+===+========+=====================================+
    |    0|R/W|POWER_ON|Inform XIP that cluster is powered on|
    +-----+---+--------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Inform XIP whether cluster is on or not (for icache flush)
                #define XIP_CLUSTER_POWER_ON_OFFSET              0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cluster_power_on_get(uint32_t base);
        static inline void xip_cluster_power_on_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Inform XIP that cluster is powered on (access: R/W)
        #define XIP_CLUSTER_POWER_ON_POWER_ON_BIT                            0
        #define XIP_CLUSTER_POWER_ON_POWER_ON_WIDTH                          1
        #define XIP_CLUSTER_POWER_ON_POWER_ON_MASK                           0x1
        #define XIP_CLUSTER_POWER_ON_POWER_ON_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CLUSTER_POWER_ON_POWER_ON_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define XIP_CLUSTER_POWER_ON_POWER_ON(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int power_on        :1 ; // Inform XIP that cluster is powered on
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cluster_power_on_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cluster_power_on : public vp::reg_32
        {
        public:
            inline void power_on_set(uint32_t value);
            inline uint32_t power_on_get();
        };

|

.. _xip_CFG_VIRT_ADDR0:

CFG_VIRT_ADDR0
""""""""""""""

Base virtual address for hyper0

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR|Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base virtual address for hyper0
                #define XIP_CFG_VIRT_ADDR0_OFFSET                0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_virt_addr0_get(uint32_t base);
        static inline void xip_cfg_virt_addr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_RESET                           0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR0_VIRT_ADDR(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_virt_addr0 : public vp::reg_32
        {
        public:
            inline void virt_addr_set(uint32_t value);
            inline uint32_t virt_addr_get();
        };

|

.. _xip_CFG_VIRT_ADDR1:

CFG_VIRT_ADDR1
""""""""""""""

Base virtual address for hyper1

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR|Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base virtual address for hyper1
                #define XIP_CFG_VIRT_ADDR1_OFFSET                0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_virt_addr1_get(uint32_t base);
        static inline void xip_cfg_virt_addr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_RESET                           0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR1_VIRT_ADDR(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_virt_addr1 : public vp::reg_32
        {
        public:
            inline void virt_addr_set(uint32_t value);
            inline uint32_t virt_addr_get();
        };

|

.. _xip_CFG_VIRT_ADDR2:

CFG_VIRT_ADDR2
""""""""""""""

Base virtual address for mram

.. table:: 

    +-----+---+---------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                          Description                                           |
    +=====+===+=========+================================================================================================+
    |31:0 |R/W|VIRT_ADDR|Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]|
    +-----+---+---------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base virtual address for mram
                #define XIP_CFG_VIRT_ADDR2_OFFSET                0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_virt_addr2_get(uint32_t base);
        static inline void xip_cfg_virt_addr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF] (access: R/W)
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_BIT                             0
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_WIDTH                           32
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_MASK                            0xffffffff
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_RESET                           0x20000000

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_GET(value)            (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_GETS(value)           (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR_SET(value,field)      (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_VIRT_ADDR2_VIRT_ADDR(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int virt_addr       :32; // Start of Virtual Address for external peripheral 0 Must be in the range[0x2000_0000-0x2FFF_FFFF]
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_virt_addr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_virt_addr2 : public vp::reg_32
        {
        public:
            inline void virt_addr_set(uint32_t value);
            inline uint32_t virt_addr_get();
        };

|

.. _xip_CFG_EXT_ADDR0:

CFG_EXT_ADDR0
"""""""""""""

Base external address for hyper0

.. table:: 

    +-----+---+--------+---------------------------------------------------+
    |Bit #|R/W|  Name  |                    Description                    |
    +=====+===+========+===================================================+
    |31:0 |R/W|EXT_ADDR|Start of External Address for external peripheral 0|
    +-----+---+--------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base external address for hyper0
                #define XIP_CFG_EXT_ADDR0_OFFSET                 0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_ext_addr0_get(uint32_t base);
        static inline void xip_cfg_ext_addr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR0_EXT_ADDR(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_ext_addr0 : public vp::reg_32
        {
        public:
            inline void ext_addr_set(uint32_t value);
            inline uint32_t ext_addr_get();
        };

|

.. _xip_CFG_EXT_ADDR1:

CFG_EXT_ADDR1
"""""""""""""

Base external address for hyper1

.. table:: 

    +-----+---+--------+---------------------------------------------------+
    |Bit #|R/W|  Name  |                    Description                    |
    +=====+===+========+===================================================+
    |31:0 |R/W|EXT_ADDR|Start of External Address for external peripheral 0|
    +-----+---+--------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base external address for hyper1
                #define XIP_CFG_EXT_ADDR1_OFFSET                 0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_ext_addr1_get(uint32_t base);
        static inline void xip_cfg_ext_addr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR1_EXT_ADDR(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_ext_addr1 : public vp::reg_32
        {
        public:
            inline void ext_addr_set(uint32_t value);
            inline uint32_t ext_addr_get();
        };

|

.. _xip_CFG_EXT_ADDR2:

CFG_EXT_ADDR2
"""""""""""""

Base external address for mram

.. table:: 

    +-----+---+--------+---------------------------------------------------+
    |Bit #|R/W|  Name  |                    Description                    |
    +=====+===+========+===================================================+
    |31:0 |R/W|EXT_ADDR|Start of External Address for external peripheral 0|
    +-----+---+--------+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base external address for mram
                #define XIP_CFG_EXT_ADDR2_OFFSET                 0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_ext_addr2_get(uint32_t base);
        static inline void xip_cfg_ext_addr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Start of External Address for external peripheral 0 (access: R/W)
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_BIT                               0
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_WIDTH                             32
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_MASK                              0xffffffff
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define XIP_CFG_EXT_ADDR2_EXT_ADDR(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ext_addr        :32; // Start of External Address for external peripheral 0
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_ext_addr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_ext_addr2 : public vp::reg_32
        {
        public:
            inline void ext_addr_set(uint32_t value);
            inline uint32_t ext_addr_get();
        };

|

.. _xip_CFG_MNT_SIZE0:

CFG_MNT_SIZE0
"""""""""""""

Virt memory size for hyper0 (pages)

.. table:: 

    +-----+---+--------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                          Description                                           |
    +=====+===+========+================================================================================================+
    |15:0 |R/W|MNT_SIZE|Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)|
    +-----+---+--------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Virt memory size for hyper0 (pages)
                #define XIP_CFG_MNT_SIZE0_OFFSET                 0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_mnt_size0_get(uint32_t base);
        static inline void xip_cfg_mnt_size0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE0_MNT_SIZE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_mnt_size0 : public vp::reg_32
        {
        public:
            inline void mnt_size_set(uint32_t value);
            inline uint32_t mnt_size_get();
        };

|

.. _xip_CFG_MNT_SIZE1:

CFG_MNT_SIZE1
"""""""""""""

Virt memory size for hyper1 (pages)

.. table:: 

    +-----+---+--------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                          Description                                           |
    +=====+===+========+================================================================================================+
    |15:0 |R/W|MNT_SIZE|Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)|
    +-----+---+--------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Virt memory size for hyper1 (pages)
                #define XIP_CFG_MNT_SIZE1_OFFSET                 0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_mnt_size1_get(uint32_t base);
        static inline void xip_cfg_mnt_size1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE1_MNT_SIZE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_mnt_size1 : public vp::reg_32
        {
        public:
            inline void mnt_size_set(uint32_t value);
            inline uint32_t mnt_size_get();
        };

|

.. _xip_CFG_MNT_SIZE2:

CFG_MNT_SIZE2
"""""""""""""

Virt memory size for mram   (pages)

.. table:: 

    +-----+---+--------+------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                          Description                                           |
    +=====+===+========+================================================================================================+
    |15:0 |R/W|MNT_SIZE|Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)|
    +-----+---+--------+------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Virt memory size for mram   (pages)
                #define XIP_CFG_MNT_SIZE2_OFFSET                 0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_mnt_size2_get(uint32_t base);
        static inline void xip_cfg_mnt_size2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size) (access: R/W)
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_BIT                               0
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_WIDTH                             16
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_MASK                              0xffff
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_GET(value)              (GAP_BEXTRACTU((value),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_GETS(value)             (GAP_BEXTRACT((value),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE_SET(value,field)        (GAP_BINSERT((value),(field),16,0))
        #define XIP_CFG_MNT_SIZE2_MNT_SIZE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mnt_size        :16; // Size of the mounted region in pages for peripheral 0 (total size of region = this reg*page size)
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_mnt_size2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_mnt_size2 : public vp::reg_32
        {
        public:
            inline void mnt_size_set(uint32_t value);
            inline uint32_t mnt_size_get();
        };

|

.. _xip_CFG_PAGE_SIZE0:

CFG_PAGE_SIZE0
""""""""""""""

Page size for hyper0 (512B - 64KB, power of two)

.. table:: 

    +-----+---+---------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                        Description                                         |
    +=====+===+=========+============================================================================================+
    |2:0  |R/W|PAGE_SIZE|Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes|
    +-----+---+---------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Page size for hyper0 (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE0_OFFSET                0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page_size0_get(uint32_t base);
        static inline void xip_cfg_page_size0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE0_PAGE_SIZE(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page_size0 : public vp::reg_32
        {
        public:
            inline void page_size_set(uint32_t value);
            inline uint32_t page_size_get();
        };

|

.. _xip_CFG_PAGE_SIZE1:

CFG_PAGE_SIZE1
""""""""""""""

Page size for hyper1 (512B - 64KB, power of two)

.. table:: 

    +-----+---+---------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                        Description                                         |
    +=====+===+=========+============================================================================================+
    |2:0  |R/W|PAGE_SIZE|Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes|
    +-----+---+---------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Page size for hyper1 (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE1_OFFSET                0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page_size1_get(uint32_t base);
        static inline void xip_cfg_page_size1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE1_PAGE_SIZE(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page_size1 : public vp::reg_32
        {
        public:
            inline void page_size_set(uint32_t value);
            inline uint32_t page_size_get();
        };

|

.. _xip_CFG_PAGE_SIZE2:

CFG_PAGE_SIZE2
""""""""""""""

Page size for mram (512B - 64KB, power of two)

.. table:: 

    +-----+---+---------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                        Description                                         |
    +=====+===+=========+============================================================================================+
    |2:0  |R/W|PAGE_SIZE|Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes|
    +-----+---+---------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Page size for mram (512B - 64KB, power of two)
                #define XIP_CFG_PAGE_SIZE2_OFFSET                0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page_size2_get(uint32_t base);
        static inline void xip_cfg_page_size2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes (access: R/W)
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_BIT                             0
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_WIDTH                           3
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_MASK                            0x7
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_GET(value)            (GAP_BEXTRACTU((value),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_GETS(value)           (GAP_BEXTRACT((value),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE_SET(value,field)      (GAP_BINSERT((value),(field),3,0))
        #define XIP_CFG_PAGE_SIZE2_PAGE_SIZE(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int page_size       :3 ; // Size of pages for peripheral 0 0: 512Bytes 1: 1KBytes 2: 2KBytes 3: 4KBytes ...  7: 64KBytes
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page_size2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page_size2 : public vp::reg_32
        {
        public:
            inline void page_size_set(uint32_t value);
            inline uint32_t page_size_get();
        };

|

.. _xip_CFG_PAGE0:

CFG_PAGE0
"""""""""

page0 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page0 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE0_OFFSET                     0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page0_get(uint32_t base);
        static inline void xip_cfg_page0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE0_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE0_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE0_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE0_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE0_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE0_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE0_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE0_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE0_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE0_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE0_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE0_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE0_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE0_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE0_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE0_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE0_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE0_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE0_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE0_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE0_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE0_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE0_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE0_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE0_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE0_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE0_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE0_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page0 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE1:

CFG_PAGE1
"""""""""

page1 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page1 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE1_OFFSET                     0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page1_get(uint32_t base);
        static inline void xip_cfg_page1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE1_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE1_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE1_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE1_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE1_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE1_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE1_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE1_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE1_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE1_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE1_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE1_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE1_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE1_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE1_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE1_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE1_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE1_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE1_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE1_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE1_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE1_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE1_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE1_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE1_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE1_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE1_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE1_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page1 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE2:

CFG_PAGE2
"""""""""

page2 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page2 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE2_OFFSET                     0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page2_get(uint32_t base);
        static inline void xip_cfg_page2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE2_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE2_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE2_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE2_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE2_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE2_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE2_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE2_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE2_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE2_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE2_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE2_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE2_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE2_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE2_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE2_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE2_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE2_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE2_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE2_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE2_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE2_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE2_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE2_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE2_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE2_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE2_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE2_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page2 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE3:

CFG_PAGE3
"""""""""

page3 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page3 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE3_OFFSET                     0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page3_get(uint32_t base);
        static inline void xip_cfg_page3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE3_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE3_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE3_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE3_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE3_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE3_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE3_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE3_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE3_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE3_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE3_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE3_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE3_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE3_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE3_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE3_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE3_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE3_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE3_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE3_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE3_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE3_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE3_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE3_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE3_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE3_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE3_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE3_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page3 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE4:

CFG_PAGE4
"""""""""

page4 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page4 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE4_OFFSET                     0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page4_get(uint32_t base);
        static inline void xip_cfg_page4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE4_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE4_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE4_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE4_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE4_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE4_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE4_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE4_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE4_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE4_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE4_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE4_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE4_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE4_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE4_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE4_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE4_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE4_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE4_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE4_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE4_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE4_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE4_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE4_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE4_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE4_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE4_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE4_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page4 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE5:

CFG_PAGE5
"""""""""

page5 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page5 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE5_OFFSET                     0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page5_get(uint32_t base);
        static inline void xip_cfg_page5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE5_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE5_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE5_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE5_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE5_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE5_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE5_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE5_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE5_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE5_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE5_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE5_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE5_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE5_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE5_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE5_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE5_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE5_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE5_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE5_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE5_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE5_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE5_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE5_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE5_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE5_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE5_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE5_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page5 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE6:

CFG_PAGE6
"""""""""

page6 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page6 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE6_OFFSET                     0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page6_get(uint32_t base);
        static inline void xip_cfg_page6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE6_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE6_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE6_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE6_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE6_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE6_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE6_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE6_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE6_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE6_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE6_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE6_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE6_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE6_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE6_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE6_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE6_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE6_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE6_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE6_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE6_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE6_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE6_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE6_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE6_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE6_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE6_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE6_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page6 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE7:

CFG_PAGE7
"""""""""

page7 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page7 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE7_OFFSET                     0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page7_get(uint32_t base);
        static inline void xip_cfg_page7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE7_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE7_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE7_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE7_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE7_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE7_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE7_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE7_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE7_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE7_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE7_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE7_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE7_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE7_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE7_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE7_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE7_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE7_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE7_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE7_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE7_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE7_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE7_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE7_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE7_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE7_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE7_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE7_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page7 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE8:

CFG_PAGE8
"""""""""

page8 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page8 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE8_OFFSET                     0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page8_get(uint32_t base);
        static inline void xip_cfg_page8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE8_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE8_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE8_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE8_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE8_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE8_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE8_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE8_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE8_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE8_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE8_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE8_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE8_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE8_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE8_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE8_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE8_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE8_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE8_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE8_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE8_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE8_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE8_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE8_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE8_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE8_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE8_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE8_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page8 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE9:

CFG_PAGE9
"""""""""

page9 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page9 configuration  (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE9_OFFSET                     0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page9_get(uint32_t base);
        static inline void xip_cfg_page9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE9_INT_ADDR_BIT                                   0
        #define XIP_CFG_PAGE9_INT_ADDR_WIDTH                                 21
        #define XIP_CFG_PAGE9_INT_ADDR_MASK                                  0x1fffff
        #define XIP_CFG_PAGE9_INT_ADDR_RESET                                 0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE9_CACHEABLE_BIT                                  28
        #define XIP_CFG_PAGE9_CACHEABLE_WIDTH                                1
        #define XIP_CFG_PAGE9_CACHEABLE_MASK                                 0x10000000
        #define XIP_CFG_PAGE9_CACHEABLE_RESET                                0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE9_ACTIVE_BIT                                     29
        #define XIP_CFG_PAGE9_ACTIVE_WIDTH                                   1
        #define XIP_CFG_PAGE9_ACTIVE_MASK                                    0x20000000
        #define XIP_CFG_PAGE9_ACTIVE_RESET                                   0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE9_PER_ID_BIT                                     30
        #define XIP_CFG_PAGE9_PER_ID_WIDTH                                   2
        #define XIP_CFG_PAGE9_PER_ID_MASK                                    0xc0000000
        #define XIP_CFG_PAGE9_PER_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE9_INT_ADDR_GET(value)                  (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR_GETS(value)                 (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR_SET(value,field)            (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE9_INT_ADDR(val)                        ((val) << 0)
        
        #define XIP_CFG_PAGE9_CACHEABLE_GET(value)                 (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE_GETS(value)                (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE_SET(value,field)           (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE9_CACHEABLE(val)                       ((val) << 28)
        
        #define XIP_CFG_PAGE9_ACTIVE_GET(value)                    (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE9_ACTIVE_GETS(value)                   (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE9_ACTIVE_SET(value,field)              (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE9_ACTIVE(val)                          ((val) << 29)
        
        #define XIP_CFG_PAGE9_PER_ID_GET(value)                    (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE9_PER_ID_GETS(value)                   (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE9_PER_ID_SET(value,field)              (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE9_PER_ID(val)                          ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page9 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE10:

CFG_PAGE10
""""""""""

page10 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page10 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE10_OFFSET                    0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page10_get(uint32_t base);
        static inline void xip_cfg_page10_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE10_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE10_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE10_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE10_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE10_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE10_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE10_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE10_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE10_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE10_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE10_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE10_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE10_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE10_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE10_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE10_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE10_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE10_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE10_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE10_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE10_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE10_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE10_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE10_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE10_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE10_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE10_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE10_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page10_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page10 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE11:

CFG_PAGE11
""""""""""

page11 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page11 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE11_OFFSET                    0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page11_get(uint32_t base);
        static inline void xip_cfg_page11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE11_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE11_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE11_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE11_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE11_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE11_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE11_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE11_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE11_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE11_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE11_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE11_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE11_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE11_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE11_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE11_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE11_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE11_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE11_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE11_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE11_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE11_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE11_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE11_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE11_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE11_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE11_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE11_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page11 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE12:

CFG_PAGE12
""""""""""

page12 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page12 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE12_OFFSET                    0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page12_get(uint32_t base);
        static inline void xip_cfg_page12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE12_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE12_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE12_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE12_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE12_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE12_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE12_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE12_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE12_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE12_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE12_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE12_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE12_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE12_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE12_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE12_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE12_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE12_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE12_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE12_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE12_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE12_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE12_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE12_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE12_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE12_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE12_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE12_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page12 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE13:

CFG_PAGE13
""""""""""

page13 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page13 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE13_OFFSET                    0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page13_get(uint32_t base);
        static inline void xip_cfg_page13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE13_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE13_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE13_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE13_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE13_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE13_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE13_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE13_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE13_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE13_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE13_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE13_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE13_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE13_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE13_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE13_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE13_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE13_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE13_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE13_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE13_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE13_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE13_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE13_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE13_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE13_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE13_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE13_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page13 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE14:

CFG_PAGE14
""""""""""

page14 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page14 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE14_OFFSET                    0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page14_get(uint32_t base);
        static inline void xip_cfg_page14_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE14_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE14_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE14_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE14_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE14_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE14_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE14_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE14_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE14_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE14_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE14_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE14_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE14_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE14_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE14_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE14_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE14_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE14_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE14_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE14_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE14_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE14_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE14_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE14_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE14_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE14_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE14_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE14_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page14_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page14 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|

.. _xip_CFG_PAGE15:

CFG_PAGE15
""""""""""

page15 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)

.. table:: 

    +-----+---+---------+-------------------------------------+
    |Bit #|R/W|  Name   |             Description             |
    +=====+===+=========+=====================================+
    |20:0 |R/W|INT_ADDR |21 LSB of L2 Address of the page     |
    +-----+---+---------+-------------------------------------+
    |28   |R/W|CACHEABLE|Make icache aware of this page or not|
    +-----+---+---------+-------------------------------------+
    |29   |R/W|ACTIVE   |Make page "active" or "ignored       |
    +-----+---+---------+-------------------------------------+
    |31:30|R/W|PER_ID   |Peripheral ID                        |
    +-----+---+---------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // page15 configuration (bit [31:30]: device id, [29]: active bit, [20:0]: l2 offset)
                #define XIP_CFG_PAGE15_OFFSET                    0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t xip_cfg_page15_get(uint32_t base);
        static inline void xip_cfg_page15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 21 LSB of L2 Address of the page (access: R/W)
        #define XIP_CFG_PAGE15_INT_ADDR_BIT                                  0
        #define XIP_CFG_PAGE15_INT_ADDR_WIDTH                                21
        #define XIP_CFG_PAGE15_INT_ADDR_MASK                                 0x1fffff
        #define XIP_CFG_PAGE15_INT_ADDR_RESET                                0x0
        
        // Make icache aware of this page or not (access: R/W)
        #define XIP_CFG_PAGE15_CACHEABLE_BIT                                 28
        #define XIP_CFG_PAGE15_CACHEABLE_WIDTH                               1
        #define XIP_CFG_PAGE15_CACHEABLE_MASK                                0x10000000
        #define XIP_CFG_PAGE15_CACHEABLE_RESET                               0x0
        
        // Make page "active" or "ignored" (access: R/W)
        #define XIP_CFG_PAGE15_ACTIVE_BIT                                    29
        #define XIP_CFG_PAGE15_ACTIVE_WIDTH                                  1
        #define XIP_CFG_PAGE15_ACTIVE_MASK                                   0x20000000
        #define XIP_CFG_PAGE15_ACTIVE_RESET                                  0x0
        
        // Peripheral ID (access: R/W)
        #define XIP_CFG_PAGE15_PER_ID_BIT                                    30
        #define XIP_CFG_PAGE15_PER_ID_WIDTH                                  2
        #define XIP_CFG_PAGE15_PER_ID_MASK                                   0xc0000000
        #define XIP_CFG_PAGE15_PER_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define XIP_CFG_PAGE15_INT_ADDR_GET(value)                 (GAP_BEXTRACTU((value),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR_GETS(value)                (GAP_BEXTRACT((value),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR_SET(value,field)           (GAP_BINSERT((value),(field),21,0))
        #define XIP_CFG_PAGE15_INT_ADDR(val)                       ((val) << 0)
        
        #define XIP_CFG_PAGE15_CACHEABLE_GET(value)                (GAP_BEXTRACTU((value),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE_GETS(value)               (GAP_BEXTRACT((value),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE_SET(value,field)          (GAP_BINSERT((value),(field),1,28))
        #define XIP_CFG_PAGE15_CACHEABLE(val)                      ((val) << 28)
        
        #define XIP_CFG_PAGE15_ACTIVE_GET(value)                   (GAP_BEXTRACTU((value),1,29))
        #define XIP_CFG_PAGE15_ACTIVE_GETS(value)                  (GAP_BEXTRACT((value),1,29))
        #define XIP_CFG_PAGE15_ACTIVE_SET(value,field)             (GAP_BINSERT((value),(field),1,29))
        #define XIP_CFG_PAGE15_ACTIVE(val)                         ((val) << 29)
        
        #define XIP_CFG_PAGE15_PER_ID_GET(value)                   (GAP_BEXTRACTU((value),2,30))
        #define XIP_CFG_PAGE15_PER_ID_GETS(value)                  (GAP_BEXTRACT((value),2,30))
        #define XIP_CFG_PAGE15_PER_ID_SET(value,field)             (GAP_BINSERT((value),(field),2,30))
        #define XIP_CFG_PAGE15_PER_ID(val)                         ((val) << 30)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_addr        :21; // 21 LSB of L2 Address of the page
            unsigned int padding0:7 ;
            unsigned int cacheable       :1 ; // Make icache aware of this page or not
            unsigned int active          :1 ; // Make page "active" or "ignored"
            unsigned int per_id          :2 ; // Peripheral ID
          };
          unsigned int raw;
        } __attribute__((packed)) xip_cfg_page15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_xip_cfg_page15 : public vp::reg_32
        {
        public:
            inline void int_addr_set(uint32_t value);
            inline uint32_t int_addr_get();
            inline void cacheable_set(uint32_t value);
            inline uint32_t cacheable_get();
            inline void active_set(uint32_t value);
            inline uint32_t active_get();
            inline void per_id_set(uint32_t value);
            inline uint32_t per_id_get();
        };

|
