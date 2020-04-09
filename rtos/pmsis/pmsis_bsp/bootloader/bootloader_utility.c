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
#include "stdbool.h"

#include "bsp/bootloader_utility.h"
#include "bsp/ota_utility.h"
#include "bsp/crc/md5.h"

pi_err_t bootloader_utility_fill_state(const flash_partition_table_t *table, bootloader_state_t *bs)
{
    pi_err_t rc;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
    const char *partition_usage;
#endif
    
    SSBL_TRC("Partition Table:");
    SSBL_TRC("## Label            SSBL usage     Type ST   Offset   Length\n");
    
    memset(bs, 0, sizeof(*bs));
    
    for (uint8_t i = 0; i < table->header.nbr_of_entries; i++)
    {
        const flash_partition_info_t *partition = table->partitions + i;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
        partition_usage = "unknown";
#endif
        
        /* valid partition table */
        switch (partition->type)
        {
            case PI_PARTITION_TYPE_APP:
                switch (partition->subtype)
                {
                    case PI_PARTITION_SUBTYPE_APP_FACTORY:
                        bs->factory = partition->pos;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                        partition_usage = "factory app";
#endif
                        break;
                    case PI_PARTITION_SUBTYPE_APP_TEST:
                        bs->test = partition->pos;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                        partition_usage = "test app";
#endif
                        break;
                    default:
                        /* OTA binary */
                        if((partition->subtype & ~PART_SUBTYPE_OTA_MASK) == PART_SUBTYPE_OTA_FLAG)
                        {
                            bs->ota[partition->subtype & PART_SUBTYPE_OTA_MASK] = partition->pos;
                            bs->app_count++;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                            partition_usage = "OTA app";
#endif
                        } else
                        {
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                            partition_usage = "Unknown app";
#endif
                        }
                        break;
                }
                break; /* PART_TYPE_APP */
            case PI_PARTITION_TYPE_DATA:
                switch (partition->subtype)
                {
                    case PI_PARTITION_SUBTYPE_DATA_OTA: /* ota data */
                        bs->ota_info = partition->pos;
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                        partition_usage = "OTA data";
#endif
                        break;
                    default:
#if PI_LOG_LOCAL_LEVEL >= PI_LOG_TRACE
                        partition_usage = "Unknown data";
#endif
                        break;
                }
                break; /* PARTITION_USAGE_DATA */
            default: /* other partition type */
                break;
        }
        
        SSBL_TRC("%2d %-16s %-16s %02x %02x   %08lx %08lx",
                 i, partition->label, partition_usage,
                 partition->type, partition->subtype,
                 partition->pos.offset, partition->pos.size);
    }
    
    
    SSBL_TRC("End of partition table\n");
    return PI_OK;
}

static bool bootloader_utility_binary_header_is_valid(const bin_desc_t *bin_desc)
{
    MD5_CTX context;
    size_t cmp;
    uint8_t res[16];
    
    SSBL_TRC("Binary header: magic code: %.4s, nbr of segments: %u, entry point: 0x%x",
             bin_desc->magic_code, bin_desc->header.nb_segments, bin_desc->header.entry);
    
    // Check magic code
    cmp = memcmp(&bin_desc->magic_code, APP_BIN_MAGIC_CODE, 4);
    if(cmp)
    {
        SSBL_ERR("App binary check failed: magic code does not match %.4s != %.4s", APP_BIN_MAGIC_CODE, bin_desc->magic_code);
        return false;
    }
    
    if(bin_desc->header.nb_segments > MAX_NB_SEGMENT)
    {
        SSBL_ERR("overflow of the number of segments allowed. Max %u, used %u", MAX_NB_SEGMENT, bin_desc->header.nb_segments);
        return false;
    }
    
    // Check CRC
    MD5_Init(&context);
    MD5_Update(&context, &bin_desc->header, sizeof(bin_header_t));
    for (uint8_t i = 0; i < bin_desc->header.nb_segments; i++)
    {
        MD5_Update(&context, bin_desc->segments + i, sizeof(bin_segment_t));
    }
    MD5_Final(res, &context);
    
    cmp = memcmp(res, bin_desc->md5, 16);
    if(cmp)
    {
        SSBL_ERR("App binary MD5 check failed");
        return false;
    }
    
    return true;
}

