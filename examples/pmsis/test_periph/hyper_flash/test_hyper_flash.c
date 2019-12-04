/* PMSIS includes */
#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/flash/hyperflash.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff;
static uint32_t hyper_buff;
static struct pi_device flash;
static struct pi_hyperflash_conf conf;

#ifdef ASYNC
static volatile uint8_t done = 0;

/* Callback for end of RX transfer. */
void __end_of_rx(void *arg)
{
    printf("Read async done.\n");
    done = 1;
}

/* Callback for end of TX transfer. */
void __end_of_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Write async done.\n");
    pi_flash_read_async(&flash, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE, cb);
}

/* Callback for end of erase. */
void __end_of_erase(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
    printf("Erase async done.\n");
    pi_flash_program_async(&flash, hyper_buff, buff, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void test_hyper_flash(void)
{
    printf("Entering main controller\n\n");

    uint32_t errors = 0;
    struct pi_flash_info flash_info;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff == NULL)
    {
        printf("rcv_buff alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff[i] = 0;
    }

    /* Init & open flash. */
    pi_hyperflash_conf_init(&conf);
    pi_open_from_conf(&flash, &conf);
    if (pi_flash_open(&flash))
    {
        printf("Error flash open !\n");
        pmsis_exit(-3);
    }

    /* Retrieve info on flash. */
    pi_flash_ioctl(&flash, PI_FLASH_IOCTL_INFO, (void *) &flash_info);
    hyper_buff = ((flash_info.flash_start + flash_info.sector_size - 1) &
                  ~(flash_info.sector_size - 1)) + 128;
    /* Erase a sector in flash, write a buffer then read back from flash. */
    #ifdef ASYNC
    pi_task_t cb_erase, cb_tx, cb_rx;
    pi_task_callback(&cb_erase, __end_of_erase, &cb_tx);
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_task_callback(&cb_rx, __end_of_rx, NULL);
    pi_flash_erase_async(&flash, hyper_buff, (uint32_t) BUFFER_SIZE, &cb_erase);
    while (!done)
    {
        pi_yield();
    }
    #else
    pi_flash_erase(&flash, hyper_buff, (uint32_t) BUFFER_SIZE);
    printf("Erase sync done.\n");
    pi_flash_program(&flash, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Write sync done.\n");
    pi_flash_read(&flash, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Read sync done.\n");
    #endif  /* ASYNC */

    /* Verification. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff[i])
        {
            errors++;
            printf("%2x-%2x ", buff[i], rcv_buff[i]);
        }
    }

    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff, (uint32_t) BUFFER_SIZE);
    pi_flash_close(&flash);

    printf("\nHyperflash transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperflash Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_flash);
}
