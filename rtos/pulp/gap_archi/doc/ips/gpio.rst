Input file: fe/ips/apb/apb_gpio/docs/APB_GPIO_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +--------------------------------------------+------+-----+-------------------------------------------------+
    |                    Name                    |Offset|Width|                   Description                   |
    +============================================+======+=====+=================================================+
    |:ref:`PADDIR_00_31<gpio_PADDIR_00_31>`      |     0|   32|GPIO pad direction configuration register.       |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`GPIOEN_00_31<gpio_GPIOEN_00_31>`      |     4|   32|GPIO enable register.                            |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADIN_00_31<gpio_PADIN_00_31>`        |     8|   32|GPIO pad input value register.                   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUT_00_31<gpio_PADOUT_00_31>`      |    12|   32|GPIO pad output value register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTSET_00_31<gpio_PADOUTSET_00_31>`|    16|   32|GPIO pad output set register.                    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTCLR_00_31<gpio_PADOUTCLR_00_31>`|    20|   32|GPIO pad output clear register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTEN_00_31<gpio_INTEN_00_31>`        |    24|   32|GPIO pad interrupt enable configuration register.|
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_00_15<gpio_INTTYPE_00_15>`    |    28|   32|GPIO pad interrupt type gpio 0 to 15 register.   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_16_31<gpio_INTTYPE_16_31>`    |    32|   32|GPIO pad interrupt type gpio 16 to 31 register.  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTSTATUS_00_31<gpio_INTSTATUS_00_31>`|    36|   32|GPIO pad interrupt status register.              |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_00_03<gpio_PADCFG_00_03>`      |    40|   32|GPIO pad pin 0 to 3 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_04_07<gpio_PADCFG_04_07>`      |    44|   32|GPIO pad pin 4 to 7 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_08_11<gpio_PADCFG_08_11>`      |    48|   32|GPIO pad pin 8 to 11 configuration register.     |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_12_15<gpio_PADCFG_12_15>`      |    52|   32|GPIO pad pin 12 to 15 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_16_19<gpio_PADCFG_16_19>`      |    56|   32|GPIO pad pin 16 to 19 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_20_23<gpio_PADCFG_20_23>`      |    60|   32|GPIO pad pin 20 to 23 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_24_27<gpio_PADCFG_24_27>`      |    64|   32|GPIO pad pin 24 to 27 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_28_31<gpio_PADCFG_28_31>`      |    68|   32|GPIO pad pin 28 to 31 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADDIR_32_63<gpio_PADDIR_32_63>`      |    72|   32|GPIO pad direction configuration register.       |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`GPIOEN_32_63<gpio_GPIOEN_32_63>`      |    76|   32|GPIO enable register.                            |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADIN_32_63<gpio_PADIN_32_63>`        |    80|   32|GPIO pad input value register.                   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUT_32_63<gpio_PADOUT_32_63>`      |    84|   32|GPIO pad output value register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTSET_32_63<gpio_PADOUTSET_32_63>`|    88|   32|GPIO pad output set register.                    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTCLR_32_63<gpio_PADOUTCLR_32_63>`|    92|   32|GPIO pad output clear register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTEN_32_63<gpio_INTEN_32_63>`        |    96|   32|GPIO pad interrupt enable configuration register.|
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_32_47<gpio_INTTYPE_32_47>`    |   100|   32|GPIO pad interrupt type gpio 0 to 15 register.   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_48_63<gpio_INTTYPE_48_63>`    |   104|   32|GPIO pad interrupt type gpio 16 to 31 register.  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTSTATUS_32_63<gpio_INTSTATUS_32_63>`|   108|   32|GPIO pad interrupt status register.              |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_32_35<gpio_PADCFG_32_35>`      |   112|   32|GPIO pad pin 0 to 3 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_36_39<gpio_PADCFG_36_39>`      |   116|   32|GPIO pad pin 4 to 7 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_40_43<gpio_PADCFG_40_43>`      |   120|   32|GPIO pad pin 8 to 11 configuration register.     |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_44_47<gpio_PADCFG_44_47>`      |   124|   32|GPIO pad pin 12 to 15 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_48_51<gpio_PADCFG_48_51>`      |   128|   32|GPIO pad pin 16 to 19 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_52_55<gpio_PADCFG_52_55>`      |   132|   32|GPIO pad pin 20 to 23 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_56_59<gpio_PADCFG_56_59>`      |   136|   32|GPIO pad pin 24 to 27 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_60_63<gpio_PADCFG_60_63>`      |   140|   32|GPIO pad pin 28 to 31 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADDIR_64_95<gpio_PADDIR_64_95>`      |   144|   32|GPIO pad direction configuration register.       |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`GPIOEN_64_95<gpio_GPIOEN_64_95>`      |   148|   32|GPIO enable register.                            |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADIN_64_95<gpio_PADIN_64_95>`        |   152|   32|GPIO pad input value register.                   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUT_64_95<gpio_PADOUT_64_95>`      |   156|   32|GPIO pad output value register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTSET_64_95<gpio_PADOUTSET_64_95>`|   160|   32|GPIO pad output set register.                    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADOUTCLR_64_95<gpio_PADOUTCLR_64_95>`|   164|   32|GPIO pad output clear register.                  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTEN_64_95<gpio_INTEN_64_95>`        |   168|   32|GPIO pad interrupt enable configuration register.|
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_64_79<gpio_INTTYPE_64_79>`    |   172|   32|GPIO pad interrupt type gpio 0 to 15 register.   |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTTYPE_80_95<gpio_INTTYPE_80_95>`    |   176|   32|GPIO pad interrupt type gpio 16 to 31 register.  |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`INTSTATUS_64_95<gpio_INTSTATUS_64_95>`|   180|   32|GPIO pad interrupt status register.              |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_64_67<gpio_PADCFG_64_67>`      |   184|   32|GPIO pad pin 0 to 3 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_68_71<gpio_PADCFG_68_71>`      |   188|   32|GPIO pad pin 4 to 7 configuration register.      |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_72_75<gpio_PADCFG_72_75>`      |   192|   32|GPIO pad pin 8 to 11 configuration register.     |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_76_79<gpio_PADCFG_76_79>`      |   196|   32|GPIO pad pin 12 to 15 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_80_83<gpio_PADCFG_80_83>`      |   200|   32|GPIO pad pin 16 to 19 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_84_87<gpio_PADCFG_84_87>`      |   204|   32|GPIO pad pin 20 to 23 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_88_91<gpio_PADCFG_88_91>`      |   208|   32|GPIO pad pin 24 to 27 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+
    |:ref:`PADCFG_92_95<gpio_PADCFG_92_95>`      |   212|   32|GPIO pad pin 28 to 31 configuration register.    |
    +--------------------------------------------+------+-----+-------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_00_31_OFFSET                 0x0
        
                // GPIO enable register.
                #define GPIO_GPIOEN_00_31_OFFSET                 0x4
        
                // GPIO pad input value register.
                #define GPIO_PADIN_00_31_OFFSET                  0x8
        
                // GPIO pad output value register.
                #define GPIO_PADOUT_00_31_OFFSET                 0xc
        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_00_31_OFFSET              0x10
        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_00_31_OFFSET              0x14
        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_00_31_OFFSET                  0x18
        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_00_15_OFFSET                0x1c
        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_16_31_OFFSET                0x20
        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_00_31_OFFSET              0x24
        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_00_03_OFFSET                 0x28
        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_04_07_OFFSET                 0x2c
        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_08_11_OFFSET                 0x30
        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_12_15_OFFSET                 0x34
        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_16_19_OFFSET                 0x38
        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_20_23_OFFSET                 0x3c
        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_24_27_OFFSET                 0x40
        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_28_31_OFFSET                 0x44
        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_32_63_OFFSET                 0x48
        
                // GPIO enable register.
                #define GPIO_GPIOEN_32_63_OFFSET                 0x4c
        
                // GPIO pad input value register.
                #define GPIO_PADIN_32_63_OFFSET                  0x50
        
                // GPIO pad output value register.
                #define GPIO_PADOUT_32_63_OFFSET                 0x54
        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_32_63_OFFSET              0x58
        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_32_63_OFFSET              0x5c
        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_32_63_OFFSET                  0x60
        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_32_47_OFFSET                0x64
        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_48_63_OFFSET                0x68
        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_32_63_OFFSET              0x6c
        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_32_35_OFFSET                 0x70
        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_36_39_OFFSET                 0x74
        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_40_43_OFFSET                 0x78
        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_44_47_OFFSET                 0x7c
        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_48_51_OFFSET                 0x80
        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_52_55_OFFSET                 0x84
        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_56_59_OFFSET                 0x88
        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_60_63_OFFSET                 0x8c
        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_64_95_OFFSET                 0x90
        
                // GPIO enable register.
                #define GPIO_GPIOEN_64_95_OFFSET                 0x94
        
                // GPIO pad input value register.
                #define GPIO_PADIN_64_95_OFFSET                  0x98
        
                // GPIO pad output value register.
                #define GPIO_PADOUT_64_95_OFFSET                 0x9c
        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_64_95_OFFSET              0xa0
        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_64_95_OFFSET              0xa4
        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_64_95_OFFSET                  0xa8
        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_64_79_OFFSET                0xac
        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_80_95_OFFSET                0xb0
        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_64_95_OFFSET              0xb4
        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_64_67_OFFSET                 0xb8
        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_68_71_OFFSET                 0xbc
        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_72_75_OFFSET                 0xc0
        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_76_79_OFFSET                 0xc4
        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_80_83_OFFSET                 0xc8
        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_84_87_OFFSET                 0xcc
        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_88_91_OFFSET                 0xd0
        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_92_95_OFFSET                 0xd4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_paddir_00_31_get(uint32_t base);
        static inline void gpio_paddir_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_gpioen_00_31_get(uint32_t base);
        static inline void gpio_gpioen_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padin_00_31_get(uint32_t base);
        static inline void gpio_padin_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padout_00_31_get(uint32_t base);
        static inline void gpio_padout_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutset_00_31_get(uint32_t base);
        static inline void gpio_padoutset_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutclr_00_31_get(uint32_t base);
        static inline void gpio_padoutclr_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inten_00_31_get(uint32_t base);
        static inline void gpio_inten_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_00_15_get(uint32_t base);
        static inline void gpio_inttype_00_15_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_16_31_get(uint32_t base);
        static inline void gpio_inttype_16_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_intstatus_00_31_get(uint32_t base);
        static inline void gpio_intstatus_00_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_00_03_get(uint32_t base);
        static inline void gpio_padcfg_00_03_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_04_07_get(uint32_t base);
        static inline void gpio_padcfg_04_07_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_08_11_get(uint32_t base);
        static inline void gpio_padcfg_08_11_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_12_15_get(uint32_t base);
        static inline void gpio_padcfg_12_15_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_16_19_get(uint32_t base);
        static inline void gpio_padcfg_16_19_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_20_23_get(uint32_t base);
        static inline void gpio_padcfg_20_23_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_24_27_get(uint32_t base);
        static inline void gpio_padcfg_24_27_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_28_31_get(uint32_t base);
        static inline void gpio_padcfg_28_31_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_paddir_32_63_get(uint32_t base);
        static inline void gpio_paddir_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_gpioen_32_63_get(uint32_t base);
        static inline void gpio_gpioen_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padin_32_63_get(uint32_t base);
        static inline void gpio_padin_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padout_32_63_get(uint32_t base);
        static inline void gpio_padout_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutset_32_63_get(uint32_t base);
        static inline void gpio_padoutset_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutclr_32_63_get(uint32_t base);
        static inline void gpio_padoutclr_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inten_32_63_get(uint32_t base);
        static inline void gpio_inten_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_32_47_get(uint32_t base);
        static inline void gpio_inttype_32_47_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_48_63_get(uint32_t base);
        static inline void gpio_inttype_48_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_intstatus_32_63_get(uint32_t base);
        static inline void gpio_intstatus_32_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_32_35_get(uint32_t base);
        static inline void gpio_padcfg_32_35_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_36_39_get(uint32_t base);
        static inline void gpio_padcfg_36_39_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_40_43_get(uint32_t base);
        static inline void gpio_padcfg_40_43_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_44_47_get(uint32_t base);
        static inline void gpio_padcfg_44_47_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_48_51_get(uint32_t base);
        static inline void gpio_padcfg_48_51_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_52_55_get(uint32_t base);
        static inline void gpio_padcfg_52_55_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_56_59_get(uint32_t base);
        static inline void gpio_padcfg_56_59_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_60_63_get(uint32_t base);
        static inline void gpio_padcfg_60_63_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_paddir_64_95_get(uint32_t base);
        static inline void gpio_paddir_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_gpioen_64_95_get(uint32_t base);
        static inline void gpio_gpioen_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padin_64_95_get(uint32_t base);
        static inline void gpio_padin_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padout_64_95_get(uint32_t base);
        static inline void gpio_padout_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutset_64_95_get(uint32_t base);
        static inline void gpio_padoutset_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padoutclr_64_95_get(uint32_t base);
        static inline void gpio_padoutclr_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inten_64_95_get(uint32_t base);
        static inline void gpio_inten_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_64_79_get(uint32_t base);
        static inline void gpio_inttype_64_79_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_inttype_80_95_get(uint32_t base);
        static inline void gpio_inttype_80_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_intstatus_64_95_get(uint32_t base);
        static inline void gpio_intstatus_64_95_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_64_67_get(uint32_t base);
        static inline void gpio_padcfg_64_67_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_68_71_get(uint32_t base);
        static inline void gpio_padcfg_68_71_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_72_75_get(uint32_t base);
        static inline void gpio_padcfg_72_75_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_76_79_get(uint32_t base);
        static inline void gpio_padcfg_76_79_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_80_83_get(uint32_t base);
        static inline void gpio_padcfg_80_83_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_84_87_get(uint32_t base);
        static inline void gpio_padcfg_84_87_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_88_91_get(uint32_t base);
        static inline void gpio_padcfg_88_91_set(uint32_t base, uint32_t value);

        static inline uint32_t gpio_padcfg_92_95_get(uint32_t base);
        static inline void gpio_padcfg_92_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_00_31_PADDIR_BIT                                 0
        #define GPIO_PADDIR_00_31_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_00_31_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_00_31_PADDIR_RESET                               0x0
        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_00_31_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_00_31_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_00_31_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_00_31_GPIOEN_RESET                               0x0
        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_00_31_PADIN_BIT                                   0
        #define GPIO_PADIN_00_31_PADIN_WIDTH                                 32
        #define GPIO_PADIN_00_31_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_00_31_PADIN_RESET                                 0x0
        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_00_31_PADOUT_BIT                                 0
        #define GPIO_PADOUT_00_31_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_00_31_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_00_31_PADOUT_RESET                               0x0
        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_00_31_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_00_31_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_00_31_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_00_31_PADOUTSET_RESET                         0x0
        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_RESET                         0x0
        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_00_31_INTEN_BIT                                   0
        #define GPIO_INTEN_00_31_INTEN_WIDTH                                 32
        #define GPIO_INTEN_00_31_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_00_31_INTEN_RESET                                 0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_00_15_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_00_15_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_00_15_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_00_15_INTTYPE_RESET                             0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_16_31_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_16_31_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_16_31_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_16_31_INTTYPE_RESET                             0x0
        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_00_31_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_00_31_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_00_31_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_00_31_INTSTATUS_RESET                         0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_00_03_PADCFG_BIT                                 0
        #define GPIO_PADCFG_00_03_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_00_03_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_00_03_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_04_07_PADCFG_BIT                                 0
        #define GPIO_PADCFG_04_07_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_04_07_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_04_07_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_08_11_PADCFG_BIT                                 0
        #define GPIO_PADCFG_08_11_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_08_11_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_08_11_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_12_15_PADCFG_BIT                                 0
        #define GPIO_PADCFG_12_15_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_12_15_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_12_15_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_16_19_PADCFG_BIT                                 0
        #define GPIO_PADCFG_16_19_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_16_19_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_16_19_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_20_23_PADCFG_BIT                                 0
        #define GPIO_PADCFG_20_23_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_20_23_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_20_23_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_24_27_PADCFG_BIT                                 0
        #define GPIO_PADCFG_24_27_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_24_27_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_24_27_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_28_31_PADCFG_BIT                                 0
        #define GPIO_PADCFG_28_31_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_28_31_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_28_31_PADCFG_RESET                               0x0
        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_32_63_PADDIR_BIT                                 0
        #define GPIO_PADDIR_32_63_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_32_63_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_32_63_PADDIR_RESET                               0x0
        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_32_63_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_32_63_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_32_63_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_32_63_GPIOEN_RESET                               0x0
        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_32_63_PADIN_BIT                                   0
        #define GPIO_PADIN_32_63_PADIN_WIDTH                                 32
        #define GPIO_PADIN_32_63_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_32_63_PADIN_RESET                                 0x0
        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_32_63_PADOUT_BIT                                 0
        #define GPIO_PADOUT_32_63_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_32_63_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_32_63_PADOUT_RESET                               0x0
        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_32_63_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_32_63_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_32_63_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_32_63_PADOUTSET_RESET                         0x0
        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_RESET                         0x0
        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_32_63_INTEN_BIT                                   0
        #define GPIO_INTEN_32_63_INTEN_WIDTH                                 32
        #define GPIO_INTEN_32_63_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_32_63_INTEN_RESET                                 0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_32_47_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_32_47_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_32_47_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_32_47_INTTYPE_RESET                             0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_48_63_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_48_63_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_48_63_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_48_63_INTTYPE_RESET                             0x0
        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_32_63_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_32_63_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_32_63_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_32_63_INTSTATUS_RESET                         0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_32_35_PADCFG_BIT                                 0
        #define GPIO_PADCFG_32_35_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_32_35_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_32_35_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_36_39_PADCFG_BIT                                 0
        #define GPIO_PADCFG_36_39_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_36_39_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_36_39_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_40_43_PADCFG_BIT                                 0
        #define GPIO_PADCFG_40_43_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_40_43_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_40_43_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_44_47_PADCFG_BIT                                 0
        #define GPIO_PADCFG_44_47_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_44_47_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_44_47_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_48_51_PADCFG_BIT                                 0
        #define GPIO_PADCFG_48_51_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_48_51_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_48_51_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_52_55_PADCFG_BIT                                 0
        #define GPIO_PADCFG_52_55_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_52_55_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_52_55_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_56_59_PADCFG_BIT                                 0
        #define GPIO_PADCFG_56_59_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_56_59_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_56_59_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_60_63_PADCFG_BIT                                 0
        #define GPIO_PADCFG_60_63_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_60_63_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_60_63_PADCFG_RESET                               0x0
        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_64_95_PADDIR_BIT                                 0
        #define GPIO_PADDIR_64_95_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_64_95_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_64_95_PADDIR_RESET                               0x0
        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_64_95_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_64_95_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_64_95_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_64_95_GPIOEN_RESET                               0x0
        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_64_95_PADIN_BIT                                   0
        #define GPIO_PADIN_64_95_PADIN_WIDTH                                 32
        #define GPIO_PADIN_64_95_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_64_95_PADIN_RESET                                 0x0
        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_64_95_PADOUT_BIT                                 0
        #define GPIO_PADOUT_64_95_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_64_95_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_64_95_PADOUT_RESET                               0x0
        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_64_95_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_64_95_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_64_95_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_64_95_PADOUTSET_RESET                         0x0
        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_RESET                         0x0
        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_64_95_INTEN_BIT                                   0
        #define GPIO_INTEN_64_95_INTEN_WIDTH                                 32
        #define GPIO_INTEN_64_95_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_64_95_INTEN_RESET                                 0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_64_79_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_64_79_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_64_79_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_64_79_INTTYPE_RESET                             0x0
        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_80_95_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_80_95_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_80_95_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_80_95_INTTYPE_RESET                             0x0
        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_64_95_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_64_95_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_64_95_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_64_95_INTSTATUS_RESET                         0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_64_67_PADCFG_BIT                                 0
        #define GPIO_PADCFG_64_67_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_64_67_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_64_67_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_68_71_PADCFG_BIT                                 0
        #define GPIO_PADCFG_68_71_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_68_71_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_68_71_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_72_75_PADCFG_BIT                                 0
        #define GPIO_PADCFG_72_75_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_72_75_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_72_75_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_76_79_PADCFG_BIT                                 0
        #define GPIO_PADCFG_76_79_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_76_79_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_76_79_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_80_83_PADCFG_BIT                                 0
        #define GPIO_PADCFG_80_83_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_80_83_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_80_83_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_84_87_PADCFG_BIT                                 0
        #define GPIO_PADCFG_84_87_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_84_87_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_84_87_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_88_91_PADCFG_BIT                                 0
        #define GPIO_PADCFG_88_91_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_88_91_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_88_91_PADCFG_RESET                               0x0
        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_92_95_PADCFG_BIT                                 0
        #define GPIO_PADCFG_92_95_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_92_95_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_92_95_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADDIR_00_31_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_00_31_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_00_31_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_00_31_PADDIR(val)                      ((val) << 0)
        
        #define GPIO_GPIOEN_00_31_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN(val)                      ((val) << 0)
        
        #define GPIO_PADIN_00_31_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_00_31_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_00_31_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_00_31_PADIN(val)                        ((val) << 0)
        
        #define GPIO_PADOUT_00_31_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_00_31_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_00_31_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_00_31_PADOUT(val)                      ((val) << 0)
        
        #define GPIO_PADOUTSET_00_31_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET(val)                ((val) << 0)
        
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR(val)                ((val) << 0)
        
        #define GPIO_INTEN_00_31_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_00_31_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_00_31_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_00_31_INTEN(val)                        ((val) << 0)
        
        #define GPIO_INTTYPE_00_15_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTTYPE_16_31_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTSTATUS_00_31_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS(val)                ((val) << 0)
        
        #define GPIO_PADCFG_00_03_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_00_03_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_00_03_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_00_03_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_04_07_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_04_07_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_04_07_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_04_07_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_08_11_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_08_11_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_08_11_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_08_11_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_12_15_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_12_15_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_12_15_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_12_15_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_16_19_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_16_19_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_16_19_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_16_19_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_20_23_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_20_23_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_20_23_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_20_23_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_24_27_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_24_27_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_24_27_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_24_27_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_28_31_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_28_31_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_28_31_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_28_31_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADDIR_32_63_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_32_63_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_32_63_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_32_63_PADDIR(val)                      ((val) << 0)
        
        #define GPIO_GPIOEN_32_63_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN(val)                      ((val) << 0)
        
        #define GPIO_PADIN_32_63_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_32_63_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_32_63_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_32_63_PADIN(val)                        ((val) << 0)
        
        #define GPIO_PADOUT_32_63_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_32_63_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_32_63_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_32_63_PADOUT(val)                      ((val) << 0)
        
        #define GPIO_PADOUTSET_32_63_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET(val)                ((val) << 0)
        
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR(val)                ((val) << 0)
        
        #define GPIO_INTEN_32_63_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_32_63_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_32_63_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_32_63_INTEN(val)                        ((val) << 0)
        
        #define GPIO_INTTYPE_32_47_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTTYPE_48_63_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTSTATUS_32_63_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS(val)                ((val) << 0)
        
        #define GPIO_PADCFG_32_35_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_32_35_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_32_35_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_32_35_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_36_39_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_36_39_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_36_39_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_36_39_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_40_43_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_40_43_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_40_43_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_40_43_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_44_47_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_44_47_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_44_47_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_44_47_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_48_51_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_48_51_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_48_51_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_48_51_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_52_55_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_52_55_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_52_55_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_52_55_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_56_59_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_56_59_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_56_59_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_56_59_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_60_63_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_60_63_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_60_63_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_60_63_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADDIR_64_95_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_64_95_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_64_95_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_64_95_PADDIR(val)                      ((val) << 0)
        
        #define GPIO_GPIOEN_64_95_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN(val)                      ((val) << 0)
        
        #define GPIO_PADIN_64_95_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_64_95_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_64_95_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_64_95_PADIN(val)                        ((val) << 0)
        
        #define GPIO_PADOUT_64_95_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_64_95_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_64_95_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_64_95_PADOUT(val)                      ((val) << 0)
        
        #define GPIO_PADOUTSET_64_95_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET(val)                ((val) << 0)
        
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR(val)                ((val) << 0)
        
        #define GPIO_INTEN_64_95_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_64_95_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_64_95_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_64_95_INTEN(val)                        ((val) << 0)
        
        #define GPIO_INTTYPE_64_79_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTTYPE_80_95_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE(val)                    ((val) << 0)
        
        #define GPIO_INTSTATUS_64_95_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS(val)                ((val) << 0)
        
        #define GPIO_PADCFG_64_67_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_64_67_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_64_67_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_64_67_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_68_71_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_68_71_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_68_71_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_68_71_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_72_75_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_72_75_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_72_75_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_72_75_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_76_79_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_76_79_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_76_79_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_76_79_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_80_83_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_80_83_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_80_83_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_80_83_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_84_87_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_84_87_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_84_87_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_84_87_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_88_91_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_88_91_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_88_91_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_88_91_PADCFG(val)                      ((val) << 0)
        
        #define GPIO_PADCFG_92_95_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_92_95_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_92_95_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_92_95_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** GPIO_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t paddir_00_31;  // GPIO pad direction configuration register.
            volatile uint32_t gpioen_00_31;  // GPIO enable register.
            volatile uint32_t padin_00_31;  // GPIO pad input value register.
            volatile uint32_t padout_00_31;  // GPIO pad output value register.
            volatile uint32_t padoutset_00_31;  // GPIO pad output set register.
            volatile uint32_t padoutclr_00_31;  // GPIO pad output clear register.
            volatile uint32_t inten_00_31;  // GPIO pad interrupt enable configuration register.
            volatile uint32_t inttype_00_15;  // GPIO pad interrupt type gpio 0 to 15 register.
            volatile uint32_t inttype_16_31;  // GPIO pad interrupt type gpio 16 to 31 register.
            volatile uint32_t intstatus_00_31;  // GPIO pad interrupt status register.
            volatile uint32_t padcfg_00_03;  // GPIO pad pin 0 to 3 configuration register.
            volatile uint32_t padcfg_04_07;  // GPIO pad pin 4 to 7 configuration register.
            volatile uint32_t padcfg_08_11;  // GPIO pad pin 8 to 11 configuration register.
            volatile uint32_t padcfg_12_15;  // GPIO pad pin 12 to 15 configuration register.
            volatile uint32_t padcfg_16_19;  // GPIO pad pin 16 to 19 configuration register.
            volatile uint32_t padcfg_20_23;  // GPIO pad pin 20 to 23 configuration register.
            volatile uint32_t padcfg_24_27;  // GPIO pad pin 24 to 27 configuration register.
            volatile uint32_t padcfg_28_31;  // GPIO pad pin 28 to 31 configuration register.
            volatile uint32_t paddir_32_63;  // GPIO pad direction configuration register.
            volatile uint32_t gpioen_32_63;  // GPIO enable register.
            volatile uint32_t padin_32_63;  // GPIO pad input value register.
            volatile uint32_t padout_32_63;  // GPIO pad output value register.
            volatile uint32_t padoutset_32_63;  // GPIO pad output set register.
            volatile uint32_t padoutclr_32_63;  // GPIO pad output clear register.
            volatile uint32_t inten_32_63;  // GPIO pad interrupt enable configuration register.
            volatile uint32_t inttype_32_47;  // GPIO pad interrupt type gpio 0 to 15 register.
            volatile uint32_t inttype_48_63;  // GPIO pad interrupt type gpio 16 to 31 register.
            volatile uint32_t intstatus_32_63;  // GPIO pad interrupt status register.
            volatile uint32_t padcfg_32_35;  // GPIO pad pin 0 to 3 configuration register.
            volatile uint32_t padcfg_36_39;  // GPIO pad pin 4 to 7 configuration register.
            volatile uint32_t padcfg_40_43;  // GPIO pad pin 8 to 11 configuration register.
            volatile uint32_t padcfg_44_47;  // GPIO pad pin 12 to 15 configuration register.
            volatile uint32_t padcfg_48_51;  // GPIO pad pin 16 to 19 configuration register.
            volatile uint32_t padcfg_52_55;  // GPIO pad pin 20 to 23 configuration register.
            volatile uint32_t padcfg_56_59;  // GPIO pad pin 24 to 27 configuration register.
            volatile uint32_t padcfg_60_63;  // GPIO pad pin 28 to 31 configuration register.
            volatile uint32_t paddir_64_95;  // GPIO pad direction configuration register.
            volatile uint32_t gpioen_64_95;  // GPIO enable register.
            volatile uint32_t padin_64_95;  // GPIO pad input value register.
            volatile uint32_t padout_64_95;  // GPIO pad output value register.
            volatile uint32_t padoutset_64_95;  // GPIO pad output set register.
            volatile uint32_t padoutclr_64_95;  // GPIO pad output clear register.
            volatile uint32_t inten_64_95;  // GPIO pad interrupt enable configuration register.
            volatile uint32_t inttype_64_79;  // GPIO pad interrupt type gpio 0 to 15 register.
            volatile uint32_t inttype_80_95;  // GPIO pad interrupt type gpio 16 to 31 register.
            volatile uint32_t intstatus_64_95;  // GPIO pad interrupt status register.
            volatile uint32_t padcfg_64_67;  // GPIO pad pin 0 to 3 configuration register.
            volatile uint32_t padcfg_68_71;  // GPIO pad pin 4 to 7 configuration register.
            volatile uint32_t padcfg_72_75;  // GPIO pad pin 8 to 11 configuration register.
            volatile uint32_t padcfg_76_79;  // GPIO pad pin 12 to 15 configuration register.
            volatile uint32_t padcfg_80_83;  // GPIO pad pin 16 to 19 configuration register.
            volatile uint32_t padcfg_84_87;  // GPIO pad pin 20 to 23 configuration register.
            volatile uint32_t padcfg_88_91;  // GPIO pad pin 24 to 27 configuration register.
            volatile uint32_t padcfg_92_95;  // GPIO pad pin 28 to 31 configuration register.
        } __attribute__((packed)) gpio_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_00_31_t;
        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_00_31_t;
        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_00_31_t;
        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_00_31_t;
        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_00_31_t;
        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_00_31_t;
        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_00_31_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_00_15_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_16_31_t;
        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_00_31_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_00_03_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_04_07_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_08_11_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_12_15_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_16_19_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_20_23_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_24_27_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_28_31_t;
        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_32_63_t;
        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_32_63_t;
        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_32_63_t;
        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_32_63_t;
        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_32_63_t;
        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_32_63_t;
        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_32_63_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_32_47_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_48_63_t;
        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_32_63_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_32_35_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_36_39_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_40_43_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_44_47_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_48_51_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_52_55_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_56_59_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_60_63_t;
        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_64_95_t;
        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_64_95_t;
        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_64_95_t;
        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_64_95_t;
        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_64_95_t;
        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_64_95_t;
        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_64_95_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_64_79_t;
        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_80_95_t;
        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_64_95_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_64_67_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_68_71_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_72_75_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_76_79_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_80_83_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_84_87_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_88_91_t;
        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_92_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_gpio : public vp::regmap
        {
        public:
            vp_gpio_paddir_00_31 paddir_00_31;
            vp_gpio_gpioen_00_31 gpioen_00_31;
            vp_gpio_padin_00_31 padin_00_31;
            vp_gpio_padout_00_31 padout_00_31;
            vp_gpio_padoutset_00_31 padoutset_00_31;
            vp_gpio_padoutclr_00_31 padoutclr_00_31;
            vp_gpio_inten_00_31 inten_00_31;
            vp_gpio_inttype_00_15 inttype_00_15;
            vp_gpio_inttype_16_31 inttype_16_31;
            vp_gpio_intstatus_00_31 intstatus_00_31;
            vp_gpio_padcfg_00_03 padcfg_00_03;
            vp_gpio_padcfg_04_07 padcfg_04_07;
            vp_gpio_padcfg_08_11 padcfg_08_11;
            vp_gpio_padcfg_12_15 padcfg_12_15;
            vp_gpio_padcfg_16_19 padcfg_16_19;
            vp_gpio_padcfg_20_23 padcfg_20_23;
            vp_gpio_padcfg_24_27 padcfg_24_27;
            vp_gpio_padcfg_28_31 padcfg_28_31;
            vp_gpio_paddir_32_63 paddir_32_63;
            vp_gpio_gpioen_32_63 gpioen_32_63;
            vp_gpio_padin_32_63 padin_32_63;
            vp_gpio_padout_32_63 padout_32_63;
            vp_gpio_padoutset_32_63 padoutset_32_63;
            vp_gpio_padoutclr_32_63 padoutclr_32_63;
            vp_gpio_inten_32_63 inten_32_63;
            vp_gpio_inttype_32_47 inttype_32_47;
            vp_gpio_inttype_48_63 inttype_48_63;
            vp_gpio_intstatus_32_63 intstatus_32_63;
            vp_gpio_padcfg_32_35 padcfg_32_35;
            vp_gpio_padcfg_36_39 padcfg_36_39;
            vp_gpio_padcfg_40_43 padcfg_40_43;
            vp_gpio_padcfg_44_47 padcfg_44_47;
            vp_gpio_padcfg_48_51 padcfg_48_51;
            vp_gpio_padcfg_52_55 padcfg_52_55;
            vp_gpio_padcfg_56_59 padcfg_56_59;
            vp_gpio_padcfg_60_63 padcfg_60_63;
            vp_gpio_paddir_64_95 paddir_64_95;
            vp_gpio_gpioen_64_95 gpioen_64_95;
            vp_gpio_padin_64_95 padin_64_95;
            vp_gpio_padout_64_95 padout_64_95;
            vp_gpio_padoutset_64_95 padoutset_64_95;
            vp_gpio_padoutclr_64_95 padoutclr_64_95;
            vp_gpio_inten_64_95 inten_64_95;
            vp_gpio_inttype_64_79 inttype_64_79;
            vp_gpio_inttype_80_95 inttype_80_95;
            vp_gpio_intstatus_64_95 intstatus_64_95;
            vp_gpio_padcfg_64_67 padcfg_64_67;
            vp_gpio_padcfg_68_71 padcfg_68_71;
            vp_gpio_padcfg_72_75 padcfg_72_75;
            vp_gpio_padcfg_76_79 padcfg_76_79;
            vp_gpio_padcfg_80_83 padcfg_80_83;
            vp_gpio_padcfg_84_87 padcfg_84_87;
            vp_gpio_padcfg_88_91 padcfg_88_91;
            vp_gpio_padcfg_92_95 padcfg_92_95;
        };

