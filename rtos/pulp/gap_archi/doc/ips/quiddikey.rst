Input file: fe/ips/quiddikey/doc/QUIDDIKEY_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +---------------------------------------+------+-----+---------------------------------------------------+
    |                 Name                  |Offset|Width|                    Description                    |
    +=======================================+======+=====+===================================================+
    |:ref:`CR<QUIDDIKEY_CR>`                |     0|   32|QUIDDIKEY Control register                         |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`SR<QUIDDIKEY_SR>`                |     8|   32|QUIDDIKEY Status register                          |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`AR<QUIDDIKEY_AR>`                |    12|   32|QUIDDIKEY Allow register                           |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`IER<QUIDDIKEY_IER>`              |    16|   32|QUIDDIKEY Interrupt Enable register                |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`IMR<QUIDDIKEY_IMR>`              |    20|   32|QUIDDIKEY Interrupt Mask register                  |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`ISR<QUIDDIKEY_ISR>`              |    24|   32|QUIDDIKEY Interrupt Status register                |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`KEY_DEST<QUIDDIKEY_KEY_DEST>`    |    32|   32|QUIDDIKEY Key Destination register                 |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`DIR<QUIDDIKEY_DIR>`              |   160|   32|QUIDDIKEY Data Input register                      |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`DOR<QUIDDIKEY_DOR>`              |   168|   32|QUIDDIKEY Data Output register                     |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`MISC<QUIDDIKEY_MISC>`            |   192|   32|QUIDDIKEY Miscellaneous register                   |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`IF_SR<QUIDDIKEY_IF_SR>`          |   208|   32|QUIDDIKEY Interface Status register                |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`TEST<QUIDDIKEY_TEST>`            |   216|   32|QUIDDIKEY Test register                            |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_RUC0<QUIDDIKEY_HW_RUC0>`      |   224|   32|QUIDDIKEY Hardware Restrict User Context 0 register|
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_RUC1<QUIDDIKEY_HW_RUC1>`      |   228|   32|QUIDDIKEY Hardware Restrict User Context 1 register|
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_SETTING<QUIDDIKEY_HW_SETTING>`|   240|   32|QUIDDIKEY Hardware Setting register                |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_INFO<QUIDDIKEY_HW_INFO>`      |   244|   32|QUIDDIKEY Hardware information register            |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_ID<QUIDDIKEY_HW_ID>`          |   248|   32|QUIDDIKEY Hardware Identifier register             |
    +---------------------------------------+------+-----+---------------------------------------------------+
    |:ref:`HW_VER<QUIDDIKEY_HW_VER>`        |   252|   32|QUIDDIKEY Hardware Version register                |
    +---------------------------------------+------+-----+---------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Control register
                #define QUIDDIKEY_CR_OFFSET                      0x0
        
                // QUIDDIKEY Status register
                #define QUIDDIKEY_SR_OFFSET                      0x8
        
                // QUIDDIKEY Allow register
                #define QUIDDIKEY_AR_OFFSET                      0xc
        
                // QUIDDIKEY Interrupt Enable register
                #define QUIDDIKEY_IER_OFFSET                     0x10
        
                // QUIDDIKEY Interrupt Mask register
                #define QUIDDIKEY_IMR_OFFSET                     0x14
        
                // QUIDDIKEY Interrupt Status register
                #define QUIDDIKEY_ISR_OFFSET                     0x18
        
                // QUIDDIKEY Key Destination register
                #define QUIDDIKEY_KEY_DEST_OFFSET                0x20
        
                // QUIDDIKEY Data Input register
                #define QUIDDIKEY_DIR_OFFSET                     0xa0
        
                // QUIDDIKEY Data Output register
                #define QUIDDIKEY_DOR_OFFSET                     0xa8
        
                // QUIDDIKEY Miscellaneous register
                #define QUIDDIKEY_MISC_OFFSET                    0xc0
        
                // QUIDDIKEY Interface Status register
                #define QUIDDIKEY_IF_SR_OFFSET                   0xd0
        
                // QUIDDIKEY Test register
                #define QUIDDIKEY_TEST_OFFSET                    0xd8
        
                // QUIDDIKEY Hardware Restrict User Context 0 register
                #define QUIDDIKEY_HW_RUC0_OFFSET                 0xe0
        
                // QUIDDIKEY Hardware Restrict User Context 1 register
                #define QUIDDIKEY_HW_RUC1_OFFSET                 0xe4
        
                // QUIDDIKEY Hardware Setting register
                #define QUIDDIKEY_HW_SETTING_OFFSET              0xf0
        
                // QUIDDIKEY Hardware information register
                #define QUIDDIKEY_HW_INFO_OFFSET                 0xf4
        
                // QUIDDIKEY Hardware Identifier register
                #define QUIDDIKEY_HW_ID_OFFSET                   0xf8
        
                // QUIDDIKEY Hardware Version register
                #define QUIDDIKEY_HW_VER_OFFSET                  0xfc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_cr_get(uint32_t base);
        static inline void quiddikey_cr_set(uint32_t base, uint32_t value);

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

        static inline uint32_t quiddikey_key_dest_get(uint32_t base);
        static inline void quiddikey_key_dest_set(uint32_t base, uint32_t value);

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

        static inline uint32_t quiddikey_hw_ruc0_get(uint32_t base);
        static inline void quiddikey_hw_ruc0_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_ruc1_get(uint32_t base);
        static inline void quiddikey_hw_ruc1_set(uint32_t base, uint32_t value);

        static inline uint32_t quiddikey_hw_setting_get(uint32_t base);
        static inline void quiddikey_hw_setting_set(uint32_t base, uint32_t value);

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
        #define QUIDDIKEY_CR_QK_ZEROIZE_BIT                                  0
        #define QUIDDIKEY_CR_QK_ZEROIZE_WIDTH                                1
        #define QUIDDIKEY_CR_QK_ZEROIZE_MASK                                 0x1
        #define QUIDDIKEY_CR_QK_ZEROIZE_RESET                                0x0
        
        // Begin Enroll operation (access: R/W)
        #define QUIDDIKEY_CR_QK_ENROLL_BIT                                   1
        #define QUIDDIKEY_CR_QK_ENROLL_WIDTH                                 1
        #define QUIDDIKEY_CR_QK_ENROLL_MASK                                  0x2
        #define QUIDDIKEY_CR_QK_ENROLL_RESET                                 0x0
        
        // Begin Start operation (access: R/W)
        #define QUIDDIKEY_CR_QK_START_BIT                                    2
        #define QUIDDIKEY_CR_QK_START_WIDTH                                  1
        #define QUIDDIKEY_CR_QK_START_MASK                                   0x4
        #define QUIDDIKEY_CR_QK_START_RESET                                  0x0
        
        // Begin Stop operation (access: R/W)
        #define QUIDDIKEY_CR_QK_STOP_BIT                                     5
        #define QUIDDIKEY_CR_QK_STOP_WIDTH                                   1
        #define QUIDDIKEY_CR_QK_STOP_MASK                                    0x20
        #define QUIDDIKEY_CR_QK_STOP_RESET                                   0x0
        
        // Begin Get Key operation (access: R/W)
        #define QUIDDIKEY_CR_QK_GET_KEY_BIT                                  6
        #define QUIDDIKEY_CR_QK_GET_KEY_WIDTH                                1
        #define QUIDDIKEY_CR_QK_GET_KEY_MASK                                 0x40
        #define QUIDDIKEY_CR_QK_GET_KEY_RESET                                0x0
        
        // Begin Unwrap operation (access: R/W)
        #define QUIDDIKEY_CR_QK_UNWRAP_BIT                                   7
        #define QUIDDIKEY_CR_QK_UNWRAP_WIDTH                                 1
        #define QUIDDIKEY_CR_QK_UNWRAP_MASK                                  0x80
        #define QUIDDIKEY_CR_QK_UNWRAP_RESET                                 0x0
        
        // Begin Wrap Generated Random operation (access: R/W)
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_BIT                    8
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_WIDTH                  1
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_MASK                   0x100
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_RESET                  0x0
        
        // Begin Wrap operation (access: R/W)
        #define QUIDDIKEY_CR_QK_WRAP_BIT                                     9
        #define QUIDDIKEY_CR_QK_WRAP_WIDTH                                   1
        #define QUIDDIKEY_CR_QK_WRAP_MASK                                    0x200
        #define QUIDDIKEY_CR_QK_WRAP_RESET                                   0x0
        
        // Begin Generate Random operation (access: R/W)
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_BIT                          15
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_WIDTH                        1
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_MASK                         0x8000
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_RESET                        0x0
        
        // Operation is in progress (access: R/W)
        #define QUIDDIKEY_SR_QK_BUSY_BIT                                     0
        #define QUIDDIKEY_SR_QK_BUSY_WIDTH                                   1
        #define QUIDDIKEY_SR_QK_BUSY_MASK                                    0x1
        #define QUIDDIKEY_SR_QK_BUSY_RESET                                   0x1
        
        // Last operation was successful (access: R/W)
        #define QUIDDIKEY_SR_QK_OK_BIT                                       1
        #define QUIDDIKEY_SR_QK_OK_WIDTH                                     1
        #define QUIDDIKEY_SR_QK_OK_MASK                                      0x2
        #define QUIDDIKEY_SR_QK_OK_RESET                                     0x0
        
        // Last operation failed (access: R/W)
        #define QUIDDIKEY_SR_QK_ERROR_BIT                                    2
        #define QUIDDIKEY_SR_QK_ERROR_WIDTH                                  1
        #define QUIDDIKEY_SR_QK_ERROR_MASK                                   0x4
        #define QUIDDIKEY_SR_QK_ERROR_RESET                                  0x0
        
        // QuiddiKey is in Zeroized or Locked state, For details see section 3.1 (access: R/W)
        #define QUIDDIKEY_SR_QK_ZEROIZED_BIT                                 3
        #define QUIDDIKEY_SR_QK_ZEROIZED_WIDTH                               1
        #define QUIDDIKEY_SR_QK_ZEROIZED_MASK                                0x8
        #define QUIDDIKEY_SR_QK_ZEROIZED_RESET                               0x0
        
        // Read: Last command was rejected Write 1: Clears this bit (access: R/W)
        #define QUIDDIKEY_SR_QK_REJECTED_BIT                                 4
        #define QUIDDIKEY_SR_QK_REJECTED_WIDTH                               1
        #define QUIDDIKEY_SR_QK_REJECTED_MASK                                0x10
        #define QUIDDIKEY_SR_QK_REJECTED_RESET                               0x0
        
        // Request for data in transfer via the QK_DIR register (access: R/W)
        #define QUIDDIKEY_SR_QK_DI_REQUEST_BIT                               5
        #define QUIDDIKEY_SR_QK_DI_REQUEST_WIDTH                             1
        #define QUIDDIKEY_SR_QK_DI_REQUEST_MASK                              0x20
        #define QUIDDIKEY_SR_QK_DI_REQUEST_RESET                             0x0
        
        // Request for data out transfer via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_SR_QK_DO_REQUEST_BIT                               6
        #define QUIDDIKEY_SR_QK_DO_REQUEST_WIDTH                             1
        #define QUIDDIKEY_SR_QK_DO_REQUEST_MASK                              0x40
        #define QUIDDIKEY_SR_QK_DO_REQUEST_RESET                             0x0
        
        // Enroll operation is allowed (access: R/W)
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_BIT                             1
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_WIDTH                           1
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_MASK                            0x2
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_RESET                           0x0
        
        // Start operation is allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_START_BIT                              2
        #define QUIDDIKEY_AR_QK_ALLOW_START_WIDTH                            1
        #define QUIDDIKEY_AR_QK_ALLOW_START_MASK                             0x4
        #define QUIDDIKEY_AR_QK_ALLOW_START_RESET                            0x0
        
        // Stop operation is allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_BIT                               5
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_WIDTH                             1
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_MASK                              0x20
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_RESET                             0x0
        
        // Get Key operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_BIT                            6
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_WIDTH                          1
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_MASK                           0x40
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_RESET                          0x0
        
        // Unwrap operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_BIT                             7
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_WIDTH                           1
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_MASK                            0x80
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_RESET                           0x0
        
        // Wrap Generated Random operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_BIT              8
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_WIDTH            1
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_MASK             0x100
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_RESET            0x0
        
        // Wrap operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_BIT                               9
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_WIDTH                             1
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_MASK                              0x200
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_RESET                             0x0
        
        // Generate Random operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_BIT                    15
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_WIDTH                  1
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_MASK                   0x8000
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_RESET                  0x0
        
        // 0: Disables all QuiddiKey interrupts 1: Enables all QuiddiKey interrupts that are enabled in the Interrupt Mask register  (access: R/W)
        #define QUIDDIKEY_IER_QK_INT_EN_BIT                                  0
        #define QUIDDIKEY_IER_QK_INT_EN_WIDTH                                1
        #define QUIDDIKEY_IER_QK_INT_EN_MASK                                 0x1
        #define QUIDDIKEY_IER_QK_INT_EN_RESET                                0x0
        
        // Enables the busy interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_BIT                             0
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_WIDTH                           1
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_MASK                            0x1
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_RESET                           0x0
        
        // Enables the ok interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_BIT                               1
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_WIDTH                             1
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_MASK                              0x2
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_RESET                             0x0
        
        // Enables the error interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_BIT                            2
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_WIDTH                          1
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_MASK                           0x4
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_RESET                          0x0
        
        // Enables the zeroized interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_BIT                         3
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_WIDTH                       1
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_MASK                        0x8
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_RESET                       0x0
        
        // Enables the rejected interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_BIT                         4
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_WIDTH                       1
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_MASK                        0x10
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_RESET                       0x0
        
        // Enables the data in request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_BIT                       5
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_WIDTH                     1
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_MASK                      0x20
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_RESET                     0x0
        
        // Enables the data out request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_BIT                       6
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_WIDTH                     1
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_MASK                      0x40
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_RESET                     0x0
        
        // A negative edge has occurred on qk_busy, which means that an operation has completed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_BUSY_BIT                                0
        #define QUIDDIKEY_ISR_QK_INT_BUSY_WIDTH                              1
        #define QUIDDIKEY_ISR_QK_INT_BUSY_MASK                               0x1
        #define QUIDDIKEY_ISR_QK_INT_BUSY_RESET                              0x0
        
        // A positive edge has occurred on qk_ok, which means that an operation successfully completed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_OK_BIT                                  1
        #define QUIDDIKEY_ISR_QK_INT_OK_WIDTH                                1
        #define QUIDDIKEY_ISR_QK_INT_OK_MASK                                 0x2
        #define QUIDDIKEY_ISR_QK_INT_OK_RESET                                0x0
        
        // A positive edge has occurred on qk_error, which means that an operation has failed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_ERROR_BIT                               2
        #define QUIDDIKEY_ISR_QK_INT_ERROR_WIDTH                             1
        #define QUIDDIKEY_ISR_QK_INT_ERROR_MASK                              0x4
        #define QUIDDIKEY_ISR_QK_INT_ERROR_RESET                             0x0
        
        // A positive edge has occurred on qk_zeroized, which means that QuiddiKey has moved to the Zeroized or Locked state For details see section 3.1 (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_BIT                            3
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_WIDTH                          1
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_MASK                           0x8
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_RESET                          0x0
        
        // A positive edge has occurred on qk_rejected, which means that a command was rejected (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_BIT                            4
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_WIDTH                          1
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_MASK                           0x10
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_RESET                          0x0
        
        // A positive edge has occurred on qk_di_request, which means that a data in transfer is requested via the QK_DIR register (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_BIT                          5
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_WIDTH                        1
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_MASK                         0x20
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_RESET                        0x0
        
        // A positive edge has occurred on qk_do_request, which means that a data out transfer is requested via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_BIT                          6
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_WIDTH                        1
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_MASK                         0x40
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_RESET                        0x0
        
        // Key will be made available via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_BIT                           0
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_WIDTH                         1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_MASK                          0x1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_RESET                         0x0
        
        // Key will be made available via the KO interface (access: R/W)
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_BIT                            1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_WIDTH                          1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_MASK                           0x2
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_RESET                          0x0
        
        // Input data to QuiddiKey Note: This field must only be written when qk_di_request = 1 (access: W)
        #define QUIDDIKEY_DIR_QK_DI_BIT                                      0
        #define QUIDDIKEY_DIR_QK_DI_WIDTH                                    32
        #define QUIDDIKEY_DIR_QK_DI_MASK                                     0xffffffff
        #define QUIDDIKEY_DIR_QK_DI_RESET                                    0x0
        
        // Output data from QuiddiKey Note: This field must only be read when qk_do_request = 1 (access: W)
        #define QUIDDIKEY_DOR_QK_DO_BIT                                      0
        #define QUIDDIKEY_DOR_QK_DO_WIDTH                                    32
        #define QUIDDIKEY_DOR_QK_DO_MASK                                     0xffffffff
        #define QUIDDIKEY_DOR_QK_DO_RESET                                    0x0
        
        //  Defines the endianness of keys and data in system memory: 0: Little endian 1: Big endian (default) See section 5.1.4 for details (access: R/W)
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_BIT                        0
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_WIDTH                      1
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_MASK                       0x1
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_RESET                      0x0
        
        //  An APB error has occurred, for details see section 5.1.2 (access: R)
        #define QUIDDIKEY_IF_SR_APB_ERROR_BIT                                0
        #define QUIDDIKEY_IF_SR_APB_ERROR_WIDTH                              1
        #define QUIDDIKEY_IF_SR_APB_ERROR_MASK                               0x1
        #define QUIDDIKEY_IF_SR_APB_ERROR_RESET                              0x0
        
        // Isolates QuiddiKey and runs BIST (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_BIT                            0
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_WIDTH                          1
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_MASK                           0x1
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_RESET                          0x0
        
        // BIST is in progress or finishing up (after qk_bist_enable = 0) (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_BIT                           4
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_WIDTH                         1
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_MASK                          0x10
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_RESET                         0x0
        
        // BIST is in progress (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_BIT                            5
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_WIDTH                          1
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_MASK                           0x20
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_RESET                          0x0
        
        // BIST has passed (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_OK_BIT                                6
        #define QUIDDIKEY_TEST_QK_BIST_OK_WIDTH                              1
        #define QUIDDIKEY_TEST_QK_BIST_OK_MASK                               0x40
        #define QUIDDIKEY_TEST_QK_BIST_OK_RESET                              0x0
        
        // BIST has failed (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_BIT                             7
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_WIDTH                           1
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_MASK                            0x80
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_RESET                           0x0
        
        // BIST is allowed (access: R/W)
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_BIT                             31
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_WIDTH                           1
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_MASK                            0x80000000
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_RESET                           0x0
        
        // Hardware Restrict UC0 register (access: R/W)
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_BIT             0
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_WIDTH           32
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_MASK            0xffffffff
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_RESET           0x0
        
        // Hardware Restrict UC1 register (access: R/W)
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_BIT             0
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_WIDTH           32
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_MASK            0xffffffff
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_RESET           0x0
        
        // 0:Enroll operations are enabled 1:Enroll operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_BIT                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_WIDTH                 1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_MASK                  0x2
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_RESET                 0x0
        
        // 0:Start operations are disabled 1:Start operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_BIT                    2
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_WIDTH                  1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_MASK                   0x4
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_RESET                  0x0
        
        // 0:Stop operations are enabled 1:Stop operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_BIT                     5
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_MASK                    0x20
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_RESET                   0x0
        
        // 0:Get Key operations are enabled 1:Get Key operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_BIT                  6
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_WIDTH                1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_MASK                 0x40
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_RESET                0x0
        
        // 0:Unwrap operations are enabled 1:Unwrap operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_BIT                   7
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_WIDTH                 1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_MASK                  0x80
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_RESET                 0x0
        
        // 0:Wrap Generated Random operations are enabled 1:Wrap Generated Random operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_BIT    8
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_WIDTH  1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_MASK   0x100
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_RESET  0x0
        
        // 0:Wrap operations are enabled 1:Wrap operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_BIT                     9
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_MASK                    0x200
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_RESET                   0x0
        
        // 0:Generate Random operations are enabled 1:Generate Random operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_BIT          15
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_WIDTH        1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_MASK         0x8000
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_RESET        0x0
        
        // QuiddiKey configuration: 0001: Safe 0010: Plus Other values: Reserved (access: R)
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_BIT                         28
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_WIDTH                       4
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_MASK                        0xf0000000
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_RESET                       0x10
        
        // 1: BIST is included 0: BIST is not included (access: R)
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_BIT                         22
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_WIDTH                       1
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_MASK                        0x400000
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_RESET                       0x1
        
        // Hardware Identifier (access: R)
        #define QUIDDIKEY_HW_ID_QK_HW_ID_BIT                                 0
        #define QUIDDIKEY_HW_ID_QK_HW_ID_WIDTH                               32
        #define QUIDDIKEY_HW_ID_QK_HW_ID_MASK                                0xffffffff
        #define QUIDDIKEY_HW_ID_QK_HW_ID_RESET                               0x0
        
        // Hardware version, major part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_BIT                     0
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_WIDTH                   8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_MASK                    0xff
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_RESET                   0x0
        
        // Hardware version, minor part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_BIT                     8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_WIDTH                   8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_MASK                    0xff00
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_RESET                   0x0
        
        // Hardware version, patch part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_REV_BIT                               16
        #define QUIDDIKEY_HW_VER_QK_HW_REV_WIDTH                             8
        #define QUIDDIKEY_HW_VER_QK_HW_REV_MASK                              0xff0000
        #define QUIDDIKEY_HW_VER_QK_HW_REV_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_CR_QK_ZEROIZE_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE(val)                       ((val) << 0)
        
        #define QUIDDIKEY_CR_QK_ENROLL_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL(val)                        ((val) << 1)
        
        #define QUIDDIKEY_CR_QK_START_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_CR_QK_START_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_CR_QK_START_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_CR_QK_START(val)                         ((val) << 2)
        
        #define QUIDDIKEY_CR_QK_STOP_GET(value)                    (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_CR_QK_STOP_GETS(value)                   (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_CR_QK_STOP_SET(value,field)              (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_CR_QK_STOP(val)                          ((val) << 5)
        
        #define QUIDDIKEY_CR_QK_GET_KEY_GET(value)                 (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY_GETS(value)                (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY_SET(value,field)           (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY(val)                       ((val) << 6)
        
        #define QUIDDIKEY_CR_QK_UNWRAP_GET(value)                  (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP_GETS(value)                 (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP_SET(value,field)            (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP(val)                        ((val) << 7)
        
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_GET(value)   (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_GETS(value)  (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM(val)         ((val) << 8)
        
        #define QUIDDIKEY_CR_QK_WRAP_GET(value)                    (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_CR_QK_WRAP_GETS(value)                   (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_CR_QK_WRAP_SET(value,field)              (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_CR_QK_WRAP(val)                          ((val) << 9)
        
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_GET(value)         (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_GETS(value)        (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_SET(value,field)   (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM(val)               ((val) << 15)
        
        #define QUIDDIKEY_SR_QK_BUSY_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_SR_QK_BUSY_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_SR_QK_BUSY_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_SR_QK_BUSY(val)                          ((val) << 0)
        
        #define QUIDDIKEY_SR_QK_OK_GET(value)                      (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_SR_QK_OK_GETS(value)                     (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_SR_QK_OK_SET(value,field)                (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_SR_QK_OK(val)                            ((val) << 1)
        
        #define QUIDDIKEY_SR_QK_ERROR_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_SR_QK_ERROR_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_SR_QK_ERROR_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_SR_QK_ERROR(val)                         ((val) << 2)
        
        #define QUIDDIKEY_SR_QK_ZEROIZED_GET(value)                (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED_GETS(value)               (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED_SET(value,field)          (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED(val)                      ((val) << 3)
        
        #define QUIDDIKEY_SR_QK_REJECTED_GET(value)                (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED_GETS(value)               (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED_SET(value,field)          (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED(val)                      ((val) << 4)
        
        #define QUIDDIKEY_SR_QK_DI_REQUEST_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST(val)                    ((val) << 5)
        
        #define QUIDDIKEY_SR_QK_DO_REQUEST_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST(val)                    ((val) << 6)
        
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL(val)                  ((val) << 1)
        
        #define QUIDDIKEY_AR_QK_ALLOW_START_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START(val)                   ((val) << 2)
        
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP(val)                    ((val) << 5)
        
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_GET(value)           (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_GETS(value)          (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_SET(value,field)     (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY(val)                 ((val) << 6)
        
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_GET(value)            (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_GETS(value)           (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_SET(value,field)      (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP(val)                  ((val) << 7)
        
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM(val)   ((val) << 8)
        
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GET(value)              (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GETS(value)             (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_SET(value,field)        (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP(val)                    ((val) << 9)
        
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_GET(value)   (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_GETS(value)  (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM(val)         ((val) << 15)
        
        #define QUIDDIKEY_IER_QK_INT_EN_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN(val)                       ((val) << 0)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY(val)                  ((val) << 0)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK(val)                    ((val) << 1)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR(val)                 ((val) << 2)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_GET(value)        (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_GETS(value)       (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_SET(value,field)  (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED(val)              ((val) << 3)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED(val)              ((val) << 4)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST(val)            ((val) << 5)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST(val)            ((val) << 6)
        
        #define QUIDDIKEY_ISR_QK_INT_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY(val)                     ((val) << 0)
        
        #define QUIDDIKEY_ISR_QK_INT_OK_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK(val)                       ((val) << 1)
        
        #define QUIDDIKEY_ISR_QK_INT_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR(val)                    ((val) << 2)
        
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_GET(value)           (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_GETS(value)          (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_SET(value,field)     (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED(val)                 ((val) << 3)
        
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_GET(value)           (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_GETS(value)          (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_SET(value,field)     (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED(val)                 ((val) << 4)
        
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST(val)               ((val) << 5)
        
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST(val)               ((val) << 6)
        
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG(val)                ((val) << 0)
        
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_GET(value)           (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_GETS(value)          (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_SET(value,field)     (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO(val)                 ((val) << 1)
        
        #define QUIDDIKEY_DIR_QK_DI_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DIR_QK_DI_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DIR_QK_DI_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DIR_QK_DI(val)                           ((val) << 0)
        
        #define QUIDDIKEY_DOR_QK_DO_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DOR_QK_DO_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DOR_QK_DO_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DOR_QK_DO(val)                           ((val) << 0)
        
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_GET(value)       (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_GETS(value)      (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS(val)             ((val) << 0)
        
        #define QUIDDIKEY_IF_SR_APB_ERROR_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IF_SR_APB_ERROR(val)                     ((val) << 0)
        
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE(val)                 ((val) << 0)
        
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_GET(value)          (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_GETS(value)         (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_SET(value,field)    (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING(val)                ((val) << 4)
        
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_GET(value)           (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_GETS(value)          (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_SET(value,field)     (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE(val)                 ((val) << 5)
        
        #define QUIDDIKEY_TEST_QK_BIST_OK_GET(value)               (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK_GETS(value)              (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK_SET(value,field)         (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK(val)                     ((val) << 6)
        
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_GET(value)            (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_GETS(value)           (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_SET(value,field)      (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR(val)                  ((val) << 7)
        
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_GET(value)            (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_GETS(value)           (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_SET(value,field)      (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST(val)                  ((val) << 31)
        
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0(val)  ((val) << 0)
        
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1(val)  ((val) << 0)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_GET(value)  (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL(val)        ((val) << 1)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_GET(value)   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_GETS(value)  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START(val)         ((val) << 2)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_GET(value)    (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_GETS(value)   (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP(val)          ((val) << 5)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY(val)       ((val) << 6)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_GET(value)  (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP(val)        ((val) << 7)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM(val) ((val) << 8)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GET(value)    (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GETS(value)   (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP(val)          ((val) << 9)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM(val) ((val) << 15)
        
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_GET(value)        (GAP_BEXTRACTU((value),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_GETS(value)       (GAP_BEXTRACT((value),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_SET(value,field)  (GAP_BINSERT((value),(field),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE(val)              ((val) << 28)
        
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_GET(value)        (GAP_BEXTRACTU((value),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_GETS(value)       (GAP_BEXTRACT((value),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_SET(value,field)  (GAP_BINSERT((value),(field),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST(val)              ((val) << 22)
        
        #define QUIDDIKEY_HW_ID_QK_HW_ID_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID(val)                      ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR(val)          ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_GET(value)    (GAP_BEXTRACTU((value),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_GETS(value)   (GAP_BEXTRACT((value),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR(val)          ((val) << 8)
        
        #define QUIDDIKEY_HW_VER_QK_HW_REV_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV(val)                    ((val) << 16)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** QUIDDIKEY_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cr;  // QUIDDIKEY Control register
            volatile uint32_t reserved_0[1];  // Reserved/Not used.
            volatile uint32_t sr;  // QUIDDIKEY Status register
            volatile uint32_t ar;  // QUIDDIKEY Allow register
            volatile uint32_t ier;  // QUIDDIKEY Interrupt Enable register
            volatile uint32_t imr;  // QUIDDIKEY Interrupt Mask register
            volatile uint32_t isr;  // QUIDDIKEY Interrupt Status register
            volatile uint32_t reserved_1[1];  // Reserved/Not used.
            volatile uint32_t key_dest;  // QUIDDIKEY Key Destination register
            volatile uint32_t reserved_2[31];  // Reserved/Not used.
            volatile uint32_t dir;  // QUIDDIKEY Data Input register
            volatile uint32_t reserved_3[1];  // Reserved/Not used.
            volatile uint32_t dor;  // QUIDDIKEY Data Output register
            volatile uint32_t reserved_4[5];  // Reserved/Not used.
            volatile uint32_t misc;  // QUIDDIKEY Miscellaneous register
            volatile uint32_t reserved_5[3];  // Reserved/Not used.
            volatile uint32_t if_sr;  // QUIDDIKEY Interface Status register
            volatile uint32_t reserved_6[1];  // Reserved/Not used.
            volatile uint32_t test;  // QUIDDIKEY Test register
            volatile uint32_t reserved_7[1];  // Reserved/Not used.
            volatile uint32_t hw_ruc0;  // QUIDDIKEY Hardware Restrict User Context 0 register
            volatile uint32_t hw_ruc1;  // QUIDDIKEY Hardware Restrict User Context 1 register
            volatile uint32_t reserved_8[2];  // Reserved/Not used.
            volatile uint32_t hw_setting;  // QUIDDIKEY Hardware Setting register
            volatile uint32_t hw_info;  // QUIDDIKEY Hardware information register
            volatile uint32_t hw_id;  // QUIDDIKEY Hardware Identifier register
            volatile uint32_t hw_ver;  // QUIDDIKEY Hardware Version register
        } __attribute__((packed)) quiddikey_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_zeroize      :1 ; // Begin Zeroize operation
            unsigned int qk_enroll       :1 ; // Begin Enroll operation
            unsigned int qk_start        :1 ; // Begin Start operation
            unsigned int padding0:2 ;
            unsigned int qk_stop         :1 ; // Begin Stop operation
            unsigned int qk_get_key      :1 ; // Begin Get Key operation
            unsigned int qk_unwrap       :1 ; // Begin Unwrap operation
            unsigned int qk_wrap_generated_random:1 ; // Begin Wrap Generated Random operation
            unsigned int qk_wrap         :1 ; // Begin Wrap operation
            unsigned int padding1:5 ;
            unsigned int qk_generate_random:1 ; // Begin Generate Random operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_cr_t;
        
        typedef union {
          struct {
            unsigned int qk_busy         :1 ; // Operation is in progress
            unsigned int qk_ok           :1 ; // Last operation was successful
            unsigned int qk_error        :1 ; // Last operation failed
            unsigned int qk_zeroized     :1 ; // QuiddiKey is in Zeroized or Locked state, For details see section 3.1
            unsigned int qk_rejected     :1 ; // Read: Last command was rejected Write 1: Clears this bit
            unsigned int qk_di_request   :1 ; // Request for data in transfer via the QK_DIR register
            unsigned int qk_do_request   :1 ; // Request for data out transfer via the QK_DOR register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_sr_t;
        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int qk_allow_enroll :1 ; // Enroll operation is allowed
            unsigned int qk_allow_start  :1 ; // Start operation is allowed
            unsigned int padding1:2 ;
            unsigned int qk_allow_stop   :1 ; // Stop operation is allowed
            unsigned int qk_allow_get_key:1 ; // Get Key operations are allowed
            unsigned int qk_allow_unwrap :1 ; // Unwrap operations are allowed
            unsigned int qk_allow_wrap_generated_random:1 ; // Wrap Generated Random operations are allowed
            unsigned int qk_allow_wrap   :1 ; // Wrap operations are allowed
            unsigned int padding2:5 ;
            unsigned int qk_allow_generate_random:1 ; // Generate Random operations are allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ar_t;
        
        typedef union {
          struct {
            unsigned int qk_int_en       :1 ; // 0: Disables all QuiddiKey interrupts 1: Enables all QuiddiKey interrupts that are enabled in the Interrupt Mask register 
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ier_t;
        
        typedef union {
          struct {
            unsigned int qk_int_en_busy  :1 ; // Enables the busy interrupt
            unsigned int qk_int_en_ok    :1 ; // Enables the ok interrupt
            unsigned int qk_int_en_error :1 ; // Enables the error interrupt
            unsigned int qk_int_en_zeroized:1 ; // Enables the zeroized interrupt
            unsigned int qk_int_en_rejected:1 ; // Enables the rejected interrupt
            unsigned int qk_int_en_di_request:1 ; // Enables the data in request interrupt
            unsigned int qk_int_en_do_request:1 ; // Enables the data out request interrupt
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_imr_t;
        
        typedef union {
          struct {
            unsigned int qk_int_busy     :1 ; // A negative edge has occurred on qk_busy, which means that an operation has completed
            unsigned int qk_int_ok       :1 ; // A positive edge has occurred on qk_ok, which means that an operation successfully completed
            unsigned int qk_int_error    :1 ; // A positive edge has occurred on qk_error, which means that an operation has failed
            unsigned int qk_int_zeroized :1 ; // A positive edge has occurred on qk_zeroized, which means that QuiddiKey has moved to the Zeroized or Locked state For details see section 3.1
            unsigned int qk_int_rejected :1 ; // A positive edge has occurred on qk_rejected, which means that a command was rejected
            unsigned int qk_int_di_request:1 ; // A positive edge has occurred on qk_di_request, which means that a data in transfer is requested via the QK_DIR register
            unsigned int qk_int_do_request:1 ; // A positive edge has occurred on qk_do_request, which means that a data out transfer is requested via the QK_DOR register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_isr_t;
        
        typedef union {
          struct {
            unsigned int qk_dest_reg     :1 ; // Key will be made available via the QK_DOR register
            unsigned int qk_dest_ko      :1 ; // Key will be made available via the KO interface
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_key_dest_t;
        
        typedef union {
          struct {
            unsigned int qk_di           :32; // Input data to QuiddiKey Note: This field must only be written when qk_di_request = 1
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dir_t;
        
        typedef union {
          struct {
            unsigned int qk_do           :32; // Output data from QuiddiKey Note: This field must only be read when qk_do_request = 1
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dor_t;
        
        typedef union {
          struct {
            unsigned int qk_data_endianness:1 ; //  Defines the endianness of keys and data in system memory: 0: Little endian 1: Big endian (default) See section 5.1.4 for details
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_misc_t;
        
        typedef union {
          struct {
            unsigned int apb_error       :1 ; //  An APB error has occurred, for details see section 5.1.2
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_if_sr_t;
        
        typedef union {
          struct {
            unsigned int qk_bist_enable  :1 ; // Isolates QuiddiKey and runs BIST
            unsigned int padding0:3 ;
            unsigned int qk_bist_running :1 ; // BIST is in progress or finishing up (after qk_bist_enable = 0)
            unsigned int qk_bist_active  :1 ; // BIST is in progress
            unsigned int qk_bist_ok      :1 ; // BIST has passed
            unsigned int qk_bist_error   :1 ; // BIST has failed
            unsigned int padding1:23;
            unsigned int qk_allow_bist   :1 ; // BIST is allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_test_t;
        
        typedef union {
          struct {
            unsigned int qk_restrict_user_context_0:32; // Hardware Restrict UC0 register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc0_t;
        
        typedef union {
          struct {
            unsigned int qk_restrict_user_context_1:32; // Hardware Restrict UC1 register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc1_t;
        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int qk_disable_enroll:1 ; // 0:Enroll operations are enabled 1:Enroll operations are disabled
            unsigned int qk_disable_start:1 ; // 0:Start operations are disabled 1:Start operations are disabled
            unsigned int padding1:2 ;
            unsigned int qk_disable_stop :1 ; // 0:Stop operations are enabled 1:Stop operations are disabled
            unsigned int qk_disable_get_key:1 ; // 0:Get Key operations are enabled 1:Get Key operations are disabled
            unsigned int qk_disable_unwrap:1 ; // 0:Unwrap operations are enabled 1:Unwrap operations are disabled
            unsigned int qk_disable_wrap_generated_random:1 ; // 0:Wrap Generated Random operations are enabled 1:Wrap Generated Random operations are disabled
            unsigned int qk_disable_wrap :1 ; // 0:Wrap operations are enabled 1:Wrap operations are disabled
            unsigned int padding2:5 ;
            unsigned int qk_disable_generate_random:1 ; // 0:Generate Random operations are enabled 1:Generate Random operations are disabled
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_setting_t;
        
        typedef union {
          struct {
            unsigned int padding0:28;
            unsigned int qk_config_type  :4 ; // QuiddiKey configuration: 0001: Safe 0010: Plus Other values: Reserved
            unsigned int qk_config_bist  :1 ; // 1: BIST is included 0: BIST is not included
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_info_t;
        
        typedef union {
          struct {
            unsigned int qk_hw_id        :32; // Hardware Identifier
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_id_t;
        
        typedef union {
          struct {
            unsigned int qk_hw_version_major:8 ; // Hardware version, major part
            unsigned int qk_hw_version_minor:8 ; // Hardware version, minor part
            unsigned int qk_hw_rev       :8 ; // Hardware version, patch part
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
            vp_quiddikey_sr sr;
            vp_quiddikey_ar ar;
            vp_quiddikey_ier ier;
            vp_quiddikey_imr imr;
            vp_quiddikey_isr isr;
            vp_quiddikey_key_dest key_dest;
            vp_quiddikey_dir dir;
            vp_quiddikey_dor dor;
            vp_quiddikey_misc misc;
            vp_quiddikey_if_sr if_sr;
            vp_quiddikey_test test;
            vp_quiddikey_hw_ruc0 hw_ruc0;
            vp_quiddikey_hw_ruc1 hw_ruc1;
            vp_quiddikey_hw_setting hw_setting;
            vp_quiddikey_hw_info hw_info;
            vp_quiddikey_hw_id hw_id;
            vp_quiddikey_hw_ver hw_ver;
        };

|

.. _QUIDDIKEY_CR:

CR
""

QUIDDIKEY Control register

.. table:: 

    +-----+---+------------------------+-------------------------------------+
    |Bit #|R/W|          Name          |             Description             |
    +=====+===+========================+=====================================+
    |    0|R/W|qk_zeroize              |Begin Zeroize operation              |
    +-----+---+------------------------+-------------------------------------+
    |    1|R/W|qk_enroll               |Begin Enroll operation               |
    +-----+---+------------------------+-------------------------------------+
    |    2|R/W|qk_start                |Begin Start operation                |
    +-----+---+------------------------+-------------------------------------+
    |    5|R/W|qk_stop                 |Begin Stop operation                 |
    +-----+---+------------------------+-------------------------------------+
    |    6|R/W|qk_get_key              |Begin Get Key operation              |
    +-----+---+------------------------+-------------------------------------+
    |    7|R/W|qk_unwrap               |Begin Unwrap operation               |
    +-----+---+------------------------+-------------------------------------+
    |    8|R/W|qk_wrap_generated_random|Begin Wrap Generated Random operation|
    +-----+---+------------------------+-------------------------------------+
    |    9|R/W|qk_wrap                 |Begin Wrap operation                 |
    +-----+---+------------------------+-------------------------------------+
    |   15|R/W|qk_generate_random      |Begin Generate Random operation      |
    +-----+---+------------------------+-------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Control register
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
        #define QUIDDIKEY_CR_QK_ZEROIZE_BIT                                  0
        #define QUIDDIKEY_CR_QK_ZEROIZE_WIDTH                                1
        #define QUIDDIKEY_CR_QK_ZEROIZE_MASK                                 0x1
        #define QUIDDIKEY_CR_QK_ZEROIZE_RESET                                0x0
        
        // Begin Enroll operation (access: R/W)
        #define QUIDDIKEY_CR_QK_ENROLL_BIT                                   1
        #define QUIDDIKEY_CR_QK_ENROLL_WIDTH                                 1
        #define QUIDDIKEY_CR_QK_ENROLL_MASK                                  0x2
        #define QUIDDIKEY_CR_QK_ENROLL_RESET                                 0x0
        
        // Begin Start operation (access: R/W)
        #define QUIDDIKEY_CR_QK_START_BIT                                    2
        #define QUIDDIKEY_CR_QK_START_WIDTH                                  1
        #define QUIDDIKEY_CR_QK_START_MASK                                   0x4
        #define QUIDDIKEY_CR_QK_START_RESET                                  0x0
        
        // Begin Stop operation (access: R/W)
        #define QUIDDIKEY_CR_QK_STOP_BIT                                     5
        #define QUIDDIKEY_CR_QK_STOP_WIDTH                                   1
        #define QUIDDIKEY_CR_QK_STOP_MASK                                    0x20
        #define QUIDDIKEY_CR_QK_STOP_RESET                                   0x0
        
        // Begin Get Key operation (access: R/W)
        #define QUIDDIKEY_CR_QK_GET_KEY_BIT                                  6
        #define QUIDDIKEY_CR_QK_GET_KEY_WIDTH                                1
        #define QUIDDIKEY_CR_QK_GET_KEY_MASK                                 0x40
        #define QUIDDIKEY_CR_QK_GET_KEY_RESET                                0x0
        
        // Begin Unwrap operation (access: R/W)
        #define QUIDDIKEY_CR_QK_UNWRAP_BIT                                   7
        #define QUIDDIKEY_CR_QK_UNWRAP_WIDTH                                 1
        #define QUIDDIKEY_CR_QK_UNWRAP_MASK                                  0x80
        #define QUIDDIKEY_CR_QK_UNWRAP_RESET                                 0x0
        
        // Begin Wrap Generated Random operation (access: R/W)
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_BIT                    8
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_WIDTH                  1
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_MASK                   0x100
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_RESET                  0x0
        
        // Begin Wrap operation (access: R/W)
        #define QUIDDIKEY_CR_QK_WRAP_BIT                                     9
        #define QUIDDIKEY_CR_QK_WRAP_WIDTH                                   1
        #define QUIDDIKEY_CR_QK_WRAP_MASK                                    0x200
        #define QUIDDIKEY_CR_QK_WRAP_RESET                                   0x0
        
        // Begin Generate Random operation (access: R/W)
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_BIT                          15
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_WIDTH                        1
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_MASK                         0x8000
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_CR_QK_ZEROIZE_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_CR_QK_ZEROIZE(val)                       ((val) << 0)
        
        #define QUIDDIKEY_CR_QK_ENROLL_GET(value)                  (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL_GETS(value)                 (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL_SET(value,field)            (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_CR_QK_ENROLL(val)                        ((val) << 1)
        
        #define QUIDDIKEY_CR_QK_START_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_CR_QK_START_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_CR_QK_START_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_CR_QK_START(val)                         ((val) << 2)
        
        #define QUIDDIKEY_CR_QK_STOP_GET(value)                    (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_CR_QK_STOP_GETS(value)                   (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_CR_QK_STOP_SET(value,field)              (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_CR_QK_STOP(val)                          ((val) << 5)
        
        #define QUIDDIKEY_CR_QK_GET_KEY_GET(value)                 (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY_GETS(value)                (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY_SET(value,field)           (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_CR_QK_GET_KEY(val)                       ((val) << 6)
        
        #define QUIDDIKEY_CR_QK_UNWRAP_GET(value)                  (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP_GETS(value)                 (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP_SET(value,field)            (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_CR_QK_UNWRAP(val)                        ((val) << 7)
        
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_GET(value)   (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_GETS(value)  (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_CR_QK_WRAP_GENERATED_RANDOM(val)         ((val) << 8)
        
        #define QUIDDIKEY_CR_QK_WRAP_GET(value)                    (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_CR_QK_WRAP_GETS(value)                   (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_CR_QK_WRAP_SET(value,field)              (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_CR_QK_WRAP(val)                          ((val) << 9)
        
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_GET(value)         (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_GETS(value)        (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM_SET(value,field)   (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_CR_QK_GENERATE_RANDOM(val)               ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_zeroize      :1 ; // Begin Zeroize operation
            unsigned int qk_enroll       :1 ; // Begin Enroll operation
            unsigned int qk_start        :1 ; // Begin Start operation
            unsigned int padding0:2 ;
            unsigned int qk_stop         :1 ; // Begin Stop operation
            unsigned int qk_get_key      :1 ; // Begin Get Key operation
            unsigned int qk_unwrap       :1 ; // Begin Unwrap operation
            unsigned int qk_wrap_generated_random:1 ; // Begin Wrap Generated Random operation
            unsigned int qk_wrap         :1 ; // Begin Wrap operation
            unsigned int padding1:5 ;
            unsigned int qk_generate_random:1 ; // Begin Generate Random operation
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_cr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_cr : public vp::reg_32
        {
        public:
            inline void qk_zeroize_set(uint32_t value);
            inline uint32_t qk_zeroize_get();
            inline void qk_enroll_set(uint32_t value);
            inline uint32_t qk_enroll_get();
            inline void qk_start_set(uint32_t value);
            inline uint32_t qk_start_get();
            inline void qk_stop_set(uint32_t value);
            inline uint32_t qk_stop_get();
            inline void qk_get_key_set(uint32_t value);
            inline uint32_t qk_get_key_get();
            inline void qk_unwrap_set(uint32_t value);
            inline uint32_t qk_unwrap_get();
            inline void qk_wrap_generated_random_set(uint32_t value);
            inline uint32_t qk_wrap_generated_random_get();
            inline void qk_wrap_set(uint32_t value);
            inline uint32_t qk_wrap_get();
            inline void qk_generate_random_set(uint32_t value);
            inline uint32_t qk_generate_random_get();
        };

|

.. _QUIDDIKEY_SR:

SR
""

QUIDDIKEY Status register

.. table:: 

    +-----+---+-------------+---------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                             Description                             |
    +=====+===+=============+=====================================================================+
    |    0|R/W|qk_busy      |Operation is in progress                                             |
    +-----+---+-------------+---------------------------------------------------------------------+
    |    1|R/W|qk_ok        |Last operation was successful                                        |
    +-----+---+-------------+---------------------------------------------------------------------+
    |    2|R/W|qk_error     |Last operation failed                                                |
    +-----+---+-------------+---------------------------------------------------------------------+
    |    3|R/W|qk_zeroized  |QuiddiKey is in Zeroized or Locked state, For details see section 3.1|
    +-----+---+-------------+---------------------------------------------------------------------+
    |    4|R/W|qk_rejected  |Read: Last command was rejected Write 1: Clears this bit             |
    +-----+---+-------------+---------------------------------------------------------------------+
    |    5|R/W|qk_di_request|Request for data in transfer via the QK_DIR register                 |
    +-----+---+-------------+---------------------------------------------------------------------+
    |    6|R/W|qk_do_request|Request for data out transfer via the QK_DOR register                |
    +-----+---+-------------+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Status register
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
        #define QUIDDIKEY_SR_QK_BUSY_BIT                                     0
        #define QUIDDIKEY_SR_QK_BUSY_WIDTH                                   1
        #define QUIDDIKEY_SR_QK_BUSY_MASK                                    0x1
        #define QUIDDIKEY_SR_QK_BUSY_RESET                                   0x1
        
        // Last operation was successful (access: R/W)
        #define QUIDDIKEY_SR_QK_OK_BIT                                       1
        #define QUIDDIKEY_SR_QK_OK_WIDTH                                     1
        #define QUIDDIKEY_SR_QK_OK_MASK                                      0x2
        #define QUIDDIKEY_SR_QK_OK_RESET                                     0x0
        
        // Last operation failed (access: R/W)
        #define QUIDDIKEY_SR_QK_ERROR_BIT                                    2
        #define QUIDDIKEY_SR_QK_ERROR_WIDTH                                  1
        #define QUIDDIKEY_SR_QK_ERROR_MASK                                   0x4
        #define QUIDDIKEY_SR_QK_ERROR_RESET                                  0x0
        
        // QuiddiKey is in Zeroized or Locked state, For details see section 3.1 (access: R/W)
        #define QUIDDIKEY_SR_QK_ZEROIZED_BIT                                 3
        #define QUIDDIKEY_SR_QK_ZEROIZED_WIDTH                               1
        #define QUIDDIKEY_SR_QK_ZEROIZED_MASK                                0x8
        #define QUIDDIKEY_SR_QK_ZEROIZED_RESET                               0x0
        
        // Read: Last command was rejected Write 1: Clears this bit (access: R/W)
        #define QUIDDIKEY_SR_QK_REJECTED_BIT                                 4
        #define QUIDDIKEY_SR_QK_REJECTED_WIDTH                               1
        #define QUIDDIKEY_SR_QK_REJECTED_MASK                                0x10
        #define QUIDDIKEY_SR_QK_REJECTED_RESET                               0x0
        
        // Request for data in transfer via the QK_DIR register (access: R/W)
        #define QUIDDIKEY_SR_QK_DI_REQUEST_BIT                               5
        #define QUIDDIKEY_SR_QK_DI_REQUEST_WIDTH                             1
        #define QUIDDIKEY_SR_QK_DI_REQUEST_MASK                              0x20
        #define QUIDDIKEY_SR_QK_DI_REQUEST_RESET                             0x0
        
        // Request for data out transfer via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_SR_QK_DO_REQUEST_BIT                               6
        #define QUIDDIKEY_SR_QK_DO_REQUEST_WIDTH                             1
        #define QUIDDIKEY_SR_QK_DO_REQUEST_MASK                              0x40
        #define QUIDDIKEY_SR_QK_DO_REQUEST_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_SR_QK_BUSY_GET(value)                    (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_SR_QK_BUSY_GETS(value)                   (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_SR_QK_BUSY_SET(value,field)              (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_SR_QK_BUSY(val)                          ((val) << 0)
        
        #define QUIDDIKEY_SR_QK_OK_GET(value)                      (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_SR_QK_OK_GETS(value)                     (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_SR_QK_OK_SET(value,field)                (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_SR_QK_OK(val)                            ((val) << 1)
        
        #define QUIDDIKEY_SR_QK_ERROR_GET(value)                   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_SR_QK_ERROR_GETS(value)                  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_SR_QK_ERROR_SET(value,field)             (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_SR_QK_ERROR(val)                         ((val) << 2)
        
        #define QUIDDIKEY_SR_QK_ZEROIZED_GET(value)                (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED_GETS(value)               (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED_SET(value,field)          (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_SR_QK_ZEROIZED(val)                      ((val) << 3)
        
        #define QUIDDIKEY_SR_QK_REJECTED_GET(value)                (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED_GETS(value)               (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED_SET(value,field)          (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_SR_QK_REJECTED(val)                      ((val) << 4)
        
        #define QUIDDIKEY_SR_QK_DI_REQUEST_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_SR_QK_DI_REQUEST(val)                    ((val) << 5)
        
        #define QUIDDIKEY_SR_QK_DO_REQUEST_GET(value)              (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST_GETS(value)             (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST_SET(value,field)        (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_SR_QK_DO_REQUEST(val)                    ((val) << 6)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_busy         :1 ; // Operation is in progress
            unsigned int qk_ok           :1 ; // Last operation was successful
            unsigned int qk_error        :1 ; // Last operation failed
            unsigned int qk_zeroized     :1 ; // QuiddiKey is in Zeroized or Locked state, For details see section 3.1
            unsigned int qk_rejected     :1 ; // Read: Last command was rejected Write 1: Clears this bit
            unsigned int qk_di_request   :1 ; // Request for data in transfer via the QK_DIR register
            unsigned int qk_do_request   :1 ; // Request for data out transfer via the QK_DOR register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_sr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_sr : public vp::reg_32
        {
        public:
            inline void qk_busy_set(uint32_t value);
            inline uint32_t qk_busy_get();
            inline void qk_ok_set(uint32_t value);
            inline uint32_t qk_ok_get();
            inline void qk_error_set(uint32_t value);
            inline uint32_t qk_error_get();
            inline void qk_zeroized_set(uint32_t value);
            inline uint32_t qk_zeroized_get();
            inline void qk_rejected_set(uint32_t value);
            inline uint32_t qk_rejected_get();
            inline void qk_di_request_set(uint32_t value);
            inline uint32_t qk_di_request_get();
            inline void qk_do_request_set(uint32_t value);
            inline uint32_t qk_do_request_get();
        };

|

.. _QUIDDIKEY_AR:

AR
""

QUIDDIKEY Allow register

.. table:: 

    +-----+---+------------------------------+--------------------------------------------+
    |Bit #|R/W|             Name             |                Description                 |
    +=====+===+==============================+============================================+
    |    1|R/W|qk_allow_enroll               |Enroll operation is allowed                 |
    +-----+---+------------------------------+--------------------------------------------+
    |    2|R  |qk_allow_start                |Start operation is allowed                  |
    +-----+---+------------------------------+--------------------------------------------+
    |    5|R  |qk_allow_stop                 |Stop operation is allowed                   |
    +-----+---+------------------------------+--------------------------------------------+
    |    6|R  |qk_allow_get_key              |Get Key operations are allowed              |
    +-----+---+------------------------------+--------------------------------------------+
    |    7|R  |qk_allow_unwrap               |Unwrap operations are allowed               |
    +-----+---+------------------------------+--------------------------------------------+
    |    8|R  |qk_allow_wrap_generated_random|Wrap Generated Random operations are allowed|
    +-----+---+------------------------------+--------------------------------------------+
    |    9|R  |qk_allow_wrap                 |Wrap operations are allowed                 |
    +-----+---+------------------------------+--------------------------------------------+
    |   15|R  |qk_allow_generate_random      |Generate Random operations are allowed      |
    +-----+---+------------------------------+--------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Allow register
                #define QUIDDIKEY_AR_OFFSET                      0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_ar_get(uint32_t base);
        static inline void quiddikey_ar_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enroll operation is allowed (access: R/W)
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_BIT                             1
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_WIDTH                           1
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_MASK                            0x2
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_RESET                           0x0
        
        // Start operation is allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_START_BIT                              2
        #define QUIDDIKEY_AR_QK_ALLOW_START_WIDTH                            1
        #define QUIDDIKEY_AR_QK_ALLOW_START_MASK                             0x4
        #define QUIDDIKEY_AR_QK_ALLOW_START_RESET                            0x0
        
        // Stop operation is allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_BIT                               5
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_WIDTH                             1
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_MASK                              0x20
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_RESET                             0x0
        
        // Get Key operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_BIT                            6
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_WIDTH                          1
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_MASK                           0x40
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_RESET                          0x0
        
        // Unwrap operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_BIT                             7
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_WIDTH                           1
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_MASK                            0x80
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_RESET                           0x0
        
        // Wrap Generated Random operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_BIT              8
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_WIDTH            1
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_MASK             0x100
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_RESET            0x0
        
        // Wrap operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_BIT                               9
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_WIDTH                             1
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_MASK                              0x200
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_RESET                             0x0
        
        // Generate Random operations are allowed (access: R)
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_BIT                    15
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_WIDTH                  1
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_MASK                   0x8000
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_RESET                  0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_GET(value)            (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_GETS(value)           (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL_SET(value,field)      (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_AR_QK_ALLOW_ENROLL(val)                  ((val) << 1)
        
        #define QUIDDIKEY_AR_QK_ALLOW_START_GET(value)             (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START_GETS(value)            (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START_SET(value,field)       (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_AR_QK_ALLOW_START(val)                   ((val) << 2)
        
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_GET(value)              (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_GETS(value)             (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP_SET(value,field)        (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_AR_QK_ALLOW_STOP(val)                    ((val) << 5)
        
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_GET(value)           (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_GETS(value)          (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY_SET(value,field)     (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_AR_QK_ALLOW_GET_KEY(val)                 ((val) << 6)
        
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_GET(value)            (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_GETS(value)           (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP_SET(value,field)      (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_AR_QK_ALLOW_UNWRAP(val)                  ((val) << 7)
        
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GENERATED_RANDOM(val)   ((val) << 8)
        
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GET(value)              (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_GETS(value)             (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP_SET(value,field)        (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_AR_QK_ALLOW_WRAP(val)                    ((val) << 9)
        
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_GET(value)   (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_GETS(value)  (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_AR_QK_ALLOW_GENERATE_RANDOM(val)         ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int qk_allow_enroll :1 ; // Enroll operation is allowed
            unsigned int qk_allow_start  :1 ; // Start operation is allowed
            unsigned int padding1:2 ;
            unsigned int qk_allow_stop   :1 ; // Stop operation is allowed
            unsigned int qk_allow_get_key:1 ; // Get Key operations are allowed
            unsigned int qk_allow_unwrap :1 ; // Unwrap operations are allowed
            unsigned int qk_allow_wrap_generated_random:1 ; // Wrap Generated Random operations are allowed
            unsigned int qk_allow_wrap   :1 ; // Wrap operations are allowed
            unsigned int padding2:5 ;
            unsigned int qk_allow_generate_random:1 ; // Generate Random operations are allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ar_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_ar : public vp::reg_32
        {
        public:
            inline void qk_allow_enroll_set(uint32_t value);
            inline uint32_t qk_allow_enroll_get();
            inline void qk_allow_start_set(uint32_t value);
            inline uint32_t qk_allow_start_get();
            inline void qk_allow_stop_set(uint32_t value);
            inline uint32_t qk_allow_stop_get();
            inline void qk_allow_get_key_set(uint32_t value);
            inline uint32_t qk_allow_get_key_get();
            inline void qk_allow_unwrap_set(uint32_t value);
            inline uint32_t qk_allow_unwrap_get();
            inline void qk_allow_wrap_generated_random_set(uint32_t value);
            inline uint32_t qk_allow_wrap_generated_random_get();
            inline void qk_allow_wrap_set(uint32_t value);
            inline uint32_t qk_allow_wrap_get();
            inline void qk_allow_generate_random_set(uint32_t value);
            inline uint32_t qk_allow_generate_random_get();
        };

|

.. _QUIDDIKEY_IER:

IER
"""

QUIDDIKEY Interrupt Enable register

.. table:: 

    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                       Description                                                       |
    +=====+===+=========+=========================================================================================================================+
    |    0|R/W|qk_int_en|0: Disables all QuiddiKey interrupts 1: Enables all QuiddiKey interrupts that are enabled in the Interrupt Mask register |
    +-----+---+---------+-------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Interrupt Enable register
                #define QUIDDIKEY_IER_OFFSET                     0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_ier_get(uint32_t base);
        static inline void quiddikey_ier_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 0: Disables all QuiddiKey interrupts 1: Enables all QuiddiKey interrupts that are enabled in the Interrupt Mask register  (access: R/W)
        #define QUIDDIKEY_IER_QK_INT_EN_BIT                                  0
        #define QUIDDIKEY_IER_QK_INT_EN_WIDTH                                1
        #define QUIDDIKEY_IER_QK_INT_EN_MASK                                 0x1
        #define QUIDDIKEY_IER_QK_INT_EN_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_IER_QK_INT_EN_GET(value)                 (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN_GETS(value)                (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN_SET(value,field)           (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IER_QK_INT_EN(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_int_en       :1 ; // 0: Disables all QuiddiKey interrupts 1: Enables all QuiddiKey interrupts that are enabled in the Interrupt Mask register 
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_ier_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_ier : public vp::reg_32
        {
        public:
            inline void qk_int_en_set(uint32_t value);
            inline uint32_t qk_int_en_get();
        };

|

.. _QUIDDIKEY_IMR:

IMR
"""

QUIDDIKEY Interrupt Mask register

.. table:: 

    +-----+---+--------------------+--------------------------------------+
    |Bit #|R/W|        Name        |             Description              |
    +=====+===+====================+======================================+
    |    0|R/W|qk_int_en_busy      |Enables the busy interrupt            |
    +-----+---+--------------------+--------------------------------------+
    |    1|R/W|qk_int_en_ok        |Enables the ok interrupt              |
    +-----+---+--------------------+--------------------------------------+
    |    2|R/W|qk_int_en_error     |Enables the error interrupt           |
    +-----+---+--------------------+--------------------------------------+
    |    3|R/W|qk_int_en_zeroized  |Enables the zeroized interrupt        |
    +-----+---+--------------------+--------------------------------------+
    |    4|R/W|qk_int_en_rejected  |Enables the rejected interrupt        |
    +-----+---+--------------------+--------------------------------------+
    |    5|R/W|qk_int_en_di_request|Enables the data in request interrupt |
    +-----+---+--------------------+--------------------------------------+
    |    6|R/W|qk_int_en_do_request|Enables the data out request interrupt|
    +-----+---+--------------------+--------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Interrupt Mask register
                #define QUIDDIKEY_IMR_OFFSET                     0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_imr_get(uint32_t base);
        static inline void quiddikey_imr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Enables the busy interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_BIT                             0
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_WIDTH                           1
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_MASK                            0x1
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_RESET                           0x0
        
        // Enables the ok interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_BIT                               1
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_WIDTH                             1
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_MASK                              0x2
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_RESET                             0x0
        
        // Enables the error interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_BIT                            2
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_WIDTH                          1
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_MASK                           0x4
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_RESET                          0x0
        
        // Enables the zeroized interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_BIT                         3
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_WIDTH                       1
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_MASK                        0x8
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_RESET                       0x0
        
        // Enables the rejected interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_BIT                         4
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_WIDTH                       1
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_MASK                        0x10
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_RESET                       0x0
        
        // Enables the data in request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_BIT                       5
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_WIDTH                     1
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_MASK                      0x20
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_RESET                     0x0
        
        // Enables the data out request interrupt (access: R/W)
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_BIT                       6
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_WIDTH                     1
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_MASK                      0x40
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_GET(value)            (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_GETS(value)           (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY_SET(value,field)      (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_IMR_QK_INT_EN_BUSY(val)                  ((val) << 0)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_GET(value)              (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_GETS(value)             (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK_SET(value,field)        (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_IMR_QK_INT_EN_OK(val)                    ((val) << 1)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_GET(value)           (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_GETS(value)          (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR_SET(value,field)     (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_IMR_QK_INT_EN_ERROR(val)                 ((val) << 2)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_GET(value)        (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_GETS(value)       (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED_SET(value,field)  (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_IMR_QK_INT_EN_ZEROIZED(val)              ((val) << 3)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_GET(value)        (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_GETS(value)       (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED_SET(value,field)  (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_IMR_QK_INT_EN_REJECTED(val)              ((val) << 4)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_GET(value)      (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_GETS(value)     (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_IMR_QK_INT_EN_DI_REQUEST(val)            ((val) << 5)
        
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_GET(value)      (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_GETS(value)     (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_IMR_QK_INT_EN_DO_REQUEST(val)            ((val) << 6)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_int_en_busy  :1 ; // Enables the busy interrupt
            unsigned int qk_int_en_ok    :1 ; // Enables the ok interrupt
            unsigned int qk_int_en_error :1 ; // Enables the error interrupt
            unsigned int qk_int_en_zeroized:1 ; // Enables the zeroized interrupt
            unsigned int qk_int_en_rejected:1 ; // Enables the rejected interrupt
            unsigned int qk_int_en_di_request:1 ; // Enables the data in request interrupt
            unsigned int qk_int_en_do_request:1 ; // Enables the data out request interrupt
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_imr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_imr : public vp::reg_32
        {
        public:
            inline void qk_int_en_busy_set(uint32_t value);
            inline uint32_t qk_int_en_busy_get();
            inline void qk_int_en_ok_set(uint32_t value);
            inline uint32_t qk_int_en_ok_get();
            inline void qk_int_en_error_set(uint32_t value);
            inline uint32_t qk_int_en_error_get();
            inline void qk_int_en_zeroized_set(uint32_t value);
            inline uint32_t qk_int_en_zeroized_get();
            inline void qk_int_en_rejected_set(uint32_t value);
            inline uint32_t qk_int_en_rejected_get();
            inline void qk_int_en_di_request_set(uint32_t value);
            inline uint32_t qk_int_en_di_request_get();
            inline void qk_int_en_do_request_set(uint32_t value);
            inline uint32_t qk_int_en_do_request_get();
        };

|

.. _QUIDDIKEY_ISR:

ISR
"""

QUIDDIKEY Interrupt Status register

.. table:: 

    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |                                                                 Description                                                                 |
    +=====+===+=================+=============================================================================================================================================+
    |    0|R/W|qk_int_busy      |A negative edge has occurred on qk_busy, which means that an operation has completed                                                         |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    1|R/W|qk_int_ok        |A positive edge has occurred on qk_ok, which means that an operation successfully completed                                                  |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    2|R/W|qk_int_error     |A positive edge has occurred on qk_error, which means that an operation has failed                                                           |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    3|R/W|qk_int_zeroized  |A positive edge has occurred on qk_zeroized, which means that QuiddiKey has moved to the Zeroized or Locked state For details see section 3.1|
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    4|R/W|qk_int_rejected  |A positive edge has occurred on qk_rejected, which means that a command was rejected                                                         |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    5|R/W|qk_int_di_request|A positive edge has occurred on qk_di_request, which means that a data in transfer is requested via the QK_DIR register                      |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+
    |    6|R/W|qk_int_do_request|A positive edge has occurred on qk_do_request, which means that a data out transfer is requested via the QK_DOR register                     |
    +-----+---+-----------------+---------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Interrupt Status register
                #define QUIDDIKEY_ISR_OFFSET                     0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_isr_get(uint32_t base);
        static inline void quiddikey_isr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // A negative edge has occurred on qk_busy, which means that an operation has completed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_BUSY_BIT                                0
        #define QUIDDIKEY_ISR_QK_INT_BUSY_WIDTH                              1
        #define QUIDDIKEY_ISR_QK_INT_BUSY_MASK                               0x1
        #define QUIDDIKEY_ISR_QK_INT_BUSY_RESET                              0x0
        
        // A positive edge has occurred on qk_ok, which means that an operation successfully completed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_OK_BIT                                  1
        #define QUIDDIKEY_ISR_QK_INT_OK_WIDTH                                1
        #define QUIDDIKEY_ISR_QK_INT_OK_MASK                                 0x2
        #define QUIDDIKEY_ISR_QK_INT_OK_RESET                                0x0
        
        // A positive edge has occurred on qk_error, which means that an operation has failed (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_ERROR_BIT                               2
        #define QUIDDIKEY_ISR_QK_INT_ERROR_WIDTH                             1
        #define QUIDDIKEY_ISR_QK_INT_ERROR_MASK                              0x4
        #define QUIDDIKEY_ISR_QK_INT_ERROR_RESET                             0x0
        
        // A positive edge has occurred on qk_zeroized, which means that QuiddiKey has moved to the Zeroized or Locked state For details see section 3.1 (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_BIT                            3
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_WIDTH                          1
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_MASK                           0x8
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_RESET                          0x0
        
        // A positive edge has occurred on qk_rejected, which means that a command was rejected (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_BIT                            4
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_WIDTH                          1
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_MASK                           0x10
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_RESET                          0x0
        
        // A positive edge has occurred on qk_di_request, which means that a data in transfer is requested via the QK_DIR register (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_BIT                          5
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_WIDTH                        1
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_MASK                         0x20
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_RESET                        0x0
        
        // A positive edge has occurred on qk_do_request, which means that a data out transfer is requested via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_BIT                          6
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_WIDTH                        1
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_MASK                         0x40
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_ISR_QK_INT_BUSY_GET(value)               (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY_GETS(value)              (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY_SET(value,field)         (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_ISR_QK_INT_BUSY(val)                     ((val) << 0)
        
        #define QUIDDIKEY_ISR_QK_INT_OK_GET(value)                 (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK_GETS(value)                (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK_SET(value,field)           (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_ISR_QK_INT_OK(val)                       ((val) << 1)
        
        #define QUIDDIKEY_ISR_QK_INT_ERROR_GET(value)              (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR_GETS(value)             (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR_SET(value,field)        (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_ISR_QK_INT_ERROR(val)                    ((val) << 2)
        
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_GET(value)           (GAP_BEXTRACTU((value),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_GETS(value)          (GAP_BEXTRACT((value),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED_SET(value,field)     (GAP_BINSERT((value),(field),1,3))
        #define QUIDDIKEY_ISR_QK_INT_ZEROIZED(val)                 ((val) << 3)
        
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_GET(value)           (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_GETS(value)          (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED_SET(value,field)     (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_ISR_QK_INT_REJECTED(val)                 ((val) << 4)
        
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_ISR_QK_INT_DI_REQUEST(val)               ((val) << 5)
        
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_GET(value)         (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_GETS(value)        (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST_SET(value,field)   (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_ISR_QK_INT_DO_REQUEST(val)               ((val) << 6)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_int_busy     :1 ; // A negative edge has occurred on qk_busy, which means that an operation has completed
            unsigned int qk_int_ok       :1 ; // A positive edge has occurred on qk_ok, which means that an operation successfully completed
            unsigned int qk_int_error    :1 ; // A positive edge has occurred on qk_error, which means that an operation has failed
            unsigned int qk_int_zeroized :1 ; // A positive edge has occurred on qk_zeroized, which means that QuiddiKey has moved to the Zeroized or Locked state For details see section 3.1
            unsigned int qk_int_rejected :1 ; // A positive edge has occurred on qk_rejected, which means that a command was rejected
            unsigned int qk_int_di_request:1 ; // A positive edge has occurred on qk_di_request, which means that a data in transfer is requested via the QK_DIR register
            unsigned int qk_int_do_request:1 ; // A positive edge has occurred on qk_do_request, which means that a data out transfer is requested via the QK_DOR register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_isr_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_isr : public vp::reg_32
        {
        public:
            inline void qk_int_busy_set(uint32_t value);
            inline uint32_t qk_int_busy_get();
            inline void qk_int_ok_set(uint32_t value);
            inline uint32_t qk_int_ok_get();
            inline void qk_int_error_set(uint32_t value);
            inline uint32_t qk_int_error_get();
            inline void qk_int_zeroized_set(uint32_t value);
            inline uint32_t qk_int_zeroized_get();
            inline void qk_int_rejected_set(uint32_t value);
            inline uint32_t qk_int_rejected_get();
            inline void qk_int_di_request_set(uint32_t value);
            inline uint32_t qk_int_di_request_get();
            inline void qk_int_do_request_set(uint32_t value);
            inline uint32_t qk_int_do_request_get();
        };

|

.. _QUIDDIKEY_KEY_DEST:

KEY_DEST
""""""""

QUIDDIKEY Key Destination register

.. table:: 

    +-----+---+-----------+--------------------------------------------------+
    |Bit #|R/W|   Name    |                   Description                    |
    +=====+===+===========+==================================================+
    |    0|R/W|qk_dest_reg|Key will be made available via the QK_DOR register|
    +-----+---+-----------+--------------------------------------------------+
    |    1|R/W|qk_dest_ko |Key will be made available via the KO interface   |
    +-----+---+-----------+--------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Key Destination register
                #define QUIDDIKEY_KEY_DEST_OFFSET                0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_key_dest_get(uint32_t base);
        static inline void quiddikey_key_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Key will be made available via the QK_DOR register (access: R/W)
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_BIT                           0
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_WIDTH                         1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_MASK                          0x1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_RESET                         0x0
        
        // Key will be made available via the KO interface (access: R/W)
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_BIT                            1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_WIDTH                          1
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_MASK                           0x2
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_GET(value)          (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_GETS(value)         (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG_SET(value,field)    (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_REG(val)                ((val) << 0)
        
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_GET(value)           (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_GETS(value)          (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO_SET(value,field)     (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_KEY_DEST_QK_DEST_KO(val)                 ((val) << 1)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_dest_reg     :1 ; // Key will be made available via the QK_DOR register
            unsigned int qk_dest_ko      :1 ; // Key will be made available via the KO interface
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_key_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_key_dest : public vp::reg_32
        {
        public:
            inline void qk_dest_reg_set(uint32_t value);
            inline uint32_t qk_dest_reg_get();
            inline void qk_dest_ko_set(uint32_t value);
            inline uint32_t qk_dest_ko_get();
        };

|

.. _QUIDDIKEY_DIR:

DIR
"""

QUIDDIKEY Data Input register

.. table:: 

    +-----+---+-----+------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                    Description                                     |
    +=====+===+=====+====================================================================================+
    |31:0 |W  |qk_di|Input data to QuiddiKey Note: This field must only be written when qk_di_request = 1|
    +-----+---+-----+------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Data Input register
                #define QUIDDIKEY_DIR_OFFSET                     0xa0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_dir_get(uint32_t base);
        static inline void quiddikey_dir_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Input data to QuiddiKey Note: This field must only be written when qk_di_request = 1 (access: W)
        #define QUIDDIKEY_DIR_QK_DI_BIT                                      0
        #define QUIDDIKEY_DIR_QK_DI_WIDTH                                    32
        #define QUIDDIKEY_DIR_QK_DI_MASK                                     0xffffffff
        #define QUIDDIKEY_DIR_QK_DI_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_DIR_QK_DI_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DIR_QK_DI_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DIR_QK_DI_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DIR_QK_DI(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_di           :32; // Input data to QuiddiKey Note: This field must only be written when qk_di_request = 1
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dir_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_dir : public vp::reg_32
        {
        public:
            inline void qk_di_set(uint32_t value);
            inline uint32_t qk_di_get();
        };

|

.. _QUIDDIKEY_DOR:

DOR
"""

QUIDDIKEY Data Output register

.. table:: 

    +-----+---+-----+------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                    Description                                     |
    +=====+===+=====+====================================================================================+
    |31:0 |W  |qk_do|Output data from QuiddiKey Note: This field must only be read when qk_do_request = 1|
    +-----+---+-----+------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Data Output register
                #define QUIDDIKEY_DOR_OFFSET                     0xa8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_dor_get(uint32_t base);
        static inline void quiddikey_dor_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Output data from QuiddiKey Note: This field must only be read when qk_do_request = 1 (access: W)
        #define QUIDDIKEY_DOR_QK_DO_BIT                                      0
        #define QUIDDIKEY_DOR_QK_DO_WIDTH                                    32
        #define QUIDDIKEY_DOR_QK_DO_MASK                                     0xffffffff
        #define QUIDDIKEY_DOR_QK_DO_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_DOR_QK_DO_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_DOR_QK_DO_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_DOR_QK_DO_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_DOR_QK_DO(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_do           :32; // Output data from QuiddiKey Note: This field must only be read when qk_do_request = 1
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_dor_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_dor : public vp::reg_32
        {
        public:
            inline void qk_do_set(uint32_t value);
            inline uint32_t qk_do_get();
        };

|

.. _QUIDDIKEY_MISC:

MISC
""""

QUIDDIKEY Miscellaneous register

.. table:: 

    +-----+---+------------------+---------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|       Name       |                                                           Description                                                           |
    +=====+===+==================+=================================================================================================================================+
    |    0|R/W|qk_data_endianness| Defines the endianness of keys and data in system memory: 0: Little endian 1: Big endian (default) See section 5.1.4 for details|
    +-----+---+------------------+---------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Miscellaneous register
                #define QUIDDIKEY_MISC_OFFSET                    0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_misc_get(uint32_t base);
        static inline void quiddikey_misc_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        //  Defines the endianness of keys and data in system memory: 0: Little endian 1: Big endian (default) See section 5.1.4 for details (access: R/W)
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_BIT                        0
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_WIDTH                      1
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_MASK                       0x1
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_GET(value)       (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_GETS(value)      (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_MISC_QK_DATA_ENDIANNESS(val)             ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_data_endianness:1 ; //  Defines the endianness of keys and data in system memory: 0: Little endian 1: Big endian (default) See section 5.1.4 for details
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_misc_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_misc : public vp::reg_32
        {
        public:
            inline void qk_data_endianness_set(uint32_t value);
            inline uint32_t qk_data_endianness_get();
        };

|

.. _QUIDDIKEY_IF_SR:

IF_SR
"""""

QUIDDIKEY Interface Status register

.. table:: 

    +-----+---+---------+---------------------------------------------------------+
    |Bit #|R/W|  Name   |                       Description                       |
    +=====+===+=========+=========================================================+
    |    0|R  |apb_error| An APB error has occurred, for details see section 5.1.2|
    +-----+---+---------+---------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Interface Status register
                #define QUIDDIKEY_IF_SR_OFFSET                   0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_if_sr_get(uint32_t base);
        static inline void quiddikey_if_sr_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        //  An APB error has occurred, for details see section 5.1.2 (access: R)
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
            unsigned int apb_error       :1 ; //  An APB error has occurred, for details see section 5.1.2
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

.. _QUIDDIKEY_TEST:

TEST
""""

QUIDDIKEY Test register

.. table:: 

    +-----+---+---------------+--------------------------------------------------------------+
    |Bit #|R/W|     Name      |                         Description                          |
    +=====+===+===============+==============================================================+
    |    0|R/W|qk_bist_enable |Isolates QuiddiKey and runs BIST                              |
    +-----+---+---------------+--------------------------------------------------------------+
    |    4|R/W|qk_bist_running|BIST is in progress or finishing up (after qk_bist_enable = 0)|
    +-----+---+---------------+--------------------------------------------------------------+
    |    5|R/W|qk_bist_active |BIST is in progress                                           |
    +-----+---+---------------+--------------------------------------------------------------+
    |    6|R/W|qk_bist_ok     |BIST has passed                                               |
    +-----+---+---------------+--------------------------------------------------------------+
    |    7|R/W|qk_bist_error  |BIST has failed                                               |
    +-----+---+---------------+--------------------------------------------------------------+
    |   31|R/W|qk_allow_bist  |BIST is allowed                                               |
    +-----+---+---------------+--------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Test register
                #define QUIDDIKEY_TEST_OFFSET                    0xd8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_test_get(uint32_t base);
        static inline void quiddikey_test_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Isolates QuiddiKey and runs BIST (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_BIT                            0
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_WIDTH                          1
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_MASK                           0x1
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_RESET                          0x0
        
        // BIST is in progress or finishing up (after qk_bist_enable = 0) (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_BIT                           4
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_WIDTH                         1
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_MASK                          0x10
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_RESET                         0x0
        
        // BIST is in progress (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_BIT                            5
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_WIDTH                          1
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_MASK                           0x20
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_RESET                          0x0
        
        // BIST has passed (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_OK_BIT                                6
        #define QUIDDIKEY_TEST_QK_BIST_OK_WIDTH                              1
        #define QUIDDIKEY_TEST_QK_BIST_OK_MASK                               0x40
        #define QUIDDIKEY_TEST_QK_BIST_OK_RESET                              0x0
        
        // BIST has failed (access: R/W)
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_BIT                             7
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_WIDTH                           1
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_MASK                            0x80
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_RESET                           0x0
        
        // BIST is allowed (access: R/W)
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_BIT                             31
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_WIDTH                           1
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_MASK                            0x80000000
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_GET(value)           (GAP_BEXTRACTU((value),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_GETS(value)          (GAP_BEXTRACT((value),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE_SET(value,field)     (GAP_BINSERT((value),(field),1,0))
        #define QUIDDIKEY_TEST_QK_BIST_ENABLE(val)                 ((val) << 0)
        
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_GET(value)          (GAP_BEXTRACTU((value),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_GETS(value)         (GAP_BEXTRACT((value),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING_SET(value,field)    (GAP_BINSERT((value),(field),1,4))
        #define QUIDDIKEY_TEST_QK_BIST_RUNNING(val)                ((val) << 4)
        
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_GET(value)           (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_GETS(value)          (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE_SET(value,field)     (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_TEST_QK_BIST_ACTIVE(val)                 ((val) << 5)
        
        #define QUIDDIKEY_TEST_QK_BIST_OK_GET(value)               (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK_GETS(value)              (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK_SET(value,field)         (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_TEST_QK_BIST_OK(val)                     ((val) << 6)
        
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_GET(value)            (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_GETS(value)           (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR_SET(value,field)      (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_TEST_QK_BIST_ERROR(val)                  ((val) << 7)
        
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_GET(value)            (GAP_BEXTRACTU((value),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_GETS(value)           (GAP_BEXTRACT((value),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST_SET(value,field)      (GAP_BINSERT((value),(field),1,31))
        #define QUIDDIKEY_TEST_QK_ALLOW_BIST(val)                  ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_bist_enable  :1 ; // Isolates QuiddiKey and runs BIST
            unsigned int padding0:3 ;
            unsigned int qk_bist_running :1 ; // BIST is in progress or finishing up (after qk_bist_enable = 0)
            unsigned int qk_bist_active  :1 ; // BIST is in progress
            unsigned int qk_bist_ok      :1 ; // BIST has passed
            unsigned int qk_bist_error   :1 ; // BIST has failed
            unsigned int padding1:23;
            unsigned int qk_allow_bist   :1 ; // BIST is allowed
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_test_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_test : public vp::reg_32
        {
        public:
            inline void qk_bist_enable_set(uint32_t value);
            inline uint32_t qk_bist_enable_get();
            inline void qk_bist_running_set(uint32_t value);
            inline uint32_t qk_bist_running_get();
            inline void qk_bist_active_set(uint32_t value);
            inline uint32_t qk_bist_active_get();
            inline void qk_bist_ok_set(uint32_t value);
            inline uint32_t qk_bist_ok_get();
            inline void qk_bist_error_set(uint32_t value);
            inline uint32_t qk_bist_error_get();
            inline void qk_allow_bist_set(uint32_t value);
            inline uint32_t qk_allow_bist_get();
        };

|

.. _QUIDDIKEY_HW_RUC0:

HW_RUC0
"""""""

QUIDDIKEY Hardware Restrict User Context 0 register

.. table:: 

    +-----+---+--------------------------+------------------------------+
    |Bit #|R/W|           Name           |         Description          |
    +=====+===+==========================+==============================+
    |31:0 |R/W|qk_restrict_user_context_0|Hardware Restrict UC0 register|
    +-----+---+--------------------------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware Restrict User Context 0 register
                #define QUIDDIKEY_HW_RUC0_OFFSET                 0xe0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ruc0_get(uint32_t base);
        static inline void quiddikey_hw_ruc0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware Restrict UC0 register (access: R/W)
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_BIT             0
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_WIDTH           32
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_MASK            0xffffffff
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_RESET           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC0_QK_RESTRICT_USER_CONTEXT_0(val)  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_restrict_user_context_0:32; // Hardware Restrict UC0 register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ruc0 : public vp::reg_32
        {
        public:
            inline void qk_restrict_user_context_0_set(uint32_t value);
            inline uint32_t qk_restrict_user_context_0_get();
        };

|

.. _QUIDDIKEY_HW_RUC1:

HW_RUC1
"""""""

QUIDDIKEY Hardware Restrict User Context 1 register

.. table:: 

    +-----+---+--------------------------+------------------------------+
    |Bit #|R/W|           Name           |         Description          |
    +=====+===+==========================+==============================+
    |31:0 |R/W|qk_restrict_user_context_1|Hardware Restrict UC1 register|
    +-----+---+--------------------------+------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware Restrict User Context 1 register
                #define QUIDDIKEY_HW_RUC1_OFFSET                 0xe4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ruc1_get(uint32_t base);
        static inline void quiddikey_hw_ruc1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware Restrict UC1 register (access: R/W)
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_BIT             0
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_WIDTH           32
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_MASK            0xffffffff
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_RESET           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_GET(value) (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_GETS(value) (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_RUC1_QK_RESTRICT_USER_CONTEXT_1(val)  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_restrict_user_context_1:32; // Hardware Restrict UC1 register
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ruc1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ruc1 : public vp::reg_32
        {
        public:
            inline void qk_restrict_user_context_1_set(uint32_t value);
            inline uint32_t qk_restrict_user_context_1_get();
        };

|

.. _QUIDDIKEY_HW_SETTING:

HW_SETTING
""""""""""

QUIDDIKEY Hardware Setting register

.. table:: 

    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |Bit #|R/W|              Name              |                                         Description                                          |
    +=====+===+================================+==============================================================================================+
    |    1|R  |qk_disable_enroll               |0:Enroll operations are enabled 1:Enroll operations are disabled                              |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    2|R  |qk_disable_start                |0:Start operations are disabled 1:Start operations are disabled                               |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    5|R  |qk_disable_stop                 |0:Stop operations are enabled 1:Stop operations are disabled                                  |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    6|R  |qk_disable_get_key              |0:Get Key operations are enabled 1:Get Key operations are disabled                            |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    7|R  |qk_disable_unwrap               |0:Unwrap operations are enabled 1:Unwrap operations are disabled                              |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    8|R  |qk_disable_wrap_generated_random|0:Wrap Generated Random operations are enabled 1:Wrap Generated Random operations are disabled|
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |    9|R  |qk_disable_wrap                 |0:Wrap operations are enabled 1:Wrap operations are disabled                                  |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+
    |   15|R  |qk_disable_generate_random      |0:Generate Random operations are enabled 1:Generate Random operations are disabled            |
    +-----+---+--------------------------------+----------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware Setting register
                #define QUIDDIKEY_HW_SETTING_OFFSET              0xf0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_setting_get(uint32_t base);
        static inline void quiddikey_hw_setting_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // 0:Enroll operations are enabled 1:Enroll operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_BIT                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_WIDTH                 1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_MASK                  0x2
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_RESET                 0x0
        
        // 0:Start operations are disabled 1:Start operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_BIT                    2
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_WIDTH                  1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_MASK                   0x4
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_RESET                  0x0
        
        // 0:Stop operations are enabled 1:Stop operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_BIT                     5
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_MASK                    0x20
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_RESET                   0x0
        
        // 0:Get Key operations are enabled 1:Get Key operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_BIT                  6
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_WIDTH                1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_MASK                 0x40
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_RESET                0x0
        
        // 0:Unwrap operations are enabled 1:Unwrap operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_BIT                   7
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_WIDTH                 1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_MASK                  0x80
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_RESET                 0x0
        
        // 0:Wrap Generated Random operations are enabled 1:Wrap Generated Random operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_BIT    8
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_WIDTH  1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_MASK   0x100
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_RESET  0x0
        
        // 0:Wrap operations are enabled 1:Wrap operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_BIT                     9
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_WIDTH                   1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_MASK                    0x200
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_RESET                   0x0
        
        // 0:Generate Random operations are enabled 1:Generate Random operations are disabled (access: R)
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_BIT          15
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_WIDTH        1
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_MASK         0x8000
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_RESET        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_GET(value)  (GAP_BEXTRACTU((value),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_GETS(value) (GAP_BEXTRACT((value),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL_SET(value,field) (GAP_BINSERT((value),(field),1,1))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_ENROLL(val)        ((val) << 1)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_GET(value)   (GAP_BEXTRACTU((value),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_GETS(value)  (GAP_BEXTRACT((value),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START_SET(value,field) (GAP_BINSERT((value),(field),1,2))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_START(val)         ((val) << 2)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_GET(value)    (GAP_BEXTRACTU((value),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_GETS(value)   (GAP_BEXTRACT((value),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP_SET(value,field) (GAP_BINSERT((value),(field),1,5))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_STOP(val)          ((val) << 5)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_GET(value) (GAP_BEXTRACTU((value),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_GETS(value) (GAP_BEXTRACT((value),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY_SET(value,field) (GAP_BINSERT((value),(field),1,6))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GET_KEY(val)       ((val) << 6)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_GET(value)  (GAP_BEXTRACTU((value),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_GETS(value) (GAP_BEXTRACT((value),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_UNWRAP(val)        ((val) << 7)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,8))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GENERATED_RANDOM(val) ((val) << 8)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GET(value)    (GAP_BEXTRACTU((value),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_GETS(value)   (GAP_BEXTRACT((value),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP_SET(value,field) (GAP_BINSERT((value),(field),1,9))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_WRAP(val)          ((val) << 9)
        
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_GET(value) (GAP_BEXTRACTU((value),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_GETS(value) (GAP_BEXTRACT((value),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM_SET(value,field) (GAP_BINSERT((value),(field),1,15))
        #define QUIDDIKEY_HW_SETTING_QK_DISABLE_GENERATE_RANDOM(val) ((val) << 15)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:1 ;
            unsigned int qk_disable_enroll:1 ; // 0:Enroll operations are enabled 1:Enroll operations are disabled
            unsigned int qk_disable_start:1 ; // 0:Start operations are disabled 1:Start operations are disabled
            unsigned int padding1:2 ;
            unsigned int qk_disable_stop :1 ; // 0:Stop operations are enabled 1:Stop operations are disabled
            unsigned int qk_disable_get_key:1 ; // 0:Get Key operations are enabled 1:Get Key operations are disabled
            unsigned int qk_disable_unwrap:1 ; // 0:Unwrap operations are enabled 1:Unwrap operations are disabled
            unsigned int qk_disable_wrap_generated_random:1 ; // 0:Wrap Generated Random operations are enabled 1:Wrap Generated Random operations are disabled
            unsigned int qk_disable_wrap :1 ; // 0:Wrap operations are enabled 1:Wrap operations are disabled
            unsigned int padding2:5 ;
            unsigned int qk_disable_generate_random:1 ; // 0:Generate Random operations are enabled 1:Generate Random operations are disabled
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_setting_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_setting : public vp::reg_32
        {
        public:
            inline void qk_disable_enroll_set(uint32_t value);
            inline uint32_t qk_disable_enroll_get();
            inline void qk_disable_start_set(uint32_t value);
            inline uint32_t qk_disable_start_get();
            inline void qk_disable_stop_set(uint32_t value);
            inline uint32_t qk_disable_stop_get();
            inline void qk_disable_get_key_set(uint32_t value);
            inline uint32_t qk_disable_get_key_get();
            inline void qk_disable_unwrap_set(uint32_t value);
            inline uint32_t qk_disable_unwrap_get();
            inline void qk_disable_wrap_generated_random_set(uint32_t value);
            inline uint32_t qk_disable_wrap_generated_random_get();
            inline void qk_disable_wrap_set(uint32_t value);
            inline uint32_t qk_disable_wrap_get();
            inline void qk_disable_generate_random_set(uint32_t value);
            inline uint32_t qk_disable_generate_random_get();
        };

|

.. _QUIDDIKEY_HW_INFO:

HW_INFO
"""""""

QUIDDIKEY Hardware information register

.. table:: 

    +-----+---+--------------+---------------------------------------------------------------------+
    |Bit #|R/W|     Name     |                             Description                             |
    +=====+===+==============+=====================================================================+
    |31:28|R  |qk_config_type|QuiddiKey configuration: 0001: Safe 0010: Plus Other values: Reserved|
    +-----+---+--------------+---------------------------------------------------------------------+
    |22   |R  |qk_config_bist|1: BIST is included 0: BIST is not included                          |
    +-----+---+--------------+---------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware information register
                #define QUIDDIKEY_HW_INFO_OFFSET                 0xf4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_info_get(uint32_t base);
        static inline void quiddikey_hw_info_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // QuiddiKey configuration: 0001: Safe 0010: Plus Other values: Reserved (access: R)
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_BIT                         28
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_WIDTH                       4
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_MASK                        0xf0000000
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_RESET                       0x10
        
        // 1: BIST is included 0: BIST is not included (access: R)
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_BIT                         22
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_WIDTH                       1
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_MASK                        0x400000
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_RESET                       0x1

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_GET(value)        (GAP_BEXTRACTU((value),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_GETS(value)       (GAP_BEXTRACT((value),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE_SET(value,field)  (GAP_BINSERT((value),(field),4,28))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_TYPE(val)              ((val) << 28)
        
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_GET(value)        (GAP_BEXTRACTU((value),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_GETS(value)       (GAP_BEXTRACT((value),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST_SET(value,field)  (GAP_BINSERT((value),(field),1,22))
        #define QUIDDIKEY_HW_INFO_QK_CONFIG_BIST(val)              ((val) << 22)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:28;
            unsigned int qk_config_type  :4 ; // QuiddiKey configuration: 0001: Safe 0010: Plus Other values: Reserved
            unsigned int qk_config_bist  :1 ; // 1: BIST is included 0: BIST is not included
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_info_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_info : public vp::reg_32
        {
        public:
            inline void qk_config_type_set(uint32_t value);
            inline uint32_t qk_config_type_get();
            inline void qk_config_bist_set(uint32_t value);
            inline uint32_t qk_config_bist_get();
        };

|

.. _QUIDDIKEY_HW_ID:

HW_ID
"""""

QUIDDIKEY Hardware Identifier register

.. table:: 

    +-----+---+--------+-------------------+
    |Bit #|R/W|  Name  |    Description    |
    +=====+===+========+===================+
    |31:0 |R  |qk_hw_id|Hardware Identifier|
    +-----+---+--------+-------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware Identifier register
                #define QUIDDIKEY_HW_ID_OFFSET                   0xf8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_id_get(uint32_t base);
        static inline void quiddikey_hw_id_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware Identifier (access: R)
        #define QUIDDIKEY_HW_ID_QK_HW_ID_BIT                                 0
        #define QUIDDIKEY_HW_ID_QK_HW_ID_WIDTH                               32
        #define QUIDDIKEY_HW_ID_QK_HW_ID_MASK                                0xffffffff
        #define QUIDDIKEY_HW_ID_QK_HW_ID_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_ID_QK_HW_ID_GET(value)                (GAP_BEXTRACTU((value),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID_GETS(value)               (GAP_BEXTRACT((value),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID_SET(value,field)          (GAP_BINSERT((value),(field),32,0))
        #define QUIDDIKEY_HW_ID_QK_HW_ID(val)                      ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_hw_id        :32; // Hardware Identifier
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_id_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_id : public vp::reg_32
        {
        public:
            inline void qk_hw_id_set(uint32_t value);
            inline uint32_t qk_hw_id_get();
        };

|

.. _QUIDDIKEY_HW_VER:

HW_VER
""""""

QUIDDIKEY Hardware Version register

.. table:: 

    +-----+---+-------------------+----------------------------+
    |Bit #|R/W|       Name        |        Description         |
    +=====+===+===================+============================+
    |7:0  |R  |qk_hw_version_major|Hardware version, major part|
    +-----+---+-------------------+----------------------------+
    |15:8 |R  |qk_hw_version_minor|Hardware version, minor part|
    +-----+---+-------------------+----------------------------+
    |23:16|R  |qk_hw_rev          |Hardware version, patch part|
    +-----+---+-------------------+----------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // QUIDDIKEY Hardware Version register
                #define QUIDDIKEY_HW_VER_OFFSET                  0xfc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t quiddikey_hw_ver_get(uint32_t base);
        static inline void quiddikey_hw_ver_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Hardware version, major part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_BIT                     0
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_WIDTH                   8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_MASK                    0xff
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_RESET                   0x0
        
        // Hardware version, minor part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_BIT                     8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_WIDTH                   8
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_MASK                    0xff00
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_RESET                   0x0
        
        // Hardware version, patch part (access: R)
        #define QUIDDIKEY_HW_VER_QK_HW_REV_BIT                               16
        #define QUIDDIKEY_HW_VER_QK_HW_REV_WIDTH                             8
        #define QUIDDIKEY_HW_VER_QK_HW_REV_MASK                              0xff0000
        #define QUIDDIKEY_HW_VER_QK_HW_REV_RESET                             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_GET(value)    (GAP_BEXTRACTU((value),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_GETS(value)   (GAP_BEXTRACT((value),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MAJOR(val)          ((val) << 0)
        
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_GET(value)    (GAP_BEXTRACTU((value),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_GETS(value)   (GAP_BEXTRACT((value),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR_SET(value,field) (GAP_BINSERT((value),(field),8,8))
        #define QUIDDIKEY_HW_VER_QK_HW_VERSION_MINOR(val)          ((val) << 8)
        
        #define QUIDDIKEY_HW_VER_QK_HW_REV_GET(value)              (GAP_BEXTRACTU((value),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV_GETS(value)             (GAP_BEXTRACT((value),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV_SET(value,field)        (GAP_BINSERT((value),(field),8,16))
        #define QUIDDIKEY_HW_VER_QK_HW_REV(val)                    ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qk_hw_version_major:8 ; // Hardware version, major part
            unsigned int qk_hw_version_minor:8 ; // Hardware version, minor part
            unsigned int qk_hw_rev       :8 ; // Hardware version, patch part
          };
          unsigned int raw;
        } __attribute__((packed)) quiddikey_hw_ver_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_quiddikey_hw_ver : public vp::reg_32
        {
        public:
            inline void qk_hw_version_major_set(uint32_t value);
            inline uint32_t qk_hw_version_major_get();
            inline void qk_hw_version_minor_set(uint32_t value);
            inline uint32_t qk_hw_version_minor_get();
            inline void qk_hw_rev_set(uint32_t value);
            inline uint32_t qk_hw_rev_get();
        };

|
