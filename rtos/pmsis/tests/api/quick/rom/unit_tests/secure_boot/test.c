/* 
 * Copyright (C) 2021 ETH Zurich, University of Bologna and GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Authors: Antoine Faravelon (antoine.faravelon@greenwaves-technologies.com)
 */

#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/flash/hyperflash.h"

#include "efuse.h"

#include "archi/rom/rom.h"

typedef struct
{
    uint32_t start;
    uint32_t ptr;
    uint32_t size;
    uint32_t crc;
} flash_mem_area_t;

typedef struct {
    uint32_t nextDesc;
    uint32_t nbAreas;
    uint32_t entry;
    uint32_t bootaddr;
    uint32_t xip_dev;
    uint32_t xip_vaddr;
    uint32_t xip_page_size;
    uint32_t xip_flash_base;
    uint32_t xip_flash_nb_pages;
    uint32_t xip_l2_base;
    uint32_t xip_l2_nb_pages;
    // Secure boot
    uint32_t kc_length;
    uint32_t key_length;
    uint8_t ac[1024]; // qk activation code
    uint8_t kc[128]; // key code
    uint8_t kc_write[128]; // key code
} flash_header_t;

typedef struct
{
    flash_header_t header;
    flash_mem_area_t memArea[16];
} flash_desc_t;

static unsigned int crc32b(unsigned char *message, unsigned int Len)
{
    int i, j;
    unsigned int byte, crc, mask;

    crc = 0xFFFFFFFF;
    for (i = 0; i < Len; i++)
    {
        byte = message[i];
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}

void pe_entry(void *arg)
{
    printf("Hello from (%d, %d)\n", pi_cluster_id(), pi_core_id());
}

void cluster_entry(void *arg)
{
    pi_cl_team_fork(0, pe_entry, 0);
    printf("Hello from (%d, %d)\n", pi_cluster_id(), pi_core_id());
}

static volatile pi_aes_utils_conf_t aes_configuration =
    {
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_256,
        .key = NULL,
        .iv = NULL,
    };

static int test_entry()
{
    efuse_start_read();
    uint32_t efuse_val = efuse_read(ROM_SECURE_BOOT_OFFSET);
    efuse_sleep();
    if(efuse_val)
    {
        printf("Reboot done\n");
#ifdef KEY_LOCK
        if(efuse_val != 0xdead5ea5)
        {
            printf("NOK\n");
            return -1;
        }
#endif
#ifdef QK_LOCK
        /* De-Clock Gating */
        apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 0;

        /* De-Reset IP */
        apb_soc_ctrl->clk_en_quiddikey |= 0x1 << 1;
        if(hal_quiddikey_unwrap_allowed(0) || hal_quiddikey_enroll_allowed(0))
        {
            printf("NOK\n");
            return -1;
        }
#endif
        printf("TOK\n");
        return 0;
    }
    printf("test entry\n");
    // qk init
    int error = 0;
    pi_device_t qk_device = {0};
    pi_quiddikey_conf_t qk_conf = {0};
    qk_conf.id = 0;
    qk_conf.enroll = 1;
    qk_conf.ac = pi_malloc(GAP_QUIDDIKEY_AC_BYTES);
    qk_device.config = &qk_conf;
    int ret = pi_quiddikey_open(&qk_device);
    printf("ret = %x\n",ret);
    pi_quiddikey_close(&qk_device);
    qk_conf.enroll = 0;
    ret = pi_quiddikey_open(&qk_device);
    printf("ret = %x\n",ret);
    // wrap a key, need to wrap once for each channels
    // first generate random key
    uint32_t key_length = 256;
    uint32_t *key = pi_malloc(key_length >> 3);
    pi_quiddikey_context_t context = {key_length, 0, 0, 0, 0};
    ret = pi_quiddikey_generate_random(&qk_device, (uint32_t *)key, &context);

    

#ifdef USE_QK
    printf("Using qk\n");
    efuse_start_program();
    // enable qk boot
    uint32_t efuse_prog = 0x0;
    efuse_prog = 0x2; // use qk
#ifdef USE_CRC
    efuse_prog |= 0x10; // use crc
#endif

#ifdef QK_LOCK
    efuse_prog |= (1<<7); // lock qk features at boot
#endif

    efuse_program(ROM_SECURE_BOOT_OFFSET, efuse_prog);

    efuse_program(ROM_FEATURE_DISABLE_QK_OFFSET,  0x3 | (1<<31));

    efuse_sleep();
    // Allocate space for key code, use key code length table
    char *key_code_tx = pi_malloc(GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));
    char *key_code_rx = pi_malloc(GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));
    // dor nok, so ok, use id=0 (aes dual core tx)
    pi_quiddikey_set_key_context(&context, uQUIDDIKEY_Started, key_length, 0, 1, 0);
    ret = pi_quiddikey_wrap(&qk_device, (uint32_t *)key, (uint32_t *)key_code_tx, &context);
    // dor nok, so ok, use id=1 (aes dual core rx)
    pi_quiddikey_set_key_context(&context, uQUIDDIKEY_Started, key_length, 0, 1, 1);
    ret = pi_quiddikey_wrap(&qk_device, (uint32_t *)key, (uint32_t *)key_code_rx, &context);
