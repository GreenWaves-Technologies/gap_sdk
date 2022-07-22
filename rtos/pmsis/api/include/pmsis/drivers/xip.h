/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#pragma once

#include "pmsis/pmsis_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @ingroup groupDrivers
*/



/**
 * @defgroup XIP XIP
 *
 *
 */

/**
 * @addtogroup XIP
 * @{
 */

/**@{*/


/**
 * \cond IMPLEM
 */
#define XIP_DEVICE_HYPER0 XIP_DEVICE0
#define XIP_DEVICE_HYPER1 XIP_DEVICE1
#define XIP_DEVICE_MRAM0 XIP_DEVICE2
/**
 * \endcond
 */

#define PI_INLINE_XIP0 static inline

#ifndef PI_INLINE_XIP0
#define PI_INLINE_XIP0
#endif



/** \struct pi_xip_conf
 * \brief XIP configuration structure.
 *
 * This structure is used to pass the desired XIP configuration to the
 * runtime when mounting a memory area.
 */
typedef struct pi_xip_conf_s {
    uint8_t ro; /*!< whether this mount is read only. Write accesses will
                  trigger an exception.*/
    uint8_t per_id; /*!< Peripheral / mem controller on which to mount.*/
    uint8_t tlb_en;
    uint16_t page_mask; /*!< XIP cache pages to be used for this mount.*/
    xip_page_size_e page_size; /*!< Desired page size, starts at 512, computes
                                 as 512 << page_size. Maximum is 64KB */
    func_t data_exception_handler; /*!< exception handler for fc data accesses.*/
    func_t instr_exception_handler; /*!< exception handler for fc insn accesses.*/
    func_t cl_irq_handler; /*!< exception handler for cluster accesses.*/
} pi_xip_conf_t;

/** \struct pi_xip_data_err
 * \brief Structure which will be used to commnicate with user handler
 * for exceptions
 *
 * This structure can be used by user exception handler to handle faults
 */
typedef struct pi_xip_data_err_s {
    uint32_t error_addr; /*!< Address at which the error happened.*/
    uint32_t violation; /*!< Type of error. */
} pi_xip_data_err_t;

/** \brief Mount an area in XIP virtual memory
 *
 * Allows to mount an area from external memory, starting at external address
 * ext_addr, of size mount_size*page_size into the XIP virtual memory starting
 * at virt_addr.
 *
 * \param mount A pi device containing the mount configuration.
 * \param virt_addr An address contained in XIP area where mount will happen.
 * \param ext_addr An external address from which the mount will happen.
 * \param mount_size A mount size, expressed in number of pages.
 * \return 0 if operation is successful, -1 otherwise.
 */
PI_INLINE_XIP0 int pi_xip_mount(pi_device_t *mount, void *virt_addr, uint32_t ext_addr,
        uint32_t mount_size, uint8_t cacheable);

/** \brief Umnount a previously mounted XIP area
 *
 * Warning: should only be used from L2 code.
 *
 * \param mount A pi device containing the mount configuration.
 */
PI_INLINE_XIP0 void pi_xip_unmount(pi_device_t *mount);

/** \brief Acquire previously allocated XIP cache pages
 *
 * \param mount A pointer to a page mask to be filled.
 * \param nb_pages Desired number of pages.
 * \return 0 on success, nb of free pages left if not enough are available.
 */
PI_INLINE_XIP0 int pi_xip_free_page_mask_get(uint32_t *page_mask,
        uint8_t nb_pages);

/** \brief Allocate chunks of L2 memory for XIP pages
 *
 * \param page_id hw page id to be allocated.
 * \param page_size Size of the page to be allocated.
 *
 * \return 0 on success, -1 otherwise.
 */
PI_INLINE_XIP0 int  pi_xip_dcache_page_alloc(uint32_t page_id,
        xip_page_size_e page_size);

/** \brief Release L2 page used by XIP
 *
 * \param page_id hw page id to be freed.
 * \param page_size Size of the page to be freed.
 *
 * \return 0 on success, -1 otherwise.
 */
PI_INLINE_XIP0 int  pi_xip_dcache_page_free(uint32_t page_id,
        xip_page_size_e page_size);


//!@}

/**
 * @} end of XIP
 */

PI_INLINE_XIP0 void pi_xip_cluster_is_on_set(uint8_t cluster_is_on);

PI_INLINE_XIP0 int pi_xip_tlb_init(pi_device_t *device);

PI_INLINE_XIP0 void pi_xip_tlb_lru_update(void *virt_addr,
        uint32_t ext_addr, uint32_t per_id, uint8_t lock, uint8_t cacheable);

PI_INLINE_XIP0 void pi_xip_tlb_page_set(uint8_t page_id, void *virt_addr,
        uint32_t ext_addr, uint32_t per_id, uint8_t lock, uint8_t cacheable);

#ifdef __cplusplus
}
#endif
