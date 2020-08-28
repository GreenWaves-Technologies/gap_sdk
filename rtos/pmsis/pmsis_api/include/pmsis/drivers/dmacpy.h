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

#ifndef __PMSIS_DRIVERS_DMACPY_H__
#define __PMSIS_DRIVERS_DMACPY_H__

#include <stdint.h>

/**
 * \ingroup groupDrivers
 *
 * \defgroup DMACPY DMACPY
 *
 * \brief Memory copy using UDMA.
 *
 * This API provides support for UDMA memory copy(DMACPY).
 * The DMACPY allows memory copy between FC L1 memory and L2 memory.
 *
 * \addtogroup DMACPY
 * \{
 */

/**
 * \struct pi_dmacpy_conf
 *
 * \brief DMA Memcpy configuration options.
 */
struct pi_dmacpy_conf
{
    uint8_t id;                 /*!< DMA memcpy device ID. */
};

/**
 * \enum pi_dmacpy_dir_e
 *
 * \brief Memcpy direction.
 */
typedef enum
{
    PI_DMACPY_FC_L1_L2 = 0,        /*!< Memcpy from FC_L1 to L2. */
    PI_DMACPY_L2_FC_L1 = 1,        /*!< Memcpy from L2 to FC_L1. */
    PI_DMACPY_L2_L2 = 2            /*!< Memcpy from L2 to L2. */
} pi_dmacpy_dir_e;

/**
 * \brief Initialize DMA memcpy config.
 *
 * This function initializes DMA memcpy configuration struct with default values.
 *
 * \param conf           Pointer to DMA Memcpy conf struct.
 */
void pi_dmacpy_conf_init(struct pi_dmacpy_conf *conf);

/**
 * \brief Open a DMA Memcpy device.
 *
 * This function opens a DMA Memcpy device.
 * Once it is opened, user can do memory copy between FC memory and L2 memory.
 *
 * \param device         Pointer to device structure.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note This function must be called before any use of the device.
 */
int pi_dmacpy_open(struct pi_device *device);

/**
 * \brief Close an opened DMA Memcpy device.
 *
 * This function closes a DMA Memcpy device.
 *
 * \param device         Pointer to device structure.
 */
void pi_dmacpy_close(struct pi_device *device);

/**
 * \brief Synchronous copy.
 *
 * A memory copy is done from FC l1 memory or L2 memory and L2 memory, depending
 * on memcpy direction.
 *
 * \param device         Pointer to device structure.
 * \param src            Pointer to source buffer.
 * \param dst            Pointer to dest buffer.
 * \param size           Size of data to copy.
 * \param dir            Direction of memcpy.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note Both src and dst buffers must be aligned on 4 bytes.
 * \note The size must be a multiple of 4 bytes.
 */
int pi_dmacpy_copy(struct pi_device *device, void *src, void *dst,
                   uint32_t size, pi_dmacpy_dir_e dir);

/**
 * \brief Asynchronous copy.
 *
 * A memory copy is done from FC l1 memory or L2 memory and L2 memory, depending
 * on memcpy direction.
 *
 * \param device         Pointer to device structure.
 * \param src            Pointer to source buffer.
 * \param dst            Pointer to dest buffer.
 * \param size           Size of data to copy.
 * \param dir            Direction of memcpy.
 * \param task           Event task used to notify end of copy.
 *
 * \retval 0             If operation is successfull.
 * \retval ERRNO         An error code otherwise.
 *
 * \note Both src and dst buffers must be aligned on 4 bytes.
 * \note The size must be a multiple of 4 bytes.
 */
int pi_dmacpy_copy_async(struct pi_device *device, void *src, void *dst,
                         uint32_t size, pi_dmacpy_dir_e dir, struct pi_task *task);

/**
 * \}
 */

#endif  /* __PMSIS_DRIVERS_DMACPY_H__ */
