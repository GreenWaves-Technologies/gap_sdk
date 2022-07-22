/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#if TEST_DURATION <= 25
#define XIP_PAGE_SIZE    (0) //(XIP_PAGE_SIZE_4KB)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 3)
#else
#define XIP_PAGE_SIZE    (3) //(XIP_PAGE_SIZE_4KB)
#define BUFFER_ALIGN_SIZE      ( 512 << XIP_PAGE_SIZE )
#define BUFFER_SIZE ( BUFFER_ALIGN_SIZE * 3)
#endif
#define XIP_NB_DCACHE_PAGE (1)

static struct pi_hyperram_conf conf = {0};

/* -------- */
/* AES data */
/* -------- */

static uint8_t aes_key_ecb_128[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
};

static pi_aes_utils_conf_t aes_configurations[] = {
    {
        /* ECB 128 */
        .enabled = 1,
        .mode = PI_AES_MODE_ECB,
        .key_len = PI_AES_KEY_128,
        .key = (uint32_t*) aes_key_ecb_128,
        .iv = NULL,
    },
};

/* ============== */
/* Configurations */
/* ============== */

#define NB_TASK_BLOCK (10)

struct test_data{
    /* Hyper data */
    uint8_t hyper_itf;
    uint8_t* snd_buffer;
    uint8_t* rcv_buffer;
    pi_device_t ram;
    uint32_t hyper_buffer_raw;
    uint32_t hyper_buffer_aligned;
    uint32_t hyper_buffer_separated;
    /* XIP data */
    pi_device_t xip_device;
    uint8_t* xip_buffer;
    /* block for transfers */
    pi_task_t task_block_w[NB_TASK_BLOCK];
    pi_task_t task_block_r[NB_TASK_BLOCK];
};

struct test_data test_datas[] = {
    {
        .hyper_itf = 0,
        .snd_buffer = NULL,
        .rcv_buffer = NULL,
        .ram = {0},
        .hyper_buffer_raw = 0,
        .hyper_buffer_aligned = 0,
        .hyper_buffer_separated = 0,
        .xip_device = {0},
        .xip_buffer = (uint8_t*) 0x20000000,
    },
    {
        .hyper_itf = 1,
        .snd_buffer = NULL,
        .rcv_buffer = NULL,
        .ram = {0},
        .hyper_buffer_raw = 0,
        .hyper_buffer_aligned = 0,
        .hyper_buffer_separated = 0,
        .xip_device = {0},
        .xip_buffer = (uint8_t*) 0x21000000,
    },
};

/* --------- */
/* Functions */
/* --------- */