#else // no qk
    // secure boot: 
    // aes_user(22) (bit2) key size (bit 3) bit4 ==> crc
    efuse_start_program();
#ifdef USE_CRC
    efuse_program(ROM_SECURE_BOOT_OFFSET,0x1C);
#else
#ifdef SIGN_ONLY // crc but signing only, no encryption of payload
    // bit6: sign only, + CRC and AES_KEY
    efuse_program(ROM_SECURE_BOOT_OFFSET,0x5C);
#else
#ifdef KEY_LOCK
    //bit5: lock the efuses containing the key
    efuse_program(ROM_SECURE_BOOT_OFFSET,0x2C);
#else
    efuse_program(ROM_SECURE_BOOT_OFFSET,0xC);
#endif
#endif
#endif
    // aes_keyX (23-30)
    for(int i=0; i<8;i++)
    {
        efuse_program(ROM_AES_KEY0_OFFSET+i,key[i]);
    }
    efuse_sleep();
#endif

    struct pi_device flash, boot_flash;
    struct pi_hyperflash_conf conf, boot_conf;
    /* Init & open ram. */

    pi_hyperflash_conf_init(&conf);
#ifdef USE_XIP
    conf.hyper_itf=1;    
#endif
#ifdef USE_QK
    aes_configuration.key_code0 = key_code_tx;
    aes_configuration.key_code1 = key_code_rx;
    aes_configuration.qk_dev = &qk_device;
    aes_configuration.qk_en = 1;
#else
    aes_configuration.enabled = 1;
    aes_configuration.mode = PI_AES_MODE_ECB;
    aes_configuration.key_len = PI_AES_KEY_256;
    aes_configuration.key = (uint32_t*) key;
    aes_configuration.iv = NULL;
    aes_configuration.qk_en = 0;
#endif
#ifdef USE_ITF1
    pi_hyperflash_conf_init(&boot_conf);
    boot_conf.hyper_itf=1; // write to itf 1
    memcpy(&(boot_conf.flash.aes_conf), &aes_configuration,
            sizeof(pi_aes_utils_conf_t));
    pi_open_from_conf(&boot_flash,&boot_conf);
#else
    memcpy(&(conf.flash.aes_conf), &aes_configuration,
            sizeof(pi_aes_utils_conf_t));
#endif
    pi_open_from_conf(&flash,&conf);
    pi_flash_open(&flash);
    pi_flash_ioctl(&flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)0);
    
    flash_desc_t *flash_desc = pi_malloc(sizeof(flash_desc_t));
    pi_flash_read(&flash, 0x0, flash_desc, (uint32_t)sizeof(flash_desc_t));
#ifdef USE_QK
    printf("kc_length=0x%x\n", flash_desc->header.kc_length);
    memcpy(flash_desc->header.ac,qk_conf.ac,GAP_QUIDDIKEY_AC_BYTES);
    memcpy(flash_desc->header.kc,key_code_rx,GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length));
    flash_desc->header.kc_length = GAP_QUIDDIKEY_KEY_CODE_LENGTH_BYTE(key_length); // use byte size
    flash_desc->header.key_length = (key_length == 256) ? 1:0;
#endif

    // load l2
    int nb_areas = flash_desc->header.nbAreas;
    printf("nb_areas=0x%x\n",nb_areas);

    uint32_t total_size = 0;
    for(int i=0; i<nb_areas; i++)
    {
        flash_mem_area_t *area = &flash_desc->memArea[i];
        unsigned int area_addr = area->ptr;
        unsigned int size = area->size;
        int isL2Section = area_addr >= 0x1C000000 && area_addr < 0x1D000000;
        int isXIPSection = area_addr >= 0x20000000 && area_addr < 0x30000000;
        if (isL2Section || isXIPSection)
        {
            total_size += area->size;
        }
    }
    printf("total_size = 0x%x\n",total_size);
    // add 16 bytes for clean padding
    uint8_t *clear_bin = pi_malloc(total_size+16);
    for(int i=0; i<16; i++)
    {
        clear_bin[i+total_size] = 0x0;
    }
    printf("clear_bin = 0x%x\n",clear_bin);
    uint32_t max_addr = 0;
    uint32_t cur_size = 0;
    for(int i=0; i<nb_areas; i++)
    {
        flash_mem_area_t *area = &flash_desc->memArea[i];
        unsigned int flash_addr = area->start;
        unsigned int area_addr = area->ptr;
        unsigned int size = area->size;
        int isL2Section = area_addr >= 0x1C000000 && area_addr < 0x1D000000;
        int isXIPSection = area_addr >= 0x20000000 && area_addr < 0x30000000;
        if ((isL2Section || isXIPSection) && (size>0))
        {
            pi_flash_read(&flash, flash_addr, clear_bin+cur_size, (uint32_t)size);
#ifdef USE_CRC
            area->crc = crc32b((unsigned char*)(clear_bin+cur_size),size);
            printf("crc=0x%x\n",area->crc);
#endif
            cur_size += size;
            printf("start: 0x%x, size: 0x%x\n",area->start, size);
            if((area->start + size) > max_addr)
            {
                max_addr = area->start + size;
            }
        }
    }
    printf("dumped binary done, max_addr=%x\n",max_addr);

    printf("erasing 0x%x bytes\n",sizeof(flash_desc_t));
