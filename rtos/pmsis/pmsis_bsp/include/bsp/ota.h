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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 12/20/2019.
 */

#ifndef OTA_H
#define OTA_H

#include "stdint.h"

#include "pmsis.h"
#include "bsp/partition.h"
#include "bsp/flash_partition.h"
#include "bsp/ota_utility.h"

/**
 * @brief Get the next partition slot to update app.
 * @param table An instance of partition table to find next OTA partition.
 * @return The partition to use for the update. NULL if partition is not found.
 */
const pi_partition_t *ota_get_next_ota_partition(const pi_partition_table_t table);

/**
 * @brief Fetch OTA information, current OTA state and pending partition.
 *
 * Usefull to know if an update is successfull
 * or for an APP, check if it is the first boot, run the test and validate the update.
 * @param table  An instance of partition table to fetch OTA information.
 * @param ota_img_state If not NULL, return the pending IMG state.
 * @param subtype If not NULL, return the current pending subtype partition.
 * @return PI_OK on success.
 */
pi_err_t ota_get_state_info(const pi_partition_table_t table, ota_img_states_t *ota_img_state, pi_partition_subtype_t *subtype);

pi_err_t ota_get_state_info_from_flash(pi_device_t *flash, ota_img_states_t *ota_img_state, pi_partition_subtype_t *subtype);

/**
 * @brief This function is called to indicate that the running app is working well.
 *
 * @param table  An instance of partition table to write OTA information.
 * @return
 *  - PI_OK: if successful.
 */
pi_err_t ota_mark_app_valid_cancel_rollback(const pi_partition_table_t table);

/**
 * @brief This function is called to roll back to the previously workable app with reboot.
 *
 * If rollback is successful then device will reset else API will return with error code.
 * Checks applications on a flash drive that can be booted in case of rollback.
 * If the flash does not have at least one app (except the running app) then rollback is not possible.
 *
 * @param table  An instance of partition table to write OTA information.
 * @return
 *  - pi_FAIL: if not successful.
 */
pi_err_t ota_mark_app_invalid_rollback_and_reboot(const pi_partition_table_t table);

/**
 * @brief Configure OTA data for a new boot partition
 *
 * @note If this function returns PI_OK, calling pi_restart() will boot the newly configured app partition.
 *
 * @param table  An instance of partition table to write OTA information.
 * @param partition Pointer to info for partition containing app image to boot.
 *
 * @return
 *    - PI_OK: OTA data updated, next reboot will use specified partition.
 *    - PI_ERR_INVALID_ARG: partition argument was NULL or didn't point to a valid OTA partition of type "app".
 *    - PI_ERR_OTA_VALIDATE_FAILED: Partition contained invalid app image.
 *    - PI_ERR_NOT_FOUND: OTA data partition not found.
 */
pi_err_t ota_set_boot_partition(const pi_partition_table_t table, const pi_partition_t *partition);

void ota_reboot(void);



#endif //OTA_H