|

.. _gpio_PADDIR_00_31:

PADDIR_00_31
""""""""""""

GPIO pad direction configuration register.

.. table:: 

    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                       Description                                                       |
    +=====+===+======+=========================================================================================================================+
    |31:0 |R/W|PADDIR|GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]|
    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_00_31_OFFSET                 0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_paddir_00_31_get(uint32_t base);
        static inline void gpio_paddir_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_00_31_PADDIR_BIT                                 0
        #define GPIO_PADDIR_00_31_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_00_31_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_00_31_PADDIR_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADDIR_00_31_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_00_31_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_00_31_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_00_31_PADDIR(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_paddir_00_31 : public vp::reg_32
        {
        public:
            inline void paddir_set(uint32_t value);
            inline uint32_t paddir_get();
        };

|

.. _gpio_GPIOEN_00_31:

GPIOEN_00_31
""""""""""""

GPIO enable register.

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                                        Description                                                                                                                                                         |
    +=====+===+======+============================================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|GPIOEN|GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.|
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO enable register.
                #define GPIO_GPIOEN_00_31_OFFSET                 0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_gpioen_00_31_get(uint32_t base);
        static inline void gpio_gpioen_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_00_31_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_00_31_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_00_31_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_00_31_GPIOEN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_GPIOEN_00_31_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_00_31_GPIOEN(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_gpioen_00_31 : public vp::reg_32
        {
        public:
            inline void gpioen_set(uint32_t value);
            inline uint32_t gpioen_get();
        };

|

.. _gpio_PADIN_00_31:

PADIN_00_31
"""""""""""

GPIO pad input value register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |31:0 |R  |PADIN|GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].|
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad input value register.
                #define GPIO_PADIN_00_31_OFFSET                  0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padin_00_31_get(uint32_t base);
        static inline void gpio_padin_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_00_31_PADIN_BIT                                   0
        #define GPIO_PADIN_00_31_PADIN_WIDTH                                 32
        #define GPIO_PADIN_00_31_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_00_31_PADIN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADIN_00_31_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_00_31_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_00_31_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_00_31_PADIN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padin_00_31 : public vp::reg_32
        {
        public:
            inline void padin_set(uint32_t value);
            inline uint32_t padin_get();
        };

|

.. _gpio_PADOUT_00_31:

PADOUT_00_31
""""""""""""

GPIO pad output value register.

.. table:: 

    +-----+---+------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                        Description                                         |
    +=====+===+======+============================================================================================+
    |31:0 |R/W|PADOUT|GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].|
    +-----+---+------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output value register.
                #define GPIO_PADOUT_00_31_OFFSET                 0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padout_00_31_get(uint32_t base);
        static inline void gpio_padout_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_00_31_PADOUT_BIT                                 0
        #define GPIO_PADOUT_00_31_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_00_31_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_00_31_PADOUT_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUT_00_31_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_00_31_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_00_31_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_00_31_PADOUT(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padout_00_31 : public vp::reg_32
        {
        public:
            inline void padout_set(uint32_t value);
            inline uint32_t padout_get();
        };

|

.. _gpio_PADOUTSET_00_31:

PADOUTSET_00_31
"""""""""""""""

GPIO pad output set register.

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                          |
    +=====+===+=========+==============================================================================================+
    |31:0 |W  |PADOUTSET|GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1|
    +-----+---+---------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_00_31_OFFSET              0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutset_00_31_get(uint32_t base);
        static inline void gpio_padoutset_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_00_31_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_00_31_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_00_31_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_00_31_PADOUTSET_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTSET_00_31_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_00_31_PADOUTSET(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutset_00_31 : public vp::reg_32
        {
        public:
            inline void padoutset_set(uint32_t value);
            inline uint32_t padoutset_get();
        };

|

.. _gpio_PADOUTCLR_00_31:

PADOUTCLR_00_31
"""""""""""""""

GPIO pad output clear register.

.. table:: 

    +-----+---+---------+---------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                         |
    +=====+===+=========+=============================================================================================+
    |31:0 |W  |PADOUTCLR|GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]|
    +-----+---+---------+---------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_00_31_OFFSET              0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutclr_00_31_get(uint32_t base);
        static inline void gpio_padoutclr_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_00_31_PADOUTCLR(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutclr_00_31 : public vp::reg_32
        {
        public:
            inline void padoutclr_set(uint32_t value);
            inline uint32_t padoutclr_get();
        };

|

.. _gpio_INTEN_00_31:

INTEN_00_31
"""""""""""

GPIO pad interrupt enable configuration register.

.. table:: 

    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                Description                                                                 |
    +=====+===+=====+============================================================================================================================================+
    |31:0 |R/W|INTEN|GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]|
    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_00_31_OFFSET                  0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inten_00_31_get(uint32_t base);
        static inline void gpio_inten_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_00_31_INTEN_BIT                                   0
        #define GPIO_INTEN_00_31_INTEN_WIDTH                                 32
        #define GPIO_INTEN_00_31_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_00_31_INTEN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTEN_00_31_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_00_31_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_00_31_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_00_31_INTEN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inten_00_31 : public vp::reg_32
        {
        public:
            inline void inten_set(uint32_t value);
            inline uint32_t inten_get();
        };

|

.. _gpio_INTTYPE_00_15:

INTTYPE_00_15
"""""""""""""

GPIO pad interrupt type gpio 0 to 15 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_00_15_OFFSET                0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_00_15_get(uint32_t base);
        static inline void gpio_inttype_00_15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_00_15_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_00_15_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_00_15_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_00_15_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_00_15_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_00_15_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_00_15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_00_15 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTTYPE_16_31:

INTTYPE_16_31
"""""""""""""

GPIO pad interrupt type gpio 16 to 31 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_16_31_OFFSET                0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_16_31_get(uint32_t base);
        static inline void gpio_inttype_16_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_16_31_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_16_31_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_16_31_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_16_31_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_16_31_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_16_31_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_16_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_16_31 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTSTATUS_00_31:

INTSTATUS_00_31
"""""""""""""""

GPIO pad interrupt status register.

.. table:: 

    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                              Description                                                                                              |
    +=====+===+=========+=======================================================================================================================================================================================================+
    |31:0 |R  |INTSTATUS|GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.|
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_00_31_OFFSET              0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_intstatus_00_31_get(uint32_t base);
        static inline void gpio_intstatus_00_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_00_31_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_00_31_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_00_31_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_00_31_INTSTATUS_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTSTATUS_00_31_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_00_31_INTSTATUS(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_00_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_intstatus_00_31 : public vp::reg_32
        {
        public:
            inline void intstatus_set(uint32_t value);
            inline uint32_t intstatus_get();
        };

|

.. _gpio_PADCFG_00_03:

PADCFG_00_03
""""""""""""

GPIO pad pin 0 to 3 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_00_03_OFFSET                 0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_00_03_get(uint32_t base);
        static inline void gpio_padcfg_00_03_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_00_03_PADCFG_BIT                                 0
        #define GPIO_PADCFG_00_03_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_00_03_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_00_03_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_00_03_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_00_03_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_00_03_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_00_03_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_00_03_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_00_03 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_04_07:

PADCFG_04_07
""""""""""""

GPIO pad pin 4 to 7 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_04_07_OFFSET                 0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_04_07_get(uint32_t base);
        static inline void gpio_padcfg_04_07_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_04_07_PADCFG_BIT                                 0
        #define GPIO_PADCFG_04_07_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_04_07_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_04_07_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_04_07_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_04_07_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_04_07_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_04_07_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_04_07_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_04_07 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_08_11:

PADCFG_08_11
""""""""""""

GPIO pad pin 8 to 11 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_08_11_OFFSET                 0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_08_11_get(uint32_t base);
        static inline void gpio_padcfg_08_11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_08_11_PADCFG_BIT                                 0
        #define GPIO_PADCFG_08_11_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_08_11_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_08_11_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_08_11_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_08_11_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_08_11_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_08_11_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_08_11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_08_11 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_12_15:

PADCFG_12_15
""""""""""""

GPIO pad pin 12 to 15 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_12_15_OFFSET                 0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_12_15_get(uint32_t base);
        static inline void gpio_padcfg_12_15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_12_15_PADCFG_BIT                                 0
        #define GPIO_PADCFG_12_15_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_12_15_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_12_15_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_12_15_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_12_15_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_12_15_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_12_15_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_12_15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_12_15 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_16_19:

PADCFG_16_19
""""""""""""

GPIO pad pin 16 to 19 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_16_19_OFFSET                 0x38

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_16_19_get(uint32_t base);
        static inline void gpio_padcfg_16_19_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_16_19_PADCFG_BIT                                 0
        #define GPIO_PADCFG_16_19_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_16_19_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_16_19_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_16_19_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_16_19_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_16_19_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_16_19_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_16_19_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_16_19 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_20_23:

PADCFG_20_23
""""""""""""

GPIO pad pin 20 to 23 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_20_23_OFFSET                 0x3c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_20_23_get(uint32_t base);
        static inline void gpio_padcfg_20_23_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_20_23_PADCFG_BIT                                 0
        #define GPIO_PADCFG_20_23_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_20_23_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_20_23_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_20_23_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_20_23_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_20_23_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_20_23_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_20_23_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_20_23 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_24_27:

PADCFG_24_27
""""""""""""

GPIO pad pin 24 to 27 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_24_27_OFFSET                 0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_24_27_get(uint32_t base);
        static inline void gpio_padcfg_24_27_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_24_27_PADCFG_BIT                                 0
        #define GPIO_PADCFG_24_27_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_24_27_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_24_27_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_24_27_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_24_27_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_24_27_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_24_27_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_24_27_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_24_27 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_28_31:

PADCFG_28_31
""""""""""""

GPIO pad pin 28 to 31 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_28_31_OFFSET                 0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_28_31_get(uint32_t base);
        static inline void gpio_padcfg_28_31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_28_31_PADCFG_BIT                                 0
        #define GPIO_PADCFG_28_31_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_28_31_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_28_31_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_28_31_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_28_31_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_28_31_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_28_31_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_28_31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_28_31 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADDIR_32_63:

PADDIR_32_63
""""""""""""

GPIO pad direction configuration register.

.. table:: 

    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                       Description                                                       |
    +=====+===+======+=========================================================================================================================+
    |31:0 |R/W|PADDIR|GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]|
    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_32_63_OFFSET                 0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_paddir_32_63_get(uint32_t base);
        static inline void gpio_paddir_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_32_63_PADDIR_BIT                                 0
        #define GPIO_PADDIR_32_63_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_32_63_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_32_63_PADDIR_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADDIR_32_63_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_32_63_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_32_63_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_32_63_PADDIR(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_paddir_32_63 : public vp::reg_32
        {
        public:
            inline void paddir_set(uint32_t value);
            inline uint32_t paddir_get();
        };

|

.. _gpio_GPIOEN_32_63:

GPIOEN_32_63
""""""""""""

GPIO enable register.

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                                        Description                                                                                                                                                         |
    +=====+===+======+============================================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|GPIOEN|GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.|
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO enable register.
                #define GPIO_GPIOEN_32_63_OFFSET                 0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_gpioen_32_63_get(uint32_t base);
        static inline void gpio_gpioen_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_32_63_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_32_63_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_32_63_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_32_63_GPIOEN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_GPIOEN_32_63_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_32_63_GPIOEN(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_gpioen_32_63 : public vp::reg_32
        {
        public:
            inline void gpioen_set(uint32_t value);
            inline uint32_t gpioen_get();
        };

|

.. _gpio_PADIN_32_63:

PADIN_32_63
"""""""""""

GPIO pad input value register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |31:0 |R  |PADIN|GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].|
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad input value register.
                #define GPIO_PADIN_32_63_OFFSET                  0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padin_32_63_get(uint32_t base);
        static inline void gpio_padin_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_32_63_PADIN_BIT                                   0
        #define GPIO_PADIN_32_63_PADIN_WIDTH                                 32
        #define GPIO_PADIN_32_63_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_32_63_PADIN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADIN_32_63_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_32_63_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_32_63_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_32_63_PADIN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padin_32_63 : public vp::reg_32
        {
        public:
            inline void padin_set(uint32_t value);
            inline uint32_t padin_get();
        };

