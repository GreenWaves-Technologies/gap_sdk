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


#ifndef __PMSIS_DRIVERS_UDMA_TIMEOUT_H__
#define __PMSIS_DRIVERS_UDMA_TIMEOUT_H__

#include "pmsis/pmsis_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup groupDrivers
 */

/**
 * @defgroup UDMA_TIMEOUT UDMA Timeout
 *
 * \brief Timeout
 *
 * This API provides support to handle timeout feature on UDMA channels.
 */

/**
 * @addtogroup UDMA_TIMEOUT
 * @{
 */

/**
 * \enum pi_udma_timeout_mode_e
 *
 * \brief UDMA timeout mode enum.
 */
typedef enum
{
    PI_UDMA_TIMEOUT_MODE_SW_TRIGGER = 0,  /*!< Timeout triggered by SW. */
    PI_UDMA_TIMEOUT_MODE_TRANSFER   = 1,  /*!< Timeout triggered at the beginning of transfer. */
    PI_UDMA_TIMEOUT_MODE_RXTX       = 2   /*!< Timeout triggered at the beginning of transfer
                                            and cleared at each data received. */
} pi_udma_timeout_mode_e;

/**
 * \enum pi_udma_timeout_ioctl_cmd
 *
 * \brief UDMA timeout ioctl commands.
 */
enum pi_udma_timeout_ioctl_cmd
{
    /**
     * \brief Start timeout counter.
     *
     * Start UDMA timeout counter.
     *
     * \note This is to be used with `PI_UDMA_TIMEOUT_MODE_SW_TRIGGER`.
     */
    PI_UDMA_TIMEOUT_IOCTL_START = 0,
    /**
     * \brief Stop timeout counter.
     *
     * Stop UDMA timeout counter and reset it.
     */
    PI_UDMA_TIMEOUT_IOCTL_STOP  = 1
};

/**
 * \brief Allocate a UDMA timeout channel.
 *
 * This function is called to allocate a UDMA timeout channel before using it.
 * A UDMA timeout channel must be allocated before calling timeout functions.
 *
 * \param mode           UDMA timeout channel mode.
 *
 * \retval -1            If no UDMA timeout channel available.
 * \retval UTID          UDMA timeout channel ID.
 */
int32_t pi_udma_timeout_alloc(pi_udma_timeout_mode_e mode);

/**
 * \brief Free a UDMA timeout channel.
 *
 * This function frees an allocated a UDMA timeout channel.
 *
 * \param timeout_id     UDMA timeout channel to free.
 */
void pi_udma_timeout_free(int32_t timeout_id);

/**
 * \brief Ioctl commands.
 *
 * This function is used to configure UDMA timeout channels.
 *
 * \retval -1            If wrong ioctl command.
 * \retval Value         Otherwise return value depending on ioctl command.
 */
int32_t pi_udma_timeout_ioctl(int32_t timeout_id, uint32_t cmd, void *arg);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PMSIS_DRIVERS_UDMA_TIMEOUT_H__ */
