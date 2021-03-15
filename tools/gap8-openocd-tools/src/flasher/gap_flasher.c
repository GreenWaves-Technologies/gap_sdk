#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash/spiflash.h"

#define HYPER 0
#define QSPI 1

#if (FLASH_TYPE == HYPER)
#define PRINT_FLASH_TYPE    "Hyperflash"
#define FLASH_SECTOR_SIZE (1<<18) // 256 KiB
#elif (FLASH_TYPE == QSPI)
#define PRINT_FLASH_TYPE    "QSPIflash"
#define FLASH_SECTOR_SIZE (1<<16) // 64 KiB
#endif

#define BUFF_SIZE (FLASH_SECTOR_SIZE)

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
    struct pi_device flash;

    *(volatile uint32_t *)&debug_struct.buff_pointer = (uint32_t)buff;

    *(volatile uint32_t *)&debug_struct.gap_ready = 1;
    printf("[Flahser]: %s flasher is ready\n", PRINT_FLASH_TYPE);
    while((*(volatile uint32_t *)&debug_struct.flash_run) == 0)
    {
        pi_time_wait_us(1);
    }

#if (FLASH_TYPE == HYPER)
    struct pi_hyperflash_conf flash_conf;
    pi_hyperflash_conf_init(&flash_conf);
#elif (FLASH_TYPE == QSPI)
    struct pi_spiflash_conf flash_conf;
    pi_spiflash_conf_init(&flash_conf);
#else
    printf("No this type !\n");
#endif

    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        printf("pi_flash_open failed\n");
        pmsis_exit(-3);
    }

    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        pmsis_exit(-3);
    }

    while(debug_struct.flash_run)
    {
        while((*(volatile uint32_t *)&debug_struct.host_ready) == 0)
        {
            pi_time_wait_us(1);
        }

        *(volatile uint32_t *)&debug_struct.gap_ready = 1;
        // wait for ACK
        while((*(volatile uint32_t *)&debug_struct.gap_ready) == 1)
        {
            pi_time_wait_us(1);
        }

        // Erase and write the sector pointed by current_flash_addr
        pi_flash_erase_sector(&flash, debug_struct.flash_addr);
        pi_flash_program(&flash, debug_struct.flash_addr, (void*)buff,
                debug_struct.flash_size);
    }

    printf("[Flahser]: flasher is done\n");
    *(volatile uint32_t *)&debug_struct.flash_run = 1;
    return 0;
    // -------------------------------------------------------- //
}

static void test_kickoff(void *arg)
{
    int ret = test_entry();
    // we don't want to exit, openocd will manage life cycles for us
    while(1);
    pmsis_exit(0);
}

int main(void)
{
    return pmsis_kickoff((void *)test_kickoff);
}
