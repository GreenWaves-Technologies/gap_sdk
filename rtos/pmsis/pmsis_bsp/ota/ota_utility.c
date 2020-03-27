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


pi_err_t ota_utility_get_ota_state_from_partition_table(const pi_partition_table_t table, ota_state_t *ota_state)
{
    pi_err_t rc;
    const flash_partition_info_t *ota_data_partition = NULL;
    const flash_partition_table_t *flash_table = (const flash_partition_table_t *) table;
    
    ota_data_partition = flash_partition_find_first(flash_table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        return PI_ERR_NOT_FOUND;
    }
    
    return ota_utility_get_ota_state(flash_table->flash, ota_data_partition->pos.offset, ota_state);
}

void ota_utility_compute_md5(const ota_state_t *state, uint8_t *res)
{
    MD5_CTX context;
    MD5_Init(&context);
    
    MD5_Update(&context, &state->seq, sizeof(state->seq));
    MD5_Update(&context, &state->stable, sizeof(state->stable));
    MD5_Update(&context, &state->previous_stable, sizeof(state->previous_stable));
    MD5_Update(&context, &state->once, sizeof(state->once));
    MD5_Update(&context, &state->state, sizeof(state->state));
    
    MD5_Final(res, &context);
}

bool ota_utility_state_is_valid(ota_state_t *state)
{
    uint8_t res[16] = {0};
    
    if(state->seq == UINT32_MAX)
    {
        PI_LOG_INF("ota", "Check ota state: bad sequence number 0x%lx", state->seq);
        return false;
    }
    
    ota_utility_compute_md5(state, res);
    int cmp = memcmp(state->md5, res, 16);
    
    if(cmp)
    {
        PI_LOG_WNG("ota", "Check ota state: MD5 differ");
        return false;
    }
    
    return true;
}

void ota_utility_init_first_ota_state(ota_state_t *state)
{
    memset(state, 0xff, sizeof(ota_state_t));
    
    state->stable = PI_PARTITION_SUBTYPE_UNKNOWN;
    state->previous_stable = PI_PARTITION_SUBTYPE_UNKNOWN;
    state->state = PI_OTA_IMG_UNDEFINED;
}

pi_err_t
ota_utility_get_ota_state(pi_device_t *flash, const uint32_t partition_offset, ota_state_t *ota_state)
{
    pi_err_t rc = PI_ERR_NOT_FOUND;
    struct pi_flash_info flash_info = {0};
    ota_state_t *ota_states_l2;
    bool s0_is_valid, s1_is_valid;
    int8_t sector_id = -1;
    
    pi_flash_ioctl(flash, PI_FLASH_IOCTL_INFO, &flash_info);
    PI_LOG_TRC("ota", "Read OTA data, flash offset %lx, sector size %lx", partition_offset, flash_info.sector_size);
    
    ota_states_l2 = pi_l2_malloc(sizeof(ota_state_t) * 2);
    if(ota_states_l2 == NULL)
        return PI_ERR_L2_NO_MEM;
    
    pi_flash_read(flash, partition_offset, ota_states_l2, sizeof(ota_state_t));
    pi_flash_read(flash, partition_offset + flash_info.sector_size, ota_states_l2 + 1, sizeof(ota_state_t));
    
    PI_LOG_TRC("ota", "Check if OTA data 0 is valid");
    s0_is_valid = ota_utility_state_is_valid(ota_states_l2 + 0);
    PI_LOG_TRC("ota", "Check if OTA data 1 is valid");
    s1_is_valid = ota_utility_state_is_valid(ota_states_l2 + 1);
    
    // Both slots are valids
    if(s0_is_valid && s1_is_valid)
    {
        if(ota_states_l2[0].seq <= ota_states_l2[1].seq)
        {
            PI_LOG_TRC("ota", "Two OTA data are valid, using OTA data 0 which is the most recent.");
            sector_id = 0;
        } else
        {
            PI_LOG_TRC("ota", "Two OTA data are valid, using OTA data 1 which is the most recent.");
            sector_id = 1;
        }
        rc = PI_OK;
    } else
    {
        
        if(s0_is_valid)
        {
            PI_LOG_TRC("ota", "Using OTA data 0 that is the only valid.");
            sector_id = 0;
            rc = PI_OK;
        }
        
        if(s1_is_valid)
        {
            PI_LOG_TRC("ota", "Using OTA data 1 that is the only valid.");
            sector_id = 1;
            rc = PI_OK;
        }
    }
    
    if(rc == PI_OK)
    {
        SSBL_TRC("OTA data found at sector %u. Seqence number %lx, OTA state %u, stable subtype 0x%x, previous stable subtype 0x%x, once subtype 0x%x",
                 sector_id, ota_state->seq, ota_state->state, ota_state->stable, ota_state->previous_stable, ota_state->once);
        
        *ota_state = ota_states_l2[sector_id];
    }
    
    pi_l2_free(ota_states_l2, sizeof(ota_state_t) * 2);
    return rc;
}

pi_err_t ota_utility_write_ota_data(const flash_partition_table_t *table, ota_state_t *ota_state)
{
    const flash_partition_info_t *ota_data_partition;
    struct pi_flash_info flash_info = {0};
    uint8_t sector_id;
    ota_state_t *ota_state_l2;
    
    ota_data_partition = flash_partition_find_first(table, PI_PARTITION_TYPE_DATA, PI_PARTITION_SUBTYPE_DATA_OTA, NULL);
    if(ota_data_partition == NULL)
    {
        return PI_ERR_NOT_FOUND;
    }
    
    pi_flash_ioctl(table->flash, PI_FLASH_IOCTL_INFO, &flash_info);
    
    ota_state_l2 = pi_l2_malloc(sizeof(ota_state_t));
    if(ota_state_l2 == NULL)
    {
        return PI_ERR_L2_NO_MEM;
    }
    
    ota_state->seq--;
    ota_utility_compute_md5(ota_state, ota_state->md5);
    *ota_state_l2 = *ota_state;
    
    sector_id = ota_state->seq % 2;
    PI_LOG_TRC("ota", "Erase OTA data sector %u", sector_id);
    pi_flash_erase_sector(table->flash, ota_data_partition->pos.offset + sector_id * flash_info.sector_size);
    
    PI_LOG_TRC("ota", "Write ota data at sector %u: seq number %lx, OTA state %u, stable subtype 0x%x, previous stable subtype 0x%x, once subtype 0x%x",
               sector_id, ota_state->seq, ota_state->state, ota_state->stable, ota_state->previous_stable, ota_state->once);
    pi_flash_program(table->flash, ota_data_partition->pos.offset + sector_id * flash_info.sector_size, ota_state_l2, sizeof(ota_state_t));
    
    pi_l2_free(ota_state_l2, sizeof(ota_state_t));
    return PI_OK;
}

