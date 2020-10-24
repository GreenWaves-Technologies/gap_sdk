Input file: fe/ips/power_manager_vega/docs/DLC_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------+------+-----+--------------------------------------------+
    |                      Name                       |Offset|Width|                Description                 |
    +=================================================+======+=====+============================================+
    |:ref:`DLCPD_MSR<power_manager_DLCPD_MSR>`        |     0|   32|Maestro Status Register.                    |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_MPACR<power_manager_DLCPD_MPACR>`    |     4|   32|Maestro PICL Access Control Register.       |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_MPADR<power_manager_DLCPD_MPADR>`    |     8|   32|Maestro PICL Access Data Register.          |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IMR<power_manager_DLCPD_IMR>`        |    12|   32|Maestro Interrupt Mask Register.            |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IFR<power_manager_DLCPD_IFR>`        |    16|   32|Maestro Interrupt Flag Register.            |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IOK_IFR<power_manager_DLCPD_IOK_IFR>`|    20|   32|Maestro ICU_OK Interrupt Flag Register.     |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IDL_IFR<power_manager_DLCPD_IDL_IFR>`|    24|   32|Maestro ICU_DELAYED Interrupt Flag Register.|
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IDN_IFR<power_manager_DLCPD_IDN_IFR>`|    28|   32|Maestro ICU_DENIED Interrupt Flag Register. |
    +-------------------------------------------------+------+-----+--------------------------------------------+
    |:ref:`DLCPD_IUP_IFR<power_manager_DLCPD_IUP_IFR>`|    32|   32|Maestro ICU_UPDATED Interrupt Flag Register.|
    +-------------------------------------------------+------+-----+--------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro Status Register.
                #define POWER_MANAGER_DLCPD_MSR_OFFSET           0x0
        
                // Maestro PICL Access Control Register.
                #define POWER_MANAGER_DLCPD_MPACR_OFFSET         0x4
        
                // Maestro PICL Access Data Register.
                #define POWER_MANAGER_DLCPD_MPADR_OFFSET         0x8
        
                // Maestro Interrupt Mask Register.
                #define POWER_MANAGER_DLCPD_IMR_OFFSET           0xc
        
                // Maestro Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IFR_OFFSET           0x10
        
                // Maestro ICU_OK Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IOK_IFR_OFFSET       0x14
        
                // Maestro ICU_DELAYED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IDL_IFR_OFFSET       0x18
        
                // Maestro ICU_DENIED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IDN_IFR_OFFSET       0x1c
        
                // Maestro ICU_UPDATED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IUP_IFR_OFFSET       0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_msr_get(uint32_t base);
        static inline void power_manager_dlcpd_msr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_mpacr_get(uint32_t base);
        static inline void power_manager_dlcpd_mpacr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_mpadr_get(uint32_t base);
        static inline void power_manager_dlcpd_mpadr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_imr_get(uint32_t base);
        static inline void power_manager_dlcpd_imr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_ifr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_iok_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_iok_ifr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_idl_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_idl_ifr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_idn_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_idn_ifr_set(uint32_t base, uint32_t value);

        static inline uint32_t power_manager_dlcpd_iup_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_iup_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer. (access: W)
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_BIT                        0
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_WIDTH                      1
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_MASK                       0x1
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_RESET                      0x0
        
        // SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends. (access: W)
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_BIT                         1
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_WIDTH                       1
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_MASK                        0x2
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_RESET                       0x0
        
        // PICL Access Address. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_BIT                         0
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_WIDTH                       16
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_MASK                        0xffff
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_RESET                       0x0
        
        // PICL Access Direction : - 1'b0: write  - 1'b1: read. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_BIT                          24
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_WIDTH                        1
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_MASK                         0x1000000
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_RESET                        0x0
        
        // PICL Access Start. This field is automatically cleared when PICL access is finished. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_BIT                        28
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_WIDTH                      1
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_MASK                       0x10000000
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_RESET                      0x0
        
        // PICL Read/Write Data. This field is automatically updated after a PICL read access is finished. (access: W)
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_BIT                        0
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_WIDTH                      16
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_MASK                       0xffff
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_RESET                      0x0
        
        // Mask of ICU_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_BIT                         0
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_MASK                        0x1
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_RESET                       0x0
        
        // Mask of ICU_DELAYED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_BIT                        1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_MASK                       0x2
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_RESET                      0x0
        
        // Mask of ICU_DENIED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_BIT                        2
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_MASK                       0x4
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_RESET                      0x0
        
        // Mask of ICU_UPDATED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_BIT                        3
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_MASK                       0x8
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_RESET                      0x0
        
        // Mask of PICL_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_BIT                        6
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_MASK                       0x40
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_RESET                      0x0
        
        // Mask of SCU_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_BIT                         7
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_MASK                        0x80
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_RESET                       0x0
        
        // Mask of SCU_FL interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_BIT                         8
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_MASK                        0x100
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_RESET                       0x0
        
        // Set when at least one of the bit of the DLCPD_IOK_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_BIT                         0
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_MASK                        0x1
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_RESET                       0x0
        
        // Set when at least one of the bit of the DLCPD_IDL_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_BIT                        1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_MASK                       0x2
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_RESET                      0x0
        
        // Set when at least one of the bit of the DLCPD_IDN_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_BIT                        2
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_MASK                       0x4
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_RESET                      0x0
        
        // Set when at least one of the bit of the DLCPD_IUP_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_BIT                        3
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_MASK                       0x8
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_RESET                      0x0
        
        // Set when PICL transfer is finished. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_BIT                        6
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_MASK                       0x40
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_RESET                      0x0
        
        // Set when SCU sequence is finished without error. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_BIT                         7
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_MASK                        0x80
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_RESET                       0x0
        
        // Set when SCU sequence is finished with error. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_BIT                         8
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_MASK                        0x100
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_RESET                       0x0
        
        // Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_BIT                 0
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_WIDTH               32
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_MASK                0xffffffff
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_RESET               0x0
        
        // Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_RESET              0x0
        
        // Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_RESET              0x0
        
        // Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_GET(value)       (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_GETS(value)      (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY(val)             ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY(val)              ((val) << 1)
        
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_GET(value)        (GAP_BEXTRACTU((value),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_GETS(value)       (GAP_BEXTRACT((value),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_GET(value)         (GAP_BEXTRACTU((value),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_GETS(value)        (GAP_BEXTRACT((value),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_SET(value,field)   (GAP_BINSERT((value),(field),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR(val)               ((val) << 24)
        
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_GET(value)       (GAP_BEXTRACTU((value),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_GETS(value)      (GAP_BEXTRACT((value),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_SET(value,field) (GAP_BINSERT((value),(field),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART(val)             ((val) << 28)
        
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_GET(value)       (GAP_BEXTRACTU((value),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_GETS(value)      (GAP_BEXTRACT((value),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA(val)             ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_GET(value)       (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_GETS(value)      (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M(val)             ((val) << 1)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_GET(value)       (GAP_BEXTRACTU((value),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_GETS(value)      (GAP_BEXTRACT((value),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M(val)             ((val) << 2)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_GET(value)       (GAP_BEXTRACTU((value),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_GETS(value)      (GAP_BEXTRACT((value),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M(val)             ((val) << 3)
        
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_GET(value)       (GAP_BEXTRACTU((value),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_GETS(value)      (GAP_BEXTRACT((value),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M(val)             ((val) << 6)
        
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M(val)              ((val) << 7)
        
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_GET(value)        (GAP_BEXTRACTU((value),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_GETS(value)       (GAP_BEXTRACT((value),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_SET(value,field)  (GAP_BINSERT((value),(field),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M(val)              ((val) << 8)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_GET(value)       (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_GETS(value)      (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F(val)             ((val) << 1)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_GET(value)       (GAP_BEXTRACTU((value),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_GETS(value)      (GAP_BEXTRACT((value),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F(val)             ((val) << 2)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_GET(value)       (GAP_BEXTRACTU((value),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_GETS(value)      (GAP_BEXTRACT((value),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F(val)             ((val) << 3)
        
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_GET(value)       (GAP_BEXTRACTU((value),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_GETS(value)      (GAP_BEXTRACT((value),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F(val)             ((val) << 6)
        
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F(val)              ((val) << 7)
        
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_GET(value)        (GAP_BEXTRACTU((value),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_GETS(value)       (GAP_BEXTRACT((value),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_SET(value,field)  (GAP_BINSERT((value),(field),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F(val)              ((val) << 8)
        
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS(val)      ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS(val)     ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS(val)     ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS(val)     ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** POWER_MANAGER_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t dlcpd_msr;  // Maestro Status Register.
            volatile uint32_t dlcpd_mpacr;  // Maestro PICL Access Control Register.
            volatile uint32_t dlcpd_mpadr;  // Maestro PICL Access Data Register.
            volatile uint32_t dlcpd_imr;  // Maestro Interrupt Mask Register.
            volatile uint32_t dlcpd_ifr;  // Maestro Interrupt Flag Register.
            volatile uint32_t dlcpd_iok_ifr;  // Maestro ICU_OK Interrupt Flag Register.
            volatile uint32_t dlcpd_idl_ifr;  // Maestro ICU_DELAYED Interrupt Flag Register.
            volatile uint32_t dlcpd_idn_ifr;  // Maestro ICU_DENIED Interrupt Flag Register.
            volatile uint32_t dlcpd_iup_ifr;  // Maestro ICU_UPDATED Interrupt Flag Register.
        } __attribute__((packed)) power_manager_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int picl_busy       :1 ; // PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer.
            unsigned int scu_busy        :1 ; // SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_msr_t;
        
        typedef union {
          struct {
            unsigned int paaddr          :16; // PICL Access Address.
            unsigned int padding0:8 ;
            unsigned int padir           :1 ; // PICL Access Direction : - 1'b0: write  - 1'b1: read.
            unsigned int padding1:3 ;
            unsigned int pastart         :1 ; // PICL Access Start. This field is automatically cleared when PICL access is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_mpacr_t;
        
        typedef union {
          struct {
            unsigned int prwdata         :16; // PICL Read/Write Data. This field is automatically updated after a PICL read access is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_mpadr_t;
        
        typedef union {
          struct {
            unsigned int icu_ok_m        :1 ; // Mask of ICU_OK interrupt.
            unsigned int icu_dly_m       :1 ; // Mask of ICU_DELAYED interrupt.
            unsigned int icu_den_m       :1 ; // Mask of ICU_DENIED interrupt.
            unsigned int icu_upd_m       :1 ; // Mask of ICU_UPDATED interrupt.
            unsigned int padding0:2 ;
            unsigned int picl_ok_m       :1 ; // Mask of PICL_OK interrupt.
            unsigned int scu_ok_m        :1 ; // Mask of SCU_OK interrupt.
            unsigned int scu_fl_m        :1 ; // Mask of SCU_FL interrupt.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_imr_t;
        
        typedef union {
          struct {
            unsigned int icu_ok_f        :1 ; // Set when at least one of the bit of the DLCPD_IOK_IFR register is set.
            unsigned int icu_dly_f       :1 ; // Set when at least one of the bit of the DLCPD_IDL_IFR register is set.
            unsigned int icu_den_f       :1 ; // Set when at least one of the bit of the DLCPD_IDN_IFR register is set.
            unsigned int icu_upd_f       :1 ; // Set when at least one of the bit of the DLCPD_IUP_IFR register is set.
            unsigned int padding0:2 ;
            unsigned int picl_ok_f       :1 ; // Set when PICL transfer is finished. Cleared when writing 1 in this field.
            unsigned int scu_ok_f        :1 ; // Set when SCU sequence is finished without error. Cleared when writing 1 in this field.
            unsigned int scu_fl_f        :1 ; // Set when SCU sequence is finished with error. Cleared when writing 1 in this field.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_ifr_t;
        
        typedef union {
          struct {
            unsigned int icu_ok_flags    :32; // Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_iok_ifr_t;
        
        typedef union {
          struct {
            unsigned int icu_dly_flags   :32; // Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_idl_ifr_t;
        
        typedef union {
          struct {
            unsigned int icu_den_flags   :32; // Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_idn_ifr_t;
        
        typedef union {
          struct {
            unsigned int icu_upd_flags   :32; // Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_iup_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_power_manager : public vp::regmap
        {
        public:
            vp_power_manager_dlcpd_msr dlcpd_msr;
            vp_power_manager_dlcpd_mpacr dlcpd_mpacr;
            vp_power_manager_dlcpd_mpadr dlcpd_mpadr;
            vp_power_manager_dlcpd_imr dlcpd_imr;
            vp_power_manager_dlcpd_ifr dlcpd_ifr;
            vp_power_manager_dlcpd_iok_ifr dlcpd_iok_ifr;
            vp_power_manager_dlcpd_idl_ifr dlcpd_idl_ifr;
            vp_power_manager_dlcpd_idn_ifr dlcpd_idn_ifr;
            vp_power_manager_dlcpd_iup_ifr dlcpd_iup_ifr;
        };

|

.. _power_manager_DLCPD_MSR:

DLCPD_MSR
"""""""""

Maestro Status Register.

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                Description                                                |
    +=====+===+=========+===========================================================================================================+
    |    0|W  |PICL_BUSY|PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer.|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------+
    |    1|W  |SCU_BUSY |SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends.                    |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro Status Register.
                #define POWER_MANAGER_DLCPD_MSR_OFFSET           0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_msr_get(uint32_t base);
        static inline void power_manager_dlcpd_msr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer. (access: W)
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_BIT                        0
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_WIDTH                      1
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_MASK                       0x1
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_RESET                      0x0
        
        // SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends. (access: W)
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_BIT                         1
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_WIDTH                       1
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_MASK                        0x2
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_GET(value)       (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_GETS(value)      (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_MSR_PICL_BUSY(val)             ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_GET(value)        (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_GETS(value)       (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY_SET(value,field)  (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_MSR_SCU_BUSY(val)              ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int picl_busy       :1 ; // PICL busy status. Set when a transfer is on going on the PICL bus. Cleared at the end of the PICL transfer.
            unsigned int scu_busy        :1 ; // SCU busy status. Set when a SCU sequence is on going. Cleared when a SCU sequence ends.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_msr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_msr : public vp::reg_32
        {
        public:
            inline void picl_busy_set(uint32_t value);
            inline uint32_t picl_busy_get();
            inline void scu_busy_set(uint32_t value);
            inline uint32_t scu_busy_get();
        };

|

.. _power_manager_DLCPD_MPACR:

DLCPD_MPACR
"""""""""""

Maestro PICL Access Control Register.

.. table:: 

    +-----+---+-------+------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                    Description                                     |
    +=====+===+=======+====================================================================================+
    |15:0 |R  |PAADDR |PICL Access Address.                                                                |
    +-----+---+-------+------------------------------------------------------------------------------------+
    |24   |R  |PADIR  |PICL Access Direction : - 1'b0: write  - 1'b1: read.                                |
    +-----+---+-------+------------------------------------------------------------------------------------+
    |28   |R  |PASTART|PICL Access Start. This field is automatically cleared when PICL access is finished.|
    +-----+---+-------+------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro PICL Access Control Register.
                #define POWER_MANAGER_DLCPD_MPACR_OFFSET         0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_mpacr_get(uint32_t base);
        static inline void power_manager_dlcpd_mpacr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // PICL Access Address. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_BIT                         0
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_WIDTH                       16
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_MASK                        0xffff
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_RESET                       0x0
        
        // PICL Access Direction : - 1'b0: write  - 1'b1: read. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_BIT                          24
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_WIDTH                        1
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_MASK                         0x1000000
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_RESET                        0x0
        
        // PICL Access Start. This field is automatically cleared when PICL access is finished. (access: R)
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_BIT                        28
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_WIDTH                      1
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_MASK                       0x10000000
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_GET(value)        (GAP_BEXTRACTU((value),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_GETS(value)       (GAP_BEXTRACT((value),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR_SET(value,field)  (GAP_BINSERT((value),(field),16,0))
        #define POWER_MANAGER_DLCPD_MPACR_PAADDR(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_GET(value)         (GAP_BEXTRACTU((value),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_GETS(value)        (GAP_BEXTRACT((value),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR_SET(value,field)   (GAP_BINSERT((value),(field),1,24))
        #define POWER_MANAGER_DLCPD_MPACR_PADIR(val)               ((val) << 24)
        
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_GET(value)       (GAP_BEXTRACTU((value),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_GETS(value)      (GAP_BEXTRACT((value),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART_SET(value,field) (GAP_BINSERT((value),(field),1,28))
        #define POWER_MANAGER_DLCPD_MPACR_PASTART(val)             ((val) << 28)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int paaddr          :16; // PICL Access Address.
            unsigned int padding0:8 ;
            unsigned int padir           :1 ; // PICL Access Direction : - 1'b0: write  - 1'b1: read.
            unsigned int padding1:3 ;
            unsigned int pastart         :1 ; // PICL Access Start. This field is automatically cleared when PICL access is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_mpacr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_mpacr : public vp::reg_32
        {
        public:
            inline void paaddr_set(uint32_t value);
            inline uint32_t paaddr_get();
            inline void padir_set(uint32_t value);
            inline uint32_t padir_get();
            inline void pastart_set(uint32_t value);
            inline uint32_t pastart_get();
        };

|

.. _power_manager_DLCPD_MPADR:

DLCPD_MPADR
"""""""""""

Maestro PICL Access Data Register.

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                          Description                                          |
    +=====+===+=======+===============================================================================================+
    |15:0 |W  |PRWDATA|PICL Read/Write Data. This field is automatically updated after a PICL read access is finished.|
    +-----+---+-------+-----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro PICL Access Data Register.
                #define POWER_MANAGER_DLCPD_MPADR_OFFSET         0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_mpadr_get(uint32_t base);
        static inline void power_manager_dlcpd_mpadr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // PICL Read/Write Data. This field is automatically updated after a PICL read access is finished. (access: W)
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_BIT                        0
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_WIDTH                      16
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_MASK                       0xffff
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_GET(value)       (GAP_BEXTRACTU((value),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_GETS(value)      (GAP_BEXTRACT((value),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define POWER_MANAGER_DLCPD_MPADR_PRWDATA(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int prwdata         :16; // PICL Read/Write Data. This field is automatically updated after a PICL read access is finished.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_mpadr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_mpadr : public vp::reg_32
        {
        public:
            inline void prwdata_set(uint32_t value);
            inline uint32_t prwdata_get();
        };

|

.. _power_manager_DLCPD_IMR:

DLCPD_IMR
"""""""""

Maestro Interrupt Mask Register.

.. table:: 

    +-----+---+---------+------------------------------+
    |Bit #|R/W|  Name   |         Description          |
    +=====+===+=========+==============================+
    |    0|W  |ICU_OK_M |Mask of ICU_OK interrupt.     |
    +-----+---+---------+------------------------------+
    |    1|W  |ICU_DLY_M|Mask of ICU_DELAYED interrupt.|
    +-----+---+---------+------------------------------+
    |    2|W  |ICU_DEN_M|Mask of ICU_DENIED interrupt. |
    +-----+---+---------+------------------------------+
    |    3|W  |ICU_UPD_M|Mask of ICU_UPDATED interrupt.|
    +-----+---+---------+------------------------------+
    |    6|W  |PICL_OK_M|Mask of PICL_OK interrupt.    |
    +-----+---+---------+------------------------------+
    |    7|W  |SCU_OK_M |Mask of SCU_OK interrupt.     |
    +-----+---+---------+------------------------------+
    |    8|W  |SCU_FL_M |Mask of SCU_FL interrupt.     |
    +-----+---+---------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro Interrupt Mask Register.
                #define POWER_MANAGER_DLCPD_IMR_OFFSET           0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_imr_get(uint32_t base);
        static inline void power_manager_dlcpd_imr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Mask of ICU_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_BIT                         0
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_MASK                        0x1
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_RESET                       0x0
        
        // Mask of ICU_DELAYED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_BIT                        1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_MASK                       0x2
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_RESET                      0x0
        
        // Mask of ICU_DENIED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_BIT                        2
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_MASK                       0x4
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_RESET                      0x0
        
        // Mask of ICU_UPDATED interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_BIT                        3
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_MASK                       0x8
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_RESET                      0x0
        
        // Mask of PICL_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_BIT                        6
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_MASK                       0x40
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_RESET                      0x0
        
        // Mask of SCU_OK interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_BIT                         7
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_MASK                        0x80
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_RESET                       0x0
        
        // Mask of SCU_FL interrupt. (access: W)
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_BIT                         8
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_MASK                        0x100
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_IMR_ICU_OK_M(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_GET(value)       (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_GETS(value)      (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DLY_M(val)             ((val) << 1)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_GET(value)       (GAP_BEXTRACTU((value),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_GETS(value)      (GAP_BEXTRACT((value),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define POWER_MANAGER_DLCPD_IMR_ICU_DEN_M(val)             ((val) << 2)
        
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_GET(value)       (GAP_BEXTRACTU((value),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_GETS(value)      (GAP_BEXTRACT((value),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define POWER_MANAGER_DLCPD_IMR_ICU_UPD_M(val)             ((val) << 3)
        
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_GET(value)       (GAP_BEXTRACTU((value),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_GETS(value)      (GAP_BEXTRACT((value),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define POWER_MANAGER_DLCPD_IMR_PICL_OK_M(val)             ((val) << 6)
        
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define POWER_MANAGER_DLCPD_IMR_SCU_OK_M(val)              ((val) << 7)
        
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_GET(value)        (GAP_BEXTRACTU((value),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_GETS(value)       (GAP_BEXTRACT((value),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M_SET(value,field)  (GAP_BINSERT((value),(field),1,8))
        #define POWER_MANAGER_DLCPD_IMR_SCU_FL_M(val)              ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_ok_m        :1 ; // Mask of ICU_OK interrupt.
            unsigned int icu_dly_m       :1 ; // Mask of ICU_DELAYED interrupt.
            unsigned int icu_den_m       :1 ; // Mask of ICU_DENIED interrupt.
            unsigned int icu_upd_m       :1 ; // Mask of ICU_UPDATED interrupt.
            unsigned int padding0:2 ;
            unsigned int picl_ok_m       :1 ; // Mask of PICL_OK interrupt.
            unsigned int scu_ok_m        :1 ; // Mask of SCU_OK interrupt.
            unsigned int scu_fl_m        :1 ; // Mask of SCU_FL interrupt.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_imr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_imr : public vp::reg_32
        {
        public:
            inline void icu_ok_m_set(uint32_t value);
            inline uint32_t icu_ok_m_get();
            inline void icu_dly_m_set(uint32_t value);
            inline uint32_t icu_dly_m_get();
            inline void icu_den_m_set(uint32_t value);
            inline uint32_t icu_den_m_get();
            inline void icu_upd_m_set(uint32_t value);
            inline uint32_t icu_upd_m_get();
            inline void picl_ok_m_set(uint32_t value);
            inline uint32_t picl_ok_m_get();
            inline void scu_ok_m_set(uint32_t value);
            inline uint32_t scu_ok_m_get();
            inline void scu_fl_m_set(uint32_t value);
            inline uint32_t scu_fl_m_get();
        };

|

.. _power_manager_DLCPD_IFR:

DLCPD_IFR
"""""""""

Maestro Interrupt Flag Register.

.. table:: 

    +-----+---+---------+--------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                     Description                                      |
    +=====+===+=========+======================================================================================+
    |    0|W  |ICU_OK_F |Set when at least one of the bit of the DLCPD_IOK_IFR register is set.                |
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    1|W  |ICU_DLY_F|Set when at least one of the bit of the DLCPD_IDL_IFR register is set.                |
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    2|W  |ICU_DEN_F|Set when at least one of the bit of the DLCPD_IDN_IFR register is set.                |
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    3|W  |ICU_UPD_F|Set when at least one of the bit of the DLCPD_IUP_IFR register is set.                |
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    6|W  |PICL_OK_F|Set when PICL transfer is finished. Cleared when writing 1 in this field.             |
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    7|W  |SCU_OK_F |Set when SCU sequence is finished without error. Cleared when writing 1 in this field.|
    +-----+---+---------+--------------------------------------------------------------------------------------+
    |    8|W  |SCU_FL_F |Set when SCU sequence is finished with error. Cleared when writing 1 in this field.   |
    +-----+---+---------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IFR_OFFSET           0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Set when at least one of the bit of the DLCPD_IOK_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_BIT                         0
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_MASK                        0x1
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_RESET                       0x0
        
        // Set when at least one of the bit of the DLCPD_IDL_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_BIT                        1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_MASK                       0x2
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_RESET                      0x0
        
        // Set when at least one of the bit of the DLCPD_IDN_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_BIT                        2
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_MASK                       0x4
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_RESET                      0x0
        
        // Set when at least one of the bit of the DLCPD_IUP_IFR register is set. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_BIT                        3
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_MASK                       0x8
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_RESET                      0x0
        
        // Set when PICL transfer is finished. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_BIT                        6
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_WIDTH                      1
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_MASK                       0x40
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_RESET                      0x0
        
        // Set when SCU sequence is finished without error. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_BIT                         7
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_MASK                        0x80
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_RESET                       0x0
        
        // Set when SCU sequence is finished with error. Cleared when writing 1 in this field. (access: W)
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_BIT                         8
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_WIDTH                       1
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_MASK                        0x100
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_GET(value)        (GAP_BEXTRACTU((value),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_GETS(value)       (GAP_BEXTRACT((value),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F_SET(value,field)  (GAP_BINSERT((value),(field),1,0))
        #define POWER_MANAGER_DLCPD_IFR_ICU_OK_F(val)              ((val) << 0)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_GET(value)       (GAP_BEXTRACTU((value),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_GETS(value)      (GAP_BEXTRACT((value),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DLY_F(val)             ((val) << 1)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_GET(value)       (GAP_BEXTRACTU((value),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_GETS(value)      (GAP_BEXTRACT((value),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define POWER_MANAGER_DLCPD_IFR_ICU_DEN_F(val)             ((val) << 2)
        
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_GET(value)       (GAP_BEXTRACTU((value),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_GETS(value)      (GAP_BEXTRACT((value),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F_SET(value,field) (GAP_BINSERT((value),(field),1,3))
        #define POWER_MANAGER_DLCPD_IFR_ICU_UPD_F(val)             ((val) << 3)
        
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_GET(value)       (GAP_BEXTRACTU((value),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_GETS(value)      (GAP_BEXTRACT((value),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define POWER_MANAGER_DLCPD_IFR_PICL_OK_F(val)             ((val) << 6)
        
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_GET(value)        (GAP_BEXTRACTU((value),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_GETS(value)       (GAP_BEXTRACT((value),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F_SET(value,field)  (GAP_BINSERT((value),(field),1,7))
        #define POWER_MANAGER_DLCPD_IFR_SCU_OK_F(val)              ((val) << 7)
        
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_GET(value)        (GAP_BEXTRACTU((value),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_GETS(value)       (GAP_BEXTRACT((value),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F_SET(value,field)  (GAP_BINSERT((value),(field),1,8))
        #define POWER_MANAGER_DLCPD_IFR_SCU_FL_F(val)              ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_ok_f        :1 ; // Set when at least one of the bit of the DLCPD_IOK_IFR register is set.
            unsigned int icu_dly_f       :1 ; // Set when at least one of the bit of the DLCPD_IDL_IFR register is set.
            unsigned int icu_den_f       :1 ; // Set when at least one of the bit of the DLCPD_IDN_IFR register is set.
            unsigned int icu_upd_f       :1 ; // Set when at least one of the bit of the DLCPD_IUP_IFR register is set.
            unsigned int padding0:2 ;
            unsigned int picl_ok_f       :1 ; // Set when PICL transfer is finished. Cleared when writing 1 in this field.
            unsigned int scu_ok_f        :1 ; // Set when SCU sequence is finished without error. Cleared when writing 1 in this field.
            unsigned int scu_fl_f        :1 ; // Set when SCU sequence is finished with error. Cleared when writing 1 in this field.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_ifr : public vp::reg_32
        {
        public:
            inline void icu_ok_f_set(uint32_t value);
            inline uint32_t icu_ok_f_get();
            inline void icu_dly_f_set(uint32_t value);
            inline uint32_t icu_dly_f_get();
            inline void icu_den_f_set(uint32_t value);
            inline uint32_t icu_den_f_get();
            inline void icu_upd_f_set(uint32_t value);
            inline uint32_t icu_upd_f_get();
            inline void picl_ok_f_set(uint32_t value);
            inline uint32_t picl_ok_f_get();
            inline void scu_ok_f_set(uint32_t value);
            inline uint32_t scu_ok_f_get();
            inline void scu_fl_f_set(uint32_t value);
            inline uint32_t scu_fl_f_get();
        };

|

.. _power_manager_DLCPD_IOK_IFR:

DLCPD_IOK_IFR
"""""""""""""

Maestro ICU_OK Interrupt Flag Register.

.. table:: 

    +-----+---+------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name    |                                                                                       Description                                                                                       |
    +=====+===+============+=========================================================================================================================================================================================+
    |31:0 |W  |ICU_OK_FLAGS|Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1.|
    +-----+---+------------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro ICU_OK Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IOK_IFR_OFFSET       0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_iok_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_iok_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_BIT                 0
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_WIDTH               32
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_MASK                0xffffffff
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IOK_IFR_ICU_OK_FLAGS(val)      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_ok_flags    :32; // Flags of the ICU_OK interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was performed. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_iok_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_iok_ifr : public vp::reg_32
        {
        public:
            inline void icu_ok_flags_set(uint32_t value);
            inline uint32_t icu_ok_flags_get();
        };

|

.. _power_manager_DLCPD_IDL_IFR:

DLCPD_IDL_IFR
"""""""""""""

Maestro ICU_DELAYED Interrupt Flag Register.

.. table:: 

    +-----+---+-------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                                                                        Description                                                                                         |
    +=====+===+=============+============================================================================================================================================================================================+
    |31:0 |W  |ICU_DLY_FLAGS|Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro ICU_DELAYED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IDL_IFR_OFFSET       0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_idl_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_idl_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IDL_IFR_ICU_DLY_FLAGS(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_dly_flags   :32; // Flags of the ICU_DELAYED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was delayed. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_idl_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_idl_ifr : public vp::reg_32
        {
        public:
            inline void icu_dly_flags_set(uint32_t value);
            inline uint32_t icu_dly_flags_get();
        };

|

.. _power_manager_DLCPD_IDN_IFR:

DLCPD_IDN_IFR
"""""""""""""

Maestro ICU_DENIED Interrupt Flag Register.

.. table:: 

    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                                                                       Description                                                                                        |
    +=====+===+=============+==========================================================================================================================================================================================+
    |31:0 |W  |ICU_DEN_FLAGS|Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro ICU_DENIED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IDN_IFR_OFFSET       0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_idn_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_idn_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IDN_IFR_ICU_DEN_FLAGS(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_den_flags   :32; // Flags of the ICU_DENIED interrupts. Each bit is set if the requested mode change from the control interface on the corresponding ICU was denied. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_idn_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_idn_ifr : public vp::reg_32
        {
        public:
            inline void icu_den_flags_set(uint32_t value);
            inline uint32_t icu_den_flags_get();
        };

|

.. _power_manager_DLCPD_IUP_IFR:

DLCPD_IUP_IFR
"""""""""""""

Maestro ICU_UPDATED Interrupt Flag Register.

.. table:: 

    +-----+---+-------------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                                                    Description                                                                     |
    +=====+===+=============+====================================================================================================================================================+
    |31:0 |W  |ICU_UPD_FLAGS|Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1.|
    +-----+---+-------------+----------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Maestro ICU_UPDATED Interrupt Flag Register.
                #define POWER_MANAGER_DLCPD_IUP_IFR_OFFSET       0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t power_manager_dlcpd_iup_ifr_get(uint32_t base);
        static inline void power_manager_dlcpd_iup_ifr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1. (access: W)
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_BIT                0
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_WIDTH              32
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_MASK               0xffffffff
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define POWER_MANAGER_DLCPD_IUP_IFR_ICU_UPD_FLAGS(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int icu_upd_flags   :32; // Flags of the ICU_UPDATED interrupts. Each bit is set when the corresponding ICU changed its mode or order. Each bit is cleared when writing it to 1.
          };
          unsigned int raw;
        } __attribute__((packed)) power_manager_dlcpd_iup_ifr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_power_manager_dlcpd_iup_ifr : public vp::reg_32
        {
        public:
            inline void icu_upd_flags_set(uint32_t value);
            inline uint32_t icu_upd_flags_get();
        };

|