|

.. _gpio_PADOUT_32_63:

PADOUT_32_63
""""""""""""

GPIO pad output value register.

.. table:: 

    +-----+---+------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                        Description                                         |
    +=====+===+======+============================================================================================+
    |31:0 |R/W|PADOUT|GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].|
    +-----+---+------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output value register.
                #define GPIO_PADOUT_32_63_OFFSET                 0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padout_32_63_get(uint32_t base);
        static inline void gpio_padout_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_32_63_PADOUT_BIT                                 0
        #define GPIO_PADOUT_32_63_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_32_63_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_32_63_PADOUT_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUT_32_63_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_32_63_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_32_63_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_32_63_PADOUT(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padout_32_63 : public vp::reg_32
        {
        public:
            inline void padout_set(uint32_t value);
            inline uint32_t padout_get();
        };

|

.. _gpio_PADOUTSET_32_63:

PADOUTSET_32_63
"""""""""""""""

GPIO pad output set register.

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                          |
    +=====+===+=========+==============================================================================================+
    |31:0 |W  |PADOUTSET|GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1|
    +-----+---+---------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_32_63_OFFSET              0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutset_32_63_get(uint32_t base);
        static inline void gpio_padoutset_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_32_63_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_32_63_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_32_63_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_32_63_PADOUTSET_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTSET_32_63_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_32_63_PADOUTSET(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutset_32_63 : public vp::reg_32
        {
        public:
            inline void padoutset_set(uint32_t value);
            inline uint32_t padoutset_get();
        };

|

.. _gpio_PADOUTCLR_32_63:

PADOUTCLR_32_63
"""""""""""""""