void test_hyper_ram(void)
{
    printf("Entering main controller, test duration=%i, page_size=0x%x\n",TEST_DURATION,XIP_PAGE_SIZE);


    uint32_t errors = 0;

    const size_t data_size = sizeof(test_datas)/sizeof(test_datas[0]);

    /* Opening 2 hyperrams */
    for (size_t k = 0; k < data_size; k++)
    {
       #ifdef GAP9_WLCSP
        // Skip hyper1 for WLCSP since those pads don't exist
            if (k == 1)
                continue;
        #endif
        printf("Opening hyper #%ld\n", k);
        test_datas[k].snd_buffer = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
        if (test_datas[k].snd_buffer == NULL)
        {
            printf("buff alloc failed !\n");
            pmsis_exit(-1);
        }

        test_datas[k].rcv_buffer = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE*2);
        if (test_datas[k].rcv_buffer == NULL)
        {
            printf("rcv_buff alloc failed !\n");
            pmsis_exit(-2);
        }

        for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
        {
            test_datas[k].snd_buffer[i] = i & 0xFF;
            test_datas[k].rcv_buffer[i] = 0;
        }

        {
            /* Init & open ram. */
            pi_hyperram_conf_init(&conf);
            (&conf)->xip_en = 1; // we want to use xip on this device
            (&conf)->baudrate = 50*1000*1000;
            (&conf)->hyper_itf = test_datas[k].hyper_itf;

            /* add aes to hyperram */
            memcpy(&(conf.ram.aes_conf), &aes_configurations[0], sizeof(pi_aes_utils_conf_t));

            printf("baudrate at hyper init %x\n",(&conf)->baudrate);
            pi_open_from_conf(&(test_datas[k].ram), &conf);
            if (pi_ram_open(&(test_datas[k].ram)))
            {
                printf("Error ram open !\n");
                pmsis_exit(-3);
            }

            if (pi_ram_alloc(&(test_datas[k].ram), &(test_datas[k].hyper_buffer_raw), (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE))
            {
                printf("Ram malloc failed !\n");
                pmsis_exit(-4);
            }
            else
            {
                printf("Ram allocated : %lx %ld.\n", test_datas[k].hyper_buffer_raw, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE);
            }
            test_datas[k].hyper_buffer_aligned = (test_datas[k].hyper_buffer_raw + BUFFER_ALIGN_SIZE) & (~(BUFFER_ALIGN_SIZE-1));
            printf("hyper_buff aligned = %x\n", test_datas[k].hyper_buffer_aligned);

            printf("HyperBus write/read\n");
            /* Write a buffer in ram, then read back from ram. */
            pi_ram_write(&(test_datas[k].ram), test_datas[k].hyper_buffer_aligned, test_datas[k].snd_buffer, (uint32_t) BUFFER_SIZE);
            printf("Write sync done.\n");
            pi_ram_read(&(test_datas[k].ram), test_datas[k].hyper_buffer_aligned, test_datas[k].rcv_buffer, (uint32_t) BUFFER_SIZE);
            printf("Read sync done.\n");

            /* Verification for normal read / write */
            printf("Verifying data\n");
            for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
            {
                if (test_datas[k].snd_buffer[i] != test_datas[k].rcv_buffer[i])
                {
                    errors++;
                    printf("%2x-%2x ",
                            test_datas[k].snd_buffer[i],
                            test_datas[k].rcv_buffer[i]);
                    pmsis_exit(-1);
                }
            }
            printf("Verification done\n");

            /* alloc memory different from the one used by XIP */
            if (pi_ram_alloc(&(test_datas[k].ram), &(test_datas[k].hyper_buffer_separated), (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE))
            {
                printf("Ram (separated) malloc failed !\n");
                pmsis_exit(-4);
            }
            else
            {
                printf("Ram (separated) allocated : %lx %ld.\n", test_datas[k].hyper_buffer_separated, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE);
            }
        }
    }

    /* Setting up XIP on both hyperrams */
    uint32_t page_counter = 0;
    for (size_t k = 0; k < data_size; k++)
    {
        printf("Setting up XIP #%d\n", k);
        uint32_t page_mask = 0;
        pi_device_t *xip_device = &(test_datas[k].xip_device);
        for( int i = 0; i<XIP_NB_DCACHE_PAGE; i++)
        {
            pi_xip_dcache_page_alloc(page_counter, XIP_PAGE_SIZE);
            printf("dcache page addr= 0x%x\n",hal_xip_dcache_page_l2_addr_get(page_counter));
            page_counter++;
        }
        pi_xip_free_page_mask_get(&page_mask,XIP_NB_DCACHE_PAGE);
        printf("page mask = 0x%x\n",page_mask);

        pi_xip_conf_t xip_conf = {0};
        xip_conf.ro = 0;
        xip_conf.per_id = (k == 0) ? XIP_DEVICE_HYPER0 : XIP_DEVICE_HYPER1;
        xip_conf.page_size = XIP_PAGE_SIZE;
        xip_conf.page_mask = page_mask;
        printf("xip page alloc done\n");

        uint32_t mount_size = BUFFER_SIZE / (512 << XIP_PAGE_SIZE);

        xip_device->config = &xip_conf;
        int ret = pi_xip_mount(xip_device, test_datas[k].xip_buffer, test_datas[k].hyper_buffer_aligned, mount_size, 0);
        if(0 != ret)
        {
            printf("xip open failed: %d\n",ret);
            pmsis_exit(-1);
        }
        printf("xip open success\n");
    }

    printf("Verification loop (long): Hyper(0|1)+AES_DC+(SW|XIP)\n");

    /* launch a bunch of hyper transfers */
    for (uint32_t j = 0; j < NB_TASK_BLOCK; j++)
    {
        for (size_t k = 0; k < data_size; k++)
        {
           #ifdef GAP9_WLCSP
        // Skip hyper1 for WLCSP since those pads don't exist
            if (k == 1)
                continue;
        #endif
            pi_task_block(&(test_datas[k].task_block_w[j]));
            pi_task_block(&(test_datas[k].task_block_r[j]));
            pi_ram_write_async(&(test_datas[k].ram), test_datas[k].hyper_buffer_separated, test_datas[k].snd_buffer + (BUFFER_SIZE/2), (uint32_t) BUFFER_SIZE/2, &(test_datas[k].task_block_w[j]));
            pi_ram_read_async (&(test_datas[k].ram), test_datas[k].hyper_buffer_separated, test_datas[k].rcv_buffer + (BUFFER_SIZE/2), (uint32_t) BUFFER_SIZE/2, &(test_datas[k].task_block_r[j]));
        }
    }
    
    /* write in both XIP areas */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        for (size_t k = 0; k < data_size; k++)
        {
         #ifdef GAP9_WLCSP
        // Skip hyper1 for WLCSP since those pads don't exist
            if (k == 1)
                continue;
        #endif
            test_datas[k].xip_buffer[i] = ((i+k) & 0xFF);
        }
    }
    /* read in both XIP areas */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        for (size_t k = 0; k < data_size; k++)
        {
           #ifdef GAP9_WLCSP
        // Skip hyper1 for WLCSP since those pads don't exist
            if (k == 1)
                continue;
        #endif
            if (test_datas[k].xip_buffer[i] != ((i+k) & 0xFF))
            {
                printf("ERROR while fetching XIP data, expected %d, got %d\n", ((i+k) & 0xFF), test_datas[k].xip_buffer[i]);
                pmsis_exit(-1);
            }
        }
    }
    printf("read xip is done\n");

    for (uint32_t j = 0; j < NB_TASK_BLOCK; j++)
    {
        for (size_t k = 0; k < data_size; k++)
        {
             #ifdef GAP9_WLCSP
              // Skip hyper1 for WLCSP since those pads don't exist
              if (k == 1)
                continue;
        #endif
            pi_task_wait_on(&(test_datas[k].task_block_w[j]));
            pi_task_wait_on(&(test_datas[k].task_block_r[j]));
        }
    }
     printf("read xip is done2\n");
    /* frees */
    for (size_t k = 0; k < data_size; k++)
    {
      #ifdef GAP9_WLCSP
              // Skip hyper1 for WLCSP since those pads don't exist
              if (k == 1)
                continue;
        #endif
        pi_xip_unmount(&(test_datas[k].xip_device));
        pmsis_l2_malloc_free(test_datas[k].snd_buffer, (uint32_t) BUFFER_SIZE);
        pmsis_l2_malloc_free(test_datas[k].rcv_buffer, (uint32_t) BUFFER_SIZE);
        pi_ram_free(&(test_datas[k].ram), test_datas[k].hyper_buffer_raw, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE);
        pi_ram_free(&(test_datas[k].ram), test_datas[k].hyper_buffer_separated, (uint32_t) BUFFER_SIZE*2+BUFFER_ALIGN_SIZE);

        pi_ram_close(&(test_datas[k].ram));
    }

    printf("\nHyperram transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS XIP Hyperram AES Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram);
}
