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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 1/6/2020.
 */

#include "string.h"
#include "stdio.h"

#include "bsp/bsp.h"
#include "bsp/flash_partition.h"
#include "bsp/crc/md5.h"

void flash_partition_print_partition_table(const flash_partition_table_t *table)
{
    flash_partition_info_t *partitions;

    if(table == NULL)
    {
        printf("No partition table\n");
        return;
    }

    if(table->header.nbr_of_entries == 0)
    {
        printf("No partition\n");
        return;
    }

    partitions = table->partitions;
    printf("## Label \t   Type Sub Type Offset Length\n");

    for (uint8_t i = 0;
         i < table->header.nbr_of_entries;
         i++)
    {
        printf("%2d %-16s 0x%02x 0x%02x 0x%-8lx 0x%-8lx\n",
               i, partitions[i].label,
               partitions[i].type, partitions[i].subtype,
               partitions[i].pos.offset, partitions[i].pos.size);
    }
}

#if 0
static void print_partition_header(flash_partition_table_header_t *header)
{
    printf("Partition table header:\n"
           "\t.magic_bytes 0x%04x\n"
           "\t.format_version %u\n"
           "\t.nbr_of_entries %u\n"
           "\t.crc_flag %u\n",
           header->magic_bytes,
           header->format_version,
           header->nbr_of_entries,
           header->crc_flags);
    printf("\t.md5 ");
    for (int i = 0; i < 16; i++)
    {
        printf("%02x ", header->md5[i]);
    }
    printf("\n");
}
#endif

pi_err_t flash_partition_table_verify(const flash_partition_table_t *table)
{
    const flash_partition_info_t *part;
    const flash_partition_table_header_t *header = &table->header;
    const flash_partition_info_t *partition_table = table->partitions;
    MD5_CTX context;
    uint8_t digest[16];

    // Check magic number for each partition
    for (uint8_t num_parts = 0; num_parts < header->nbr_of_entries; num_parts++)
    {
        part = partition_table + num_parts;
        if (part->magic_bytes != PI_PARTITION_MAGIC)
        {
            return PI_ERR_INVALID_STATE;
        }
    }

    if (header->crc_flags)
    {
        MD5_Init(&context);
        MD5_Update(&context, (unsigned char *) partition_table,
                   header->nbr_of_entries * sizeof(flash_partition_info_t));
        MD5_Final(digest, &context);

        if (strncmp((const char *) header->md5, (const char *) digest, sizeof(digest)))
        {
            return PI_ERR_INVALID_CRC;
        }
    }

    return PI_OK;
}

pi_err_t flash_partition_table_load(pi_device_t *flash, const flash_partition_table_t **partition_table,
                                    uint8_t *nbr_of_entries)
{
    pi_err_t rc = PI_OK;
    uint32_t *table_offset_l2 = NULL;
    uint32_t _table_offset;
    flash_partition_table_t *table = NULL;
    flash_partition_info_t *partitions = NULL;

    if(partition_table == NULL)
    {
        PARTITION_TRACE_ERR("Table argument is NULL");
        return PI_ERR_INVALID_ARG;
    }

// Alloc table containing header
    table = pi_l2_malloc(sizeof(*table));
    if(table == NULL)
    {
        PARTITION_TRACE_ERR("Unable to allocate partition table in L2.");
        rc = PI_ERR_L2_NO_MEM;
        goto mount_error;
    }

    table_offset_l2 = pi_l2_malloc(sizeof(*table_offset_l2));
    if(table_offset_l2 == NULL)
    {
        PARTITION_TRACE_ERR("Unable to allocate table offset variable in L2.");
        rc = PI_ERR_L2_NO_MEM;
        goto mount_error;
    }

    pi_flash_read(flash, 0, table_offset_l2, 4);
    if(*table_offset_l2 == 0)
    {
        PARTITION_TRACE_ERR("Partition table offset not found in flash.");
        rc = PI_ERR_NOT_FOUND;
        goto mount_error;
    }

    PARTITION_TRACE_TRC("Partition table offset 0x%lx", *table_offset_l2);
    _table_offset = *table_offset_l2;
    pi_l2_free(table_offset_l2, sizeof(*table_offset_l2));
    table_offset_l2 = NULL;


    // Load table header
    pi_flash_read(flash, _table_offset, &table->header, sizeof(flash_partition_table_header_t));

//print_partition_header(&table->header);

    if(table->header.magic_bytes != PI_PARTITION_TABLE_HEADER_MAGIC)
    {
        PARTITION_TRACE_ERR("Partition table header magic number error\n");
        rc = PI_ERR_NOT_FOUND;
        goto mount_error;
    }

    if(table->header.format_version != PI_PARTITION_TABLE_FORMAT_VERSION)
    {
        PARTITION_TRACE_ERR("Partition table format version missmatch: flash version %u != BSP version %u\n",
                   table->header.format_version,
                   PI_PARTITION_TABLE_FORMAT_VERSION);
        rc = PI_ERR_INVALID_VERSION;
        goto mount_error;
    }

    // Alloc partition entries
    table->partitions = pi_l2_malloc(sizeof(flash_partition_info_t) * table->header.nbr_of_entries);
    if(table->partitions == NULL)
    {
        PARTITION_TRACE_ERR("Unable to allocate partition table entries.");
        rc = PI_ERR_L2_NO_MEM;
        goto mount_error;
    }

    pi_flash_read(flash, _table_offset + PI_PARTITION_HEADER_SIZE, table->partitions,
                  sizeof(flash_partition_info_t) * table->header.nbr_of_entries);


    if(table->header.crc_flags)
    {
        rc = flash_partition_table_verify(table);
        if(rc != PI_OK)
        {
            PARTITION_TRACE_ERR("Partitions table verification failed.\n");
            pi_l2_free(table->partitions, sizeof(flash_partition_info_t) * table->header.nbr_of_entries);
            goto mount_error;
        }
    }

    table->flash = flash;

    *partition_table = table;
    if(nbr_of_entries)
        *nbr_of_entries = table->header.nbr_of_entries;

    return PI_OK;

    mount_error:
    if(table_offset_l2)
        pi_l2_free(table_offset_l2, sizeof(*table_offset_l2));
    if(table)
        pi_l2_free(table, sizeof(*table));
    return rc;
}

void flash_partition_table_free(const flash_partition_table_t *table)
{
    pi_l2_free((void *) table->partitions, sizeof(flash_partition_info_t) * table->header.nbr_of_entries);
    pi_l2_free((void *) table, sizeof(flash_partition_table_t));
}


const flash_partition_info_t *flash_partition_find_first(const flash_partition_table_t *table, pi_partition_type_t type,
                                                         pi_partition_subtype_t subtype, const char *label)
{
    const flash_partition_info_t *part = NULL;

    for (uint8_t i = 0; i < table->header.nbr_of_entries; i++)
    {
        part = table->partitions + i;
        if(part->type != type || part->subtype != subtype)
            continue;
        if(label == NULL)
            return part;
        if(strncmp(label, (char *) &part->label, PI_PARTITION_LABEL_LENGTH) == 0)
            return part;
    }
    return NULL;
}