GPIO pad output clear register.

.. table:: 

    +-----+---+---------+---------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                         |
    +=====+===+=========+=============================================================================================+
    |31:0 |W  |PADOUTCLR|GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]|
    +-----+---+---------+---------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_32_63_OFFSET              0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutclr_32_63_get(uint32_t base);
        static inline void gpio_padoutclr_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_32_63_PADOUTCLR(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutclr_32_63 : public vp::reg_32
        {
        public:
            inline void padoutclr_set(uint32_t value);
            inline uint32_t padoutclr_get();
        };

|

.. _gpio_INTEN_32_63:

INTEN_32_63
"""""""""""

GPIO pad interrupt enable configuration register.

.. table:: 

    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                Description                                                                 |
    +=====+===+=====+============================================================================================================================================+
    |31:0 |R/W|INTEN|GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]|
    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_32_63_OFFSET                  0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inten_32_63_get(uint32_t base);
        static inline void gpio_inten_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_32_63_INTEN_BIT                                   0
        #define GPIO_INTEN_32_63_INTEN_WIDTH                                 32
        #define GPIO_INTEN_32_63_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_32_63_INTEN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTEN_32_63_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_32_63_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_32_63_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_32_63_INTEN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inten_32_63 : public vp::reg_32
        {
        public:
            inline void inten_set(uint32_t value);
            inline uint32_t inten_get();
        };

|

.. _gpio_INTTYPE_32_47:

INTTYPE_32_47
"""""""""""""

GPIO pad interrupt type gpio 0 to 15 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_32_47_OFFSET                0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_32_47_get(uint32_t base);
        static inline void gpio_inttype_32_47_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_32_47_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_32_47_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_32_47_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_32_47_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_32_47_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_32_47_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_32_47_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_32_47 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTTYPE_48_63:

INTTYPE_48_63
"""""""""""""

GPIO pad interrupt type gpio 16 to 31 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_48_63_OFFSET                0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_48_63_get(uint32_t base);
        static inline void gpio_inttype_48_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_48_63_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_48_63_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_48_63_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_48_63_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_48_63_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_48_63_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_48_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_48_63 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTSTATUS_32_63:

INTSTATUS_32_63
"""""""""""""""

GPIO pad interrupt status register.

.. table:: 

    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                              Description                                                                                              |
    +=====+===+=========+=======================================================================================================================================================================================================+
    |31:0 |R  |INTSTATUS|GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.|
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_32_63_OFFSET              0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_intstatus_32_63_get(uint32_t base);
        static inline void gpio_intstatus_32_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_32_63_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_32_63_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_32_63_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_32_63_INTSTATUS_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTSTATUS_32_63_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_32_63_INTSTATUS(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_32_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_intstatus_32_63 : public vp::reg_32
        {
        public:
            inline void intstatus_set(uint32_t value);
            inline uint32_t intstatus_get();
        };

|

.. _gpio_PADCFG_32_35:

PADCFG_32_35
""""""""""""

GPIO pad pin 0 to 3 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_32_35_OFFSET                 0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_32_35_get(uint32_t base);
        static inline void gpio_padcfg_32_35_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_32_35_PADCFG_BIT                                 0
        #define GPIO_PADCFG_32_35_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_32_35_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_32_35_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_32_35_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_32_35_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_32_35_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_32_35_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_32_35_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_32_35 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_36_39:

PADCFG_36_39
""""""""""""

GPIO pad pin 4 to 7 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_36_39_OFFSET                 0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_36_39_get(uint32_t base);
        static inline void gpio_padcfg_36_39_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_36_39_PADCFG_BIT                                 0
        #define GPIO_PADCFG_36_39_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_36_39_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_36_39_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_36_39_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_36_39_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_36_39_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_36_39_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_36_39_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_36_39 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_40_43:

PADCFG_40_43
""""""""""""

GPIO pad pin 8 to 11 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_40_43_OFFSET                 0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_40_43_get(uint32_t base);
        static inline void gpio_padcfg_40_43_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_40_43_PADCFG_BIT                                 0
        #define GPIO_PADCFG_40_43_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_40_43_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_40_43_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_40_43_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_40_43_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_40_43_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_40_43_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_40_43_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_40_43 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_44_47:

PADCFG_44_47
""""""""""""

GPIO pad pin 12 to 15 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_44_47_OFFSET                 0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_44_47_get(uint32_t base);
        static inline void gpio_padcfg_44_47_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_44_47_PADCFG_BIT                                 0
        #define GPIO_PADCFG_44_47_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_44_47_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_44_47_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_44_47_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_44_47_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_44_47_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_44_47_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_44_47_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_44_47 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_48_51:

PADCFG_48_51
""""""""""""

GPIO pad pin 16 to 19 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_48_51_OFFSET                 0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_48_51_get(uint32_t base);
        static inline void gpio_padcfg_48_51_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_48_51_PADCFG_BIT                                 0
        #define GPIO_PADCFG_48_51_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_48_51_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_48_51_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_48_51_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_48_51_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_48_51_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_48_51_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_48_51_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_48_51 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_52_55:

PADCFG_52_55
""""""""""""

GPIO pad pin 20 to 23 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_52_55_OFFSET                 0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_52_55_get(uint32_t base);
        static inline void gpio_padcfg_52_55_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_52_55_PADCFG_BIT                                 0
        #define GPIO_PADCFG_52_55_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_52_55_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_52_55_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_52_55_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_52_55_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_52_55_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_52_55_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_52_55_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_52_55 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_56_59:

PADCFG_56_59
""""""""""""

GPIO pad pin 24 to 27 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_56_59_OFFSET                 0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_56_59_get(uint32_t base);
        static inline void gpio_padcfg_56_59_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_56_59_PADCFG_BIT                                 0
        #define GPIO_PADCFG_56_59_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_56_59_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_56_59_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_56_59_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_56_59_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_56_59_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_56_59_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_56_59_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_56_59 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_60_63:

PADCFG_60_63
""""""""""""

GPIO pad pin 28 to 31 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_60_63_OFFSET                 0x8c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_60_63_get(uint32_t base);
        static inline void gpio_padcfg_60_63_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_60_63_PADCFG_BIT                                 0
        #define GPIO_PADCFG_60_63_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_60_63_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_60_63_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_60_63_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_60_63_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_60_63_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_60_63_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_60_63_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_60_63 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADDIR_64_95:

PADDIR_64_95
""""""""""""

GPIO pad direction configuration register.

.. table:: 

    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                       Description                                                       |
    +=====+===+======+=========================================================================================================================+
    |31:0 |R/W|PADDIR|GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]|
    +-----+---+------+-------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad direction configuration register.
                #define GPIO_PADDIR_64_95_OFFSET                 0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_paddir_64_95_get(uint32_t base);
        static inline void gpio_paddir_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i] (access: R/W)
        #define GPIO_PADDIR_64_95_PADDIR_BIT                                 0
        #define GPIO_PADDIR_64_95_PADDIR_WIDTH                               32
        #define GPIO_PADDIR_64_95_PADDIR_MASK                                0xffffffff
        #define GPIO_PADDIR_64_95_PADDIR_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADDIR_64_95_PADDIR_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADDIR_64_95_PADDIR_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADDIR_64_95_PADDIR_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADDIR_64_95_PADDIR(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int paddir          :32; // GPIO[31:0] direction configuration bitfield: - bit[i]=1'b0: Input mode for GPIO[i] - bit[i]=1'b1: Output mode for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_paddir_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_paddir_64_95 : public vp::reg_32
        {
        public:
            inline void paddir_set(uint32_t value);
            inline uint32_t paddir_get();
        };

