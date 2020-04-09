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

#include "string.h"
#include "stdio.h"

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/flash.h"
#include "bsp/partition.h"
#include "bsp/flash_partition.h"

size_t pi_partition_get_size(const pi_partition_t *partition)
{
    if(partition)
        return partition->size;
    else
        return 0;
}

uint32_t pi_partition_get_flash_offset(const pi_partition_t *partition)
{
    if(partition)
        return partition->offset;
    else
        return UINT32_MAX;
}

void pi_partition_table_free(pi_partition_table_t table)
{
    flash_partition_table_free((flash_partition_table_t *) table);
}

pi_err_t pi_partition_table_load(pi_device_t *flash, const pi_partition_table_t *table)
{
    pi_err_t rc;
    
    rc = flash_partition_table_load(flash, (const flash_partition_table_t **) table, NULL);
//    if (rc == PI_OK)
//    {
//    flash_partition_print_partition_table((flash_partition_table_t *) *table);
//    }
    
    return rc;
}

const pi_partition_t *
pi_partition_find_first(const pi_partition_table_t table, const pi_partition_type_t type,
                        const pi_partition_subtype_t subtype,
                        const char *label)
{
    pi_partition_t *partition;
    const flash_partition_info_t *info;
    
    info = flash_partition_find_first((const flash_partition_table_t *) table, type, subtype, label);
    if(info == NULL)
        return NULL;
    
    partition = pi_l2_malloc(sizeof(pi_partition_t));
    partition->type = info->type;
    partition->subtype = info->subtype;
    partition->size = info->pos.size;
    partition->offset = info->pos.offset;
    partition->flash = ((const flash_partition_table_t *)table)->flash;
    memcpy(partition->label, info->label, 16);
    partition->label[16] = 0;
    partition->encrypted = false;
    partition->read_only = false;
    
    return (const pi_partition_t *) partition;
}

