/*
 * Copyright (C) 2018 GreenWaves Technologies
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
 * Authors: Mathieu Barbe, GreenWaves Technologies (mathieu.barbe@greenwaves-technologies.com)
 */

#include "stdio.h"
#include "stdint.h"

#include "bsp/crc/md5.h"
#include "pmsis.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash_partition.h"

#include "bsp/bootloader_utility.h"

/*
 * Global variables
 */


#include "bsp/ota_utility.h"
#include "bsp/ota.h"

pi_partition_subtype_t ota_get_next_free_ota_slot(const ota_state_t *state, const bootloader_state_t *bs)
{
    switch (state->stable)
    {
        case PI_PARTITION_SUBTYPE_APP_FACTORY:
        case PI_PARTITION_SUBTYPE_UNKNOWN:
            if(bs->ota[0].offset)
                return PI_PARTITION_SUBTYPE_APP_OTA_0;
            else
            {
                PI_LOG_ERR("ota", "The next partition to update app is ota0 but it is not present into partition table.");
                return PI_PARTITION_SUBTYPE_UNKNOWN;
            }
        
        case PI_PARTITION_SUBTYPE_APP_OTA_0:
            if(bs->ota[1].offset)
                return PI_PARTITION_SUBTYPE_APP_OTA_1;
            else
            {
                PI_LOG_ERR("ota", "The next partition to update app is ota1 but it is not present into partition table.");
                return PI_PARTITION_SUBTYPE_UNKNOWN;
            }
        
        case PI_PARTITION_SUBTYPE_APP_OTA_1:
            if(bs->ota[0].offset)
                return PI_PARTITION_SUBTYPE_APP_OTA_0;
            else
            {
                PI_LOG_ERR("ota", "The next partition to update app is ota0 but it is not present into partition table.");
                return PI_PARTITION_SUBTYPE_UNKNOWN;
            }
        
        default:
            PI_LOG_ERR("ota", "Unable to determine the next ota slot. Current type %d", state->stable);
            return PI_PARTITION_SUBTYPE_UNKNOWN;
    }
}

const pi_partition_t *ota_get_next_ota_partition(const pi_partition_table_t table)
{
    pi_err_t rc;
    pi_partition_subtype_t next_partition_type;
    const pi_partition_t *next_partition = NULL;
    ota_state_t ota_state;
    bootloader_state_t bs;
    const flash_partition_table_t *flash_table = (const flash_partition_table_t *) table;
    
    rc = ota_utility_get_ota_state_from_partition_table(table, &ota_state);
    if(rc != PI_OK)
    {
        ota_utility_init_first_ota_state(&ota_state);;
    }
    
    rc = bootloader_utility_fill_state(flash_table, &bs);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to extract bootloader state.");
        return NULL;
    }
    
    next_partition_type = ota_get_next_free_ota_slot(&ota_state, &bs);
    
    next_partition = pi_partition_find_first(table, PI_PARTITION_TYPE_APP, next_partition_type, NULL);
    if(next_partition == NULL)
    {
        PI_LOG_ERR("ota", "Unable to load partition type %d", next_partition_type);
        return NULL;
    }
    
    return next_partition;
}