|

.. _gpio_GPIOEN_64_95:

GPIOEN_64_95
""""""""""""

GPIO enable register.

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                                        Description                                                                                                                                                         |
    +=====+===+======+============================================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|GPIOEN|GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.|
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO enable register.
                #define GPIO_GPIOEN_64_95_OFFSET                 0x94

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_gpioen_64_95_get(uint32_t base);
        static inline void gpio_gpioen_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode. (access: R/W)
        #define GPIO_GPIOEN_64_95_GPIOEN_BIT                                 0
        #define GPIO_GPIOEN_64_95_GPIOEN_WIDTH                               32
        #define GPIO_GPIOEN_64_95_GPIOEN_MASK                                0xffffffff
        #define GPIO_GPIOEN_64_95_GPIOEN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_GPIOEN_64_95_GPIOEN_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_GPIOEN_64_95_GPIOEN(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpioen          :32; // GPIO[31:0] clock enable configuration bitfield: - bit[i]=1'b0: disable clock for GPIO[i] - bit[i]=1'b1: enable clock for GPIO[i] GPIOs are gathered by groups of 4. The clock gating of one group is done only if all 4 GPIOs are disabled.  Clock must be enabled for a GPIO if it's direction is configured in input mode.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_gpioen_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_gpioen_64_95 : public vp::reg_32
        {
        public:
            inline void gpioen_set(uint32_t value);
            inline uint32_t gpioen_get();
        };

