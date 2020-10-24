Input file: fe/ips/apb/apb_adv_timer/doc/APB_ADV_TIMER_reference.xlsx

Register map
^^^^^^^^^^^^


Overview
""""""""

.. table:: 

    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |                         Name                          |Offset|Width|                      Description                       |
    +=======================================================+======+=====+========================================================+
    |:ref:`T0_CMD<adv_timer_T0_CMD>`                        |     0|   32|ADV_TIMER0 command register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_CONFIG<adv_timer_T0_CONFIG>`                  |     4|   32|ADV_TIMER0 configuration register.                      |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_THRESHOLD<adv_timer_T0_THRESHOLD>`            |     8|   32|ADV_TIMER0 threshold configuration register.            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL0<adv_timer_T0_TH_CHANNEL0>`        |    12|   32|ADV_TIMER0 channel 0 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL1<adv_timer_T0_TH_CHANNEL1>`        |    16|   32|ADV_TIMER0 channel 1 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL2<adv_timer_T0_TH_CHANNEL2>`        |    20|   32|ADV_TIMER0 channel 2 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL3<adv_timer_T0_TH_CHANNEL3>`        |    24|   32|ADV_TIMER0 channel 3 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL0_LUT<adv_timer_T0_TH_CHANNEL0_LUT>`|    28|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL1_LUT<adv_timer_T0_TH_CHANNEL1_LUT>`|    32|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL2_LUT<adv_timer_T0_TH_CHANNEL2_LUT>`|    36|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_TH_CHANNEL3_LUT<adv_timer_T0_TH_CHANNEL3_LUT>`|    40|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T0_COUNTER<adv_timer_T0_COUNTER>`                |    44|   32|ADV_TIMER0 counter register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_CMD<adv_timer_T1_CMD>`                        |    64|   32|ADV_TIMER1 command register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_CONFIG<adv_timer_T1_CONFIG>`                  |    68|   32|ADV_TIMER1 configuration register.                      |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_THRESHOLD<adv_timer_T1_THRESHOLD>`            |    72|   32|ADV_TIMER1 threshold configuration register.            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL0<adv_timer_T1_TH_CHANNEL0>`        |    76|   32|ADV_TIMER1 channel 0 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL1<adv_timer_T1_TH_CHANNEL1>`        |    80|   32|ADV_TIMER1 channel 1 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL2<adv_timer_T1_TH_CHANNEL2>`        |    84|   32|ADV_TIMER1 channel 2 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL3<adv_timer_T1_TH_CHANNEL3>`        |    88|   32|ADV_TIMER1 channel 3 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL0_LUT<adv_timer_T1_TH_CHANNEL0_LUT>`|    92|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL1_LUT<adv_timer_T1_TH_CHANNEL1_LUT>`|    96|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL2_LUT<adv_timer_T1_TH_CHANNEL2_LUT>`|   100|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_TH_CHANNEL3_LUT<adv_timer_T1_TH_CHANNEL3_LUT>`|   104|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T1_COUNTER<adv_timer_T1_COUNTER>`                |   108|   32|ADV_TIMER1 counter register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_CMD<adv_timer_T2_CMD>`                        |   128|   32|ADV_TIMER2 command register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_CONFIG<adv_timer_T2_CONFIG>`                  |   132|   32|ADV_TIMER2 configuration register.                      |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_THRESHOLD<adv_timer_T2_THRESHOLD>`            |   136|   32|ADV_TIMER2 threshold configuration register.            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL0<adv_timer_T2_TH_CHANNEL0>`        |   140|   32|ADV_TIMER2 channel 0 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL1<adv_timer_T2_TH_CHANNEL1>`        |   144|   32|ADV_TIMER2 channel 1 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL2<adv_timer_T2_TH_CHANNEL2>`        |   148|   32|ADV_TIMER2 channel 2 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL3<adv_timer_T2_TH_CHANNEL3>`        |   152|   32|ADV_TIMER2 channel 3 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL0_LUT<adv_timer_T2_TH_CHANNEL0_LUT>`|   156|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL1_LUT<adv_timer_T2_TH_CHANNEL1_LUT>`|   160|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL2_LUT<adv_timer_T2_TH_CHANNEL2_LUT>`|   164|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_TH_CHANNEL3_LUT<adv_timer_T2_TH_CHANNEL3_LUT>`|   168|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T2_COUNTER<adv_timer_T2_COUNTER>`                |   172|   32|ADV_TIMER2 counter register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_CMD<adv_timer_T3_CMD>`                        |   192|   32|ADV_TIMER3 command register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_CONFIG<adv_timer_T3_CONFIG>`                  |   196|   32|ADV_TIMER3 configuration register.                      |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_THRESHOLD<adv_timer_T3_THRESHOLD>`            |   200|   32|ADV_TIMER3 threshold configuration register.            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL0<adv_timer_T3_TH_CHANNEL0>`        |   204|   32|ADV_TIMER3 channel 0 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL1<adv_timer_T3_TH_CHANNEL1>`        |   208|   32|ADV_TIMER3 channel 1 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL2<adv_timer_T3_TH_CHANNEL2>`        |   212|   32|ADV_TIMER3 channel 2 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL3<adv_timer_T3_TH_CHANNEL3>`        |   216|   32|ADV_TIMER3 channel 3 threshold configuration register.  |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL0_LUT<adv_timer_T3_TH_CHANNEL0_LUT>`|   220|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL1_LUT<adv_timer_T3_TH_CHANNEL1_LUT>`|   224|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL2_LUT<adv_timer_T3_TH_CHANNEL2_LUT>`|   228|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_TH_CHANNEL3_LUT<adv_timer_T3_TH_CHANNEL3_LUT>`|   232|   32|nan                                                     |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`T3_COUNTER<adv_timer_T3_COUNTER>`                |   236|   32|ADV_TIMER3 counter register.                            |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`EVENT_CFG<adv_timer_EVENT_CFG>`                  |   256|   32|ADV_TIMERS events configuration register.               |
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+
    |:ref:`CG<adv_timer_CG>`                                |   260|   32|ADV_TIMERS channels clock gating configuration register.|
    +-------------------------------------------------------+------+-----+--------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 command register.
                #define ADV_TIMER_T0_CMD_OFFSET                  0x0
        
                // ADV_TIMER0 configuration register.
                #define ADV_TIMER_T0_CONFIG_OFFSET               0x4
        
                // ADV_TIMER0 threshold configuration register.
                #define ADV_TIMER_T0_THRESHOLD_OFFSET            0x8
        
                // ADV_TIMER0 channel 0 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL0_OFFSET          0xc
        
                // ADV_TIMER0 channel 1 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL1_OFFSET          0x10
        
                // ADV_TIMER0 channel 2 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL2_OFFSET          0x14
        
                // ADV_TIMER0 channel 3 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL3_OFFSET          0x18
        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL0_LUT_OFFSET      0x1c
        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL1_LUT_OFFSET      0x20
        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL2_LUT_OFFSET      0x24
        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL3_LUT_OFFSET      0x28
        
                // ADV_TIMER0 counter register.
                #define ADV_TIMER_T0_COUNTER_OFFSET              0x2c
        
                // ADV_TIMER1 command register.
                #define ADV_TIMER_T1_CMD_OFFSET                  0x40
        
                // ADV_TIMER1 configuration register.
                #define ADV_TIMER_T1_CONFIG_OFFSET               0x44
        
                // ADV_TIMER1 threshold configuration register.
                #define ADV_TIMER_T1_THRESHOLD_OFFSET            0x48
        
                // ADV_TIMER1 channel 0 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL0_OFFSET          0x4c
        
                // ADV_TIMER1 channel 1 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL1_OFFSET          0x50
        
                // ADV_TIMER1 channel 2 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL2_OFFSET          0x54
        
                // ADV_TIMER1 channel 3 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL3_OFFSET          0x58
        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL0_LUT_OFFSET      0x5c
        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL1_LUT_OFFSET      0x60
        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL2_LUT_OFFSET      0x64
        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL3_LUT_OFFSET      0x68
        
                // ADV_TIMER1 counter register.
                #define ADV_TIMER_T1_COUNTER_OFFSET              0x6c
        
                // ADV_TIMER2 command register.
                #define ADV_TIMER_T2_CMD_OFFSET                  0x80
        
                // ADV_TIMER2 configuration register.
                #define ADV_TIMER_T2_CONFIG_OFFSET               0x84
        
                // ADV_TIMER2 threshold configuration register.
                #define ADV_TIMER_T2_THRESHOLD_OFFSET            0x88
        
                // ADV_TIMER2 channel 0 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL0_OFFSET          0x8c
        
                // ADV_TIMER2 channel 1 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL1_OFFSET          0x90
        
                // ADV_TIMER2 channel 2 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL2_OFFSET          0x94
        
                // ADV_TIMER2 channel 3 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL3_OFFSET          0x98
        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL0_LUT_OFFSET      0x9c
        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL1_LUT_OFFSET      0xa0
        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL2_LUT_OFFSET      0xa4
        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL3_LUT_OFFSET      0xa8
        
                // ADV_TIMER2 counter register.
                #define ADV_TIMER_T2_COUNTER_OFFSET              0xac
        
                // ADV_TIMER3 command register.
                #define ADV_TIMER_T3_CMD_OFFSET                  0xc0
        
                // ADV_TIMER3 configuration register.
                #define ADV_TIMER_T3_CONFIG_OFFSET               0xc4
        
                // ADV_TIMER3 threshold configuration register.
                #define ADV_TIMER_T3_THRESHOLD_OFFSET            0xc8
        
                // ADV_TIMER3 channel 0 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL0_OFFSET          0xcc
        
                // ADV_TIMER3 channel 1 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL1_OFFSET          0xd0
        
                // ADV_TIMER3 channel 2 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL2_OFFSET          0xd4
        
                // ADV_TIMER3 channel 3 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL3_OFFSET          0xd8
        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL0_LUT_OFFSET      0xdc
        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL1_LUT_OFFSET      0xe0
        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL2_LUT_OFFSET      0xe4
        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL3_LUT_OFFSET      0xe8
        
                // ADV_TIMER3 counter register.
                #define ADV_TIMER_T3_COUNTER_OFFSET              0xec
        
                // ADV_TIMERS events configuration register.
                #define ADV_TIMER_EVENT_CFG_OFFSET               0x100
        
                // ADV_TIMERS channels clock gating configuration register.
                #define ADV_TIMER_CG_OFFSET                      0x104

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_cmd_get(uint32_t base);
        static inline void adv_timer_t0_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_config_get(uint32_t base);
        static inline void adv_timer_t0_config_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_threshold_get(uint32_t base);
        static inline void adv_timer_t0_threshold_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel0_get(uint32_t base);
        static inline void adv_timer_t0_th_channel0_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel1_get(uint32_t base);
        static inline void adv_timer_t0_th_channel1_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel2_get(uint32_t base);
        static inline void adv_timer_t0_th_channel2_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel3_get(uint32_t base);
        static inline void adv_timer_t0_th_channel3_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel0_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel1_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel2_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel3_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t0_counter_get(uint32_t base);
        static inline void adv_timer_t0_counter_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_cmd_get(uint32_t base);
        static inline void adv_timer_t1_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_config_get(uint32_t base);
        static inline void adv_timer_t1_config_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_threshold_get(uint32_t base);
        static inline void adv_timer_t1_threshold_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel0_get(uint32_t base);
        static inline void adv_timer_t1_th_channel0_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel1_get(uint32_t base);
        static inline void adv_timer_t1_th_channel1_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel2_get(uint32_t base);
        static inline void adv_timer_t1_th_channel2_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel3_get(uint32_t base);
        static inline void adv_timer_t1_th_channel3_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel0_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel1_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel2_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel3_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t1_counter_get(uint32_t base);
        static inline void adv_timer_t1_counter_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_cmd_get(uint32_t base);
        static inline void adv_timer_t2_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_config_get(uint32_t base);
        static inline void adv_timer_t2_config_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_threshold_get(uint32_t base);
        static inline void adv_timer_t2_threshold_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel0_get(uint32_t base);
        static inline void adv_timer_t2_th_channel0_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel1_get(uint32_t base);
        static inline void adv_timer_t2_th_channel1_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel2_get(uint32_t base);
        static inline void adv_timer_t2_th_channel2_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel3_get(uint32_t base);
        static inline void adv_timer_t2_th_channel3_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel0_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel1_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel2_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel3_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t2_counter_get(uint32_t base);
        static inline void adv_timer_t2_counter_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_cmd_get(uint32_t base);
        static inline void adv_timer_t3_cmd_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_config_get(uint32_t base);
        static inline void adv_timer_t3_config_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_threshold_get(uint32_t base);
        static inline void adv_timer_t3_threshold_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel0_get(uint32_t base);
        static inline void adv_timer_t3_th_channel0_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel1_get(uint32_t base);
        static inline void adv_timer_t3_th_channel1_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel2_get(uint32_t base);
        static inline void adv_timer_t3_th_channel2_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel3_get(uint32_t base);
        static inline void adv_timer_t3_th_channel3_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel0_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel1_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel2_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel3_lut_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_t3_counter_get(uint32_t base);
        static inline void adv_timer_t3_counter_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_event_cfg_get(uint32_t base);
        static inline void adv_timer_event_cfg_set(uint32_t base, uint32_t value);

        static inline uint32_t adv_timer_cg_get(uint32_t base);
        static inline void adv_timer_cg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 start command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_START_BIT                                   0
        #define ADV_TIMER_T0_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T0_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T0_CMD_START_RESET                                 0x0
        
        // ADV_TIMER0 stop command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T0_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T0_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T0_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER0 update command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T0_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T0_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T0_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER0 reset command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T0_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T0_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T0_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER0 arm command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T0_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T0_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T0_CMD_ARM_RESET                                   0x0
        
        // nan (access: nan)
        #define ADV_TIMER_T0_CMD_RFU_BIT                                     5
        #define ADV_TIMER_T0_CMD_RFU_WIDTH                                   27
        #define ADV_TIMER_T0_CMD_RFU_MASK                                    0xffffffe0
        #define ADV_TIMER_T0_CMD_RFU_RESET                                   0x0
        
        // ADV_TIMER0 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T0_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T0_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T0_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T0_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER0 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T0_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T0_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T0_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T0_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER0 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T0_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T0_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T0_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T0_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER0 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER0 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T0_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T0_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T0_CONFIG_PRESC_RESET                              0x0
        
        // ADV_TIMER0 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER0 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_RESET                           0x0
        
        // ADV_TIMER0 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_RESET                          0x0
        
        // ADV_TIMER0 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_RESET                          0x0
        
        // ADV_TIMER0 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_RESET                          0x0
        
        // ADV_TIMER0 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_RESET                          0x0
        
        // ADV_TIMER0 counter value. (access: R)
        #define ADV_TIMER_T0_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T0_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T0_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T0_COUNTER_COUNTER_RESET                           0x0
        
        // ADV_TIMER1 start command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_START_BIT                                   0
        #define ADV_TIMER_T1_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T1_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T1_CMD_START_RESET                                 0x0
        
        // ADV_TIMER1 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T1_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T1_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T1_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER1 update command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T1_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T1_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T1_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER1 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T1_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T1_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T1_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER1 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T1_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T1_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T1_CMD_ARM_RESET                                   0x0
        
        // ADV_TIMER1 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T1_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T1_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T1_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T1_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER1 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T1_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T1_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T1_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T1_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER1 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T1_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T1_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T1_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T1_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER1 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER1 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T1_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T1_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T1_CONFIG_PRESC_RESET                              0x0
        
        // ADV_TIMER1 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER1 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_RESET                           0x0
        
        // ADV_TIMER1 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_RESET                          0x0
        
        // ADV_TIMER1 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_RESET                          0x0
        
        // ADV_TIMER1 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_RESET                          0x0
        
        // ADV_TIMER1 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_RESET                          0x0
        
        // ADV_TIMER1 counter value. (access: R)
        #define ADV_TIMER_T1_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T1_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T1_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T1_COUNTER_COUNTER_RESET                           0x0
        
        // ADV_TIMER2 start command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_START_BIT                                   0
        #define ADV_TIMER_T2_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T2_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T2_CMD_START_RESET                                 0x0
        
        // ADV_TIMER2 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T2_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T2_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T2_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER2 update command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T2_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T2_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T2_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER2 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T2_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T2_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T2_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER2 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T2_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T2_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T2_CMD_ARM_RESET                                   0x0
        
        // ADV_TIMER2 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T2_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T2_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T2_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T2_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER2 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T2_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T2_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T2_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T2_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER2 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T2_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T2_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T2_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T2_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER2 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER2 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T2_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T2_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T2_CONFIG_PRESC_RESET                              0x0
        
        // ADV_TIMER2 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER2 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_RESET                           0x0
        
        // ADV_TIMER2 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_RESET                          0x0
        
        // ADV_TIMER2 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_RESET                          0x0
        
        // ADV_TIMER2 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_RESET                          0x0
        
        // ADV_TIMER2 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_RESET                          0x0
        
        // ADV_TIMER2 counter value. (access: R)
        #define ADV_TIMER_T2_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T2_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T2_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T2_COUNTER_COUNTER_RESET                           0x0
        
        // ADV_TIMER3 start command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_START_BIT                                   0
        #define ADV_TIMER_T3_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T3_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T3_CMD_START_RESET                                 0x0
        
        // ADV_TIMER3 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T3_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T3_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T3_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER3 update command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T3_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T3_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T3_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER3 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T3_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T3_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T3_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER3 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T3_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T3_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T3_CMD_ARM_RESET                                   0x0
        
        // ADV_TIMER3 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T3_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T3_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T3_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T3_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER3 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T3_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T3_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T3_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T3_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER3 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T3_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T3_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T3_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T3_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER3 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER3 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T3_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T3_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T3_CONFIG_PRESC_RESET                              0x0
        
        // ADV_TIMER3 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER3 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_RESET                           0x0
        
        // ADV_TIMER3 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_RESET                          0x0
        
        // ADV_TIMER3 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_RESET                          0x0
        
        // ADV_TIMER3 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_RESET                          0x0
        
        // ADV_TIMER3 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_RESET                          0x0
        
        // ADV_TIMER3 counter value. (access: R)
        #define ADV_TIMER_T3_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T3_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T3_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T3_COUNTER_COUNTER_RESET                           0x0
        
        // ADV_TIMER output event 0 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL0_BIT                                 0
        #define ADV_TIMER_EVENT_CFG_SEL0_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL0_MASK                                0xf
        #define ADV_TIMER_EVENT_CFG_SEL0_RESET                               0x0
        
        // ADV_TIMER output event 1 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL1_BIT                                 4
        #define ADV_TIMER_EVENT_CFG_SEL1_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL1_MASK                                0xf0
        #define ADV_TIMER_EVENT_CFG_SEL1_RESET                               0x0
        
        // ADV_TIMER output event 2 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL2_BIT                                 8
        #define ADV_TIMER_EVENT_CFG_SEL2_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL2_MASK                                0xf00
        #define ADV_TIMER_EVENT_CFG_SEL2_RESET                               0x0
        
        // ADV_TIMER output event 3 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL3_BIT                                 12
        #define ADV_TIMER_EVENT_CFG_SEL3_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL3_MASK                                0xf000
        #define ADV_TIMER_EVENT_CFG_SEL3_RESET                               0x0
        
        // ADV_TIMER output event enable configuration bitfield. ENA[i]=1 enables output event i generation. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_ENA_BIT                                  16
        #define ADV_TIMER_EVENT_CFG_ENA_WIDTH                                4
        #define ADV_TIMER_EVENT_CFG_ENA_MASK                                 0xf0000
        #define ADV_TIMER_EVENT_CFG_ENA_RESET                                0x0
        
        // ADV_TIMER clock gating configuration bitfield.  - ENA[i]=0: clock gate ADV_TIMERi. - ENA[i]=1: enable ADV_TIMERi.  (access: R/W)
        #define ADV_TIMER_CG_ENA_BIT                                         0
        #define ADV_TIMER_CG_ENA_WIDTH                                       4
        #define ADV_TIMER_CG_ENA_MASK                                        0xf
        #define ADV_TIMER_CG_ENA_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T0_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T0_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T0_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T0_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T0_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T0_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T0_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T0_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T0_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T0_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T0_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T0_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T0_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T0_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T0_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T0_CMD_ARM(val)                          ((val) << 4)
        
        #define ADV_TIMER_T0_CMD_RFU_GET(value)                    (GAP_BEXTRACTU((value),27,5))
        #define ADV_TIMER_T0_CMD_RFU_GETS(value)                   (GAP_BEXTRACT((value),27,5))
        #define ADV_TIMER_T0_CMD_RFU_SET(value,field)              (GAP_BINSERT((value),(field),27,5))
        #define ADV_TIMER_T0_CMD_RFU(val)                          ((val) << 5)
        
        #define ADV_TIMER_T0_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T0_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T0_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T0_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC(val)                     ((val) << 16)
        
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI(val)                  ((val) << 16)
        
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T0_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER(val)                  ((val) << 0)
        
        #define ADV_TIMER_T1_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T1_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T1_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T1_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T1_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T1_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T1_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T1_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T1_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T1_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T1_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T1_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T1_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T1_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T1_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T1_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T1_CMD_ARM(val)                          ((val) << 4)
        
        #define ADV_TIMER_T1_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T1_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T1_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T1_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC(val)                     ((val) << 16)
        
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI(val)                  ((val) << 16)
        
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T1_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER(val)                  ((val) << 0)
        
        #define ADV_TIMER_T2_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T2_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T2_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T2_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T2_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T2_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T2_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T2_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T2_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T2_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T2_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T2_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T2_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T2_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T2_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T2_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T2_CMD_ARM(val)                          ((val) << 4)
        
        #define ADV_TIMER_T2_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T2_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T2_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T2_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC(val)                     ((val) << 16)
        
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI(val)                  ((val) << 16)
        
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T2_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER(val)                  ((val) << 0)
        
        #define ADV_TIMER_T3_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T3_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T3_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T3_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T3_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T3_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T3_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T3_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T3_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T3_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T3_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T3_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T3_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T3_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T3_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T3_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T3_CMD_ARM(val)                          ((val) << 4)
        
        #define ADV_TIMER_T3_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T3_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T3_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T3_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC(val)                     ((val) << 16)
        
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI(val)                  ((val) << 16)
        
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE(val)                 ((val) << 16)
        
        #define ADV_TIMER_T3_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER(val)                  ((val) << 0)
        
        #define ADV_TIMER_EVENT_CFG_SEL0_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0(val)                      ((val) << 0)
        
        #define ADV_TIMER_EVENT_CFG_SEL1_GET(value)                (GAP_BEXTRACTU((value),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1_GETS(value)               (GAP_BEXTRACT((value),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1_SET(value,field)          (GAP_BINSERT((value),(field),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1(val)                      ((val) << 4)
        
        #define ADV_TIMER_EVENT_CFG_SEL2_GET(value)                (GAP_BEXTRACTU((value),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2_GETS(value)               (GAP_BEXTRACT((value),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2_SET(value,field)          (GAP_BINSERT((value),(field),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2(val)                      ((val) << 8)
        
        #define ADV_TIMER_EVENT_CFG_SEL3_GET(value)                (GAP_BEXTRACTU((value),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3_GETS(value)               (GAP_BEXTRACT((value),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3_SET(value,field)          (GAP_BINSERT((value),(field),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3(val)                      ((val) << 12)
        
        #define ADV_TIMER_EVENT_CFG_ENA_GET(value)                 (GAP_BEXTRACTU((value),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA_GETS(value)                (GAP_BEXTRACT((value),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA_SET(value,field)           (GAP_BINSERT((value),(field),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA(val)                       ((val) << 16)
        
        #define ADV_TIMER_CG_ENA_GET(value)                        (GAP_BEXTRACTU((value),4,0))
        #define ADV_TIMER_CG_ENA_GETS(value)                       (GAP_BEXTRACT((value),4,0))
        #define ADV_TIMER_CG_ENA_SET(value,field)                  (GAP_BINSERT((value),(field),4,0))
        #define ADV_TIMER_CG_ENA(val)                              ((val) << 0)

