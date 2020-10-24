Input file: fe/ips/udma/udma_camera/doc/CAM_CPI_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |                         Name                         |Offset|Width|                      Description                       |
    +======================================================+======+=====+========================================================+
    |:ref:`CAM_RX_DEST<udma_cpi_CAM_RX_DEST>`              |     0|    7|Stream ID for the uDMA channel                          |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_RX_DATASIZE<udma_cpi_CAM_RX_DATASIZE>`      |     4|    2|Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit|
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_CFG_GLOB<udma_cpi_CAM_CFG_GLOB>`            |    32|   32|Global configuration register                           |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_CFG_LL<udma_cpi_CAM_CFG_LL>`                |    36|   32|Lower Left corner configuration register                |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_CFG_UR<udma_cpi_CAM_CFG_UR>`                |    40|   32|Upper Right corner configuration register               |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_CFG_SIZE<udma_cpi_CAM_CFG_SIZE>`            |    44|   32|Horizontal Resolution configuration register            |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_CFG_FILTER<udma_cpi_CAM_CFG_FILTER>`        |    48|   32|RGB coefficients configuration register                 |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CAM_VSYNC_POLARITY<udma_cpi_CAM_VSYNC_POLARITY>`|    52|   32|VSYNC Polarity register                                 |
    +------------------------------------------------------+------+-----+--------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_CPI_CAM_RX_DEST_OFFSET              0x0
        
                // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit
                #define UDMA_CPI_CAM_RX_DATASIZE_OFFSET          0x4
        
                // Global configuration register
                #define UDMA_CPI_CAM_CFG_GLOB_OFFSET             0x20
        
                // Lower Left corner configuration register
                #define UDMA_CPI_CAM_CFG_LL_OFFSET               0x24
        
                // Upper Right corner configuration register
                #define UDMA_CPI_CAM_CFG_UR_OFFSET               0x28
        
                // Horizontal Resolution configuration register
                #define UDMA_CPI_CAM_CFG_SIZE_OFFSET             0x2c
        
                // RGB coefficients configuration register
                #define UDMA_CPI_CAM_CFG_FILTER_OFFSET           0x30
        
                // VSYNC Polarity register
                #define UDMA_CPI_CAM_VSYNC_POLARITY_OFFSET       0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_rx_dest_get(uint32_t base);
        static inline void udma_cpi_cam_rx_dest_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_rx_datasize_get(uint32_t base);
        static inline void udma_cpi_cam_rx_datasize_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_cfg_glob_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_glob_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_cfg_ll_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_ll_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_cfg_ur_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_ur_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_cfg_size_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_size_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_cfg_filter_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_filter_set(uint32_t base, uint32_t value);

        static inline uint32_t udma_cpi_cam_vsync_polarity_get(uint32_t base);
        static inline void udma_cpi_cam_vsync_polarity_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_BIT                             0
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_WIDTH                           7
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_MASK                            0x7f
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_RESET                           0x7f
        
        // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit (access: R/W)
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_BIT                     0
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_WIDTH                   2
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_MASK                    0x3
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_RESET                   0x0
        
        // Frame dropping: - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_BIT                       0
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_WIDTH                     1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_MASK                      0x1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_RESET                     0x0
        
        // Sets how many frames should be dropped after each received. (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_BIT                      1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_WIDTH                    6
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_MASK                     0x7e
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_RESET                    0x0
        
        // Input frame slicing: - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_BIT                      7
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_WIDTH                    1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_MASK                     0x80
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_RESET                    0x0
        
        // Input frame format: - 3'b000: RGB565 - 3'b001: RGB555 - 3'b010: RGB444 - 3'b100: BYPASS_LITEND - 3b101: BYPASS_BIGEND (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_BIT                             8
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_WIDTH                           3
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_MASK                            0x700
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_RESET                           0x0
        
        // Right shift of final pixel value (DivFactor) NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_BIT                              11
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_WIDTH                            4
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_MASK                             0x7800
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_RESET                            0x0
        
        // Enable data rx from camera interface.  The enable/disable happens only at the start of a frame. - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_EN_BIT                                 31
        #define UDMA_CPI_CAM_CFG_GLOB_EN_WIDTH                               1
        #define UDMA_CPI_CAM_CFG_GLOB_EN_MASK                                0x80000000
        #define UDMA_CPI_CAM_CFG_GLOB_EN_RESET                               0x0
        
        // X coordinate of lower left corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_BIT                       0
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_MASK                      0xffff
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_RESET                     0x0
        
        // Y coordinate of lower left corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_BIT                       16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_MASK                      0xffff0000
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_RESET                     0x0
        
        // X coordinate of upper right corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_BIT                       0
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_MASK                      0xffff
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_RESET                     0x0
        
        // Y coordinate of upper right corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_BIT                       16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_MASK                      0xffff0000
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_RESET                     0x0
        
        // Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1). (access: R/W)
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_BIT                             0
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_WIDTH                           16
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_MASK                            0xffff
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_RESET                           0x0
        
        // Coefficient that multiplies the B component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_BIT                          0
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_MASK                         0xff
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_RESET                        0x0
        
        // Coefficient that multiplies the G component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_BIT                          8
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_MASK                         0xff00
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_RESET                        0x0
        
        // Coefficient that multiplies the R component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_BIT                          16
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_MASK                         0xff0000
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_RESET                        0x0
        
        // Set vsync polarity of CPI. - 1'b0: Active 0 - 1'b1: Active 1 (access: R/W)
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_BIT               0
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_WIDTH             1
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_MASK              0x1
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_RESET             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_GET(value)            (GAP_BEXTRACTU((value),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_GETS(value)           (GAP_BEXTRACT((value),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_SET(value,field)      (GAP_BINSERT((value),(field),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST(val)                  ((val) << 0)
        
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_GET(value)    (GAP_BEXTRACTU((value),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_GETS(value)   (GAP_BEXTRACT((value),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE(val)          ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_GET(value)     (GAP_BEXTRACTU((value),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_GETS(value)    (GAP_BEXTRACT((value),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_SET(value,field) (GAP_BINSERT((value),(field),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL(val)           ((val) << 1)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_GET(value)     (GAP_BEXTRACTU((value),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_GETS(value)    (GAP_BEXTRACT((value),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN(val)           ((val) << 7)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_GET(value)            (GAP_BEXTRACTU((value),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_GETS(value)           (GAP_BEXTRACT((value),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_SET(value,field)      (GAP_BINSERT((value),(field),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT(val)                  ((val) << 8)
        
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_GET(value)             (GAP_BEXTRACTU((value),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_GETS(value)            (GAP_BEXTRACT((value),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_SET(value,field)       (GAP_BINSERT((value),(field),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT(val)                   ((val) << 11)
        
        #define UDMA_CPI_CAM_CFG_GLOB_EN_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN(val)                      ((val) << 31)
        
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_GET(value)      (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_GETS(value)     (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_GET(value)      (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_GETS(value)     (GAP_BEXTRACT((value),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY(val)            ((val) << 16)
        
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_GET(value)      (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_GETS(value)     (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_GET(value)      (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_GETS(value)     (GAP_BEXTRACT((value),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY(val)            ((val) << 16)
        
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN(val)                  ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF(val)               ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF(val)               ((val) << 8)
        
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF(val)               ((val) << 16)
        
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY(val)    ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** UDMA_CPI_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t cam_rx_dest;  // Stream ID for the uDMA channel
            volatile uint32_t cam_rx_datasize;  // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit
            volatile uint32_t reserved_0[6];  // Reserved/Not used.
            volatile uint32_t cam_cfg_glob;  // Global configuration register
            volatile uint32_t cam_cfg_ll;  // Lower Left corner configuration register
            volatile uint32_t cam_cfg_ur;  // Upper Right corner configuration register
            volatile uint32_t cam_cfg_size;  // Horizontal Resolution configuration register
            volatile uint32_t cam_cfg_filter;  // RGB coefficients configuration register
            volatile uint32_t cam_vsync_polarity;  // VSYNC Polarity register
        } __attribute__((packed)) udma_cpi_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the uDMA channel. Default is 0x7F(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_rx_dest_t;
        
        typedef union {
          struct {
            unsigned int rx_datasize     :2 ; // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_rx_datasize_t;
        
        typedef union {
          struct {
            unsigned int framedrop_en    :1 ; // Frame dropping: - 1'b0: disable - 1'b1: enable
            unsigned int framedrop_val   :6 ; // Sets how many frames should be dropped after each received.
            unsigned int frameslice_en   :1 ; // Input frame slicing: - 1'b0: disable - 1'b1: enable
            unsigned int format          :3 ; // Input frame format: - 3'b000: RGB565 - 3'b001: RGB555 - 3'b010: RGB444 - 3'b100: BYPASS_LITEND - 3b101: BYPASS_BIGEND
            unsigned int shift           :4 ; // Right shift of final pixel value (DivFactor) NOTE: not used if FORMAT == BYPASS
            unsigned int padding0:16;
            unsigned int en              :1 ; // Enable data rx from camera interface.  The enable/disable happens only at the start of a frame. - 1'b0: disable - 1'b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_glob_t;
        
        typedef union {
          struct {
            unsigned int frameslice_llx  :16; // X coordinate of lower left corner of slice
            unsigned int frameslice_lly  :16; // Y coordinate of lower left corner of slice
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_ll_t;
        
        typedef union {
          struct {
            unsigned int frameslice_urx  :16; // X coordinate of upper right corner of slice
            unsigned int frameslice_ury  :16; // Y coordinate of upper right corner of slice
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_ur_t;
        
        typedef union {
          struct {
            unsigned int rowlen          :16; // Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_size_t;
        
        typedef union {
          struct {
            unsigned int b_coeff         :8 ; // Coefficient that multiplies the B component NOTE: not used if FORMAT == BYPASS
            unsigned int g_coeff         :8 ; // Coefficient that multiplies the G component NOTE: not used if FORMAT == BYPASS
            unsigned int r_coeff         :8 ; // Coefficient that multiplies the R component NOTE: not used if FORMAT == BYPASS
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_filter_t;
        
        typedef union {
          struct {
            unsigned int vsync_polarity  :1 ; // Set vsync polarity of CPI. - 1'b0: Active 0 - 1'b1: Active 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_vsync_polarity_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_udma_cpi : public vp::regmap
        {
        public:
            vp_udma_cpi_cam_rx_dest cam_rx_dest;
            vp_udma_cpi_cam_rx_datasize cam_rx_datasize;
            vp_udma_cpi_cam_cfg_glob cam_cfg_glob;
            vp_udma_cpi_cam_cfg_ll cam_cfg_ll;
            vp_udma_cpi_cam_cfg_ur cam_cfg_ur;
            vp_udma_cpi_cam_cfg_size cam_cfg_size;
            vp_udma_cpi_cam_cfg_filter cam_cfg_filter;
            vp_udma_cpi_cam_vsync_polarity cam_vsync_polarity;
        };

|

.. _udma_cpi_CAM_RX_DEST:

CAM_RX_DEST
"""""""""""

Stream ID for the uDMA channel

.. table:: 

    +-----+---+-------+-----------------------------------------------------------------+
    |Bit #|R/W| Name  |                           Description                           |
    +=====+===+=======+=================================================================+
    |6:0  |R/W|RX_DEST|Stream ID for the uDMA channel. Default is 0x7F(channel disabled)|
    +-----+---+-------+-----------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Stream ID for the uDMA channel
                #define UDMA_CPI_CAM_RX_DEST_OFFSET              0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_rx_dest_get(uint32_t base);
        static inline void udma_cpi_cam_rx_dest_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Stream ID for the uDMA channel. Default is 0x7F(channel disabled) (access: R/W)
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_BIT                             0
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_WIDTH                           7
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_MASK                            0x7f
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_RESET                           0x7f

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_GET(value)            (GAP_BEXTRACTU((value),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_GETS(value)           (GAP_BEXTRACT((value),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST_SET(value,field)      (GAP_BINSERT((value),(field),7,0))
        #define UDMA_CPI_CAM_RX_DEST_RX_DEST(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_dest         :7 ; // Stream ID for the uDMA channel. Default is 0x7F(channel disabled)
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_rx_dest_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_rx_dest : public vp::reg_8
        {
        public:
            inline void rx_dest_set(uint8_t value);
            inline uint8_t rx_dest_get();
        };

|

.. _udma_cpi_CAM_RX_DATASIZE:

CAM_RX_DATASIZE
"""""""""""""""

Transfer datasize
00: 8bit
01: 16bit
10: 24bit
11: 32bit

.. table:: 

    +-----+---+-----------+--------------------------------------------------------+
    |Bit #|R/W|   Name    |                      Description                       |
    +=====+===+===========+========================================================+
    |1:0  |R/W|RX_DATASIZE|Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit|
    +-----+---+-----------+--------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit
                #define UDMA_CPI_CAM_RX_DATASIZE_OFFSET          0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_rx_datasize_get(uint32_t base);
        static inline void udma_cpi_cam_rx_datasize_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit (access: R/W)
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_BIT                     0
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_WIDTH                   2
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_MASK                    0x3
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_RESET                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_GET(value)    (GAP_BEXTRACTU((value),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_GETS(value)   (GAP_BEXTRACT((value),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE_SET(value,field) (GAP_BINSERT((value),(field),2,0))
        #define UDMA_CPI_CAM_RX_DATASIZE_RX_DATASIZE(val)          ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rx_datasize     :2 ; // Transfer datasize 00: 8bit 01: 16bit 10: 24bit 11: 32bit
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_rx_datasize_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_rx_datasize : public vp::reg_8
        {
        public:
            inline void rx_datasize_set(uint8_t value);
            inline uint8_t rx_datasize_get();
        };

|

.. _udma_cpi_CAM_CFG_GLOB:

CAM_CFG_GLOB
""""""""""""

Global configuration register

.. table:: 

    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|    Name     |                                                         Description                                                          |
    +=====+===+=============+==============================================================================================================================+
    |    0|R/W|FRAMEDROP_EN |Frame dropping: - 1'b0: disable - 1'b1: enable                                                                                |
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |6:1  |R/W|FRAMEDROP_VAL|Sets how many frames should be dropped after each received.                                                                   |
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |    7|R/W|FRAMESLICE_EN|Input frame slicing: - 1'b0: disable - 1'b1: enable                                                                           |
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|FORMAT       |Input frame format: - 3'b000: RGB565 - 3'b001: RGB555 - 3'b010: RGB444 - 3'b100: BYPASS_LITEND - 3’b101: BYPASS_BIGEND        |
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |14:11|R/W|SHIFT        |Right shift of final pixel value (DivFactor) NOTE: not used if FORMAT == BYPASS                                               |
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+
    |31   |R/W|EN           |Enable data rx from camera interface.  The enable/disable happens only at the start of a frame. - 1'b0: disable - 1'b1: enable|
    +-----+---+-------------+------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Global configuration register
                #define UDMA_CPI_CAM_CFG_GLOB_OFFSET             0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_cfg_glob_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_glob_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Frame dropping: - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_BIT                       0
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_WIDTH                     1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_MASK                      0x1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_RESET                     0x0
        
        // Sets how many frames should be dropped after each received. (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_BIT                      1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_WIDTH                    6
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_MASK                     0x7e
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_RESET                    0x0
        
        // Input frame slicing: - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_BIT                      7
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_WIDTH                    1
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_MASK                     0x80
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_RESET                    0x0
        
        // Input frame format: - 3'b000: RGB565 - 3'b001: RGB555 - 3'b010: RGB444 - 3'b100: BYPASS_LITEND - 3b101: BYPASS_BIGEND (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_BIT                             8
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_WIDTH                           3
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_MASK                            0x700
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_RESET                           0x0
        
        // Right shift of final pixel value (DivFactor) NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_BIT                              11
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_WIDTH                            4
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_MASK                             0x7800
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_RESET                            0x0
        
        // Enable data rx from camera interface.  The enable/disable happens only at the start of a frame. - 1'b0: disable - 1'b1: enable (access: R/W)
        #define UDMA_CPI_CAM_CFG_GLOB_EN_BIT                                 31
        #define UDMA_CPI_CAM_CFG_GLOB_EN_WIDTH                               1
        #define UDMA_CPI_CAM_CFG_GLOB_EN_MASK                                0x80000000
        #define UDMA_CPI_CAM_CFG_GLOB_EN_RESET                               0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_GET(value)      (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_GETS(value)     (GAP_BEXTRACT((value),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_EN(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_GET(value)     (GAP_BEXTRACTU((value),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_GETS(value)    (GAP_BEXTRACT((value),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL_SET(value,field) (GAP_BINSERT((value),(field),6,1))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMEDROP_VAL(val)           ((val) << 1)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_GET(value)     (GAP_BEXTRACTU((value),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_GETS(value)    (GAP_BEXTRACT((value),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN_SET(value,field) (GAP_BINSERT((value),(field),1,7))
        #define UDMA_CPI_CAM_CFG_GLOB_FRAMESLICE_EN(val)           ((val) << 7)
        
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_GET(value)            (GAP_BEXTRACTU((value),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_GETS(value)           (GAP_BEXTRACT((value),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT_SET(value,field)      (GAP_BINSERT((value),(field),3,8))
        #define UDMA_CPI_CAM_CFG_GLOB_FORMAT(val)                  ((val) << 8)
        
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_GET(value)             (GAP_BEXTRACTU((value),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_GETS(value)            (GAP_BEXTRACT((value),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT_SET(value,field)       (GAP_BINSERT((value),(field),4,11))
        #define UDMA_CPI_CAM_CFG_GLOB_SHIFT(val)                   ((val) << 11)
        
        #define UDMA_CPI_CAM_CFG_GLOB_EN_GET(value)                (GAP_BEXTRACTU((value),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN_GETS(value)               (GAP_BEXTRACT((value),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN_SET(value,field)          (GAP_BINSERT((value),(field),1,31))
        #define UDMA_CPI_CAM_CFG_GLOB_EN(val)                      ((val) << 31)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int framedrop_en    :1 ; // Frame dropping: - 1'b0: disable - 1'b1: enable
            unsigned int framedrop_val   :6 ; // Sets how many frames should be dropped after each received.
            unsigned int frameslice_en   :1 ; // Input frame slicing: - 1'b0: disable - 1'b1: enable
            unsigned int format          :3 ; // Input frame format: - 3'b000: RGB565 - 3'b001: RGB555 - 3'b010: RGB444 - 3'b100: BYPASS_LITEND - 3b101: BYPASS_BIGEND
            unsigned int shift           :4 ; // Right shift of final pixel value (DivFactor) NOTE: not used if FORMAT == BYPASS
            unsigned int padding0:16;
            unsigned int en              :1 ; // Enable data rx from camera interface.  The enable/disable happens only at the start of a frame. - 1'b0: disable - 1'b1: enable
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_glob_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_cfg_glob : public vp::reg_32
        {
        public:
            inline void framedrop_en_set(uint32_t value);
            inline uint32_t framedrop_en_get();
            inline void framedrop_val_set(uint32_t value);
            inline uint32_t framedrop_val_get();
            inline void frameslice_en_set(uint32_t value);
            inline uint32_t frameslice_en_get();
            inline void format_set(uint32_t value);
            inline uint32_t format_get();
            inline void shift_set(uint32_t value);
            inline uint32_t shift_get();
            inline void en_set(uint32_t value);
            inline uint32_t en_get();
        };

|

.. _udma_cpi_CAM_CFG_LL:

CAM_CFG_LL
""""""""""

Lower Left corner configuration register

.. table:: 

    +-----+---+--------------+------------------------------------------+
    |Bit #|R/W|     Name     |               Description                |
    +=====+===+==============+==========================================+
    |15:0 |R/W|FRAMESLICE_LLX|X coordinate of lower left corner of slice|
    +-----+---+--------------+------------------------------------------+
    |31:16|R/W|FRAMESLICE_LLY|Y coordinate of lower left corner of slice|
    +-----+---+--------------+------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Lower Left corner configuration register
                #define UDMA_CPI_CAM_CFG_LL_OFFSET               0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_cfg_ll_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_ll_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // X coordinate of lower left corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_BIT                       0
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_MASK                      0xffff
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_RESET                     0x0
        
        // Y coordinate of lower left corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_BIT                       16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_MASK                      0xffff0000
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_GET(value)      (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_GETS(value)     (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLX(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_GET(value)      (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_GETS(value)     (GAP_BEXTRACT((value),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CPI_CAM_CFG_LL_FRAMESLICE_LLY(val)            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int frameslice_llx  :16; // X coordinate of lower left corner of slice
            unsigned int frameslice_lly  :16; // Y coordinate of lower left corner of slice
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_ll_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_cfg_ll : public vp::reg_32
        {
        public:
            inline void frameslice_llx_set(uint32_t value);
            inline uint32_t frameslice_llx_get();
            inline void frameslice_lly_set(uint32_t value);
            inline uint32_t frameslice_lly_get();
        };

|

.. _udma_cpi_CAM_CFG_UR:

CAM_CFG_UR
""""""""""

Upper Right corner configuration register

.. table:: 

    +-----+---+--------------+-------------------------------------------+
    |Bit #|R/W|     Name     |                Description                |
    +=====+===+==============+===========================================+
    |15:0 |R/W|FRAMESLICE_URX|X coordinate of upper right corner of slice|
    +-----+---+--------------+-------------------------------------------+
    |31:16|R/W|FRAMESLICE_URY|Y coordinate of upper right corner of slice|
    +-----+---+--------------+-------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Upper Right corner configuration register
                #define UDMA_CPI_CAM_CFG_UR_OFFSET               0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_cfg_ur_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_ur_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // X coordinate of upper right corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_BIT                       0
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_MASK                      0xffff
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_RESET                     0x0
        
        // Y coordinate of upper right corner of slice (access: R/W)
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_BIT                       16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_WIDTH                     16
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_MASK                      0xffff0000
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_RESET                     0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_GET(value)      (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_GETS(value)     (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX_SET(value,field) (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URX(val)            ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_GET(value)      (GAP_BEXTRACTU((value),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_GETS(value)     (GAP_BEXTRACT((value),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY_SET(value,field) (GAP_BINSERT((value),(field),16,16))
        #define UDMA_CPI_CAM_CFG_UR_FRAMESLICE_URY(val)            ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int frameslice_urx  :16; // X coordinate of upper right corner of slice
            unsigned int frameslice_ury  :16; // Y coordinate of upper right corner of slice
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_ur_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_cfg_ur : public vp::reg_32
        {
        public:
            inline void frameslice_urx_set(uint32_t value);
            inline uint32_t frameslice_urx_get();
            inline void frameslice_ury_set(uint32_t value);
            inline uint32_t frameslice_ury_get();
        };

|

.. _udma_cpi_CAM_CFG_SIZE:

CAM_CFG_SIZE
""""""""""""

Horizontal Resolution configuration register

.. table:: 

    +-----+---+------+----------------------------------------------------------------------------------------------------------+
    |Bit #|R/W| Name |                                               Description                                                |
    +=====+===+======+==========================================================================================================+
    |15:0 |R/W|ROWLEN|Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1).|
    +-----+---+------+----------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // Horizontal Resolution configuration register
                #define UDMA_CPI_CAM_CFG_SIZE_OFFSET             0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_cfg_size_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_size_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1). (access: R/W)
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_BIT                             0
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_WIDTH                           16
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_MASK                            0xffff
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define UDMA_CPI_CAM_CFG_SIZE_ROWLEN(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int rowlen          :16; // Horizontal Resolution. It is used for slice mode. Value set into the bitfield must be equal to (rowlen-1).
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_size_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_cfg_size : public vp::reg_32
        {
        public:
            inline void rowlen_set(uint32_t value);
            inline uint32_t rowlen_get();
        };

|

.. _udma_cpi_CAM_CFG_FILTER:

CAM_CFG_FILTER
""""""""""""""

RGB coefficients configuration register

.. table:: 

    +-----+---+-------+------------------------------------------------------------------------------+
    |Bit #|R/W| Name  |                                 Description                                  |
    +=====+===+=======+==============================================================================+
    |7:0  |R/W|B_COEFF|Coefficient that multiplies the B component NOTE: not used if FORMAT == BYPASS|
    +-----+---+-------+------------------------------------------------------------------------------+
    |15:8 |R/W|G_COEFF|Coefficient that multiplies the G component NOTE: not used if FORMAT == BYPASS|
    +-----+---+-------+------------------------------------------------------------------------------+
    |23:16|R/W|R_COEFF|Coefficient that multiplies the R component NOTE: not used if FORMAT == BYPASS|
    +-----+---+-------+------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // RGB coefficients configuration register
                #define UDMA_CPI_CAM_CFG_FILTER_OFFSET           0x30

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_cfg_filter_get(uint32_t base);
        static inline void udma_cpi_cam_cfg_filter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Coefficient that multiplies the B component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_BIT                          0
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_MASK                         0xff
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_RESET                        0x0
        
        // Coefficient that multiplies the G component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_BIT                          8
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_MASK                         0xff00
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_RESET                        0x0
        
        // Coefficient that multiplies the R component NOTE: not used if FORMAT == BYPASS (access: R/W)
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_BIT                          16
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_WIDTH                        8
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_MASK                         0xff0000
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_RESET                        0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,0))
        #define UDMA_CPI_CAM_CFG_FILTER_B_COEFF(val)               ((val) << 0)
        
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,8))
        #define UDMA_CPI_CAM_CFG_FILTER_G_COEFF(val)               ((val) << 8)
        
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_GET(value)         (GAP_BEXTRACTU((value),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_GETS(value)        (GAP_BEXTRACT((value),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF_SET(value,field)   (GAP_BINSERT((value),(field),8,16))
        #define UDMA_CPI_CAM_CFG_FILTER_R_COEFF(val)               ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int b_coeff         :8 ; // Coefficient that multiplies the B component NOTE: not used if FORMAT == BYPASS
            unsigned int g_coeff         :8 ; // Coefficient that multiplies the G component NOTE: not used if FORMAT == BYPASS
            unsigned int r_coeff         :8 ; // Coefficient that multiplies the R component NOTE: not used if FORMAT == BYPASS
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_cfg_filter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_cfg_filter : public vp::reg_32
        {
        public:
            inline void b_coeff_set(uint32_t value);
            inline uint32_t b_coeff_get();
            inline void g_coeff_set(uint32_t value);
            inline uint32_t g_coeff_get();
            inline void r_coeff_set(uint32_t value);
            inline uint32_t r_coeff_get();
        };

|

.. _udma_cpi_CAM_VSYNC_POLARITY:

CAM_VSYNC_POLARITY
""""""""""""""""""

VSYNC Polarity register

.. table:: 

    +-----+---+--------------+------------------------------------------------------------+
    |Bit #|R/W|     Name     |                        Description                         |
    +=====+===+==============+============================================================+
    |    0|R/W|VSYNC_POLARITY|Set vsync polarity of CPI. - 1'b0: Active 0 - 1'b1: Active 1|
    +-----+---+--------------+------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // VSYNC Polarity register
                #define UDMA_CPI_CAM_VSYNC_POLARITY_OFFSET       0x34

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t udma_cpi_cam_vsync_polarity_get(uint32_t base);
        static inline void udma_cpi_cam_vsync_polarity_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // Set vsync polarity of CPI. - 1'b0: Active 0 - 1'b1: Active 1 (access: R/W)
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_BIT               0
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_WIDTH             1
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_MASK              0x1
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_RESET             0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_GET(value) (GAP_BEXTRACTU((value),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_GETS(value) (GAP_BEXTRACT((value),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY_SET(value,field) (GAP_BINSERT((value),(field),1,0))
        #define UDMA_CPI_CAM_VSYNC_POLARITY_VSYNC_POLARITY(val)    ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int vsync_polarity  :1 ; // Set vsync polarity of CPI. - 1'b0: Active 0 - 1'b1: Active 1
          };
          unsigned int raw;
        } __attribute__((packed)) udma_cpi_cam_vsync_polarity_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_udma_cpi_cam_vsync_polarity : public vp::reg_32
        {
        public:
            inline void vsync_polarity_set(uint32_t value);
            inline uint32_t vsync_polarity_get();
        };

|
