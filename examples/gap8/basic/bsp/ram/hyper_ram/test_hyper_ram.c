/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff;
static uint32_t hyper_buff;
static struct pi_device ram;
static struct pi_hyperram_conf conf;

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
    pi_ram_read_async(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void test_hyper_ram(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;

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

    /* Init & open ram. */
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    if (pi_ram_alloc(&ram, &hyper_buff, (uint32_t) BUFFER_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-4);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", hyper_buff, (uint32_t) BUFFER_SIZE);
    }

    /* Write a buffer in ram, then read back from ram. */
    #ifdef ASYNC
    pi_task_t cb_tx, cb_rx;
    pi_task_callback(&cb_rx, __end_of_rx, NULL);
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_ram_write_async(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    while (!done)
    {
        pi_yield();
    }
    #else
    pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
    printf("Write sync done.\n");
    pi_ram_read(&ram, hyper_buff, rcv_buff, (uint32_t) BUFFER_SIZE);
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
    pi_ram_free(&ram, hyper_buff, (uint32_t) BUFFER_SIZE);
    pi_ram_close(&ram);

    printf("\nHyperram transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperram Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram);
}
