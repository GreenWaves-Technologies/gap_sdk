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
 * on 1/3/2020.
 */

#ifndef PI_FLASH_PARTITION_H
#define PI_FLASH_PARTITION_H

#include "stdint.h"

#include "pmsis/errno.h"
#include "bsp/partition.h"

#define PI_PARTITION_TABLE_FORMAT_VERSION 1
#define PI_PARTITION_MAGIC 0x01BA
#define PI_PARTITION_TABLE_HEADER_MAGIC 0x02BA
#define PI_PARTITION_HEADER_SIZE 32

#define PART_SUBTYPE_OTA_FLAG 0x10
#define PART_SUBTYPE_OTA_MASK 0x0f

#define PART_FLAG_ENCRYPTED (1<<0)
#define PART_FLAG_READONLY (1<<1)

#define PI_PARTITION_TABLE_MAX_LEN 0xC00 /* Maximum length of partition table data */
#define PI_PARTITION_TABLE_MAX_ENTRIES (PI_PARTITION_TABLE_MAX_LEN / sizeof(pi_partition_info_t)) /* Maximum length of partition table data, including terminating entry */
#define PI_PARTITION_LABEL_LENGTH 16U

typedef struct {
	uint32_t offset;
	uint32_t size;
} flash_partition_pos_t;

typedef struct {
	uint16_t magic_bytes;
	uint8_t format_version;
	uint8_t nbr_of_entries;
	uint8_t crc_flags;
	uint8_t pad[11];
	uint8_t md5[16];
} flash_partition_table_header_t;

/* Structure which describes the layout of partition table entry.
 * See docs/partition_tables.rst for more information about individual fields.
 */
typedef struct {
	uint16_t magic_bytes;
	uint8_t type; // pi_partition_type_t (Replaced by uint8_t to keep this field at 8 bits)
	uint8_t subtype; // pi_partition_subtype_t
	flash_partition_pos_t pos;
	uint8_t label[PI_PARTITION_LABEL_LENGTH];
	uint32_t flags;
} flash_partition_info_t;

typedef struct {
    pi_device_t *flash;
	flash_partition_table_header_t header;
	flash_partition_info_t *partitions;
} flash_partition_table_t;

void flash_partition_print_partition_table(const flash_partition_table_t *table);

/**
 * @brief Loads, verifies and  allocates a copy of the partition table of given flash.
 *
 * @param flash The flash device pointer where the partition table is stored.
 * @param table in case the return code is PI_OK, this double pointer will contain a newly allocated partition table.
 * @param nbr_of_entries In case the return code is PI_OK and this poiteur is not null nbr_of_entries will contain the number of partitions. The boot loader partition is not included.
 * @return PI_OK on success,
 * PI_ERR_NOT_FOUND if partition table is not found on the flash device,
 * PI_L2_NO_MEM if table can not be allocated in the L2 memory,
 * PI_ERR_INVALID_VERSION if the version of the partition table differs between flash and SDK,
 * PI_ERR_INVALID_STATE or PI_ERR_INVALID_CRC if partition table is not valid.
 */
pi_err_t flash_partition_table_load(pi_device_t *flash, const flash_partition_table_t **table, uint8_t *nbr_of_entries);

void flash_partition_table_free(const flash_partition_table_t *table);

const flash_partition_info_t *
flash_partition_find_first(const flash_partition_table_t *table, pi_partition_type_t type,
                           pi_partition_subtype_t subtype, const char *label);

/**
 * @brief Verify the partition table
 *
 * @param header Pointer to the partition header.
 * @param partition_table Pointer to at least PI_PARTITION_TABLE_MAX_ENTRIES of potential partition table data. (PI_PARTITION_TABLE_MAX_LEN bytes.)
 *
 * @return PI_OK on success, PI_ERR_INVALID_STATE if partition table is not valid and PI_ERR_INVALID_CRC if MD5 missmatch.
 */
pi_err_t flash_partition_table_verify(const flash_partition_table_t *table);


#endif //PI_FLASH_PARTITION_H
