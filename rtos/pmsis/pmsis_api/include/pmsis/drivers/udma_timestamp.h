/*
 * Copyright (C) 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __PMSIS_DRIVERS_UDMA_TIMESTAMP_H__
#define __PMSIS_DRIVERS_UDMA_TIMESTAMP_H__

#include "pmsis/pmsis_types.h"

/**
 * @ingroup groupDrivers
 */

/**
 * @defgroup UDMA_TIMESTAMP UDMA Timestamp
 *
 * \brief Timestamp
 *
 * This API provides support to handle timestamp feature on UDMA channels.
 */

/**
 * @addtogroup UDMA_TIMESTAMP
 * @{
 */

enum {
    PI_UDMA_TIMESTAMP_IOCTL_CLR = 1,
    PI_UDMA_TIMESTAMP_IOCTL_STOP,
    PI_UDMA_TIMESTAMP_IOCTL_SET_EVT,
    PI_UDMA_TIMESTAMP_IOCTL_SET_FIFO,
    PI_UDMA_TIMESTAMP_IOCTL_SET_INPUT_GPIO,
};

typedef enum {
    PI_TIMESTAMP_CNT_PWM = 0,
    PI_TIMESTAMP_CNT_GPIO,
    PI_TIMESTAMP_CNT_REF_CLK_QUICK,
    PI_TIMESTAMP_CNT_SOC_CLK,
}pi_timestamp_cnt_src_e;

typedef enum {
    PI_TIMESTAMP_GPIO_RISE_EDGE = 0 ,
    PI_TIMESTAMP_GPIO_FALL_EDGE ,
    PI_TIMESTAMP_GPIO_BOTH_EDGE ,
    PI_TIMESTAMP_GPIO_NONE ,
}pi_timestamp_cnt_gpio_trig_type_e;

typedef struct pi_udma_timestamp_s pi_udma_timestamp_t;

struct pi_timestamp_conf
{
    uint8_t     itf;             /*!< device ID for timestamp */ 
    uint8_t     cnt_trig_gpio;   /*!< gpio number for trigger the timestamp cnter */ 
    pi_timestamp_cnt_gpio_trig_type_e     cnt_trig_type;     /*!< how the gpio trigger the timestamp counter */ 
    pi_timestamp_cnt_src_e     cnt_src;     /*!< timestamp counter source */
    uint8_t     cnt_src_id;      /*!< GPIO/PWM ID depends on the counter source */
    uint8_t     prescaler;       /*!< Prescaler for timestamp counter */ 
};

typedef struct {
    uint8_t ts_input_id;        /*!< Timestamp input ID, max 8 input */
    uint8_t gpio;               /*!< Timestamp input GPIO ID */
    uint8_t gpio_trigger;       /*!< Timestamp input GPIO trigger */
}pi_timestamp_gpio_input_t;

pi_udma_timestamp_t * pi_udma_timestamp_alloc();
void pi_udma_timestamp_free( pi_udma_timestamp_t * timestamp );
void pi_timestamp_conf_init(struct pi_timestamp_conf *conf);
void pi_udma_timestamp_open(struct pi_timestamp_conf *conf);
void pi_udma_timestamp_close();
int32_t pi_udma_timestamp_ioctl( pi_udma_timestamp_t * timestamp, uint32_t cmd, void *arg);

/**
 * @}
 */

#endif  /* __PMSIS_DRIVERS_UDMA_TIMESTAMP_H__ */