#ifdef USE_ITF1
    if(pi_flash_open(&boot_flash))
    {
        printf("TKO\n");
    }
    pi_flash_ioctl(&boot_flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)0);
    pi_flash_erase(&boot_flash, 0x0, (uint32_t)sizeof(flash_desc_t));
    printf("erase done\n");
    // program main header first
    pi_flash_program(&boot_flash, 0x0, flash_desc, (uint32_t)sizeof(flash_header_t));
    printf("program 1 done\n");
    pi_flash_ioctl(&boot_flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)1);
    pi_flash_program(&boot_flash, sizeof(flash_header_t),
            ((void*)flash_desc)+sizeof(flash_header_t), 
            (uint32_t)sizeof(flash_mem_area_t)*nb_areas);
#else
#ifdef USE_XIP
    printf("USE_XIP\n");
    flash_desc->header.xip_dev=1;
    printf("flash_base=%x\n",flash_desc->header.xip_flash_base);
#endif
    pi_flash_erase(&flash, 0x0, (uint32_t)sizeof(flash_desc_t));
    printf("erase done\n");
    // program main header first
    pi_flash_program(&flash, 0x0, flash_desc, (uint32_t)sizeof(flash_header_t));
    pi_flash_ioctl(&flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)1);
    // program section headers (encrypted) second
    printf("size=%x, addr =%x\n",sizeof(flash_desc_t) - sizeof(flash_header_t), ((void*)flash_desc)+sizeof(flash_header_t));
    pi_flash_program(&flash, sizeof(flash_header_t),
            ((void*)flash_desc)+sizeof(flash_header_t), 
            (uint32_t)sizeof(flash_mem_area_t)*nb_areas);
#endif
#ifdef SIGN_ONLY
    // if sign only, do not encrypt payload, only metadata
#ifdef USE_ITF1
    pi_flash_ioctl(&boot_flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)0);
#else
    pi_flash_ioctl(&flash,PI_FLASH_IOCTL_AES_ENABLE,(void*)0);
#endif
#endif
    printf("program done\n");
    cur_size = 0;
    for(int i=0; i<nb_areas; i++)
    {
        flash_mem_area_t *area = &flash_desc->memArea[i];
        unsigned int flash_addr = area->start;
        unsigned int area_addr = area->ptr;
        unsigned int size = area->size;
        int isL2Section = area_addr >= 0x1C000000 && area_addr < 0x1D000000;
        int isXIPSection = area_addr >= 0x20000000 && area_addr < 0x30000000;
        if ((isL2Section || isXIPSection )&& (size>0))
        {
            printf("writting size=0x%x at flash addr=0x%x from addr=0x%x\n",
                    size,flash_addr,clear_bin+cur_size);
            uint32_t _size = size - (size % 16) + ((size % 16)?16:0);
            printf("size padded: 0x%x\n",_size);
#ifdef USE_ITF1
            pi_flash_program(&boot_flash, flash_addr, clear_bin+cur_size, (uint32_t)_size);
#else
            pi_flash_program(&flash, flash_addr, clear_bin+cur_size, (uint32_t)_size);
#endif

            cur_size += size;
        }
    }

#if defined(USE_ITF1) || defined(USE_XIP)
    efuse_start_program();
    // Use itf1 to boot
    efuse_program(ROM_INFO_3_OFFSET, (1 << 2) | (1<<3));
    efuse_sleep();
#endif

    // reboot with watchdog
    printf("going to use sw reset to reboot\n");
    apb_soc_ctrl->sw_rst = 1;
    printf("TNOK Reboot not done\n");

    return 0;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    pmsis_exit(ret);
}

int main()
{
    return pmsis_kickoff((void *)test_kickoff);
}