|

.. _gpio_PADIN_64_95:

PADIN_64_95
"""""""""""

GPIO pad input value register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |31:0 |R  |PADIN|GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].|
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad input value register.
                #define GPIO_PADIN_64_95_OFFSET                  0x98

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padin_64_95_get(uint32_t base);
        static inline void gpio_padin_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i]. (access: R)
        #define GPIO_PADIN_64_95_PADIN_BIT                                   0
        #define GPIO_PADIN_64_95_PADIN_WIDTH                                 32
        #define GPIO_PADIN_64_95_PADIN_MASK                                  0xffffffff
        #define GPIO_PADIN_64_95_PADIN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADIN_64_95_PADIN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADIN_64_95_PADIN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADIN_64_95_PADIN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADIN_64_95_PADIN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padin           :32; // GPIO[31:0] input data read bitfield. DATA_IN[i] corresponds to input data of GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padin_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padin_64_95 : public vp::reg_32
        {
        public:
            inline void padin_set(uint32_t value);
            inline uint32_t padin_get();
        };

|

.. _gpio_PADOUT_64_95:

PADOUT_64_95
""""""""""""

GPIO pad output value register.

.. table:: 

    +-----+---+------+--------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                        Description                                         |
    +=====+===+======+============================================================================================+
    |31:0 |R/W|PADOUT|GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].|
    +-----+---+------+--------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output value register.
                #define GPIO_PADOUT_64_95_OFFSET                 0x9c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padout_64_95_get(uint32_t base);
        static inline void gpio_padout_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i]. (access: R/W)
        #define GPIO_PADOUT_64_95_PADOUT_BIT                                 0
        #define GPIO_PADOUT_64_95_PADOUT_WIDTH                               32
        #define GPIO_PADOUT_64_95_PADOUT_MASK                                0xffffffff
        #define GPIO_PADOUT_64_95_PADOUT_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUT_64_95_PADOUT_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUT_64_95_PADOUT_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUT_64_95_PADOUT_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUT_64_95_PADOUT(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padout          :32; // GPIO[31:0] output data read bitfield. DATA_OUT[i] corresponds to output data set on GPIO[i].
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padout_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padout_64_95 : public vp::reg_32
        {
        public:
            inline void padout_set(uint32_t value);
            inline uint32_t padout_get();
        };

|

.. _gpio_PADOUTSET_64_95:

PADOUTSET_64_95
"""""""""""""""

