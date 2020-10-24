Input file: fe/ips/riscv/doc/SECURED_RI5CY_DEBUG_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |                       Name                        |Offset|Width|                            Description                            |
    +===================================================+======+=====+===================================================================+
    |:ref:`CTRL<secured_riscv_debug_CTRL>`              |     0|   32|Debug control configuration register.                              |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`HIT<secured_riscv_debug_HIT>`                |     4|   32|Debug hit status register.                                         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`IE<secured_riscv_debug_IE>`                  |     8|   32|Debug exception trap enable configuration register.                |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CAUSE<secured_riscv_debug_CAUSE>`            |    12|   32|Debug trap cause status register.                                  |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`NPC<secured_riscv_debug_NPC>`                |  8192|   32|Debug next program counter value register.                         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`PPC<secured_riscv_debug_PPC>`                |  8196|   32|Debug previous program counter value register.                     |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR0<secured_riscv_debug_GPR0>`              |  1024|   32|Core general purpose register 0 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR1<secured_riscv_debug_GPR1>`              |  1028|   32|Core general purpose register 1 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR2<secured_riscv_debug_GPR2>`              |  1032|   32|Core general purpose register 2 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR3<secured_riscv_debug_GPR3>`              |  1036|   32|Core general purpose register 3 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR4<secured_riscv_debug_GPR4>`              |  1040|   32|Core general purpose register 4 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR5<secured_riscv_debug_GPR5>`              |  1044|   32|Core general purpose register 5 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR6<secured_riscv_debug_GPR6>`              |  1048|   32|Core general purpose register 6 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR7<secured_riscv_debug_GPR7>`              |  1052|   32|Core general purpose register 7 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR8<secured_riscv_debug_GPR8>`              |  1056|   32|Core general purpose register 8 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR9<secured_riscv_debug_GPR9>`              |  1060|   32|Core general purpose register 9 value register.                    |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR10<secured_riscv_debug_GPR10>`            |  1064|   32|Core general purpose register 10 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR11<secured_riscv_debug_GPR11>`            |  1068|   32|Core general purpose register 11 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR12<secured_riscv_debug_GPR12>`            |  1072|   32|Core general purpose register 12 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR13<secured_riscv_debug_GPR13>`            |  1076|   32|Core general purpose register 13 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR14<secured_riscv_debug_GPR14>`            |  1080|   32|Core general purpose register 14 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR15<secured_riscv_debug_GPR15>`            |  1084|   32|Core general purpose register 15 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR16<secured_riscv_debug_GPR16>`            |  1088|   32|Core general purpose register 16 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR17<secured_riscv_debug_GPR17>`            |  1092|   32|Core general purpose register 17 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR18<secured_riscv_debug_GPR18>`            |  1096|   32|Core general purpose register 18 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR19<secured_riscv_debug_GPR19>`            |  1100|   32|Core general purpose register 19 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR20<secured_riscv_debug_GPR20>`            |  1104|   32|Core general purpose register 20 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR21<secured_riscv_debug_GPR21>`            |  1108|   32|Core general purpose register 21 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR22<secured_riscv_debug_GPR22>`            |  1112|   32|Core general purpose register 22 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR23<secured_riscv_debug_GPR23>`            |  1116|   32|Core general purpose register 23 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR24<secured_riscv_debug_GPR24>`            |  1120|   32|Core general purpose register 24 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR25<secured_riscv_debug_GPR25>`            |  1124|   32|Core general purpose register 25 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR26<secured_riscv_debug_GPR26>`            |  1128|   32|Core general purpose register 26 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR27<secured_riscv_debug_GPR27>`            |  1132|   32|Core general purpose register 27 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR28<secured_riscv_debug_GPR28>`            |  1136|   32|Core general purpose register 28 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR29<secured_riscv_debug_GPR29>`            |  1140|   32|Core general purpose register 29 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR30<secured_riscv_debug_GPR30>`            |  1144|   32|Core general purpose register 30 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`GPR31<secured_riscv_debug_GPR31>`            |  1148|   32|Core general purpose register 31 value register.                   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MSTATUS<secured_riscv_debug_CSR_MSTATUS>`| 19456|   32|Core CSR machine status value register.                            |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MTVEC<secured_riscv_debug_CSR_MTVEC>`    | 19476|   32|Core CSR machine vector-trap base address value register.          |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MEPC<secured_riscv_debug_CSR_MEPC>`      | 19716|   32|Core CSR machine exception program counter value register.         |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MCAUSE<secured_riscv_debug_CSR_MCAUSE>`  | 19720|   32|Core CSR machine trap cause value register.                        |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCCR<secured_riscv_debug_CSR_PCCR>`      | 24064|   32|Core CSR performance counter counter register.                     |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCER<secured_riscv_debug_CSR_PCER>`      | 24192|   32|Core CSR performance counter enable configuration register.        |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PCMR<secured_riscv_debug_CSR_PCMR>`      | 24196|   32|Core CSR performance counter mode configuration register.          |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0S<secured_riscv_debug_CSR_HWLP0S>`  | 24256|   32|Core CSR hardware loop 0 start configuration register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0E<secured_riscv_debug_CSR_HWLP0E>`  | 24260|   32|Core CSR hardware loop 0 end configuration register.               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP0C<secured_riscv_debug_CSR_HWLP0C>`  | 24264|   32|Core CSR hardware loop 0 counter configuration register.           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1S<secured_riscv_debug_CSR_HWLP1S>`  | 24272|   32|Core CSR hardware loop 1 start configuration register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1E<secured_riscv_debug_CSR_HWLP1E>`  | 24276|   32|Core CSR hardware loop 1 end configuration register.               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_HWLP1C<secured_riscv_debug_CSR_HWLP1C>`  | 24280|   32|Core CSR hardware loop 1 counter configuration register.           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_PRIVLV<secured_riscv_debug_CSR_PRIVLV>`  | 28736|   32|Cose CSR current privilege level status register.                  |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UHARTID<secured_riscv_debug_CSR_UHARTID>`| 16464|   32|Core CSR user privilege mode hardware thread ID status register.   |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_MHARTID<secured_riscv_debug_CSR_MHARTID>`| 31824|   32|Core CSR machine privilege mode hardware thread ID status register.|
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_USTATUS<secured_riscv_debug_CSR_USTATUS>`| 16404|   32|Core CSR user status value register.                               |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UTVEC<secured_riscv_debug_CSR_UTVEC>`    | 16464|   32|Core CSR user vector-trap base address value register.             |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UEPC<secured_riscv_debug_CSR_UEPC>`      | 16644|   32|Core CSR user exception program counter value register.            |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+
    |:ref:`CSR_UCAUSE<secured_riscv_debug_CSR_UCAUSE>`  | 16648|   32|Core CSR user trap cause value register.                           |
    +---------------------------------------------------+------+-----+-------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug control configuration register.
                #define SECURED_RISCV_DEBUG_CTRL_OFFSET          0x0
        
                // Debug hit status register.
                #define SECURED_RISCV_DEBUG_HIT_OFFSET           0x4
        
                // Debug exception trap enable configuration register.
                #define SECURED_RISCV_DEBUG_IE_OFFSET            0x8
        
                // Debug trap cause status register.
                #define SECURED_RISCV_DEBUG_CAUSE_OFFSET         0xc
        
                // Debug next program counter value register.
                #define SECURED_RISCV_DEBUG_NPC_OFFSET           0x2000
        
                // Debug previous program counter value register.
                #define SECURED_RISCV_DEBUG_PPC_OFFSET           0x2004
        
                // Core general purpose register 0 value register.
                #define SECURED_RISCV_DEBUG_GPR0_OFFSET          0x400
        
                // Core general purpose register 1 value register.
                #define SECURED_RISCV_DEBUG_GPR1_OFFSET          0x404
        
                // Core general purpose register 2 value register.
                #define SECURED_RISCV_DEBUG_GPR2_OFFSET          0x408
        
                // Core general purpose register 3 value register.
                #define SECURED_RISCV_DEBUG_GPR3_OFFSET          0x40c
        
                // Core general purpose register 4 value register.
                #define SECURED_RISCV_DEBUG_GPR4_OFFSET          0x410
        
                // Core general purpose register 5 value register.
                #define SECURED_RISCV_DEBUG_GPR5_OFFSET          0x414
        
                // Core general purpose register 6 value register.
                #define SECURED_RISCV_DEBUG_GPR6_OFFSET          0x418
        
                // Core general purpose register 7 value register.
                #define SECURED_RISCV_DEBUG_GPR7_OFFSET          0x41c
        
                // Core general purpose register 8 value register.
                #define SECURED_RISCV_DEBUG_GPR8_OFFSET          0x420
        
                // Core general purpose register 9 value register.
                #define SECURED_RISCV_DEBUG_GPR9_OFFSET          0x424
        
                // Core general purpose register 10 value register.
                #define SECURED_RISCV_DEBUG_GPR10_OFFSET         0x428
        
                // Core general purpose register 11 value register.
                #define SECURED_RISCV_DEBUG_GPR11_OFFSET         0x42c
        
                // Core general purpose register 12 value register.
                #define SECURED_RISCV_DEBUG_GPR12_OFFSET         0x430
        
                // Core general purpose register 13 value register.
                #define SECURED_RISCV_DEBUG_GPR13_OFFSET         0x434
        
                // Core general purpose register 14 value register.
                #define SECURED_RISCV_DEBUG_GPR14_OFFSET         0x438
        
                // Core general purpose register 15 value register.
                #define SECURED_RISCV_DEBUG_GPR15_OFFSET         0x43c
        
                // Core general purpose register 16 value register.
                #define SECURED_RISCV_DEBUG_GPR16_OFFSET         0x440
        
                // Core general purpose register 17 value register.
                #define SECURED_RISCV_DEBUG_GPR17_OFFSET         0x444
        
                // Core general purpose register 18 value register.
                #define SECURED_RISCV_DEBUG_GPR18_OFFSET         0x448
        
                // Core general purpose register 19 value register.
                #define SECURED_RISCV_DEBUG_GPR19_OFFSET         0x44c
        
                // Core general purpose register 20 value register.
                #define SECURED_RISCV_DEBUG_GPR20_OFFSET         0x450
        
                // Core general purpose register 21 value register.
                #define SECURED_RISCV_DEBUG_GPR21_OFFSET         0x454
        
                // Core general purpose register 22 value register.
                #define SECURED_RISCV_DEBUG_GPR22_OFFSET         0x458
        
                // Core general purpose register 23 value register.
                #define SECURED_RISCV_DEBUG_GPR23_OFFSET         0x45c
        
                // Core general purpose register 24 value register.
                #define SECURED_RISCV_DEBUG_GPR24_OFFSET         0x460
        
                // Core general purpose register 25 value register.
                #define SECURED_RISCV_DEBUG_GPR25_OFFSET         0x464
        
                // Core general purpose register 26 value register.
                #define SECURED_RISCV_DEBUG_GPR26_OFFSET         0x468
        
                // Core general purpose register 27 value register.
                #define SECURED_RISCV_DEBUG_GPR27_OFFSET         0x46c
        
                // Core general purpose register 28 value register.
                #define SECURED_RISCV_DEBUG_GPR28_OFFSET         0x470
        
                // Core general purpose register 29 value register.
                #define SECURED_RISCV_DEBUG_GPR29_OFFSET         0x474
        
                // Core general purpose register 30 value register.
                #define SECURED_RISCV_DEBUG_GPR30_OFFSET         0x478
        
                // Core general purpose register 31 value register.
                #define SECURED_RISCV_DEBUG_GPR31_OFFSET         0x47c
        
                // Core CSR machine status value register.
                #define SECURED_RISCV_DEBUG_CSR_MSTATUS_OFFSET   0x4c00
        
                // Core CSR machine vector-trap base address value register.
                #define SECURED_RISCV_DEBUG_CSR_MTVEC_OFFSET     0x4c14
        
                // Core CSR machine exception program counter value register.
                #define SECURED_RISCV_DEBUG_CSR_MEPC_OFFSET      0x4d04
        
                // Core CSR machine trap cause value register.
                #define SECURED_RISCV_DEBUG_CSR_MCAUSE_OFFSET    0x4d08
        
                // Core CSR performance counter counter register.
                #define SECURED_RISCV_DEBUG_CSR_PCCR_OFFSET      0x5e00
        
                // Core CSR performance counter enable configuration register.
                #define SECURED_RISCV_DEBUG_CSR_PCER_OFFSET      0x5e80
        
                // Core CSR performance counter mode configuration register.
                #define SECURED_RISCV_DEBUG_CSR_PCMR_OFFSET      0x5e84
        
                // Core CSR hardware loop 0 start configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0S_OFFSET    0x5ec0
        
                // Core CSR hardware loop 0 end configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0E_OFFSET    0x5ec4
        
                // Core CSR hardware loop 0 counter configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0C_OFFSET    0x5ec8
        
                // Core CSR hardware loop 1 start configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1S_OFFSET    0x5ed0
        
                // Core CSR hardware loop 1 end configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1E_OFFSET    0x5ed4
        
                // Core CSR hardware loop 1 counter configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1C_OFFSET    0x5ed8
        
                // Cose CSR current privilege level status register.
                #define SECURED_RISCV_DEBUG_CSR_PRIVLV_OFFSET    0x7040
        
                // Core CSR user privilege mode hardware thread ID status register.
                #define SECURED_RISCV_DEBUG_CSR_UHARTID_OFFSET   0x4050
        
                // Core CSR machine privilege mode hardware thread ID status register.
                #define SECURED_RISCV_DEBUG_CSR_MHARTID_OFFSET   0x7c50
        
                // Core CSR user status value register.
                #define SECURED_RISCV_DEBUG_CSR_USTATUS_OFFSET   0x4014
        
                // Core CSR user vector-trap base address value register.
                #define SECURED_RISCV_DEBUG_CSR_UTVEC_OFFSET     0x4050
        
                // Core CSR user exception program counter value register.
                #define SECURED_RISCV_DEBUG_CSR_UEPC_OFFSET      0x4104
        
                // Core CSR user trap cause value register.
                #define SECURED_RISCV_DEBUG_CSR_UCAUSE_OFFSET    0x4108

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_ctrl_get(uint32_t base);
        static inline void secured_riscv_debug_ctrl_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_hit_get(uint32_t base);
        static inline void secured_riscv_debug_hit_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_ie_get(uint32_t base);
        static inline void secured_riscv_debug_ie_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_cause_get(uint32_t base);
        static inline void secured_riscv_debug_cause_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_npc_get(uint32_t base);
        static inline void secured_riscv_debug_npc_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_ppc_get(uint32_t base);
        static inline void secured_riscv_debug_ppc_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr0_get(uint32_t base);
        static inline void secured_riscv_debug_gpr0_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr1_get(uint32_t base);
        static inline void secured_riscv_debug_gpr1_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr2_get(uint32_t base);
        static inline void secured_riscv_debug_gpr2_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr3_get(uint32_t base);
        static inline void secured_riscv_debug_gpr3_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr4_get(uint32_t base);
        static inline void secured_riscv_debug_gpr4_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr5_get(uint32_t base);
        static inline void secured_riscv_debug_gpr5_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr6_get(uint32_t base);
        static inline void secured_riscv_debug_gpr6_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr7_get(uint32_t base);
        static inline void secured_riscv_debug_gpr7_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr8_get(uint32_t base);
        static inline void secured_riscv_debug_gpr8_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr9_get(uint32_t base);
        static inline void secured_riscv_debug_gpr9_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr10_get(uint32_t base);
        static inline void secured_riscv_debug_gpr10_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr11_get(uint32_t base);
        static inline void secured_riscv_debug_gpr11_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr12_get(uint32_t base);
        static inline void secured_riscv_debug_gpr12_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr13_get(uint32_t base);
        static inline void secured_riscv_debug_gpr13_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr14_get(uint32_t base);
        static inline void secured_riscv_debug_gpr14_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr15_get(uint32_t base);
        static inline void secured_riscv_debug_gpr15_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr16_get(uint32_t base);
        static inline void secured_riscv_debug_gpr16_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr17_get(uint32_t base);
        static inline void secured_riscv_debug_gpr17_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr18_get(uint32_t base);
        static inline void secured_riscv_debug_gpr18_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr19_get(uint32_t base);
        static inline void secured_riscv_debug_gpr19_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr20_get(uint32_t base);
        static inline void secured_riscv_debug_gpr20_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr21_get(uint32_t base);
        static inline void secured_riscv_debug_gpr21_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr22_get(uint32_t base);
        static inline void secured_riscv_debug_gpr22_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr23_get(uint32_t base);
        static inline void secured_riscv_debug_gpr23_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr24_get(uint32_t base);
        static inline void secured_riscv_debug_gpr24_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr25_get(uint32_t base);
        static inline void secured_riscv_debug_gpr25_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr26_get(uint32_t base);
        static inline void secured_riscv_debug_gpr26_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr27_get(uint32_t base);
        static inline void secured_riscv_debug_gpr27_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr28_get(uint32_t base);
        static inline void secured_riscv_debug_gpr28_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr29_get(uint32_t base);
        static inline void secured_riscv_debug_gpr29_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr30_get(uint32_t base);
        static inline void secured_riscv_debug_gpr30_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_gpr31_get(uint32_t base);
        static inline void secured_riscv_debug_gpr31_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_mstatus_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mstatus_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_mtvec_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mtvec_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_mepc_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mepc_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_mcause_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mcause_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_pccr_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pccr_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_pcer_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pcer_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_pcmr_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pcmr_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp0s_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0s_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp0e_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0e_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp0c_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0c_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp1s_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1s_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp1e_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1e_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_hwlp1c_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1c_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_privlv_get(uint32_t base);
        static inline void secured_riscv_debug_csr_privlv_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_uhartid_get(uint32_t base);
        static inline void secured_riscv_debug_csr_uhartid_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_mhartid_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mhartid_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_ustatus_get(uint32_t base);
        static inline void secured_riscv_debug_csr_ustatus_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_utvec_get(uint32_t base);
        static inline void secured_riscv_debug_csr_utvec_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_uepc_get(uint32_t base);
        static inline void secured_riscv_debug_csr_uepc_set(uint32_t base, uint32_t value);

        static inline uint32_t secured_riscv_debug_csr_ucause_get(uint32_t base);
        static inline void secured_riscv_debug_csr_ucause_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Single step mode configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_BIT                            0
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_MASK                           0x1
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_RESET                          0x0
        
        // Debug mode status bitfield: - 1'b0: running mode - 1'b1: debug mode (access: R)
        #define SECURED_RISCV_DEBUG_CTRL_HALT_BIT                            16
        #define SECURED_RISCV_DEBUG_CTRL_HALT_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CTRL_HALT_MASK                           0x10000
        #define SECURED_RISCV_DEBUG_CTRL_HALT_RESET                          0x0
        
        // Single step hit status bitfield: - 1'b0: single step mode disabled - 1'b1: single step mode enabled Sticky bit that must be cleared by external debugger. (access: R/W)
        #define SECURED_RISCV_DEBUG_HIT_SSTH_BIT                             0
        #define SECURED_RISCV_DEBUG_HIT_SSTH_WIDTH                           1
        #define SECURED_RISCV_DEBUG_HIT_SSTH_MASK                            0x1
        #define SECURED_RISCV_DEBUG_HIT_SSTH_RESET                           0x0
        
        // Sleep mode status bitfield: - 1'b0: core is in running state - 1'b1: core is in sleeping state and waits for en event to wake up (access: R)
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_BIT                            16
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_WIDTH                          1
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_MASK                           0x10000
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_RESET                          0x0
        
        // Illegal instruction trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_EILL_BIT                              2
        #define SECURED_RISCV_DEBUG_IE_EILL_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_EILL_MASK                             0x4
        #define SECURED_RISCV_DEBUG_IE_EILL_RESET                            0x0
        
        // Environment break trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_EBRK_BIT                              3
        #define SECURED_RISCV_DEBUG_IE_EBRK_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_EBRK_MASK                             0x8
        #define SECURED_RISCV_DEBUG_IE_EBRK_RESET                            0x0
        
        // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ELSU_BIT                              5
        #define SECURED_RISCV_DEBUG_IE_ELSU_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_ELSU_MASK                             0x20
        #define SECURED_RISCV_DEBUG_IE_ELSU_RESET                            0x0
        
        // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode This bitfield is redondant to ELSU bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_BIT                          7
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_WIDTH                        1
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_MASK                         0x80
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_RESET                        0x0
        
        // Environment call trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ECALL_BIT                             11
        #define SECURED_RISCV_DEBUG_IE_ECALL_WIDTH                           1
        #define SECURED_RISCV_DEBUG_IE_ECALL_MASK                            0x800
        #define SECURED_RISCV_DEBUG_IE_ECALL_RESET                           0x0
        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_BIT                          0
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_WIDTH                        5
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_MASK                         0x1f
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_RESET                        0x0
        
        // Core in debug mode due to interrupt trap status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_BIT                            31
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_MASK                           0x80000000
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_RESET                          0x0
        
        // Next program counter value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_NPC_NPC_BIT                              0
        #define SECURED_RISCV_DEBUG_NPC_NPC_WIDTH                            32
        #define SECURED_RISCV_DEBUG_NPC_NPC_MASK                             0xffffffff
        #define SECURED_RISCV_DEBUG_NPC_NPC_RESET                            0x0
        
        // Previous program counter value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_PPC_PPC_BIT                              0
        #define SECURED_RISCV_DEBUG_PPC_PPC_WIDTH                            32
        #define SECURED_RISCV_DEBUG_PPC_PPC_MASK                             0xffffffff
        #define SECURED_RISCV_DEBUG_PPC_PPC_RESET                            0x0
        
        // General purpose register 0 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_RESET                          0x0
        
        // General purpose register 1 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_RESET                          0x0
        
        // General purpose register 2 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_RESET                          0x0
        
        // General purpose register 3 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_RESET                          0x0
        
        // General purpose register 4 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_RESET                          0x0
        
        // General purpose register 5 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_RESET                          0x0
        
        // General purpose register 6 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_RESET                          0x0
        
        // General purpose register 7 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_RESET                          0x0
        
        // General purpose register 8 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_RESET                          0x0
        
        // General purpose register 9 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_RESET                          0x0
        
        // General purpose register 10 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_RESET                        0x0
        
        // General purpose register 11 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_RESET                        0x0
        
        // General purpose register 12 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_RESET                        0x0
        
        // General purpose register 13 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_RESET                        0x0
        
        // General purpose register 14 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_RESET                        0x0
        
        // General purpose register 15 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_RESET                        0x0
        
        // General purpose register 16 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_RESET                        0x0
        
        // General purpose register 17 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_RESET                        0x0
        
        // General purpose register 18 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_RESET                        0x0
        
        // General purpose register 19 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_RESET                        0x0
        
        // General purpose register 20 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_RESET                        0x0
        
        // General purpose register 21 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_RESET                        0x0
        
        // General purpose register 22 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_RESET                        0x0
        
        // General purpose register 23 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_RESET                        0x0
        
        // General purpose register 24 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_RESET                        0x0
        
        // General purpose register 25 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_RESET                        0x0
        
        // General purpose register 26 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_RESET                        0x0
        
        // General purpose register 27 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_RESET                        0x0
        
        // General purpose register 28 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_RESET                        0x0
        
        // General purpose register 29 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_RESET                        0x0
        
        // General purpose register 30 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_RESET                        0x0
        
        // General purpose register 31 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_RESET                        0x0
        
        // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_MASK                     0x1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_RESET                    0x0
        
        // Machine privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_BIT                      3
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_MASK                     0x8
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_RESET                    0x0
        
        // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_BIT                     4
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_MASK                    0x10
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_RESET                   0x0
        
        // Machine privilege mode previous interrupt enable value bitfield. When an interrupis encountered, MPIE will store the value existing in MIE. When mret instruction is executed, the value of MPIE is restored into MIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_BIT                     7
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_MASK                    0x80
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_RESET                   0x0
        
        // Machine privilege mode previous privilege mode value bitfield: - 2b00: User mode - 2b11: Machine mode (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_BIT                      11
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_WIDTH                    2
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_MASK                     0x1800
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_RESET                    0x0
        
        // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address.  (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_BIT                      8
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_WIDTH                    24
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_MASK                     0xffffff00
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_RESET                    0x0
        
        // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_RESET                      0x0
        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_WIDTH                   5
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_MASK                    0x1f
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_RESET                   0x0
        
        // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_BIT                       31
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_WIDTH                     1
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_MASK                      0x80000000
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_RESET                     0x0
        
        // Program counter counter value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_RESET                      0x0
        
        // See documentation on [RI5CY core](<https://pulp-platform.org//wp-content/uploads/2017/11/ri5cy_user_manual.pdf>) for details. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_WIDTH                      21
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_MASK                       0x1fffff
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_RESET                      0x0
        
        // Performance counter saturation mode configuration bitfield: - 1'b0: wrap-around mode - 1'b1: saturation mode (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_BIT                         0
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_WIDTH                       1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_MASK                        0x1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_RESET                       0x0
        
        // Performance counter activation configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_BIT                          1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_WIDTH                        1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_MASK                         0x2
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_RESET                        0x0
        
        // Hardware loop start address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_WIDTH                   32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_MASK                    0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_RESET                   0x0
        
        // Hardware loop end address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_RESET                     0x0
        
        // Hardware loop counter configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_RESET                     0x0
        
        // Hardware loop start address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_WIDTH                   32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_MASK                    0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_RESET                   0x0
        
        // Hardware loop end address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_RESET                     0x0
        
        // Hardware loop counter configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_RESET                     0x0
        
        // Current privilege level status bitfield: - 2b00: User mode - 2b11: Machine mode (access: R)
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_WIDTH                    2
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_MASK                     0x3
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_RESET                    0x0
        
        // RI5CY core ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_BIT                  0
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_WIDTH                4
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_MASK                 0xf
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_RESET                0x0
        
        // Cluster ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_BIT               5
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_WIDTH             6
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_MASK              0x7e0
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_RESET             0x0
        
        // RI5CY core ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_BIT                  0
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_WIDTH                4
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_MASK                 0xf
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_RESET                0x0
        
        // Cluster ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_BIT               5
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_WIDTH             6
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_MASK              0x7e0
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_RESET             0x0
        
        // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_MASK                     0x1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_RESET                    0x0
        
        // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_BIT                     4
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_MASK                    0x10
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_RESET                   0x0
        
        // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address.  (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_BIT                      8
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_WIDTH                    24
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_MASK                     0xffffff00
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_RESET                    0x0
        
        // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_RESET                      0x0
        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_WIDTH                   5
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_MASK                    0x1f
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_RESET                   0x0
        
        // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_BIT                       31
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_WIDTH                     1
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_MASK                      0x80000000
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CTRL_HALT_GET(value)           (GAP_BEXTRACTU((value),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT_GETS(value)          (GAP_BEXTRACT((value),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT_SET(value,field)     (GAP_BINSERT((value),(field),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT(val)                 ((val) << 16)
        
        #define SECURED_RISCV_DEBUG_HIT_SSTH_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH(val)                  ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_GET(value)           (GAP_BEXTRACTU((value),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_GETS(value)          (GAP_BEXTRACT((value),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_SET(value,field)     (GAP_BINSERT((value),(field),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP(val)                 ((val) << 16)
        
        #define SECURED_RISCV_DEBUG_IE_EILL_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL(val)                   ((val) << 2)
        
        #define SECURED_RISCV_DEBUG_IE_EBRK_GET(value)             (GAP_BEXTRACTU((value),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK_GETS(value)            (GAP_BEXTRACT((value),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK_SET(value,field)       (GAP_BINSERT((value),(field),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK(val)                   ((val) << 3)
        
        #define SECURED_RISCV_DEBUG_IE_ELSU_GET(value)             (GAP_BEXTRACTU((value),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU_GETS(value)            (GAP_BEXTRACT((value),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU_SET(value,field)       (GAP_BINSERT((value),(field),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU(val)                   ((val) << 5)
        
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_GET(value)         (GAP_BEXTRACTU((value),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_GETS(value)        (GAP_BEXTRACT((value),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_SET(value,field)   (GAP_BINSERT((value),(field),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP(val)               ((val) << 7)
        
        #define SECURED_RISCV_DEBUG_IE_ECALL_GET(value)            (GAP_BEXTRACTU((value),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL_GETS(value)           (GAP_BEXTRACT((value),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL_SET(value,field)      (GAP_BINSERT((value),(field),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL(val)                  ((val) << 11)
        
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_GET(value)         (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_GETS(value)        (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_SET(value,field)   (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_GET(value)           (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_GETS(value)          (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_SET(value,field)     (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ(val)                 ((val) << 31)
        
        #define SECURED_RISCV_DEBUG_NPC_NPC_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC(val)                   ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_PPC_PPC_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC(val)                   ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_GET(value)     (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_GETS(value)    (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE(val)           ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_GET(value)     (GAP_BEXTRACTU((value),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_GETS(value)    (GAP_BEXTRACT((value),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE(val)           ((val) << 3)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE(val)          ((val) << 4)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_GET(value)    (GAP_BEXTRACTU((value),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_GETS(value)   (GAP_BEXTRACT((value),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE(val)          ((val) << 7)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_GET(value)     (GAP_BEXTRACTU((value),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_GETS(value)    (GAP_BEXTRACT((value),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_SET(value,field) (GAP_BINSERT((value),(field),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP(val)           ((val) << 11)
        
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_GET(value)     (GAP_BEXTRACTU((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_GETS(value)    (GAP_BEXTRACT((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_SET(value,field) (GAP_BINSERT((value),(field),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC(val)           ((val) << 8)
        
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC(val)             ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ(val)            ((val) << 31)
        
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR(val)             ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_GET(value)       (GAP_BEXTRACTU((value),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_GETS(value)      (GAP_BEXTRACT((value),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_SET(value,field) (GAP_BINSERT((value),(field),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER(val)             ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT(val)              ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_GET(value)         (GAP_BEXTRACTU((value),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_GETS(value)        (GAP_BEXTRACT((value),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_SET(value,field)   (GAP_BINSERT((value),(field),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE(val)               ((val) << 1)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END(val)            ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT(val)            ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END(val)            ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT(val)            ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_GET(value)     (GAP_BEXTRACTU((value),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_GETS(value)    (GAP_BEXTRACT((value),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV(val)           ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID(val)       ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_GET(value) (GAP_BEXTRACTU((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_GETS(value) (GAP_BEXTRACT((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_SET(value,field) (GAP_BINSERT((value),(field),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID(val)    ((val) << 5)
        
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID(val)       ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_GET(value) (GAP_BEXTRACTU((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_GETS(value) (GAP_BEXTRACT((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_SET(value,field) (GAP_BINSERT((value),(field),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID(val)    ((val) << 5)
        
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_GET(value)     (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_GETS(value)    (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE(val)           ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE(val)          ((val) << 4)
        
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_GET(value)     (GAP_BEXTRACTU((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_GETS(value)    (GAP_BEXTRACT((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_SET(value,field) (GAP_BINSERT((value),(field),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC(val)           ((val) << 8)
        
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC(val)             ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ(val)            ((val) << 31)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** SECURED_RISCV_DEBUG_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t ctrl;  // Debug control configuration register.
            volatile uint32_t hit;  // Debug hit status register.
            volatile uint32_t ie;  // Debug exception trap enable configuration register.
            volatile uint32_t cause;  // Debug trap cause status register.
            volatile uint32_t reserved_0[2044];  // Reserved/Not used.
            volatile uint32_t npc;  // Debug next program counter value register.
            volatile uint32_t ppc;  // Debug previous program counter value register.
            volatile uint32_t gpr0;  // Core general purpose register 0 value register.
            volatile uint32_t gpr1;  // Core general purpose register 1 value register.
            volatile uint32_t gpr2;  // Core general purpose register 2 value register.
            volatile uint32_t gpr3;  // Core general purpose register 3 value register.
            volatile uint32_t gpr4;  // Core general purpose register 4 value register.
            volatile uint32_t gpr5;  // Core general purpose register 5 value register.
            volatile uint32_t gpr6;  // Core general purpose register 6 value register.
            volatile uint32_t gpr7;  // Core general purpose register 7 value register.
            volatile uint32_t gpr8;  // Core general purpose register 8 value register.
            volatile uint32_t gpr9;  // Core general purpose register 9 value register.
            volatile uint32_t gpr10;  // Core general purpose register 10 value register.
            volatile uint32_t gpr11;  // Core general purpose register 11 value register.
            volatile uint32_t gpr12;  // Core general purpose register 12 value register.
            volatile uint32_t gpr13;  // Core general purpose register 13 value register.
            volatile uint32_t gpr14;  // Core general purpose register 14 value register.
            volatile uint32_t gpr15;  // Core general purpose register 15 value register.
            volatile uint32_t gpr16;  // Core general purpose register 16 value register.
            volatile uint32_t gpr17;  // Core general purpose register 17 value register.
            volatile uint32_t gpr18;  // Core general purpose register 18 value register.
            volatile uint32_t gpr19;  // Core general purpose register 19 value register.
            volatile uint32_t gpr20;  // Core general purpose register 20 value register.
            volatile uint32_t gpr21;  // Core general purpose register 21 value register.
            volatile uint32_t gpr22;  // Core general purpose register 22 value register.
            volatile uint32_t gpr23;  // Core general purpose register 23 value register.
            volatile uint32_t gpr24;  // Core general purpose register 24 value register.
            volatile uint32_t gpr25;  // Core general purpose register 25 value register.
            volatile uint32_t gpr26;  // Core general purpose register 26 value register.
            volatile uint32_t gpr27;  // Core general purpose register 27 value register.
            volatile uint32_t gpr28;  // Core general purpose register 28 value register.
            volatile uint32_t gpr29;  // Core general purpose register 29 value register.
            volatile uint32_t gpr30;  // Core general purpose register 30 value register.
            volatile uint32_t gpr31;  // Core general purpose register 31 value register.
            volatile uint32_t reserved_1[4576];  // Reserved/Not used.
            volatile uint32_t csr_mstatus;  // Core CSR machine status value register.
            volatile uint32_t reserved_2[4];  // Reserved/Not used.
            volatile uint32_t csr_mtvec;  // Core CSR machine vector-trap base address value register.
            volatile uint32_t reserved_3[59];  // Reserved/Not used.
            volatile uint32_t csr_mepc;  // Core CSR machine exception program counter value register.
            volatile uint32_t csr_mcause;  // Core CSR machine trap cause value register.
            volatile uint32_t reserved_4[1085];  // Reserved/Not used.
            volatile uint32_t csr_pccr;  // Core CSR performance counter counter register.
            volatile uint32_t reserved_5[31];  // Reserved/Not used.
            volatile uint32_t csr_pcer;  // Core CSR performance counter enable configuration register.
            volatile uint32_t csr_pcmr;  // Core CSR performance counter mode configuration register.
            volatile uint32_t reserved_6[14];  // Reserved/Not used.
            volatile uint32_t csr_hwlp0s;  // Core CSR hardware loop 0 start configuration register.
            volatile uint32_t csr_hwlp0e;  // Core CSR hardware loop 0 end configuration register.
            volatile uint32_t csr_hwlp0c;  // Core CSR hardware loop 0 counter configuration register.
            volatile uint32_t reserved_7[1];  // Reserved/Not used.
            volatile uint32_t csr_hwlp1s;  // Core CSR hardware loop 1 start configuration register.
            volatile uint32_t csr_hwlp1e;  // Core CSR hardware loop 1 end configuration register.
            volatile uint32_t csr_hwlp1c;  // Core CSR hardware loop 1 counter configuration register.
            volatile uint32_t reserved_8[1113];  // Reserved/Not used.
            volatile uint32_t csr_privlv;  // Cose CSR current privilege level status register.
            volatile uint32_t csr_uhartid;  // Core CSR user privilege mode hardware thread ID status register.
            volatile uint32_t reserved_9[3839];  // Reserved/Not used.
            volatile uint32_t csr_mhartid;  // Core CSR machine privilege mode hardware thread ID status register.
            volatile uint32_t csr_ustatus;  // Core CSR user status value register.
            volatile uint32_t reserved_10[14];  // Reserved/Not used.
            volatile uint32_t csr_utvec;  // Core CSR user vector-trap base address value register.
            volatile uint32_t reserved_11[44];  // Reserved/Not used.
            volatile uint32_t csr_uepc;  // Core CSR user exception program counter value register.
            volatile uint32_t csr_ucause;  // Core CSR user trap cause value register.
        } __attribute__((packed)) secured_riscv_debug_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sste            :1 ; // Single step mode configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:15;
            unsigned int halt            :1 ; // Debug mode status bitfield: - 1'b0: running mode - 1'b1: debug mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ctrl_t;
        
        typedef union {
          struct {
            unsigned int ssth            :1 ; // Single step hit status bitfield: - 1'b0: single step mode disabled - 1'b1: single step mode enabled Sticky bit that must be cleared by external debugger.
            unsigned int padding0:15;
            unsigned int sleep           :1 ; // Sleep mode status bitfield: - 1'b0: core is in running state - 1'b1: core is in sleeping state and waits for en event to wake up
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_hit_t;
        
        typedef union {
          struct {
            unsigned int padding0:2 ;
            unsigned int eill            :1 ; // Illegal instruction trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int ebrk            :1 ; // Environment break trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int padding1:1 ;
            unsigned int elsu            :1 ; // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int padding2:1 ;
            unsigned int elsu_dup        :1 ; // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode This bitfield is redondant to ELSU bitfield.
            unsigned int padding3:3 ;
            unsigned int ecall           :1 ; // Environment call trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ie_t;
        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core in debug mode due to interrupt trap status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_cause_t;
        
        typedef union {
          struct {
            unsigned int npc             :32; // Next program counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_npc_t;
        
        typedef union {
          struct {
            unsigned int ppc             :32; // Previous program counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ppc_t;
        
        typedef union {
          struct {
            unsigned int gpr0            :32; // General purpose register 0 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr0_t;
        
        typedef union {
          struct {
            unsigned int gpr1            :32; // General purpose register 1 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr1_t;
        
        typedef union {
          struct {
            unsigned int gpr2            :32; // General purpose register 2 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr2_t;
        
        typedef union {
          struct {
            unsigned int gpr3            :32; // General purpose register 3 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr3_t;
        
        typedef union {
          struct {
            unsigned int gpr4            :32; // General purpose register 4 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr4_t;
        
        typedef union {
          struct {
            unsigned int gpr5            :32; // General purpose register 5 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr5_t;
        
        typedef union {
          struct {
            unsigned int gpr6            :32; // General purpose register 6 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr6_t;
        
        typedef union {
          struct {
            unsigned int gpr7            :32; // General purpose register 7 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr7_t;
        
        typedef union {
          struct {
            unsigned int gpr8            :32; // General purpose register 8 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr8_t;
        
        typedef union {
          struct {
            unsigned int gpr9            :32; // General purpose register 9 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr9_t;
        
        typedef union {
          struct {
            unsigned int gpr10           :32; // General purpose register 10 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr10_t;
        
        typedef union {
          struct {
            unsigned int gpr11           :32; // General purpose register 11 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr11_t;
        
        typedef union {
          struct {
            unsigned int gpr12           :32; // General purpose register 12 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr12_t;
        
        typedef union {
          struct {
            unsigned int gpr13           :32; // General purpose register 13 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr13_t;
        
        typedef union {
          struct {
            unsigned int gpr14           :32; // General purpose register 14 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr14_t;
        
        typedef union {
          struct {
            unsigned int gpr15           :32; // General purpose register 15 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr15_t;
        
        typedef union {
          struct {
            unsigned int gpr16           :32; // General purpose register 16 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr16_t;
        
        typedef union {
          struct {
            unsigned int gpr17           :32; // General purpose register 17 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr17_t;
        
        typedef union {
          struct {
            unsigned int gpr18           :32; // General purpose register 18 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr18_t;
        
        typedef union {
          struct {
            unsigned int gpr19           :32; // General purpose register 19 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr19_t;
        
        typedef union {
          struct {
            unsigned int gpr20           :32; // General purpose register 20 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr20_t;
        
        typedef union {
          struct {
            unsigned int gpr21           :32; // General purpose register 21 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr21_t;
        
        typedef union {
          struct {
            unsigned int gpr22           :32; // General purpose register 22 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr22_t;
        
        typedef union {
          struct {
            unsigned int gpr23           :32; // General purpose register 23 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr23_t;
        
        typedef union {
          struct {
            unsigned int gpr24           :32; // General purpose register 24 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr24_t;
        
        typedef union {
          struct {
            unsigned int gpr25           :32; // General purpose register 25 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr25_t;
        
        typedef union {
          struct {
            unsigned int gpr26           :32; // General purpose register 26 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr26_t;
        
        typedef union {
          struct {
            unsigned int gpr27           :32; // General purpose register 27 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr27_t;
        
        typedef union {
          struct {
            unsigned int gpr28           :32; // General purpose register 28 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr28_t;
        
        typedef union {
          struct {
            unsigned int gpr29           :32; // General purpose register 29 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr29_t;
        
        typedef union {
          struct {
            unsigned int gpr30           :32; // General purpose register 30 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr30_t;
        
        typedef union {
          struct {
            unsigned int gpr31           :32; // General purpose register 31 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr31_t;
        
        typedef union {
          struct {
            unsigned int uie             :1 ; // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:2 ;
            unsigned int mie             :1 ; // Machine privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int upie            :1 ; // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.
            unsigned int padding1:2 ;
            unsigned int mpie            :1 ; // Machine privilege mode previous interrupt enable value bitfield. When an interrupis encountered, MPIE will store the value existing in MIE. When mret instruction is executed, the value of MPIE is restored into MIE.
            unsigned int padding2:3 ;
            unsigned int mpp             :2 ; // Machine privilege mode previous privilege mode value bitfield: - 2b00: User mode - 2b11: Machine mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mstatus_t;
        
        typedef union {
          struct {
            unsigned int padding0:8 ;
            unsigned int mtvec           :24; // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. 
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mtvec_t;
        
        typedef union {
          struct {
            unsigned int mepc            :32; // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mepc_t;
        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mcause_t;
        
        typedef union {
          struct {
            unsigned int pccr            :32; // Program counter counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pccr_t;
        
        typedef union {
          struct {
            unsigned int pcer            :21; // See documentation on [RI5CY core](<https://pulp-platform.org//wp-content/uploads/2017/11/ri5cy_user_manual.pdf>) for details.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pcer_t;
        
        typedef union {
          struct {
            unsigned int sat             :1 ; // Performance counter saturation mode configuration bitfield: - 1'b0: wrap-around mode - 1'b1: saturation mode
            unsigned int ge              :1 ; // Performance counter activation configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pcmr_t;
        
        typedef union {
          struct {
            unsigned int start           :32; // Hardware loop start address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0s_t;
        
        typedef union {
          struct {
            unsigned int end             :32; // Hardware loop end address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0e_t;
        
        typedef union {
          struct {
            unsigned int cnt             :32; // Hardware loop counter configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0c_t;
        
        typedef union {
          struct {
            unsigned int start           :32; // Hardware loop start address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1s_t;
        
        typedef union {
          struct {
            unsigned int end             :32; // Hardware loop end address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1e_t;
        
        typedef union {
          struct {
            unsigned int cnt             :32; // Hardware loop counter configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1c_t;
        
        typedef union {
          struct {
            unsigned int plev            :2 ; // Current privilege level status bitfield: - 2b00: User mode - 2b11: Machine mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_privlv_t;
        
        typedef union {
          struct {
            unsigned int core_id         :4 ; // RI5CY core ID value bitfield.
            unsigned int padding0:1 ;
            unsigned int cluster_id      :6 ; // Cluster ID value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_uhartid_t;
        
        typedef union {
          struct {
            unsigned int core_id         :4 ; // RI5CY core ID value bitfield.
            unsigned int padding0:1 ;
            unsigned int cluster_id      :6 ; // Cluster ID value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mhartid_t;
        
        typedef union {
          struct {
            unsigned int uie             :1 ; // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:3 ;
            unsigned int upie            :1 ; // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_ustatus_t;
        
        typedef union {
          struct {
            unsigned int padding0:8 ;
            unsigned int utvec           :24; // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. 
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_utvec_t;
        
        typedef union {
          struct {
            unsigned int uepc            :32; // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_uepc_t;
        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_ucause_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_secured_riscv_debug : public vp::regmap
        {
        public:
            vp_secured_riscv_debug_ctrl ctrl;
            vp_secured_riscv_debug_hit hit;
            vp_secured_riscv_debug_ie ie;
            vp_secured_riscv_debug_cause cause;
            vp_secured_riscv_debug_npc npc;
            vp_secured_riscv_debug_ppc ppc;
            vp_secured_riscv_debug_gpr0 gpr0;
            vp_secured_riscv_debug_gpr1 gpr1;
            vp_secured_riscv_debug_gpr2 gpr2;
            vp_secured_riscv_debug_gpr3 gpr3;
            vp_secured_riscv_debug_gpr4 gpr4;
            vp_secured_riscv_debug_gpr5 gpr5;
            vp_secured_riscv_debug_gpr6 gpr6;
            vp_secured_riscv_debug_gpr7 gpr7;
            vp_secured_riscv_debug_gpr8 gpr8;
            vp_secured_riscv_debug_gpr9 gpr9;
            vp_secured_riscv_debug_gpr10 gpr10;
            vp_secured_riscv_debug_gpr11 gpr11;
            vp_secured_riscv_debug_gpr12 gpr12;
            vp_secured_riscv_debug_gpr13 gpr13;
            vp_secured_riscv_debug_gpr14 gpr14;
            vp_secured_riscv_debug_gpr15 gpr15;
            vp_secured_riscv_debug_gpr16 gpr16;
            vp_secured_riscv_debug_gpr17 gpr17;
            vp_secured_riscv_debug_gpr18 gpr18;
            vp_secured_riscv_debug_gpr19 gpr19;
            vp_secured_riscv_debug_gpr20 gpr20;
            vp_secured_riscv_debug_gpr21 gpr21;
            vp_secured_riscv_debug_gpr22 gpr22;
            vp_secured_riscv_debug_gpr23 gpr23;
            vp_secured_riscv_debug_gpr24 gpr24;
            vp_secured_riscv_debug_gpr25 gpr25;
            vp_secured_riscv_debug_gpr26 gpr26;
            vp_secured_riscv_debug_gpr27 gpr27;
            vp_secured_riscv_debug_gpr28 gpr28;
            vp_secured_riscv_debug_gpr29 gpr29;
            vp_secured_riscv_debug_gpr30 gpr30;
            vp_secured_riscv_debug_gpr31 gpr31;
            vp_secured_riscv_debug_csr_mstatus csr_mstatus;
            vp_secured_riscv_debug_csr_mtvec csr_mtvec;
            vp_secured_riscv_debug_csr_mepc csr_mepc;
            vp_secured_riscv_debug_csr_mcause csr_mcause;
            vp_secured_riscv_debug_csr_pccr csr_pccr;
            vp_secured_riscv_debug_csr_pcer csr_pcer;
            vp_secured_riscv_debug_csr_pcmr csr_pcmr;
            vp_secured_riscv_debug_csr_hwlp0s csr_hwlp0s;
            vp_secured_riscv_debug_csr_hwlp0e csr_hwlp0e;
            vp_secured_riscv_debug_csr_hwlp0c csr_hwlp0c;
            vp_secured_riscv_debug_csr_hwlp1s csr_hwlp1s;
            vp_secured_riscv_debug_csr_hwlp1e csr_hwlp1e;
            vp_secured_riscv_debug_csr_hwlp1c csr_hwlp1c;
            vp_secured_riscv_debug_csr_privlv csr_privlv;
            vp_secured_riscv_debug_csr_uhartid csr_uhartid;
            vp_secured_riscv_debug_csr_mhartid csr_mhartid;
            vp_secured_riscv_debug_csr_ustatus csr_ustatus;
            vp_secured_riscv_debug_csr_utvec csr_utvec;
            vp_secured_riscv_debug_csr_uepc csr_uepc;
            vp_secured_riscv_debug_csr_ucause csr_ucause;
        };

|

.. _secured_riscv_debug_CTRL:

CTRL
""""

Debug control configuration register.

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                               |
    +=====+===+====+=========================================================================+
    |    0|R/W|SSTE|Single step mode configuration bitfield: - 1'b0: disabled - 1'b1: enabled|
    +-----+---+----+-------------------------------------------------------------------------+
    |   16|R  |HALT|Debug mode status bitfield: - 1'b0: running mode - 1'b1: debug mode      |
    +-----+---+----+-------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug control configuration register.
                #define SECURED_RISCV_DEBUG_CTRL_OFFSET          0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_ctrl_get(uint32_t base);
        static inline void secured_riscv_debug_ctrl_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Single step mode configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_BIT                            0
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_MASK                           0x1
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_RESET                          0x0
        
        // Debug mode status bitfield: - 1'b0: running mode - 1'b1: debug mode (access: R)
        #define SECURED_RISCV_DEBUG_CTRL_HALT_BIT                            16
        #define SECURED_RISCV_DEBUG_CTRL_HALT_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CTRL_HALT_MASK                           0x10000
        #define SECURED_RISCV_DEBUG_CTRL_HALT_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CTRL_SSTE(val)                 ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CTRL_HALT_GET(value)           (GAP_BEXTRACTU((value),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT_GETS(value)          (GAP_BEXTRACT((value),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT_SET(value,field)     (GAP_BINSERT((value),(field),1,16))
        #define SECURED_RISCV_DEBUG_CTRL_HALT(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sste            :1 ; // Single step mode configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:15;
            unsigned int halt            :1 ; // Debug mode status bitfield: - 1'b0: running mode - 1'b1: debug mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ctrl_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_ctrl : public vp::reg_32
        {
        public:
            inline void sste_set(uint32_t value);
            inline uint32_t sste_get();
            inline void halt_set(uint32_t value);
            inline uint32_t halt_get();
        };

|

.. _secured_riscv_debug_HIT:

HIT
"""

Debug hit status register.

.. table:: 

    +-----+---+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                       Description                                                                       |
    +=====+===+=====+=========================================================================================================================================================+
    |    0|R/W|SSTH |Single step hit status bitfield: - 1'b0: single step mode disabled - 1'b1: single step mode enabled Sticky bit that must be cleared by external debugger.|
    +-----+---+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   16|R  |SLEEP|Sleep mode status bitfield: - 1'b0: core is in running state - 1'b1: core is in sleeping state and waits for en event to wake up                         |
    +-----+---+-----+---------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug hit status register.
                #define SECURED_RISCV_DEBUG_HIT_OFFSET           0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_hit_get(uint32_t base);
        static inline void secured_riscv_debug_hit_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Single step hit status bitfield: - 1'b0: single step mode disabled - 1'b1: single step mode enabled Sticky bit that must be cleared by external debugger. (access: R/W)
        #define SECURED_RISCV_DEBUG_HIT_SSTH_BIT                             0
        #define SECURED_RISCV_DEBUG_HIT_SSTH_WIDTH                           1
        #define SECURED_RISCV_DEBUG_HIT_SSTH_MASK                            0x1
        #define SECURED_RISCV_DEBUG_HIT_SSTH_RESET                           0x0
        
        // Sleep mode status bitfield: - 1'b0: core is in running state - 1'b1: core is in sleeping state and waits for en event to wake up (access: R)
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_BIT                            16
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_WIDTH                          1
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_MASK                           0x10000
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_HIT_SSTH_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_HIT_SSTH(val)                  ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_GET(value)           (GAP_BEXTRACTU((value),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_GETS(value)          (GAP_BEXTRACT((value),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP_SET(value,field)     (GAP_BINSERT((value),(field),1,16))
        #define SECURED_RISCV_DEBUG_HIT_SLEEP(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ssth            :1 ; // Single step hit status bitfield: - 1'b0: single step mode disabled - 1'b1: single step mode enabled Sticky bit that must be cleared by external debugger.
            unsigned int padding0:15;
            unsigned int sleep           :1 ; // Sleep mode status bitfield: - 1'b0: core is in running state - 1'b1: core is in sleeping state and waits for en event to wake up
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_hit_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_hit : public vp::reg_32
        {
        public:
            inline void ssth_set(uint32_t value);
            inline uint32_t ssth_get();
            inline void sleep_set(uint32_t value);
            inline uint32_t sleep_get();
        };

|

.. _secured_riscv_debug_IE:

IE
""

Debug exception trap enable configuration register.

.. table:: 

    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                                             Description                                                                                              |
    +=====+===+========+======================================================================================================================================================================================================+
    |    2|R/W|EILL    |Illegal instruction trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode                                                 |
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|EBRK    |Environment break trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode                                                   |
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|ELSU    |Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode                                             |
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|ELSU_DUP|Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode This bitfield is redondant to ELSU bitfield.|
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |   11|R/W|ECALL   |Environment call trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode                                                    |
    +-----+---+--------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug exception trap enable configuration register.
                #define SECURED_RISCV_DEBUG_IE_OFFSET            0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_ie_get(uint32_t base);
        static inline void secured_riscv_debug_ie_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Illegal instruction trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_EILL_BIT                              2
        #define SECURED_RISCV_DEBUG_IE_EILL_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_EILL_MASK                             0x4
        #define SECURED_RISCV_DEBUG_IE_EILL_RESET                            0x0
        
        // Environment break trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_EBRK_BIT                              3
        #define SECURED_RISCV_DEBUG_IE_EBRK_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_EBRK_MASK                             0x8
        #define SECURED_RISCV_DEBUG_IE_EBRK_RESET                            0x0
        
        // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ELSU_BIT                              5
        #define SECURED_RISCV_DEBUG_IE_ELSU_WIDTH                            1
        #define SECURED_RISCV_DEBUG_IE_ELSU_MASK                             0x20
        #define SECURED_RISCV_DEBUG_IE_ELSU_RESET                            0x0
        
        // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode This bitfield is redondant to ELSU bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_BIT                          7
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_WIDTH                        1
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_MASK                         0x80
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_RESET                        0x0
        
        // Environment call trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode (access: R/W)
        #define SECURED_RISCV_DEBUG_IE_ECALL_BIT                             11
        #define SECURED_RISCV_DEBUG_IE_ECALL_WIDTH                           1
        #define SECURED_RISCV_DEBUG_IE_ECALL_MASK                            0x800
        #define SECURED_RISCV_DEBUG_IE_ECALL_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_IE_EILL_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define SECURED_RISCV_DEBUG_IE_EILL(val)                   ((val) << 2)
        
        #define SECURED_RISCV_DEBUG_IE_EBRK_GET(value)             (GAP_BEXTRACTU((value),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK_GETS(value)            (GAP_BEXTRACT((value),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK_SET(value,field)       (GAP_BINSERT((value),(field),1,3))
        #define SECURED_RISCV_DEBUG_IE_EBRK(val)                   ((val) << 3)
        
        #define SECURED_RISCV_DEBUG_IE_ELSU_GET(value)             (GAP_BEXTRACTU((value),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU_GETS(value)            (GAP_BEXTRACT((value),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU_SET(value,field)       (GAP_BINSERT((value),(field),1,5))
        #define SECURED_RISCV_DEBUG_IE_ELSU(val)                   ((val) << 5)
        
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_GET(value)         (GAP_BEXTRACTU((value),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_GETS(value)        (GAP_BEXTRACT((value),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP_SET(value,field)   (GAP_BINSERT((value),(field),1,7))
        #define SECURED_RISCV_DEBUG_IE_ELSU_DUP(val)               ((val) << 7)
        
        #define SECURED_RISCV_DEBUG_IE_ECALL_GET(value)            (GAP_BEXTRACTU((value),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL_GETS(value)           (GAP_BEXTRACT((value),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL_SET(value,field)      (GAP_BINSERT((value),(field),1,11))
        #define SECURED_RISCV_DEBUG_IE_ECALL(val)                  ((val) << 11)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:2 ;
            unsigned int eill            :1 ; // Illegal instruction trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int ebrk            :1 ; // Environment break trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int padding1:1 ;
            unsigned int elsu            :1 ; // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
            unsigned int padding2:1 ;
            unsigned int elsu_dup        :1 ; // Load/store access fault trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode This bitfield is redondant to ELSU bitfield.
            unsigned int padding3:3 ;
            unsigned int ecall           :1 ; // Environment call trap configuration bitfield: - 1'b0: normal exception behavior mode - 1'b1: exception causes trap and core switch into debug mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ie_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_ie : public vp::reg_32
        {
        public:
            inline void eill_set(uint32_t value);
            inline uint32_t eill_get();
            inline void ebrk_set(uint32_t value);
            inline uint32_t ebrk_get();
            inline void elsu_set(uint32_t value);
            inline uint32_t elsu_get();
            inline void elsu_dup_set(uint32_t value);
            inline uint32_t elsu_dup_get();
            inline void ecall_set(uint32_t value);
            inline uint32_t ecall_get();
        };

|

.. _secured_riscv_debug_CAUSE:

CAUSE
"""""

Debug trap cause status register.

.. table:: 

    +-----+---+-----+------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                    Description                                     |
    +=====+===+=====+====================================================================================+
    |4:0  |R  |CAUSE|Exception ID bitfield.                                                              |
    +-----+---+-----+------------------------------------------------------------------------------------+
    |31   |R  |IRQ  |Core in debug mode due to interrupt trap status bitfield: - 1'b0: false - 1'b1: true|
    +-----+---+-----+------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug trap cause status register.
                #define SECURED_RISCV_DEBUG_CAUSE_OFFSET         0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_cause_get(uint32_t base);
        static inline void secured_riscv_debug_cause_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_BIT                          0
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_WIDTH                        5
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_MASK                         0x1f
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_RESET                        0x0
        
        // Core in debug mode due to interrupt trap status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_BIT                            31
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_WIDTH                          1
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_MASK                           0x80000000
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_GET(value)         (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_GETS(value)        (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE_SET(value,field)   (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CAUSE_CAUSE(val)               ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_GET(value)           (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_GETS(value)          (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ_SET(value,field)     (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CAUSE_IRQ(val)                 ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core in debug mode due to interrupt trap status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_cause_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_cause : public vp::reg_32
        {
        public:
            inline void cause_set(uint32_t value);
            inline uint32_t cause_get();
            inline void irq_set(uint32_t value);
            inline uint32_t irq_get();
        };

|

.. _secured_riscv_debug_NPC:

NPC
"""

Debug next program counter value register.

.. table:: 

    +-----+---+----+------------------------------------+
    |Bit #|R/W|Name|            Description             |
    +=====+===+====+====================================+
    |31:0 |R/W|NPC |Next program counter value bitfield.|
    +-----+---+----+------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug next program counter value register.
                #define SECURED_RISCV_DEBUG_NPC_OFFSET           0x2000

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_npc_get(uint32_t base);
        static inline void secured_riscv_debug_npc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Next program counter value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_NPC_NPC_BIT                              0
        #define SECURED_RISCV_DEBUG_NPC_NPC_WIDTH                            32
        #define SECURED_RISCV_DEBUG_NPC_NPC_MASK                             0xffffffff
        #define SECURED_RISCV_DEBUG_NPC_NPC_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_NPC_NPC_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_NPC_NPC(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int npc             :32; // Next program counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_npc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_npc : public vp::reg_32
        {
        public:
            inline void npc_set(uint32_t value);
            inline uint32_t npc_get();
        };

|

.. _secured_riscv_debug_PPC:

PPC
"""

Debug previous program counter value register.

.. table:: 

    +-----+---+----+----------------------------------------+
    |Bit #|R/W|Name|              Description               |
    +=====+===+====+========================================+
    |31:0 |R  |PPC |Previous program counter value bitfield.|
    +-----+---+----+----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Debug previous program counter value register.
                #define SECURED_RISCV_DEBUG_PPC_OFFSET           0x2004

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_ppc_get(uint32_t base);
        static inline void secured_riscv_debug_ppc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Previous program counter value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_PPC_PPC_BIT                              0
        #define SECURED_RISCV_DEBUG_PPC_PPC_WIDTH                            32
        #define SECURED_RISCV_DEBUG_PPC_PPC_MASK                             0xffffffff
        #define SECURED_RISCV_DEBUG_PPC_PPC_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_PPC_PPC_GET(value)             (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC_GETS(value)            (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC_SET(value,field)       (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_PPC_PPC(val)                   ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ppc             :32; // Previous program counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_ppc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_ppc : public vp::reg_32
        {
        public:
            inline void ppc_set(uint32_t value);
            inline uint32_t ppc_get();
        };

|

.. _secured_riscv_debug_GPR0:

GPR0
""""

Core general purpose register 0 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR0|General purpose register 0 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 0 value register.
                #define SECURED_RISCV_DEBUG_GPR0_OFFSET          0x400

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr0_get(uint32_t base);
        static inline void secured_riscv_debug_gpr0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 0 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR0_GPR0(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr0            :32; // General purpose register 0 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr0 : public vp::reg_32
        {
        public:
            inline void gpr0_set(uint32_t value);
            inline uint32_t gpr0_get();
        };

|

.. _secured_riscv_debug_GPR1:

GPR1
""""

Core general purpose register 1 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR1|General purpose register 1 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 1 value register.
                #define SECURED_RISCV_DEBUG_GPR1_OFFSET          0x404

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr1_get(uint32_t base);
        static inline void secured_riscv_debug_gpr1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 1 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR1_GPR1(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr1            :32; // General purpose register 1 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr1 : public vp::reg_32
        {
        public:
            inline void gpr1_set(uint32_t value);
            inline uint32_t gpr1_get();
        };

|

.. _secured_riscv_debug_GPR2:

GPR2
""""

Core general purpose register 2 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR2|General purpose register 2 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 2 value register.
                #define SECURED_RISCV_DEBUG_GPR2_OFFSET          0x408

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr2_get(uint32_t base);
        static inline void secured_riscv_debug_gpr2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 2 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR2_GPR2(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr2            :32; // General purpose register 2 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr2 : public vp::reg_32
        {
        public:
            inline void gpr2_set(uint32_t value);
            inline uint32_t gpr2_get();
        };

|

.. _secured_riscv_debug_GPR3:

GPR3
""""

Core general purpose register 3 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR3|General purpose register 3 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 3 value register.
                #define SECURED_RISCV_DEBUG_GPR3_OFFSET          0x40c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr3_get(uint32_t base);
        static inline void secured_riscv_debug_gpr3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 3 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR3_GPR3(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr3            :32; // General purpose register 3 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr3 : public vp::reg_32
        {
        public:
            inline void gpr3_set(uint32_t value);
            inline uint32_t gpr3_get();
        };

|

.. _secured_riscv_debug_GPR4:

GPR4
""""

Core general purpose register 4 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR4|General purpose register 4 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 4 value register.
                #define SECURED_RISCV_DEBUG_GPR4_OFFSET          0x410

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr4_get(uint32_t base);
        static inline void secured_riscv_debug_gpr4_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 4 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR4_GPR4(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr4            :32; // General purpose register 4 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr4_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr4 : public vp::reg_32
        {
        public:
            inline void gpr4_set(uint32_t value);
            inline uint32_t gpr4_get();
        };

|

.. _secured_riscv_debug_GPR5:

GPR5
""""

Core general purpose register 5 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR5|General purpose register 5 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 5 value register.
                #define SECURED_RISCV_DEBUG_GPR5_OFFSET          0x414

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr5_get(uint32_t base);
        static inline void secured_riscv_debug_gpr5_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 5 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR5_GPR5(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr5            :32; // General purpose register 5 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr5_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr5 : public vp::reg_32
        {
        public:
            inline void gpr5_set(uint32_t value);
            inline uint32_t gpr5_get();
        };

|

.. _secured_riscv_debug_GPR6:

GPR6
""""

Core general purpose register 6 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR6|General purpose register 6 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 6 value register.
                #define SECURED_RISCV_DEBUG_GPR6_OFFSET          0x418

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr6_get(uint32_t base);
        static inline void secured_riscv_debug_gpr6_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 6 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR6_GPR6(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr6            :32; // General purpose register 6 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr6_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr6 : public vp::reg_32
        {
        public:
            inline void gpr6_set(uint32_t value);
            inline uint32_t gpr6_get();
        };

|

.. _secured_riscv_debug_GPR7:

GPR7
""""

Core general purpose register 7 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR7|General purpose register 7 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 7 value register.
                #define SECURED_RISCV_DEBUG_GPR7_OFFSET          0x41c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr7_get(uint32_t base);
        static inline void secured_riscv_debug_gpr7_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 7 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR7_GPR7(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr7            :32; // General purpose register 7 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr7_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr7 : public vp::reg_32
        {
        public:
            inline void gpr7_set(uint32_t value);
            inline uint32_t gpr7_get();
        };

|

.. _secured_riscv_debug_GPR8:

GPR8
""""

Core general purpose register 8 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR8|General purpose register 8 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 8 value register.
                #define SECURED_RISCV_DEBUG_GPR8_OFFSET          0x420

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr8_get(uint32_t base);
        static inline void secured_riscv_debug_gpr8_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 8 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR8_GPR8(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr8            :32; // General purpose register 8 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr8_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr8 : public vp::reg_32
        {
        public:
            inline void gpr8_set(uint32_t value);
            inline uint32_t gpr8_get();
        };

|

.. _secured_riscv_debug_GPR9:

GPR9
""""

Core general purpose register 9 value register.

.. table:: 

    +-----+---+----+------------------------------------------+
    |Bit #|R/W|Name|               Description                |
    +=====+===+====+==========================================+
    |31:0 |R/W|GPR9|General purpose register 9 value bitfield.|
    +-----+---+----+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 9 value register.
                #define SECURED_RISCV_DEBUG_GPR9_OFFSET          0x424

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr9_get(uint32_t base);
        static inline void secured_riscv_debug_gpr9_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 9 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_BIT                            0
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_WIDTH                          32
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_MASK                           0xffffffff
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR9_GPR9(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr9            :32; // General purpose register 9 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr9_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr9 : public vp::reg_32
        {
        public:
            inline void gpr9_set(uint32_t value);
            inline uint32_t gpr9_get();
        };

|

.. _secured_riscv_debug_GPR10:

GPR10
"""""

Core general purpose register 10 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR10|General purpose register 10 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 10 value register.
                #define SECURED_RISCV_DEBUG_GPR10_OFFSET         0x428

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr10_get(uint32_t base);
        static inline void secured_riscv_debug_gpr10_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 10 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR10_GPR10(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr10           :32; // General purpose register 10 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr10_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr10 : public vp::reg_32
        {
        public:
            inline void gpr10_set(uint32_t value);
            inline uint32_t gpr10_get();
        };

|

.. _secured_riscv_debug_GPR11:

GPR11
"""""

Core general purpose register 11 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR11|General purpose register 11 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 11 value register.
                #define SECURED_RISCV_DEBUG_GPR11_OFFSET         0x42c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr11_get(uint32_t base);
        static inline void secured_riscv_debug_gpr11_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 11 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR11_GPR11(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr11           :32; // General purpose register 11 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr11_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr11 : public vp::reg_32
        {
        public:
            inline void gpr11_set(uint32_t value);
            inline uint32_t gpr11_get();
        };

|

.. _secured_riscv_debug_GPR12:

GPR12
"""""

Core general purpose register 12 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR12|General purpose register 12 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 12 value register.
                #define SECURED_RISCV_DEBUG_GPR12_OFFSET         0x430

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr12_get(uint32_t base);
        static inline void secured_riscv_debug_gpr12_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 12 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR12_GPR12(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr12           :32; // General purpose register 12 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr12_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr12 : public vp::reg_32
        {
        public:
            inline void gpr12_set(uint32_t value);
            inline uint32_t gpr12_get();
        };

|

.. _secured_riscv_debug_GPR13:

GPR13
"""""

Core general purpose register 13 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR13|General purpose register 13 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 13 value register.
                #define SECURED_RISCV_DEBUG_GPR13_OFFSET         0x434

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr13_get(uint32_t base);
        static inline void secured_riscv_debug_gpr13_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 13 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR13_GPR13(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr13           :32; // General purpose register 13 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr13_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr13 : public vp::reg_32
        {
        public:
            inline void gpr13_set(uint32_t value);
            inline uint32_t gpr13_get();
        };

|

.. _secured_riscv_debug_GPR14:

GPR14
"""""

Core general purpose register 14 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR14|General purpose register 14 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 14 value register.
                #define SECURED_RISCV_DEBUG_GPR14_OFFSET         0x438

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr14_get(uint32_t base);
        static inline void secured_riscv_debug_gpr14_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 14 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR14_GPR14(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr14           :32; // General purpose register 14 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr14_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr14 : public vp::reg_32
        {
        public:
            inline void gpr14_set(uint32_t value);
            inline uint32_t gpr14_get();
        };

|

.. _secured_riscv_debug_GPR15:

GPR15
"""""

Core general purpose register 15 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR15|General purpose register 15 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 15 value register.
                #define SECURED_RISCV_DEBUG_GPR15_OFFSET         0x43c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr15_get(uint32_t base);
        static inline void secured_riscv_debug_gpr15_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 15 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR15_GPR15(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr15           :32; // General purpose register 15 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr15_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr15 : public vp::reg_32
        {
        public:
            inline void gpr15_set(uint32_t value);
            inline uint32_t gpr15_get();
        };

|

.. _secured_riscv_debug_GPR16:

GPR16
"""""

Core general purpose register 16 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR16|General purpose register 16 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 16 value register.
                #define SECURED_RISCV_DEBUG_GPR16_OFFSET         0x440

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr16_get(uint32_t base);
        static inline void secured_riscv_debug_gpr16_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 16 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR16_GPR16(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr16           :32; // General purpose register 16 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr16_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr16 : public vp::reg_32
        {
        public:
            inline void gpr16_set(uint32_t value);
            inline uint32_t gpr16_get();
        };

|

.. _secured_riscv_debug_GPR17:

GPR17
"""""

Core general purpose register 17 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR17|General purpose register 17 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 17 value register.
                #define SECURED_RISCV_DEBUG_GPR17_OFFSET         0x444

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr17_get(uint32_t base);
        static inline void secured_riscv_debug_gpr17_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 17 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR17_GPR17(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr17           :32; // General purpose register 17 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr17_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr17 : public vp::reg_32
        {
        public:
            inline void gpr17_set(uint32_t value);
            inline uint32_t gpr17_get();
        };

|

.. _secured_riscv_debug_GPR18:

GPR18
"""""

Core general purpose register 18 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR18|General purpose register 18 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 18 value register.
                #define SECURED_RISCV_DEBUG_GPR18_OFFSET         0x448

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr18_get(uint32_t base);
        static inline void secured_riscv_debug_gpr18_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 18 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR18_GPR18(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr18           :32; // General purpose register 18 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr18_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr18 : public vp::reg_32
        {
        public:
            inline void gpr18_set(uint32_t value);
            inline uint32_t gpr18_get();
        };

|

.. _secured_riscv_debug_GPR19:

GPR19
"""""

Core general purpose register 19 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR19|General purpose register 19 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 19 value register.
                #define SECURED_RISCV_DEBUG_GPR19_OFFSET         0x44c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr19_get(uint32_t base);
        static inline void secured_riscv_debug_gpr19_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 19 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR19_GPR19(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr19           :32; // General purpose register 19 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr19_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr19 : public vp::reg_32
        {
        public:
            inline void gpr19_set(uint32_t value);
            inline uint32_t gpr19_get();
        };

|

.. _secured_riscv_debug_GPR20:

GPR20
"""""

Core general purpose register 20 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR20|General purpose register 20 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 20 value register.
                #define SECURED_RISCV_DEBUG_GPR20_OFFSET         0x450

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr20_get(uint32_t base);
        static inline void secured_riscv_debug_gpr20_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 20 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR20_GPR20(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr20           :32; // General purpose register 20 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr20_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr20 : public vp::reg_32
        {
        public:
            inline void gpr20_set(uint32_t value);
            inline uint32_t gpr20_get();
        };

|

.. _secured_riscv_debug_GPR21:

GPR21
"""""

Core general purpose register 21 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR21|General purpose register 21 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 21 value register.
                #define SECURED_RISCV_DEBUG_GPR21_OFFSET         0x454

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr21_get(uint32_t base);
        static inline void secured_riscv_debug_gpr21_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 21 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR21_GPR21(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr21           :32; // General purpose register 21 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr21_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr21 : public vp::reg_32
        {
        public:
            inline void gpr21_set(uint32_t value);
            inline uint32_t gpr21_get();
        };

|

.. _secured_riscv_debug_GPR22:

GPR22
"""""

Core general purpose register 22 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR22|General purpose register 22 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 22 value register.
                #define SECURED_RISCV_DEBUG_GPR22_OFFSET         0x458

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr22_get(uint32_t base);
        static inline void secured_riscv_debug_gpr22_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 22 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR22_GPR22(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr22           :32; // General purpose register 22 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr22_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr22 : public vp::reg_32
        {
        public:
            inline void gpr22_set(uint32_t value);
            inline uint32_t gpr22_get();
        };

|

.. _secured_riscv_debug_GPR23:

GPR23
"""""

Core general purpose register 23 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR23|General purpose register 23 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 23 value register.
                #define SECURED_RISCV_DEBUG_GPR23_OFFSET         0x45c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr23_get(uint32_t base);
        static inline void secured_riscv_debug_gpr23_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 23 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR23_GPR23(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr23           :32; // General purpose register 23 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr23_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr23 : public vp::reg_32
        {
        public:
            inline void gpr23_set(uint32_t value);
            inline uint32_t gpr23_get();
        };

|

.. _secured_riscv_debug_GPR24:

GPR24
"""""

Core general purpose register 24 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR24|General purpose register 24 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 24 value register.
                #define SECURED_RISCV_DEBUG_GPR24_OFFSET         0x460

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr24_get(uint32_t base);
        static inline void secured_riscv_debug_gpr24_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 24 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR24_GPR24(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr24           :32; // General purpose register 24 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr24_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr24 : public vp::reg_32
        {
        public:
            inline void gpr24_set(uint32_t value);
            inline uint32_t gpr24_get();
        };

|

.. _secured_riscv_debug_GPR25:

GPR25
"""""

Core general purpose register 25 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR25|General purpose register 25 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 25 value register.
                #define SECURED_RISCV_DEBUG_GPR25_OFFSET         0x464

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr25_get(uint32_t base);
        static inline void secured_riscv_debug_gpr25_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 25 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR25_GPR25(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr25           :32; // General purpose register 25 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr25_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr25 : public vp::reg_32
        {
        public:
            inline void gpr25_set(uint32_t value);
            inline uint32_t gpr25_get();
        };

|

.. _secured_riscv_debug_GPR26:

GPR26
"""""

Core general purpose register 26 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR26|General purpose register 26 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 26 value register.
                #define SECURED_RISCV_DEBUG_GPR26_OFFSET         0x468

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr26_get(uint32_t base);
        static inline void secured_riscv_debug_gpr26_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 26 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR26_GPR26(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr26           :32; // General purpose register 26 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr26_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr26 : public vp::reg_32
        {
        public:
            inline void gpr26_set(uint32_t value);
            inline uint32_t gpr26_get();
        };

|

.. _secured_riscv_debug_GPR27:

GPR27
"""""

Core general purpose register 27 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR27|General purpose register 27 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 27 value register.
                #define SECURED_RISCV_DEBUG_GPR27_OFFSET         0x46c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr27_get(uint32_t base);
        static inline void secured_riscv_debug_gpr27_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 27 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR27_GPR27(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr27           :32; // General purpose register 27 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr27_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr27 : public vp::reg_32
        {
        public:
            inline void gpr27_set(uint32_t value);
            inline uint32_t gpr27_get();
        };

|

.. _secured_riscv_debug_GPR28:

GPR28
"""""

Core general purpose register 28 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR28|General purpose register 28 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 28 value register.
                #define SECURED_RISCV_DEBUG_GPR28_OFFSET         0x470

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr28_get(uint32_t base);
        static inline void secured_riscv_debug_gpr28_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 28 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR28_GPR28(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr28           :32; // General purpose register 28 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr28_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr28 : public vp::reg_32
        {
        public:
            inline void gpr28_set(uint32_t value);
            inline uint32_t gpr28_get();
        };

|

.. _secured_riscv_debug_GPR29:

GPR29
"""""

Core general purpose register 29 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR29|General purpose register 29 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 29 value register.
                #define SECURED_RISCV_DEBUG_GPR29_OFFSET         0x474

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr29_get(uint32_t base);
        static inline void secured_riscv_debug_gpr29_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 29 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR29_GPR29(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr29           :32; // General purpose register 29 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr29_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr29 : public vp::reg_32
        {
        public:
            inline void gpr29_set(uint32_t value);
            inline uint32_t gpr29_get();
        };

|

.. _secured_riscv_debug_GPR30:

GPR30
"""""

Core general purpose register 30 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR30|General purpose register 30 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 30 value register.
                #define SECURED_RISCV_DEBUG_GPR30_OFFSET         0x478

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr30_get(uint32_t base);
        static inline void secured_riscv_debug_gpr30_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 30 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR30_GPR30(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr30           :32; // General purpose register 30 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr30_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr30 : public vp::reg_32
        {
        public:
            inline void gpr30_set(uint32_t value);
            inline uint32_t gpr30_get();
        };

|

.. _secured_riscv_debug_GPR31:

GPR31
"""""

Core general purpose register 31 value register.

.. table:: 

    +-----+---+-----+-------------------------------------------+
    |Bit #|R/W|Name |                Description                |
    +=====+===+=====+===========================================+
    |31:0 |R/W|GPR31|General purpose register 31 value bitfield.|
    +-----+---+-----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core general purpose register 31 value register.
                #define SECURED_RISCV_DEBUG_GPR31_OFFSET         0x47c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_gpr31_get(uint32_t base);
        static inline void secured_riscv_debug_gpr31_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // General purpose register 31 value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_BIT                          0
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_WIDTH                        32
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_MASK                         0xffffffff
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_GET(value)         (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_GETS(value)        (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31_SET(value,field)   (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_GPR31_GPR31(val)               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int gpr31           :32; // General purpose register 31 value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_gpr31_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_gpr31 : public vp::reg_32
        {
        public:
            inline void gpr31_set(uint32_t value);
            inline uint32_t gpr31_get();
        };

|

.. _secured_riscv_debug_CSR_MSTATUS:

CSR_MSTATUS
"""""""""""

Core CSR machine status value register.

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                     Description                                                                                                      |
    +=====+===+====+======================================================================================================================================================================================================================+
    |    0|R/W|UIE |User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                         |
    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|MIE |Machine privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                      |
    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|UPIE|User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.   |
    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|MPIE|Machine privilege mode previous interrupt enable value bitfield. When an interrupis encountered, MPIE will store the value existing in MIE. When mret instruction is executed, the value of MPIE is restored into MIE.|
    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12:11|R/W|MPP |Machine privilege mode previous privilege mode value bitfield: - 2’b00: User mode - 2’b11: Machine mode                                                                                                               |
    +-----+---+----+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR machine status value register.
                #define SECURED_RISCV_DEBUG_CSR_MSTATUS_OFFSET   0x4c00

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_mstatus_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mstatus_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_MASK                     0x1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_RESET                    0x0
        
        // Machine privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_BIT                      3
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_MASK                     0x8
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_RESET                    0x0
        
        // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_BIT                     4
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_MASK                    0x10
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_RESET                   0x0
        
        // Machine privilege mode previous interrupt enable value bitfield. When an interrupis encountered, MPIE will store the value existing in MIE. When mret instruction is executed, the value of MPIE is restored into MIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_BIT                     7
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_MASK                    0x80
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_RESET                   0x0
        
        // Machine privilege mode previous privilege mode value bitfield: - 2b00: User mode - 2b11: Machine mode (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_BIT                      11
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_WIDTH                    2
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_MASK                     0x1800
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_GET(value)     (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_GETS(value)    (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UIE(val)           ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_GET(value)     (GAP_BEXTRACTU((value),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_GETS(value)    (GAP_BEXTRACT((value),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MIE(val)           ((val) << 3)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_UPIE(val)          ((val) << 4)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_GET(value)    (GAP_BEXTRACTU((value),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_GETS(value)   (GAP_BEXTRACT((value),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPIE(val)          ((val) << 7)
        
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_GET(value)     (GAP_BEXTRACTU((value),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_GETS(value)    (GAP_BEXTRACT((value),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP_SET(value,field) (GAP_BINSERT((value),(field),2,11))
        #define SECURED_RISCV_DEBUG_CSR_MSTATUS_MPP(val)           ((val) << 11)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int uie             :1 ; // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:2 ;
            unsigned int mie             :1 ; // Machine privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int upie            :1 ; // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.
            unsigned int padding1:2 ;
            unsigned int mpie            :1 ; // Machine privilege mode previous interrupt enable value bitfield. When an interrupis encountered, MPIE will store the value existing in MIE. When mret instruction is executed, the value of MPIE is restored into MIE.
            unsigned int padding2:3 ;
            unsigned int mpp             :2 ; // Machine privilege mode previous privilege mode value bitfield: - 2b00: User mode - 2b11: Machine mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mstatus_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_mstatus : public vp::reg_32
        {
        public:
            inline void uie_set(uint32_t value);
            inline uint32_t uie_get();
            inline void mie_set(uint32_t value);
            inline uint32_t mie_get();
            inline void upie_set(uint32_t value);
            inline uint32_t upie_get();
            inline void mpie_set(uint32_t value);
            inline uint32_t mpie_get();
            inline void mpp_set(uint32_t value);
            inline uint32_t mpp_get();
        };

|

.. _secured_riscv_debug_CSR_MTVEC:

CSR_MTVEC
"""""""""

Core CSR machine vector-trap base address value register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                                  Description                                                                                  |
    +=====+===+=====+===============================================================================================================================================================================+
    |31:8 |R/W|MTVEC|Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. |
    +-----+---+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR machine vector-trap base address value register.
                #define SECURED_RISCV_DEBUG_CSR_MTVEC_OFFSET     0x4c14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_mtvec_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mtvec_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address.  (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_BIT                      8
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_WIDTH                    24
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_MASK                     0xffffff00
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_GET(value)     (GAP_BEXTRACTU((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_GETS(value)    (GAP_BEXTRACT((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC_SET(value,field) (GAP_BINSERT((value),(field),24,8))
        #define SECURED_RISCV_DEBUG_CSR_MTVEC_MTVEC(val)           ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:8 ;
            unsigned int mtvec           :24; // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. 
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mtvec_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_mtvec : public vp::reg_32
        {
        public:
            inline void mtvec_set(uint32_t value);
            inline uint32_t mtvec_get();
        };

|

.. _secured_riscv_debug_CSR_MEPC:

CSR_MEPC
""""""""

Core CSR machine exception program counter value register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                    Description                                                                                                                                     |
    +=====+===+====+====================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|MEPC|Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR machine exception program counter value register.
                #define SECURED_RISCV_DEBUG_CSR_MEPC_OFFSET      0x4d04

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_mepc_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mepc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_MEPC_MEPC(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int mepc            :32; // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mepc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_mepc : public vp::reg_32
        {
        public:
            inline void mepc_set(uint32_t value);
            inline uint32_t mepc_get();
        };

|

.. _secured_riscv_debug_CSR_MCAUSE:

CSR_MCAUSE
""""""""""

Core CSR machine trap cause value register.

.. table:: 

    +-----+---+-----+----------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                      Description                                       |
    +=====+===+=====+========================================================================================+
    |4:0  |R  |CAUSE|Exception ID bitfield.                                                                  |
    +-----+---+-----+----------------------------------------------------------------------------------------+
    |31   |R  |IRQ  |Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true|
    +-----+---+-----+----------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR machine trap cause value register.
                #define SECURED_RISCV_DEBUG_CSR_MCAUSE_OFFSET    0x4d08

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_mcause_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mcause_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_WIDTH                   5
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_MASK                    0x1f
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_RESET                   0x0
        
        // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_BIT                       31
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_WIDTH                     1
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_MASK                      0x80000000
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_CAUSE(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CSR_MCAUSE_IRQ(val)            ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mcause_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_mcause : public vp::reg_32
        {
        public:
            inline void cause_set(uint32_t value);
            inline uint32_t cause_get();
            inline void irq_set(uint32_t value);
            inline uint32_t irq_get();
        };

|

.. _secured_riscv_debug_CSR_PCCR:

CSR_PCCR
""""""""

Core CSR performance counter counter register.

.. table:: 

    +-----+---+----+---------------------------------------+
    |Bit #|R/W|Name|              Description              |
    +=====+===+====+=======================================+
    |31:0 |R/W|PCCR|Program counter counter value bitfield.|
    +-----+---+----+---------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR performance counter counter register.
                #define SECURED_RISCV_DEBUG_CSR_PCCR_OFFSET      0x5e00

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_pccr_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pccr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Program counter counter value bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_PCCR_PCCR(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pccr            :32; // Program counter counter value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pccr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_pccr : public vp::reg_32
        {
        public:
            inline void pccr_set(uint32_t value);
            inline uint32_t pccr_get();
        };

|

.. _secured_riscv_debug_CSR_PCER:

CSR_PCER
""""""""

Core CSR performance counter enable configuration register.

.. table:: 

    +-----+---+----+-----------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                         Description                                                         |
    +=====+===+====+=============================================================================================================================+
    |20:0 |R/W|PCER|See documentation on [RI5CY core](<https://pulp-platform.org//wp-content/uploads/2017/11/ri5cy_user_manual.pdf>) for details.|
    +-----+---+----+-----------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR performance counter enable configuration register.
                #define SECURED_RISCV_DEBUG_CSR_PCER_OFFSET      0x5e80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_pcer_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pcer_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // See documentation on [RI5CY core](<https://pulp-platform.org//wp-content/uploads/2017/11/ri5cy_user_manual.pdf>) for details. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_WIDTH                      21
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_MASK                       0x1fffff
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_GET(value)       (GAP_BEXTRACTU((value),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_GETS(value)      (GAP_BEXTRACT((value),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER_SET(value,field) (GAP_BINSERT((value),(field),21,0))
        #define SECURED_RISCV_DEBUG_CSR_PCER_PCER(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int pcer            :21; // See documentation on [RI5CY core](<https://pulp-platform.org//wp-content/uploads/2017/11/ri5cy_user_manual.pdf>) for details.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pcer_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_pcer : public vp::reg_32
        {
        public:
            inline void pcer_set(uint32_t value);
            inline uint32_t pcer_get();
        };

|

.. _secured_riscv_debug_CSR_PCMR:

CSR_PCMR
""""""""

Core CSR performance counter mode configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                Description                                                 |
    +=====+===+====+============================================================================================================+
    |    0|R/W|SAT |Performance counter saturation mode configuration bitfield: - 1'b0: wrap-around mode - 1'b1: saturation mode|
    +-----+---+----+------------------------------------------------------------------------------------------------------------+
    |    1|R/W|GE  |Performance counter activation configuration bitfield: - 1'b0: disabled - 1'b1: enabled                     |
    +-----+---+----+------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR performance counter mode configuration register.
                #define SECURED_RISCV_DEBUG_CSR_PCMR_OFFSET      0x5e84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_pcmr_get(uint32_t base);
        static inline void secured_riscv_debug_csr_pcmr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Performance counter saturation mode configuration bitfield: - 1'b0: wrap-around mode - 1'b1: saturation mode (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_BIT                         0
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_WIDTH                       1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_MASK                        0x1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_RESET                       0x0
        
        // Performance counter activation configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_BIT                          1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_WIDTH                        1
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_MASK                         0x2
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_SAT(val)              ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_GET(value)         (GAP_BEXTRACTU((value),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_GETS(value)        (GAP_BEXTRACT((value),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE_SET(value,field)   (GAP_BINSERT((value),(field),1,1))
        #define SECURED_RISCV_DEBUG_CSR_PCMR_GE(val)               ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sat             :1 ; // Performance counter saturation mode configuration bitfield: - 1'b0: wrap-around mode - 1'b1: saturation mode
            unsigned int ge              :1 ; // Performance counter activation configuration bitfield: - 1'b0: disabled - 1'b1: enabled
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_pcmr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_pcmr : public vp::reg_32
        {
        public:
            inline void sat_set(uint32_t value);
            inline uint32_t sat_get();
            inline void ge_set(uint32_t value);
            inline uint32_t ge_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP0S:

CSR_HWLP0S
""""""""""

Core CSR hardware loop 0 start configuration register.

.. table:: 

    +-----+---+-----+---------------------------------------------------+
    |Bit #|R/W|Name |                    Description                    |
    +=====+===+=====+===================================================+
    |31:0 |R/W|START|Hardware loop start address configuration bitfield.|
    +-----+---+-----+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 0 start configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0S_OFFSET    0x5ec0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp0s_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0s_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop start address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_WIDTH                   32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_MASK                    0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0S_START(val)          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :32; // Hardware loop start address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0s_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp0s : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP0E:

CSR_HWLP0E
""""""""""

Core CSR hardware loop 0 end configuration register.

.. table:: 

    +-----+---+----+-------------------------------------------------+
    |Bit #|R/W|Name|                   Description                   |
    +=====+===+====+=================================================+
    |31:0 |R/W|END |Hardware loop end address configuration bitfield.|
    +-----+---+----+-------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 0 end configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0E_OFFSET    0x5ec4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp0e_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0e_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop end address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0E_END(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int end             :32; // Hardware loop end address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0e_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp0e : public vp::reg_32
        {
        public:
            inline void end_set(uint32_t value);
            inline uint32_t end_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP0C:

CSR_HWLP0C
""""""""""

Core CSR hardware loop 0 counter configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------+
    |Bit #|R/W|Name|                 Description                 |
    +=====+===+====+=============================================+
    |31:0 |R/W|CNT |Hardware loop counter configuration bitfield.|
    +-----+---+----+---------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 0 counter configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP0C_OFFSET    0x5ec8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp0c_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp0c_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop counter configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP0C_CNT(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :32; // Hardware loop counter configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp0c_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp0c : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP1S:

CSR_HWLP1S
""""""""""

Core CSR hardware loop 1 start configuration register.

.. table:: 

    +-----+---+-----+---------------------------------------------------+
    |Bit #|R/W|Name |                    Description                    |
    +=====+===+=====+===================================================+
    |31:0 |R/W|START|Hardware loop start address configuration bitfield.|
    +-----+---+-----+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 1 start configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1S_OFFSET    0x5ed0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp1s_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1s_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop start address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_WIDTH                   32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_MASK                    0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_GET(value)    (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_GETS(value)   (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1S_START(val)          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :32; // Hardware loop start address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1s_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp1s : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP1E:

CSR_HWLP1E
""""""""""

Core CSR hardware loop 1 end configuration register.

.. table:: 

    +-----+---+----+-------------------------------------------------+
    |Bit #|R/W|Name|                   Description                   |
    +=====+===+====+=================================================+
    |31:0 |R/W|END |Hardware loop end address configuration bitfield.|
    +-----+---+----+-------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 1 end configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1E_OFFSET    0x5ed4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp1e_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1e_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop end address configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1E_END(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int end             :32; // Hardware loop end address configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1e_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp1e : public vp::reg_32
        {
        public:
            inline void end_set(uint32_t value);
            inline uint32_t end_get();
        };

|

.. _secured_riscv_debug_CSR_HWLP1C:

CSR_HWLP1C
""""""""""

Core CSR hardware loop 1 counter configuration register.

.. table:: 

    +-----+---+----+---------------------------------------------+
    |Bit #|R/W|Name|                 Description                 |
    +=====+===+====+=============================================+
    |31:0 |R/W|CNT |Hardware loop counter configuration bitfield.|
    +-----+---+----+---------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR hardware loop 1 counter configuration register.
                #define SECURED_RISCV_DEBUG_CSR_HWLP1C_OFFSET    0x5ed8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_hwlp1c_get(uint32_t base);
        static inline void secured_riscv_debug_csr_hwlp1c_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware loop counter configuration bitfield. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_BIT                       0
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_WIDTH                     32
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_MASK                      0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_HWLP1C_CNT(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cnt             :32; // Hardware loop counter configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_hwlp1c_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_hwlp1c : public vp::reg_32
        {
        public:
            inline void cnt_set(uint32_t value);
            inline uint32_t cnt_get();
        };

|

.. _secured_riscv_debug_CSR_PRIVLV:

CSR_PRIVLV
""""""""""

Cose CSR current privilege level status register.

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                   Description                                   |
    +=====+===+====+=================================================================================+
    |1:0  |R  |PLEV|Current privilege level status bitfield: - 2’b00: User mode - 2’b11: Machine mode|
    +-----+---+----+---------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Cose CSR current privilege level status register.
                #define SECURED_RISCV_DEBUG_CSR_PRIVLV_OFFSET    0x7040

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_privlv_get(uint32_t base);
        static inline void secured_riscv_debug_csr_privlv_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Current privilege level status bitfield: - 2b00: User mode - 2b11: Machine mode (access: R)
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_WIDTH                    2
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_MASK                     0x3
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_GET(value)     (GAP_BEXTRACTU((value),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_GETS(value)    (GAP_BEXTRACT((value),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define SECURED_RISCV_DEBUG_CSR_PRIVLV_PLEV(val)           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int plev            :2 ; // Current privilege level status bitfield: - 2b00: User mode - 2b11: Machine mode
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_privlv_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_privlv : public vp::reg_32
        {
        public:
            inline void plev_set(uint32_t value);
            inline uint32_t plev_get();
        };

|

.. _secured_riscv_debug_CSR_UHARTID:

CSR_UHARTID
"""""""""""

Core CSR user privilege mode hardware thread ID status register.

.. table:: 

    +-----+---+----------+-----------------------------+
    |Bit #|R/W|   Name   |         Description         |
    +=====+===+==========+=============================+
    |3:0  |R  |CORE_ID   |RI5CY core ID value bitfield.|
    +-----+---+----------+-----------------------------+
    |10:5 |R  |CLUSTER_ID|Cluster ID value bitfield.   |
    +-----+---+----------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR user privilege mode hardware thread ID status register.
                #define SECURED_RISCV_DEBUG_CSR_UHARTID_OFFSET   0x4050

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_uhartid_get(uint32_t base);
        static inline void secured_riscv_debug_csr_uhartid_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // RI5CY core ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_BIT                  0
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_WIDTH                4
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_MASK                 0xf
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_RESET                0x0
        
        // Cluster ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_BIT               5
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_WIDTH             6
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_MASK              0x7e0
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_RESET             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CORE_ID(val)       ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_GET(value) (GAP_BEXTRACTU((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_GETS(value) (GAP_BEXTRACT((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID_SET(value,field) (GAP_BINSERT((value),(field),6,5))
        #define SECURED_RISCV_DEBUG_CSR_UHARTID_CLUSTER_ID(val)    ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core_id         :4 ; // RI5CY core ID value bitfield.
            unsigned int padding0:1 ;
            unsigned int cluster_id      :6 ; // Cluster ID value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_uhartid_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_uhartid : public vp::reg_32
        {
        public:
            inline void core_id_set(uint32_t value);
            inline uint32_t core_id_get();
            inline void cluster_id_set(uint32_t value);
            inline uint32_t cluster_id_get();
        };

|

.. _secured_riscv_debug_CSR_MHARTID:

CSR_MHARTID
"""""""""""

Core CSR machine privilege mode hardware thread ID status register.

.. table:: 

    +-----+---+----------+-----------------------------+
    |Bit #|R/W|   Name   |         Description         |
    +=====+===+==========+=============================+
    |3:0  |R  |CORE_ID   |RI5CY core ID value bitfield.|
    +-----+---+----------+-----------------------------+
    |10:5 |R  |CLUSTER_ID|Cluster ID value bitfield.   |
    +-----+---+----------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR machine privilege mode hardware thread ID status register.
                #define SECURED_RISCV_DEBUG_CSR_MHARTID_OFFSET   0x7c50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_mhartid_get(uint32_t base);
        static inline void secured_riscv_debug_csr_mhartid_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // RI5CY core ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_BIT                  0
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_WIDTH                4
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_MASK                 0xf
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_RESET                0x0
        
        // Cluster ID value bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_BIT               5
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_WIDTH             6
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_MASK              0x7e0
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_RESET             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CORE_ID(val)       ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_GET(value) (GAP_BEXTRACTU((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_GETS(value) (GAP_BEXTRACT((value),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID_SET(value,field) (GAP_BINSERT((value),(field),6,5))
        #define SECURED_RISCV_DEBUG_CSR_MHARTID_CLUSTER_ID(val)    ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int core_id         :4 ; // RI5CY core ID value bitfield.
            unsigned int padding0:1 ;
            unsigned int cluster_id      :6 ; // Cluster ID value bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_mhartid_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_mhartid : public vp::reg_32
        {
        public:
            inline void core_id_set(uint32_t value);
            inline uint32_t core_id_get();
            inline void cluster_id_set(uint32_t value);
            inline uint32_t cluster_id_get();
        };

|

.. _secured_riscv_debug_CSR_USTATUS:

CSR_USTATUS
"""""""""""

Core CSR user status value register.

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                    Description                                                                                                    |
    +=====+===+====+===================================================================================================================================================================================================================+
    |    0|R/W|UIE |User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled                                                                                                                      |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|UPIE|User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR user status value register.
                #define SECURED_RISCV_DEBUG_CSR_USTATUS_OFFSET   0x4014

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_ustatus_get(uint32_t base);
        static inline void secured_riscv_debug_csr_ustatus_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_BIT                      0
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_WIDTH                    1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_MASK                     0x1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_RESET                    0x0
        
        // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_BIT                     4
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_WIDTH                   1
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_MASK                    0x10
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_GET(value)     (GAP_BEXTRACTU((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_GETS(value)    (GAP_BEXTRACT((value),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UIE(val)           ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_GET(value)    (GAP_BEXTRACTU((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_GETS(value)   (GAP_BEXTRACT((value),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE_SET(value,field) (GAP_BINSERT((value),(field),1,4))
        #define SECURED_RISCV_DEBUG_CSR_USTATUS_UPIE(val)          ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int uie             :1 ; // User privilege mode interrupt enable configuration bitfield: - 1'b0: disabled - 1'b1: enabled
            unsigned int padding0:3 ;
            unsigned int upie            :1 ; // User privilege mode previous interrupt enable value bitfield. When an interrupis encountered, UPIE will store the value existing in UIE. When uret instruction is executed, the value of UPIE is restored into UIE.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_ustatus_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_ustatus : public vp::reg_32
        {
        public:
            inline void uie_set(uint32_t value);
            inline uint32_t uie_get();
            inline void upie_set(uint32_t value);
            inline uint32_t upie_get();
        };

|

.. _secured_riscv_debug_CSR_UTVEC:

CSR_UTVEC
"""""""""

Core CSR user vector-trap base address value register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                                                                  Description                                                                                  |
    +=====+===+=====+===============================================================================================================================================================================+
    |31:8 |R/W|UTVEC|Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. |
    +-----+---+-----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR user vector-trap base address value register.
                #define SECURED_RISCV_DEBUG_CSR_UTVEC_OFFSET     0x4050

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_utvec_get(uint32_t base);
        static inline void secured_riscv_debug_csr_utvec_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address.  (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_BIT                      8
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_WIDTH                    24
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_MASK                     0xffffff00
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_GET(value)     (GAP_BEXTRACTU((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_GETS(value)    (GAP_BEXTRACT((value),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC_SET(value,field) (GAP_BINSERT((value),(field),24,8))
        #define SECURED_RISCV_DEBUG_CSR_UTVEC_UTVEC(val)           ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:8 ;
            unsigned int utvec           :24; // Machine trap-vector base address value bitfield. When an exception is encountered, the core jumps to the corresponding handler using the content of the MTVEC as base address. 
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_utvec_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_utvec : public vp::reg_32
        {
        public:
            inline void utvec_set(uint32_t value);
            inline uint32_t utvec_get();
        };

|

.. _secured_riscv_debug_CSR_UEPC:

CSR_UEPC
""""""""

Core CSR user exception program counter value register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                    Description                                                                                                                                     |
    +=====+===+====+====================================================================================================================================================================================================================================================================================+
    |31:0 |R/W|UEPC|Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR user exception program counter value register.
                #define SECURED_RISCV_DEBUG_CSR_UEPC_OFFSET      0x4104

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_uepc_get(uint32_t base);
        static inline void secured_riscv_debug_csr_uepc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter. (access: R/W)
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_BIT                        0
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_WIDTH                      32
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_MASK                       0xffffffff
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_GET(value)       (GAP_BEXTRACTU((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_GETS(value)      (GAP_BEXTRACT((value),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define SECURED_RISCV_DEBUG_CSR_UEPC_UEPC(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int uepc            :32; // Machine exception program counter value bitfield. When an exception is encountered, the current program counter is saved in MEPC, and the core jumps to the exception address. When an mret instruction is executed, the value from MEPC is restored to the current program counter.
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_uepc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_uepc : public vp::reg_32
        {
        public:
            inline void uepc_set(uint32_t value);
            inline uint32_t uepc_get();
        };

|

.. _secured_riscv_debug_CSR_UCAUSE:

CSR_UCAUSE
""""""""""

Core CSR user trap cause value register.

.. table:: 

    +-----+---+-----+----------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                      Description                                       |
    +=====+===+=====+========================================================================================+
    |4:0  |R  |CAUSE|Exception ID bitfield.                                                                  |
    +-----+---+-----+----------------------------------------------------------------------------------------+
    |31   |R  |IRQ  |Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true|
    +-----+---+-----+----------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Core CSR user trap cause value register.
                #define SECURED_RISCV_DEBUG_CSR_UCAUSE_OFFSET    0x4108

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t secured_riscv_debug_csr_ucause_get(uint32_t base);
        static inline void secured_riscv_debug_csr_ucause_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Exception ID bitfield. (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_BIT                     0
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_WIDTH                   5
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_MASK                    0x1f
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_RESET                   0x0
        
        // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true (access: R)
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_BIT                       31
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_WIDTH                     1
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_MASK                      0x80000000
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_GET(value)    (GAP_BEXTRACTU((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_GETS(value)   (GAP_BEXTRACT((value),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE_SET(value,field) (GAP_BINSERT((value),(field),5,0))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_CAUSE(val)          ((val) << 0)
        
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define SECURED_RISCV_DEBUG_CSR_UCAUSE_IRQ(val)            ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int cause           :5 ; // Exception ID bitfield.
            unsigned int padding0:26;
            unsigned int irq             :1 ; // Core triggered an exception due to interrupt status bitfield: - 1'b0: false - 1'b1: true
          };
          unsigned int raw;
        } __attribute__((packed)) secured_riscv_debug_csr_ucause_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_secured_riscv_debug_csr_ucause : public vp::reg_32
        {
        public:
            inline void cause_set(uint32_t value);
            inline uint32_t cause_get();
            inline void irq_set(uint32_t value);
            inline uint32_t irq_get();
        };

|
