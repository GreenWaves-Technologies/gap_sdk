/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define NB_TASKS         ( 4 )
#define BUFFER_SIZE      ( 2048 )

struct args_s
{
    pi_device_t *ram;
    uint8_t *buff;
    uint8_t *rcv_buff;
    uint32_t hyper_buff;
    uint8_t id;
};

#if defined(ASYNC)
/* Callback for end of TX transfer. */
void __end_of_tx(void *arg)
{
    struct pi_task *cb = (struct pi_task *) arg;
    struct args_s *args = (struct args_s *) cb->arg[1];
    printf("Task_%d: write async done.\n", args->id);
    pi_ram_read_async(args->ram, args->hyper_buff, args->rcv_buff, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void transfer_test(void *parameters)
{
    struct args_s *args = (struct args_s *) parameters;
    /* Write a buffer in ram, then read back from ram. */
    #if defined(ASYNC)
    struct pi_task cb_tx = {0}, cb_rx = {0};
    pi_task_block(&cb_rx);
    cb_rx.arg[1] = (uint32_t) args;
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_ram_write_async(args->ram, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    pi_task_wait_on(&cb_rx);
    printf("Task_%d: read async done.\n", args->id);
    #else
    pi_ram_write(args->ram, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: write sync done.\n", args->id);
    pi_ram_read(args->ram, args->hyper_buff, args->rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: read sync done.\n", args->id);
    #endif  /* ASYNC */

    /* Suspend current task. */
    vTaskSuspend(NULL);
}

void hyper_ram(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;
    uint8_t *buff[NB_TASKS], *rcv_buff[NB_TASKS];
    uint32_t hyper_buff[NB_TASKS];

    for (uint32_t i=0; i<(uint32_t) NB_TASKS; i++)
    {
        buff[i] = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
        if (buff[i] == NULL)
        {
            printf("buff alloc failed !\n");
            pmsis_exit(-1 -i);
        }

        rcv_buff[i] = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
        if (rcv_buff[i] == NULL)
        {
            printf("rcv_buff alloc failed !\n");
            pmsis_exit(-2 -i);
        }

        for (uint32_t j=0; j<(uint32_t) BUFFER_SIZE; j++)
        {
            buff[i][j] = ((j + i) & 0xFF);
            rcv_buff[i][j] = 0;
        }
    }

    struct pi_device ram;
    struct pi_hyperram_conf conf;
    /* Init & open ram. */
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-3);
    }

    for (uint32_t i=0; i < (uint32_t) NB_TASKS; i++)
    {
        if (pi_ram_alloc(&ram, &hyper_buff[i], (uint32_t) BUFFER_SIZE))
        {
            printf("Ram malloc failed !\n");
            pmsis_exit(-4 -i);
        }
        else
        {
            printf("Ram allocated : %lx %ld.\n", hyper_buff[i], (uint32_t) BUFFER_SIZE);
        }
    }

    struct args_s args[NB_TASKS];
    TaskHandle_t xHandler[NB_TASKS] = {NULL};
    BaseType_t xTask;
    char taskname[8];
    for (uint32_t i=0; i<(uint32_t) NB_TASKS; i++)
    {
        sprintf(taskname, "Task_%d", i);
        args[i].ram = &ram;
        args[i].buff = buff[i];
        args[i].rcv_buff = rcv_buff[i];
        args[i].hyper_buff = hyper_buff[i];
        args[i].id = i;
        xTask = xTaskCreate(transfer_test, taskname, 512, (void *) &args[i],
                            tskIDLE_PRIORITY + 1, &xHandler[i]);
        if (xTask != pdPASS)
        {
            printf("%s is NULL !\n", taskname);
            pmsis_exit(-5 -i);
        }
        else
        {
            printf("%s created !\n", taskname);
        }
    }

    while( ( eTaskGetState(xHandler[0]) != eSuspended ) ||
           ( eTaskGetState(xHandler[1]) != eSuspended ) ||
           ( eTaskGetState(xHandler[2]) != eSuspended ) ||
           ( eTaskGetState(xHandler[3]) != eSuspended ) )
    {
        printf("%s wait for tasks to finish\n", pcTaskGetName(NULL));
        pi_yield();
    }

    /* Verification. */
    for (uint32_t k=0; k<(uint32_t) NB_TASKS; k++)
    {
        for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
        {
            if (buff[k][i] != rcv_buff[k][i])
            {
                errors++;
                printf("%2x-%2x ", buff[k][i], rcv_buff[k][i]);
            }
        }
    }

    for (uint32_t i=0; i<(uint32_t) NB_TASKS; i++)
    {
        pmsis_l2_malloc_free(buff[i], (uint32_t) BUFFER_SIZE);
        pmsis_l2_malloc_free(rcv_buff[i], (uint32_t) BUFFER_SIZE);
        pi_ram_free(&ram, hyper_buff[i], (uint32_t) BUFFER_SIZE);
    }
    pi_ram_close(&ram);

    printf("\nHyperram transfers done with %ld error(s) !\n", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperram w/ multiple threads ***\n\n");
    return pmsis_kickoff((void *) hyper_ram);
}