GPIO pad output set register.

.. table:: 

    +-----+---+---------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                          |
    +=====+===+=========+==============================================================================================+
    |31:0 |W  |PADOUTSET|GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1|
    +-----+---+---------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output set register.
                #define GPIO_PADOUTSET_64_95_OFFSET              0xa0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutset_64_95_get(uint32_t base);
        static inline void gpio_padoutset_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1 (access: W)
        #define GPIO_PADOUTSET_64_95_PADOUTSET_BIT                           0
        #define GPIO_PADOUTSET_64_95_PADOUTSET_WIDTH                         32
        #define GPIO_PADOUTSET_64_95_PADOUTSET_MASK                          0xffffffff
        #define GPIO_PADOUTSET_64_95_PADOUTSET_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTSET_64_95_PADOUTSET_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTSET_64_95_PADOUTSET(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutset       :32; // GPIO[31:0] set bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Sets GPIO[i] to 1
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutset_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutset_64_95 : public vp::reg_32
        {
        public:
            inline void padoutset_set(uint32_t value);
            inline uint32_t padoutset_get();
        };

|

.. _gpio_PADOUTCLR_64_95:

PADOUTCLR_64_95
"""""""""""""""

GPIO pad output clear register.

.. table:: 

    +-----+---+---------+---------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                         Description                                         |
    +=====+===+=========+=============================================================================================+
    |31:0 |W  |PADOUTCLR|GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]|
    +-----+---+---------+---------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad output clear register.
                #define GPIO_PADOUTCLR_64_95_OFFSET              0xa4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padoutclr_64_95_get(uint32_t base);
        static inline void gpio_padoutclr_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i] (access: W)
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_BIT                           0
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_WIDTH                         32
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_MASK                          0xffffffff
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_PADOUTCLR_64_95_PADOUTCLR(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padoutclr       :32; // GPIO[31:0] clear bitfield: - bit[i]=1'b0: No change for GPIO[i] - bit[i]=1'b1: Clears GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padoutclr_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padoutclr_64_95 : public vp::reg_32
        {
        public:
            inline void padoutclr_set(uint32_t value);
            inline uint32_t padoutclr_get();
        };

|

.. _gpio_INTEN_64_95:

INTEN_64_95
"""""""""""

GPIO pad interrupt enable configuration register.

.. table:: 

    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                Description                                                                 |
    +=====+===+=====+============================================================================================================================================+
    |31:0 |R/W|INTEN|GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]|
    +-----+---+-----+--------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt enable configuration register.
                #define GPIO_INTEN_64_95_OFFSET                  0xa8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inten_64_95_get(uint32_t base);
        static inline void gpio_inten_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i] (access: R/W)
        #define GPIO_INTEN_64_95_INTEN_BIT                                   0
        #define GPIO_INTEN_64_95_INTEN_WIDTH                                 32
        #define GPIO_INTEN_64_95_INTEN_MASK                                  0xffffffff
        #define GPIO_INTEN_64_95_INTEN_RESET                                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTEN_64_95_INTEN_GET(value)                  (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTEN_64_95_INTEN_GETS(value)                 (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTEN_64_95_INTEN_SET(value,field)            (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTEN_64_95_INTEN(val)                        ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inten           :32; // GPIO[31:0] interrupt enable configuration bitfield: - bit[i]=1'b0: disable interrupt for GPIO[i] - bit[i]=1'b1: enable interrupt for GPIO[i]
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inten_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inten_64_95 : public vp::reg_32
        {
        public:
            inline void inten_set(uint32_t value);
            inline uint32_t inten_get();
        };

