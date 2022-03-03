#include "pmsis.h"
#include "bsp/bsp.h"
#include "bsp/flash.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/flash/spiflash.h"

#define HYPER 0
#define QSPI 1

#ifdef USE_MRAM
#define FLASH_SECTOR_SIZE (1<<13) // 8 KiB
#else
#define FLASH_SECTOR_SIZE (1<<18) // 256 KiB
#endif

#define BUFF_SIZE (FLASH_SECTOR_SIZE)

PI_L2 unsigned char *buff;

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

    buff = (unsigned char *) pmsis_l2_malloc ((uint32_t) BUFF_SIZE);
    if(buff == NULL)
    {
        printf("[Flasher]: l2 alloc failed\n");
        pmsis_exit(-1);
    }

    *(volatile uint32_t *)&debug_struct.buff_pointer = (uint32_t) buff;

    *(volatile uint32_t *)&debug_struct.gap_ready = 1;
#ifdef USE_MRAM
    printf("[Flahser]: MRAM flasher is ready\n");
#else
    printf("[Flahser]: Default flasher is ready\n");
#endif
    while((*(volatile uint32_t *)&debug_struct.flash_run) == 0)
    {
        pi_time_wait_us(1);
    }

#ifdef USE_MRAM
    struct pi_mram_conf flash_conf;
    pi_mram_conf_init(&flash_conf);
#else
    struct pi_default_flash_conf flash_conf;
    pi_default_flash_conf_init(&flash_conf);
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
