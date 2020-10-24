#include "pmsis.h"
#include "bsp/bsp.h"

#define L2_SECTOR_SIZE (1<<18) // 256 KiB
#define BUFF_SIZE (L2_SECTOR_SIZE)

#define HYPERFLASH 0
#define SPI_FLASH 1

// Adapt for hyperflash on gap8
PI_L2 unsigned char buff[BUFF_SIZE];

extern void *__rt_debug_struct_ptr;

typedef struct 
{
    uint32_t host_ready;
    uint32_t gap_ready;
    uint32_t buff_pointer;
    // size is ignored here
    uint32_t buff_size;
    uint32_t flash_run;
    uint32_t flash_addr;
    uint32_t flash_size;
    uint32_t flash_type;
} bridge_t;

bridge_t debug_struct = {0};

static int test_entry(void)
{
    __rt_debug_struct_ptr = &debug_struct;

    *(volatile uint32_t *)&debug_struct.buff_pointer = (uint32_t)buff;

    *(volatile uint32_t *)&debug_struct.gap_ready = 1;
    printf("flasher is ready\n");
    while((*(volatile uint32_t *)&debug_struct.flash_run) == 0)
    {
        pi_time_wait_us(1);
    }
    struct pi_device flash;
    struct pi_hyperflash_conf flash_conf;

    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        printf("pi_flash_open failed\n");
        pmsis_exit(-3);
    }

    while(debug_struct.flash_run)
    { 
        while((*(volatile uint32_t *)&debug_struct.host_ready) == 0)
        {
            pi_time_wait_us(1);
        }

        // Erase and write the sector pointed by current_flash_addr
        pi_flash_read(&flash, debug_struct.flash_addr, (void*)buff,
                debug_struct.flash_size);

        *(volatile uint32_t *)&debug_struct.gap_ready = 1;
        // wait for ACK
        while((*(volatile uint32_t *)&debug_struct.gap_ready) == 1)
        {
            pi_time_wait_us(1);
        }

        // -------------------------------------------------------- //


    }

    printf("flasher is done\n");
    *(volatile uint32_t *)&debug_struct.flash_run = 1;
    return 0;
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    // we don't want to exit, openocd will manage life cycles for us
    while(1)
    {
        pi_time_wait_us(1);
    }
}

PMSIS_APP_MAIN
{
    return pmsis_kickoff((void *)test_kickoff);
}