|

.. _gpio_INTTYPE_64_79:

INTTYPE_64_79
"""""""""""""

GPIO pad interrupt type gpio 0 to 15 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 0 to 15 register.
                #define GPIO_INTTYPE_64_79_OFFSET                0xac

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_64_79_get(uint32_t base);
        static inline void gpio_inttype_64_79_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_64_79_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_64_79_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_64_79_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_64_79_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_64_79_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_64_79_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_64_79_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_64_79 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTTYPE_80_95:

INTTYPE_80_95
"""""""""""""

GPIO pad interrupt type gpio 16 to 31 register.

.. table:: 

    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                                                                 Description                                                                                                                                                 |
    +=====+===+=======+=============================================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|INTTYPE|GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU|
    +-----+---+-------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt type gpio 16 to 31 register.
                #define GPIO_INTTYPE_80_95_OFFSET                0xb0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_inttype_80_95_get(uint32_t base);
        static inline void gpio_inttype_80_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU (access: R/W)
        #define GPIO_INTTYPE_80_95_INTTYPE_BIT                               0
        #define GPIO_INTTYPE_80_95_INTTYPE_WIDTH                             32
        #define GPIO_INTTYPE_80_95_INTTYPE_MASK                              0xffffffff
        #define GPIO_INTTYPE_80_95_INTTYPE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTTYPE_80_95_INTTYPE_GET(value)              (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE_GETS(value)             (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE_SET(value,field)        (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTTYPE_80_95_INTTYPE(val)                    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int inttype         :32; // GPIO[15:0] interrupt type configuration bitfield: - bit[2<em>i+1:2</em>i]=2'b00: interrupt on falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b01: interrupt on rising edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b10: interrupt on rising and falling edge for GPIO[i] - bit[2<em>i+1:2</em>i]=2'b11: RFU
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_inttype_80_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_inttype_80_95 : public vp::reg_32
        {
        public:
            inline void inttype_set(uint32_t value);
            inline uint32_t inttype_get();
        };

|

.. _gpio_INTSTATUS_64_95:

INTSTATUS_64_95
"""""""""""""""

GPIO pad interrupt status register.

.. table:: 

    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                              Description                                                                                              |
    +=====+===+=========+=======================================================================================================================================================================================================+
    |31:0 |R  |INTSTATUS|GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.|
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad interrupt status register.
                #define GPIO_INTSTATUS_64_95_OFFSET              0xb4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_intstatus_64_95_get(uint32_t base);
        static inline void gpio_intstatus_64_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red. (access: R)
        #define GPIO_INTSTATUS_64_95_INTSTATUS_BIT                           0
        #define GPIO_INTSTATUS_64_95_INTSTATUS_WIDTH                         32
        #define GPIO_INTSTATUS_64_95_INTSTATUS_MASK                          0xffffffff
        #define GPIO_INTSTATUS_64_95_INTSTATUS_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_INTSTATUS_64_95_INTSTATUS_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define GPIO_INTSTATUS_64_95_INTSTATUS(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int intstatus       :32; // GPIO[31:0] Interrupt status flags bitfield. INTSTATUS[i]=1 when interrupt received on GPIO[i]. INTSTATUS is cleared when it is red. GPIO interrupt line is also cleared when INTSTATUS register is red.
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_intstatus_64_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_intstatus_64_95 : public vp::reg_32
        {
        public:
            inline void intstatus_set(uint32_t value);
            inline uint32_t intstatus_get();
        };

|

.. _gpio_PADCFG_64_67:

PADCFG_64_67
""""""""""""

GPIO pad pin 0 to 3 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 0 to 3 configuration register.
                #define GPIO_PADCFG_64_67_OFFSET                 0xb8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_64_67_get(uint32_t base);
        static inline void gpio_padcfg_64_67_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_64_67_PADCFG_BIT                                 0
        #define GPIO_PADCFG_64_67_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_64_67_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_64_67_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_64_67_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_64_67_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_64_67_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_64_67_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_64_67_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_64_67 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_68_71:

PADCFG_68_71
""""""""""""

GPIO pad pin 4 to 7 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 4 to 7 configuration register.
                #define GPIO_PADCFG_68_71_OFFSET                 0xbc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_68_71_get(uint32_t base);
        static inline void gpio_padcfg_68_71_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_68_71_PADCFG_BIT                                 0
        #define GPIO_PADCFG_68_71_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_68_71_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_68_71_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_68_71_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_68_71_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_68_71_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_68_71_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_68_71_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_68_71 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_72_75:

PADCFG_72_75
""""""""""""

GPIO pad pin 8 to 11 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 8 to 11 configuration register.
                #define GPIO_PADCFG_72_75_OFFSET                 0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_72_75_get(uint32_t base);
        static inline void gpio_padcfg_72_75_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_72_75_PADCFG_BIT                                 0
        #define GPIO_PADCFG_72_75_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_72_75_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_72_75_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_72_75_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_72_75_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_72_75_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_72_75_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_72_75_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_72_75 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_76_79:

PADCFG_76_79
""""""""""""

GPIO pad pin 12 to 15 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 12 to 15 configuration register.
                #define GPIO_PADCFG_76_79_OFFSET                 0xc4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_76_79_get(uint32_t base);
        static inline void gpio_padcfg_76_79_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_76_79_PADCFG_BIT                                 0
        #define GPIO_PADCFG_76_79_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_76_79_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_76_79_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_76_79_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_76_79_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_76_79_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_76_79_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_76_79_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_76_79 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_80_83:

PADCFG_80_83
""""""""""""

GPIO pad pin 16 to 19 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 16 to 19 configuration register.
                #define GPIO_PADCFG_80_83_OFFSET                 0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_80_83_get(uint32_t base);
        static inline void gpio_padcfg_80_83_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_80_83_PADCFG_BIT                                 0
        #define GPIO_PADCFG_80_83_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_80_83_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_80_83_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_80_83_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_80_83_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_80_83_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_80_83_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_80_83_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_80_83 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_84_87:

PADCFG_84_87
""""""""""""

GPIO pad pin 20 to 23 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 20 to 23 configuration register.
                #define GPIO_PADCFG_84_87_OFFSET                 0xcc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_84_87_get(uint32_t base);
        static inline void gpio_padcfg_84_87_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_84_87_PADCFG_BIT                                 0
        #define GPIO_PADCFG_84_87_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_84_87_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_84_87_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_84_87_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_84_87_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_84_87_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_84_87_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_84_87_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_84_87 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_88_91:

PADCFG_88_91
""""""""""""

GPIO pad pin 24 to 27 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 24 to 27 configuration register.
                #define GPIO_PADCFG_88_91_OFFSET                 0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_88_91_get(uint32_t base);
        static inline void gpio_padcfg_88_91_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_88_91_PADCFG_BIT                                 0
        #define GPIO_PADCFG_88_91_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_88_91_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_88_91_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_88_91_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_88_91_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_88_91_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_88_91_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_88_91_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_88_91 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|

.. _gpio_PADCFG_92_95:

PADCFG_92_95
""""""""""""

GPIO pad pin 28 to 31 configuration register.

.. table:: 

    +-----+---+------+------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                       Description                                        |
    +=====+===+======+==========================================================================================+
    |3:0  |R/W|PADCFG|GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled|
    +-----+---+------+------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // GPIO pad pin 28 to 31 configuration register.
                #define GPIO_PADCFG_92_95_OFFSET                 0xd4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t gpio_padcfg_92_95_get(uint32_t base);
        static inline void gpio_padcfg_92_95_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled (access: R/W)
        #define GPIO_PADCFG_92_95_PADCFG_BIT                                 0
        #define GPIO_PADCFG_92_95_PADCFG_WIDTH                               4
        #define GPIO_PADCFG_92_95_PADCFG_MASK                                0xf
        #define GPIO_PADCFG_92_95_PADCFG_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define GPIO_PADCFG_92_95_PADCFG_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define GPIO_PADCFG_92_95_PADCFG_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define GPIO_PADCFG_92_95_PADCFG_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define GPIO_PADCFG_92_95_PADCFG(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padcfg          :4 ; // GPIO[0] pull activation configuration bitfield: - 1'b0: pull disabled - 1'b1: pull enabled
          };
          unsigned int raw;
        } __attribute__((packed)) gpio_padcfg_92_95_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_gpio_padcfg_92_95 : public vp::reg_32
        {
        public:
            inline void padcfg_set(uint32_t value);
            inline uint32_t padcfg_get();
        };

|
