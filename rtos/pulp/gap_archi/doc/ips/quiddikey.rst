Input file: fe/ips/quiddikey/doc/quiddikey_reference.md

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |                              Name                               |Offset|Width|               Description               |
    +=================================================================+======+=====+=========================================+
    |:ref:`CR<quiddikey_CR>`                                          |     0|   32|Control register                         |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`ORR<quiddikey_ORR>`                                        |     4|   32|Operation Result register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`SR<quiddikey_SR>`                                          |     8|   32|Status register                          |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`AR<quiddikey_AR>`                                          |    12|   32|Allow register                           |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IER<quiddikey_IER>`                                        |    16|   32|Interrupt Enable register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IMR<quiddikey_IMR>`                                        |    20|   32|Interrupt Mask register                  |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`ISR<quiddikey_ISR>`                                        |    24|   32|Interrupt Status register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DATA_DEST<quiddikey_DATA_DEST>`                            |    32|   32|Destination Data register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DATA_SRC (not mapped ?)<quiddikey_DATA_SRC (not mapped ?)>`|    40|   32|Data Source register                     |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`KEY_INDEX<quiddikey_KEY_INDEX>`                            |    40|   32|Key Index register                       |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DIR<quiddikey_DIR>`                                        |   160|   32|Data Input register                      |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`DOR<quiddikey_DOR>`                                        |   168|   32|Data Output register                     |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`MISC<quiddikey_MISC>`                                      |   192|   32|Miscellaneous register                   |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`IF_SR<quiddikey_IF_SR>`                                    |   208|   32|Interface Status register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`TEST<quiddikey_TEST>`                                      |   216|   32|Test register                            |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`PSR<quiddikey_PSR>`                                        |   220|   32|PUF Score register                       |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_RUC0<quiddikey_HW_RUC0>`                                |   224|   32|Hardware Restrict User Context 0 register|
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_RUC1<quiddikey_HW_RUC1>`                                |   228|   32|Hardware Restrict User Context 1 register|
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_SETTINGS<quiddikey_HW_SETTINGS>`                        |   240|   32|Hardware Settings register               |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_INFO<quiddikey_HW_INFO>`                                |   244|   32|Hardware Information register            |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_ID<quiddikey_HW_ID>`                                    |   248|   32|Hardware Identifier register             |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+
    |:ref:`HW_VER<quiddikey_HW_VER>`                                  |   252|   32|Hardware Version register                |
    +-----------------------------------------------------------------+------+-----+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Control register
                #define QUIDDIKEY_CR_OFFSET                      0x0
        
                // Operation Result register
                #define QUIDDIKEY_ORR_OFFSET                     0x4
        
                // Status register
                #define QUIDDIKEY_SR_OFFSET                      0x8
        
                // Allow register
                #define QUIDDIKEY_AR_OFFSET                      0xc
        
                // Interrupt Enable register
                #define QUIDDIKEY_IER_OFFSET                     0x10
        
                // Interrupt Mask register
                #define QUIDDIKEY_IMR_OFFSET                     0x14
        
                // Interrupt Status register
                #define QUIDDIKEY_ISR_OFFSET                     0x18
        
                // Destination Data register
                #define QUIDDIKEY_DATA_DEST_OFFSET               0x20
        
                // Data Source register
                #define QUIDDIKEY_DATA_SRC (NOT MAPPED ?)_OFFSET 0x28
        
                // Key Index register
                #define QUIDDIKEY_KEY_INDEX_OFFSET               0x28
        
                // Data Input register
                #define QUIDDIKEY_DIR_OFFSET                     0xa0
        
                // Data Output register
                #define QUIDDIKEY_DOR_OFFSET                     0xa8
        
                // Miscellaneous register
                #define QUIDDIKEY_MISC_OFFSET                    0xc0
        
                // Interface Status register
                #define QUIDDIKEY_IF_SR_OFFSET                   0xd0
        
                // Test register
                #define QUIDDIKEY_TEST_OFFSET                    0xd8
        
                // PUF Score register
                #define QUIDDIKEY_PSR_OFFSET                     0xdc
        
                // Hardware Restrict User Context 0 register
                #define QUIDDIKEY_HW_RUC0_OFFSET                 0xe0
        
                // Hardware Restrict User Context 1 register
                #define QUIDDIKEY_HW_RUC1_OFFSET                 0xe4
        
                // Hardware Settings register
                #define QUIDDIKEY_HW_SETTINGS_OFFSET             0xf0
        
                // Hardware Information register
                #define QUIDDIKEY_HW_INFO_OFFSET                 0xf4
        
                // Hardware Identifier register
                #define QUIDDIKEY_HW_ID_OFFSET                   0xf8
        
                // Hardware Version register
                #define QUIDDIKEY_HW_VER_OFFSET                  0xfc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_cr_get(uint32_t base);
        static inline void quiddikey_cr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_orr_get(uint32_t base);
        static inline void quiddikey_orr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_sr_get(uint32_t base);
        static inline void quiddikey_sr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_ar_get(uint32_t base);
        static inline void quiddikey_ar_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_ier_get(uint32_t base);
        static inline void quiddikey_ier_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_imr_get(uint32_t base);
        static inline void quiddikey_imr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_isr_get(uint32_t base);
        static inline void quiddikey_isr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_data_dest_get(uint32_t base);
        static inline void quiddikey_data_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_data_src (not mapped ?)_get(uint32_t base);
        static inline void quiddikey_data_src (not mapped ?)_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_key_index_get(uint32_t base);
        static inline void quiddikey_key_index_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_dir_get(uint32_t base);
        static inline void quiddikey_dir_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_dor_get(uint32_t base);
        static inline void quiddikey_dor_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_misc_get(uint32_t base);
        static inline void quiddikey_misc_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_if_sr_get(uint32_t base);
        static inline void quiddikey_if_sr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_test_get(uint32_t base);
        static inline void quiddikey_test_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_psr_get(uint32_t base);
        static inline void quiddikey_psr_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_ruc0_get(uint32_t base);
        static inline void quiddikey_hw_ruc0_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_ruc1_get(uint32_t base);
        static inline void quiddikey_hw_ruc1_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_settings_get(uint32_t base);
        static inline void quiddikey_hw_settings_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_info_get(uint32_t base);
        static inline void quiddikey_hw_info_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_id_get(uint32_t base);
        static inline void quiddikey_hw_id_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_ver_get(uint32_t base);
        static inline void quiddikey_hw_ver_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Begin Zeroize operation (access: R/W)
        #define QUIDDIKEY_CR_ZEROIZE_BIT                                     0
        #define QUIDDIKEY_CR_ZEROIZE_WIDTH                                   1
        #define QUIDDIKEY_CR_ZEROIZE_MASK                                    0x1
        #define QUIDDIKEY_CR_ZEROIZE_RESET                                   0x0
        
        // Begin Enroll operation (access: R/W)
        #define QUIDDIKEY_CR_ENROLL_BIT                                      1
        #define QUIDDIKEY_CR_ENROLL_WIDTH                                    1
        #define QUIDDIKEY_CR_ENROLL_MASK                                     0x2
        #define QUIDDIKEY_CR_ENROLL_RESET                                    0x0
        
        // Begin Start operation (access: R/W)
        #define QUIDDIKEY_CR_START_BIT                                       2
        #define QUIDDIKEY_CR_START_WIDTH                                     1
        #define QUIDDIKEY_CR_START_MASK                                      0x4
        #define QUIDDIKEY_CR_START_RESET                                     0x0
        
        // Begin Stop operation (access: R/W)
        #define QUIDDIKEY_CR_STOP_BIT                                        5
        #define QUIDDIKEY_CR_STOP_WIDTH                                      1
        #define QUIDDIKEY_CR_STOP_MASK                                       0x20
        #define QUIDDIKEY_CR_STOP_RESET                                      0x0
        
        // Begin Get Key operation (access: R/W)
        #define QUIDDIKEY_CR_GET_KEY_BIT                                     6
        #define QUIDDIKEY_CR_GET_KEY_WIDTH                                   1
        #define QUIDDIKEY_CR_GET_KEY_MASK                                    0x40
        #define QUIDDIKEY_CR_GET_KEY_RESET                                   0x0
        
        // Begin Unwrap operation (access: R/W)
        #define QUIDDIKEY_CR_UNWRAP_BIT                                      7
        #define QUIDDIKEY_CR_UNWRAP_WIDTH                                    1
        #define QUIDDIKEY_CR_UNWRAP_MASK                                     0x80
        #define QUIDDIKEY_CR_UNWRAP_RESET                                    0x0
        
        // Begin Wrap Generation Random operation (access: R/W)
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_BIT                       8
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_WIDTH                     1
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_MASK                      0x100
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_RESET                     0x0
        
        // Begin Wrap operation (access: R/W)
        #define QUIDDIKEY_CR_WRAP_BIT                                        9
        #define QUIDDIKEY_CR_WRAP_WIDTH                                      1
        #define QUIDDIKEY_CR_WRAP_MASK                                       0x200
        #define QUIDDIKEY_CR_WRAP_RESET                                      0x0
        
        // Begin Generate Random operation (access: R/W)
        #define QUIDDIKEY_CR_GENERATE_RANDOM_BIT                             15
        #define QUIDDIKEY_CR_GENERATE_RANDOM_WIDTH                           1
        #define QUIDDIKEY_CR_GENERATE_RANDOM_MASK                            0x8000
        #define QUIDDIKEY_CR_GENERATE_RANDOM_RESET                           0x0
        
        // Begin Reseed operation (access: R/W)
        #define QUIDDIKEY_CR_RESEED_BIT                                      16
        #define QUIDDIKEY_CR_RESEED_WIDTH                                    1
        #define QUIDDIKEY_CR_RESEED_MASK                                     0x10000
        #define QUIDDIKEY_CR_RESEED_RESET                                    0x0
        
        // Begin Test PUF operation (access: R/W)
        #define QUIDDIKEY_CR_TEST_PUF_BIT                                    31
        #define QUIDDIKEY_CR_TEST_PUF_WIDTH                                  1
        #define QUIDDIKEY_CR_TEST_PUF_MASK                                   0x80000000
        #define QUIDDIKEY_CR_TEST_PUF_RESET                                  0x0
        
        // Result code of last operation (access: R/W)
        #define QUIDDIKEY_ORR_RESULT_CODE_BIT                                0
        #define QUIDDIKEY_ORR_RESULT_CODE_WIDTH                              8
        #define QUIDDIKEY_ORR_RESULT_CODE_MASK                               0xff
        #define QUIDDIKEY_ORR_RESULT_CODE_RESET                              0x0
        
        // Reseed Warning register field (access: R/W)
        #define QUIDDIKEY_ORR_RESEED_WARNING_BIT                             14
        #define QUIDDIKEY_ORR_RESEED_WARNING_WIDTH                           1
        #define QUIDDIKEY_ORR_RESEED_WARNING_MASK                            0x4000
        #define QUIDDIKEY_ORR_RESEED_WARNING_RESET                           0x0
        
        // Reseed Required register field (access: R/W)
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_BIT                            15
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_WIDTH                          1
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_MASK                           0x8000
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_RESET                          0x0
        
        // Number of last operation (access: R/W)
        #define QUIDDIKEY_ORR_LAST_OPERATION_BIT                             24
        #define QUIDDIKEY_ORR_LAST_OPERATION_WIDTH                           8
        #define QUIDDIKEY_ORR_LAST_OPERATION_MASK                            0xff000000
        #define QUIDDIKEY_ORR_LAST_OPERATION_RESET                           0x0
        
        // Operation is in progress (access: R/W)
        #define QUIDDIKEY_SR_BUSY_BIT                                        0
        #define QUIDDIKEY_SR_BUSY_WIDTH                                      1
        #define QUIDDIKEY_SR_BUSY_MASK                                       0x1
        #define QUIDDIKEY_SR_BUSY_RESET                                      0x0
        
        // Last operation was successful (access: R/W)
        #define QUIDDIKEY_SR_OK_BIT                                          1
        #define QUIDDIKEY_SR_OK_WIDTH                                        1
        #define QUIDDIKEY_SR_OK_MASK                                         0x2
        #define QUIDDIKEY_SR_OK_RESET                                        0x0
        
        // Last operation failed (access: R/W)
        #define QUIDDIKEY_SR_ERROR_BIT                                       2
        #define QUIDDIKEY_SR_ERROR_WIDTH                                     1
        #define QUIDDIKEY_SR_ERROR_MASK                                      0x4
        #define QUIDDIKEY_SR_ERROR_RESET                                     0x0
        
        // Quiddikey is in Zeroized or in Locked state (access: R/W)
        #define QUIDDIKEY_SR_ZEROIZED_BIT                                    3
        #define QUIDDIKEY_SR_ZEROIZED_WIDTH                                  1
        #define QUIDDIKEY_SR_ZEROIZED_MASK                                   0x8
        #define QUIDDIKEY_SR_ZEROIZED_RESET                                  0x0
        
        // Read: last command rejected, Write 1: Clear this bit (access: R/W)
        #define QUIDDIKEY_SR_REJECTED_BIT                                    4
        #define QUIDDIKEY_SR_REJECTED_WIDTH                                  1
        #define QUIDDIKEY_SR_REJECTED_MASK                                   0x10
        #define QUIDDIKEY_SR_REJECTED_RESET                                  0x0
        
        // Request for Data in transfer via DIR register (access: R/W)
        #define QUIDDIKEY_SR_DI_REQUEST_BIT                                  5
        #define QUIDDIKEY_SR_DI_REQUEST_WIDTH                                1
        #define QUIDDIKEY_SR_DI_REQUEST_MASK                                 0x20
        #define QUIDDIKEY_SR_DI_REQUEST_RESET                                0x0
        
        // Request for Data out transfer via DOR register (access: R/W)
        #define QUIDDIKEY_SR_DO_REQUEST_BIT                                  6
        #define QUIDDIKEY_SR_DO_REQUEST_WIDTH                                1
        #define QUIDDIKEY_SR_DO_REQUEST_MASK                                 0x40
        #define QUIDDIKEY_SR_DO_REQUEST_RESET                                0x0
        
        // Reseed warning (see ORR RESEED_WARNING) (access: R/W)
        #define QUIDDIKEY_SR_RESEED_WARNING_BIT                              29
        #define QUIDDIKEY_SR_RESEED_WARNING_WIDTH                            1
        #define QUIDDIKEY_SR_RESEED_WARNING_MASK                             0x20000000
        #define QUIDDIKEY_SR_RESEED_WARNING_RESET                            0x0
        
        // Reseed required (see ORR RESEED_REQUIRED) (access: R/W)
        #define QUIDDIKEY_SR_RESEED_REQUIRED_BIT                             30
        #define QUIDDIKEY_SR_RESEED_REQUIRED_WIDTH                           1
        #define QUIDDIKEY_SR_RESEED_REQUIRED_MASK                            0x40000000
        #define QUIDDIKEY_SR_RESEED_REQUIRED_RESET                           0x0
        
        // Quiddikey is in state Lab Test Mode (access: R/W)
        #define QUIDDIKEY_SR_LAB_TEST_MODE_BIT                               31
        #define QUIDDIKEY_SR_LAB_TEST_MODE_WIDTH                             1
        #define QUIDDIKEY_SR_LAB_TEST_MODE_MASK                              0x80000000
        #define QUIDDIKEY_SR_LAB_TEST_MODE_RESET                             0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_ENROLL_BIT                                1
        #define QUIDDIKEY_AR_ALLOW_ENROLL_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_ENROLL_MASK                               0x2
        #define QUIDDIKEY_AR_ALLOW_ENROLL_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_START_BIT                                 2
        #define QUIDDIKEY_AR_ALLOW_START_WIDTH                               1
        #define QUIDDIKEY_AR_ALLOW_START_MASK                                0x4
        #define QUIDDIKEY_AR_ALLOW_START_RESET                               0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_STOP_BIT                                  5
        #define QUIDDIKEY_AR_ALLOW_STOP_WIDTH                                1
        #define QUIDDIKEY_AR_ALLOW_STOP_MASK                                 0x20
        #define QUIDDIKEY_AR_ALLOW_STOP_RESET                                0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_BIT                               6
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_WIDTH                             1
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_MASK                              0x40
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_RESET                             0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_BIT                                7
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_MASK                               0x80
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_BIT                          8
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_WIDTH                        1
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_MASK                         0x100
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_RESET                        0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_WRAP_BIT                                  9
        #define QUIDDIKEY_AR_ALLOW_WRAP_WIDTH                                1
        #define QUIDDIKEY_AR_ALLOW_WRAP_MASK                                 0x200
        #define QUIDDIKEY_AR_ALLOW_WRAP_RESET                                0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_BIT                               15
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_WIDTH                             1
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_MASK                              0x8000
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_RESET                             0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_RESEED_BIT                                16
        #define QUIDDIKEY_AR_ALLOW_RESEED_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_RESEED_MASK                               0x10000
        #define QUIDDIKEY_AR_ALLOW_RESEED_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_BIT                              31
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_WIDTH                            1
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_MASK                             0x80000000
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_RESET                            0x0
        
        // Interrupt enable register (access: R/W)
        #define QUIDDIKEY_IER_INT_EN_BIT                                     0
        #define QUIDDIKEY_IER_INT_EN_WIDTH                                   1
        #define QUIDDIKEY_IER_INT_EN_MASK                                    0x1
        #define QUIDDIKEY_IER_INT_EN_RESET                                   0x0
        
        // Enable Busy interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_BUSY_BIT                                0
        #define QUIDDIKEY_IMR_INT_EN_BUSY_WIDTH                              1
        #define QUIDDIKEY_IMR_INT_EN_BUSY_MASK                               0x1
        #define QUIDDIKEY_IMR_INT_EN_BUSY_RESET                              0x0
        
        // Enable Ok interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_OK_BIT                                  1
        #define QUIDDIKEY_IMR_INT_EN_OK_WIDTH                                1
        #define QUIDDIKEY_IMR_INT_EN_OK_MASK                                 0x2
        #define QUIDDIKEY_IMR_INT_EN_OK_RESET                                0x0
        
        // Enable Error interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_ERROR_BIT                               2
        #define QUIDDIKEY_IMR_INT_EN_ERROR_WIDTH                             1
        #define QUIDDIKEY_IMR_INT_EN_ERROR_MASK                              0x4
        #define QUIDDIKEY_IMR_INT_EN_ERROR_RESET                             0x0
        
        // Enable Zeroized interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_BIT                            3
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_WIDTH                          1
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_MASK                           0x8
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_RESET                          0x0
        
        // Enable Rejected interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_BIT                            4
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_WIDTH                          1
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_MASK                           0x10
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_RESET                          0x0
        
        // Enable Data In Request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_BIT                          5
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_WIDTH                        1
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_MASK                         0x20
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_RESET                        0x0
        
        // Enable Data Out Request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_BIT                          6
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_WIDTH                        1
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_MASK                         0x40
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_RESET                        0x0
        
        // Enable Reseed Warning interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_BIT                      29
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_WIDTH                    1
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_MASK                     0x20000000
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_RESET                    0x0
        
        // Enable Reseed Required interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_BIT                     30
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_WIDTH                   1
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_MASK                    0x40000000
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_RESET                   0x0
        
        // Enable Lab Test Mode interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_BIT                       31
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_WIDTH                     1
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_MASK                      0x80000000
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_RESET                     0x0
        
        // Busy interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_BUSY_BIT                                   0
        #define QUIDDIKEY_ISR_INT_BUSY_WIDTH                                 1
        #define QUIDDIKEY_ISR_INT_BUSY_MASK                                  0x1
        #define QUIDDIKEY_ISR_INT_BUSY_RESET                                 0x0
        
        // Ok interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_OK_BIT                                     1
        #define QUIDDIKEY_ISR_INT_OK_WIDTH                                   1
        #define QUIDDIKEY_ISR_INT_OK_MASK                                    0x2
        #define QUIDDIKEY_ISR_INT_OK_RESET                                   0x0
        
        // Error interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_ERROR_BIT                                  2
        #define QUIDDIKEY_ISR_INT_ERROR_WIDTH                                1
        #define QUIDDIKEY_ISR_INT_ERROR_MASK                                 0x4
        #define QUIDDIKEY_ISR_INT_ERROR_RESET                                0x0
        
        // Zeroized interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_ZEROIZED_BIT                               3
        #define QUIDDIKEY_ISR_INT_ZEROIZED_WIDTH                             1
        #define QUIDDIKEY_ISR_INT_ZEROIZED_MASK                              0x8
        #define QUIDDIKEY_ISR_INT_ZEROIZED_RESET                             0x0
        
        // Rejected interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_REJECTED_BIT                               4
        #define QUIDDIKEY_ISR_INT_REJECTED_WIDTH                             1
        #define QUIDDIKEY_ISR_INT_REJECTED_MASK                              0x10
        #define QUIDDIKEY_ISR_INT_REJECTED_RESET                             0x0
        
        // Data In Request interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_BIT                             5
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_WIDTH                           1
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_MASK                            0x20
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_RESET                           0x0
        
        // Data Out Request interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_BIT                             6
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_WIDTH                           1
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_MASK                            0x40
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_RESET                           0x0
        
        // Reseed Warning interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_BIT                         29
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_WIDTH                       1
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_MASK                        0x20000000
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_RESET                       0x0
        
        // Reseed Required interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_BIT                        30
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_WIDTH                      1
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_MASK                       0x40000000
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_RESET                      0x0
        
        // Lab Test Mode interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_BIT                          31
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_WIDTH                        1
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_MASK                         0x80000000
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_RESET                        0x0
        
        // Data out register destination (access: R/W)
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_BIT                             0
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_WIDTH                           1
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_MASK                            0x1
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_RESET                           0x0
        
        // Secure Output interface destination (access: R/W)
        #define QUIDDIKEY_DATA_DEST_DEST_SO_BIT                              1
        #define QUIDDIKEY_DATA_DEST_DEST_SO_WIDTH                            1
        #define QUIDDIKEY_DATA_DEST_DEST_SO_MASK                             0x2
        #define QUIDDIKEY_DATA_DEST_DEST_SO_RESET                            0x0
        
        // Value of the Key Index (access: R)
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_BIT                            0
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_WIDTH                          4
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_MASK                           0xf
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_RESET                          0x0
        
        // Data In field (access: R/W)
        #define QUIDDIKEY_DIR_DI_BIT                                         0
        #define QUIDDIKEY_DIR_DI_WIDTH                                       32
        #define QUIDDIKEY_DIR_DI_MASK                                        0xffffffff
        #define QUIDDIKEY_DIR_DI_RESET                                       0x0
        
        // Data Out field (access: R)
        #define QUIDDIKEY_DOR_DO_BIT                                         0
        #define QUIDDIKEY_DOR_DO_WIDTH                                       32
        #define QUIDDIKEY_DOR_DO_MASK                                        0xffffffff
        #define QUIDDIKEY_DOR_DO_RESET                                       0x0
        
        // An APB error has occured (access: R/W)
        #define QUIDDIKEY_IF_SR_APB_ERROR_BIT                                0
        #define QUIDDIKEY_IF_SR_APB_ERROR_WIDTH                              1
        #define QUIDDIKEY_IF_SR_APB_ERROR_MASK                               0x1
        #define QUIDDIKEY_IF_SR_APB_ERROR_RESET                              0x0
        
        // Isolates Quiddikey and runs BIST (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ENABLE_BIT                               0
        #define QUIDDIKEY_TEST_BIST_ENABLE_WIDTH                             1
        #define QUIDDIKEY_TEST_BIST_ENABLE_MASK                              0x1
        #define QUIDDIKEY_TEST_BIST_ENABLE_RESET                             0x0
        
        // BIST is in progress or finishing up (access: R/W)
        #define QUIDDIKEY_TEST_BIST_RUNNING_BIT                              4
        #define QUIDDIKEY_TEST_BIST_RUNNING_WIDTH                            1
        #define QUIDDIKEY_TEST_BIST_RUNNING_MASK                             0x10
        #define QUIDDIKEY_TEST_BIST_RUNNING_RESET                            0x0
        
        // BIST is in progress (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ACTIVE_BIT                               5
        #define QUIDDIKEY_TEST_BIST_ACTIVE_WIDTH                             1
        #define QUIDDIKEY_TEST_BIST_ACTIVE_MASK                              0x20
        #define QUIDDIKEY_TEST_BIST_ACTIVE_RESET                             0x0
        
        // BIST has passed (access: R/W)
        #define QUIDDIKEY_TEST_BIST_OK_BIT                                   6
        #define QUIDDIKEY_TEST_BIST_OK_WIDTH                                 1
        #define QUIDDIKEY_TEST_BIST_OK_MASK                                  0x40
        #define QUIDDIKEY_TEST_BIST_OK_RESET                                 0x0
        
        // BIST has failed (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ERROR_BIT                                7
        #define QUIDDIKEY_TEST_BIST_ERROR_WIDTH                              1
        #define QUIDDIKEY_TEST_BIST_ERROR_MASK                               0x80
        #define QUIDDIKEY_TEST_BIST_ERROR_RESET                              0x0
        
        // BIST is not allowed (access: R/W)
        #define QUIDDIKEY_TEST_ALLOW_BIST_BIT                                31
        #define QUIDDIKEY_TEST_ALLOW_BIST_WIDTH                              1
        #define QUIDDIKEY_TEST_ALLOW_BIST_MASK                               0x80000000
        #define QUIDDIKEY_TEST_ALLOW_BIST_RESET                              0x0
        
        // PUF Score field (access: R/W)
        #define QUIDDIKEY_PSR_PUF_SCORE_BIT                                  0
        #define QUIDDIKEY_PSR_PUF_SCORE_WIDTH                                4
        #define QUIDDIKEY_PSR_PUF_SCORE_MASK                                 0xf
        #define QUIDDIKEY_PSR_PUF_SCORE_RESET                                0x0
        
        // Restrict User Context 0 field (access: R/W)
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_BIT                0
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_WIDTH              32
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_MASK               0xffffffff
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_RESET              0x0
        
        // Restrict User Context 1 field (access: R/W)
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_BIT                0
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_WIDTH              32
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_MASK               0xffffffff
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_RESET              0x0
        
        // Enroll settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_BIT                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_MASK                    0x2
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_RESET                   0x0
        
        // Start settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_BIT                      2
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_WIDTH                    1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_MASK                     0x4
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_RESET                    0x0
        
        // Stop settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_BIT                       5
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_WIDTH                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_MASK                      0x20
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_RESET                     0x0
        
        // Get Key settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_BIT                    6
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_WIDTH                  1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_MASK                   0x40
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_RESET                  0x0
        
        // Unwrap settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_BIT                     7
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_MASK                    0x80
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_RESET                   0x0
        
        // Wrap Generated Random settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_BIT               8
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_WIDTH             1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_MASK              0x100
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_RESET             0x0
        
        // Wrap settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_BIT                       9
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_WIDTH                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_MASK                      0x200
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_RESET                     0x0
        
        // Generate Random settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_BIT                    15
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_WIDTH                  1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_MASK                   0x8000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_RESET                  0x0
        
        // Reseed settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_BIT                     16
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_MASK                    0x10000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_RESET                   0x0
        
        // Lab Test Mode settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_BIT              24
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_WIDTH            1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_MASK             0x1000000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_RESET            0x0
        
        // Lab Test Mode select field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_BIT               25
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_WIDTH             1
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_MASK              0x2000000
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_RESET             0x0
        
        // Reseed via DIR settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_BIT         27
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_WIDTH       1
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_MASK        0x8000000
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_RESET       0x0
        
        // Reseed via SI settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_BIT          28
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_WIDTH        1
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_MASK         0x10000000
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_RESET        0x0
        
        // Test PUF settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_BIT                   31
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_WIDTH                 1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_MASK                  0x80000000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_RESET                 0x0
        
        // 1: SP 800-90 is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_BIT                       21
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_WIDTH                     1
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_MASK                      0x200000
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_RESET                     0x0
        
        // 1: BIST is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_BIT                            22
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_WIDTH                          1
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_MASK                           0x400000
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_RESET                          0x0
        
        // 1: Safe, 0: Plus (access: R/W)
        #define QUIDDIKEY_HW_INFO_RESERVED_BIT                               23
        #define QUIDDIKEY_HW_INFO_RESERVED_WIDTH                             1
        #define QUIDDIKEY_HW_INFO_RESERVED_MASK                              0x800000
        #define QUIDDIKEY_HW_INFO_RESERVED_RESET                             0x0
        
        // 1: Wrap is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_BIT                            24
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_WIDTH                          1
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_MASK                           0x1000000
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_RESET                          0x0
        
        // Quiddikey configuration (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_BIT                            28
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_WIDTH                          4
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_MASK                           0xf0000000
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_RESET                          0x0
        
        // Hardware Identifier (access: R/W)
        #define QUIDDIKEY_HW_ID_HW_ID_BIT                                    0
        #define QUIDDIKEY_HW_ID_HW_ID_WIDTH                                  32
        #define QUIDDIKEY_HW_ID_HW_ID_MASK                                   0xffffffff
        #define QUIDDIKEY_HW_ID_HW_ID_RESET                                  0x0
        
        // Hardware version, revision part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_REV_BIT                              0
        #define QUIDDIKEY_HW_VER_HW_VER_REV_WIDTH                            8
        #define QUIDDIKEY_HW_VER_HW_VER_REV_MASK                             0xff
        #define QUIDDIKEY_HW_VER_HW_VER_REV_RESET                            0x0
        
        // Hardware version, minor part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_BIT                            8
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_WIDTH                          8
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_MASK                           0xff00
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_RESET                          0x0
        
        // Hardware version, major part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_BIT                            16
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_WIDTH                          8
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_MASK                           0xff0000
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_CR_ZEROIZE_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_CR_ZEROIZE_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_CR_ZEROIZE_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_CR_ZEROIZE(val)                          ((val) << 0)
        
        #define QUIDDIKEY_CR_ENROLL_GET(value)                     (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_CR_ENROLL_GETS(value)                    (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_CR_ENROLL_SET(value,field)               (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_CR_ENROLL(val)                           ((val) << 1)
        
        #define QUIDDIKEY_CR_START_GET(value)                      (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_CR_START_GETS(value)                     (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_CR_START_SET(value,field)                (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_CR_START(val)                            ((val) << 2)
        
        #define QUIDDIKEY_CR_STOP_GET(value)                       (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_CR_STOP_GETS(value)                      (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_CR_STOP_SET(value,field)                 (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_CR_STOP(val)                             ((val) << 5)
        
        #define QUIDDIKEY_CR_GET_KEY_GET(value)                    (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_CR_GET_KEY_GETS(value)                   (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_CR_GET_KEY_SET(value,field)              (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_CR_GET_KEY(val)                          ((val) << 6)
        
        #define QUIDDIKEY_CR_UNWRAP_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_CR_UNWRAP_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_CR_UNWRAP_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_CR_UNWRAP(val)                           ((val) << 7)
        
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_GET(value)      (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_GETS(value)     (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM(val)            ((val) << 8)
        
        #define QUIDDIKEY_CR_WRAP_GET(value)                       (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_CR_WRAP_GETS(value)                      (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_CR_WRAP_SET(value,field)                 (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_CR_WRAP(val)                             ((val) << 9)
        
        #define QUIDDIKEY_CR_GENERATE_RANDOM_GET(value)            (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM_GETS(value)           (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM_SET(value,field)      (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM(val)                  ((val) << 15)
        
        #define QUIDDIKEY_CR_RESEED_GET(value)                     (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_CR_RESEED_GETS(value)                    (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_CR_RESEED_SET(value,field)               (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_CR_RESEED(val)                           ((val) << 16)
        
        #define QUIDDIKEY_CR_TEST_PUF_GET(value)                   (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_CR_TEST_PUF_GETS(value)                  (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_CR_TEST_PUF_SET(value,field)             (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_CR_TEST_PUF(val)                         ((val) << 31)
        
        #define QUIDDIKEY_ORR_RESULT_CODE_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE(val)                     ((val) << 0)
        
        #define QUIDDIKEY_ORR_RESEED_WARNING_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING(val)                  ((val) << 14)
        
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_GET(value)           (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_GETS(value)          (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_SET(value,field)     (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED(val)                 ((val) << 15)
        
        #define QUIDDIKEY_ORR_LAST_OPERATION_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION(val)                  ((val) << 24)
        
        #define QUIDDIKEY_SR_BUSY_GET(value)                       (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_SR_BUSY_GETS(value)                      (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_SR_BUSY_SET(value,field)                 (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_SR_BUSY(val)                             ((val) << 0)
        
        #define QUIDDIKEY_SR_OK_GET(value)                         (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_SR_OK_GETS(value)                        (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_SR_OK_SET(value,field)                   (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_SR_OK(val)                               ((val) << 1)
        
        #define QUIDDIKEY_SR_ERROR_GET(value)                      (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_SR_ERROR_GETS(value)                     (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_SR_ERROR_SET(value,field)                (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_SR_ERROR(val)                            ((val) << 2)
        
        #define QUIDDIKEY_SR_ZEROIZED_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_SR_ZEROIZED_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_SR_ZEROIZED_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_SR_ZEROIZED(val)                         ((val) << 3)
        
        #define QUIDDIKEY_SR_REJECTED_GET(value)                   (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_SR_REJECTED_GETS(value)                  (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_SR_REJECTED_SET(value,field)             (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_SR_REJECTED(val)                         ((val) << 4)
        
        #define QUIDDIKEY_SR_DI_REQUEST_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST(val)                       ((val) << 5)
        
        #define QUIDDIKEY_SR_DO_REQUEST_GET(value)                 (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST_GETS(value)                (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST_SET(value,field)           (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST(val)                       ((val) << 6)
        
        #define QUIDDIKEY_SR_RESEED_WARNING_GET(value)             (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING_GETS(value)            (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING_SET(value,field)       (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING(val)                   ((val) << 29)
        
        #define QUIDDIKEY_SR_RESEED_REQUIRED_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED(val)                  ((val) << 30)
        
        #define QUIDDIKEY_SR_LAB_TEST_MODE_GET(value)              (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE_GETS(value)             (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE_SET(value,field)        (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE(val)                    ((val) << 31)
        
        #define QUIDDIKEY_AR_ALLOW_ENROLL_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL(val)                     ((val) << 1)
        
        #define QUIDDIKEY_AR_ALLOW_START_GET(value)                (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_AR_ALLOW_START_GETS(value)               (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_AR_ALLOW_START_SET(value,field)          (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_AR_ALLOW_START(val)                      ((val) << 2)
        
        #define QUIDDIKEY_AR_ALLOW_STOP_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP(val)                       ((val) << 5)
        
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY(val)                    ((val) << 6)
        
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_GET(value)               (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_GETS(value)              (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_SET(value,field)         (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP(val)                     ((val) << 7)
        
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_GET(value)         (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_GETS(value)        (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_SET(value,field)   (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND(val)               ((val) << 8)
        
        #define QUIDDIKEY_AR_ALLOW_WRAP_GET(value)                 (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GETS(value)                (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP_SET(value,field)           (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP(val)                       ((val) << 9)
        
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_GET(value)              (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_GETS(value)             (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_SET(value,field)        (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND(val)                    ((val) << 15)
        
        #define QUIDDIKEY_AR_ALLOW_RESEED_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED(val)                     ((val) << 16)
        
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_GET(value)             (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_GETS(value)            (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_SET(value,field)       (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF(val)                   ((val) << 31)
        
        #define QUIDDIKEY_IER_INT_EN_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IER_INT_EN_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IER_INT_EN_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IER_INT_EN(val)                          ((val) << 0)
        
        #define QUIDDIKEY_IMR_INT_EN_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY(val)                     ((val) << 0)
        
        #define QUIDDIKEY_IMR_INT_EN_OK_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK(val)                       ((val) << 1)
        
        #define QUIDDIKEY_IMR_INT_EN_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR(val)                    ((val) << 2)
        
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_GET(value)           (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_GETS(value)          (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_SET(value,field)     (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED(val)                 ((val) << 3)
        
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_GET(value)           (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_GETS(value)          (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_SET(value,field)     (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED(val)                 ((val) << 4)
        
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST(val)               ((val) << 5)
        
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST(val)               ((val) << 6)
        
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_GET(value)     (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_GETS(value)    (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_SET(value,field) (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING(val)           ((val) << 29)
        
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_GET(value)    (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_GETS(value)   (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_SET(value,field) (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED(val)          ((val) << 30)
        
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE(val)            ((val) << 31)
        
        #define QUIDDIKEY_ISR_INT_BUSY_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY(val)                        ((val) << 0)
        
        #define QUIDDIKEY_ISR_INT_OK_GET(value)                    (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_ISR_INT_OK_GETS(value)                   (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_ISR_INT_OK_SET(value,field)              (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_ISR_INT_OK(val)                          ((val) << 1)
        
        #define QUIDDIKEY_ISR_INT_ERROR_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR(val)                       ((val) << 2)
        
        #define QUIDDIKEY_ISR_INT_ZEROIZED_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED(val)                    ((val) << 3)
        
        #define QUIDDIKEY_ISR_INT_REJECTED_GET(value)              (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED_GETS(value)             (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED_SET(value,field)        (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED(val)                    ((val) << 4)
        
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_GET(value)            (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_GETS(value)           (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_SET(value,field)      (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST(val)                  ((val) << 5)
        
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_GET(value)            (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_GETS(value)           (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_SET(value,field)      (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST(val)                  ((val) << 6)
        
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_GET(value)        (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_GETS(value)       (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_SET(value,field)  (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING(val)              ((val) << 29)
        
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_GET(value)       (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_GETS(value)      (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_SET(value,field) (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED(val)             ((val) << 30)
        
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_GET(value)         (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_GETS(value)        (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_SET(value,field)   (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE(val)               ((val) << 31)
        
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR(val)                  ((val) << 0)
        
        #define QUIDDIKEY_DATA_DEST_DEST_SO_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO(val)                   ((val) << 1)
        
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX(val)                 ((val) << 0)
        
        #define QUIDDIKEY_DIR_DI_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DIR_DI_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DIR_DI_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DIR_DI(val)                              ((val) << 0)
        
        #define QUIDDIKEY_DOR_DO_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DOR_DO_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DOR_DO_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DOR_DO(val)                              ((val) << 0)
        
        #define QUIDDIKEY_IF_SR_APB_ERROR_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR(val)                     ((val) << 0)
        
        #define QUIDDIKEY_TEST_BIST_ENABLE_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE(val)                    ((val) << 0)
        
        #define QUIDDIKEY_TEST_BIST_RUNNING_GET(value)             (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING_GETS(value)            (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING_SET(value,field)       (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING(val)                   ((val) << 4)
        
        #define QUIDDIKEY_TEST_BIST_ACTIVE_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE(val)                    ((val) << 5)
        
        #define QUIDDIKEY_TEST_BIST_OK_GET(value)                  (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_TEST_BIST_OK_GETS(value)                 (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_TEST_BIST_OK_SET(value,field)            (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_TEST_BIST_OK(val)                        ((val) << 6)
        
        #define QUIDDIKEY_TEST_BIST_ERROR_GET(value)               (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR_GETS(value)              (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR_SET(value,field)         (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR(val)                     ((val) << 7)
        
        #define QUIDDIKEY_TEST_ALLOW_BIST_GET(value)               (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST_GETS(value)              (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST_SET(value,field)         (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST(val)                     ((val) << 31)
        
        #define QUIDDIKEY_PSR_PUF_SCORE_GET(value)                 (GAP_BEXTRACTU((value),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE_GETS(value)                (GAP_BEXTRACT((value),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE_SET(value,field)           (GAP_BINSERT((value),(field),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE(val)                       ((val) << 0)
        
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0(val)     ((val) << 0)
        
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1(val)     ((val) << 0)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_GET(value)    (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_GETS(value)   (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL(val)          ((val) << 1)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_GET(value)     (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_GETS(value)    (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START(val)           ((val) << 2)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP(val)            ((val) << 5)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_GET(value)   (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_GETS(value)  (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY(val)         ((val) << 6)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_GET(value)    (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_GETS(value)   (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP(val)          ((val) << 7)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND(val)    ((val) << 8)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GET(value)      (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GETS(value)     (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP(val)            ((val) << 9)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_GET(value)   (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_GETS(value)  (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND(val)         ((val) << 15)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_GET(value)    (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_GETS(value)   (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED(val)          ((val) << 16)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_GET(value) (GAP_BEXTRACTU((value),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_GETS(value) (GAP_BEXTRACT((value),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE(val)   ((val) << 24)
        
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_GET(value) (GAP_BEXTRACTU((value),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_GETS(value) (GAP_BEXTRACT((value),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE(val)    ((val) << 25)
        
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_GET(value) (GAP_BEXTRACTU((value),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_GETS(value) (GAP_BEXTRACT((value),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_SET(value,field) (GAP_BINSERT((value),(field),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR(val) ((val) << 27)
        
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_GET(value) (GAP_BEXTRACTU((value),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_GETS(value) (GAP_BEXTRACT((value),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_SET(value,field) (GAP_BINSERT((value),(field),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI(val) ((val) << 28)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_GET(value)  (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_GETS(value) (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF(val)        ((val) << 31)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_GET(value)      (GAP_BEXTRACTU((value),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_GETS(value)     (GAP_BEXTRACT((value),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_SET(value,field) (GAP_BINSERT((value),(field),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90(val)            ((val) << 21)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST(val)                 ((val) << 22)
        
        #define QUIDDIKEY_HW_INFO_RESERVED_GET(value)              (GAP_BEXTRACTU((value),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED_GETS(value)             (GAP_BEXTRACT((value),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED_SET(value,field)        (GAP_BINSERT((value),(field),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED(val)                    ((val) << 23)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_GET(value)           (GAP_BEXTRACTU((value),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_GETS(value)          (GAP_BEXTRACT((value),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_SET(value,field)     (GAP_BINSERT((value),(field),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP(val)                 ((val) << 24)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_GET(value)           (GAP_BEXTRACTU((value),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_GETS(value)          (GAP_BEXTRACT((value),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_SET(value,field)     (GAP_BINSERT((value),(field),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE(val)                 ((val) << 28)
        
        #define QUIDDIKEY_HW_ID_HW_ID_GET(value)                   (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID_GETS(value)                  (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID_SET(value,field)             (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID(val)                         ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_HW_VER_REV_GET(value)             (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV_GETS(value)            (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV_SET(value,field)       (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV(val)                   ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR(val)                 ((val) << 8)
        
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** QUIDDIKEY_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cr;  // Control register
            volatile uint32_t orr;  // Operation Result register
            volatile uint32_t sr;  // Status register
            volatile uint32_t ar;  // Allow register
            volatile uint32_t ier;  // Interrupt Enable register
            volatile uint32_t imr;  // Interrupt Mask register
            volatile uint32_t isr;  // Interrupt Status register
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t data_dest;  // Destination Data register
            volatile uint32_t reserved_1[1];  // Reserved/Not used.
            volatile uint32_t data_src (not mapped ?);  // Data Source register
            volatile uint32_t key_index;  // Key Index register
            volatile uint32_t reserved_2[29];  // Reserved/Not used.
            volatile uint32_t dir;  // Data Input register
            volatile uint32_t reserved_3[1];  // Reserved/Not used.
            volatile uint32_t dor;  // Data Output register
            volatile uint32_t reserved_4[5];  // Reserved/Not used.
            volatile uint32_t misc;  // Miscellaneous register
            volatile uint32_t reserved_5[3];  // Reserved/Not used.
            volatile uint32_t if_sr;  // Interface Status register
            volatile uint32_t reserved_6[1];  // Reserved/Not used.
            volatile uint32_t test;  // Test register
            volatile uint32_t psr;  // PUF Score register
            volatile uint32_t hw_ruc0;  // Hardware Restrict User Context 0 register
            volatile uint32_t hw_ruc1;  // Hardware Restrict User Context 1 register
            volatile uint32_t reserved_7[2];  // Reserved/Not used.
            volatile uint32_t hw_settings;  // Hardware Settings register
            volatile uint32_t hw_info;  // Hardware Information register
            volatile uint32_t hw_id;  // Hardware Identifier register
            volatile uint32_t hw_ver;  // Hardware Version register
        } __attribute__((packed)) quiddikey_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int zeroize         :1 ; // Begin Zeroize operation
            unsigned int enroll          :1 ; // Begin Enroll operation
            unsigned int start           :1 ; // Begin Start operation
            unsigned int padding0:2 ;
            unsigned int stop            :1 ; // Begin Stop operation
            unsigned int get_key         :1 ; // Begin Get Key operation
            unsigned int unwrap          :1 ; // Begin Unwrap operation
            unsigned int wrap_generated_random:1 ; // Begin Wrap Generation Random operation
            unsigned int wrap            :1 ; // Begin Wrap operation
            unsigned int padding1:5 ;
            unsigned int generate_random :1 ; // Begin Generate Random operation
            unsigned int reseed          :1 ; // Begin Reseed operation
            unsigned int padding2:14;
            unsigned int test_puf        :1 ; // Begin Test PUF operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_cr_t;
        
        typedef union {
          struct {
            unsigned int result_code     :8 ; // Result code of last operation
            unsigned int padding0:6 ;
            unsigned int reseed_warning  :1 ; // Reseed Warning register field
            unsigned int reseed_required :1 ; // Reseed Required register field
            unsigned int padding1:8 ;
            unsigned int last_operation  :8 ; // Number of last operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_orr_t;
        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Operation is in progress
            unsigned int ok              :1 ; // Last operation was successful
            unsigned int error           :1 ; // Last operation failed
            unsigned int zeroized        :1 ; // Quiddikey is in Zeroized or in Locked state
            unsigned int rejected        :1 ; // Read: last command rejected, Write 1: Clear this bit
            unsigned int di_request      :1 ; // Request for Data in transfer via DIR register
            unsigned int do_request      :1 ; // Request for Data out transfer via DOR register
            unsigned int padding0:22;
            unsigned int reseed_warning  :1 ; // Reseed warning (see ORR RESEED_WARNING)
            unsigned int reseed_required :1 ; // Reseed required (see ORR RESEED_REQUIRED)
            unsigned int lab_test_mode   :1 ; // Quiddikey is in state Lab Test Mode
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_sr_t;
        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int allow_enroll    :1 ; // Operation allowed status
            unsigned int allow_start     :1 ; // Operation allowed status
            unsigned int padding1:2 ;
            unsigned int allow_stop      :1 ; // Operation allowed status
            unsigned int allow_get_key   :1 ; // Operation allowed status
            unsigned int allow_unwrap    :1 ; // Operation allowed status
            unsigned int allow_wrap_gen_rnd:1 ; // Operation allowed status
            unsigned int allow_wrap      :1 ; // Operation allowed status
            unsigned int padding2:5 ;
            unsigned int allow_gen_rnd   :1 ; // Operation allowed status
            unsigned int allow_reseed    :1 ; // Operation allowed status
            unsigned int padding3:14;
            unsigned int allow_test_puf  :1 ; // Operation allowed status
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ar_t;
        
        typedef union {
          struct {
            unsigned int int_en          :1 ; // Interrupt enable register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ier_t;
        
        typedef union {
          struct {
            unsigned int int_en_busy     :1 ; // Enable Busy interrupt
            unsigned int int_en_ok       :1 ; // Enable Ok interrupt
            unsigned int int_en_error    :1 ; // Enable Error interrupt
            unsigned int int_en_zeroized :1 ; // Enable Zeroized interrupt
            unsigned int int_en_rejected :1 ; // Enable Rejected interrupt
            unsigned int int_en_di_request:1 ; // Enable Data In Request interrupt
            unsigned int int_en_do_request:1 ; // Enable Data Out Request interrupt
            unsigned int padding0:22;
            unsigned int int_en_reseed_warning:1 ; // Enable Reseed Warning interrupt
            unsigned int int_en_reseed_required:1 ; // Enable Reseed Required interrupt
            unsigned int int_en_lab_test_mode:1 ; // Enable Lab Test Mode interrupt
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_imr_t;
        
        typedef union {
          struct {
            unsigned int int_busy        :1 ; // Busy interrupt status register
            unsigned int int_ok          :1 ; // Ok interrupt status register
            unsigned int int_error       :1 ; // Error interrupt status register
            unsigned int int_zeroized    :1 ; // Zeroized interrupt status register
            unsigned int int_rejected    :1 ; // Rejected interrupt status register
            unsigned int int_di_request  :1 ; // Data In Request interrupt status register
            unsigned int int_do_request  :1 ; // Data Out Request interrupt status register
            unsigned int padding0:22;
            unsigned int int_reseed_warning:1 ; // Reseed Warning interrupt status register
            unsigned int int_reseed_required:1 ; // Reseed Required interrupt status register
            unsigned int int_lab_test_mode:1 ; // Lab Test Mode interrupt status register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_isr_t;
        
        typedef union {
          struct {
            unsigned int dest_dor        :1 ; // Data out register destination
            unsigned int dest_so         :1 ; // Secure Output interface destination
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_data_dest_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_data_src (not mapped ?)_t;
        
        typedef union {
          struct {
            unsigned int key_index       :4 ; // Value of the Key Index
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_key_index_t;
        
        typedef union {
          struct {
            unsigned int di              :32; // Data In field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dir_t;
        
        typedef union {
          struct {
            unsigned int do              :32; // Data Out field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dor_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_misc_t;
        
        typedef union {
          struct {
            unsigned int apb_error       :1 ; // An APB error has occured
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_if_sr_t;
        
        typedef union {
          struct {
            unsigned int bist_enable     :1 ; // Isolates Quiddikey and runs BIST
            unsigned int padding0:3 ;
            unsigned int bist_running    :1 ; // BIST is in progress or finishing up
            unsigned int bist_active     :1 ; // BIST is in progress
            unsigned int bist_ok         :1 ; // BIST has passed
            unsigned int bist_error      :1 ; // BIST has failed
            unsigned int padding1:23;
            unsigned int allow_bist      :1 ; // BIST is not allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_test_t;
        
        typedef union {
          struct {
            unsigned int puf_score       :4 ; // PUF Score field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_psr_t;
        
        typedef union {
          struct {
            unsigned int restrict_user_context_0:32; // Restrict User Context 0 field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc0_t;
        
        typedef union {
          struct {
            unsigned int restrict_user_context_1:32; // Restrict User Context 1 field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc1_t;
        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int disable_enroll  :1 ; // Enroll settings field
            unsigned int disable_start   :1 ; // Start settings field
            unsigned int padding1:2 ;
            unsigned int disable_stop    :1 ; // Stop settings field
            unsigned int disable_get_key :1 ; // Get Key settings field
            unsigned int disable_unwrap  :1 ; // Unwrap settings field
            unsigned int disable_wrap_gen_rnd:1 ; // Wrap Generated Random settings field
            unsigned int disable_wrap    :1 ; // Wrap settings field
            unsigned int padding2:5 ;
            unsigned int disable_gen_rnd :1 ; // Generate Random settings field
            unsigned int disable_reseed  :1 ; // Reseed settings field
            unsigned int padding3:7 ;
            unsigned int disable_lab_test_mode:1 ; // Lab Test Mode settings field
            unsigned int select_lab_test_mode:1 ; // Lab Test Mode select field
            unsigned int padding4:1 ;
            unsigned int require_reseed_src_via_dir:1 ; // Reseed via DIR settings field
            unsigned int require_reseed_src_via_si:1 ; // Reseed via SI settings field
            unsigned int padding5:2 ;
            unsigned int disable_test_puf:1 ; // Test PUF settings field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_settings_t;
        
        typedef union {
          struct {
            unsigned int padding0:21;
            unsigned int config_sp_800_90:1 ; // 1: SP 800-90 is included, 0: not included
            unsigned int config_bist     :1 ; // 1: BIST is included, 0: not included
            unsigned int reserved        :1 ; // 1: Safe, 0: Plus
            unsigned int config_wrap     :1 ; // 1: Wrap is included, 0: not included
            unsigned int padding1:3 ;
            unsigned int config_type     :4 ; // Quiddikey configuration
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_info_t;
        
        typedef union {
          struct {
            unsigned int hw_id           :32; // Hardware Identifier
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_id_t;
        
        typedef union {
          struct {
            unsigned int hw_ver_rev      :8 ; // Hardware version, revision part
            unsigned int hw_ver_minor    :8 ; // Hardware version, minor part
            unsigned int hw_ver_major    :8 ; // Hardware version, major part
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ver_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_quiddikey : public vp::regmap
        {
        public:
            vp_quiddikey_cr cr;
            vp_quiddikey_orr orr;
            vp_quiddikey_sr sr;
            vp_quiddikey_ar ar;
            vp_quiddikey_ier ier;
            vp_quiddikey_imr imr;
            vp_quiddikey_isr isr;
            vp_quiddikey_data_dest data_dest;
            vp_quiddikey_data_src (not mapped ?) data_src (not mapped ?);
            vp_quiddikey_key_index key_index;
            vp_quiddikey_dir dir;
            vp_quiddikey_dor dor;
            vp_quiddikey_misc misc;
            vp_quiddikey_if_sr if_sr;
            vp_quiddikey_test test;
            vp_quiddikey_psr psr;
            vp_quiddikey_hw_ruc0 hw_ruc0;
            vp_quiddikey_hw_ruc1 hw_ruc1;
            vp_quiddikey_hw_settings hw_settings;
            vp_quiddikey_hw_info hw_info;
            vp_quiddikey_hw_id hw_id;
            vp_quiddikey_hw_ver hw_ver;
        };

|

.. _quiddikey_CR:

CR
""

Control register

.. table:: 

    +-----+---+---------------------+--------------------------------------+
    |Bit #|R/W|        Name         |             Description              |
    +=====+===+=====================+======================================+
    |    0|R/W|ZEROIZE              |Begin Zeroize operation               |
    +-----+---+---------------------+--------------------------------------+
    |    1|R/W|ENROLL               |Begin Enroll operation                |
    +-----+---+---------------------+--------------------------------------+
    |    2|R/W|START                |Begin Start operation                 |
    +-----+---+---------------------+--------------------------------------+
    |    5|R/W|STOP                 |Begin Stop operation                  |
    +-----+---+---------------------+--------------------------------------+
    |    6|R/W|GET_KEY              |Begin Get Key operation               |
    +-----+---+---------------------+--------------------------------------+
    |    7|R/W|UNWRAP               |Begin Unwrap operation                |
    +-----+---+---------------------+--------------------------------------+
    |    8|R/W|WRAP_GENERATED_RANDOM|Begin Wrap Generation Random operation|
    +-----+---+---------------------+--------------------------------------+
    |    9|R/W|WRAP                 |Begin Wrap operation                  |
    +-----+---+---------------------+--------------------------------------+
    |   15|R/W|GENERATE_RANDOM      |Begin Generate Random operation       |
    +-----+---+---------------------+--------------------------------------+
    |   16|R/W|RESEED               |Begin Reseed operation                |
    +-----+---+---------------------+--------------------------------------+
    |   31|R/W|TEST_PUF             |Begin Test PUF operation              |
    +-----+---+---------------------+--------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Control register
                #define QUIDDIKEY_CR_OFFSET                      0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_cr_get(uint32_t base);
        static inline void quiddikey_cr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Begin Zeroize operation (access: R/W)
        #define QUIDDIKEY_CR_ZEROIZE_BIT                                     0
        #define QUIDDIKEY_CR_ZEROIZE_WIDTH                                   1
        #define QUIDDIKEY_CR_ZEROIZE_MASK                                    0x1
        #define QUIDDIKEY_CR_ZEROIZE_RESET                                   0x0
        
        // Begin Enroll operation (access: R/W)
        #define QUIDDIKEY_CR_ENROLL_BIT                                      1
        #define QUIDDIKEY_CR_ENROLL_WIDTH                                    1
        #define QUIDDIKEY_CR_ENROLL_MASK                                     0x2
        #define QUIDDIKEY_CR_ENROLL_RESET                                    0x0
        
        // Begin Start operation (access: R/W)
        #define QUIDDIKEY_CR_START_BIT                                       2
        #define QUIDDIKEY_CR_START_WIDTH                                     1
        #define QUIDDIKEY_CR_START_MASK                                      0x4
        #define QUIDDIKEY_CR_START_RESET                                     0x0
        
        // Begin Stop operation (access: R/W)
        #define QUIDDIKEY_CR_STOP_BIT                                        5
        #define QUIDDIKEY_CR_STOP_WIDTH                                      1
        #define QUIDDIKEY_CR_STOP_MASK                                       0x20
        #define QUIDDIKEY_CR_STOP_RESET                                      0x0
        
        // Begin Get Key operation (access: R/W)
        #define QUIDDIKEY_CR_GET_KEY_BIT                                     6
        #define QUIDDIKEY_CR_GET_KEY_WIDTH                                   1
        #define QUIDDIKEY_CR_GET_KEY_MASK                                    0x40
        #define QUIDDIKEY_CR_GET_KEY_RESET                                   0x0
        
        // Begin Unwrap operation (access: R/W)
        #define QUIDDIKEY_CR_UNWRAP_BIT                                      7
        #define QUIDDIKEY_CR_UNWRAP_WIDTH                                    1
        #define QUIDDIKEY_CR_UNWRAP_MASK                                     0x80
        #define QUIDDIKEY_CR_UNWRAP_RESET                                    0x0
        
        // Begin Wrap Generation Random operation (access: R/W)
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_BIT                       8
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_WIDTH                     1
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_MASK                      0x100
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_RESET                     0x0
        
        // Begin Wrap operation (access: R/W)
        #define QUIDDIKEY_CR_WRAP_BIT                                        9
        #define QUIDDIKEY_CR_WRAP_WIDTH                                      1
        #define QUIDDIKEY_CR_WRAP_MASK                                       0x200
        #define QUIDDIKEY_CR_WRAP_RESET                                      0x0
        
        // Begin Generate Random operation (access: R/W)
        #define QUIDDIKEY_CR_GENERATE_RANDOM_BIT                             15
        #define QUIDDIKEY_CR_GENERATE_RANDOM_WIDTH                           1
        #define QUIDDIKEY_CR_GENERATE_RANDOM_MASK                            0x8000
        #define QUIDDIKEY_CR_GENERATE_RANDOM_RESET                           0x0
        
        // Begin Reseed operation (access: R/W)
        #define QUIDDIKEY_CR_RESEED_BIT                                      16
        #define QUIDDIKEY_CR_RESEED_WIDTH                                    1
        #define QUIDDIKEY_CR_RESEED_MASK                                     0x10000
        #define QUIDDIKEY_CR_RESEED_RESET                                    0x0
        
        // Begin Test PUF operation (access: R/W)
        #define QUIDDIKEY_CR_TEST_PUF_BIT                                    31
        #define QUIDDIKEY_CR_TEST_PUF_WIDTH                                  1
        #define QUIDDIKEY_CR_TEST_PUF_MASK                                   0x80000000
        #define QUIDDIKEY_CR_TEST_PUF_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_CR_ZEROIZE_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_CR_ZEROIZE_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_CR_ZEROIZE_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_CR_ZEROIZE(val)                          ((val) << 0)
        
        #define QUIDDIKEY_CR_ENROLL_GET(value)                     (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_CR_ENROLL_GETS(value)                    (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_CR_ENROLL_SET(value,field)               (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_CR_ENROLL(val)                           ((val) << 1)
        
        #define QUIDDIKEY_CR_START_GET(value)                      (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_CR_START_GETS(value)                     (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_CR_START_SET(value,field)                (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_CR_START(val)                            ((val) << 2)
        
        #define QUIDDIKEY_CR_STOP_GET(value)                       (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_CR_STOP_GETS(value)                      (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_CR_STOP_SET(value,field)                 (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_CR_STOP(val)                             ((val) << 5)
        
        #define QUIDDIKEY_CR_GET_KEY_GET(value)                    (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_CR_GET_KEY_GETS(value)                   (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_CR_GET_KEY_SET(value,field)              (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_CR_GET_KEY(val)                          ((val) << 6)
        
        #define QUIDDIKEY_CR_UNWRAP_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_CR_UNWRAP_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_CR_UNWRAP_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_CR_UNWRAP(val)                           ((val) << 7)
        
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_GET(value)      (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_GETS(value)     (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_CR_WRAP_GENERATED_RANDOM(val)            ((val) << 8)
        
        #define QUIDDIKEY_CR_WRAP_GET(value)                       (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_CR_WRAP_GETS(value)                      (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_CR_WRAP_SET(value,field)                 (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_CR_WRAP(val)                             ((val) << 9)
        
        #define QUIDDIKEY_CR_GENERATE_RANDOM_GET(value)            (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM_GETS(value)           (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM_SET(value,field)      (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_CR_GENERATE_RANDOM(val)                  ((val) << 15)
        
        #define QUIDDIKEY_CR_RESEED_GET(value)                     (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_CR_RESEED_GETS(value)                    (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_CR_RESEED_SET(value,field)               (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_CR_RESEED(val)                           ((val) << 16)
        
        #define QUIDDIKEY_CR_TEST_PUF_GET(value)                   (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_CR_TEST_PUF_GETS(value)                  (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_CR_TEST_PUF_SET(value,field)             (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_CR_TEST_PUF(val)                         ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int zeroize         :1 ; // Begin Zeroize operation
            unsigned int enroll          :1 ; // Begin Enroll operation
            unsigned int start           :1 ; // Begin Start operation
            unsigned int padding0:2 ;
            unsigned int stop            :1 ; // Begin Stop operation
            unsigned int get_key         :1 ; // Begin Get Key operation
            unsigned int unwrap          :1 ; // Begin Unwrap operation
            unsigned int wrap_generated_random:1 ; // Begin Wrap Generation Random operation
            unsigned int wrap            :1 ; // Begin Wrap operation
            unsigned int padding1:5 ;
            unsigned int generate_random :1 ; // Begin Generate Random operation
            unsigned int reseed          :1 ; // Begin Reseed operation
            unsigned int padding2:14;
            unsigned int test_puf        :1 ; // Begin Test PUF operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_cr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_cr : public vp::reg_32
        {
        public:
            inline void zeroize_set(uint32_t value);
            inline uint32_t zeroize_get();
            inline void enroll_set(uint32_t value);
            inline uint32_t enroll_get();
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void get_key_set(uint32_t value);
            inline uint32_t get_key_get();
            inline void unwrap_set(uint32_t value);
            inline uint32_t unwrap_get();
            inline void wrap_generated_random_set(uint32_t value);
            inline uint32_t wrap_generated_random_get();
            inline void wrap_set(uint32_t value);
            inline uint32_t wrap_get();
            inline void generate_random_set(uint32_t value);
            inline uint32_t generate_random_get();
            inline void reseed_set(uint32_t value);
            inline uint32_t reseed_get();
            inline void test_puf_set(uint32_t value);
            inline uint32_t test_puf_get();
        };

|

.. _quiddikey_ORR:

ORR
"""

Operation Result register

.. table:: 

    +-----+---+---------------+------------------------------+
    |Bit #|R/W|     Name      |         Description          |
    +=====+===+===============+==============================+
    |7:0  |R/W|RESULT_CODE    |Result code of last operation |
    +-----+---+---------------+------------------------------+
    |14   |R/W|RESEED_WARNING |Reseed Warning register field |
    +-----+---+---------------+------------------------------+
    |15   |R/W|RESEED_REQUIRED|Reseed Required register field|
    +-----+---+---------------+------------------------------+
    |31:24|R/W|LAST_OPERATION |Number of last operation      |
    +-----+---+---------------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Operation Result register
                #define QUIDDIKEY_ORR_OFFSET                     0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_orr_get(uint32_t base);
        static inline void quiddikey_orr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Result code of last operation (access: R/W)
        #define QUIDDIKEY_ORR_RESULT_CODE_BIT                                0
        #define QUIDDIKEY_ORR_RESULT_CODE_WIDTH                              8
        #define QUIDDIKEY_ORR_RESULT_CODE_MASK                               0xff
        #define QUIDDIKEY_ORR_RESULT_CODE_RESET                              0x0
        
        // Reseed Warning register field (access: R/W)
        #define QUIDDIKEY_ORR_RESEED_WARNING_BIT                             14
        #define QUIDDIKEY_ORR_RESEED_WARNING_WIDTH                           1
        #define QUIDDIKEY_ORR_RESEED_WARNING_MASK                            0x4000
        #define QUIDDIKEY_ORR_RESEED_WARNING_RESET                           0x0
        
        // Reseed Required register field (access: R/W)
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_BIT                            15
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_WIDTH                          1
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_MASK                           0x8000
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_RESET                          0x0
        
        // Number of last operation (access: R/W)
        #define QUIDDIKEY_ORR_LAST_OPERATION_BIT                             24
        #define QUIDDIKEY_ORR_LAST_OPERATION_WIDTH                           8
        #define QUIDDIKEY_ORR_LAST_OPERATION_MASK                            0xff000000
        #define QUIDDIKEY_ORR_LAST_OPERATION_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_ORR_RESULT_CODE_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_ORR_RESULT_CODE(val)                     ((val) << 0)
        
        #define QUIDDIKEY_ORR_RESEED_WARNING_GET(value)            (GAP_BEXTRACTU((value),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING_GETS(value)           (GAP_BEXTRACT((value),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING_SET(value,field)      (GAP_BINSERT((value),(field),1,14))
        #define QUIDDIKEY_ORR_RESEED_WARNING(val)                  ((val) << 14)
        
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_GET(value)           (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_GETS(value)          (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED_SET(value,field)     (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_ORR_RESEED_REQUIRED(val)                 ((val) << 15)
        
        #define QUIDDIKEY_ORR_LAST_OPERATION_GET(value)            (GAP_BEXTRACTU((value),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION_GETS(value)           (GAP_BEXTRACT((value),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION_SET(value,field)      (GAP_BINSERT((value),(field),8,24))
        #define QUIDDIKEY_ORR_LAST_OPERATION(val)                  ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int result_code     :8 ; // Result code of last operation
            unsigned int padding0:6 ;
            unsigned int reseed_warning  :1 ; // Reseed Warning register field
            unsigned int reseed_required :1 ; // Reseed Required register field
            unsigned int padding1:8 ;
            unsigned int last_operation  :8 ; // Number of last operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_orr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_orr : public vp::reg_32
        {
        public:
            inline void result_code_set(uint32_t value);
            inline uint32_t result_code_get();
            inline void reseed_warning_set(uint32_t value);
            inline uint32_t reseed_warning_get();
            inline void reseed_required_set(uint32_t value);
            inline uint32_t reseed_required_get();
            inline void last_operation_set(uint32_t value);
            inline uint32_t last_operation_get();
        };

|

.. _quiddikey_SR:

SR
""

Status register

.. table:: 

    +-----+---+---------------+----------------------------------------------------+
    |Bit #|R/W|     Name      |                    Description                     |
    +=====+===+===============+====================================================+
    |    0|R/W|BUSY           |Operation is in progress                            |
    +-----+---+---------------+----------------------------------------------------+
    |    1|R/W|OK             |Last operation was successful                       |
    +-----+---+---------------+----------------------------------------------------+
    |    2|R/W|ERROR          |Last operation failed                               |
    +-----+---+---------------+----------------------------------------------------+
    |    3|R/W|ZEROIZED       |Quiddikey is in Zeroized or in Locked state         |
    +-----+---+---------------+----------------------------------------------------+
    |    4|R/W|REJECTED       |Read: last command rejected, Write 1: Clear this bit|
    +-----+---+---------------+----------------------------------------------------+
    |    5|R/W|DI_REQUEST     |Request for Data in transfer via DIR register       |
    +-----+---+---------------+----------------------------------------------------+
    |    6|R/W|DO_REQUEST     |Request for Data out transfer via DOR register      |
    +-----+---+---------------+----------------------------------------------------+
    |   29|R/W|RESEED_WARNING |Reseed warning (see ORR RESEED_WARNING)             |
    +-----+---+---------------+----------------------------------------------------+
    |   30|R/W|RESEED_REQUIRED|Reseed required (see ORR RESEED_REQUIRED)           |
    +-----+---+---------------+----------------------------------------------------+
    |   31|R/W|LAB_TEST_MODE  |Quiddikey is in state Lab Test Mode                 |
    +-----+---+---------------+----------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Status register
                #define QUIDDIKEY_SR_OFFSET                      0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_sr_get(uint32_t base);
        static inline void quiddikey_sr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Operation is in progress (access: R/W)
        #define QUIDDIKEY_SR_BUSY_BIT                                        0
        #define QUIDDIKEY_SR_BUSY_WIDTH                                      1
        #define QUIDDIKEY_SR_BUSY_MASK                                       0x1
        #define QUIDDIKEY_SR_BUSY_RESET                                      0x0
        
        // Last operation was successful (access: R/W)
        #define QUIDDIKEY_SR_OK_BIT                                          1
        #define QUIDDIKEY_SR_OK_WIDTH                                        1
        #define QUIDDIKEY_SR_OK_MASK                                         0x2
        #define QUIDDIKEY_SR_OK_RESET                                        0x0
        
        // Last operation failed (access: R/W)
        #define QUIDDIKEY_SR_ERROR_BIT                                       2
        #define QUIDDIKEY_SR_ERROR_WIDTH                                     1
        #define QUIDDIKEY_SR_ERROR_MASK                                      0x4
        #define QUIDDIKEY_SR_ERROR_RESET                                     0x0
        
        // Quiddikey is in Zeroized or in Locked state (access: R/W)
        #define QUIDDIKEY_SR_ZEROIZED_BIT                                    3
        #define QUIDDIKEY_SR_ZEROIZED_WIDTH                                  1
        #define QUIDDIKEY_SR_ZEROIZED_MASK                                   0x8
        #define QUIDDIKEY_SR_ZEROIZED_RESET                                  0x0
        
        // Read: last command rejected, Write 1: Clear this bit (access: R/W)
        #define QUIDDIKEY_SR_REJECTED_BIT                                    4
        #define QUIDDIKEY_SR_REJECTED_WIDTH                                  1
        #define QUIDDIKEY_SR_REJECTED_MASK                                   0x10
        #define QUIDDIKEY_SR_REJECTED_RESET                                  0x0
        
        // Request for Data in transfer via DIR register (access: R/W)
        #define QUIDDIKEY_SR_DI_REQUEST_BIT                                  5
        #define QUIDDIKEY_SR_DI_REQUEST_WIDTH                                1
        #define QUIDDIKEY_SR_DI_REQUEST_MASK                                 0x20
        #define QUIDDIKEY_SR_DI_REQUEST_RESET                                0x0
        
        // Request for Data out transfer via DOR register (access: R/W)
        #define QUIDDIKEY_SR_DO_REQUEST_BIT                                  6
        #define QUIDDIKEY_SR_DO_REQUEST_WIDTH                                1
        #define QUIDDIKEY_SR_DO_REQUEST_MASK                                 0x40
        #define QUIDDIKEY_SR_DO_REQUEST_RESET                                0x0
        
        // Reseed warning (see ORR RESEED_WARNING) (access: R/W)
        #define QUIDDIKEY_SR_RESEED_WARNING_BIT                              29
        #define QUIDDIKEY_SR_RESEED_WARNING_WIDTH                            1
        #define QUIDDIKEY_SR_RESEED_WARNING_MASK                             0x20000000
        #define QUIDDIKEY_SR_RESEED_WARNING_RESET                            0x0
        
        // Reseed required (see ORR RESEED_REQUIRED) (access: R/W)
        #define QUIDDIKEY_SR_RESEED_REQUIRED_BIT                             30
        #define QUIDDIKEY_SR_RESEED_REQUIRED_WIDTH                           1
        #define QUIDDIKEY_SR_RESEED_REQUIRED_MASK                            0x40000000
        #define QUIDDIKEY_SR_RESEED_REQUIRED_RESET                           0x0
        
        // Quiddikey is in state Lab Test Mode (access: R/W)
        #define QUIDDIKEY_SR_LAB_TEST_MODE_BIT                               31
        #define QUIDDIKEY_SR_LAB_TEST_MODE_WIDTH                             1
        #define QUIDDIKEY_SR_LAB_TEST_MODE_MASK                              0x80000000
        #define QUIDDIKEY_SR_LAB_TEST_MODE_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_SR_BUSY_GET(value)                       (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_SR_BUSY_GETS(value)                      (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_SR_BUSY_SET(value,field)                 (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_SR_BUSY(val)                             ((val) << 0)
        
        #define QUIDDIKEY_SR_OK_GET(value)                         (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_SR_OK_GETS(value)                        (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_SR_OK_SET(value,field)                   (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_SR_OK(val)                               ((val) << 1)
        
        #define QUIDDIKEY_SR_ERROR_GET(value)                      (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_SR_ERROR_GETS(value)                     (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_SR_ERROR_SET(value,field)                (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_SR_ERROR(val)                            ((val) << 2)
        
        #define QUIDDIKEY_SR_ZEROIZED_GET(value)                   (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_SR_ZEROIZED_GETS(value)                  (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_SR_ZEROIZED_SET(value,field)             (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_SR_ZEROIZED(val)                         ((val) << 3)
        
        #define QUIDDIKEY_SR_REJECTED_GET(value)                   (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_SR_REJECTED_GETS(value)                  (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_SR_REJECTED_SET(value,field)             (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_SR_REJECTED(val)                         ((val) << 4)
        
        #define QUIDDIKEY_SR_DI_REQUEST_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_SR_DI_REQUEST(val)                       ((val) << 5)
        
        #define QUIDDIKEY_SR_DO_REQUEST_GET(value)                 (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST_GETS(value)                (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST_SET(value,field)           (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_SR_DO_REQUEST(val)                       ((val) << 6)
        
        #define QUIDDIKEY_SR_RESEED_WARNING_GET(value)             (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING_GETS(value)            (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING_SET(value,field)       (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_SR_RESEED_WARNING(val)                   ((val) << 29)
        
        #define QUIDDIKEY_SR_RESEED_REQUIRED_GET(value)            (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED_GETS(value)           (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED_SET(value,field)      (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_SR_RESEED_REQUIRED(val)                  ((val) << 30)
        
        #define QUIDDIKEY_SR_LAB_TEST_MODE_GET(value)              (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE_GETS(value)             (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE_SET(value,field)        (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_SR_LAB_TEST_MODE(val)                    ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int busy            :1 ; // Operation is in progress
            unsigned int ok              :1 ; // Last operation was successful
            unsigned int error           :1 ; // Last operation failed
            unsigned int zeroized        :1 ; // Quiddikey is in Zeroized or in Locked state
            unsigned int rejected        :1 ; // Read: last command rejected, Write 1: Clear this bit
            unsigned int di_request      :1 ; // Request for Data in transfer via DIR register
            unsigned int do_request      :1 ; // Request for Data out transfer via DOR register
            unsigned int padding0:22;
            unsigned int reseed_warning  :1 ; // Reseed warning (see ORR RESEED_WARNING)
            unsigned int reseed_required :1 ; // Reseed required (see ORR RESEED_REQUIRED)
            unsigned int lab_test_mode   :1 ; // Quiddikey is in state Lab Test Mode
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_sr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_sr : public vp::reg_32
        {
        public:
            inline void busy_set(uint32_t value);
            inline uint32_t busy_get();
            inline void ok_set(uint32_t value);
            inline uint32_t ok_get();
            inline void error_set(uint32_t value);
            inline uint32_t error_get();
            inline void zeroized_set(uint32_t value);
            inline uint32_t zeroized_get();
            inline void rejected_set(uint32_t value);
            inline uint32_t rejected_get();
            inline void di_request_set(uint32_t value);
            inline uint32_t di_request_get();
            inline void do_request_set(uint32_t value);
            inline uint32_t do_request_get();
            inline void reseed_warning_set(uint32_t value);
            inline uint32_t reseed_warning_get();
            inline void reseed_required_set(uint32_t value);
            inline uint32_t reseed_required_get();
            inline void lab_test_mode_set(uint32_t value);
            inline uint32_t lab_test_mode_get();
        };

|

.. _quiddikey_AR:

AR
""

Allow register

.. table:: 

    +-----+---+------------------+------------------------+
    |Bit #|R/W|       Name       |      Description       |
    +=====+===+==================+========================+
    |    1|R/W|ALLOW_ENROLL      |Operation allowed status|
    +-----+---+------------------+------------------------+
    |    2|R/W|ALLOW_START       |Operation allowed status|
    +-----+---+------------------+------------------------+
    |    5|R/W|ALLOW_STOP        |Operation allowed status|
    +-----+---+------------------+------------------------+
    |    6|R/W|ALLOW_GET_KEY     |Operation allowed status|
    +-----+---+------------------+------------------------+
    |    7|R/W|ALLOW_UNWRAP      |Operation allowed status|
    +-----+---+------------------+------------------------+
    |    8|R/W|ALLOW_WRAP_GEN_RND|Operation allowed status|
    +-----+---+------------------+------------------------+
    |    9|R/W|ALLOW_WRAP        |Operation allowed status|
    +-----+---+------------------+------------------------+
    |   15|R/W|ALLOW_GEN_RND     |Operation allowed status|
    +-----+---+------------------+------------------------+
    |   16|R/W|ALLOW_RESEED      |Operation allowed status|
    +-----+---+------------------+------------------------+
    |   31|R/W|ALLOW_TEST_PUF    |Operation allowed status|
    +-----+---+------------------+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Allow register
                #define QUIDDIKEY_AR_OFFSET                      0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_ar_get(uint32_t base);
        static inline void quiddikey_ar_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_ENROLL_BIT                                1
        #define QUIDDIKEY_AR_ALLOW_ENROLL_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_ENROLL_MASK                               0x2
        #define QUIDDIKEY_AR_ALLOW_ENROLL_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_START_BIT                                 2
        #define QUIDDIKEY_AR_ALLOW_START_WIDTH                               1
        #define QUIDDIKEY_AR_ALLOW_START_MASK                                0x4
        #define QUIDDIKEY_AR_ALLOW_START_RESET                               0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_STOP_BIT                                  5
        #define QUIDDIKEY_AR_ALLOW_STOP_WIDTH                                1
        #define QUIDDIKEY_AR_ALLOW_STOP_MASK                                 0x20
        #define QUIDDIKEY_AR_ALLOW_STOP_RESET                                0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_BIT                               6
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_WIDTH                             1
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_MASK                              0x40
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_RESET                             0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_BIT                                7
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_MASK                               0x80
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_BIT                          8
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_WIDTH                        1
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_MASK                         0x100
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_RESET                        0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_WRAP_BIT                                  9
        #define QUIDDIKEY_AR_ALLOW_WRAP_WIDTH                                1
        #define QUIDDIKEY_AR_ALLOW_WRAP_MASK                                 0x200
        #define QUIDDIKEY_AR_ALLOW_WRAP_RESET                                0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_BIT                               15
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_WIDTH                             1
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_MASK                              0x8000
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_RESET                             0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_RESEED_BIT                                16
        #define QUIDDIKEY_AR_ALLOW_RESEED_WIDTH                              1
        #define QUIDDIKEY_AR_ALLOW_RESEED_MASK                               0x10000
        #define QUIDDIKEY_AR_ALLOW_RESEED_RESET                              0x0
        
        // Operation allowed status (access: R/W)
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_BIT                              31
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_WIDTH                            1
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_MASK                             0x80000000
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_AR_ALLOW_ENROLL_GET(value)               (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL_GETS(value)              (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL_SET(value,field)         (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_AR_ALLOW_ENROLL(val)                     ((val) << 1)
        
        #define QUIDDIKEY_AR_ALLOW_START_GET(value)                (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_AR_ALLOW_START_GETS(value)               (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_AR_ALLOW_START_SET(value,field)          (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_AR_ALLOW_START(val)                      ((val) << 2)
        
        #define QUIDDIKEY_AR_ALLOW_STOP_GET(value)                 (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP_GETS(value)                (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP_SET(value,field)           (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_AR_ALLOW_STOP(val)                       ((val) << 5)
        
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_AR_ALLOW_GET_KEY(val)                    ((val) << 6)
        
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_GET(value)               (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_GETS(value)              (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP_SET(value,field)         (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_AR_ALLOW_UNWRAP(val)                     ((val) << 7)
        
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_GET(value)         (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_GETS(value)        (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND_SET(value,field)   (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GEN_RND(val)               ((val) << 8)
        
        #define QUIDDIKEY_AR_ALLOW_WRAP_GET(value)                 (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP_GETS(value)                (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP_SET(value,field)           (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_AR_ALLOW_WRAP(val)                       ((val) << 9)
        
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_GET(value)              (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_GETS(value)             (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND_SET(value,field)        (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_AR_ALLOW_GEN_RND(val)                    ((val) << 15)
        
        #define QUIDDIKEY_AR_ALLOW_RESEED_GET(value)               (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED_GETS(value)              (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED_SET(value,field)         (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_AR_ALLOW_RESEED(val)                     ((val) << 16)
        
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_GET(value)             (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_GETS(value)            (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF_SET(value,field)       (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_AR_ALLOW_TEST_PUF(val)                   ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int allow_enroll    :1 ; // Operation allowed status
            unsigned int allow_start     :1 ; // Operation allowed status
            unsigned int padding1:2 ;
            unsigned int allow_stop      :1 ; // Operation allowed status
            unsigned int allow_get_key   :1 ; // Operation allowed status
            unsigned int allow_unwrap    :1 ; // Operation allowed status
            unsigned int allow_wrap_gen_rnd:1 ; // Operation allowed status
            unsigned int allow_wrap      :1 ; // Operation allowed status
            unsigned int padding2:5 ;
            unsigned int allow_gen_rnd   :1 ; // Operation allowed status
            unsigned int allow_reseed    :1 ; // Operation allowed status
            unsigned int padding3:14;
            unsigned int allow_test_puf  :1 ; // Operation allowed status
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ar_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_ar : public vp::reg_32
        {
        public:
            inline void allow_enroll_set(uint32_t value);
            inline uint32_t allow_enroll_get();
            inline void allow_start_set(uint32_t value);
            inline uint32_t allow_start_get();
            inline void allow_stop_set(uint32_t value);
            inline uint32_t allow_stop_get();
            inline void allow_get_key_set(uint32_t value);
            inline uint32_t allow_get_key_get();
            inline void allow_unwrap_set(uint32_t value);
            inline uint32_t allow_unwrap_get();
            inline void allow_wrap_gen_rnd_set(uint32_t value);
            inline uint32_t allow_wrap_gen_rnd_get();
            inline void allow_wrap_set(uint32_t value);
            inline uint32_t allow_wrap_get();
            inline void allow_gen_rnd_set(uint32_t value);
            inline uint32_t allow_gen_rnd_get();
            inline void allow_reseed_set(uint32_t value);
            inline uint32_t allow_reseed_get();
            inline void allow_test_puf_set(uint32_t value);
            inline uint32_t allow_test_puf_get();
        };

|

.. _quiddikey_IER:

IER
"""

Interrupt Enable register

.. table:: 

    +-----+---+------+-------------------------+
    |Bit #|R/W| Name |       Description       |
    +=====+===+======+=========================+
    |    0|R/W|INT_EN|Interrupt enable register|
    +-----+---+------+-------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Interrupt Enable register
                #define QUIDDIKEY_IER_OFFSET                     0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_ier_get(uint32_t base);
        static inline void quiddikey_ier_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Interrupt enable register (access: R/W)
        #define QUIDDIKEY_IER_INT_EN_BIT                                     0
        #define QUIDDIKEY_IER_INT_EN_WIDTH                                   1
        #define QUIDDIKEY_IER_INT_EN_MASK                                    0x1
        #define QUIDDIKEY_IER_INT_EN_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_IER_INT_EN_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IER_INT_EN_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IER_INT_EN_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IER_INT_EN(val)                          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_en          :1 ; // Interrupt enable register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ier_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_ier : public vp::reg_32
        {
        public:
            inline void int_en_set(uint32_t value);
            inline uint32_t int_en_get();
        };

|

.. _quiddikey_IMR:

IMR
"""

Interrupt Mask register

.. table:: 

    +-----+---+----------------------+---------------------------------+
    |Bit #|R/W|         Name         |           Description           |
    +=====+===+======================+=================================+
    |    0|R/W|INT_EN_BUSY           |Enable Busy interrupt            |
    +-----+---+----------------------+---------------------------------+
    |    1|R/W|INT_EN_OK             |Enable Ok interrupt              |
    +-----+---+----------------------+---------------------------------+
    |    2|R/W|INT_EN_ERROR          |Enable Error interrupt           |
    +-----+---+----------------------+---------------------------------+
    |    3|R/W|INT_EN_ZEROIZED       |Enable Zeroized interrupt        |
    +-----+---+----------------------+---------------------------------+
    |    4|R/W|INT_EN_REJECTED       |Enable Rejected interrupt        |
    +-----+---+----------------------+---------------------------------+
    |    5|R/W|INT_EN_DI_REQUEST     |Enable Data In Request interrupt |
    +-----+---+----------------------+---------------------------------+
    |    6|R/W|INT_EN_DO_REQUEST     |Enable Data Out Request interrupt|
    +-----+---+----------------------+---------------------------------+
    |   29|R/W|INT_EN_RESEED_WARNING |Enable Reseed Warning interrupt  |
    +-----+---+----------------------+---------------------------------+
    |   30|R/W|INT_EN_RESEED_REQUIRED|Enable Reseed Required interrupt |
    +-----+---+----------------------+---------------------------------+
    |   31|R/W|INT_EN_LAB_TEST_MODE  |Enable Lab Test Mode interrupt   |
    +-----+---+----------------------+---------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Interrupt Mask register
                #define QUIDDIKEY_IMR_OFFSET                     0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_imr_get(uint32_t base);
        static inline void quiddikey_imr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enable Busy interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_BUSY_BIT                                0
        #define QUIDDIKEY_IMR_INT_EN_BUSY_WIDTH                              1
        #define QUIDDIKEY_IMR_INT_EN_BUSY_MASK                               0x1
        #define QUIDDIKEY_IMR_INT_EN_BUSY_RESET                              0x0
        
        // Enable Ok interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_OK_BIT                                  1
        #define QUIDDIKEY_IMR_INT_EN_OK_WIDTH                                1
        #define QUIDDIKEY_IMR_INT_EN_OK_MASK                                 0x2
        #define QUIDDIKEY_IMR_INT_EN_OK_RESET                                0x0
        
        // Enable Error interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_ERROR_BIT                               2
        #define QUIDDIKEY_IMR_INT_EN_ERROR_WIDTH                             1
        #define QUIDDIKEY_IMR_INT_EN_ERROR_MASK                              0x4
        #define QUIDDIKEY_IMR_INT_EN_ERROR_RESET                             0x0
        
        // Enable Zeroized interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_BIT                            3
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_WIDTH                          1
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_MASK                           0x8
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_RESET                          0x0
        
        // Enable Rejected interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_BIT                            4
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_WIDTH                          1
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_MASK                           0x10
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_RESET                          0x0
        
        // Enable Data In Request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_BIT                          5
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_WIDTH                        1
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_MASK                         0x20
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_RESET                        0x0
        
        // Enable Data Out Request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_BIT                          6
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_WIDTH                        1
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_MASK                         0x40
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_RESET                        0x0
        
        // Enable Reseed Warning interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_BIT                      29
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_WIDTH                    1
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_MASK                     0x20000000
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_RESET                    0x0
        
        // Enable Reseed Required interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_BIT                     30
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_WIDTH                   1
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_MASK                    0x40000000
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_RESET                   0x0
        
        // Enable Lab Test Mode interrupt (access: R/W)
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_BIT                       31
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_WIDTH                     1
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_MASK                      0x80000000
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_IMR_INT_EN_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IMR_INT_EN_BUSY(val)                     ((val) << 0)
        
        #define QUIDDIKEY_IMR_INT_EN_OK_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_IMR_INT_EN_OK(val)                       ((val) << 1)
        
        #define QUIDDIKEY_IMR_INT_EN_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_IMR_INT_EN_ERROR(val)                    ((val) << 2)
        
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_GET(value)           (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_GETS(value)          (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED_SET(value,field)     (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_IMR_INT_EN_ZEROIZED(val)                 ((val) << 3)
        
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_GET(value)           (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_GETS(value)          (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED_SET(value,field)     (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_IMR_INT_EN_REJECTED(val)                 ((val) << 4)
        
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_IMR_INT_EN_DI_REQUEST(val)               ((val) << 5)
        
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_IMR_INT_EN_DO_REQUEST(val)               ((val) << 6)
        
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_GET(value)     (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_GETS(value)    (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING_SET(value,field) (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_WARNING(val)           ((val) << 29)
        
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_GET(value)    (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_GETS(value)   (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED_SET(value,field) (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_IMR_INT_EN_RESEED_REQUIRED(val)          ((val) << 30)
        
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_GET(value)      (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_GETS(value)     (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_IMR_INT_EN_LAB_TEST_MODE(val)            ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_en_busy     :1 ; // Enable Busy interrupt
            unsigned int int_en_ok       :1 ; // Enable Ok interrupt
            unsigned int int_en_error    :1 ; // Enable Error interrupt
            unsigned int int_en_zeroized :1 ; // Enable Zeroized interrupt
            unsigned int int_en_rejected :1 ; // Enable Rejected interrupt
            unsigned int int_en_di_request:1 ; // Enable Data In Request interrupt
            unsigned int int_en_do_request:1 ; // Enable Data Out Request interrupt
            unsigned int padding0:22;
            unsigned int int_en_reseed_warning:1 ; // Enable Reseed Warning interrupt
            unsigned int int_en_reseed_required:1 ; // Enable Reseed Required interrupt
            unsigned int int_en_lab_test_mode:1 ; // Enable Lab Test Mode interrupt
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_imr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_imr : public vp::reg_32
        {
        public:
            inline void int_en_busy_set(uint32_t value);
            inline uint32_t int_en_busy_get();
            inline void int_en_ok_set(uint32_t value);
            inline uint32_t int_en_ok_get();
            inline void int_en_error_set(uint32_t value);
            inline uint32_t int_en_error_get();
            inline void int_en_zeroized_set(uint32_t value);
            inline uint32_t int_en_zeroized_get();
            inline void int_en_rejected_set(uint32_t value);
            inline uint32_t int_en_rejected_get();
            inline void int_en_di_request_set(uint32_t value);
            inline uint32_t int_en_di_request_get();
            inline void int_en_do_request_set(uint32_t value);
            inline uint32_t int_en_do_request_get();
            inline void int_en_reseed_warning_set(uint32_t value);
            inline uint32_t int_en_reseed_warning_get();
            inline void int_en_reseed_required_set(uint32_t value);
            inline uint32_t int_en_reseed_required_get();
            inline void int_en_lab_test_mode_set(uint32_t value);
            inline uint32_t int_en_lab_test_mode_get();
        };

|

.. _quiddikey_ISR:

ISR
"""

Interrupt Status register

.. table:: 

    +-----+---+-------------------+------------------------------------------+
    |Bit #|R/W|       Name        |               Description                |
    +=====+===+===================+==========================================+
    |    0|R/W|INT_BUSY           |Busy interrupt status register            |
    +-----+---+-------------------+------------------------------------------+
    |    1|R/W|INT_OK             |Ok interrupt status register              |
    +-----+---+-------------------+------------------------------------------+
    |    2|R/W|INT_ERROR          |Error interrupt status register           |
    +-----+---+-------------------+------------------------------------------+
    |    3|R/W|INT_ZEROIZED       |Zeroized interrupt status register        |
    +-----+---+-------------------+------------------------------------------+
    |    4|R/W|INT_REJECTED       |Rejected interrupt status register        |
    +-----+---+-------------------+------------------------------------------+
    |    5|R/W|INT_DI_REQUEST     |Data In Request interrupt status register |
    +-----+---+-------------------+------------------------------------------+
    |    6|R/W|INT_DO_REQUEST     |Data Out Request interrupt status register|
    +-----+---+-------------------+------------------------------------------+
    |   29|R/W|INT_RESEED_WARNING |Reseed Warning interrupt status register  |
    +-----+---+-------------------+------------------------------------------+
    |   30|R/W|INT_RESEED_REQUIRED|Reseed Required interrupt status register |
    +-----+---+-------------------+------------------------------------------+
    |   31|R/W|INT_LAB_TEST_MODE  |Lab Test Mode interrupt status register   |
    +-----+---+-------------------+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Interrupt Status register
                #define QUIDDIKEY_ISR_OFFSET                     0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_isr_get(uint32_t base);
        static inline void quiddikey_isr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Busy interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_BUSY_BIT                                   0
        #define QUIDDIKEY_ISR_INT_BUSY_WIDTH                                 1
        #define QUIDDIKEY_ISR_INT_BUSY_MASK                                  0x1
        #define QUIDDIKEY_ISR_INT_BUSY_RESET                                 0x0
        
        // Ok interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_OK_BIT                                     1
        #define QUIDDIKEY_ISR_INT_OK_WIDTH                                   1
        #define QUIDDIKEY_ISR_INT_OK_MASK                                    0x2
        #define QUIDDIKEY_ISR_INT_OK_RESET                                   0x0
        
        // Error interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_ERROR_BIT                                  2
        #define QUIDDIKEY_ISR_INT_ERROR_WIDTH                                1
        #define QUIDDIKEY_ISR_INT_ERROR_MASK                                 0x4
        #define QUIDDIKEY_ISR_INT_ERROR_RESET                                0x0
        
        // Zeroized interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_ZEROIZED_BIT                               3
        #define QUIDDIKEY_ISR_INT_ZEROIZED_WIDTH                             1
        #define QUIDDIKEY_ISR_INT_ZEROIZED_MASK                              0x8
        #define QUIDDIKEY_ISR_INT_ZEROIZED_RESET                             0x0
        
        // Rejected interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_REJECTED_BIT                               4
        #define QUIDDIKEY_ISR_INT_REJECTED_WIDTH                             1
        #define QUIDDIKEY_ISR_INT_REJECTED_MASK                              0x10
        #define QUIDDIKEY_ISR_INT_REJECTED_RESET                             0x0
        
        // Data In Request interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_BIT                             5
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_WIDTH                           1
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_MASK                            0x20
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_RESET                           0x0
        
        // Data Out Request interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_BIT                             6
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_WIDTH                           1
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_MASK                            0x40
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_RESET                           0x0
        
        // Reseed Warning interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_BIT                         29
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_WIDTH                       1
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_MASK                        0x20000000
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_RESET                       0x0
        
        // Reseed Required interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_BIT                        30
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_WIDTH                      1
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_MASK                       0x40000000
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_RESET                      0x0
        
        // Lab Test Mode interrupt status register (access: R/W)
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_BIT                          31
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_WIDTH                        1
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_MASK                         0x80000000
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_ISR_INT_BUSY_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_ISR_INT_BUSY(val)                        ((val) << 0)
        
        #define QUIDDIKEY_ISR_INT_OK_GET(value)                    (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_ISR_INT_OK_GETS(value)                   (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_ISR_INT_OK_SET(value,field)              (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_ISR_INT_OK(val)                          ((val) << 1)
        
        #define QUIDDIKEY_ISR_INT_ERROR_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_ISR_INT_ERROR(val)                       ((val) << 2)
        
        #define QUIDDIKEY_ISR_INT_ZEROIZED_GET(value)              (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED_GETS(value)             (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED_SET(value,field)        (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_ISR_INT_ZEROIZED(val)                    ((val) << 3)
        
        #define QUIDDIKEY_ISR_INT_REJECTED_GET(value)              (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED_GETS(value)             (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED_SET(value,field)        (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_ISR_INT_REJECTED(val)                    ((val) << 4)
        
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_GET(value)            (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_GETS(value)           (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST_SET(value,field)      (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_ISR_INT_DI_REQUEST(val)                  ((val) << 5)
        
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_GET(value)            (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_GETS(value)           (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST_SET(value,field)      (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_ISR_INT_DO_REQUEST(val)                  ((val) << 6)
        
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_GET(value)        (GAP_BEXTRACTU((value),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_GETS(value)       (GAP_BEXTRACT((value),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING_SET(value,field)  (GAP_BINSERT((value),(field),1,29))
        #define QUIDDIKEY_ISR_INT_RESEED_WARNING(val)              ((val) << 29)
        
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_GET(value)       (GAP_BEXTRACTU((value),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_GETS(value)      (GAP_BEXTRACT((value),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED_SET(value,field) (GAP_BINSERT((value),(field),1,30))
        #define QUIDDIKEY_ISR_INT_RESEED_REQUIRED(val)             ((val) << 30)
        
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_GET(value)         (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_GETS(value)        (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE_SET(value,field)   (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_ISR_INT_LAB_TEST_MODE(val)               ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int int_busy        :1 ; // Busy interrupt status register
            unsigned int int_ok          :1 ; // Ok interrupt status register
            unsigned int int_error       :1 ; // Error interrupt status register
            unsigned int int_zeroized    :1 ; // Zeroized interrupt status register
            unsigned int int_rejected    :1 ; // Rejected interrupt status register
            unsigned int int_di_request  :1 ; // Data In Request interrupt status register
            unsigned int int_do_request  :1 ; // Data Out Request interrupt status register
            unsigned int padding0:22;
            unsigned int int_reseed_warning:1 ; // Reseed Warning interrupt status register
            unsigned int int_reseed_required:1 ; // Reseed Required interrupt status register
            unsigned int int_lab_test_mode:1 ; // Lab Test Mode interrupt status register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_isr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_isr : public vp::reg_32
        {
        public:
            inline void int_busy_set(uint32_t value);
            inline uint32_t int_busy_get();
            inline void int_ok_set(uint32_t value);
            inline uint32_t int_ok_get();
            inline void int_error_set(uint32_t value);
            inline uint32_t int_error_get();
            inline void int_zeroized_set(uint32_t value);
            inline uint32_t int_zeroized_get();
            inline void int_rejected_set(uint32_t value);
            inline uint32_t int_rejected_get();
            inline void int_di_request_set(uint32_t value);
            inline uint32_t int_di_request_get();
            inline void int_do_request_set(uint32_t value);
            inline uint32_t int_do_request_get();
            inline void int_reseed_warning_set(uint32_t value);
            inline uint32_t int_reseed_warning_get();
            inline void int_reseed_required_set(uint32_t value);
            inline uint32_t int_reseed_required_get();
            inline void int_lab_test_mode_set(uint32_t value);
            inline uint32_t int_lab_test_mode_get();
        };

|

.. _quiddikey_DATA_DEST:

DATA_DEST
"""""""""

Destination Data register

.. table:: 

    +-----+---+--------+-----------------------------------+
    |Bit #|R/W|  Name  |            Description            |
    +=====+===+========+===================================+
    |    0|R/W|DEST_DOR|Data out register destination      |
    +-----+---+--------+-----------------------------------+
    |    1|R/W|DEST_SO |Secure Output interface destination|
    +-----+---+--------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Destination Data register
                #define QUIDDIKEY_DATA_DEST_OFFSET               0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_data_dest_get(uint32_t base);
        static inline void quiddikey_data_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data out register destination (access: R/W)
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_BIT                             0
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_WIDTH                           1
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_MASK                            0x1
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_RESET                           0x0
        
        // Secure Output interface destination (access: R/W)
        #define QUIDDIKEY_DATA_DEST_DEST_SO_BIT                              1
        #define QUIDDIKEY_DATA_DEST_DEST_SO_WIDTH                            1
        #define QUIDDIKEY_DATA_DEST_DEST_SO_MASK                             0x2
        #define QUIDDIKEY_DATA_DEST_DEST_SO_RESET                            0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_DATA_DEST_DEST_DOR(val)                  ((val) << 0)
        
        #define QUIDDIKEY_DATA_DEST_DEST_SO_GET(value)             (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO_GETS(value)            (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO_SET(value,field)       (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_DATA_DEST_DEST_SO(val)                   ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int dest_dor        :1 ; // Data out register destination
            unsigned int dest_so         :1 ; // Secure Output interface destination
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_data_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_data_dest : public vp::reg_32
        {
        public:
            inline void dest_dor_set(uint32_t value);
            inline uint32_t dest_dor_get();
            inline void dest_so_set(uint32_t value);
            inline uint32_t dest_so_get();
        };

|

.. _quiddikey_DATA_SRC (not mapped ?):

DATA_SRC (not mapped ?)
"""""""""""""""""""""""

Data Source register

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

        
                // Data Source register
                #define QUIDDIKEY_DATA_SRC (NOT MAPPED ?)_OFFSET 0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_data_src (not mapped ?)_get(uint32_t base);
        static inline void quiddikey_data_src (not mapped ?)_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) quiddikey_data_src (not mapped ?)_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_data_src (not mapped ?) : public vp::reg_32
        {
        public:
        };

|

.. _quiddikey_KEY_INDEX:

KEY_INDEX
"""""""""

Key Index register

.. table:: 

    +-----+---+---------+----------------------+
    |Bit #|R/W|  Name   |     Description      |
    +=====+===+=========+======================+
    |3:0  |R  |KEY_INDEX|Value of the Key Index|
    +-----+---+---------+----------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Key Index register
                #define QUIDDIKEY_KEY_INDEX_OFFSET               0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_key_index_get(uint32_t base);
        static inline void quiddikey_key_index_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Value of the Key Index (access: R)
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_BIT                            0
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_WIDTH                          4
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_MASK                           0xf
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_GET(value)           (GAP_BEXTRACTU((value),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_GETS(value)          (GAP_BEXTRACT((value),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX_SET(value,field)     (GAP_BINSERT((value),(field),4,0))
        #define QUIDDIKEY_KEY_INDEX_KEY_INDEX(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int key_index       :4 ; // Value of the Key Index
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_key_index_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_key_index : public vp::reg_32
        {
        public:
            inline void key_index_set(uint32_t value);
            inline uint32_t key_index_get();
        };

|

.. _quiddikey_DIR:

DIR
"""

Data Input register

.. table:: 

    +-----+---+----+-------------+
    |Bit #|R/W|Name| Description |
    +=====+===+====+=============+
    |31:0 |R/W|DI  |Data In field|
    +-----+---+----+-------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Data Input register
                #define QUIDDIKEY_DIR_OFFSET                     0xa0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_dir_get(uint32_t base);
        static inline void quiddikey_dir_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data In field (access: R/W)
        #define QUIDDIKEY_DIR_DI_BIT                                         0
        #define QUIDDIKEY_DIR_DI_WIDTH                                       32
        #define QUIDDIKEY_DIR_DI_MASK                                        0xffffffff
        #define QUIDDIKEY_DIR_DI_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_DIR_DI_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DIR_DI_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DIR_DI_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DIR_DI(val)                              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int di              :32; // Data In field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dir_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_dir : public vp::reg_32
        {
        public:
            inline void di_set(uint32_t value);
            inline uint32_t di_get();
        };

|

.. _quiddikey_DOR:

DOR
"""

Data Output register

.. table:: 

    +-----+---+----+--------------+
    |Bit #|R/W|Name| Description  |
    +=====+===+====+==============+
    |31:0 |R  |DO  |Data Out field|
    +-----+---+----+--------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Data Output register
                #define QUIDDIKEY_DOR_OFFSET                     0xa8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_dor_get(uint32_t base);
        static inline void quiddikey_dor_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Data Out field (access: R)
        #define QUIDDIKEY_DOR_DO_BIT                                         0
        #define QUIDDIKEY_DOR_DO_WIDTH                                       32
        #define QUIDDIKEY_DOR_DO_MASK                                        0xffffffff
        #define QUIDDIKEY_DOR_DO_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_DOR_DO_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DOR_DO_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DOR_DO_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DOR_DO(val)                              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int do              :32; // Data Out field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dor_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_dor : public vp::reg_32
        {
        public:
            inline void do_set(uint32_t value);
            inline uint32_t do_get();
        };

|

.. _quiddikey_MISC:

MISC
""""

Miscellaneous register

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

        
                // Miscellaneous register
                #define QUIDDIKEY_MISC_OFFSET                    0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_misc_get(uint32_t base);
        static inline void quiddikey_misc_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) quiddikey_misc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_misc : public vp::reg_32
        {
        public:
        };

|

.. _quiddikey_IF_SR:

IF_SR
"""""

Interface Status register

.. table:: 

    +-----+---+---------+------------------------+
    |Bit #|R/W|  Name   |      Description       |
    +=====+===+=========+========================+
    |    0|R/W|APB_ERROR|An APB error has occured|
    +-----+---+---------+------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Interface Status register
                #define QUIDDIKEY_IF_SR_OFFSET                   0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_if_sr_get(uint32_t base);
        static inline void quiddikey_if_sr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // An APB error has occured (access: R/W)
        #define QUIDDIKEY_IF_SR_APB_ERROR_BIT                                0
        #define QUIDDIKEY_IF_SR_APB_ERROR_WIDTH                              1
        #define QUIDDIKEY_IF_SR_APB_ERROR_MASK                               0x1
        #define QUIDDIKEY_IF_SR_APB_ERROR_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_IF_SR_APB_ERROR_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR(val)                     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int apb_error       :1 ; // An APB error has occured
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_if_sr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_if_sr : public vp::reg_32
        {
        public:
            inline void apb_error_set(uint32_t value);
            inline uint32_t apb_error_get();
        };

|

.. _quiddikey_TEST:

TEST
""""

Test register

.. table:: 

    +-----+---+------------+-----------------------------------+
    |Bit #|R/W|    Name    |            Description            |
    +=====+===+============+===================================+
    |    0|R/W|BIST_ENABLE |Isolates Quiddikey and runs BIST   |
    +-----+---+------------+-----------------------------------+
    |    4|R/W|BIST_RUNNING|BIST is in progress or finishing up|
    +-----+---+------------+-----------------------------------+
    |    5|R/W|BIST_ACTIVE |BIST is in progress                |
    +-----+---+------------+-----------------------------------+
    |    6|R/W|BIST_OK     |BIST has passed                    |
    +-----+---+------------+-----------------------------------+
    |    7|R/W|BIST_ERROR  |BIST has failed                    |
    +-----+---+------------+-----------------------------------+
    |   31|R/W|ALLOW_BIST  |BIST is not allowed                |
    +-----+---+------------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Test register
                #define QUIDDIKEY_TEST_OFFSET                    0xd8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_test_get(uint32_t base);
        static inline void quiddikey_test_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Isolates Quiddikey and runs BIST (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ENABLE_BIT                               0
        #define QUIDDIKEY_TEST_BIST_ENABLE_WIDTH                             1
        #define QUIDDIKEY_TEST_BIST_ENABLE_MASK                              0x1
        #define QUIDDIKEY_TEST_BIST_ENABLE_RESET                             0x0
        
        // BIST is in progress or finishing up (access: R/W)
        #define QUIDDIKEY_TEST_BIST_RUNNING_BIT                              4
        #define QUIDDIKEY_TEST_BIST_RUNNING_WIDTH                            1
        #define QUIDDIKEY_TEST_BIST_RUNNING_MASK                             0x10
        #define QUIDDIKEY_TEST_BIST_RUNNING_RESET                            0x0
        
        // BIST is in progress (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ACTIVE_BIT                               5
        #define QUIDDIKEY_TEST_BIST_ACTIVE_WIDTH                             1
        #define QUIDDIKEY_TEST_BIST_ACTIVE_MASK                              0x20
        #define QUIDDIKEY_TEST_BIST_ACTIVE_RESET                             0x0
        
        // BIST has passed (access: R/W)
        #define QUIDDIKEY_TEST_BIST_OK_BIT                                   6
        #define QUIDDIKEY_TEST_BIST_OK_WIDTH                                 1
        #define QUIDDIKEY_TEST_BIST_OK_MASK                                  0x40
        #define QUIDDIKEY_TEST_BIST_OK_RESET                                 0x0
        
        // BIST has failed (access: R/W)
        #define QUIDDIKEY_TEST_BIST_ERROR_BIT                                7
        #define QUIDDIKEY_TEST_BIST_ERROR_WIDTH                              1
        #define QUIDDIKEY_TEST_BIST_ERROR_MASK                               0x80
        #define QUIDDIKEY_TEST_BIST_ERROR_RESET                              0x0
        
        // BIST is not allowed (access: R/W)
        #define QUIDDIKEY_TEST_ALLOW_BIST_BIT                                31
        #define QUIDDIKEY_TEST_ALLOW_BIST_WIDTH                              1
        #define QUIDDIKEY_TEST_ALLOW_BIST_MASK                               0x80000000
        #define QUIDDIKEY_TEST_ALLOW_BIST_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_TEST_BIST_ENABLE_GET(value)              (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE_GETS(value)             (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE_SET(value,field)        (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_TEST_BIST_ENABLE(val)                    ((val) << 0)
        
        #define QUIDDIKEY_TEST_BIST_RUNNING_GET(value)             (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING_GETS(value)            (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING_SET(value,field)       (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_TEST_BIST_RUNNING(val)                   ((val) << 4)
        
        #define QUIDDIKEY_TEST_BIST_ACTIVE_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_TEST_BIST_ACTIVE(val)                    ((val) << 5)
        
        #define QUIDDIKEY_TEST_BIST_OK_GET(value)                  (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_TEST_BIST_OK_GETS(value)                 (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_TEST_BIST_OK_SET(value,field)            (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_TEST_BIST_OK(val)                        ((val) << 6)
        
        #define QUIDDIKEY_TEST_BIST_ERROR_GET(value)               (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR_GETS(value)              (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR_SET(value,field)         (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_TEST_BIST_ERROR(val)                     ((val) << 7)
        
        #define QUIDDIKEY_TEST_ALLOW_BIST_GET(value)               (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST_GETS(value)              (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST_SET(value,field)         (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_TEST_ALLOW_BIST(val)                     ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int bist_enable     :1 ; // Isolates Quiddikey and runs BIST
            unsigned int padding0:3 ;
            unsigned int bist_running    :1 ; // BIST is in progress or finishing up
            unsigned int bist_active     :1 ; // BIST is in progress
            unsigned int bist_ok         :1 ; // BIST has passed
            unsigned int bist_error      :1 ; // BIST has failed
            unsigned int padding1:23;
            unsigned int allow_bist      :1 ; // BIST is not allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_test_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_test : public vp::reg_32
        {
        public:
            inline void bist_enable_set(uint32_t value);
            inline uint32_t bist_enable_get();
            inline void bist_running_set(uint32_t value);
            inline uint32_t bist_running_get();
            inline void bist_active_set(uint32_t value);
            inline uint32_t bist_active_get();
            inline void bist_ok_set(uint32_t value);
            inline uint32_t bist_ok_get();
            inline void bist_error_set(uint32_t value);
            inline uint32_t bist_error_get();
            inline void allow_bist_set(uint32_t value);
            inline uint32_t allow_bist_get();
        };

|

.. _quiddikey_PSR:

PSR
"""

PUF Score register

.. table:: 

    +-----+---+---------+---------------+
    |Bit #|R/W|  Name   |  Description  |
    +=====+===+=========+===============+
    |3:0  |R/W|PUF_SCORE|PUF Score field|
    +-----+---+---------+---------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // PUF Score register
                #define QUIDDIKEY_PSR_OFFSET                     0xdc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_psr_get(uint32_t base);
        static inline void quiddikey_psr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // PUF Score field (access: R/W)
        #define QUIDDIKEY_PSR_PUF_SCORE_BIT                                  0
        #define QUIDDIKEY_PSR_PUF_SCORE_WIDTH                                4
        #define QUIDDIKEY_PSR_PUF_SCORE_MASK                                 0xf
        #define QUIDDIKEY_PSR_PUF_SCORE_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_PSR_PUF_SCORE_GET(value)                 (GAP_BEXTRACTU((value),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE_GETS(value)                (GAP_BEXTRACT((value),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE_SET(value,field)           (GAP_BINSERT((value),(field),4,0))
        #define QUIDDIKEY_PSR_PUF_SCORE(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int puf_score       :4 ; // PUF Score field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_psr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_psr : public vp::reg_32
        {
        public:
            inline void puf_score_set(uint32_t value);
            inline uint32_t puf_score_get();
        };

|

.. _quiddikey_HW_RUC0:

HW_RUC0
"""""""

Hardware Restrict User Context 0 register

.. table:: 

    +-----+---+-----------------------+-----------------------------+
    |Bit #|R/W|         Name          |         Description         |
    +=====+===+=======================+=============================+
    |31:0 |R/W|RESTRICT_USER_CONTEXT_0|Restrict User Context 0 field|
    +-----+---+-----------------------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Restrict User Context 0 register
                #define QUIDDIKEY_HW_RUC0_OFFSET                 0xe0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ruc0_get(uint32_t base);
        static inline void quiddikey_hw_ruc0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Restrict User Context 0 field (access: R/W)
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_BIT                0
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_WIDTH              32
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_MASK               0xffffffff
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC0_RESTRICT_USER_CONTEXT_0(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int restrict_user_context_0:32; // Restrict User Context 0 field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ruc0 : public vp::reg_32
        {
        public:
            inline void restrict_user_context_0_set(uint32_t value);
            inline uint32_t restrict_user_context_0_get();
        };

|

.. _quiddikey_HW_RUC1:

HW_RUC1
"""""""

Hardware Restrict User Context 1 register

.. table:: 

    +-----+---+-----------------------+-----------------------------+
    |Bit #|R/W|         Name          |         Description         |
    +=====+===+=======================+=============================+
    |31:0 |R/W|RESTRICT_USER_CONTEXT_1|Restrict User Context 1 field|
    +-----+---+-----------------------+-----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Restrict User Context 1 register
                #define QUIDDIKEY_HW_RUC1_OFFSET                 0xe4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ruc1_get(uint32_t base);
        static inline void quiddikey_hw_ruc1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Restrict User Context 1 field (access: R/W)
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_BIT                0
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_WIDTH              32
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_MASK               0xffffffff
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_RESET              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC1_RESTRICT_USER_CONTEXT_1(val)     ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int restrict_user_context_1:32; // Restrict User Context 1 field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ruc1 : public vp::reg_32
        {
        public:
            inline void restrict_user_context_1_set(uint32_t value);
            inline uint32_t restrict_user_context_1_get();
        };

|

.. _quiddikey_HW_SETTINGS:

HW_SETTINGS
"""""""""""

Hardware Settings register

.. table:: 

    +-----+---+--------------------------+------------------------------------+
    |Bit #|R/W|           Name           |            Description             |
    +=====+===+==========================+====================================+
    |    1|R/W|DISABLE_ENROLL            |Enroll settings field               |
    +-----+---+--------------------------+------------------------------------+
    |    2|R/W|DISABLE_START             |Start settings field                |
    +-----+---+--------------------------+------------------------------------+
    |    5|R/W|DISABLE_STOP              |Stop settings field                 |
    +-----+---+--------------------------+------------------------------------+
    |    6|R/W|DISABLE_GET_KEY           |Get Key settings field              |
    +-----+---+--------------------------+------------------------------------+
    |    7|R/W|DISABLE_UNWRAP            |Unwrap settings field               |
    +-----+---+--------------------------+------------------------------------+
    |    8|R/W|DISABLE_WRAP_GEN_RND      |Wrap Generated Random settings field|
    +-----+---+--------------------------+------------------------------------+
    |    9|R/W|DISABLE_WRAP              |Wrap settings field                 |
    +-----+---+--------------------------+------------------------------------+
    |   15|R/W|DISABLE_GEN_RND           |Generate Random settings field      |
    +-----+---+--------------------------+------------------------------------+
    |   16|R/W|DISABLE_RESEED            |Reseed settings field               |
    +-----+---+--------------------------+------------------------------------+
    |   24|R/W|DISABLE_LAB_TEST_MODE     |Lab Test Mode settings field        |
    +-----+---+--------------------------+------------------------------------+
    |   25|R/W|SELECT_LAB_TEST_MODE      |Lab Test Mode select field          |
    +-----+---+--------------------------+------------------------------------+
    |   27|R/W|REQUIRE_RESEED_SRC_VIA_DIR|Reseed via DIR settings field       |
    +-----+---+--------------------------+------------------------------------+
    |   28|R/W|REQUIRE_RESEED_SRC_VIA_SI |Reseed via SI settings field        |
    +-----+---+--------------------------+------------------------------------+
    |   31|R/W|DISABLE_TEST_PUF          |Test PUF settings field             |
    +-----+---+--------------------------+------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Settings register
                #define QUIDDIKEY_HW_SETTINGS_OFFSET             0xf0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_settings_get(uint32_t base);
        static inline void quiddikey_hw_settings_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enroll settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_BIT                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_MASK                    0x2
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_RESET                   0x0
        
        // Start settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_BIT                      2
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_WIDTH                    1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_MASK                     0x4
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_RESET                    0x0
        
        // Stop settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_BIT                       5
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_WIDTH                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_MASK                      0x20
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_RESET                     0x0
        
        // Get Key settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_BIT                    6
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_WIDTH                  1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_MASK                   0x40
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_RESET                  0x0
        
        // Unwrap settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_BIT                     7
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_MASK                    0x80
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_RESET                   0x0
        
        // Wrap Generated Random settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_BIT               8
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_WIDTH             1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_MASK              0x100
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_RESET             0x0
        
        // Wrap settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_BIT                       9
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_WIDTH                     1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_MASK                      0x200
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_RESET                     0x0
        
        // Generate Random settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_BIT                    15
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_WIDTH                  1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_MASK                   0x8000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_RESET                  0x0
        
        // Reseed settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_BIT                     16
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_WIDTH                   1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_MASK                    0x10000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_RESET                   0x0
        
        // Lab Test Mode settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_BIT              24
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_WIDTH            1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_MASK             0x1000000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_RESET            0x0
        
        // Lab Test Mode select field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_BIT               25
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_WIDTH             1
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_MASK              0x2000000
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_RESET             0x0
        
        // Reseed via DIR settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_BIT         27
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_WIDTH       1
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_MASK        0x8000000
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_RESET       0x0
        
        // Reseed via SI settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_BIT          28
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_WIDTH        1
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_MASK         0x10000000
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_RESET        0x0
        
        // Test PUF settings field (access: R/W)
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_BIT                   31
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_WIDTH                 1
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_MASK                  0x80000000
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_RESET                 0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_GET(value)    (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_GETS(value)   (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_ENROLL(val)          ((val) << 1)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_GET(value)     (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_GETS(value)    (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_START(val)           ((val) << 2)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_STOP(val)            ((val) << 5)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_GET(value)   (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_GETS(value)  (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GET_KEY(val)         ((val) << 6)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_GET(value)    (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_GETS(value)   (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_UNWRAP(val)          ((val) << 7)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GEN_RND(val)    ((val) << 8)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GET(value)      (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_GETS(value)     (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_WRAP(val)            ((val) << 9)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_GET(value)   (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_GETS(value)  (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_GEN_RND(val)         ((val) << 15)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_GET(value)    (GAP_BEXTRACTU((value),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_GETS(value)   (GAP_BEXTRACT((value),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED_SET(value,field) (GAP_BINSERT((value),(field),1,16))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_RESEED(val)          ((val) << 16)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_GET(value) (GAP_BEXTRACTU((value),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_GETS(value) (GAP_BEXTRACT((value),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,24))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_LAB_TEST_MODE(val)   ((val) << 24)
        
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_GET(value) (GAP_BEXTRACTU((value),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_GETS(value) (GAP_BEXTRACT((value),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE_SET(value,field) (GAP_BINSERT((value),(field),1,25))
        #define QUIDDIKEY_HW_SETTINGS_SELECT_LAB_TEST_MODE(val)    ((val) << 25)
        
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_GET(value) (GAP_BEXTRACTU((value),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_GETS(value) (GAP_BEXTRACT((value),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR_SET(value,field) (GAP_BINSERT((value),(field),1,27))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_DIR(val) ((val) << 27)
        
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_GET(value) (GAP_BEXTRACTU((value),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_GETS(value) (GAP_BEXTRACT((value),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI_SET(value,field) (GAP_BINSERT((value),(field),1,28))
        #define QUIDDIKEY_HW_SETTINGS_REQUIRE_RESEED_SRC_VIA_SI(val) ((val) << 28)
        
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_GET(value)  (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_GETS(value) (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF_SET(value,field) (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_HW_SETTINGS_DISABLE_TEST_PUF(val)        ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int disable_enroll  :1 ; // Enroll settings field
            unsigned int disable_start   :1 ; // Start settings field
            unsigned int padding1:2 ;
            unsigned int disable_stop    :1 ; // Stop settings field
            unsigned int disable_get_key :1 ; // Get Key settings field
            unsigned int disable_unwrap  :1 ; // Unwrap settings field
            unsigned int disable_wrap_gen_rnd:1 ; // Wrap Generated Random settings field
            unsigned int disable_wrap    :1 ; // Wrap settings field
            unsigned int padding2:5 ;
            unsigned int disable_gen_rnd :1 ; // Generate Random settings field
            unsigned int disable_reseed  :1 ; // Reseed settings field
            unsigned int padding3:7 ;
            unsigned int disable_lab_test_mode:1 ; // Lab Test Mode settings field
            unsigned int select_lab_test_mode:1 ; // Lab Test Mode select field
            unsigned int padding4:1 ;
            unsigned int require_reseed_src_via_dir:1 ; // Reseed via DIR settings field
            unsigned int require_reseed_src_via_si:1 ; // Reseed via SI settings field
            unsigned int padding5:2 ;
            unsigned int disable_test_puf:1 ; // Test PUF settings field
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_settings_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_settings : public vp::reg_32
        {
        public:
            inline void disable_enroll_set(uint32_t value);
            inline uint32_t disable_enroll_get();
            inline void disable_start_set(uint32_t value);
            inline uint32_t disable_start_get();
            inline void disable_stop_set(uint32_t value);
            inline uint32_t disable_stop_get();
            inline void disable_get_key_set(uint32_t value);
            inline uint32_t disable_get_key_get();
            inline void disable_unwrap_set(uint32_t value);
            inline uint32_t disable_unwrap_get();
            inline void disable_wrap_gen_rnd_set(uint32_t value);
            inline uint32_t disable_wrap_gen_rnd_get();
            inline void disable_wrap_set(uint32_t value);
            inline uint32_t disable_wrap_get();
            inline void disable_gen_rnd_set(uint32_t value);
            inline uint32_t disable_gen_rnd_get();
            inline void disable_reseed_set(uint32_t value);
            inline uint32_t disable_reseed_get();
            inline void disable_lab_test_mode_set(uint32_t value);
            inline uint32_t disable_lab_test_mode_get();
            inline void select_lab_test_mode_set(uint32_t value);
            inline uint32_t select_lab_test_mode_get();
            inline void require_reseed_src_via_dir_set(uint32_t value);
            inline uint32_t require_reseed_src_via_dir_get();
            inline void require_reseed_src_via_si_set(uint32_t value);
            inline uint32_t require_reseed_src_via_si_get();
            inline void disable_test_puf_set(uint32_t value);
            inline uint32_t disable_test_puf_get();
        };

|

.. _quiddikey_HW_INFO:

HW_INFO
"""""""

Hardware Information register

.. table:: 

    +-----+---+----------------+-----------------------------------------+
    |Bit #|R/W|      Name      |               Description               |
    +=====+===+================+=========================================+
    |   21|R/W|CONFIG_SP_800_90|1: SP 800-90 is included, 0: not included|
    +-----+---+----------------+-----------------------------------------+
    |   22|R/W|CONFIG_BIST     |1: BIST is included, 0: not included     |
    +-----+---+----------------+-----------------------------------------+
    |   23|R/W|RESERVED        |1: Safe, 0: Plus                         |
    +-----+---+----------------+-----------------------------------------+
    |   24|R/W|CONFIG_WRAP     |1: Wrap is included, 0: not included     |
    +-----+---+----------------+-----------------------------------------+
    |31:28|R/W|CONFIG_TYPE     |Quiddikey configuration                  |
    +-----+---+----------------+-----------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Information register
                #define QUIDDIKEY_HW_INFO_OFFSET                 0xf4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_info_get(uint32_t base);
        static inline void quiddikey_hw_info_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 1: SP 800-90 is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_BIT                       21
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_WIDTH                     1
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_MASK                      0x200000
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_RESET                     0x0
        
        // 1: BIST is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_BIT                            22
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_WIDTH                          1
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_MASK                           0x400000
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_RESET                          0x0
        
        // 1: Safe, 0: Plus (access: R/W)
        #define QUIDDIKEY_HW_INFO_RESERVED_BIT                               23
        #define QUIDDIKEY_HW_INFO_RESERVED_WIDTH                             1
        #define QUIDDIKEY_HW_INFO_RESERVED_MASK                              0x800000
        #define QUIDDIKEY_HW_INFO_RESERVED_RESET                             0x0
        
        // 1: Wrap is included, 0: not included (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_BIT                            24
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_WIDTH                          1
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_MASK                           0x1000000
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_RESET                          0x0
        
        // Quiddikey configuration (access: R/W)
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_BIT                            28
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_WIDTH                          4
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_MASK                           0xf0000000
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_GET(value)      (GAP_BEXTRACTU((value),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_GETS(value)     (GAP_BEXTRACT((value),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90_SET(value,field) (GAP_BINSERT((value),(field),1,21))
        #define QUIDDIKEY_HW_INFO_CONFIG_SP_800_90(val)            ((val) << 21)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define QUIDDIKEY_HW_INFO_CONFIG_BIST(val)                 ((val) << 22)
        
        #define QUIDDIKEY_HW_INFO_RESERVED_GET(value)              (GAP_BEXTRACTU((value),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED_GETS(value)             (GAP_BEXTRACT((value),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED_SET(value,field)        (GAP_BINSERT((value),(field),1,23))
        #define QUIDDIKEY_HW_INFO_RESERVED(val)                    ((val) << 23)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_GET(value)           (GAP_BEXTRACTU((value),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_GETS(value)          (GAP_BEXTRACT((value),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP_SET(value,field)     (GAP_BINSERT((value),(field),1,24))
        #define QUIDDIKEY_HW_INFO_CONFIG_WRAP(val)                 ((val) << 24)
        
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_GET(value)           (GAP_BEXTRACTU((value),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_GETS(value)          (GAP_BEXTRACT((value),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE_SET(value,field)     (GAP_BINSERT((value),(field),4,28))
        #define QUIDDIKEY_HW_INFO_CONFIG_TYPE(val)                 ((val) << 28)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:21;
            unsigned int config_sp_800_90:1 ; // 1: SP 800-90 is included, 0: not included
            unsigned int config_bist     :1 ; // 1: BIST is included, 0: not included
            unsigned int reserved        :1 ; // 1: Safe, 0: Plus
            unsigned int config_wrap     :1 ; // 1: Wrap is included, 0: not included
            unsigned int padding1:3 ;
            unsigned int config_type     :4 ; // Quiddikey configuration
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_info_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_info : public vp::reg_32
        {
        public:
            inline void config_sp_800_90_set(uint32_t value);
            inline uint32_t config_sp_800_90_get();
            inline void config_bist_set(uint32_t value);
            inline uint32_t config_bist_get();
            inline void reserved_set(uint32_t value);
            inline uint32_t reserved_get();
            inline void config_wrap_set(uint32_t value);
            inline uint32_t config_wrap_get();
            inline void config_type_set(uint32_t value);
            inline uint32_t config_type_get();
        };

|

.. _quiddikey_HW_ID:

HW_ID
"""""

Hardware Identifier register

.. table:: 

    +-----+---+-----+-------------------+
    |Bit #|R/W|Name |    Description    |
    +=====+===+=====+===================+
    |31:0 |R/W|HW_ID|Hardware Identifier|
    +-----+---+-----+-------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Identifier register
                #define QUIDDIKEY_HW_ID_OFFSET                   0xf8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_id_get(uint32_t base);
        static inline void quiddikey_hw_id_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware Identifier (access: R/W)
        #define QUIDDIKEY_HW_ID_HW_ID_BIT                                    0
        #define QUIDDIKEY_HW_ID_HW_ID_WIDTH                                  32
        #define QUIDDIKEY_HW_ID_HW_ID_MASK                                   0xffffffff
        #define QUIDDIKEY_HW_ID_HW_ID_RESET                                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_ID_HW_ID_GET(value)                   (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID_GETS(value)                  (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID_SET(value,field)             (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_ID_HW_ID(val)                         ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int hw_id           :32; // Hardware Identifier
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_id_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_id : public vp::reg_32
        {
        public:
            inline void hw_id_set(uint32_t value);
            inline uint32_t hw_id_get();
        };

|

.. _quiddikey_HW_VER:

HW_VER
""""""

Hardware Version register

.. table:: 

    +-----+---+------------+-------------------------------+
    |Bit #|R/W|    Name    |          Description          |
    +=====+===+============+===============================+
    |7:0  |R/W|HW_VER_REV  |Hardware version, revision part|
    +-----+---+------------+-------------------------------+
    |15:8 |R/W|HW_VER_MINOR|Hardware version, minor part   |
    +-----+---+------------+-------------------------------+
    |23:16|R/W|HW_VER_MAJOR|Hardware version, major part   |
    +-----+---+------------+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Hardware Version register
                #define QUIDDIKEY_HW_VER_OFFSET                  0xfc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ver_get(uint32_t base);
        static inline void quiddikey_hw_ver_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware version, revision part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_REV_BIT                              0
        #define QUIDDIKEY_HW_VER_HW_VER_REV_WIDTH                            8
        #define QUIDDIKEY_HW_VER_HW_VER_REV_MASK                             0xff
        #define QUIDDIKEY_HW_VER_HW_VER_REV_RESET                            0x0
        
        // Hardware version, minor part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_BIT                            8
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_WIDTH                          8
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_MASK                           0xff00
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_RESET                          0x0
        
        // Hardware version, major part (access: R/W)
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_BIT                            16
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_WIDTH                          8
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_MASK                           0xff0000
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_VER_HW_VER_REV_GET(value)             (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV_GETS(value)            (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV_SET(value,field)       (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_HW_VER_HW_VER_REV(val)                   ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define QUIDDIKEY_HW_VER_HW_VER_MINOR(val)                 ((val) << 8)
        
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_GET(value)           (GAP_BEXTRACTU((value),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_GETS(value)          (GAP_BEXTRACT((value),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR_SET(value,field)     (GAP_BINSERT((value),(field),8,16))
        #define QUIDDIKEY_HW_VER_HW_VER_MAJOR(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int hw_ver_rev      :8 ; // Hardware version, revision part
            unsigned int hw_ver_minor    :8 ; // Hardware version, minor part
            unsigned int hw_ver_major    :8 ; // Hardware version, major part
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ver_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ver : public vp::reg_32
        {
        public:
            inline void hw_ver_rev_set(uint32_t value);
            inline uint32_t hw_ver_rev_get();
            inline void hw_ver_minor_set(uint32_t value);
            inline uint32_t hw_ver_minor_get();
            inline void hw_ver_major_set(uint32_t value);
            inline uint32_t hw_ver_major_get();
        };

|