.. toggle-header::
    :header: *Register map structure*

    .. code-block:: c

        /** ADV_TIMER_Type Register Layout Typedef */
        typedef struct {
            volatile uint32_t t0_cmd;  // ADV_TIMER0 command register.
            volatile uint32_t t0_config;  // ADV_TIMER0 configuration register.
            volatile uint32_t t0_threshold;  // ADV_TIMER0 threshold configuration register.
            volatile uint32_t t0_th_channel0;  // ADV_TIMER0 channel 0 threshold configuration register.
            volatile uint32_t t0_th_channel1;  // ADV_TIMER0 channel 1 threshold configuration register.
            volatile uint32_t t0_th_channel2;  // ADV_TIMER0 channel 2 threshold configuration register.
            volatile uint32_t t0_th_channel3;  // ADV_TIMER0 channel 3 threshold configuration register.
            volatile uint32_t t0_th_channel0_lut;  // nan
            volatile uint32_t t0_th_channel1_lut;  // nan
            volatile uint32_t t0_th_channel2_lut;  // nan
            volatile uint32_t t0_th_channel3_lut;  // nan
            volatile uint32_t t0_counter;  // ADV_TIMER0 counter register.
            volatile uint32_t reserved_0[4];  // Reserved/Not used.
            volatile uint32_t t1_cmd;  // ADV_TIMER1 command register.
            volatile uint32_t t1_config;  // ADV_TIMER1 configuration register.
            volatile uint32_t t1_threshold;  // ADV_TIMER1 threshold configuration register.
            volatile uint32_t t1_th_channel0;  // ADV_TIMER1 channel 0 threshold configuration register.
            volatile uint32_t t1_th_channel1;  // ADV_TIMER1 channel 1 threshold configuration register.
            volatile uint32_t t1_th_channel2;  // ADV_TIMER1 channel 2 threshold configuration register.
            volatile uint32_t t1_th_channel3;  // ADV_TIMER1 channel 3 threshold configuration register.
            volatile uint32_t t1_th_channel0_lut;  // nan
            volatile uint32_t t1_th_channel1_lut;  // nan
            volatile uint32_t t1_th_channel2_lut;  // nan
            volatile uint32_t t1_th_channel3_lut;  // nan
            volatile uint32_t t1_counter;  // ADV_TIMER1 counter register.
            volatile uint32_t reserved_1[4];  // Reserved/Not used.
            volatile uint32_t t2_cmd;  // ADV_TIMER2 command register.
            volatile uint32_t t2_config;  // ADV_TIMER2 configuration register.
            volatile uint32_t t2_threshold;  // ADV_TIMER2 threshold configuration register.
            volatile uint32_t t2_th_channel0;  // ADV_TIMER2 channel 0 threshold configuration register.
            volatile uint32_t t2_th_channel1;  // ADV_TIMER2 channel 1 threshold configuration register.
            volatile uint32_t t2_th_channel2;  // ADV_TIMER2 channel 2 threshold configuration register.
            volatile uint32_t t2_th_channel3;  // ADV_TIMER2 channel 3 threshold configuration register.
            volatile uint32_t t2_th_channel0_lut;  // nan
            volatile uint32_t t2_th_channel1_lut;  // nan
            volatile uint32_t t2_th_channel2_lut;  // nan
            volatile uint32_t t2_th_channel3_lut;  // nan
            volatile uint32_t t2_counter;  // ADV_TIMER2 counter register.
            volatile uint32_t reserved_2[4];  // Reserved/Not used.
            volatile uint32_t t3_cmd;  // ADV_TIMER3 command register.
            volatile uint32_t t3_config;  // ADV_TIMER3 configuration register.
            volatile uint32_t t3_threshold;  // ADV_TIMER3 threshold configuration register.
            volatile uint32_t t3_th_channel0;  // ADV_TIMER3 channel 0 threshold configuration register.
            volatile uint32_t t3_th_channel1;  // ADV_TIMER3 channel 1 threshold configuration register.
            volatile uint32_t t3_th_channel2;  // ADV_TIMER3 channel 2 threshold configuration register.
            volatile uint32_t t3_th_channel3;  // ADV_TIMER3 channel 3 threshold configuration register.
            volatile uint32_t t3_th_channel0_lut;  // nan
            volatile uint32_t t3_th_channel1_lut;  // nan
            volatile uint32_t t3_th_channel2_lut;  // nan
            volatile uint32_t t3_th_channel3_lut;  // nan
            volatile uint32_t t3_counter;  // ADV_TIMER3 counter register.
            volatile uint32_t reserved_3[4];  // Reserved/Not used.
            volatile uint32_t event_cfg;  // ADV_TIMERS events configuration register.
            volatile uint32_t cg;  // ADV_TIMERS channels clock gating configuration register.
        } __attribute__((packed)) adv_timer_t;

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER0 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER0 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER0 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER0 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER0 arm command bitfield.
            unsigned int rfu             :27; // nan
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_cmd_t;
        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER0 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER0 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER0 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER0 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER0 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_config_t;
        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER0 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER0 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_threshold_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel0_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel1_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel2_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel0_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel1_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel2_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel3_lut_t;
        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER0 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_counter_t;
        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER1 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER1 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER1 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER1 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER1 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_cmd_t;
        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER1 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER1 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER1 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER1 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER1 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_config_t;
        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER1 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER1 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_threshold_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel0_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel1_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel2_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel0_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel1_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel2_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel3_lut_t;
        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER1 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_counter_t;
        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER2 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER2 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER2 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER2 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER2 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_cmd_t;
        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER2 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER2 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER2 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER2 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER2 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_config_t;
        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER2 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER2 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_threshold_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel0_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel1_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel2_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel0_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel1_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel2_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel3_lut_t;
        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER2 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_counter_t;
        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER3 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER3 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER3 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER3 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER3 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_cmd_t;
        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER3 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER3 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER3 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER3 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER3 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_config_t;
        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER3 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER3 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_threshold_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel0_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel1_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel2_t;
        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel3_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel0_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel1_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel2_lut_t;
        
        typedef union {
          struct {
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel3_lut_t;
        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER3 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_counter_t;
        
        typedef union {
          struct {
            unsigned int sel0            :4 ; // ADV_TIMER output event 0 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel1            :4 ; // ADV_TIMER output event 1 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel2            :4 ; // ADV_TIMER output event 2 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel3            :4 ; // ADV_TIMER output event 3 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int ena             :4 ; // ADV_TIMER output event enable configuration bitfield. ENA[i]=1 enables output event i generation.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_event_cfg_t;
        
        typedef union {
          struct {
            unsigned int ena             :4 ; // ADV_TIMER clock gating configuration bitfield.  - ENA[i]=0: clock gate ADV_TIMERi. - ENA[i]=1: enable ADV_TIMERi. 
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_cg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_regmap_adv_timer : public vp::regmap
        {
        public:
            vp_adv_timer_t0_cmd t0_cmd;
            vp_adv_timer_t0_config t0_config;
            vp_adv_timer_t0_threshold t0_threshold;
            vp_adv_timer_t0_th_channel0 t0_th_channel0;
            vp_adv_timer_t0_th_channel1 t0_th_channel1;
            vp_adv_timer_t0_th_channel2 t0_th_channel2;
            vp_adv_timer_t0_th_channel3 t0_th_channel3;
            vp_adv_timer_t0_th_channel0_lut t0_th_channel0_lut;
            vp_adv_timer_t0_th_channel1_lut t0_th_channel1_lut;
            vp_adv_timer_t0_th_channel2_lut t0_th_channel2_lut;
            vp_adv_timer_t0_th_channel3_lut t0_th_channel3_lut;
            vp_adv_timer_t0_counter t0_counter;
            vp_adv_timer_t1_cmd t1_cmd;
            vp_adv_timer_t1_config t1_config;
            vp_adv_timer_t1_threshold t1_threshold;
            vp_adv_timer_t1_th_channel0 t1_th_channel0;
            vp_adv_timer_t1_th_channel1 t1_th_channel1;
            vp_adv_timer_t1_th_channel2 t1_th_channel2;
            vp_adv_timer_t1_th_channel3 t1_th_channel3;
            vp_adv_timer_t1_th_channel0_lut t1_th_channel0_lut;
            vp_adv_timer_t1_th_channel1_lut t1_th_channel1_lut;
            vp_adv_timer_t1_th_channel2_lut t1_th_channel2_lut;
            vp_adv_timer_t1_th_channel3_lut t1_th_channel3_lut;
            vp_adv_timer_t1_counter t1_counter;
            vp_adv_timer_t2_cmd t2_cmd;
            vp_adv_timer_t2_config t2_config;
            vp_adv_timer_t2_threshold t2_threshold;
            vp_adv_timer_t2_th_channel0 t2_th_channel0;
            vp_adv_timer_t2_th_channel1 t2_th_channel1;
            vp_adv_timer_t2_th_channel2 t2_th_channel2;
            vp_adv_timer_t2_th_channel3 t2_th_channel3;
            vp_adv_timer_t2_th_channel0_lut t2_th_channel0_lut;
            vp_adv_timer_t2_th_channel1_lut t2_th_channel1_lut;
            vp_adv_timer_t2_th_channel2_lut t2_th_channel2_lut;
            vp_adv_timer_t2_th_channel3_lut t2_th_channel3_lut;
            vp_adv_timer_t2_counter t2_counter;
            vp_adv_timer_t3_cmd t3_cmd;
            vp_adv_timer_t3_config t3_config;
            vp_adv_timer_t3_threshold t3_threshold;
            vp_adv_timer_t3_th_channel0 t3_th_channel0;
            vp_adv_timer_t3_th_channel1 t3_th_channel1;
            vp_adv_timer_t3_th_channel2 t3_th_channel2;
            vp_adv_timer_t3_th_channel3 t3_th_channel3;
            vp_adv_timer_t3_th_channel0_lut t3_th_channel0_lut;
            vp_adv_timer_t3_th_channel1_lut t3_th_channel1_lut;
            vp_adv_timer_t3_th_channel2_lut t3_th_channel2_lut;
            vp_adv_timer_t3_th_channel3_lut t3_th_channel3_lut;
            vp_adv_timer_t3_counter t3_counter;
            vp_adv_timer_event_cfg event_cfg;
            vp_adv_timer_cg cg;
        };

|

.. _adv_timer_T0_CMD:

T0_CMD
""""""

ADV_TIMER0 command register.

.. table:: 

    +-----+---+------+-----------------------------------+
    |Bit #|R/W| Name |            Description            |
    +=====+===+======+===================================+
    |    0|W  |START |ADV_TIMER0 start command bitfield. |
    +-----+---+------+-----------------------------------+
    |    1|W  |STOP  |ADV_TIMER0 stop command bitfield.  |
    +-----+---+------+-----------------------------------+
    |    2|W  |UPDATE|ADV_TIMER0 update command bitfield.|
    +-----+---+------+-----------------------------------+
    |    3|W  |RESET |ADV_TIMER0 reset command bitfield. |
    +-----+---+------+-----------------------------------+
    |    4|W  |ARM   |ADV_TIMER0 arm command bitfield.   |
    +-----+---+------+-----------------------------------+
    |31:5 |nan|RFU   |nan                                |
    +-----+---+------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 command register.
                #define ADV_TIMER_T0_CMD_OFFSET                  0x0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_cmd_get(uint32_t base);
        static inline void adv_timer_t0_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 start command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_START_BIT                                   0
        #define ADV_TIMER_T0_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T0_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T0_CMD_START_RESET                                 0x0
        
        // ADV_TIMER0 stop command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T0_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T0_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T0_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER0 update command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T0_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T0_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T0_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER0 reset command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T0_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T0_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T0_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER0 arm command bitfield. (access: W)
        #define ADV_TIMER_T0_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T0_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T0_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T0_CMD_ARM_RESET                                   0x0
        
        // nan (access: nan)
        #define ADV_TIMER_T0_CMD_RFU_BIT                                     5
        #define ADV_TIMER_T0_CMD_RFU_WIDTH                                   27
        #define ADV_TIMER_T0_CMD_RFU_MASK                                    0xffffffe0
        #define ADV_TIMER_T0_CMD_RFU_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T0_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T0_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T0_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T0_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T0_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T0_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T0_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T0_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T0_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T0_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T0_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T0_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T0_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T0_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T0_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T0_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T0_CMD_ARM(val)                          ((val) << 4)
        
        #define ADV_TIMER_T0_CMD_RFU_GET(value)                    (GAP_BEXTRACTU((value),27,5))
        #define ADV_TIMER_T0_CMD_RFU_GETS(value)                   (GAP_BEXTRACT((value),27,5))
        #define ADV_TIMER_T0_CMD_RFU_SET(value,field)              (GAP_BINSERT((value),(field),27,5))
        #define ADV_TIMER_T0_CMD_RFU(val)                          ((val) << 5)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER0 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER0 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER0 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER0 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER0 arm command bitfield.
            unsigned int rfu             :27; // nan
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_cmd : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void update_set(uint32_t value);
            inline uint32_t update_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void arm_set(uint32_t value);
            inline uint32_t arm_get();
            inline void rfu_set(uint32_t value);
            inline uint32_t rfu_get();
        };

|

.. _adv_timer_T0_CONFIG:

T0_CONFIG
"""""""""

ADV_TIMER0 configuration register.

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                                                                                                                                                 Description                                                                                                                                                                                                                                 |
    +=====+===+=========+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |7:0  |R/W|INSEL    |ADV_TIMER0 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3                                                                                                                                                                                                                                          |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|MODE     |ADV_TIMER0 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11   |R/W|CLKSEL   |ADV_TIMER0 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|UPDOWNSEL|ADV_TIMER0 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.                                                                                                                                                                                                                                                                                             |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:16|R/W|PRESC    |ADV_TIMER0 prescaler value configuration bitfield.                                                                                                                                                                                                                                                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 configuration register.
                #define ADV_TIMER_T0_CONFIG_OFFSET               0x4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_config_get(uint32_t base);
        static inline void adv_timer_t0_config_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T0_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T0_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T0_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T0_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER0 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T0_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T0_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T0_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T0_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER0 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T0_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T0_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T0_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T0_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER0 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER0 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T0_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T0_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T0_CONFIG_PRESC_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T0_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T0_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T0_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T0_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T0_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T0_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T0_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T0_CONFIG_PRESC(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER0 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER0 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER0 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER0 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER0 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_config_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_config : public vp::reg_32
        {
        public:
            inline void insel_set(uint32_t value);
            inline uint32_t insel_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void clksel_set(uint32_t value);
            inline uint32_t clksel_get();
            inline void updownsel_set(uint32_t value);
            inline uint32_t updownsel_get();
            inline void presc_set(uint32_t value);
            inline uint32_t presc_get();
        };

|

.. _adv_timer_T0_THRESHOLD:

T0_THRESHOLD
""""""""""""

ADV_TIMER0 threshold configuration register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |15:0 |R/W|TH_LO|ADV_TIMER0 threshold low part configuration bitfield. It defines start counter value.|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |31:16|R/W|TH_HI|ADV_TIMER0 threshold high part configuration bitfield. It defines end counter value. |
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 threshold configuration register.
                #define ADV_TIMER_T0_THRESHOLD_OFFSET            0x8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_threshold_get(uint32_t base);
        static inline void adv_timer_t0_threshold_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER0 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T0_THRESHOLD_TH_HI(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER0 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER0 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_threshold_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_threshold : public vp::reg_32
        {
        public:
            inline void th_lo_set(uint32_t value);
            inline uint32_t th_lo_get();
            inline void th_hi_set(uint32_t value);
            inline uint32_t th_hi_get();
        };

|

.. _adv_timer_T0_TH_CHANNEL0:

T0_TH_CHANNEL0
""""""""""""""

ADV_TIMER0 channel 0 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER0 channel 0 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER0 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 channel 0 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL0_OFFSET          0xc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel0_get(uint32_t base);
        static inline void adv_timer_t0_th_channel0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL0_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel0 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T0_TH_CHANNEL1:

T0_TH_CHANNEL1
""""""""""""""

ADV_TIMER0 channel 1 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER0 channel 1 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER0 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 channel 1 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL1_OFFSET          0x10

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel1_get(uint32_t base);
        static inline void adv_timer_t0_th_channel1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL1_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel1 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T0_TH_CHANNEL2:

T0_TH_CHANNEL2
""""""""""""""

ADV_TIMER0 channel 2 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER0 channel 2 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER0 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 channel 2 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL2_OFFSET          0x14

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel2_get(uint32_t base);
        static inline void adv_timer_t0_th_channel2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL2_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel2 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T0_TH_CHANNEL3:

T0_TH_CHANNEL3
""""""""""""""

ADV_TIMER0 channel 3 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER0 channel 3 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER0 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 channel 3 threshold configuration register.
                #define ADV_TIMER_T0_TH_CHANNEL3_OFFSET          0x18

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel3_get(uint32_t base);
        static inline void adv_timer_t0_th_channel3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER0 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T0_TH_CHANNEL3_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER0 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER0 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_th_channel3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel3 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T0_TH_CHANNEL0_LUT:

T0_TH_CHANNEL0_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL0_LUT_OFFSET      0x1c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel0_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t0_th_channel0_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel0_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T0_TH_CHANNEL1_LUT:

T0_TH_CHANNEL1_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL1_LUT_OFFSET      0x20

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel1_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t0_th_channel1_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel1_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T0_TH_CHANNEL2_LUT:

T0_TH_CHANNEL2_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL2_LUT_OFFSET      0x24

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel2_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t0_th_channel2_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel2_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T0_TH_CHANNEL3_LUT:

T0_TH_CHANNEL3_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T0_TH_CHANNEL3_LUT_OFFSET      0x28

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t0_th_channel3_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t0_th_channel3_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_th_channel3_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T0_COUNTER:

T0_COUNTER
""""""""""

ADV_TIMER0 counter register.

.. table:: 

    +-----+---+-------+-------------------------+
    |Bit #|R/W| Name  |       Description       |
    +=====+===+=======+=========================+
    |15:0 |R  |COUNTER|ADV_TIMER0 counter value.|
    +-----+---+-------+-------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER0 counter register.
                #define ADV_TIMER_T0_COUNTER_OFFSET              0x2c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t0_counter_get(uint32_t base);
        static inline void adv_timer_t0_counter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER0 counter value. (access: R)
        #define ADV_TIMER_T0_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T0_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T0_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T0_COUNTER_COUNTER_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T0_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T0_COUNTER_COUNTER(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER0 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t0_counter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t0_counter : public vp::reg_32
        {
        public:
            inline void counter_set(uint32_t value);
            inline uint32_t counter_get();
        };

|

.. _adv_timer_T1_CMD:

T1_CMD
""""""

ADV_TIMER1 command register.

.. table:: 

    +-----+---+------+-----------------------------------+
    |Bit #|R/W| Name |            Description            |
    +=====+===+======+===================================+
    |    0|R/W|START |ADV_TIMER1 start command bitfield. |
    +-----+---+------+-----------------------------------+
    |    1|R/W|STOP  |ADV_TIMER1 stop command bitfield.  |
    +-----+---+------+-----------------------------------+
    |    2|R/W|UPDATE|ADV_TIMER1 update command bitfield.|
    +-----+---+------+-----------------------------------+
    |    3|R/W|RESET |ADV_TIMER1 reset command bitfield. |
    +-----+---+------+-----------------------------------+
    |    4|R/W|ARM   |ADV_TIMER1 arm command bitfield.   |
    +-----+---+------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 command register.
                #define ADV_TIMER_T1_CMD_OFFSET                  0x40

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_cmd_get(uint32_t base);
        static inline void adv_timer_t1_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 start command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_START_BIT                                   0
        #define ADV_TIMER_T1_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T1_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T1_CMD_START_RESET                                 0x0
        
        // ADV_TIMER1 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T1_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T1_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T1_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER1 update command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T1_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T1_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T1_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER1 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T1_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T1_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T1_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER1 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T1_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T1_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T1_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T1_CMD_ARM_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T1_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T1_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T1_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T1_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T1_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T1_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T1_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T1_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T1_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T1_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T1_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T1_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T1_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T1_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T1_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T1_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T1_CMD_ARM(val)                          ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER1 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER1 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER1 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER1 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER1 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_cmd : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void update_set(uint32_t value);
            inline uint32_t update_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void arm_set(uint32_t value);
            inline uint32_t arm_get();
        };

|

.. _adv_timer_T1_CONFIG:

T1_CONFIG
"""""""""

ADV_TIMER1 configuration register.

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                                                                                                                                                 Description                                                                                                                                                                                                                                 |
    +=====+===+=========+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |7:0  |R/W|INSEL    |ADV_TIMER1 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3                                                                                                                                                                                                                                          |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|MODE     |ADV_TIMER1 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11   |R/W|CLKSEL   |ADV_TIMER1 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|UPDOWNSEL|ADV_TIMER1 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.                                                                                                                                                                                                                                                                                             |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:16|R/W|PRESC    |ADV_TIMER1 prescaler value configuration bitfield.                                                                                                                                                                                                                                                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 configuration register.
                #define ADV_TIMER_T1_CONFIG_OFFSET               0x44

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_config_get(uint32_t base);
        static inline void adv_timer_t1_config_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T1_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T1_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T1_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T1_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER1 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T1_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T1_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T1_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T1_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER1 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T1_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T1_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T1_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T1_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER1 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER1 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T1_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T1_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T1_CONFIG_PRESC_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T1_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T1_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T1_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T1_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T1_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T1_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T1_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T1_CONFIG_PRESC(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER1 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER1 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER1 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER1 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER1 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_config_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_config : public vp::reg_32
        {
        public:
            inline void insel_set(uint32_t value);
            inline uint32_t insel_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void clksel_set(uint32_t value);
            inline uint32_t clksel_get();
            inline void updownsel_set(uint32_t value);
            inline uint32_t updownsel_get();
            inline void presc_set(uint32_t value);
            inline uint32_t presc_get();
        };

|

.. _adv_timer_T1_THRESHOLD:

T1_THRESHOLD
""""""""""""

ADV_TIMER1 threshold configuration register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |15:0 |R/W|TH_LO|ADV_TIMER1 threshold low part configuration bitfield. It defines start counter value.|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |31:16|R/W|TH_HI|ADV_TIMER1 threshold high part configuration bitfield. It defines end counter value. |
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 threshold configuration register.
                #define ADV_TIMER_T1_THRESHOLD_OFFSET            0x48

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_threshold_get(uint32_t base);
        static inline void adv_timer_t1_threshold_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER1 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T1_THRESHOLD_TH_HI(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER1 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER1 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_threshold_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_threshold : public vp::reg_32
        {
        public:
            inline void th_lo_set(uint32_t value);
            inline uint32_t th_lo_get();
            inline void th_hi_set(uint32_t value);
            inline uint32_t th_hi_get();
        };

|

.. _adv_timer_T1_TH_CHANNEL0:

T1_TH_CHANNEL0
""""""""""""""

ADV_TIMER1 channel 0 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER1 channel 0 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER1 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 channel 0 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL0_OFFSET          0x4c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel0_get(uint32_t base);
        static inline void adv_timer_t1_th_channel0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL0_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel0 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T1_TH_CHANNEL1:

T1_TH_CHANNEL1
""""""""""""""

ADV_TIMER1 channel 1 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER1 channel 1 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER1 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 channel 1 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL1_OFFSET          0x50

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel1_get(uint32_t base);
        static inline void adv_timer_t1_th_channel1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL1_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel1 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T1_TH_CHANNEL2:

T1_TH_CHANNEL2
""""""""""""""

ADV_TIMER1 channel 2 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER1 channel 2 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER1 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 channel 2 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL2_OFFSET          0x54

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel2_get(uint32_t base);
        static inline void adv_timer_t1_th_channel2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL2_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel2 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T1_TH_CHANNEL3:

T1_TH_CHANNEL3
""""""""""""""

ADV_TIMER1 channel 3 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER1 channel 3 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER1 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 channel 3 threshold configuration register.
                #define ADV_TIMER_T1_TH_CHANNEL3_OFFSET          0x58

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel3_get(uint32_t base);
        static inline void adv_timer_t1_th_channel3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER1 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T1_TH_CHANNEL3_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER1 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER1 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_th_channel3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel3 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T1_TH_CHANNEL0_LUT:

T1_TH_CHANNEL0_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL0_LUT_OFFSET      0x5c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel0_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t1_th_channel0_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel0_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T1_TH_CHANNEL1_LUT:

T1_TH_CHANNEL1_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL1_LUT_OFFSET      0x60

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel1_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t1_th_channel1_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel1_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T1_TH_CHANNEL2_LUT:

T1_TH_CHANNEL2_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL2_LUT_OFFSET      0x64

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel2_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t1_th_channel2_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel2_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T1_TH_CHANNEL3_LUT:

T1_TH_CHANNEL3_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T1_TH_CHANNEL3_LUT_OFFSET      0x68

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t1_th_channel3_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t1_th_channel3_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_th_channel3_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T1_COUNTER:

T1_COUNTER
""""""""""

ADV_TIMER1 counter register.

.. table:: 

    +-----+---+-------+-------------------------+
    |Bit #|R/W| Name  |       Description       |
    +=====+===+=======+=========================+
    |15:0 |R  |COUNTER|ADV_TIMER1 counter value.|
    +-----+---+-------+-------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER1 counter register.
                #define ADV_TIMER_T1_COUNTER_OFFSET              0x6c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t1_counter_get(uint32_t base);
        static inline void adv_timer_t1_counter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER1 counter value. (access: R)
        #define ADV_TIMER_T1_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T1_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T1_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T1_COUNTER_COUNTER_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T1_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T1_COUNTER_COUNTER(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER1 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t1_counter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t1_counter : public vp::reg_32
        {
        public:
            inline void counter_set(uint32_t value);
            inline uint32_t counter_get();
        };

|

.. _adv_timer_T2_CMD:

T2_CMD
""""""

ADV_TIMER2 command register.

.. table:: 

    +-----+---+------+-----------------------------------+
    |Bit #|R/W| Name |            Description            |
    +=====+===+======+===================================+
    |    0|R/W|START |ADV_TIMER2 start command bitfield. |
    +-----+---+------+-----------------------------------+
    |    1|R/W|STOP  |ADV_TIMER2 stop command bitfield.  |
    +-----+---+------+-----------------------------------+
    |    2|R/W|UPDATE|ADV_TIMER2 update command bitfield.|
    +-----+---+------+-----------------------------------+
    |    3|R/W|RESET |ADV_TIMER2 reset command bitfield. |
    +-----+---+------+-----------------------------------+
    |    4|R/W|ARM   |ADV_TIMER2 arm command bitfield.   |
    +-----+---+------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 command register.
                #define ADV_TIMER_T2_CMD_OFFSET                  0x80

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_cmd_get(uint32_t base);
        static inline void adv_timer_t2_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 start command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_START_BIT                                   0
        #define ADV_TIMER_T2_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T2_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T2_CMD_START_RESET                                 0x0
        
        // ADV_TIMER2 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T2_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T2_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T2_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER2 update command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T2_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T2_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T2_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER2 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T2_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T2_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T2_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER2 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T2_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T2_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T2_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T2_CMD_ARM_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T2_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T2_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T2_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T2_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T2_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T2_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T2_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T2_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T2_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T2_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T2_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T2_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T2_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T2_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T2_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T2_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T2_CMD_ARM(val)                          ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER2 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER2 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER2 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER2 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER2 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_cmd : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void update_set(uint32_t value);
            inline uint32_t update_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void arm_set(uint32_t value);
            inline uint32_t arm_get();
        };

|

.. _adv_timer_T2_CONFIG:

T2_CONFIG
"""""""""

ADV_TIMER2 configuration register.

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                                                                                                                                                 Description                                                                                                                                                                                                                                 |
    +=====+===+=========+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |7:0  |R/W|INSEL    |ADV_TIMER2 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3                                                                                                                                                                                                                                          |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|MODE     |ADV_TIMER2 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11   |R/W|CLKSEL   |ADV_TIMER2 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|UPDOWNSEL|ADV_TIMER2 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.                                                                                                                                                                                                                                                                                             |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:16|R/W|PRESC    |ADV_TIMER2 prescaler value configuration bitfield.                                                                                                                                                                                                                                                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 configuration register.
                #define ADV_TIMER_T2_CONFIG_OFFSET               0x84

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_config_get(uint32_t base);
        static inline void adv_timer_t2_config_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T2_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T2_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T2_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T2_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER2 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T2_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T2_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T2_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T2_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER2 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T2_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T2_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T2_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T2_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER2 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER2 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T2_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T2_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T2_CONFIG_PRESC_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T2_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T2_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T2_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T2_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T2_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T2_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T2_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T2_CONFIG_PRESC(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER2 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER2 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER2 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER2 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER2 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_config_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_config : public vp::reg_32
        {
        public:
            inline void insel_set(uint32_t value);
            inline uint32_t insel_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void clksel_set(uint32_t value);
            inline uint32_t clksel_get();
            inline void updownsel_set(uint32_t value);
            inline uint32_t updownsel_get();
            inline void presc_set(uint32_t value);
            inline uint32_t presc_get();
        };

|

.. _adv_timer_T2_THRESHOLD:

T2_THRESHOLD
""""""""""""

ADV_TIMER2 threshold configuration register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |15:0 |R/W|TH_LO|ADV_TIMER2 threshold low part configuration bitfield. It defines start counter value.|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |31:16|R/W|TH_HI|ADV_TIMER2 threshold high part configuration bitfield. It defines end counter value. |
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 threshold configuration register.
                #define ADV_TIMER_T2_THRESHOLD_OFFSET            0x88

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_threshold_get(uint32_t base);
        static inline void adv_timer_t2_threshold_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER2 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T2_THRESHOLD_TH_HI(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER2 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER2 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_threshold_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_threshold : public vp::reg_32
        {
        public:
            inline void th_lo_set(uint32_t value);
            inline uint32_t th_lo_get();
            inline void th_hi_set(uint32_t value);
            inline uint32_t th_hi_get();
        };

|

.. _adv_timer_T2_TH_CHANNEL0:

T2_TH_CHANNEL0
""""""""""""""

ADV_TIMER2 channel 0 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER2 channel 0 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER2 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 channel 0 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL0_OFFSET          0x8c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel0_get(uint32_t base);
        static inline void adv_timer_t2_th_channel0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL0_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel0 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T2_TH_CHANNEL1:

T2_TH_CHANNEL1
""""""""""""""

ADV_TIMER2 channel 1 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER2 channel 1 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER2 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 channel 1 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL1_OFFSET          0x90

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel1_get(uint32_t base);
        static inline void adv_timer_t2_th_channel1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL1_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel1 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T2_TH_CHANNEL2:

T2_TH_CHANNEL2
""""""""""""""

ADV_TIMER2 channel 2 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER2 channel 2 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER2 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 channel 2 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL2_OFFSET          0x94

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel2_get(uint32_t base);
        static inline void adv_timer_t2_th_channel2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL2_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel2 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T2_TH_CHANNEL3:

T2_TH_CHANNEL3
""""""""""""""

ADV_TIMER2 channel 3 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER2 channel 3 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER2 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 channel 3 threshold configuration register.
                #define ADV_TIMER_T2_TH_CHANNEL3_OFFSET          0x98

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel3_get(uint32_t base);
        static inline void adv_timer_t2_th_channel3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER2 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T2_TH_CHANNEL3_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER2 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER2 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_th_channel3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel3 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T2_TH_CHANNEL0_LUT:

T2_TH_CHANNEL0_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL0_LUT_OFFSET      0x9c

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel0_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t2_th_channel0_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel0_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T2_TH_CHANNEL1_LUT:

T2_TH_CHANNEL1_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL1_LUT_OFFSET      0xa0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel1_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t2_th_channel1_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel1_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T2_TH_CHANNEL2_LUT:

T2_TH_CHANNEL2_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL2_LUT_OFFSET      0xa4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel2_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t2_th_channel2_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel2_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T2_TH_CHANNEL3_LUT:

T2_TH_CHANNEL3_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T2_TH_CHANNEL3_LUT_OFFSET      0xa8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t2_th_channel3_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t2_th_channel3_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_th_channel3_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T2_COUNTER:

T2_COUNTER
""""""""""

ADV_TIMER2 counter register.

.. table:: 

    +-----+---+-------+-------------------------+
    |Bit #|R/W| Name  |       Description       |
    +=====+===+=======+=========================+
    |15:0 |R  |COUNTER|ADV_TIMER2 counter value.|
    +-----+---+-------+-------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER2 counter register.
                #define ADV_TIMER_T2_COUNTER_OFFSET              0xac

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t2_counter_get(uint32_t base);
        static inline void adv_timer_t2_counter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER2 counter value. (access: R)
        #define ADV_TIMER_T2_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T2_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T2_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T2_COUNTER_COUNTER_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T2_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T2_COUNTER_COUNTER(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER2 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t2_counter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t2_counter : public vp::reg_32
        {
        public:
            inline void counter_set(uint32_t value);
            inline uint32_t counter_get();
        };

|

.. _adv_timer_T3_CMD:

T3_CMD
""""""

ADV_TIMER3 command register.

.. table:: 

    +-----+---+------+-----------------------------------+
    |Bit #|R/W| Name |            Description            |
    +=====+===+======+===================================+
    |    0|R/W|START |ADV_TIMER3 start command bitfield. |
    +-----+---+------+-----------------------------------+
    |    1|R/W|STOP  |ADV_TIMER3 stop command bitfield.  |
    +-----+---+------+-----------------------------------+
    |    2|R/W|UPDATE|ADV_TIMER3 update command bitfield.|
    +-----+---+------+-----------------------------------+
    |    3|R/W|RESET |ADV_TIMER3 reset command bitfield. |
    +-----+---+------+-----------------------------------+
    |    4|R/W|ARM   |ADV_TIMER3 arm command bitfield.   |
    +-----+---+------+-----------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 command register.
                #define ADV_TIMER_T3_CMD_OFFSET                  0xc0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_cmd_get(uint32_t base);
        static inline void adv_timer_t3_cmd_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 start command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_START_BIT                                   0
        #define ADV_TIMER_T3_CMD_START_WIDTH                                 1
        #define ADV_TIMER_T3_CMD_START_MASK                                  0x1
        #define ADV_TIMER_T3_CMD_START_RESET                                 0x0
        
        // ADV_TIMER3 stop command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_STOP_BIT                                    1
        #define ADV_TIMER_T3_CMD_STOP_WIDTH                                  1
        #define ADV_TIMER_T3_CMD_STOP_MASK                                   0x2
        #define ADV_TIMER_T3_CMD_STOP_RESET                                  0x0
        
        // ADV_TIMER3 update command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_UPDATE_BIT                                  2
        #define ADV_TIMER_T3_CMD_UPDATE_WIDTH                                1
        #define ADV_TIMER_T3_CMD_UPDATE_MASK                                 0x4
        #define ADV_TIMER_T3_CMD_UPDATE_RESET                                0x0
        
        // ADV_TIMER3 reset command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_RESET_BIT                                   3
        #define ADV_TIMER_T3_CMD_RESET_WIDTH                                 1
        #define ADV_TIMER_T3_CMD_RESET_MASK                                  0x8
        #define ADV_TIMER_T3_CMD_RESET_RESET                                 0x0
        
        // ADV_TIMER3 arm command bitfield. (access: R/W)
        #define ADV_TIMER_T3_CMD_ARM_BIT                                     4
        #define ADV_TIMER_T3_CMD_ARM_WIDTH                                   1
        #define ADV_TIMER_T3_CMD_ARM_MASK                                    0x10
        #define ADV_TIMER_T3_CMD_ARM_RESET                                   0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_CMD_START_GET(value)                  (GAP_BEXTRACTU((value),1,0))
        #define ADV_TIMER_T3_CMD_START_GETS(value)                 (GAP_BEXTRACT((value),1,0))
        #define ADV_TIMER_T3_CMD_START_SET(value,field)            (GAP_BINSERT((value),(field),1,0))
        #define ADV_TIMER_T3_CMD_START(val)                        ((val) << 0)
        
        #define ADV_TIMER_T3_CMD_STOP_GET(value)                   (GAP_BEXTRACTU((value),1,1))
        #define ADV_TIMER_T3_CMD_STOP_GETS(value)                  (GAP_BEXTRACT((value),1,1))
        #define ADV_TIMER_T3_CMD_STOP_SET(value,field)             (GAP_BINSERT((value),(field),1,1))
        #define ADV_TIMER_T3_CMD_STOP(val)                         ((val) << 1)
        
        #define ADV_TIMER_T3_CMD_UPDATE_GET(value)                 (GAP_BEXTRACTU((value),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE_GETS(value)                (GAP_BEXTRACT((value),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE_SET(value,field)           (GAP_BINSERT((value),(field),1,2))
        #define ADV_TIMER_T3_CMD_UPDATE(val)                       ((val) << 2)
        
        #define ADV_TIMER_T3_CMD_RESET_GET(value)                  (GAP_BEXTRACTU((value),1,3))
        #define ADV_TIMER_T3_CMD_RESET_GETS(value)                 (GAP_BEXTRACT((value),1,3))
        #define ADV_TIMER_T3_CMD_RESET_SET(value,field)            (GAP_BINSERT((value),(field),1,3))
        #define ADV_TIMER_T3_CMD_RESET(val)                        ((val) << 3)
        
        #define ADV_TIMER_T3_CMD_ARM_GET(value)                    (GAP_BEXTRACTU((value),1,4))
        #define ADV_TIMER_T3_CMD_ARM_GETS(value)                   (GAP_BEXTRACT((value),1,4))
        #define ADV_TIMER_T3_CMD_ARM_SET(value,field)              (GAP_BINSERT((value),(field),1,4))
        #define ADV_TIMER_T3_CMD_ARM(val)                          ((val) << 4)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int start           :1 ; // ADV_TIMER3 start command bitfield.
            unsigned int stop            :1 ; // ADV_TIMER3 stop command bitfield.
            unsigned int update          :1 ; // ADV_TIMER3 update command bitfield.
            unsigned int reset           :1 ; // ADV_TIMER3 reset command bitfield.
            unsigned int arm             :1 ; // ADV_TIMER3 arm command bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_cmd_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_cmd : public vp::reg_32
        {
        public:
            inline void start_set(uint32_t value);
            inline uint32_t start_get();
            inline void stop_set(uint32_t value);
            inline uint32_t stop_get();
            inline void update_set(uint32_t value);
            inline uint32_t update_get();
            inline void reset_set(uint32_t value);
            inline uint32_t reset_get();
            inline void arm_set(uint32_t value);
            inline uint32_t arm_get();
        };

|

.. _adv_timer_T3_CONFIG:

T3_CONFIG
"""""""""

ADV_TIMER3 configuration register.

.. table:: 

    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|  Name   |                                                                                                                                                                                                                                 Description                                                                                                                                                                                                                                 |
    +=====+===+=========+=============================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |7:0  |R/W|INSEL    |ADV_TIMER3 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3                                                                                                                                                                                                                                          |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |10:8 |R/W|MODE     |ADV_TIMER3 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed|
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11   |R/W|CLKSEL   |ADV_TIMER3 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz                                                                                                                                                                                                                                                                                                                                                                                 |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |12   |R/W|UPDOWNSEL|ADV_TIMER3 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.                                                                                                                                                                                                                                                                                             |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |23:16|R/W|PRESC    |ADV_TIMER3 prescaler value configuration bitfield.                                                                                                                                                                                                                                                                                                                                                                                                                           |
    +-----+---+---------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 configuration register.
                #define ADV_TIMER_T3_CONFIG_OFFSET               0xc4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_config_get(uint32_t base);
        static inline void adv_timer_t3_config_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3 (access: R/W)
        #define ADV_TIMER_T3_CONFIG_INSEL_BIT                                0
        #define ADV_TIMER_T3_CONFIG_INSEL_WIDTH                              8
        #define ADV_TIMER_T3_CONFIG_INSEL_MASK                               0xff
        #define ADV_TIMER_T3_CONFIG_INSEL_RESET                              0x0
        
        // ADV_TIMER3 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed (access: R/W)
        #define ADV_TIMER_T3_CONFIG_MODE_BIT                                 8
        #define ADV_TIMER_T3_CONFIG_MODE_WIDTH                               3
        #define ADV_TIMER_T3_CONFIG_MODE_MASK                                0x700
        #define ADV_TIMER_T3_CONFIG_MODE_RESET                               0x0
        
        // ADV_TIMER3 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz (access: R/W)
        #define ADV_TIMER_T3_CONFIG_CLKSEL_BIT                               11
        #define ADV_TIMER_T3_CONFIG_CLKSEL_WIDTH                             1
        #define ADV_TIMER_T3_CONFIG_CLKSEL_MASK                              0x800
        #define ADV_TIMER_T3_CONFIG_CLKSEL_RESET                             0x0
        
        // ADV_TIMER3 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold. (access: R/W)
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_BIT                            12
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_WIDTH                          1
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_MASK                           0x1000
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_RESET                          0x1
        
        // ADV_TIMER3 prescaler value configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_CONFIG_PRESC_BIT                                16
        #define ADV_TIMER_T3_CONFIG_PRESC_WIDTH                              8
        #define ADV_TIMER_T3_CONFIG_PRESC_MASK                               0xff0000
        #define ADV_TIMER_T3_CONFIG_PRESC_RESET                              0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_CONFIG_INSEL_GET(value)               (GAP_BEXTRACTU((value),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL_GETS(value)              (GAP_BEXTRACT((value),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL_SET(value,field)         (GAP_BINSERT((value),(field),8,0))
        #define ADV_TIMER_T3_CONFIG_INSEL(val)                     ((val) << 0)
        
        #define ADV_TIMER_T3_CONFIG_MODE_GET(value)                (GAP_BEXTRACTU((value),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE_GETS(value)               (GAP_BEXTRACT((value),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE_SET(value,field)          (GAP_BINSERT((value),(field),3,8))
        #define ADV_TIMER_T3_CONFIG_MODE(val)                      ((val) << 8)
        
        #define ADV_TIMER_T3_CONFIG_CLKSEL_GET(value)              (GAP_BEXTRACTU((value),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL_GETS(value)             (GAP_BEXTRACT((value),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL_SET(value,field)        (GAP_BINSERT((value),(field),1,11))
        #define ADV_TIMER_T3_CONFIG_CLKSEL(val)                    ((val) << 11)
        
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_GET(value)           (GAP_BEXTRACTU((value),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_GETS(value)          (GAP_BEXTRACT((value),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL_SET(value,field)     (GAP_BINSERT((value),(field),1,12))
        #define ADV_TIMER_T3_CONFIG_UPDOWNSEL(val)                 ((val) << 12)
        
        #define ADV_TIMER_T3_CONFIG_PRESC_GET(value)               (GAP_BEXTRACTU((value),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC_GETS(value)              (GAP_BEXTRACT((value),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC_SET(value,field)         (GAP_BINSERT((value),(field),8,16))
        #define ADV_TIMER_T3_CONFIG_PRESC(val)                     ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int insel           :8 ; // ADV_TIMER3 input source configuration bitfield: - 0-31: GPIO[0] to GPIO[31] - 32-35: Channel 0 to 3 of ADV_TIMER0 - 36-39: Channel 0 to 3 of ADV_TIMER1 - 40-43: Channel 0 to 3 of ADV_TIMER2 - 44-47: Channel 0 to 3 of ADV_TIMER3
            unsigned int mode            :3 ; // ADV_TIMER3 trigger mode configuration bitfield: - 3'h0: trigger event at each clock cycle. - 3'h1: trigger event if input source is 0 - 3'h2: trigger event if input source is 1 - 3'h3: trigger event on input source rising edge - 3'h4: trigger event on input source falling edge - 3'h5: trigger event on input source falling or rising edge - 3'h6: trigger event on input source rising edge when armed - 3'h7: trigger event on input source falling edge when armed
            unsigned int clksel          :1 ; // ADV_TIMER3 clock source configuration bitfield: - 1'b0: FLL - 1'b1: reference clock at 32kHz
            unsigned int updownsel       :1 ; // ADV_TIMER3 center-aligned mode configuration bitfield: - 1'b0: The counter counts up and down alternatively. - 1'b1: The counter counts up and resets to 0 when reach threshold.
            unsigned int padding0:3 ;
            unsigned int presc           :8 ; // ADV_TIMER3 prescaler value configuration bitfield.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_config_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_config : public vp::reg_32
        {
        public:
            inline void insel_set(uint32_t value);
            inline uint32_t insel_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
            inline void clksel_set(uint32_t value);
            inline uint32_t clksel_get();
            inline void updownsel_set(uint32_t value);
            inline uint32_t updownsel_get();
            inline void presc_set(uint32_t value);
            inline uint32_t presc_get();
        };

|

.. _adv_timer_T3_THRESHOLD:

T3_THRESHOLD
""""""""""""

ADV_TIMER3 threshold configuration register.

.. table:: 

    +-----+---+-----+-------------------------------------------------------------------------------------+
    |Bit #|R/W|Name |                                     Description                                     |
    +=====+===+=====+=====================================================================================+
    |15:0 |R/W|TH_LO|ADV_TIMER3 threshold low part configuration bitfield. It defines start counter value.|
    +-----+---+-----+-------------------------------------------------------------------------------------+
    |31:16|R/W|TH_HI|ADV_TIMER3 threshold high part configuration bitfield. It defines end counter value. |
    +-----+---+-----+-------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 threshold configuration register.
                #define ADV_TIMER_T3_THRESHOLD_OFFSET            0xc8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_threshold_get(uint32_t base);
        static inline void adv_timer_t3_threshold_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 threshold low part configuration bitfield. It defines start counter value. (access: R/W)
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_BIT                             0
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_WIDTH                           16
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_MASK                            0xffff
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_RESET                           0x0
        
        // ADV_TIMER3 threshold high part configuration bitfield. It defines end counter value. (access: R/W)
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_BIT                             16
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_WIDTH                           16
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_MASK                            0xffff0000
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_THRESHOLD_TH_LO(val)                  ((val) << 0)
        
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_GET(value)            (GAP_BEXTRACTU((value),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_GETS(value)           (GAP_BEXTRACT((value),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI_SET(value,field)      (GAP_BINSERT((value),(field),16,16))
        #define ADV_TIMER_T3_THRESHOLD_TH_HI(val)                  ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th_lo           :16; // ADV_TIMER3 threshold low part configuration bitfield. It defines start counter value.
            unsigned int th_hi           :16; // ADV_TIMER3 threshold high part configuration bitfield. It defines end counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_threshold_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_threshold : public vp::reg_32
        {
        public:
            inline void th_lo_set(uint32_t value);
            inline uint32_t th_lo_get();
            inline void th_hi_set(uint32_t value);
            inline uint32_t th_hi_get();
        };

|

.. _adv_timer_T3_TH_CHANNEL0:

T3_TH_CHANNEL0
""""""""""""""

ADV_TIMER3 channel 0 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER3 channel 0 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER3 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 channel 0 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL0_OFFSET          0xcc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel0_get(uint32_t base);
        static inline void adv_timer_t3_th_channel0_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 channel 0 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL0_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL0_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 0 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 0 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel0_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel0 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T3_TH_CHANNEL1:

T3_TH_CHANNEL1
""""""""""""""

ADV_TIMER3 channel 1 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER3 channel 1 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER3 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 channel 1 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL1_OFFSET          0xd0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel1_get(uint32_t base);
        static inline void adv_timer_t3_th_channel1_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 channel 1 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL1_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL1_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 1 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 1 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel1_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel1 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T3_TH_CHANNEL2:

T3_TH_CHANNEL2
""""""""""""""

ADV_TIMER3 channel 2 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER3 channel 2 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER3 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 channel 2 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL2_OFFSET          0xd4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel2_get(uint32_t base);
        static inline void adv_timer_t3_th_channel2_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 channel 2 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL2_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL2_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 2 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 2 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel2_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel2 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T3_TH_CHANNEL3:

T3_TH_CHANNEL3
""""""""""""""

ADV_TIMER3 channel 3 threshold configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                              Description                                                                                                                                                                               |
    +=====+===+====+========================================================================================================================================================================================================================================================================================================================================================================+
    |15:0 |R/W|TH  |ADV_TIMER3 channel 3 threshold configuration bitfield.                                                                                                                                                                                                                                                                                                                  |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |18:16|R/W|MODE|ADV_TIMER3 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.|
    +-----+---+----+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 channel 3 threshold configuration register.
                #define ADV_TIMER_T3_TH_CHANNEL3_OFFSET          0xd8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel3_get(uint32_t base);
        static inline void adv_timer_t3_th_channel3_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 channel 3 threshold configuration bitfield. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_BIT                              0
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_WIDTH                            16
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_MASK                             0xffff
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_RESET                            0x0
        
        // ADV_TIMER3 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set. (access: R/W)
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_BIT                            16
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_WIDTH                          3
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_MASK                           0x70000
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_RESET                          0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_GET(value)             (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_GETS(value)            (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH_SET(value,field)       (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_TH_CHANNEL3_TH(val)                   ((val) << 0)
        
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_GET(value)           (GAP_BEXTRACTU((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_GETS(value)          (GAP_BEXTRACT((value),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE_SET(value,field)     (GAP_BINSERT((value),(field),3,16))
        #define ADV_TIMER_T3_TH_CHANNEL3_MODE(val)                 ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int th              :16; // ADV_TIMER3 channel 3 threshold configuration bitfield.
            unsigned int mode            :3 ; // ADV_TIMER3 channel 3 threshold match action on channel output signal configuration bitfield: - 3'h0: set. - 3'h1: toggle then next threshold match action is clear. - 3'h2: set then next threshold match action is clear. - 3'h3: toggle. - 3'h4: clear. - 3'h5: toggle then next threshold match action is set. - 3'h6: clear then next threshold match action is set.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_th_channel3_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel3 : public vp::reg_32
        {
        public:
            inline void th_set(uint32_t value);
            inline uint32_t th_get();
            inline void mode_set(uint32_t value);
            inline uint32_t mode_get();
        };

|

.. _adv_timer_T3_TH_CHANNEL0_LUT:

T3_TH_CHANNEL0_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL0_LUT_OFFSET      0xdc

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel0_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel0_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t3_th_channel0_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel0_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T3_TH_CHANNEL1_LUT:

T3_TH_CHANNEL1_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL1_LUT_OFFSET      0xe0

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel1_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel1_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t3_th_channel1_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel1_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T3_TH_CHANNEL2_LUT:

T3_TH_CHANNEL2_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL2_LUT_OFFSET      0xe4

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel2_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel2_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t3_th_channel2_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel2_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T3_TH_CHANNEL3_LUT:

T3_TH_CHANNEL3_LUT
""""""""""""""""""

nan

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

        
                // nan
                #define ADV_TIMER_T3_TH_CHANNEL3_LUT_OFFSET      0xe8

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_th_channel3_lut_get(uint32_t base);
        static inline void adv_timer_t3_th_channel3_lut_set(uint32_t base, uint32_t value);

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
        } __attribute__((packed)) adv_timer_t3_th_channel3_lut_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_th_channel3_lut : public vp::reg_32
        {
        public:
        };

|

.. _adv_timer_T3_COUNTER:

T3_COUNTER
""""""""""

ADV_TIMER3 counter register.

.. table:: 

    +-----+---+-------+-------------------------+
    |Bit #|R/W| Name  |       Description       |
    +=====+===+=======+=========================+
    |15:0 |R  |COUNTER|ADV_TIMER3 counter value.|
    +-----+---+-------+-------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMER3 counter register.
                #define ADV_TIMER_T3_COUNTER_OFFSET              0xec

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_t3_counter_get(uint32_t base);
        static inline void adv_timer_t3_counter_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER3 counter value. (access: R)
        #define ADV_TIMER_T3_COUNTER_COUNTER_BIT                             0
        #define ADV_TIMER_T3_COUNTER_COUNTER_WIDTH                           16
        #define ADV_TIMER_T3_COUNTER_COUNTER_MASK                            0xffff
        #define ADV_TIMER_T3_COUNTER_COUNTER_RESET                           0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_T3_COUNTER_COUNTER_GET(value)            (GAP_BEXTRACTU((value),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER_GETS(value)           (GAP_BEXTRACT((value),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER_SET(value,field)      (GAP_BINSERT((value),(field),16,0))
        #define ADV_TIMER_T3_COUNTER_COUNTER(val)                  ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int counter         :16; // ADV_TIMER3 counter value.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_t3_counter_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_t3_counter : public vp::reg_32
        {
        public:
            inline void counter_set(uint32_t value);
            inline uint32_t counter_get();
        };

|

.. _adv_timer_EVENT_CFG:

EVENT_CFG
"""""""""

ADV_TIMERS events configuration register.

.. table:: 

    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                                                                                                                                                                                                                                      Description                                                                                                                                                                                                                                                                      |
    +=====+===+====+=======================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================+
    |3:0  |R/W|SEL0|ADV_TIMER output event 0 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |7:4  |R/W|SEL1|ADV_TIMER output event 1 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |11:8 |R/W|SEL2|ADV_TIMER output event 2 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |15:12|R/W|SEL3|ADV_TIMER output event 3 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.|
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |19:16|R/W|ENA |ADV_TIMER output event enable configuration bitfield. ENA[i]=1 enables output event i generation.                                                                                                                                                                                                                                                                                                                                                                                                                                                      |
    +-----+---+----+-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMERS events configuration register.
                #define ADV_TIMER_EVENT_CFG_OFFSET               0x100

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_event_cfg_get(uint32_t base);
        static inline void adv_timer_event_cfg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER output event 0 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL0_BIT                                 0
        #define ADV_TIMER_EVENT_CFG_SEL0_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL0_MASK                                0xf
        #define ADV_TIMER_EVENT_CFG_SEL0_RESET                               0x0
        
        // ADV_TIMER output event 1 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL1_BIT                                 4
        #define ADV_TIMER_EVENT_CFG_SEL1_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL1_MASK                                0xf0
        #define ADV_TIMER_EVENT_CFG_SEL1_RESET                               0x0
        
        // ADV_TIMER output event 2 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL2_BIT                                 8
        #define ADV_TIMER_EVENT_CFG_SEL2_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL2_MASK                                0xf00
        #define ADV_TIMER_EVENT_CFG_SEL2_RESET                               0x0
        
        // ADV_TIMER output event 3 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_SEL3_BIT                                 12
        #define ADV_TIMER_EVENT_CFG_SEL3_WIDTH                               4
        #define ADV_TIMER_EVENT_CFG_SEL3_MASK                                0xf000
        #define ADV_TIMER_EVENT_CFG_SEL3_RESET                               0x0
        
        // ADV_TIMER output event enable configuration bitfield. ENA[i]=1 enables output event i generation. (access: R/W)
        #define ADV_TIMER_EVENT_CFG_ENA_BIT                                  16
        #define ADV_TIMER_EVENT_CFG_ENA_WIDTH                                4
        #define ADV_TIMER_EVENT_CFG_ENA_MASK                                 0xf0000
        #define ADV_TIMER_EVENT_CFG_ENA_RESET                                0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_EVENT_CFG_SEL0_GET(value)                (GAP_BEXTRACTU((value),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0_GETS(value)               (GAP_BEXTRACT((value),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0_SET(value,field)          (GAP_BINSERT((value),(field),4,0))
        #define ADV_TIMER_EVENT_CFG_SEL0(val)                      ((val) << 0)
        
        #define ADV_TIMER_EVENT_CFG_SEL1_GET(value)                (GAP_BEXTRACTU((value),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1_GETS(value)               (GAP_BEXTRACT((value),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1_SET(value,field)          (GAP_BINSERT((value),(field),4,4))
        #define ADV_TIMER_EVENT_CFG_SEL1(val)                      ((val) << 4)
        
        #define ADV_TIMER_EVENT_CFG_SEL2_GET(value)                (GAP_BEXTRACTU((value),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2_GETS(value)               (GAP_BEXTRACT((value),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2_SET(value,field)          (GAP_BINSERT((value),(field),4,8))
        #define ADV_TIMER_EVENT_CFG_SEL2(val)                      ((val) << 8)
        
        #define ADV_TIMER_EVENT_CFG_SEL3_GET(value)                (GAP_BEXTRACTU((value),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3_GETS(value)               (GAP_BEXTRACT((value),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3_SET(value,field)          (GAP_BINSERT((value),(field),4,12))
        #define ADV_TIMER_EVENT_CFG_SEL3(val)                      ((val) << 12)
        
        #define ADV_TIMER_EVENT_CFG_ENA_GET(value)                 (GAP_BEXTRACTU((value),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA_GETS(value)                (GAP_BEXTRACT((value),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA_SET(value,field)           (GAP_BINSERT((value),(field),4,16))
        #define ADV_TIMER_EVENT_CFG_ENA(val)                       ((val) << 16)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int sel0            :4 ; // ADV_TIMER output event 0 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel1            :4 ; // ADV_TIMER output event 1 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel2            :4 ; // ADV_TIMER output event 2 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int sel3            :4 ; // ADV_TIMER output event 3 source configuration bitfiled: - 4'h0: ADV_TIMER0 channel 0. - 4'h1: ADV_TIMER0 channel 1. - 4'h2: ADV_TIMER0 channel 2. - 4'h3: ADV_TIMER0 channel 3. - 4'h4: ADV_TIMER1 channel 0. - 4'h5: ADV_TIMER1 channel 1. - 4'h6: ADV_TIMER1 channel 2. - 4'h7: ADV_TIMER1 channel 3. - 4'h8: ADV_TIMER2 channel 0. - 4'h9: ADV_TIMER2 channel 1. - 4'hA: ADV_TIMER2 channel 2. - 4'hB: ADV_TIMER2 channel 3. - 4'hC: ADV_TIMER3 channel 0. - 4'hD: ADV_TIMER3 channel 1. - 4'hE: ADV_TIMER3 channel 2. - 4'hF: ADV_TIMER3 channel 3.
            unsigned int ena             :4 ; // ADV_TIMER output event enable configuration bitfield. ENA[i]=1 enables output event i generation.
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_event_cfg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_event_cfg : public vp::reg_32
        {
        public:
            inline void sel0_set(uint32_t value);
            inline uint32_t sel0_get();
            inline void sel1_set(uint32_t value);
            inline uint32_t sel1_get();
            inline void sel2_set(uint32_t value);
            inline uint32_t sel2_get();
            inline void sel3_set(uint32_t value);
            inline uint32_t sel3_get();
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|

.. _adv_timer_CG:

CG
""

ADV_TIMERS channels clock gating configuration register.

.. table:: 

    +-----+---+----+------------------------------------------------------------------------------------------------------------------+
    |Bit #|R/W|Name|                                                   Description                                                    |
    +=====+===+====+==================================================================================================================+
    |3:0  |R/W|ENA |ADV_TIMER clock gating configuration bitfield.  - ENA[i]=0: clock gate ADV_TIMERi. - ENA[i]=1: enable ADV_TIMERi. |
    +-----+---+----+------------------------------------------------------------------------------------------------------------------+

Generated headers
"""""""""""""""""


.. toggle-header::
    :header: *Register map C offsets*

    .. code-block:: c

        
                // ADV_TIMERS channels clock gating configuration register.
                #define ADV_TIMER_CG_OFFSET                      0x104

.. toggle-header::
    :header: *Register accessors*

    .. code-block:: c


        static inline uint32_t adv_timer_cg_get(uint32_t base);
        static inline void adv_timer_cg_set(uint32_t base, uint32_t value);

.. toggle-header::
    :header: *Register fields defines*

    .. code-block:: c

        
        // ADV_TIMER clock gating configuration bitfield.  - ENA[i]=0: clock gate ADV_TIMERi. - ENA[i]=1: enable ADV_TIMERi.  (access: R/W)
        #define ADV_TIMER_CG_ENA_BIT                                         0
        #define ADV_TIMER_CG_ENA_WIDTH                                       4
        #define ADV_TIMER_CG_ENA_MASK                                        0xf
        #define ADV_TIMER_CG_ENA_RESET                                       0x0

.. toggle-header::
    :header: *Register fields macros*

    .. code-block:: c

        
        #define ADV_TIMER_CG_ENA_GET(value)                        (GAP_BEXTRACTU((value),4,0))
        #define ADV_TIMER_CG_ENA_GETS(value)                       (GAP_BEXTRACT((value),4,0))
        #define ADV_TIMER_CG_ENA_SET(value,field)                  (GAP_BINSERT((value),(field),4,0))
        #define ADV_TIMER_CG_ENA(val)                              ((val) << 0)

.. toggle-header::
    :header: *Register fields structures*

    .. code-block:: c

        
        typedef union {
          struct {
            unsigned int ena             :4 ; // ADV_TIMER clock gating configuration bitfield.  - ENA[i]=0: clock gate ADV_TIMERi. - ENA[i]=1: enable ADV_TIMERi. 
          };
          unsigned int raw;
        } __attribute__((packed)) adv_timer_cg_t;

.. toggle-header::
    :header: *GVSOC registers*

    .. code-block:: c

        
        class vp_adv_timer_cg : public vp::reg_32
        {
        public:
            inline void ena_set(uint32_t value);
            inline uint32_t ena_get();
        };

|
