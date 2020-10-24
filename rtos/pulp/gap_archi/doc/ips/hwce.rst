Input file: fe/ips/hwce-new/docs/HWCE_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |                                 Name                                 |Offset|Width|                              Description                               |
    +======================================================================+======+=====+========================================================================+
    |:ref:`TRIGGER<hwce_TRIGGER>`                                          |     0|   32|Trigger the execution of an offloaded job                               |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`ACQUIRE<hwce_ACQUIRE>`                                          |     4|   32|Acquire the lock to offload job                                         |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`FINISHED_JOBS<hwce_FINISHED_JOBS>`                              |     8|   32|Number of concluded jobs since last read                                |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`STATUS<hwce_STATUS>`                                            |    12|   32|Status of the HWCE                                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`RUNNING_JOB<hwce_RUNNING_JOB>`                                  |    16|   32|ID of the currently running job                                         |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`SOFT_CLEAR<hwce_SOFT_CLEAR>`                                    |    20|   32|Reset HWCE to known idle state                                          |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`OFFLOADER_ID<hwce_OFFLOADER_ID>`                                |    24|   32|ID of offloader cores                                                   |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`SW_EVT<hwce_SW_EVT>`                                            |    28|   32|Software synchronization                                                |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`GEN_CONFIG0<hwce_GEN_CONFIG0>`                                  |    32|   32|Generic configuration register 0                                        |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`GEN_CONFIG1<hwce_GEN_CONFIG1>`                                  |    36|   32|Generic configuration register 1                                        |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`GEN_CONFIG2<hwce_GEN_CONFIG2>`                                  |    40|   32|Generic configuration register 2                                        |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`GEN_CONFIG3<hwce_GEN_CONFIG3>`                                  |    44|   32|Generic configuration register 3                                        |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_TRANS_SIZE_ALIASED<hwce_Y_TRANS_SIZE_ALIASED>`                |    64|   32|Total number of words to be read for y<sub>in</sub> and y<sub>out</sub> |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_LINE_STRIDE_LENGTH_ALIASED<hwce_Y_LINE_STRIDE_LENGTH_ALIASED>`|    68|   32|Line stride and length for y<sub>in</sub> and y<sub>out</sub>           |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_FEAT_STRIDE_LENGTH_ALIASED<hwce_Y_FEAT_STRIDE_LENGTH_ALIASED>`|    72|   32|Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>|
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_2_BASE_ADDR_ALIASED<hwce_Y_OUT_2_BASE_ADDR_ALIASED>`      |    80|   32|Base address of y<sub>out</sub>[2]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_1_BASE_ADDR_ALIASED<hwce_Y_OUT_1_BASE_ADDR_ALIASED>`      |    84|   32|Base address of y<sub>out</sub>[1]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_0_BASE_ADDR_ALIASED<hwce_Y_OUT_0_BASE_ADDR_ALIASED>`      |    88|   32|Base address of y<sub>out</sub>[0]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_2_BASE_ADDR_ALIASED<hwce_Y_IN_2_BASE_ADDR_ALIASED>`        |    96|   32|Base address of y<sub>in</sub>[2]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_1_BASE_ADDR_ALIASED<hwce_Y_IN_1_BASE_ADDR_ALIASED>`        |   100|   32|Base address of y<sub>in</sub>[1]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_0_BASE_ADDR_ALIASED<hwce_Y_IN_0_BASE_ADDR_ALIASED>`        |   104|   32|Base address of y<sub>in</sub>[0]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_TRANS_SIZE_ALIASED<hwce_X_TRANS_SIZE_ALIASED>`                |   108|   32|Total number of words to be read for x<sub>in</sub>                     |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_LINE_STRIDE_LENGTH_ALIASED<hwce_X_LINE_STRIDE_LENGTH_ALIASED>`|   112|   32|Line stride and length for x<sub>in</sub>                               |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_FEAT_STRIDE_LENGTH_ALIASED<hwce_X_FEAT_STRIDE_LENGTH_ALIASED>`|   116|   32|Feature (block) stride and length for x<sub>in</sub>                    |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_IN_BASE_ADDR_ALIASED<hwce_X_IN_BASE_ADDR_ALIASED>`            |   120|   32|Base address of x<sub>in</sub>                                          |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`W_BASE_ADDR_ALIASED<hwce_W_BASE_ADDR_ALIASED>`                  |   124|   32|Base address of W                                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG0_ALIASED<hwce_JOB_CONFIG0_ALIASED>`                  |   128|   32|Job configuration register 0                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG1_ALIASED<hwce_JOB_CONFIG1_ALIASED>`                  |   132|   32|Job configuration register 1                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG2_ALIASED<hwce_JOB_CONFIG2_ALIASED>`                  |   136|   32|Job configuration register 2                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_TRANS_SIZE_CTX0<hwce_Y_TRANS_SIZE_CTX0>`                      |   320|   32|Total number of words to be read for y<sub>in</sub> and y<sub>out</sub> |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_LINE_STRIDE_LENGTH_CTX0<hwce_Y_LINE_STRIDE_LENGTH_CTX0>`      |   324|   32|Line stride and length for y<sub>in</sub> and y<sub>out</sub>           |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_FEAT_STRIDE_LENGTH_CTX0<hwce_Y_FEAT_STRIDE_LENGTH_CTX0>`      |   328|   32|Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>|
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_2_BASE_ADDR_CTX0<hwce_Y_OUT_2_BASE_ADDR_CTX0>`            |   336|   32|Base address of y<sub>out</sub>[2]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_1_BASE_ADDR_CTX0<hwce_Y_OUT_1_BASE_ADDR_CTX0>`            |   340|   32|Base address of y<sub>out</sub>[1]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_0_BASE_ADDR_CTX0<hwce_Y_OUT_0_BASE_ADDR_CTX0>`            |   344|   32|Base address of y<sub>out</sub>[0]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_2_BASE_ADDR_CTX0<hwce_Y_IN_2_BASE_ADDR_CTX0>`              |   352|   32|Base address of y<sub>in</sub>[2]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_1_BASE_ADDR_CTX0<hwce_Y_IN_1_BASE_ADDR_CTX0>`              |   356|   32|Base address of y<sub>in</sub>[1]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_0_BASE_ADDR_CTX0<hwce_Y_IN_0_BASE_ADDR_CTX0>`              |   360|   32|Base address of y<sub>in</sub>[0]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_TRANS_SIZE_CTX0<hwce_X_TRANS_SIZE_CTX0>`                      |   364|   32|Total number of words to be read for x<sub>in</sub>                     |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_LINE_STRIDE_LENGTH_CTX0<hwce_X_LINE_STRIDE_LENGTH_CTX0>`      |   368|   32|Line stride and length for x<sub>in</sub>                               |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_FEAT_STRIDE_LENGTH_CTX0<hwce_X_FEAT_STRIDE_LENGTH_CTX0>`      |   372|   32|Feature (block) stride and length for x<sub>in</sub>                    |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_IN_BASE_ADDR_CTX0<hwce_X_IN_BASE_ADDR_CTX0>`                  |   376|   32|Base address of x<sub>in</sub>                                          |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`W_BASE_ADDR_CTX0<hwce_W_BASE_ADDR_CTX0>`                        |   380|   32|Base address of W                                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG0_CTX0<hwce_JOB_CONFIG0_CTX0>`                        |   384|   32|Job configuration register 0                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG1_CTX0<hwce_JOB_CONFIG1_CTX0>`                        |   388|   32|Job configuration register 1                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG2_CTX0<hwce_JOB_CONFIG2_CTX0>`                        |   392|   32|Job configuration register 2                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_TRANS_SIZE_CTX1<hwce_Y_TRANS_SIZE_CTX1>`                      |   576|   32|Total number of words to be read for y<sub>in</sub> and y<sub>out</sub> |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_LINE_STRIDE_LENGTH_CTX1<hwce_Y_LINE_STRIDE_LENGTH_CTX1>`      |   580|   32|Line stride and length for y<sub>in</sub> and y<sub>out</sub>           |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_FEAT_STRIDE_LENGTH_CTX1<hwce_Y_FEAT_STRIDE_LENGTH_CTX1>`      |   584|   32|Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>|
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_2_BASE_ADDR_CTX1<hwce_Y_OUT_2_BASE_ADDR_CTX1>`            |   592|   32|Base address of y<sub>out</sub>[2]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_1_BASE_ADDR_CTX1<hwce_Y_OUT_1_BASE_ADDR_CTX1>`            |   596|   32|Base address of y<sub>out</sub>[1]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_OUT_0_BASE_ADDR_CTX1<hwce_Y_OUT_0_BASE_ADDR_CTX1>`            |   600|   32|Base address of y<sub>out</sub>[0]                                      |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_2_BASE_ADDR_CTX1<hwce_Y_IN_2_BASE_ADDR_CTX1>`              |   608|   32|Base address of y<sub>in</sub>[2]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_1_BASE_ADDR_CTX1<hwce_Y_IN_1_BASE_ADDR_CTX1>`              |   612|   32|Base address of y<sub>in</sub>[1]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`Y_IN_0_BASE_ADDR_CTX1<hwce_Y_IN_0_BASE_ADDR_CTX1>`              |   616|   32|Base address of y<sub>in</sub>[0]                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_TRANS_SIZE_CTX1<hwce_X_TRANS_SIZE_CTX1>`                      |   620|   32|Total number of words to be read for x<sub>in</sub>                     |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_LINE_STRIDE_LENGTH_CTX1<hwce_X_LINE_STRIDE_LENGTH_CTX1>`      |   624|   32|Line stride and length for x<sub>in</sub>                               |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_FEAT_STRIDE_LENGTH_CTX1<hwce_X_FEAT_STRIDE_LENGTH_CTX1>`      |   628|   32|Feature (block) stride and length for x<sub>in</sub>                    |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`X_IN_BASE_ADDR_CTX1<hwce_X_IN_BASE_ADDR_CTX1>`                  |   632|   32|Base address of x<sub>in</sub>                                          |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`W_BASE_ADDR_CTX1<hwce_W_BASE_ADDR_CTX1>`                        |   636|   32|Base address of W                                                       |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG0_CTX1<hwce_JOB_CONFIG0_CTX1>`                        |   640|   32|Job configuration register 0                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG1_CTX1<hwce_JOB_CONFIG1_CTX1>`                        |   644|   32|Job configuration register 1                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+
    |:ref:`JOB_CONFIG2_CTX1<hwce_JOB_CONFIG2_CTX1>`                        |   648|   32|Job configuration register 2                                            |
    +----------------------------------------------------------------------+------+-----+------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Trigger the execution of an offloaded job
                #define HWCE_TRIGGER_OFFSET                      0x0
        
                // Acquire the lock to offload job
                #define HWCE_ACQUIRE_OFFSET                      0x4
        
                // Number of concluded jobs since last read
                #define HWCE_FINISHED_JOBS_OFFSET                0x8
        
                // Status of the HWCE
                #define HWCE_STATUS_OFFSET                       0xc
        
                // ID of the currently running job
                #define HWCE_RUNNING_JOB_OFFSET                  0x10
        
                // Reset HWCE to known idle state
                #define HWCE_SOFT_CLEAR_OFFSET                   0x14
        
                // ID of offloader cores
                #define HWCE_OFFLOADER_ID_OFFSET                 0x18
        
                // Software synchronization
                #define HWCE_SW_EVT_OFFSET                       0x1c
        
                // Generic configuration register 0
                #define HWCE_GEN_CONFIG0_OFFSET                  0x20
        
                // Generic configuration register 1
                #define HWCE_GEN_CONFIG1_OFFSET                  0x24
        
                // Generic configuration register 2
                #define HWCE_GEN_CONFIG2_OFFSET                  0x28
        
                // Generic configuration register 3
                #define HWCE_GEN_CONFIG3_OFFSET                  0x2c
        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_ALIASED_OFFSET         0x40
        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_OFFSET 0x44
        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_OFFSET 0x48
        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_OFFSET    0x50
        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_OFFSET    0x54
        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_OFFSET    0x58
        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_OFFSET     0x60
        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_OFFSET     0x64
        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_OFFSET     0x68
        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_ALIASED_OFFSET         0x6c
        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_OFFSET 0x70
        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_OFFSET 0x74
        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_ALIASED_OFFSET       0x78
        
                // Base address of W
                #define HWCE_W_BASE_ADDR_ALIASED_OFFSET          0x7c
        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_ALIASED_OFFSET          0x80
        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_ALIASED_OFFSET          0x84
        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_ALIASED_OFFSET          0x88
        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_CTX0_OFFSET            0x140
        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x144
        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x148
        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_CTX0_OFFSET       0x150
        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_CTX0_OFFSET       0x154
        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_CTX0_OFFSET       0x158
        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_CTX0_OFFSET        0x160
        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_CTX0_OFFSET        0x164
        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_CTX0_OFFSET        0x168
        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_CTX0_OFFSET            0x16c
        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x170
        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x174
        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_CTX0_OFFSET          0x178
        
                // Base address of W
                #define HWCE_W_BASE_ADDR_CTX0_OFFSET             0x17c
        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_CTX0_OFFSET             0x180
        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_CTX0_OFFSET             0x184
        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_CTX0_OFFSET             0x188
        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_CTX1_OFFSET            0x240
        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x244
        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x248
        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_CTX1_OFFSET       0x250
        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_CTX1_OFFSET       0x254
        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_CTX1_OFFSET       0x258
        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_CTX1_OFFSET        0x260
        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_CTX1_OFFSET        0x264
        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_CTX1_OFFSET        0x268
        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_CTX1_OFFSET            0x26c
        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x270
        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x274
        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_CTX1_OFFSET          0x278
        
                // Base address of W
                #define HWCE_W_BASE_ADDR_CTX1_OFFSET             0x27c
        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_CTX1_OFFSET             0x280
        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_CTX1_OFFSET             0x284
        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_CTX1_OFFSET             0x288

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_trigger_get(uint32_t base);
        static inline void hwce_trigger_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_acquire_get(uint32_t base);
        static inline void hwce_acquire_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_finished_jobs_get(uint32_t base);
        static inline void hwce_finished_jobs_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_status_get(uint32_t base);
        static inline void hwce_status_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_running_job_get(uint32_t base);
        static inline void hwce_running_job_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_soft_clear_get(uint32_t base);
        static inline void hwce_soft_clear_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_offloader_id_get(uint32_t base);
        static inline void hwce_offloader_id_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_sw_evt_get(uint32_t base);
        static inline void hwce_sw_evt_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_gen_config0_get(uint32_t base);
        static inline void hwce_gen_config0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_gen_config1_get(uint32_t base);
        static inline void hwce_gen_config1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_gen_config2_get(uint32_t base);
        static inline void hwce_gen_config2_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_gen_config3_get(uint32_t base);
        static inline void hwce_gen_config3_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_trans_size_aliased_get(uint32_t base);
        static inline void hwce_y_trans_size_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_line_stride_length_aliased_get(uint32_t base);
        static inline void hwce_y_line_stride_length_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_feat_stride_length_aliased_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_2_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_1_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_0_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_2_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_1_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_0_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_trans_size_aliased_get(uint32_t base);
        static inline void hwce_x_trans_size_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_line_stride_length_aliased_get(uint32_t base);
        static inline void hwce_x_line_stride_length_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_feat_stride_length_aliased_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_in_base_addr_aliased_get(uint32_t base);
        static inline void hwce_x_in_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_w_base_addr_aliased_get(uint32_t base);
        static inline void hwce_w_base_addr_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config0_aliased_get(uint32_t base);
        static inline void hwce_job_config0_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config1_aliased_get(uint32_t base);
        static inline void hwce_job_config1_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config2_aliased_get(uint32_t base);
        static inline void hwce_job_config2_aliased_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_trans_size_ctx0_get(uint32_t base);
        static inline void hwce_y_trans_size_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_line_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_y_line_stride_length_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_feat_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_2_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_1_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_0_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_2_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_1_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_0_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_trans_size_ctx0_get(uint32_t base);
        static inline void hwce_x_trans_size_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_line_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_x_line_stride_length_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_feat_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_in_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_x_in_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_w_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_w_base_addr_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config0_ctx0_get(uint32_t base);
        static inline void hwce_job_config0_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config1_ctx0_get(uint32_t base);
        static inline void hwce_job_config1_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config2_ctx0_get(uint32_t base);
        static inline void hwce_job_config2_ctx0_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_trans_size_ctx1_get(uint32_t base);
        static inline void hwce_y_trans_size_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_line_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_y_line_stride_length_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_feat_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_2_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_1_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_out_0_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_2_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_1_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_y_in_0_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_trans_size_ctx1_get(uint32_t base);
        static inline void hwce_x_trans_size_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_line_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_x_line_stride_length_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_feat_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_x_in_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_x_in_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_w_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_w_base_addr_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config0_ctx1_get(uint32_t base);
        static inline void hwce_job_config0_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config1_ctx1_get(uint32_t base);
        static inline void hwce_job_config1_ctx1_set(uint32_t base, uint32_t value);

        static inline uint32_t hwce_job_config2_ctx1_get(uint32_t base);
        static inline void hwce_job_config2_ctx1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue. (access: W)
        #define HWCE_TRIGGER_ANY_BIT                                         0
        #define HWCE_TRIGGER_ANY_WIDTH                                       32
        #define HWCE_TRIGGER_ANY_MASK                                        0xffffffff
        #define HWCE_TRIGGER_ANY_RESET                                       0x0
        
        // If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code (access: R)
        #define HWCE_ACQUIRE_ID_ERR_BIT                                      0
        #define HWCE_ACQUIRE_ID_ERR_WIDTH                                    8
        #define HWCE_ACQUIRE_ID_ERR_MASK                                     0xff
        #define HWCE_ACQUIRE_ID_ERR_RESET                                    0x0
        
        // An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1) (access: R)
        #define HWCE_ACQUIRE_ERR_BIT                                         8
        #define HWCE_ACQUIRE_ERR_WIDTH                                       24
        #define HWCE_ACQUIRE_ERR_MASK                                        0xffffff00
        #define HWCE_ACQUIRE_ERR_RESET                                       0x0
        
        // The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access (access: R)
        #define HWCE_FINISHED_JOBS_JOBS_BIT                                  0
        #define HWCE_FINISHED_JOBS_JOBS_WIDTH                                32
        #define HWCE_FINISHED_JOBS_JOBS_MASK                                 0xffffffff
        #define HWCE_FINISHED_JOBS_JOBS_RESET                                0x0
        
        // Status of the HWCE - 0x0 Not running a job - 0x1 Running a job (access: R)
        #define HWCE_STATUS_ST_BIT                                           0
        #define HWCE_STATUS_ST_WIDTH                                         1
        #define HWCE_STATUS_ST_MASK                                          0x1
        #define HWCE_STATUS_ST_RESET                                         0x0
        
        // ID of the currently running job (access: R)
        #define HWCE_RUNNING_JOB_ID_BIT                                      0
        #define HWCE_RUNNING_JOB_ID_WIDTH                                    8
        #define HWCE_RUNNING_JOB_ID_MASK                                     0xff
        #define HWCE_RUNNING_JOB_ID_RESET                                    0x0
        
        // A write of any value to this register will reset the HWCE to its idle state. (access: W)
        #define HWCE_SOFT_CLEAR_ANY_BIT                                      0
        #define HWCE_SOFT_CLEAR_ANY_WIDTH                                    32
        #define HWCE_SOFT_CLEAR_ANY_MASK                                     0xffffffff
        #define HWCE_SOFT_CLEAR_ANY_RESET                                    0x0
        
        // N-th is the ID+1 of the offloader core for the N-th job (access: R)
        #define HWCE_OFFLOADER_ID_ID_BIT                                     0
        #define HWCE_OFFLOADER_ID_ID_WIDTH                                   32
        #define HWCE_OFFLOADER_ID_ID_MASK                                    0xffffffff
        #define HWCE_OFFLOADER_ID_ID_RESET                                   0x0
        
        // Any write triggers a software event that the HWCE can wait on, depending on the WAIT_FEAT value set in GEN_CONFIG2 (access: W)
        #define HWCE_SW_EVT_EVT_BIT                                          0
        #define HWCE_SW_EVT_EVT_WIDTH                                        32
        #define HWCE_SW_EVT_EVT_MASK                                         0xffffffff
        #define HWCE_SW_EVT_EVT_RESET                                        0x0
        
        // Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage. (access: R/W)
        #define HWCE_GEN_CONFIG0_QF_BIT                                      0
        #define HWCE_GEN_CONFIG0_QF_WIDTH                                    6
        #define HWCE_GEN_CONFIG0_QF_MASK                                     0x3f
        #define HWCE_GEN_CONFIG0_QF_RESET                                    0xc
        
        // Operation type: - 0x0 - By-the-book 2D convolution (with weight flipping) - 0x1 - Replace by-the-book 2D convolution with 2D cross-correlation (without weight flipping) (access: R/W)
        #define HWCE_GEN_CONFIG0_NF_BIT                                      6
        #define HWCE_GEN_CONFIG0_NF_WIDTH                                    1
        #define HWCE_GEN_CONFIG0_NF_MASK                                     0x40
        #define HWCE_GEN_CONFIG0_NF_RESET                                    0x1
        
        // No y<sub>in</sub> mode: - 0x0 - Normal operation - 0x1 - Disable loading of y<sub>in</sub> and replace it with a constant bias set in the CONFIG2 register (access: R/W)
        #define HWCE_GEN_CONFIG0_NY_BIT                                      7
        #define HWCE_GEN_CONFIG0_NY_WIDTH                                    1
        #define HWCE_GEN_CONFIG0_NY_MASK                                     0x80
        #define HWCE_GEN_CONFIG0_NY_RESET                                    0x0
        
        // Convolution mode: - 0x0 - mode is 5x5. - 0x1 - mode is 3x3. - 0x2 - mode is 4x7. (access: R/W)
        #define HWCE_GEN_CONFIG0_CONV_BIT                                    11
        #define HWCE_GEN_CONFIG0_CONV_WIDTH                                  2
        #define HWCE_GEN_CONFIG0_CONV_MASK                                   0x1800
        #define HWCE_GEN_CONFIG0_CONV_RESET                                  0x0
        
        // No job copy: - 0x0 - do job copy - 0x1 - don't do job copy (access: R/W)
        #define HWCE_GEN_CONFIG0_NCP_BIT                                     13
        #define HWCE_GEN_CONFIG0_NCP_WIDTH                                   1
        #define HWCE_GEN_CONFIG0_NCP_MASK                                    0x2000
        #define HWCE_GEN_CONFIG0_NCP_RESET                                   0x0
        
        // Rounding: - 0x0 - perform only normalization (right-shift by QF) at the end of sum-of-products. - 0x1 - perform rounding (sum with 1 left-shifted by (QF-1) ) before normalization at the end of sum-of-products. (access: R/W)
        #define HWCE_GEN_CONFIG0_RND_BIT                                     14
        #define HWCE_GEN_CONFIG0_RND_WIDTH                                   1
        #define HWCE_GEN_CONFIG0_RND_MASK                                    0x4000
        #define HWCE_GEN_CONFIG0_RND_RESET                                   0x0
        
        // Stride in bytes between one loaded filter and the next. If multiple filters are used in 3x3 mode, they are assumed to be contiguous and the stride is applied to the block of 2 / 3 of them. (access: R/W)
        #define HWCE_GEN_CONFIG0_WSTRIDE_BIT                                 16
        #define HWCE_GEN_CONFIG0_WSTRIDE_WIDTH                               16
        #define HWCE_GEN_CONFIG0_WSTRIDE_MASK                                0xffff0000
        #define HWCE_GEN_CONFIG0_WSTRIDE_RESET                               0x0
        
        // Stride in bytes between quantization thresholds for strongly quantized modes (2bit / 4bit for weights, 4bit for activations). (access: R/W)
        #define HWCE_GEN_CONFIG1_THSTRIDE_BIT                                0
        #define HWCE_GEN_CONFIG1_THSTRIDE_WIDTH                              16
        #define HWCE_GEN_CONFIG1_THSTRIDE_MASK                               0xffff
        #define HWCE_GEN_CONFIG1_THSTRIDE_RESET                              0x0
        
        // Quantization shift for y<sub>in</sub> and y<sub>out</sub> streams, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG1_QSHIFTY_BIT                                 16
        #define HWCE_GEN_CONFIG1_QSHIFTY_WIDTH                               5
        #define HWCE_GEN_CONFIG1_QSHIFTY_MASK                                0x1f0000
        #define HWCE_GEN_CONFIG1_QSHIFTY_RESET                               0x0
        
        // Quantization mode for y<sub>in</sub> and y<sub>out</sub> streams: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG1_QMODEY_BIT                                  21
        #define HWCE_GEN_CONFIG1_QMODEY_WIDTH                                3
        #define HWCE_GEN_CONFIG1_QMODEY_MASK                                 0xe00000
        #define HWCE_GEN_CONFIG1_QMODEY_RESET                                0x0
        
        // Quantization shift for x stream, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG1_QSHIFTX_BIT                                 24
        #define HWCE_GEN_CONFIG1_QSHIFTX_WIDTH                               5
        #define HWCE_GEN_CONFIG1_QSHIFTX_MASK                                0x1f000000
        #define HWCE_GEN_CONFIG1_QSHIFTX_RESET                               0x0
        
        // Quantization mode for x stream: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG1_QMODEX_BIT                                  29
        #define HWCE_GEN_CONFIG1_QMODEX_WIDTH                                3
        #define HWCE_GEN_CONFIG1_QMODEX_MASK                                 0xe0000000
        #define HWCE_GEN_CONFIG1_QMODEX_RESET                                0x0
        
        // If set to 1, perform rectification (ReLU) before final quantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_RECT_BIT                                    14
        #define HWCE_GEN_CONFIG2_RECT_WIDTH                                  1
        #define HWCE_GEN_CONFIG2_RECT_MASK                                   0x4000
        #define HWCE_GEN_CONFIG2_RECT_RESET                                  0x0
        
        // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_NORM_BIT                                    15
        #define HWCE_GEN_CONFIG2_NORM_WIDTH                                  1
        #define HWCE_GEN_CONFIG2_NORM_MASK                                   0x8000
        #define HWCE_GEN_CONFIG2_NORM_RESET                                  0x0
        
        // Number of features to process before waiting for software synchronization by  SW_EVT register. (access: R/W)
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_BIT                              16
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_WIDTH                            8
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_MASK                             0xff0000
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_RESET                            0x0
        
        // Quantization shift for weights, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_QSHIFTW_BIT                                 24
        #define HWCE_GEN_CONFIG2_QSHIFTW_WIDTH                               5
        #define HWCE_GEN_CONFIG2_QSHIFTW_MASK                                0x1f000000
        #define HWCE_GEN_CONFIG2_QSHIFTW_RESET                               0x0
        
        // Quantization mode for weights: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization - 0x2 - 2bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG2_QMODEW_BIT                                  29
        #define HWCE_GEN_CONFIG2_QMODEW_WIDTH                                3
        #define HWCE_GEN_CONFIG2_QMODEW_MASK                                 0xe0000000
        #define HWCE_GEN_CONFIG2_QMODEW_RESET                                0x0
        
        // Quantization offset for weights, used to displace them before left-shift. (access: R/W)
        #define HWCE_GEN_CONFIG3_QOFFSW_BIT                                  0
        #define HWCE_GEN_CONFIG3_QOFFSW_WIDTH                                16
        #define HWCE_GEN_CONFIG3_QOFFSW_MASK                                 0xffff
        #define HWCE_GEN_CONFIG3_QOFFSW_RESET                                0x0
        
        // Quantization offset for activations, used to displace them before left-shift. (access: R/W)
        #define HWCE_GEN_CONFIG3_QOFFSX_BIT                                  16
        #define HWCE_GEN_CONFIG3_QOFFSX_WIDTH                                16
        #define HWCE_GEN_CONFIG3_QOFFSX_MASK                                 0xffff0000
        #define HWCE_GEN_CONFIG3_QOFFSX_RESET                                0x0
        
        // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams. (access: R/W)
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_BIT                           0
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_WIDTH                         32
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_MASK                          0xffffffff
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_RESET                         0x0
        
        // Ceil(line length) of y<sub>in</sub> / y<sub>out</sub> in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related YLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer. (access: R/W)
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive lines within a channel (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0
        
        // Height of a channel (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive channels (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0
        
        // Pointer into cluster L1 memory for third y<sub>out</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_RESET                    0x0
        
        // Pointer into cluster L1 memory for second y<sub>out</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_RESET                    0x0
        
        // Pointer into cluster L1 memory for first y<sub>out</sub> stream (all modes) (access: R/W)
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_RESET                    0x0
        
        // Pointer into cluster L1 memory for third y<sub>in</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_RESET                     0x0
        
        // Pointer into cluster L1 memory for second y<sub>in</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_RESET                     0x0
        
        // Pointer into cluster L1 memory for first y<sub>in</sub> stream (all modes) (access: R/W)
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_RESET                     0x0
        
        // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x  stream. (access: R/W)
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_BIT                           0
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_WIDTH                         32
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_MASK                          0xffffffff
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_RESET                         0x0
        
        // Ceil(line length) of x in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related XLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer. (access: R/W)
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive lines within a channel (x). (access: R/W)
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0
        
        // Height of a channel (x). (access: R/W)
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive channels (x). (access: R/W)
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0
        
        // Pointer into cluster L1 memory for x stream. (access: R/W)
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_BIT                         0
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_WIDTH                       32
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_MASK                        0xffffffff
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_RESET                       0x0
        
        // Pointer into cluster L1 memory for weights. (access: R/W)
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_BIT                            0
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_WIDTH                          32
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_MASK                           0xffffffff
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_RESET                          0x0
        
        // Linebuffer virtual length. Acceptable values range between 2 and LINEBUF_LENGTH. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_BIT                         0
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_WIDTH                       8
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_MASK                        0xff
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_RESET                       0x0
        
        // Low index of column range to be remove from linebuffer at line change. Set to: - 0x2 - for 3x3 and 5x5 modes - 0x1 - for 4x7 mode (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_BIT                      8
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_WIDTH                    4
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_MASK                     0xf00
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_RESET                    0x0
        
        // High index of column range to be remove from linebuffer at line change. Set to 3 for all modes. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_BIT                      12
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_WIDTH                    4
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_MASK                     0xf000
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_RESET                    0x0
        
        // Constant to sum instead of y<sub>in</sub> if the NY flag is active in the GEN_CONFIG1 register. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_BIT                        16
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_WIDTH                      16
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_MASK                       0xffff0000
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_RESET                      0x0
        
        // Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using multiple 3x3 streams, The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, and bit 1 vector 0. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_BIT               0
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_WIDTH             4
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_MASK              0xf
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_RESET             0x0
        
        // Output channels counter wrap parameter. If LO=0, the counter of output channels will be updated every WOF inner iterations. If LO=1, the counter of output channels will be updated every inner iteration and reset every WOF  iterations. If both WIF and WOF are 0, the looping mechanism is disabled. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_BIT                       8
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_WIDTH                     6
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_MASK                      0x3f00
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_RESET                     0x0
        
        // Input channels counter wrap parameter. If LO=0, the counter of input channels will be updated every inner iteration and reset every WIF iterations. If LO=1, the counter of input channels will be updated every WIF iterations. IIf both WIF and WOF are 0, the looping mechanism is disabled. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_BIT                       16
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_WIDTH                     6
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_MASK                      0x3f0000
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_RESET                     0x0
        
        // If set to 1, perform rectification (ReLU) before final quantization. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_BIT                            22
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_WIDTH                          1
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_MASK                           0x400000
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_RESET                          0x0
        
        // Loop order: - 0x0 - output features (OF) are the outer loop - 0x1 - input features (IF) are the outer loop (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_LO_BIT                              23
        #define HWCE_JOB_CONFIG1_ALIASED_LO_WIDTH                            1
        #define HWCE_JOB_CONFIG1_ALIASED_LO_MASK                             0x800000
        #define HWCE_JOB_CONFIG1_ALIASED_LO_RESET                            0x0
        
        // Total number of output channels computed in the HWCE loop. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_BIT                        24
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_WIDTH                      8
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_MASK                       0xff000000
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_RESET                      0x0
        
        // Counter used internally to acknowledge the end of the line buffer preloading phase. Set to: - 2*LBUFLEN+4 - in 3x3 mode - 4*LBUFLEN+4 - in 5x5 mode - 3*LBUFLEN+4 - in 4x7 mode (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_BIT                      0
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_WIDTH                    8
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_MASK                     0xff
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_RESET                    0x0
        
        // Length in number of pixels of y<sub>in</sub> / y<sub>out</sub>, as used by the internal datapath. Can be shorter than the length programmed in the Y_LINE_STRIDE_LENGTH register to guarantee that the correct number of inputs/outputs is consumed/produced while respecting alignment contraints. Ignored if 0x0. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_BIT                            8
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_WIDTH                          8
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_MASK                           0xff00
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_RESET                          0x0
        
        // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_BIT                            17
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_WIDTH                          1
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_MASK                           0x20000
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_RESET                          0x0
        
        // Remainder of Y_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_BIT                           18
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_WIDTH                         2
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_MASK                          0xc0000
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_RESET                         0x0
        
        // Remainder of X_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_BIT                           20
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_WIDTH                         2
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_MASK                          0x300000
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_RESET                         0x0
        
        // If set to 1, perform 2x2 striding on y<sub>in</sub> and y<sub>out</sub>. Note that this mode still requires loading all pixels in the linebuffer. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_S2_BIT                              22
        #define HWCE_JOB_CONFIG2_ALIASED_S2_WIDTH                            1
        #define HWCE_JOB_CONFIG2_ALIASED_S2_MASK                             0x400000
        #define HWCE_JOB_CONFIG2_ALIASED_S2_RESET                            0x0
        
        // Enable the bypass buffer. This means that when BYNFEAT > 1, the y<sub>out</sub> / y<sub>in</sub> feedback loop is closed internally in the HWCE keeping always full precision (16 bits). (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_BIT                             23
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_WIDTH                           1
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_MASK                            0x800000
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_RESET                           0x0
        
        // Number of channels to loop upon inside the bypass buffer. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_BIT                         24
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_WIDTH                       8
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_MASK                        0xff000000
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_TRIGGER_ANY_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define HWCE_TRIGGER_ANY_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define HWCE_TRIGGER_ANY_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define HWCE_TRIGGER_ANY(val)                              ((val) << 0)
        
        #define HWCE_ACQUIRE_ID_ERR_GET(value)                     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_ACQUIRE_ID_ERR_GETS(value)                    (GAP_BEXTRACT((value),8,0))
        #define HWCE_ACQUIRE_ID_ERR_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
        #define HWCE_ACQUIRE_ID_ERR(val)                           ((val) << 0)
        
        #define HWCE_ACQUIRE_ERR_GET(value)                        (GAP_BEXTRACTU((value),24,8))
        #define HWCE_ACQUIRE_ERR_GETS(value)                       (GAP_BEXTRACT((value),24,8))
        #define HWCE_ACQUIRE_ERR_SET(value,field)                  (GAP_BINSERT((value),(field),24,8))
        #define HWCE_ACQUIRE_ERR(val)                              ((val) << 8)
        
        #define HWCE_FINISHED_JOBS_JOBS_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define HWCE_FINISHED_JOBS_JOBS_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define HWCE_FINISHED_JOBS_JOBS_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define HWCE_FINISHED_JOBS_JOBS(val)                       ((val) << 0)
        
        #define HWCE_STATUS_ST_GET(value)                          (GAP_BEXTRACTU((value),1,0))
        #define HWCE_STATUS_ST_GETS(value)                         (GAP_BEXTRACT((value),1,0))
        #define HWCE_STATUS_ST_SET(value,field)                    (GAP_BINSERT((value),(field),1,0))
        #define HWCE_STATUS_ST(val)                                ((val) << 0)
        
        #define HWCE_RUNNING_JOB_ID_GET(value)                     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_RUNNING_JOB_ID_GETS(value)                    (GAP_BEXTRACT((value),8,0))
        #define HWCE_RUNNING_JOB_ID_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
        #define HWCE_RUNNING_JOB_ID(val)                           ((val) << 0)
        
        #define HWCE_SOFT_CLEAR_ANY_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_SOFT_CLEAR_ANY_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define HWCE_SOFT_CLEAR_ANY_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define HWCE_SOFT_CLEAR_ANY(val)                           ((val) << 0)
        
        #define HWCE_OFFLOADER_ID_ID_GET(value)                    (GAP_BEXTRACTU((value),32,0))
        #define HWCE_OFFLOADER_ID_ID_GETS(value)                   (GAP_BEXTRACT((value),32,0))
        #define HWCE_OFFLOADER_ID_ID_SET(value,field)              (GAP_BINSERT((value),(field),32,0))
        #define HWCE_OFFLOADER_ID_ID(val)                          ((val) << 0)
        
        #define HWCE_SW_EVT_EVT_GET(value)                         (GAP_BEXTRACTU((value),32,0))
        #define HWCE_SW_EVT_EVT_GETS(value)                        (GAP_BEXTRACT((value),32,0))
        #define HWCE_SW_EVT_EVT_SET(value,field)                   (GAP_BINSERT((value),(field),32,0))
        #define HWCE_SW_EVT_EVT(val)                               ((val) << 0)
        
        #define HWCE_GEN_CONFIG0_QF_GET(value)                     (GAP_BEXTRACTU((value),6,0))
        #define HWCE_GEN_CONFIG0_QF_GETS(value)                    (GAP_BEXTRACT((value),6,0))
        #define HWCE_GEN_CONFIG0_QF_SET(value,field)               (GAP_BINSERT((value),(field),6,0))
        #define HWCE_GEN_CONFIG0_QF(val)                           ((val) << 0)
        
        #define HWCE_GEN_CONFIG0_NF_GET(value)                     (GAP_BEXTRACTU((value),1,6))
        #define HWCE_GEN_CONFIG0_NF_GETS(value)                    (GAP_BEXTRACT((value),1,6))
        #define HWCE_GEN_CONFIG0_NF_SET(value,field)               (GAP_BINSERT((value),(field),1,6))
        #define HWCE_GEN_CONFIG0_NF(val)                           ((val) << 6)
        
        #define HWCE_GEN_CONFIG0_NY_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define HWCE_GEN_CONFIG0_NY_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define HWCE_GEN_CONFIG0_NY_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define HWCE_GEN_CONFIG0_NY(val)                           ((val) << 7)
        
        #define HWCE_GEN_CONFIG0_CONV_GET(value)                   (GAP_BEXTRACTU((value),2,11))
        #define HWCE_GEN_CONFIG0_CONV_GETS(value)                  (GAP_BEXTRACT((value),2,11))
        #define HWCE_GEN_CONFIG0_CONV_SET(value,field)             (GAP_BINSERT((value),(field),2,11))
        #define HWCE_GEN_CONFIG0_CONV(val)                         ((val) << 11)
        
        #define HWCE_GEN_CONFIG0_NCP_GET(value)                    (GAP_BEXTRACTU((value),1,13))
        #define HWCE_GEN_CONFIG0_NCP_GETS(value)                   (GAP_BEXTRACT((value),1,13))
        #define HWCE_GEN_CONFIG0_NCP_SET(value,field)              (GAP_BINSERT((value),(field),1,13))
        #define HWCE_GEN_CONFIG0_NCP(val)                          ((val) << 13)
        
        #define HWCE_GEN_CONFIG0_RND_GET(value)                    (GAP_BEXTRACTU((value),1,14))
        #define HWCE_GEN_CONFIG0_RND_GETS(value)                   (GAP_BEXTRACT((value),1,14))
        #define HWCE_GEN_CONFIG0_RND_SET(value,field)              (GAP_BINSERT((value),(field),1,14))
        #define HWCE_GEN_CONFIG0_RND(val)                          ((val) << 14)
        
        #define HWCE_GEN_CONFIG0_WSTRIDE_GET(value)                (GAP_BEXTRACTU((value),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE_GETS(value)               (GAP_BEXTRACT((value),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE_SET(value,field)          (GAP_BINSERT((value),(field),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE(val)                      ((val) << 16)
        
        #define HWCE_GEN_CONFIG1_THSTRIDE_GET(value)               (GAP_BEXTRACTU((value),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE_GETS(value)              (GAP_BEXTRACT((value),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE_SET(value,field)         (GAP_BINSERT((value),(field),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE(val)                     ((val) << 0)
        
        #define HWCE_GEN_CONFIG1_QSHIFTY_GET(value)                (GAP_BEXTRACTU((value),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY_GETS(value)               (GAP_BEXTRACT((value),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY_SET(value,field)          (GAP_BINSERT((value),(field),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY(val)                      ((val) << 16)
        
        #define HWCE_GEN_CONFIG1_QMODEY_GET(value)                 (GAP_BEXTRACTU((value),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY_GETS(value)                (GAP_BEXTRACT((value),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY_SET(value,field)           (GAP_BINSERT((value),(field),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY(val)                       ((val) << 21)
        
        #define HWCE_GEN_CONFIG1_QSHIFTX_GET(value)                (GAP_BEXTRACTU((value),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX_GETS(value)               (GAP_BEXTRACT((value),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX_SET(value,field)          (GAP_BINSERT((value),(field),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX(val)                      ((val) << 24)
        
        #define HWCE_GEN_CONFIG1_QMODEX_GET(value)                 (GAP_BEXTRACTU((value),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX_GETS(value)                (GAP_BEXTRACT((value),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX_SET(value,field)           (GAP_BINSERT((value),(field),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX(val)                       ((val) << 29)
        
        #define HWCE_GEN_CONFIG2_RECT_GET(value)                   (GAP_BEXTRACTU((value),1,14))
        #define HWCE_GEN_CONFIG2_RECT_GETS(value)                  (GAP_BEXTRACT((value),1,14))
        #define HWCE_GEN_CONFIG2_RECT_SET(value,field)             (GAP_BINSERT((value),(field),1,14))
        #define HWCE_GEN_CONFIG2_RECT(val)                         ((val) << 14)
        
        #define HWCE_GEN_CONFIG2_NORM_GET(value)                   (GAP_BEXTRACTU((value),1,15))
        #define HWCE_GEN_CONFIG2_NORM_GETS(value)                  (GAP_BEXTRACT((value),1,15))
        #define HWCE_GEN_CONFIG2_NORM_SET(value,field)             (GAP_BINSERT((value),(field),1,15))
        #define HWCE_GEN_CONFIG2_NORM(val)                         ((val) << 15)
        
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_GET(value)             (GAP_BEXTRACTU((value),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_GETS(value)            (GAP_BEXTRACT((value),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_SET(value,field)       (GAP_BINSERT((value),(field),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT(val)                   ((val) << 16)
        
        #define HWCE_GEN_CONFIG2_QSHIFTW_GET(value)                (GAP_BEXTRACTU((value),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW_GETS(value)               (GAP_BEXTRACT((value),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW_SET(value,field)          (GAP_BINSERT((value),(field),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW(val)                      ((val) << 24)
        
        #define HWCE_GEN_CONFIG2_QMODEW_GET(value)                 (GAP_BEXTRACTU((value),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW_GETS(value)                (GAP_BEXTRACT((value),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW_SET(value,field)           (GAP_BINSERT((value),(field),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW(val)                       ((val) << 29)
        
        #define HWCE_GEN_CONFIG3_QOFFSW_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW(val)                       ((val) << 0)
        
        #define HWCE_GEN_CONFIG3_QOFFSX_GET(value)                 (GAP_BEXTRACTU((value),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX_GETS(value)                (GAP_BEXTRACT((value),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX_SET(value,field)           (GAP_BINSERT((value),(field),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX(val)                       ((val) << 16)
        
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE(val)                ((val) << 0)
        
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)
        
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)
        
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)
        
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)
        
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)
        
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)
        
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)
        
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)
        
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE(val)                ((val) << 0)
        
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)
        
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)
        
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_GET(value)        (GAP_BEXTRACTU((value),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_GETS(value)       (GAP_BEXTRACT((value),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_SET(value,field)  (GAP_BINSERT((value),(field),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR(val)              ((val) << 0)
        
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR(val)                 ((val) << 0)
        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_GET(value)        (GAP_BEXTRACTU((value),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_GETS(value)       (GAP_BEXTRACT((value),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_SET(value,field)  (GAP_BINSERT((value),(field),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN(val)              ((val) << 0)
        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO(val)           ((val) << 8)
        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_GET(value)     (GAP_BEXTRACTU((value),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_GETS(value)    (GAP_BEXTRACT((value),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_SET(value,field) (GAP_BINSERT((value),(field),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI(val)           ((val) << 12)
        
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_GET(value)       (GAP_BEXTRACTU((value),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_GETS(value)      (GAP_BEXTRACT((value),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST(val)             ((val) << 16)
        
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK(val)    ((val) << 0)
        
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_GET(value)      (GAP_BEXTRACTU((value),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_GETS(value)     (GAP_BEXTRACT((value),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_SET(value,field) (GAP_BINSERT((value),(field),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM(val)            ((val) << 8)
        
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_GET(value)      (GAP_BEXTRACTU((value),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_GETS(value)     (GAP_BEXTRACT((value),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_SET(value,field) (GAP_BINSERT((value),(field),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM(val)            ((val) << 16)
        
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT(val)                 ((val) << 22)
        
        #define HWCE_JOB_CONFIG1_ALIASED_LO_GET(value)             (GAP_BEXTRACTU((value),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO_GETS(value)            (GAP_BEXTRACT((value),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO_SET(value,field)       (GAP_BINSERT((value),(field),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO(val)                   ((val) << 23)
        
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_GET(value)       (GAP_BEXTRACTU((value),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_GETS(value)      (GAP_BEXTRACT((value),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT(val)             ((val) << 24)
        
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_GET(value)     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_GETS(value)    (GAP_BEXTRACT((value),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT(val)           ((val) << 0)
        
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN(val)                 ((val) << 8)
        
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_GET(value)           (GAP_BEXTRACTU((value),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_GETS(value)          (GAP_BEXTRACT((value),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_SET(value,field)     (GAP_BINSERT((value),(field),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM(val)                 ((val) << 17)
        
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM(val)                ((val) << 18)
        
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM(val)                ((val) << 20)
        
        #define HWCE_JOB_CONFIG2_ALIASED_S2_GET(value)             (GAP_BEXTRACTU((value),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2_GETS(value)            (GAP_BEXTRACT((value),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2_SET(value,field)       (GAP_BINSERT((value),(field),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2(val)                   ((val) << 22)
        
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_GET(value)            (GAP_BEXTRACTU((value),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_GETS(value)           (GAP_BEXTRACT((value),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_SET(value,field)      (GAP_BINSERT((value),(field),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE(val)                  ((val) << 23)
        
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_GET(value)        (GAP_BEXTRACTU((value),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_GETS(value)       (GAP_BEXTRACT((value),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_SET(value,field)  (GAP_BINSERT((value),(field),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT(val)              ((val) << 24)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** HWCE_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t trigger;  // Trigger the execution of an offloaded job
            volatile uint32_t acquire;  // Acquire the lock to offload job
            volatile uint32_t finished_jobs;  // Number of concluded jobs since last read
            volatile uint32_t status;  // Status of the HWCE
            volatile uint32_t running_job;  // ID of the currently running job
            volatile uint32_t soft_clear;  // Reset HWCE to known idle state
            volatile uint32_t offloader_id;  // ID of offloader cores
            volatile uint32_t sw_evt;  // Software synchronization
            volatile uint32_t gen_config0;  // Generic configuration register 0
            volatile uint32_t gen_config1;  // Generic configuration register 1
            volatile uint32_t gen_config2;  // Generic configuration register 2
            volatile uint32_t gen_config3;  // Generic configuration register 3
            volatile uint32_t reserved_0[4];  // Reserved/Not used.
            volatile uint32_t y_trans_size_aliased;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_line_stride_length_aliased;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_feat_stride_length_aliased;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t reserved_1[1];  // Reserved/Not used.
            volatile uint32_t y_out_2_base_addr_aliased;  // Base address of y<sub>out</sub>[2]
            volatile uint32_t y_out_1_base_addr_aliased;  // Base address of y<sub>out</sub>[1]
            volatile uint32_t y_out_0_base_addr_aliased;  // Base address of y<sub>out</sub>[0]
            volatile uint32_t reserved_2[1];  // Reserved/Not used.
            volatile uint32_t y_in_2_base_addr_aliased;  // Base address of y<sub>in</sub>[2]
            volatile uint32_t y_in_1_base_addr_aliased;  // Base address of y<sub>in</sub>[1]
            volatile uint32_t y_in_0_base_addr_aliased;  // Base address of y<sub>in</sub>[0]
            volatile uint32_t x_trans_size_aliased;  // Total number of words to be read for x<sub>in</sub>
            volatile uint32_t x_line_stride_length_aliased;  // Line stride and length for x<sub>in</sub>
            volatile uint32_t x_feat_stride_length_aliased;  // Feature (block) stride and length for x<sub>in</sub>
            volatile uint32_t x_in_base_addr_aliased;  // Base address of x<sub>in</sub>
            volatile uint32_t w_base_addr_aliased;  // Base address of W
            volatile uint32_t job_config0_aliased;  // Job configuration register 0
            volatile uint32_t job_config1_aliased;  // Job configuration register 1
            volatile uint32_t job_config2_aliased;  // Job configuration register 2
            volatile uint32_t reserved_3[45];  // Reserved/Not used.
            volatile uint32_t y_trans_size_ctx0;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_line_stride_length_ctx0;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_feat_stride_length_ctx0;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t reserved_4[1];  // Reserved/Not used.
            volatile uint32_t y_out_2_base_addr_ctx0;  // Base address of y<sub>out</sub>[2]
            volatile uint32_t y_out_1_base_addr_ctx0;  // Base address of y<sub>out</sub>[1]
            volatile uint32_t y_out_0_base_addr_ctx0;  // Base address of y<sub>out</sub>[0]
            volatile uint32_t reserved_5[1];  // Reserved/Not used.
            volatile uint32_t y_in_2_base_addr_ctx0;  // Base address of y<sub>in</sub>[2]
            volatile uint32_t y_in_1_base_addr_ctx0;  // Base address of y<sub>in</sub>[1]
            volatile uint32_t y_in_0_base_addr_ctx0;  // Base address of y<sub>in</sub>[0]
            volatile uint32_t x_trans_size_ctx0;  // Total number of words to be read for x<sub>in</sub>
            volatile uint32_t x_line_stride_length_ctx0;  // Line stride and length for x<sub>in</sub>
            volatile uint32_t x_feat_stride_length_ctx0;  // Feature (block) stride and length for x<sub>in</sub>
            volatile uint32_t x_in_base_addr_ctx0;  // Base address of x<sub>in</sub>
            volatile uint32_t w_base_addr_ctx0;  // Base address of W
            volatile uint32_t job_config0_ctx0;  // Job configuration register 0
            volatile uint32_t job_config1_ctx0;  // Job configuration register 1
            volatile uint32_t job_config2_ctx0;  // Job configuration register 2
            volatile uint32_t reserved_6[45];  // Reserved/Not used.
            volatile uint32_t y_trans_size_ctx1;  // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_line_stride_length_ctx1;  // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t y_feat_stride_length_ctx1;  // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
            volatile uint32_t reserved_7[1];  // Reserved/Not used.
            volatile uint32_t y_out_2_base_addr_ctx1;  // Base address of y<sub>out</sub>[2]
            volatile uint32_t y_out_1_base_addr_ctx1;  // Base address of y<sub>out</sub>[1]
            volatile uint32_t y_out_0_base_addr_ctx1;  // Base address of y<sub>out</sub>[0]
            volatile uint32_t reserved_8[1];  // Reserved/Not used.
            volatile uint32_t y_in_2_base_addr_ctx1;  // Base address of y<sub>in</sub>[2]
            volatile uint32_t y_in_1_base_addr_ctx1;  // Base address of y<sub>in</sub>[1]
            volatile uint32_t y_in_0_base_addr_ctx1;  // Base address of y<sub>in</sub>[0]
            volatile uint32_t x_trans_size_ctx1;  // Total number of words to be read for x<sub>in</sub>
            volatile uint32_t x_line_stride_length_ctx1;  // Line stride and length for x<sub>in</sub>
            volatile uint32_t x_feat_stride_length_ctx1;  // Feature (block) stride and length for x<sub>in</sub>
            volatile uint32_t x_in_base_addr_ctx1;  // Base address of x<sub>in</sub>
            volatile uint32_t w_base_addr_ctx1;  // Base address of W
            volatile uint32_t job_config0_ctx1;  // Job configuration register 0
            volatile uint32_t job_config1_ctx1;  // Job configuration register 1
            volatile uint32_t job_config2_ctx1;  // Job configuration register 2
        } __attribute__((packed)) hwce_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int any             :32; // Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_trigger_t;
        
        typedef union {
          struct {
            unsigned int id_err          :8 ; // If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code
            unsigned int err             :24; // An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_acquire_t;
        
        typedef union {
          struct {
            unsigned int jobs            :32; // The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_finished_jobs_t;
        
        typedef union {
          struct {
            unsigned int st              :1 ; // Status of the HWCE - 0x0 Not running a job - 0x1 Running a job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_status_t;
        
        typedef union {
          struct {
            unsigned int id              :8 ; // ID of the currently running job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_running_job_t;
        
        typedef union {
          struct {
            unsigned int any             :32; // A write of any value to this register will reset the HWCE to its idle state.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_soft_clear_t;
        
        typedef union {
          struct {
            unsigned int id              :32; // N-th is the ID+1 of the offloader core for the N-th job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_offloader_id_t;
        
        typedef union {
          struct {
            unsigned int evt             :32; // Any write triggers a software event that the HWCE can wait on, depending on the WAIT_FEAT value set in GEN_CONFIG2
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_sw_evt_t;
        
        typedef union {
          struct {
            unsigned int qf              :6 ; // Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage.
            unsigned int nf              :1 ; // Operation type: - 0x0 - By-the-book 2D convolution (with weight flipping) - 0x1 - Replace by-the-book 2D convolution with 2D cross-correlation (without weight flipping)
            unsigned int ny              :1 ; // No y<sub>in</sub> mode: - 0x0 - Normal operation - 0x1 - Disable loading of y<sub>in</sub> and replace it with a constant bias set in the CONFIG2 register
            unsigned int padding0:3 ;
            unsigned int conv            :2 ; // Convolution mode: - 0x0 - mode is 5x5. - 0x1 - mode is 3x3. - 0x2 - mode is 4x7.
            unsigned int ncp             :1 ; // No job copy: - 0x0 - do job copy - 0x1 - don't do job copy
            unsigned int rnd             :1 ; // Rounding: - 0x0 - perform only normalization (right-shift by QF) at the end of sum-of-products. - 0x1 - perform rounding (sum with 1 left-shifted by (QF-1) ) before normalization at the end of sum-of-products.
            unsigned int padding1:1 ;
            unsigned int wstride         :16; // Stride in bytes between one loaded filter and the next. If multiple filters are used in 3x3 mode, they are assumed to be contiguous and the stride is applied to the block of 2 / 3 of them.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config0_t;
        
        typedef union {
          struct {
            unsigned int thstride        :16; // Stride in bytes between quantization thresholds for strongly quantized modes (2bit / 4bit for weights, 4bit for activations).
            unsigned int qshifty         :5 ; // Quantization shift for y<sub>in</sub> and y<sub>out</sub> streams, used for 8bit mode quantization/dequantization.
            unsigned int qmodey          :3 ; // Quantization mode for y<sub>in</sub> and y<sub>out</sub> streams: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization
            unsigned int qshiftx         :5 ; // Quantization shift for x stream, used for 8bit mode quantization/dequantization.
            unsigned int qmodex          :3 ; // Quantization mode for x stream: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config1_t;
        
        typedef union {
          struct {
            unsigned int padding0:14;
            unsigned int rect            :1 ; // If set to 1, perform rectification (ReLU) before final quantization.
            unsigned int norm            :1 ; // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.
            unsigned int wait_nfeat      :8 ; // Number of features to process before waiting for software synchronization by  SW_EVT register.
            unsigned int qshiftw         :5 ; // Quantization shift for weights, used for 8bit mode quantization/dequantization.
            unsigned int qmodew          :3 ; // Quantization mode for weights: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization - 0x2 - 2bit quantization
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config2_t;
        
        typedef union {
          struct {
            unsigned int qoffsw          :16; // Quantization offset for weights, used to displace them before left-shift.
            unsigned int qoffsx          :16; // Quantization offset for activations, used to displace them before left-shift.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config3_t;
        
        typedef union {
          struct {
            unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_trans_size_aliased_t;
        
        typedef union {
          struct {
            unsigned int length          :16; // Ceil(line length) of y<sub>in</sub> / y<sub>out</sub> in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related YLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.
            unsigned int stride          :16; // Distance in bytes between two consecutive lines within a channel (y<sub>in</sub> / y<sub>out</sub>).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_line_stride_length_aliased_t;
        
        typedef union {
          struct {
            unsigned int length          :16; // Height of a channel (y<sub>in</sub> / y<sub>out</sub>).
            unsigned int stride          :16; // Distance in bytes between two consecutive channels (y<sub>in</sub> / y<sub>out</sub>).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_feat_stride_length_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for third y<sub>out</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_2_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for second y<sub>out</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_1_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for first y<sub>out</sub> stream (all modes)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_0_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for third y<sub>in</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_2_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for second y<sub>in</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_1_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for first y<sub>in</sub> stream (all modes)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_0_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x  stream.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_trans_size_aliased_t;
        
        typedef union {
          struct {
            unsigned int length          :16; // Ceil(line length) of x in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related XLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.
            unsigned int stride          :16; // Distance in bytes between two consecutive lines within a channel (x).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_line_stride_length_aliased_t;
        
        typedef union {
          struct {
            unsigned int length          :16; // Height of a channel (x).
            unsigned int stride          :16; // Distance in bytes between two consecutive channels (x).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_feat_stride_length_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for x stream.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_in_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for weights.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_w_base_addr_aliased_t;
        
        typedef union {
          struct {
            unsigned int lbuflen         :8 ; // Linebuffer virtual length. Acceptable values range between 2 and LINEBUF_LENGTH.
            unsigned int lbufskiplo      :4 ; // Low index of column range to be remove from linebuffer at line change. Set to: - 0x2 - for 3x3 and 5x5 modes - 0x1 - for 4x7 mode
            unsigned int lbufskiphi      :4 ; // High index of column range to be remove from linebuffer at line change. Set to 3 for all modes.
            unsigned int noyconst        :16; // Constant to sum instead of y<sub>in</sub> if the NY flag is active in the GEN_CONFIG1 register.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config0_aliased_t;
        
        typedef union {
          struct {
            unsigned int vect_disable_mask:4 ; // Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using multiple 3x3 streams, The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, and bit 1 vector 0.
            unsigned int padding0:4 ;
            unsigned int wof_param       :6 ; // Output channels counter wrap parameter. If LO=0, the counter of output channels will be updated every WOF inner iterations. If LO=1, the counter of output channels will be updated every inner iteration and reset every WOF  iterations. If both WIF and WOF are 0, the looping mechanism is disabled.
            unsigned int padding1:2 ;
            unsigned int wif_param       :6 ; // Input channels counter wrap parameter. If LO=0, the counter of input channels will be updated every inner iteration and reset every WIF iterations. If LO=1, the counter of input channels will be updated every WIF iterations. IIf both WIF and WOF are 0, the looping mechanism is disabled.
            unsigned int rect            :1 ; // If set to 1, perform rectification (ReLU) before final quantization.
            unsigned int lo              :1 ; // Loop order: - 0x0 - output features (OF) are the outer loop - 0x1 - input features (IF) are the outer loop
            unsigned int noutfeat        :8 ; // Total number of output channels computed in the HWCE loop.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config1_aliased_t;
        
        typedef union {
          struct {
            unsigned int lbufprecnt      :8 ; // Counter used internally to acknowledge the end of the line buffer preloading phase. Set to: - 2*LBUFLEN+4 - in 3x3 mode - 4*LBUFLEN+4 - in 5x5 mode - 3*LBUFLEN+4 - in 4x7 mode
            unsigned int ylen            :8 ; // Length in number of pixels of y<sub>in</sub> / y<sub>out</sub>, as used by the internal datapath. Can be shorter than the length programmed in the Y_LINE_STRIDE_LENGTH register to guarantee that the correct number of inputs/outputs is consumed/produced while respecting alignment contraints. Ignored if 0x0.
            unsigned int padding0:1 ;
            unsigned int norm            :1 ; // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.
            unsigned int ylrem           :2 ; // Remainder of Y_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.
            unsigned int xlrem           :2 ; // Remainder of X_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.
            unsigned int s2              :1 ; // If set to 1, perform 2x2 striding on y<sub>in</sub> and y<sub>out</sub>. Note that this mode still requires loading all pixels in the linebuffer.
            unsigned int bye             :1 ; // Enable the bypass buffer. This means that when BYNFEAT > 1, the y<sub>out</sub> / y<sub>in</sub> feedback loop is closed internally in the HWCE keeping always full precision (16 bits).
            unsigned int bynfeat         :8 ; // Number of channels to loop upon inside the bypass buffer.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config2_aliased_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_trans_size_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_line_stride_length_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_feat_stride_length_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_2_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_1_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_0_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_2_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_1_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_0_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_trans_size_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_line_stride_length_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_feat_stride_length_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_in_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_w_base_addr_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config0_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config1_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config2_ctx0_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_trans_size_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_line_stride_length_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_feat_stride_length_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_2_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_1_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_0_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_2_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_1_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_0_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_trans_size_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_line_stride_length_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_feat_stride_length_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_in_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_w_base_addr_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config0_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config1_ctx1_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config2_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_hwce : public vp::regmap
        {
        public:
            vp_hwce_trigger trigger;
            vp_hwce_acquire acquire;
            vp_hwce_finished_jobs finished_jobs;
            vp_hwce_status status;
            vp_hwce_running_job running_job;
            vp_hwce_soft_clear soft_clear;
            vp_hwce_offloader_id offloader_id;
            vp_hwce_sw_evt sw_evt;
            vp_hwce_gen_config0 gen_config0;
            vp_hwce_gen_config1 gen_config1;
            vp_hwce_gen_config2 gen_config2;
            vp_hwce_gen_config3 gen_config3;
            vp_hwce_y_trans_size_aliased y_trans_size_aliased;
            vp_hwce_y_line_stride_length_aliased y_line_stride_length_aliased;
            vp_hwce_y_feat_stride_length_aliased y_feat_stride_length_aliased;
            vp_hwce_y_out_2_base_addr_aliased y_out_2_base_addr_aliased;
            vp_hwce_y_out_1_base_addr_aliased y_out_1_base_addr_aliased;
            vp_hwce_y_out_0_base_addr_aliased y_out_0_base_addr_aliased;
            vp_hwce_y_in_2_base_addr_aliased y_in_2_base_addr_aliased;
            vp_hwce_y_in_1_base_addr_aliased y_in_1_base_addr_aliased;
            vp_hwce_y_in_0_base_addr_aliased y_in_0_base_addr_aliased;
            vp_hwce_x_trans_size_aliased x_trans_size_aliased;
            vp_hwce_x_line_stride_length_aliased x_line_stride_length_aliased;
            vp_hwce_x_feat_stride_length_aliased x_feat_stride_length_aliased;
            vp_hwce_x_in_base_addr_aliased x_in_base_addr_aliased;
            vp_hwce_w_base_addr_aliased w_base_addr_aliased;
            vp_hwce_job_config0_aliased job_config0_aliased;
            vp_hwce_job_config1_aliased job_config1_aliased;
            vp_hwce_job_config2_aliased job_config2_aliased;
            vp_hwce_y_trans_size_ctx0 y_trans_size_ctx0;
            vp_hwce_y_line_stride_length_ctx0 y_line_stride_length_ctx0;
            vp_hwce_y_feat_stride_length_ctx0 y_feat_stride_length_ctx0;
            vp_hwce_y_out_2_base_addr_ctx0 y_out_2_base_addr_ctx0;
            vp_hwce_y_out_1_base_addr_ctx0 y_out_1_base_addr_ctx0;
            vp_hwce_y_out_0_base_addr_ctx0 y_out_0_base_addr_ctx0;
            vp_hwce_y_in_2_base_addr_ctx0 y_in_2_base_addr_ctx0;
            vp_hwce_y_in_1_base_addr_ctx0 y_in_1_base_addr_ctx0;
            vp_hwce_y_in_0_base_addr_ctx0 y_in_0_base_addr_ctx0;
            vp_hwce_x_trans_size_ctx0 x_trans_size_ctx0;
            vp_hwce_x_line_stride_length_ctx0 x_line_stride_length_ctx0;
            vp_hwce_x_feat_stride_length_ctx0 x_feat_stride_length_ctx0;
            vp_hwce_x_in_base_addr_ctx0 x_in_base_addr_ctx0;
            vp_hwce_w_base_addr_ctx0 w_base_addr_ctx0;
            vp_hwce_job_config0_ctx0 job_config0_ctx0;
            vp_hwce_job_config1_ctx0 job_config1_ctx0;
            vp_hwce_job_config2_ctx0 job_config2_ctx0;
            vp_hwce_y_trans_size_ctx1 y_trans_size_ctx1;
            vp_hwce_y_line_stride_length_ctx1 y_line_stride_length_ctx1;
            vp_hwce_y_feat_stride_length_ctx1 y_feat_stride_length_ctx1;
            vp_hwce_y_out_2_base_addr_ctx1 y_out_2_base_addr_ctx1;
            vp_hwce_y_out_1_base_addr_ctx1 y_out_1_base_addr_ctx1;
            vp_hwce_y_out_0_base_addr_ctx1 y_out_0_base_addr_ctx1;
            vp_hwce_y_in_2_base_addr_ctx1 y_in_2_base_addr_ctx1;
            vp_hwce_y_in_1_base_addr_ctx1 y_in_1_base_addr_ctx1;
            vp_hwce_y_in_0_base_addr_ctx1 y_in_0_base_addr_ctx1;
            vp_hwce_x_trans_size_ctx1 x_trans_size_ctx1;
            vp_hwce_x_line_stride_length_ctx1 x_line_stride_length_ctx1;
            vp_hwce_x_feat_stride_length_ctx1 x_feat_stride_length_ctx1;
            vp_hwce_x_in_base_addr_ctx1 x_in_base_addr_ctx1;
            vp_hwce_w_base_addr_ctx1 w_base_addr_ctx1;
            vp_hwce_job_config0_ctx1 job_config0_ctx1;
            vp_hwce_job_config1_ctx1 job_config1_ctx1;
            vp_hwce_job_config2_ctx1 job_config2_ctx1;
        };

|

.. _hwce_TRIGGER:

TRIGGER
"""""""

Trigger the execution of an offloaded job

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                       Description                                                                       |
    +=====+===+====+=========================================================================================================================================================+
    |31:0 |W  |ANY |Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue.|
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Trigger the execution of an offloaded job
                #define HWCE_TRIGGER_OFFSET                      0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_trigger_get(uint32_t base);
        static inline void hwce_trigger_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue. (access: W)
        #define HWCE_TRIGGER_ANY_BIT                                         0
        #define HWCE_TRIGGER_ANY_WIDTH                                       32
        #define HWCE_TRIGGER_ANY_MASK                                        0xffffffff
        #define HWCE_TRIGGER_ANY_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_TRIGGER_ANY_GET(value)                        (GAP_BEXTRACTU((value),32,0))
        #define HWCE_TRIGGER_ANY_GETS(value)                       (GAP_BEXTRACT((value),32,0))
        #define HWCE_TRIGGER_ANY_SET(value,field)                  (GAP_BINSERT((value),(field),32,0))
        #define HWCE_TRIGGER_ANY(val)                              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int any             :32; // Write of any value will close the current offload phase by releasing the job offload lock and inserting the currently offloaded job in the control queue.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_trigger_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_trigger : public vp::reg_32
        {
        public:
            inline void any_set(uint32_t value);
            inline uint32_t any_get();
        };

|

.. _hwce_ACQUIRE:

ACQUIRE
"""""""

Acquire the lock to offload job

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                   Description                                                                                                                                    |
    +=====+===+======+==================================================================================================================================================================================================================================================================================+
    |7:0  |R  |ID/ERR|If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code                                                                                                                                                                                                   |
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:8 |R  |ERR   |An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1)|
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Acquire the lock to offload job
                #define HWCE_ACQUIRE_OFFSET                      0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_acquire_get(uint32_t base);
        static inline void hwce_acquire_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code (access: R)
        #define HWCE_ACQUIRE_ID_ERR_BIT                                      0
        #define HWCE_ACQUIRE_ID_ERR_WIDTH                                    8
        #define HWCE_ACQUIRE_ID_ERR_MASK                                     0xff
        #define HWCE_ACQUIRE_ID_ERR_RESET                                    0x0
        
        // An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1) (access: R)
        #define HWCE_ACQUIRE_ERR_BIT                                         8
        #define HWCE_ACQUIRE_ERR_WIDTH                                       24
        #define HWCE_ACQUIRE_ERR_MASK                                        0xffffff00
        #define HWCE_ACQUIRE_ERR_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_ACQUIRE_ID_ERR_GET(value)                     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_ACQUIRE_ID_ERR_GETS(value)                    (GAP_BEXTRACT((value),8,0))
        #define HWCE_ACQUIRE_ID_ERR_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
        #define HWCE_ACQUIRE_ID_ERR(val)                           ((val) << 0)
        
        #define HWCE_ACQUIRE_ERR_GET(value)                        (GAP_BEXTRACTU((value),24,8))
        #define HWCE_ACQUIRE_ERR_GETS(value)                       (GAP_BEXTRACT((value),24,8))
        #define HWCE_ACQUIRE_ERR_SET(value,field)                  (GAP_BINSERT((value),(field),24,8))
        #define HWCE_ACQUIRE_ERR(val)                              ((val) << 8)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id_err          :8 ; // If ERR is 0 then the ID of the offloaded job. Otherwise, part of the error code
            unsigned int err             :24; // An error code if one of the following conditions apply: 1. if the context copy is going on, it will answer 0xfffffffd (-3) 2. else, if the job offload lock has been established, it will answer 0xfffffffe (-2) 3. else, if the job queue is full, it will answer 0xffffffff (-1)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_acquire_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_acquire : public vp::reg_32
        {
        public:
            inline void id_err_set(uint32_t value);
            inline uint32_t id_err_get();
            inline void err_set(uint32_t value);
            inline uint32_t err_get();
        };

|

.. _hwce_FINISHED_JOBS:

FINISHED_JOBS
"""""""""""""

Number of concluded jobs since last read

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                             Description                                                                                                                                                              |
    +=====+===+====+======================================================================================================================================================================================================================================================================================================================================+
    |31:0 |R  |JOBS|The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access|
    +-----+---+----+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Number of concluded jobs since last read
                #define HWCE_FINISHED_JOBS_OFFSET                0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_finished_jobs_get(uint32_t base);
        static inline void hwce_finished_jobs_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access (access: R)
        #define HWCE_FINISHED_JOBS_JOBS_BIT                                  0
        #define HWCE_FINISHED_JOBS_JOBS_WIDTH                                32
        #define HWCE_FINISHED_JOBS_JOBS_MASK                                 0xffffffff
        #define HWCE_FINISHED_JOBS_JOBS_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_FINISHED_JOBS_JOBS_GET(value)                 (GAP_BEXTRACTU((value),32,0))
        #define HWCE_FINISHED_JOBS_JOBS_GETS(value)                (GAP_BEXTRACT((value),32,0))
        #define HWCE_FINISHED_JOBS_JOBS_SET(value,field)           (GAP_BINSERT((value),(field),32,0))
        #define HWCE_FINISHED_JOBS_JOBS(val)                       ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int jobs            :32; // The number of jobs that the HWCE executed and finished since the last time the same FINISHED_JOBS register was accessed. A read to FINISHED_JOBS returns: - 0x0 if no job was completed since the last access - 0x1 if a single job was completed since the last access - 0x2 if two or more jobs were completed since the last access
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_finished_jobs_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_finished_jobs : public vp::reg_32
        {
        public:
            inline void jobs_set(uint32_t value);
            inline uint32_t jobs_get();
        };

|

.. _hwce_STATUS:

STATUS
""""""

Status of the HWCE

.. table:: 

    +-----+---+----+--------------------------------------------------------------+
    |Bit #|R/W|Name|                         Description                          |
    +=====+===+====+==============================================================+
    |    0|R  |ST  |Status of the HWCE - 0x0 Not running a job - 0x1 Running a job|
    +-----+---+----+--------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Status of the HWCE
                #define HWCE_STATUS_OFFSET                       0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_status_get(uint32_t base);
        static inline void hwce_status_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Status of the HWCE - 0x0 Not running a job - 0x1 Running a job (access: R)
        #define HWCE_STATUS_ST_BIT                                           0
        #define HWCE_STATUS_ST_WIDTH                                         1
        #define HWCE_STATUS_ST_MASK                                          0x1
        #define HWCE_STATUS_ST_RESET                                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_STATUS_ST_GET(value)                          (GAP_BEXTRACTU((value),1,0))
        #define HWCE_STATUS_ST_GETS(value)                         (GAP_BEXTRACT((value),1,0))
        #define HWCE_STATUS_ST_SET(value,field)                    (GAP_BINSERT((value),(field),1,0))
        #define HWCE_STATUS_ST(val)                                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int st              :1 ; // Status of the HWCE - 0x0 Not running a job - 0x1 Running a job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_status_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_status : public vp::reg_32
        {
        public:
            inline void st_set(uint32_t value);
            inline uint32_t st_get();
        };

|

.. _hwce_RUNNING_JOB:

RUNNING_JOB
"""""""""""

ID of the currently running job

.. table:: 

    +-----+---+----+-------------------------------+
    |Bit #|R/W|Name|          Description          |
    +=====+===+====+===============================+
    |7:0  |R  |ID  |ID of the currently running job|
    +-----+---+----+-------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ID of the currently running job
                #define HWCE_RUNNING_JOB_OFFSET                  0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_running_job_get(uint32_t base);
        static inline void hwce_running_job_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ID of the currently running job (access: R)
        #define HWCE_RUNNING_JOB_ID_BIT                                      0
        #define HWCE_RUNNING_JOB_ID_WIDTH                                    8
        #define HWCE_RUNNING_JOB_ID_MASK                                     0xff
        #define HWCE_RUNNING_JOB_ID_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_RUNNING_JOB_ID_GET(value)                     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_RUNNING_JOB_ID_GETS(value)                    (GAP_BEXTRACT((value),8,0))
        #define HWCE_RUNNING_JOB_ID_SET(value,field)               (GAP_BINSERT((value),(field),8,0))
        #define HWCE_RUNNING_JOB_ID(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id              :8 ; // ID of the currently running job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_running_job_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_running_job : public vp::reg_32
        {
        public:
            inline void id_set(uint32_t value);
            inline uint32_t id_get();
        };

|

.. _hwce_SOFT_CLEAR:

SOFT_CLEAR
""""""""""

Reset HWCE to known idle state

.. table:: 

    +-----+---+----+----------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                Description                                 |
    +=====+===+====+============================================================================+
    |31:0 |W  |ANY |A write of any value to this register will reset the HWCE to its idle state.|
    +-----+---+----+----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Reset HWCE to known idle state
                #define HWCE_SOFT_CLEAR_OFFSET                   0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_soft_clear_get(uint32_t base);
        static inline void hwce_soft_clear_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // A write of any value to this register will reset the HWCE to its idle state. (access: W)
        #define HWCE_SOFT_CLEAR_ANY_BIT                                      0
        #define HWCE_SOFT_CLEAR_ANY_WIDTH                                    32
        #define HWCE_SOFT_CLEAR_ANY_MASK                                     0xffffffff
        #define HWCE_SOFT_CLEAR_ANY_RESET                                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_SOFT_CLEAR_ANY_GET(value)                     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_SOFT_CLEAR_ANY_GETS(value)                    (GAP_BEXTRACT((value),32,0))
        #define HWCE_SOFT_CLEAR_ANY_SET(value,field)               (GAP_BINSERT((value),(field),32,0))
        #define HWCE_SOFT_CLEAR_ANY(val)                           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int any             :32; // A write of any value to this register will reset the HWCE to its idle state.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_soft_clear_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_soft_clear : public vp::reg_32
        {
        public:
            inline void any_set(uint32_t value);
            inline uint32_t any_get();
        };

|

.. _hwce_OFFLOADER_ID:

OFFLOADER_ID
""""""""""""

ID of offloader cores

.. table:: 

    +-----+---+----+-------------------------------------------------------+
    |Bit #|R/W|Name|                      Description                      |
    +=====+===+====+=======================================================+
    |31:0 |R  |ID  |N-th is the ID+1 of the offloader core for the N-th job|
    +-----+---+----+-------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ID of offloader cores
                #define HWCE_OFFLOADER_ID_OFFSET                 0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_offloader_id_get(uint32_t base);
        static inline void hwce_offloader_id_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // N-th is the ID+1 of the offloader core for the N-th job (access: R)
        #define HWCE_OFFLOADER_ID_ID_BIT                                     0
        #define HWCE_OFFLOADER_ID_ID_WIDTH                                   32
        #define HWCE_OFFLOADER_ID_ID_MASK                                    0xffffffff
        #define HWCE_OFFLOADER_ID_ID_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_OFFLOADER_ID_ID_GET(value)                    (GAP_BEXTRACTU((value),32,0))
        #define HWCE_OFFLOADER_ID_ID_GETS(value)                   (GAP_BEXTRACT((value),32,0))
        #define HWCE_OFFLOADER_ID_ID_SET(value,field)              (GAP_BINSERT((value),(field),32,0))
        #define HWCE_OFFLOADER_ID_ID(val)                          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int id              :32; // N-th is the ID+1 of the offloader core for the N-th job
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_offloader_id_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_offloader_id : public vp::reg_32
        {
        public:
            inline void id_set(uint32_t value);
            inline uint32_t id_get();
        };

|

.. _hwce_SW_EVT:

SW_EVT
""""""

Software synchronization

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                   Description                                                    |
    +=====+===+====+==================================================================================================================+
    |31:0 |W  |EVT |Any write triggers a software event that the HWCE can wait on, depending on the WAIT_FEAT value set in GEN_CONFIG2|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Software synchronization
                #define HWCE_SW_EVT_OFFSET                       0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_sw_evt_get(uint32_t base);
        static inline void hwce_sw_evt_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Any write triggers a software event that the HWCE can wait on, depending on the WAIT_FEAT value set in GEN_CONFIG2 (access: W)
        #define HWCE_SW_EVT_EVT_BIT                                          0
        #define HWCE_SW_EVT_EVT_WIDTH                                        32
        #define HWCE_SW_EVT_EVT_MASK                                         0xffffffff
        #define HWCE_SW_EVT_EVT_RESET                                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_SW_EVT_EVT_GET(value)                         (GAP_BEXTRACTU((value),32,0))
        #define HWCE_SW_EVT_EVT_GETS(value)                        (GAP_BEXTRACT((value),32,0))
        #define HWCE_SW_EVT_EVT_SET(value,field)                   (GAP_BINSERT((value),(field),32,0))
        #define HWCE_SW_EVT_EVT(val)                               ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int evt             :32; // Any write triggers a software event that the HWCE can wait on, depending on the WAIT_FEAT value set in GEN_CONFIG2
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_sw_evt_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_sw_evt : public vp::reg_32
        {
        public:
            inline void evt_set(uint32_t value);
            inline uint32_t evt_get();
        };

|

.. _hwce_GEN_CONFIG0:

GEN_CONFIG0
"""""""""""

Generic configuration register 0

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                                                                                   Description                                                                                                   |
    +=====+===+=======+=================================================================================================================================================================================================================+
    |5:0  |R/W|QF     |Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage.                                                                                    |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |6    |R/W|NF     |Operation type: - 0x0 - By-the-book 2D convolution (with weight flipping) - 0x1 - Replace by-the-book 2D convolution with 2D cross-correlation (without weight flipping)                                         |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7    |R/W|NY     |No y<sub>in</sub> mode: - 0x0 - Normal operation - 0x1 - Disable loading of y<sub>in</sub> and replace it with a constant bias set in the CONFIG2 register                                                       |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12:11|R/W|CONV   |Convolution mode: - 0x0 - mode is 5x5. - 0x1 - mode is 3x3. - 0x2 - mode is 4x7.                                                                                                                                 |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13   |R/W|NCP    |No job copy: - 0x0 - do job copy - 0x1 - don't do job copy                                                                                                                                                       |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |14   |R/W|RND    |Rounding: - 0x0 - perform only normalization (right-shift by QF) at the end of sum-of-products. - 0x1 - perform rounding (sum with 1 left-shifted by (QF-1) ) before normalization at the end of sum-of-products.|
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|WSTRIDE|Stride in bytes between one loaded filter and the next. If multiple filters are used in 3x3 mode, they are assumed to be contiguous and the stride is applied to the block of 2 / 3 of them.                     |
    +-----+---+-------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Generic configuration register 0
                #define HWCE_GEN_CONFIG0_OFFSET                  0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_gen_config0_get(uint32_t base);
        static inline void hwce_gen_config0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage. (access: R/W)
        #define HWCE_GEN_CONFIG0_QF_BIT                                      0
        #define HWCE_GEN_CONFIG0_QF_WIDTH                                    6
        #define HWCE_GEN_CONFIG0_QF_MASK                                     0x3f
        #define HWCE_GEN_CONFIG0_QF_RESET                                    0xc
        
        // Operation type: - 0x0 - By-the-book 2D convolution (with weight flipping) - 0x1 - Replace by-the-book 2D convolution with 2D cross-correlation (without weight flipping) (access: R/W)
        #define HWCE_GEN_CONFIG0_NF_BIT                                      6
        #define HWCE_GEN_CONFIG0_NF_WIDTH                                    1
        #define HWCE_GEN_CONFIG0_NF_MASK                                     0x40
        #define HWCE_GEN_CONFIG0_NF_RESET                                    0x1
        
        // No y<sub>in</sub> mode: - 0x0 - Normal operation - 0x1 - Disable loading of y<sub>in</sub> and replace it with a constant bias set in the CONFIG2 register (access: R/W)
        #define HWCE_GEN_CONFIG0_NY_BIT                                      7
        #define HWCE_GEN_CONFIG0_NY_WIDTH                                    1
        #define HWCE_GEN_CONFIG0_NY_MASK                                     0x80
        #define HWCE_GEN_CONFIG0_NY_RESET                                    0x0
        
        // Convolution mode: - 0x0 - mode is 5x5. - 0x1 - mode is 3x3. - 0x2 - mode is 4x7. (access: R/W)
        #define HWCE_GEN_CONFIG0_CONV_BIT                                    11
        #define HWCE_GEN_CONFIG0_CONV_WIDTH                                  2
        #define HWCE_GEN_CONFIG0_CONV_MASK                                   0x1800
        #define HWCE_GEN_CONFIG0_CONV_RESET                                  0x0
        
        // No job copy: - 0x0 - do job copy - 0x1 - don't do job copy (access: R/W)
        #define HWCE_GEN_CONFIG0_NCP_BIT                                     13
        #define HWCE_GEN_CONFIG0_NCP_WIDTH                                   1
        #define HWCE_GEN_CONFIG0_NCP_MASK                                    0x2000
        #define HWCE_GEN_CONFIG0_NCP_RESET                                   0x0
        
        // Rounding: - 0x0 - perform only normalization (right-shift by QF) at the end of sum-of-products. - 0x1 - perform rounding (sum with 1 left-shifted by (QF-1) ) before normalization at the end of sum-of-products. (access: R/W)
        #define HWCE_GEN_CONFIG0_RND_BIT                                     14
        #define HWCE_GEN_CONFIG0_RND_WIDTH                                   1
        #define HWCE_GEN_CONFIG0_RND_MASK                                    0x4000
        #define HWCE_GEN_CONFIG0_RND_RESET                                   0x0
        
        // Stride in bytes between one loaded filter and the next. If multiple filters are used in 3x3 mode, they are assumed to be contiguous and the stride is applied to the block of 2 / 3 of them. (access: R/W)
        #define HWCE_GEN_CONFIG0_WSTRIDE_BIT                                 16
        #define HWCE_GEN_CONFIG0_WSTRIDE_WIDTH                               16
        #define HWCE_GEN_CONFIG0_WSTRIDE_MASK                                0xffff0000
        #define HWCE_GEN_CONFIG0_WSTRIDE_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_GEN_CONFIG0_QF_GET(value)                     (GAP_BEXTRACTU((value),6,0))
        #define HWCE_GEN_CONFIG0_QF_GETS(value)                    (GAP_BEXTRACT((value),6,0))
        #define HWCE_GEN_CONFIG0_QF_SET(value,field)               (GAP_BINSERT((value),(field),6,0))
        #define HWCE_GEN_CONFIG0_QF(val)                           ((val) << 0)
        
        #define HWCE_GEN_CONFIG0_NF_GET(value)                     (GAP_BEXTRACTU((value),1,6))
        #define HWCE_GEN_CONFIG0_NF_GETS(value)                    (GAP_BEXTRACT((value),1,6))
        #define HWCE_GEN_CONFIG0_NF_SET(value,field)               (GAP_BINSERT((value),(field),1,6))
        #define HWCE_GEN_CONFIG0_NF(val)                           ((val) << 6)
        
        #define HWCE_GEN_CONFIG0_NY_GET(value)                     (GAP_BEXTRACTU((value),1,7))
        #define HWCE_GEN_CONFIG0_NY_GETS(value)                    (GAP_BEXTRACT((value),1,7))
        #define HWCE_GEN_CONFIG0_NY_SET(value,field)               (GAP_BINSERT((value),(field),1,7))
        #define HWCE_GEN_CONFIG0_NY(val)                           ((val) << 7)
        
        #define HWCE_GEN_CONFIG0_CONV_GET(value)                   (GAP_BEXTRACTU((value),2,11))
        #define HWCE_GEN_CONFIG0_CONV_GETS(value)                  (GAP_BEXTRACT((value),2,11))
        #define HWCE_GEN_CONFIG0_CONV_SET(value,field)             (GAP_BINSERT((value),(field),2,11))
        #define HWCE_GEN_CONFIG0_CONV(val)                         ((val) << 11)
        
        #define HWCE_GEN_CONFIG0_NCP_GET(value)                    (GAP_BEXTRACTU((value),1,13))
        #define HWCE_GEN_CONFIG0_NCP_GETS(value)                   (GAP_BEXTRACT((value),1,13))
        #define HWCE_GEN_CONFIG0_NCP_SET(value,field)              (GAP_BINSERT((value),(field),1,13))
        #define HWCE_GEN_CONFIG0_NCP(val)                          ((val) << 13)
        
        #define HWCE_GEN_CONFIG0_RND_GET(value)                    (GAP_BEXTRACTU((value),1,14))
        #define HWCE_GEN_CONFIG0_RND_GETS(value)                   (GAP_BEXTRACT((value),1,14))
        #define HWCE_GEN_CONFIG0_RND_SET(value,field)              (GAP_BINSERT((value),(field),1,14))
        #define HWCE_GEN_CONFIG0_RND(val)                          ((val) << 14)
        
        #define HWCE_GEN_CONFIG0_WSTRIDE_GET(value)                (GAP_BEXTRACTU((value),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE_GETS(value)               (GAP_BEXTRACT((value),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE_SET(value,field)          (GAP_BINSERT((value),(field),16,16))
        #define HWCE_GEN_CONFIG0_WSTRIDE(val)                      ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qf              :6 ; // Fixed-point format. Pixels will be shifted to the right by QF bits in the normalization step after the sum-of-products stage.
            unsigned int nf              :1 ; // Operation type: - 0x0 - By-the-book 2D convolution (with weight flipping) - 0x1 - Replace by-the-book 2D convolution with 2D cross-correlation (without weight flipping)
            unsigned int ny              :1 ; // No y<sub>in</sub> mode: - 0x0 - Normal operation - 0x1 - Disable loading of y<sub>in</sub> and replace it with a constant bias set in the CONFIG2 register
            unsigned int padding0:3 ;
            unsigned int conv            :2 ; // Convolution mode: - 0x0 - mode is 5x5. - 0x1 - mode is 3x3. - 0x2 - mode is 4x7.
            unsigned int ncp             :1 ; // No job copy: - 0x0 - do job copy - 0x1 - don't do job copy
            unsigned int rnd             :1 ; // Rounding: - 0x0 - perform only normalization (right-shift by QF) at the end of sum-of-products. - 0x1 - perform rounding (sum with 1 left-shifted by (QF-1) ) before normalization at the end of sum-of-products.
            unsigned int padding1:1 ;
            unsigned int wstride         :16; // Stride in bytes between one loaded filter and the next. If multiple filters are used in 3x3 mode, they are assumed to be contiguous and the stride is applied to the block of 2 / 3 of them.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_gen_config0 : public vp::reg_32
        {
        public:
            inline void qf_set(uint32_t value);
            inline uint32_t qf_get();
            inline void nf_set(uint32_t value);
            inline uint32_t nf_get();
            inline void ny_set(uint32_t value);
            inline uint32_t ny_get();
            inline void conv_set(uint32_t value);
            inline uint32_t conv_get();
            inline void ncp_set(uint32_t value);
            inline uint32_t ncp_get();
            inline void rnd_set(uint32_t value);
            inline uint32_t rnd_get();
            inline void wstride_set(uint32_t value);
            inline uint32_t wstride_get();
        };

|

.. _hwce_GEN_CONFIG1:

GEN_CONFIG1
"""""""""""

Generic configuration register 1

.. table:: 

    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name  |                                                                    Description                                                                     |
    +=====+===+========+====================================================================================================================================================+
    |15:0 |R/W|THSTRIDE|Stride in bytes between quantization thresholds for strongly quantized modes (2bit / 4bit for weights, 4bit for activations).                       |
    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |20:16|R/W|QSHIFTY |Quantization shift for y<sub>in</sub> and y<sub>out</sub> streams, used for 8bit mode quantization/dequantization.                                  |
    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:21|R/W|QMODEY  |Quantization mode for y<sub>in</sub> and y<sub>out</sub> streams: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization|
    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |28:24|R/W|QSHIFTX |Quantization shift for x stream, used for 8bit mode quantization/dequantization.                                                                    |
    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:29|R/W|QMODEX  |Quantization mode for x stream: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization                                  |
    +-----+---+--------+----------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Generic configuration register 1
                #define HWCE_GEN_CONFIG1_OFFSET                  0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_gen_config1_get(uint32_t base);
        static inline void hwce_gen_config1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stride in bytes between quantization thresholds for strongly quantized modes (2bit / 4bit for weights, 4bit for activations). (access: R/W)
        #define HWCE_GEN_CONFIG1_THSTRIDE_BIT                                0
        #define HWCE_GEN_CONFIG1_THSTRIDE_WIDTH                              16
        #define HWCE_GEN_CONFIG1_THSTRIDE_MASK                               0xffff
        #define HWCE_GEN_CONFIG1_THSTRIDE_RESET                              0x0
        
        // Quantization shift for y<sub>in</sub> and y<sub>out</sub> streams, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG1_QSHIFTY_BIT                                 16
        #define HWCE_GEN_CONFIG1_QSHIFTY_WIDTH                               5
        #define HWCE_GEN_CONFIG1_QSHIFTY_MASK                                0x1f0000
        #define HWCE_GEN_CONFIG1_QSHIFTY_RESET                               0x0
        
        // Quantization mode for y<sub>in</sub> and y<sub>out</sub> streams: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG1_QMODEY_BIT                                  21
        #define HWCE_GEN_CONFIG1_QMODEY_WIDTH                                3
        #define HWCE_GEN_CONFIG1_QMODEY_MASK                                 0xe00000
        #define HWCE_GEN_CONFIG1_QMODEY_RESET                                0x0
        
        // Quantization shift for x stream, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG1_QSHIFTX_BIT                                 24
        #define HWCE_GEN_CONFIG1_QSHIFTX_WIDTH                               5
        #define HWCE_GEN_CONFIG1_QSHIFTX_MASK                                0x1f000000
        #define HWCE_GEN_CONFIG1_QSHIFTX_RESET                               0x0
        
        // Quantization mode for x stream: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG1_QMODEX_BIT                                  29
        #define HWCE_GEN_CONFIG1_QMODEX_WIDTH                                3
        #define HWCE_GEN_CONFIG1_QMODEX_MASK                                 0xe0000000
        #define HWCE_GEN_CONFIG1_QMODEX_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_GEN_CONFIG1_THSTRIDE_GET(value)               (GAP_BEXTRACTU((value),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE_GETS(value)              (GAP_BEXTRACT((value),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE_SET(value,field)         (GAP_BINSERT((value),(field),16,0))
        #define HWCE_GEN_CONFIG1_THSTRIDE(val)                     ((val) << 0)
        
        #define HWCE_GEN_CONFIG1_QSHIFTY_GET(value)                (GAP_BEXTRACTU((value),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY_GETS(value)               (GAP_BEXTRACT((value),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY_SET(value,field)          (GAP_BINSERT((value),(field),5,16))
        #define HWCE_GEN_CONFIG1_QSHIFTY(val)                      ((val) << 16)
        
        #define HWCE_GEN_CONFIG1_QMODEY_GET(value)                 (GAP_BEXTRACTU((value),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY_GETS(value)                (GAP_BEXTRACT((value),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY_SET(value,field)           (GAP_BINSERT((value),(field),3,21))
        #define HWCE_GEN_CONFIG1_QMODEY(val)                       ((val) << 21)
        
        #define HWCE_GEN_CONFIG1_QSHIFTX_GET(value)                (GAP_BEXTRACTU((value),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX_GETS(value)               (GAP_BEXTRACT((value),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX_SET(value,field)          (GAP_BINSERT((value),(field),5,24))
        #define HWCE_GEN_CONFIG1_QSHIFTX(val)                      ((val) << 24)
        
        #define HWCE_GEN_CONFIG1_QMODEX_GET(value)                 (GAP_BEXTRACTU((value),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX_GETS(value)                (GAP_BEXTRACT((value),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX_SET(value,field)           (GAP_BINSERT((value),(field),3,29))
        #define HWCE_GEN_CONFIG1_QMODEX(val)                       ((val) << 29)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int thstride        :16; // Stride in bytes between quantization thresholds for strongly quantized modes (2bit / 4bit for weights, 4bit for activations).
            unsigned int qshifty         :5 ; // Quantization shift for y<sub>in</sub> and y<sub>out</sub> streams, used for 8bit mode quantization/dequantization.
            unsigned int qmodey          :3 ; // Quantization mode for y<sub>in</sub> and y<sub>out</sub> streams: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization
            unsigned int qshiftx         :5 ; // Quantization shift for x stream, used for 8bit mode quantization/dequantization.
            unsigned int qmodex          :3 ; // Quantization mode for x stream: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_gen_config1 : public vp::reg_32
        {
        public:
            inline void thstride_set(uint32_t value);
            inline uint32_t thstride_get();
            inline void qshifty_set(uint32_t value);
            inline uint32_t qshifty_get();
            inline void qmodey_set(uint32_t value);
            inline uint32_t qmodey_get();
            inline void qshiftx_set(uint32_t value);
            inline uint32_t qshiftx_get();
            inline void qmodex_set(uint32_t value);
            inline uint32_t qmodex_get();
        };

|

.. _hwce_GEN_CONFIG2:

GEN_CONFIG2
"""""""""""

Generic configuration register 2

.. table:: 

    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                                Description                                                                |
    +=====+===+==========+===========================================================================================================================================+
    |   14|R/W|RECT      |If set to 1, perform rectification (ReLU) before final quantization.                                                                       |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+
    |   15|R/W|NORM      |If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.             |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+
    |23:16|R/W|WAIT_NFEAT|Number of features to process before waiting for software synchronization by  SW_EVT register.                                             |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+
    |28:24|R/W|QSHIFTW   |Quantization shift for weights, used for 8bit mode quantization/dequantization.                                                            |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+
    |31:29|R/W|QMODEW    |Quantization mode for weights: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization - 0x2 - 2bit quantization|
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Generic configuration register 2
                #define HWCE_GEN_CONFIG2_OFFSET                  0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_gen_config2_get(uint32_t base);
        static inline void hwce_gen_config2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // If set to 1, perform rectification (ReLU) before final quantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_RECT_BIT                                    14
        #define HWCE_GEN_CONFIG2_RECT_WIDTH                                  1
        #define HWCE_GEN_CONFIG2_RECT_MASK                                   0x4000
        #define HWCE_GEN_CONFIG2_RECT_RESET                                  0x0
        
        // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_NORM_BIT                                    15
        #define HWCE_GEN_CONFIG2_NORM_WIDTH                                  1
        #define HWCE_GEN_CONFIG2_NORM_MASK                                   0x8000
        #define HWCE_GEN_CONFIG2_NORM_RESET                                  0x0
        
        // Number of features to process before waiting for software synchronization by  SW_EVT register. (access: R/W)
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_BIT                              16
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_WIDTH                            8
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_MASK                             0xff0000
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_RESET                            0x0
        
        // Quantization shift for weights, used for 8bit mode quantization/dequantization. (access: R/W)
        #define HWCE_GEN_CONFIG2_QSHIFTW_BIT                                 24
        #define HWCE_GEN_CONFIG2_QSHIFTW_WIDTH                               5
        #define HWCE_GEN_CONFIG2_QSHIFTW_MASK                                0x1f000000
        #define HWCE_GEN_CONFIG2_QSHIFTW_RESET                               0x0
        
        // Quantization mode for weights: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization - 0x2 - 2bit quantization (access: R/W)
        #define HWCE_GEN_CONFIG2_QMODEW_BIT                                  29
        #define HWCE_GEN_CONFIG2_QMODEW_WIDTH                                3
        #define HWCE_GEN_CONFIG2_QMODEW_MASK                                 0xe0000000
        #define HWCE_GEN_CONFIG2_QMODEW_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_GEN_CONFIG2_RECT_GET(value)                   (GAP_BEXTRACTU((value),1,14))
        #define HWCE_GEN_CONFIG2_RECT_GETS(value)                  (GAP_BEXTRACT((value),1,14))
        #define HWCE_GEN_CONFIG2_RECT_SET(value,field)             (GAP_BINSERT((value),(field),1,14))
        #define HWCE_GEN_CONFIG2_RECT(val)                         ((val) << 14)
        
        #define HWCE_GEN_CONFIG2_NORM_GET(value)                   (GAP_BEXTRACTU((value),1,15))
        #define HWCE_GEN_CONFIG2_NORM_GETS(value)                  (GAP_BEXTRACT((value),1,15))
        #define HWCE_GEN_CONFIG2_NORM_SET(value,field)             (GAP_BINSERT((value),(field),1,15))
        #define HWCE_GEN_CONFIG2_NORM(val)                         ((val) << 15)
        
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_GET(value)             (GAP_BEXTRACTU((value),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_GETS(value)            (GAP_BEXTRACT((value),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT_SET(value,field)       (GAP_BINSERT((value),(field),8,16))
        #define HWCE_GEN_CONFIG2_WAIT_NFEAT(val)                   ((val) << 16)
        
        #define HWCE_GEN_CONFIG2_QSHIFTW_GET(value)                (GAP_BEXTRACTU((value),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW_GETS(value)               (GAP_BEXTRACT((value),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW_SET(value,field)          (GAP_BINSERT((value),(field),5,24))
        #define HWCE_GEN_CONFIG2_QSHIFTW(val)                      ((val) << 24)
        
        #define HWCE_GEN_CONFIG2_QMODEW_GET(value)                 (GAP_BEXTRACTU((value),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW_GETS(value)                (GAP_BEXTRACT((value),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW_SET(value,field)           (GAP_BINSERT((value),(field),3,29))
        #define HWCE_GEN_CONFIG2_QMODEW(val)                       ((val) << 29)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int padding0:14;
            unsigned int rect            :1 ; // If set to 1, perform rectification (ReLU) before final quantization.
            unsigned int norm            :1 ; // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.
            unsigned int wait_nfeat      :8 ; // Number of features to process before waiting for software synchronization by  SW_EVT register.
            unsigned int qshiftw         :5 ; // Quantization shift for weights, used for 8bit mode quantization/dequantization.
            unsigned int qmodew          :3 ; // Quantization mode for weights: - 0x0 - 16bit (full precision) - 0x4 - 8bit quantization - 0x3 - 4bit quantization - 0x2 - 2bit quantization
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_gen_config2 : public vp::reg_32
        {
        public:
            inline void rect_set(uint32_t value);
            inline uint32_t rect_get();
            inline void norm_set(uint32_t value);
            inline uint32_t norm_get();
            inline void wait_nfeat_set(uint32_t value);
            inline uint32_t wait_nfeat_get();
            inline void qshiftw_set(uint32_t value);
            inline uint32_t qshiftw_get();
            inline void qmodew_set(uint32_t value);
            inline uint32_t qmodew_get();
        };

|

.. _hwce_GEN_CONFIG3:

GEN_CONFIG3
"""""""""""

Generic configuration register 3

.. table:: 

    +-----+---+------+-----------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                 Description                                 |
    +=====+===+======+=============================================================================+
    |15:0 |R/W|QOFFSW|Quantization offset for weights, used to displace them before left-shift.    |
    +-----+---+------+-----------------------------------------------------------------------------+
    |31:16|R/W|QOFFSX|Quantization offset for activations, used to displace them before left-shift.|
    +-----+---+------+-----------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Generic configuration register 3
                #define HWCE_GEN_CONFIG3_OFFSET                  0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_gen_config3_get(uint32_t base);
        static inline void hwce_gen_config3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Quantization offset for weights, used to displace them before left-shift. (access: R/W)
        #define HWCE_GEN_CONFIG3_QOFFSW_BIT                                  0
        #define HWCE_GEN_CONFIG3_QOFFSW_WIDTH                                16
        #define HWCE_GEN_CONFIG3_QOFFSW_MASK                                 0xffff
        #define HWCE_GEN_CONFIG3_QOFFSW_RESET                                0x0
        
        // Quantization offset for activations, used to displace them before left-shift. (access: R/W)
        #define HWCE_GEN_CONFIG3_QOFFSX_BIT                                  16
        #define HWCE_GEN_CONFIG3_QOFFSX_WIDTH                                16
        #define HWCE_GEN_CONFIG3_QOFFSX_MASK                                 0xffff0000
        #define HWCE_GEN_CONFIG3_QOFFSX_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_GEN_CONFIG3_QOFFSW_GET(value)                 (GAP_BEXTRACTU((value),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW_GETS(value)                (GAP_BEXTRACT((value),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW_SET(value,field)           (GAP_BINSERT((value),(field),16,0))
        #define HWCE_GEN_CONFIG3_QOFFSW(val)                       ((val) << 0)
        
        #define HWCE_GEN_CONFIG3_QOFFSX_GET(value)                 (GAP_BEXTRACTU((value),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX_GETS(value)                (GAP_BEXTRACT((value),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX_SET(value,field)           (GAP_BINSERT((value),(field),16,16))
        #define HWCE_GEN_CONFIG3_QOFFSX(val)                       ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int qoffsw          :16; // Quantization offset for weights, used to displace them before left-shift.
            unsigned int qoffsx          :16; // Quantization offset for activations, used to displace them before left-shift.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_gen_config3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_gen_config3 : public vp::reg_32
        {
        public:
            inline void qoffsw_set(uint32_t value);
            inline uint32_t qoffsw_get();
            inline void qoffsx_set(uint32_t value);
            inline uint32_t qoffsx_get();
        };

|

.. _hwce_Y_TRANS_SIZE_ALIASED:

Y_TRANS_SIZE_ALIASED
""""""""""""""""""""

Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                             Description                                                                                              |
    +=====+===+====+======================================================================================================================================================================================================+
    |31:0 |R/W|SIZE|Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_ALIASED_OFFSET         0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_trans_size_aliased_get(uint32_t base);
        static inline void hwce_y_trans_size_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams. (access: R/W)
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_BIT                           0
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_WIDTH                         32
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_MASK                          0xffffffff
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_TRANS_SIZE_ALIASED_SIZE(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source and sink interfaces for y<sub>in</sub> and y<sub>out</sub> streams.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_trans_size_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_trans_size_aliased : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _hwce_Y_LINE_STRIDE_LENGTH_ALIASED:

Y_LINE_STRIDE_LENGTH_ALIASED
""""""""""""""""""""""""""""

Line stride and length for y<sub>in</sub> and y<sub>out</sub>

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                                      Description                                                                                                                                       |
    +=====+===+======+========================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|LENGTH|Ceil(line length) of y<sub>in</sub> / y<sub>out</sub> in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related YLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.|
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|STRIDE|Distance in bytes between two consecutive lines within a channel (y<sub>in</sub> / y<sub>out</sub>).                                                                                                                                                                                    |
    +-----+---+------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_OFFSET 0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_line_stride_length_aliased_get(uint32_t base);
        static inline void hwce_y_line_stride_length_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Ceil(line length) of y<sub>in</sub> / y<sub>out</sub> in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related YLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer. (access: R/W)
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive lines within a channel (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_Y_LINE_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int length          :16; // Ceil(line length) of y<sub>in</sub> / y<sub>out</sub> in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related YLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.
            unsigned int stride          :16; // Distance in bytes between two consecutive lines within a channel (y<sub>in</sub> / y<sub>out</sub>).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_line_stride_length_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_line_stride_length_aliased : public vp::reg_32
        {
        public:
            inline void length_set(uint32_t value);
            inline uint32_t length_get();
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _hwce_Y_FEAT_STRIDE_LENGTH_ALIASED:

Y_FEAT_STRIDE_LENGTH_ALIASED
""""""""""""""""""""""""""""

Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>

.. table:: 

    +-----+---+------+--------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                     Description                                      |
    +=====+===+======+======================================================================================+
    |15:0 |R/W|LENGTH|Height of a channel (y<sub>in</sub> / y<sub>out</sub>).                               |
    +-----+---+------+--------------------------------------------------------------------------------------+
    |31:16|R/W|STRIDE|Distance in bytes between two consecutive channels (y<sub>in</sub> / y<sub>out</sub>).|
    +-----+---+------+--------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_OFFSET 0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_feat_stride_length_aliased_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Height of a channel (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive channels (y<sub>in</sub> / y<sub>out</sub>). (access: R/W)
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_Y_FEAT_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int length          :16; // Height of a channel (y<sub>in</sub> / y<sub>out</sub>).
            unsigned int stride          :16; // Distance in bytes between two consecutive channels (y<sub>in</sub> / y<sub>out</sub>).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_feat_stride_length_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_feat_stride_length_aliased : public vp::reg_32
        {
        public:
            inline void length_set(uint32_t value);
            inline uint32_t length_get();
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _hwce_Y_OUT_2_BASE_ADDR_ALIASED:

Y_OUT_2_BASE_ADDR_ALIASED
"""""""""""""""""""""""""

Base address of y<sub>out</sub>[2]

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                                |
    +=====+===+====+==========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for third y<sub>out</sub> stream (3x3 mode)|
    +-----+---+----+--------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_OFFSET    0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_2_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for third y<sub>out</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_2_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for third y<sub>out</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_2_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_2_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_Y_OUT_1_BASE_ADDR_ALIASED:

Y_OUT_1_BASE_ADDR_ALIASED
"""""""""""""""""""""""""

Base address of y<sub>out</sub>[1]

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                Description                                |
    +=====+===+====+===========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for second y<sub>out</sub> stream (3x3 mode)|
    +-----+---+----+---------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_OFFSET    0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_1_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for second y<sub>out</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_1_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for second y<sub>out</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_1_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_1_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_Y_OUT_0_BASE_ADDR_ALIASED:

Y_OUT_0_BASE_ADDR_ALIASED
"""""""""""""""""""""""""

Base address of y<sub>out</sub>[0]

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                Description                                |
    +=====+===+====+===========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for first y<sub>out</sub> stream (all modes)|
    +-----+---+----+---------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_OFFSET    0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_0_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for first y<sub>out</sub> stream (all modes) (access: R/W)
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_BIT                      0
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_WIDTH                    32
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_MASK                     0xffffffff
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_RESET                    0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_GET(value)     (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_GETS(value)    (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_OUT_0_BASE_ADDR_ALIASED_ADDR(val)           ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for first y<sub>out</sub> stream (all modes)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_out_0_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_0_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_Y_IN_2_BASE_ADDR_ALIASED:

Y_IN_2_BASE_ADDR_ALIASED
""""""""""""""""""""""""

Base address of y<sub>in</sub>[2]

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                               |
    +=====+===+====+=========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for third y<sub>in</sub> stream (3x3 mode)|
    +-----+---+----+-------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_OFFSET     0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_2_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for third y<sub>in</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_2_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for third y<sub>in</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_2_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_2_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_Y_IN_1_BASE_ADDR_ALIASED:

Y_IN_1_BASE_ADDR_ALIASED
""""""""""""""""""""""""

Base address of y<sub>in</sub>[1]

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                                |
    +=====+===+====+==========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for second y<sub>in</sub> stream (3x3 mode)|
    +-----+---+----+--------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_OFFSET     0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_1_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for second y<sub>in</sub> stream (3x3 mode) (access: R/W)
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_1_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for second y<sub>in</sub> stream (3x3 mode)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_1_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_1_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_Y_IN_0_BASE_ADDR_ALIASED:

Y_IN_0_BASE_ADDR_ALIASED
""""""""""""""""""""""""

Base address of y<sub>in</sub>[0]

.. table:: 

    +-----+---+----+--------------------------------------------------------------------------+
    |Bit #|R/W|Name|                               Description                                |
    +=====+===+====+==========================================================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for first y<sub>in</sub> stream (all modes)|
    +-----+---+----+--------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_OFFSET     0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_0_base_addr_aliased_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for first y<sub>in</sub> stream (all modes) (access: R/W)
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_BIT                       0
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_WIDTH                     32
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_MASK                      0xffffffff
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_GET(value)      (GAP_BEXTRACTU((value),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_GETS(value)     (GAP_BEXTRACT((value),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR_SET(value,field) (GAP_BINSERT((value),(field),32,0))
        #define HWCE_Y_IN_0_BASE_ADDR_ALIASED_ADDR(val)            ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for first y<sub>in</sub> stream (all modes)
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_y_in_0_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_0_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_X_TRANS_SIZE_ALIASED:

X_TRANS_SIZE_ALIASED
""""""""""""""""""""

Total number of words to be read for x<sub>in</sub>

.. table:: 

    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                          Description                                                                          |
    +=====+===+====+===============================================================================================================================================================+
    |31:0 |R/W|SIZE|Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x  stream.|
    +-----+---+----+---------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_ALIASED_OFFSET         0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_trans_size_aliased_get(uint32_t base);
        static inline void hwce_x_trans_size_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x  stream. (access: R/W)
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_BIT                           0
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_WIDTH                         32
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_MASK                          0xffffffff
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_RESET                         0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_GET(value)          (GAP_BEXTRACTU((value),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_GETS(value)         (GAP_BEXTRACT((value),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE_SET(value,field)    (GAP_BINSERT((value),(field),32,0))
        #define HWCE_X_TRANS_SIZE_ALIASED_SIZE(val)                ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int size            :32; // Contains the total number of words (i.e. double 16bit pixels, quad 8bit pixels, etc.) to be read / written by the streaming source interface for the x  stream.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_trans_size_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_trans_size_aliased : public vp::reg_32
        {
        public:
            inline void size_set(uint32_t value);
            inline uint32_t size_get();
        };

|

.. _hwce_X_LINE_STRIDE_LENGTH_ALIASED:

X_LINE_STRIDE_LENGTH_ALIASED
""""""""""""""""""""""""""""

Line stride and length for x<sub>in</sub>

.. table:: 

    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                                                                                                       Description                                                                                                                       |
    +=====+===+======+=========================================================================================================================================================================================================================================================+
    |15:0 |R/W|LENGTH|Ceil(line length) of x in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related XLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.|
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|STRIDE|Distance in bytes between two consecutive lines within a channel (x).                                                                                                                                                                                    |
    +-----+---+------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_OFFSET 0x70

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_line_stride_length_aliased_get(uint32_t base);
        static inline void hwce_x_line_stride_length_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Ceil(line length) of x in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related XLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer. (access: R/W)
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive lines within a channel (x). (access: R/W)
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_X_LINE_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int length          :16; // Ceil(line length) of x in number of words. If the final word is not fully consumed, e.g. for uneven 16bit transfers, the related XLREM field in JOB_CONFIG2 must be set to the number of bytes that are actually consumed/produced in the final transfer.
            unsigned int stride          :16; // Distance in bytes between two consecutive lines within a channel (x).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_line_stride_length_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_line_stride_length_aliased : public vp::reg_32
        {
        public:
            inline void length_set(uint32_t value);
            inline uint32_t length_get();
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _hwce_X_FEAT_STRIDE_LENGTH_ALIASED:

X_FEAT_STRIDE_LENGTH_ALIASED
""""""""""""""""""""""""""""

Feature (block) stride and length for x<sub>in</sub>

.. table:: 

    +-----+---+------+-------------------------------------------------------+
    |Bit #|R/W| Name |                      Description                      |
    +=====+===+======+=======================================================+
    |15:0 |R/W|LENGTH|Height of a channel (x).                               |
    +-----+---+------+-------------------------------------------------------+
    |31:16|R/W|STRIDE|Distance in bytes between two consecutive channels (x).|
    +-----+---+------+-------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_OFFSET 0x74

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_feat_stride_length_aliased_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Height of a channel (x). (access: R/W)
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_BIT                 0
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_WIDTH               16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_MASK                0xffff
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_RESET               0x0
        
        // Distance in bytes between two consecutive channels (x). (access: R/W)
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_BIT                 16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_WIDTH               16
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_MASK                0xffff0000
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_RESET               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GET(value) (GAP_BEXTRACTU((value),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_GETS(value) (GAP_BEXTRACT((value),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_LENGTH(val)      ((val) << 0)
        
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GET(value) (GAP_BEXTRACTU((value),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_GETS(value) (GAP_BEXTRACT((value),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_X_FEAT_STRIDE_LENGTH_ALIASED_STRIDE(val)      ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int length          :16; // Height of a channel (x).
            unsigned int stride          :16; // Distance in bytes between two consecutive channels (x).
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_feat_stride_length_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_feat_stride_length_aliased : public vp::reg_32
        {
        public:
            inline void length_set(uint32_t value);
            inline uint32_t length_get();
            inline void stride_set(uint32_t value);
            inline uint32_t stride_get();
        };

|

.. _hwce_X_IN_BASE_ADDR_ALIASED:

X_IN_BASE_ADDR_ALIASED
""""""""""""""""""""""

Base address of x<sub>in</sub>

.. table:: 

    +-----+---+----+--------------------------------------------+
    |Bit #|R/W|Name|                Description                 |
    +=====+===+====+============================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for x stream.|
    +-----+---+----+--------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_ALIASED_OFFSET       0x78

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_in_base_addr_aliased_get(uint32_t base);
        static inline void hwce_x_in_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for x stream. (access: R/W)
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_BIT                         0
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_WIDTH                       32
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_MASK                        0xffffffff
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_GET(value)        (GAP_BEXTRACTU((value),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_GETS(value)       (GAP_BEXTRACT((value),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR_SET(value,field)  (GAP_BINSERT((value),(field),32,0))
        #define HWCE_X_IN_BASE_ADDR_ALIASED_ADDR(val)              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for x stream.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_x_in_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_in_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_W_BASE_ADDR_ALIASED:

W_BASE_ADDR_ALIASED
"""""""""""""""""""

Base address of W

.. table:: 

    +-----+---+----+-------------------------------------------+
    |Bit #|R/W|Name|                Description                |
    +=====+===+====+===========================================+
    |31:0 |R/W|ADDR|Pointer into cluster L1 memory for weights.|
    +-----+---+----+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Base address of W
                #define HWCE_W_BASE_ADDR_ALIASED_OFFSET          0x7c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_w_base_addr_aliased_get(uint32_t base);
        static inline void hwce_w_base_addr_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Pointer into cluster L1 memory for weights. (access: R/W)
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_BIT                            0
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_WIDTH                          32
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_MASK                           0xffffffff
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_GET(value)           (GAP_BEXTRACTU((value),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_GETS(value)          (GAP_BEXTRACT((value),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR_SET(value,field)     (GAP_BINSERT((value),(field),32,0))
        #define HWCE_W_BASE_ADDR_ALIASED_ADDR(val)                 ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int addr            :32; // Pointer into cluster L1 memory for weights.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_w_base_addr_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_w_base_addr_aliased : public vp::reg_32
        {
        public:
            inline void addr_set(uint32_t value);
            inline uint32_t addr_get();
        };

|

.. _hwce_JOB_CONFIG0_ALIASED:

JOB_CONFIG0_ALIASED
"""""""""""""""""""

Job configuration register 0

.. table:: 

    +-----+---+----------+---------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                           Description                                                           |
    +=====+===+==========+=================================================================================================================================+
    |7:0  |R/W|LBUFLEN   |Linebuffer virtual length. Acceptable values range between 2 and LINEBUF_LENGTH.                                                 |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|LBUFSKIPLO|Low index of column range to be remove from linebuffer at line change. Set to: - 0x2 - for 3x3 and 5x5 modes - 0x1 - for 4x7 mode|
    +-----+---+----------+---------------------------------------------------------------------------------------------------------------------------------+
    |15:12|R/W|LBUFSKIPHI|High index of column range to be remove from linebuffer at line change. Set to 3 for all modes.                                  |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------------------------------+
    |31:16|R/W|NOYCONST  |Constant to sum instead of y<sub>in</sub> if the NY flag is active in the GEN_CONFIG1 register.                                  |
    +-----+---+----------+---------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_ALIASED_OFFSET          0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config0_aliased_get(uint32_t base);
        static inline void hwce_job_config0_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Linebuffer virtual length. Acceptable values range between 2 and LINEBUF_LENGTH. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_BIT                         0
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_WIDTH                       8
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_MASK                        0xff
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_RESET                       0x0
        
        // Low index of column range to be remove from linebuffer at line change. Set to: - 0x2 - for 3x3 and 5x5 modes - 0x1 - for 4x7 mode (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_BIT                      8
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_WIDTH                    4
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_MASK                     0xf00
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_RESET                    0x0
        
        // High index of column range to be remove from linebuffer at line change. Set to 3 for all modes. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_BIT                      12
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_WIDTH                    4
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_MASK                     0xf000
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_RESET                    0x0
        
        // Constant to sum instead of y<sub>in</sub> if the NY flag is active in the GEN_CONFIG1 register. (access: R/W)
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_BIT                        16
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_WIDTH                      16
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_MASK                       0xffff0000
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_GET(value)        (GAP_BEXTRACTU((value),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_GETS(value)       (GAP_BEXTRACT((value),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN_SET(value,field)  (GAP_BINSERT((value),(field),8,0))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFLEN(val)              ((val) << 0)
        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_GET(value)     (GAP_BEXTRACTU((value),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_GETS(value)    (GAP_BEXTRACT((value),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO_SET(value,field) (GAP_BINSERT((value),(field),4,8))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPLO(val)           ((val) << 8)
        
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_GET(value)     (GAP_BEXTRACTU((value),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_GETS(value)    (GAP_BEXTRACT((value),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI_SET(value,field) (GAP_BINSERT((value),(field),4,12))
        #define HWCE_JOB_CONFIG0_ALIASED_LBUFSKIPHI(val)           ((val) << 12)
        
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_GET(value)       (GAP_BEXTRACTU((value),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_GETS(value)      (GAP_BEXTRACT((value),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define HWCE_JOB_CONFIG0_ALIASED_NOYCONST(val)             ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int lbuflen         :8 ; // Linebuffer virtual length. Acceptable values range between 2 and LINEBUF_LENGTH.
            unsigned int lbufskiplo      :4 ; // Low index of column range to be remove from linebuffer at line change. Set to: - 0x2 - for 3x3 and 5x5 modes - 0x1 - for 4x7 mode
            unsigned int lbufskiphi      :4 ; // High index of column range to be remove from linebuffer at line change. Set to 3 for all modes.
            unsigned int noyconst        :16; // Constant to sum instead of y<sub>in</sub> if the NY flag is active in the GEN_CONFIG1 register.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config0_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config0_aliased : public vp::reg_32
        {
        public:
            inline void lbuflen_set(uint32_t value);
            inline uint32_t lbuflen_get();
            inline void lbufskiplo_set(uint32_t value);
            inline uint32_t lbufskiplo_get();
            inline void lbufskiphi_set(uint32_t value);
            inline uint32_t lbufskiphi_get();
            inline void noyconst_set(uint32_t value);
            inline uint32_t noyconst_get();
        };

|

.. _hwce_JOB_CONFIG1_ALIASED:

JOB_CONFIG1_ALIASED
"""""""""""""""""""

Job configuration register 1

.. table:: 

    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|      Name       |                                                                                                                                              Description                                                                                                                                               |
    +=====+===+=================+========================================================================================================================================================================================================================================================================================================+
    |3:0  |R/W|VECT_DISABLE_MASK|Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using multiple 3x3 streams, The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, and bit 1 vector 0.                                              |
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |13:8 |R/W|WOF_PARAM        |Output channels counter wrap parameter. If LO=0, the counter of output channels will be updated every WOF inner iterations. If LO=1, the counter of output channels will be updated every inner iteration and reset every WOF  iterations. If both WIF and WOF are 0, the looping mechanism is disabled.|
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |21:16|R/W|WIF_PARAM        |Input channels counter wrap parameter. If LO=0, the counter of input channels will be updated every inner iteration and reset every WIF iterations. If LO=1, the counter of input channels will be updated every WIF iterations. IIf both WIF and WOF are 0, the looping mechanism is disabled.         |
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22   |R/W|RECT             |If set to 1, perform rectification (ReLU) before final quantization.                                                                                                                                                                                                                                    |
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23   |R/W|LO               |Loop order: - 0x0 - output features (OF) are the outer loop - 0x1 - input features (IF) are the outer loop                                                                                                                                                                                              |
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:24|R/W|NOUTFEAT         |Total number of output channels computed in the HWCE loop.                                                                                                                                                                                                                                              |
    +-----+---+-----------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_ALIASED_OFFSET          0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config1_aliased_get(uint32_t base);
        static inline void hwce_job_config1_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using multiple 3x3 streams, The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, and bit 1 vector 0. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_BIT               0
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_WIDTH             4
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_MASK              0xf
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_RESET             0x0
        
        // Output channels counter wrap parameter. If LO=0, the counter of output channels will be updated every WOF inner iterations. If LO=1, the counter of output channels will be updated every inner iteration and reset every WOF  iterations. If both WIF and WOF are 0, the looping mechanism is disabled. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_BIT                       8
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_WIDTH                     6
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_MASK                      0x3f00
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_RESET                     0x0
        
        // Input channels counter wrap parameter. If LO=0, the counter of input channels will be updated every inner iteration and reset every WIF iterations. If LO=1, the counter of input channels will be updated every WIF iterations. IIf both WIF and WOF are 0, the looping mechanism is disabled. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_BIT                       16
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_WIDTH                     6
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_MASK                      0x3f0000
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_RESET                     0x0
        
        // If set to 1, perform rectification (ReLU) before final quantization. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_BIT                            22
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_WIDTH                          1
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_MASK                           0x400000
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_RESET                          0x0
        
        // Loop order: - 0x0 - output features (OF) are the outer loop - 0x1 - input features (IF) are the outer loop (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_LO_BIT                              23
        #define HWCE_JOB_CONFIG1_ALIASED_LO_WIDTH                            1
        #define HWCE_JOB_CONFIG1_ALIASED_LO_MASK                             0x800000
        #define HWCE_JOB_CONFIG1_ALIASED_LO_RESET                            0x0
        
        // Total number of output channels computed in the HWCE loop. (access: R/W)
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_BIT                        24
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_WIDTH                      8
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_MASK                       0xff000000
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_RESET                      0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_GET(value) (GAP_BEXTRACTU((value),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_GETS(value) (GAP_BEXTRACT((value),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK_SET(value,field) (GAP_BINSERT((value),(field),4,0))
        #define HWCE_JOB_CONFIG1_ALIASED_VECT_DISABLE_MASK(val)    ((val) << 0)
        
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_GET(value)      (GAP_BEXTRACTU((value),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_GETS(value)     (GAP_BEXTRACT((value),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM_SET(value,field) (GAP_BINSERT((value),(field),6,8))
        #define HWCE_JOB_CONFIG1_ALIASED_WOF_PARAM(val)            ((val) << 8)
        
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_GET(value)      (GAP_BEXTRACTU((value),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_GETS(value)     (GAP_BEXTRACT((value),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM_SET(value,field) (GAP_BINSERT((value),(field),6,16))
        #define HWCE_JOB_CONFIG1_ALIASED_WIF_PARAM(val)            ((val) << 16)
        
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_GET(value)           (GAP_BEXTRACTU((value),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_GETS(value)          (GAP_BEXTRACT((value),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT_SET(value,field)     (GAP_BINSERT((value),(field),1,22))
        #define HWCE_JOB_CONFIG1_ALIASED_RECT(val)                 ((val) << 22)
        
        #define HWCE_JOB_CONFIG1_ALIASED_LO_GET(value)             (GAP_BEXTRACTU((value),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO_GETS(value)            (GAP_BEXTRACT((value),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO_SET(value,field)       (GAP_BINSERT((value),(field),1,23))
        #define HWCE_JOB_CONFIG1_ALIASED_LO(val)                   ((val) << 23)
        
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_GET(value)       (GAP_BEXTRACTU((value),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_GETS(value)      (GAP_BEXTRACT((value),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT_SET(value,field) (GAP_BINSERT((value),(field),8,24))
        #define HWCE_JOB_CONFIG1_ALIASED_NOUTFEAT(val)             ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int vect_disable_mask:4 ; // Vector mode mask. Defaults to 0x0, which means that all vectors are enabled. Can be used to disable unused vector routes when using multiple 3x3 streams, The bits are reversed in order, so bit 3 indicates vector 0, bit 2 vector 1, and bit 1 vector 0.
            unsigned int padding0:4 ;
            unsigned int wof_param       :6 ; // Output channels counter wrap parameter. If LO=0, the counter of output channels will be updated every WOF inner iterations. If LO=1, the counter of output channels will be updated every inner iteration and reset every WOF  iterations. If both WIF and WOF are 0, the looping mechanism is disabled.
            unsigned int padding1:2 ;
            unsigned int wif_param       :6 ; // Input channels counter wrap parameter. If LO=0, the counter of input channels will be updated every inner iteration and reset every WIF iterations. If LO=1, the counter of input channels will be updated every WIF iterations. IIf both WIF and WOF are 0, the looping mechanism is disabled.
            unsigned int rect            :1 ; // If set to 1, perform rectification (ReLU) before final quantization.
            unsigned int lo              :1 ; // Loop order: - 0x0 - output features (OF) are the outer loop - 0x1 - input features (IF) are the outer loop
            unsigned int noutfeat        :8 ; // Total number of output channels computed in the HWCE loop.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config1_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config1_aliased : public vp::reg_32
        {
        public:
            inline void vect_disable_mask_set(uint32_t value);
            inline uint32_t vect_disable_mask_get();
            inline void wof_param_set(uint32_t value);
            inline uint32_t wof_param_get();
            inline void wif_param_set(uint32_t value);
            inline uint32_t wif_param_get();
            inline void rect_set(uint32_t value);
            inline uint32_t rect_get();
            inline void lo_set(uint32_t value);
            inline uint32_t lo_get();
            inline void noutfeat_set(uint32_t value);
            inline uint32_t noutfeat_get();
        };

|

.. _hwce_JOB_CONFIG2_ALIASED:

JOB_CONFIG2_ALIASED
"""""""""""""""""""

Job configuration register 2

.. table:: 

    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|   Name   |                                                                                                                                                    Description                                                                                                                                                    |
    +=====+===+==========+===================================================================================================================================================================================================================================================================================================================+
    |7:0  |R/W|LBUFPRECNT|Counter used internally to acknowledge the end of the line buffer preloading phase. Set to: - 2*LBUFLEN+4 - in 3x3 mode - 4*LBUFLEN+4 - in 5x5 mode - 3*LBUFLEN+4 - in 4x7 mode                                                                                                                                    |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:8 |R/W|YLEN      |Length in number of pixels of y<sub>in</sub> / y<sub>out</sub>, as used by the internal datapath. Can be shorter than the length programmed in the Y_LINE_STRIDE_LENGTH register to guarantee that the correct number of inputs/outputs is consumed/produced while respecting alignment contraints. Ignored if 0x0.|
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |17   |R/W|NORM      |If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.                                                                                                                                                                                     |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19:18|R/W|YLREM     |Remainder of Y_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.                                                                                                                                                                                                                          |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |21:20|R/W|XLREM     |Remainder of X_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.                                                                                                                                                                                                                          |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |22   |R/W|S2        |If set to 1, perform 2x2 striding on y<sub>in</sub> and y<sub>out</sub>. Note that this mode still requires loading all pixels in the linebuffer.                                                                                                                                                                  |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23   |R/W|BYE       |Enable the bypass buffer. This means that when BYNFEAT > 1, the y<sub>out</sub> / y<sub>in</sub> feedback loop is closed internally in the HWCE keeping always full precision (16 bits).                                                                                                                           |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |31:24|R/W|BYNFEAT   |Number of channels to loop upon inside the bypass buffer.                                                                                                                                                                                                                                                          |
    +-----+---+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_ALIASED_OFFSET          0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config2_aliased_get(uint32_t base);
        static inline void hwce_job_config2_aliased_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Counter used internally to acknowledge the end of the line buffer preloading phase. Set to: - 2*LBUFLEN+4 - in 3x3 mode - 4*LBUFLEN+4 - in 5x5 mode - 3*LBUFLEN+4 - in 4x7 mode (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_BIT                      0
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_WIDTH                    8
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_MASK                     0xff
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_RESET                    0x0
        
        // Length in number of pixels of y<sub>in</sub> / y<sub>out</sub>, as used by the internal datapath. Can be shorter than the length programmed in the Y_LINE_STRIDE_LENGTH register to guarantee that the correct number of inputs/outputs is consumed/produced while respecting alignment contraints. Ignored if 0x0. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_BIT                            8
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_WIDTH                          8
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_MASK                           0xff00
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_RESET                          0x0
        
        // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_BIT                            17
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_WIDTH                          1
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_MASK                           0x20000
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_RESET                          0x0
        
        // Remainder of Y_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_BIT                           18
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_WIDTH                         2
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_MASK                          0xc0000
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_RESET                         0x0
        
        // Remainder of X_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_BIT                           20
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_WIDTH                         2
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_MASK                          0x300000
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_RESET                         0x0
        
        // If set to 1, perform 2x2 striding on y<sub>in</sub> and y<sub>out</sub>. Note that this mode still requires loading all pixels in the linebuffer. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_S2_BIT                              22
        #define HWCE_JOB_CONFIG2_ALIASED_S2_WIDTH                            1
        #define HWCE_JOB_CONFIG2_ALIASED_S2_MASK                             0x400000
        #define HWCE_JOB_CONFIG2_ALIASED_S2_RESET                            0x0
        
        // Enable the bypass buffer. This means that when BYNFEAT > 1, the y<sub>out</sub> / y<sub>in</sub> feedback loop is closed internally in the HWCE keeping always full precision (16 bits). (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_BIT                             23
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_WIDTH                           1
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_MASK                            0x800000
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_RESET                           0x0
        
        // Number of channels to loop upon inside the bypass buffer. (access: R/W)
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_BIT                         24
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_WIDTH                       8
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_MASK                        0xff000000
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_RESET                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_GET(value)     (GAP_BEXTRACTU((value),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_GETS(value)    (GAP_BEXTRACT((value),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT_SET(value,field) (GAP_BINSERT((value),(field),8,0))
        #define HWCE_JOB_CONFIG2_ALIASED_LBUFPRECNT(val)           ((val) << 0)
        
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_GET(value)           (GAP_BEXTRACTU((value),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_GETS(value)          (GAP_BEXTRACT((value),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN_SET(value,field)     (GAP_BINSERT((value),(field),8,8))
        #define HWCE_JOB_CONFIG2_ALIASED_YLEN(val)                 ((val) << 8)
        
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_GET(value)           (GAP_BEXTRACTU((value),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_GETS(value)          (GAP_BEXTRACT((value),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM_SET(value,field)     (GAP_BINSERT((value),(field),1,17))
        #define HWCE_JOB_CONFIG2_ALIASED_NORM(val)                 ((val) << 17)
        
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_GET(value)          (GAP_BEXTRACTU((value),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_GETS(value)         (GAP_BEXTRACT((value),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM_SET(value,field)    (GAP_BINSERT((value),(field),2,18))
        #define HWCE_JOB_CONFIG2_ALIASED_YLREM(val)                ((val) << 18)
        
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_GET(value)          (GAP_BEXTRACTU((value),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_GETS(value)         (GAP_BEXTRACT((value),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM_SET(value,field)    (GAP_BINSERT((value),(field),2,20))
        #define HWCE_JOB_CONFIG2_ALIASED_XLREM(val)                ((val) << 20)
        
        #define HWCE_JOB_CONFIG2_ALIASED_S2_GET(value)             (GAP_BEXTRACTU((value),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2_GETS(value)            (GAP_BEXTRACT((value),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2_SET(value,field)       (GAP_BINSERT((value),(field),1,22))
        #define HWCE_JOB_CONFIG2_ALIASED_S2(val)                   ((val) << 22)
        
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_GET(value)            (GAP_BEXTRACTU((value),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_GETS(value)           (GAP_BEXTRACT((value),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE_SET(value,field)      (GAP_BINSERT((value),(field),1,23))
        #define HWCE_JOB_CONFIG2_ALIASED_BYE(val)                  ((val) << 23)
        
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_GET(value)        (GAP_BEXTRACTU((value),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_GETS(value)       (GAP_BEXTRACT((value),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT_SET(value,field)  (GAP_BINSERT((value),(field),8,24))
        #define HWCE_JOB_CONFIG2_ALIASED_BYNFEAT(val)              ((val) << 24)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int lbufprecnt      :8 ; // Counter used internally to acknowledge the end of the line buffer preloading phase. Set to: - 2*LBUFLEN+4 - in 3x3 mode - 4*LBUFLEN+4 - in 5x5 mode - 3*LBUFLEN+4 - in 4x7 mode
            unsigned int ylen            :8 ; // Length in number of pixels of y<sub>in</sub> / y<sub>out</sub>, as used by the internal datapath. Can be shorter than the length programmed in the Y_LINE_STRIDE_LENGTH register to guarantee that the correct number of inputs/outputs is consumed/produced while respecting alignment contraints. Ignored if 0x0.
            unsigned int padding0:1 ;
            unsigned int norm            :1 ; // If set to 1, in 8-bit activation mode perform normalization (using thresholds from 0 to 2 as params) before ReLU/quantization.
            unsigned int ylrem           :2 ; // Remainder of Y_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.
            unsigned int xlrem           :2 ; // Remainder of X_LINE_STRIDE_LENGTH.LENGTH in bytes, to be used for byte-aligned transfers.
            unsigned int s2              :1 ; // If set to 1, perform 2x2 striding on y<sub>in</sub> and y<sub>out</sub>. Note that this mode still requires loading all pixels in the linebuffer.
            unsigned int bye             :1 ; // Enable the bypass buffer. This means that when BYNFEAT > 1, the y<sub>out</sub> / y<sub>in</sub> feedback loop is closed internally in the HWCE keeping always full precision (16 bits).
            unsigned int bynfeat         :8 ; // Number of channels to loop upon inside the bypass buffer.
          };
          unsigned int raw;
        } __attribute__((packed)) hwce_job_config2_aliased_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config2_aliased : public vp::reg_32
        {
        public:
            inline void lbufprecnt_set(uint32_t value);
            inline uint32_t lbufprecnt_get();
            inline void ylen_set(uint32_t value);
            inline uint32_t ylen_get();
            inline void norm_set(uint32_t value);
            inline uint32_t norm_get();
            inline void ylrem_set(uint32_t value);
            inline uint32_t ylrem_get();
            inline void xlrem_set(uint32_t value);
            inline uint32_t xlrem_get();
            inline void s2_set(uint32_t value);
            inline uint32_t s2_get();
            inline void bye_set(uint32_t value);
            inline uint32_t bye_get();
            inline void bynfeat_set(uint32_t value);
            inline uint32_t bynfeat_get();
        };

|

.. _hwce_Y_TRANS_SIZE_CTX0:

Y_TRANS_SIZE_CTX0
"""""""""""""""""

Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_CTX0_OFFSET            0x140

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_trans_size_ctx0_get(uint32_t base);
        static inline void hwce_y_trans_size_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_trans_size_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_trans_size_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_LINE_STRIDE_LENGTH_CTX0:

Y_LINE_STRIDE_LENGTH_CTX0
"""""""""""""""""""""""""

Line stride and length for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x144

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_line_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_y_line_stride_length_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_line_stride_length_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_line_stride_length_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_FEAT_STRIDE_LENGTH_CTX0:

Y_FEAT_STRIDE_LENGTH_CTX0
"""""""""""""""""""""""""

Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x148

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_feat_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_feat_stride_length_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_feat_stride_length_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_2_BASE_ADDR_CTX0:

Y_OUT_2_BASE_ADDR_CTX0
""""""""""""""""""""""

Base address of y<sub>out</sub>[2]

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

        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_CTX0_OFFSET       0x150

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_2_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_2_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_2_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_1_BASE_ADDR_CTX0:

Y_OUT_1_BASE_ADDR_CTX0
""""""""""""""""""""""

Base address of y<sub>out</sub>[1]

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

        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_CTX0_OFFSET       0x154

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_1_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_1_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_1_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_0_BASE_ADDR_CTX0:

Y_OUT_0_BASE_ADDR_CTX0
""""""""""""""""""""""

Base address of y<sub>out</sub>[0]

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

        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_CTX0_OFFSET       0x158

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_0_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_0_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_0_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_2_BASE_ADDR_CTX0:

Y_IN_2_BASE_ADDR_CTX0
"""""""""""""""""""""

Base address of y<sub>in</sub>[2]

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

        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_CTX0_OFFSET        0x160

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_2_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_2_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_2_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_1_BASE_ADDR_CTX0:

Y_IN_1_BASE_ADDR_CTX0
"""""""""""""""""""""

Base address of y<sub>in</sub>[1]

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

        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_CTX0_OFFSET        0x164

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_1_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_1_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_1_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_0_BASE_ADDR_CTX0:

Y_IN_0_BASE_ADDR_CTX0
"""""""""""""""""""""

Base address of y<sub>in</sub>[0]

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

        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_CTX0_OFFSET        0x168

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_0_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_0_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_0_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_TRANS_SIZE_CTX0:

X_TRANS_SIZE_CTX0
"""""""""""""""""

Total number of words to be read for x<sub>in</sub>

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

        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_CTX0_OFFSET            0x16c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_trans_size_ctx0_get(uint32_t base);
        static inline void hwce_x_trans_size_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_trans_size_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_trans_size_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_LINE_STRIDE_LENGTH_CTX0:

X_LINE_STRIDE_LENGTH_CTX0
"""""""""""""""""""""""""

Line stride and length for x<sub>in</sub>

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

        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_CTX0_OFFSET    0x170

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_line_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_x_line_stride_length_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_line_stride_length_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_line_stride_length_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_FEAT_STRIDE_LENGTH_CTX0:

X_FEAT_STRIDE_LENGTH_CTX0
"""""""""""""""""""""""""

Feature (block) stride and length for x<sub>in</sub>

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

        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_CTX0_OFFSET    0x174

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_feat_stride_length_ctx0_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_feat_stride_length_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_feat_stride_length_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_IN_BASE_ADDR_CTX0:

X_IN_BASE_ADDR_CTX0
"""""""""""""""""""

Base address of x<sub>in</sub>

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

        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_CTX0_OFFSET          0x178

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_in_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_x_in_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_in_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_in_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_W_BASE_ADDR_CTX0:

W_BASE_ADDR_CTX0
""""""""""""""""

Base address of W

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

        
                // Base address of W
                #define HWCE_W_BASE_ADDR_CTX0_OFFSET             0x17c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_w_base_addr_ctx0_get(uint32_t base);
        static inline void hwce_w_base_addr_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_w_base_addr_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_w_base_addr_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG0_CTX0:

JOB_CONFIG0_CTX0
""""""""""""""""

Job configuration register 0

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

        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_CTX0_OFFSET             0x180

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config0_ctx0_get(uint32_t base);
        static inline void hwce_job_config0_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config0_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config0_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG1_CTX0:

JOB_CONFIG1_CTX0
""""""""""""""""

Job configuration register 1

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

        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_CTX0_OFFSET             0x184

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config1_ctx0_get(uint32_t base);
        static inline void hwce_job_config1_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config1_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config1_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG2_CTX0:

JOB_CONFIG2_CTX0
""""""""""""""""

Job configuration register 2

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

        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_CTX0_OFFSET             0x188

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config2_ctx0_get(uint32_t base);
        static inline void hwce_job_config2_ctx0_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config2_ctx0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config2_ctx0 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_TRANS_SIZE_CTX1:

Y_TRANS_SIZE_CTX1
"""""""""""""""""

Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Total number of words to be read for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_TRANS_SIZE_CTX1_OFFSET            0x240

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_trans_size_ctx1_get(uint32_t base);
        static inline void hwce_y_trans_size_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_trans_size_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_trans_size_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_LINE_STRIDE_LENGTH_CTX1:

Y_LINE_STRIDE_LENGTH_CTX1
"""""""""""""""""""""""""

Line stride and length for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Line stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x244

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_line_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_y_line_stride_length_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_line_stride_length_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_line_stride_length_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_FEAT_STRIDE_LENGTH_CTX1:

Y_FEAT_STRIDE_LENGTH_CTX1
"""""""""""""""""""""""""

Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>

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

        
                // Feature (block) stride and length for y<sub>in</sub> and y<sub>out</sub>
                #define HWCE_Y_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x248

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_feat_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_y_feat_stride_length_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_feat_stride_length_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_feat_stride_length_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_2_BASE_ADDR_CTX1:

Y_OUT_2_BASE_ADDR_CTX1
""""""""""""""""""""""

Base address of y<sub>out</sub>[2]

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

        
                // Base address of y<sub>out</sub>[2]
                #define HWCE_Y_OUT_2_BASE_ADDR_CTX1_OFFSET       0x250

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_2_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_2_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_2_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_2_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_1_BASE_ADDR_CTX1:

Y_OUT_1_BASE_ADDR_CTX1
""""""""""""""""""""""

Base address of y<sub>out</sub>[1]

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

        
                // Base address of y<sub>out</sub>[1]
                #define HWCE_Y_OUT_1_BASE_ADDR_CTX1_OFFSET       0x254

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_1_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_1_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_1_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_1_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_OUT_0_BASE_ADDR_CTX1:

Y_OUT_0_BASE_ADDR_CTX1
""""""""""""""""""""""

Base address of y<sub>out</sub>[0]

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

        
                // Base address of y<sub>out</sub>[0]
                #define HWCE_Y_OUT_0_BASE_ADDR_CTX1_OFFSET       0x258

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_out_0_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_out_0_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_out_0_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_out_0_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_2_BASE_ADDR_CTX1:

Y_IN_2_BASE_ADDR_CTX1
"""""""""""""""""""""

Base address of y<sub>in</sub>[2]

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

        
                // Base address of y<sub>in</sub>[2]
                #define HWCE_Y_IN_2_BASE_ADDR_CTX1_OFFSET        0x260

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_2_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_2_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_2_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_2_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_1_BASE_ADDR_CTX1:

Y_IN_1_BASE_ADDR_CTX1
"""""""""""""""""""""

Base address of y<sub>in</sub>[1]

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

        
                // Base address of y<sub>in</sub>[1]
                #define HWCE_Y_IN_1_BASE_ADDR_CTX1_OFFSET        0x264

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_1_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_1_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_1_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_1_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_Y_IN_0_BASE_ADDR_CTX1:

Y_IN_0_BASE_ADDR_CTX1
"""""""""""""""""""""

Base address of y<sub>in</sub>[0]

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

        
                // Base address of y<sub>in</sub>[0]
                #define HWCE_Y_IN_0_BASE_ADDR_CTX1_OFFSET        0x268

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_y_in_0_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_y_in_0_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_y_in_0_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_y_in_0_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_TRANS_SIZE_CTX1:

X_TRANS_SIZE_CTX1
"""""""""""""""""

Total number of words to be read for x<sub>in</sub>

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

        
                // Total number of words to be read for x<sub>in</sub>
                #define HWCE_X_TRANS_SIZE_CTX1_OFFSET            0x26c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_trans_size_ctx1_get(uint32_t base);
        static inline void hwce_x_trans_size_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_trans_size_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_trans_size_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_LINE_STRIDE_LENGTH_CTX1:

X_LINE_STRIDE_LENGTH_CTX1
"""""""""""""""""""""""""

Line stride and length for x<sub>in</sub>

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

        
                // Line stride and length for x<sub>in</sub>
                #define HWCE_X_LINE_STRIDE_LENGTH_CTX1_OFFSET    0x270

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_line_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_x_line_stride_length_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_line_stride_length_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_line_stride_length_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_FEAT_STRIDE_LENGTH_CTX1:

X_FEAT_STRIDE_LENGTH_CTX1
"""""""""""""""""""""""""

Feature (block) stride and length for x<sub>in</sub>

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

        
                // Feature (block) stride and length for x<sub>in</sub>
                #define HWCE_X_FEAT_STRIDE_LENGTH_CTX1_OFFSET    0x274

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_feat_stride_length_ctx1_get(uint32_t base);
        static inline void hwce_x_feat_stride_length_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_feat_stride_length_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_feat_stride_length_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_X_IN_BASE_ADDR_CTX1:

X_IN_BASE_ADDR_CTX1
"""""""""""""""""""

Base address of x<sub>in</sub>

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

        
                // Base address of x<sub>in</sub>
                #define HWCE_X_IN_BASE_ADDR_CTX1_OFFSET          0x278

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_x_in_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_x_in_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_x_in_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_x_in_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_W_BASE_ADDR_CTX1:

W_BASE_ADDR_CTX1
""""""""""""""""

Base address of W

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

        
                // Base address of W
                #define HWCE_W_BASE_ADDR_CTX1_OFFSET             0x27c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_w_base_addr_ctx1_get(uint32_t base);
        static inline void hwce_w_base_addr_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_w_base_addr_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_w_base_addr_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG0_CTX1:

JOB_CONFIG0_CTX1
""""""""""""""""

Job configuration register 0

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

        
                // Job configuration register 0
                #define HWCE_JOB_CONFIG0_CTX1_OFFSET             0x280

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config0_ctx1_get(uint32_t base);
        static inline void hwce_job_config0_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config0_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config0_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG1_CTX1:

JOB_CONFIG1_CTX1
""""""""""""""""

Job configuration register 1

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

        
                // Job configuration register 1
                #define HWCE_JOB_CONFIG1_CTX1_OFFSET             0x284

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config1_ctx1_get(uint32_t base);
        static inline void hwce_job_config1_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config1_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config1_ctx1 : public vp::reg_32
        {
        public:
        };

|

.. _hwce_JOB_CONFIG2_CTX1:

JOB_CONFIG2_CTX1
""""""""""""""""

Job configuration register 2

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

        
                // Job configuration register 2
                #define HWCE_JOB_CONFIG2_CTX1_OFFSET             0x288

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t hwce_job_config2_ctx1_get(uint32_t base);
        static inline void hwce_job_config2_ctx1_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) hwce_job_config2_ctx1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_hwce_job_config2_ctx1 : public vp::reg_32
        {
        public:
        };

|
