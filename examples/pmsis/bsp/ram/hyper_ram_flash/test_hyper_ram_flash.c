/* PMSIS includes. */
#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/ram.h"
#include "bsp/flash/hyperflash.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff_ram, *rcv_buff_flash;
static uint32_t ram_buff, flash_buff;
static struct pi_device flash;
static struct pi_device ram;
static struct pi_hyperflash_conf flash_conf;
static struct pi_hyperram_conf ram_conf;

#ifdef ASYNC
static volatile uint8_t done_flash = 0, done_ram = 0;
/* Callback for end of flash RX transfer. */
void __end_of_flash_rx(void *arg)
{
    printf("Flash: read async done.\n");
    done_flash = 1;
}

/* Callback for end of flash TX transfer. */
void __end_of_flash_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Flash: write async done.\n");
    pi_flash_read_async(&flash, flash_buff, rcv_buff_flash, (uint32_t) BUFFER_SIZE, cb);
}

/* Callback for end of erase. */
void __end_of_erase(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Flash: erase async done.\n");
    pi_flash_program_async(&flash, flash_buff, buff, (uint32_t) BUFFER_SIZE, cb);
}

/* Callback for end of ram RX transfer. */
void __end_of_ram_rx(void *arg)
{
    printf("Ram: read async done.\n");
    done_ram = 1;
}

/* Callback for end of ram TX transfer. */
void __end_of_ram_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Ram: write async done.\n");
    pi_ram_read_async(&ram, ram_buff, rcv_buff_ram, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void test_hyper_ram_flash(void)
{
    printf("Entering main controller\n");

    uint32_t errors_ram = 0, errors_flash = 0;
    struct pi_flash_info flash_info;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff_flash = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff_flash == NULL)
    {
        printf("rcv_buff_flash alloc failed !\n");
        pmsis_exit(-2);
    }

    rcv_buff_ram = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff_ram == NULL)
    {
        printf("rcv_buff_ram alloc failed !\n");
        pmsis_exit(-3);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff_flash[i] = 0;
        rcv_buff_ram[i] = 0;
    }

    /* Init & open flash. */
    pi_hyperflash_conf_init(&flash_conf);
    pi_open_from_conf(&flash, &flash_conf);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-4);
    }

    /* Init & open ram. */
    pi_hyperram_conf_init(&ram_conf);
    pi_open_from_conf(&ram, &ram_conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-5);
    }

    if (pi_ram_alloc(&ram, &ram_buff, (uint32_t) BUFFER_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-6);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", ram_buff, (uint32_t) BUFFER_SIZE);
    }

    /* Retrieve info on flash. */
    pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *) &flash_info);
    flash_buff = ((flash_info.flash_start + flash_info.sector_size - 1) & ~(flash_info.sector_size - 1)) + 128;

    /* Erase a sector in flash, write a buffer then read back from flash. */
    /* Write a buffer in ram, then read back from ram. */
    #ifdef ASYNC
    pi_task_t cb_erase, cb_flash_tx, cb_flash_rx, cb_ram_tx, cb_ram_rx;
    pi_task_callback(&cb_erase, __end_of_erase, &cb_flash_tx);
    pi_task_callback(&cb_flash_tx, __end_of_flash_tx, &cb_flash_rx);
    pi_task_callback(&cb_flash_rx, __end_of_flash_rx, NULL);
    pi_task_callback(&cb_ram_tx, __end_of_ram_tx, &cb_ram_rx);
    pi_task_callback(&cb_ram_rx, __end_of_ram_rx, NULL);
    pi_flash_erase_async(&flash, flash_buff, (uint32_t) BUFFER_SIZE, &cb_erase);
    pi_ram_write_async(&ram, ram_buff, buff, (uint32_t) BUFFER_SIZE, &cb_ram_tx);
    while (!done_flash || !done_ram)
    {
        pi_yield();
    }
    #else
    pi_flash_erase(&flash, flash_buff, (uint32_t) BUFFER_SIZE);
    printf("Flash: erase sync done.\n");

    pi_ram_write(&ram, ram_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Ram: write sync done.\n");

    pi_flash_program(&flash, flash_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Flash: write sync done.\n");

    pi_ram_read(&ram, ram_buff, rcv_buff_ram, (uint32_t) BUFFER_SIZE);
    printf("Ram: read sync done.\n");

    pi_flash_read(&flash, flash_buff, rcv_buff_flash, (uint32_t) BUFFER_SIZE);
    printf("Flash: read sync done.\n");
    #endif  /* ASYNC */

    /* Verification. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff_ram[i])
        {
            errors_ram++;
            printf("%2x-%2x ", buff[i], rcv_buff_ram[i]);
        }
    }
    printf("\nHyperram transfer done with %ld error(s) !\n", errors_ram);

    for(uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff_flash[i])
        {
            errors_flash++;
            printf("%2x-%2x ", buff[i], rcv_buff_flash[i]);
        }
    }
    printf("\nHyperflash transfer done with %ld error(s) !\n", errors_flash);

    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff_flash, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff_ram, (uint32_t) BUFFER_SIZE);
    pi_ram_free(&ram, ram_buff, (uint32_t) BUFFER_SIZE);
    pi_ram_close(&ram);
    pi_flash_close(&flash);

    uint32_t errors = errors_ram + errors_flash;
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperflash & Hyperram Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram_flash);
}