bool bootloader_utility_binary_is_valid(pi_device_t *flash, uint32_t flash_offset)
{
    bool is_valid;
    bin_desc_t *bin_desc;
    
    bin_desc = pi_l2_malloc(sizeof(bin_desc_t));
    if(bin_desc == NULL)
    {
        return false;
    }
    
    pi_flash_read(flash, flash_offset, bin_desc, sizeof(bin_desc_t));
    
    is_valid = bootloader_utility_binary_header_is_valid(bin_desc);
    
    pi_l2_free(bin_desc, sizeof(bin_desc_t));
    
    return is_valid;
}

static void load_segment(pi_device_t *flash, const uint32_t partition_offset, const bin_segment_t *segment)
{
    static PI_L2 uint8_t
    l2_buffer[L2_BUFFER_SIZE];

//	int encrypted = conf.info.encrypted;
    
    bool isL2Section = segment->ptr >= 0x1C000000 && segment->ptr < 0x1D000000;
    
    if(isL2Section)
    {
        SSBL_TRC("Load segment to L2 memory at 0x%lX", segment->ptr);
        pi_flash_read(flash, partition_offset + segment->start, (void *) segment->ptr, segment->size);
    } else
    {
        
        SSBL_TRC("Load segment to FC TCDM memory at 0xlX (using a L2 buffer).", segment->ptr);
        size_t remaining_size = segment->size;
        while (remaining_size > 0)
        {
            size_t iter_size = (remaining_size > L2_BUFFER_SIZE) ? L2_BUFFER_SIZE : remaining_size;
            SSBL_TRC("Remaining size 0x%lX, it size %lu", remaining_size, iter_size);
            pi_flash_read(flash, partition_offset + segment->start, l2_buffer, iter_size);
            memcpy((void *) segment->ptr, (void *) l2_buffer, iter_size);
            remaining_size -= iter_size;
        }
    }

//	aes_unencrypt(area->ptr, area->size);
}

pi_err_t bootloader_utility_boot_from_partition(pi_device_t *flash, const uint32_t partition_offset)
{
    static PI_L2
    bin_desc_t bin_desc;
    static PI_L2 uint8_t
    buff[0x94];
    bool differ_copy_of_irq_table = false;
    
    // Load binary header
    pi_flash_read(flash, partition_offset, &bin_desc, sizeof(bin_desc_t));

//    aes_init = 1;
//	aes_unencrypt((unsigned int)&flash_desc, sizeof(flash_desc_t));
    
    if(!bootloader_utility_binary_header_is_valid(&bin_desc))
    {
        SSBL_ERR("Binary is invalid, unable to boot to this app.");
        return PI_ERR_INVALID_APP;
    }
    
    for (uint8_t i = 0; i < bin_desc.header.nb_segments; i++)
    {
        bin_segment_t *seg = bin_desc.segments + i;
        SSBL_TRC("Load segment %u: flash offset 0x%lX - size 0x%lX",
                 i, seg->start, seg->size);
        
        // Skip interrupt table entries
        if(seg->ptr == 0x1C000000)
        {
            differ_copy_of_irq_table = true;
            SSBL_TRC("Differ the copy of irq table");
            pi_flash_read(flash, partition_offset + seg->start, (void *) buff, 0x94);
            seg->ptr += 0x94;
            seg->start += 0x94;
            seg->size -= 0x94;
        }
        
        load_segment(flash, partition_offset, seg);
    }
    
    
    SSBL_TRC("Close flash");
    pi_flash_close(flash);
    
    SSBL_TRC("Disable global IRQ and timer interrupt");
    disable_irq();
#ifdef __FREERTOS__
    NVIC_DisableIRQ(SYSTICK_IRQN);
#endif
    
    if(differ_copy_of_irq_table)
    {
        SSBL_TRC("Copy IRQ table whithout uDMA.");
        uint8_t *ptr = (uint8_t * )
        0x1C000000;
        for (size_t i = 0; i < 0x94; i++)
        {
            ptr[i] = buff[i];
        }
    }
    
    // Flush instruction cache
#ifdef __FREERTOS__
    SSBL_TRC("Flush icache");
    SCBC_Type *icache = SCBC;
    icache->ICACHE_FLUSH = 1;
#endif
    
    SSBL_INF("Jump to app entry point at 0x%lX", bin_desc.header.entry);
    jump_to_address(bin_desc.header.entry);
}