pi_err_t ota_set_once_boot_partition(const pi_partition_table_t table, const pi_partition_t *partition)
{
    pi_err_t rc = PI_OK;
    const pi_partition_t *ota_data_partition = NULL;
    ota_state_t ota_state;
    
    if(partition == NULL)
    {
        PI_LOG_ERR("ota", "Partition arg is not valid");
        return PI_ERR_INVALID_ARG;
    }
    
    if(partition->type != PI_PARTITION_TYPE_APP)
    {
        PI_LOG_ERR("ota", "A bootable partition must be of app type.");
        return PI_ERR_INVALID_ARG;
    }
    
    // Check binary integrity
    if(!bootloader_utility_binary_is_valid(((const flash_partition_table_t *) table)->flash, partition->offset))
    {
        PI_LOG_ERR("ota", "Binary under partition is not bootable. OTA state is unchanged.");
        return PI_ERR_INVALID_APP;
    }
    
    PI_LOG_TRC("ota", "Open OTA data partition.");
    ota_data_partition = pi_partition_find_first(table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        PI_LOG_ERR("ota", "Unable to load ota data partition. OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    rc = ota_utility_get_ota_state(ota_data_partition->flash, ota_data_partition->offset, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_WNG("ota", "Unable to load ota data information, use a new one.");
        ota_utility_init_first_ota_state(&ota_state);;
    }
    
    ota_state.state = PI_OTA_IMG_BOOT_ONCE;
    ota_state.once = partition->subtype;
    rc = ota_utility_write_ota_data(table, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to write new OTA data.");
        goto close_partition_and_return_rc;
    }
    
    close_partition_and_return_rc:
    pi_partition_close(ota_data_partition);
    return rc;
}

pi_err_t ota_set_boot_partition(const pi_partition_table_t table, const pi_partition_t *partition)
{
    pi_err_t rc = PI_OK;
    const pi_partition_t *ota_data_partition = NULL;
    ota_state_t ota_state;
    
    if(partition == NULL)
    {
        PI_LOG_ERR("ota", "Partition arg is not valid");
        return PI_ERR_INVALID_ARG;
    }
    
    
    // Check binary integrity
    if(!bootloader_utility_binary_is_valid(((const flash_partition_table_t *) table)->flash, partition->offset))
    {
        PI_LOG_ERR("ota", "Binary under partition is not bootable. OTA state is unchanged.");
        return PI_ERR_INVALID_APP;
    }
    
    
    PI_LOG_TRC("ota", "Open OTA data partition.");
    ota_data_partition = pi_partition_find_first(table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        PI_LOG_ERR("ota", "Unable to load ota data partition. OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    if(partition->subtype == PI_PARTITION_SUBTYPE_APP_FACTORY)
    {
        PI_LOG_TRC("ota", "Erase OTA data information. During the next reboot, Factory partition will be used to boot.");
        rc = pi_partition_format(partition);
        if(rc != PI_OK)
        {
            PI_LOG_ERR("ota", "Erase OTA data partition error.");
            rc = PI_ERR_INVALID_STATE;
        } else
        {
            rc = PI_OK;
        }
        
        goto close_partition_and_return_rc;
    }
    
    if(!(partition->subtype == PI_PARTITION_SUBTYPE_APP_OTA_0 ||
         partition->subtype == PI_PARTITION_SUBTYPE_APP_OTA_1))
    {
        PI_LOG_ERR("ota", "The new partition for an update must be an OTA partition or a Factory partition.");
        rc = PI_ERR_INVALID_ARG;
        goto close_partition_and_return_rc;
    }
    
    rc = ota_utility_get_ota_state(ota_data_partition->flash, ota_data_partition->offset, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_WNG("ota", "Unable to load ota data information, use a new one.");
        ota_utility_init_first_ota_state(&ota_state);;
    }
    
    ota_state.state = PI_OTA_IMG_NEW;
    ota_state.once = partition->subtype;
    rc = ota_utility_write_ota_data(table, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to write new OTA data.");
        goto close_partition_and_return_rc;
    }
    
    close_partition_and_return_rc:
    pi_partition_close(ota_data_partition);
    return rc;
}

pi_err_t ota_get_state_info(const pi_partition_table_t table, ota_img_states_t *ota_img_state, pi_partition_subtype_t *subtype)
{
    pi_err_t rc;
    ota_state_t ota_state_info;
    
    rc = ota_utility_get_ota_state_from_partition_table(table, &ota_state_info);
    if(rc != PI_OK)
    {
        return rc;
    }
    
    if (ota_img_state)
        *ota_img_state = ota_state_info.state;
    
    if (subtype)
        *subtype = ota_state_info.once;
    
    return PI_OK;
}

pi_err_t ota_get_state_info_from_flash(pi_device_t *flash, ota_img_states_t *ota_img_state, pi_partition_subtype_t *subtype)
{
    const pi_partition_table_t table;
    pi_err_t rc;
    
    rc = pi_partition_table_load(flash, &table);
    if (rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to load partition table.");
        return rc;
    }
    
    rc = ota_get_state_info(table, ota_img_state, subtype);
    
    pi_partition_table_free(table);
    return rc;
}

pi_err_t ota_mark_app_valid_cancel_rollback(const pi_partition_table_t table)
{
    pi_err_t rc;
    const flash_partition_table_t *flash_table = (const flash_partition_table_t *) table;
    const flash_partition_info_t *ota_data_partition;
    ota_state_t ota_state;
    
    ota_data_partition = flash_partition_find_first(flash_table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        PI_LOG_ERR("ota", "Unable to load ota data partition, OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    rc = ota_utility_get_ota_state(flash_table->flash, ota_data_partition->pos.offset, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to load ota data information, OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    if(ota_state.state != PI_OTA_IMG_PENDING_VERIFY)
    {
        PI_LOG_ERR("ota", "OTA state must be in PI_OTA_IMG_PENDING_VERIFY state. Current OTA state is %u", ota_state.state);
        return PI_ERR_INVALID_STATE;
    }
    
    ota_state.state = PI_OTA_IMG_VALID;
    ota_state.previous_stable = ota_state.stable;
    ota_state.stable = ota_state.once;
    
    rc = ota_utility_write_ota_data(flash_table, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to write new OTA data. OTA state is unchanged.");
        return PI_ERR_INVALID_STATE;
    }
    
    PI_LOG_INF("ota", "Current app is marked valid state.");
    
    return PI_OK;
}

pi_err_t ota_mark_app_invalid_rollback_and_reboot(const pi_partition_table_t table)
{
    pi_err_t rc;
    const flash_partition_table_t *flash_table = (const flash_partition_table_t *) table;
    const flash_partition_info_t *ota_data_partition;
    ota_state_t ota_state;
    
    ota_data_partition = flash_partition_find_first(flash_table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        PI_LOG_ERR("ota", "Unable to load ota data partition, OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    rc = ota_utility_get_ota_state(flash_table->flash, ota_data_partition->pos.offset, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to load ota data information, OTA state is unchanged.");
        return PI_ERR_NOT_FOUND;
    }
    
    if(ota_state.state != PI_OTA_IMG_PENDING_VERIFY)
    {
        PI_LOG_ERR("ota", "OTA state must be in PI_OTA_IMG_PENDING_VERIFY state. Current OTA state is %u", ota_state.state);
        return PI_ERR_INVALID_STATE;
    }
    
    ota_state.state = PI_OTA_IMG_INVALID;
    
    rc = ota_utility_write_ota_data(flash_table, &ota_state);
    if(rc != PI_OK)
    {
        PI_LOG_ERR("ota", "Unable to write new OTA data. OTA state is unchanged.");
        return PI_ERR_INVALID_STATE;
    }
    
    PI_LOG_INF("ota", "Current app is marked as invalid.");
    
    ota_reboot();
    __builtin_unreachable();
}

void ota_reboot(void)
{
    printf("Please reboot the device.\n");
    while (1)
    {
        __asm__("wfi"::);
    }
    
    __builtin_unreachable();
}



