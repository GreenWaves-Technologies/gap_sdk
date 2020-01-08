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

#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/flash.h"
#include "bsp/partition.h"

static pi_device_api_t partition_api = {
        .open = pi_partition_open,
        .close = pi_partition_close,
        .open_async = NULL,
        .close_async = NULL,
        .read = NULL,
        .write = NULL,
        .ioctl = NULL,
        .ioctl_async = NULL,
        .specific_api = NULL,
};

struct partition_table {
    uint32_t fs_offset;
    uint32_t reserved;
};

int pi_partition_open(struct pi_device *device)
{
    pi_partition_t *partition;
    struct partition_table *table;
    struct pi_partition_conf *conf = (struct pi_partition_conf *) device->config;

    // At this time, only two partitions are availables on the flash,
    if (conf->id >= 2) return -1;

    table = pi_l2_malloc(sizeof(*table));
    if (table == NULL) return -1;
    // Read partition 1 offset
    pi_flash_read(conf->flash, 0, table, 4);

    device->data = pi_l2_malloc(sizeof(pi_partition_t));
    if (device->data == NULL) goto table_error;
    partition = (pi_partition_t *) device->data;

    partition->flash = conf->flash;
    if (conf->id == 0)
    {
        partition->offset = 4;
        partition->size = table->fs_offset - partition->offset;
    } else
    {
        partition->offset = table->fs_offset;
        partition->size = (1 << 26) - partition->offset;
        // todo fetch flash size from flash ioctl.
    }

    device->api = &partition_api;

    pi_l2_free(table, sizeof(*table));
    return 0;

    table_error:
    pi_l2_free(table, sizeof(*table));
    return -1;
}

size_t pi_partition_get_size(pi_device_t *device)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    if (partition)
        return partition->size;
    else
        return 0;
}

uint32_t pi_partition_get_flash_offset(pi_device_t *device)
{
    pi_partition_t *partition = (pi_partition_t *) device->data;
    if (partition)
        return partition->offset;
    else
        return UINT32_MAX;
}