pi_partition_subtype_t bootloader_utility_get_boot_partition_without_ota_data(const bootloader_state_t *bs)
{
    if(bs->factory.offset)
    {
        SSBL_INF("Factory partition has been found.");
        return PI_PARTITION_SUBTYPE_APP_FACTORY;
    }
    
    if(bs->ota[0].offset)
    {
        SSBL_INF("Factory partition is not present, try to boot to OTA1 partition.");
        return PI_PARTITION_SUBTYPE_APP_OTA_0;
    }
    
    SSBL_ERR("Factory or OTA0 partition not found. Unable to find bootable partition.");
    return PI_PARTITION_SUBTYPE_UNKNOWN;
}

pi_partition_subtype_t bootloader_utility_get_boot_stable_partition(const bootloader_state_t *bs, const ota_state_t *ota_state)
{
    SSBL_TRC("Search stable partition");
    if(ota_state->stable != PI_PARTITION_SUBTYPE_UNKNOWN)
    {
        SSBL_TRC("OTA data stable partition found: %u", ota_state->stable);
        return ota_state->stable;
    }
    
    SSBL_TRC("Stable app not found into OTA data informations. Try to found bootable partition.");
    return bootloader_utility_get_boot_partition_without_ota_data(bs);
}

pi_partition_subtype_t bootloader_utility_get_boot_partition(const flash_partition_table_t *table, const bootloader_state_t *bs)
{
    pi_err_t rc;
    ota_state_t *ota_state = NULL;
    pi_partition_subtype_t subtype;
    
    SSBL_INF("Try to read OTA data from flash.");
    
    if(bs->ota_info.offset == 0)
    {
        SSBL_WNG("OTA info partition not found, try to find bootable partition.");
        return bootloader_utility_get_boot_partition_without_ota_data(bs);
    }
    
    rc = ota_utility_get_ota_state(table->flash, bs->ota_info.offset, ota_state);
    if(rc != PI_OK)
    {
        SSBL_WNG("Unable to read OTA data. Try to boot to factory or ota0 partition.");
        return bootloader_utility_get_boot_partition_without_ota_data(bs);
    }
    
    switch (ota_state->state)
    {
        case PI_OTA_IMG_VALID:
            SSBL_INF("Select stable partition type.");
            return ota_state->stable;
        
        case PI_OTA_IMG_NEW:
            SSBL_INF("An update is available, try to upgrade app.");
            ota_state->state = PI_OTA_IMG_PENDING_VERIFY;
            ota_utility_write_ota_data(table, ota_state);
            return ota_state->once;
        
        case PI_OTA_IMG_PENDING_VERIFY:
            SSBL_ERR("Last upgrade fail! App could not confirm the workable or non-workable. Invalidate the partition and boot to the stable app.");
            ota_state->state = PI_OTA_IMG_ABORTED;
            ota_utility_write_ota_data(table, ota_state);
            return bootloader_utility_get_boot_stable_partition(bs, ota_state);
        
        case PI_OTA_IMG_INVALID:
            SSBL_INF("Last upgrade has been marked invalid. Boot to the stable app.");
            return bootloader_utility_get_boot_stable_partition(bs, ota_state);
        
        case PI_OTA_IMG_ABORTED:
            SSBL_INF("Last upgrade was aborted by the bootloader. Try boot to the stable app.");
            return bootloader_utility_get_boot_stable_partition(bs, ota_state);
        
        case PI_OTA_IMG_BOOT_ONCE:
            SSBL_INF("Boot just once to a specific app.");
            ota_state->state = PI_OTA_IMG_VALID;
            subtype = ota_state->once;
            ota_state->once = PI_PARTITION_SUBTYPE_UNKNOWN;
            ota_utility_write_ota_data(table, ota_state);
            return subtype;
        
        case PI_OTA_IMG_UNDEFINED:
            SSBL_INF("OTA state is not set. Try to boot to the stable app.");
            return bootloader_utility_get_boot_stable_partition(bs, ota_state);
    }
    
    SSBL_ERR("Internal error into %s, try to find bootable app.");
    return bootloader_utility_get_boot_partition_without_ota_data(bs);
}
