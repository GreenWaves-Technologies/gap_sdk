/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define NB_TASKS         ( 4 )
#define BUFFER_SIZE      ( 2048 )

TaskHandle_t xHandler[NB_TASKS] = {NULL};

struct args_s
{
    pi_device_t *ram;
    uint8_t *buff;
    uint8_t *rcv_buff;
    uint32_t hyper_buff;
    uint8_t id;
};

static uint8_t *buff[NB_TASKS], *rcv_buff[NB_TASKS];
static uint32_t hyper_buff[NB_TASKS];
static struct pi_device ram;
static struct pi_hyperram_conf conf;
static struct args_s args[NB_TASKS];

static volatile uint8_t done = 0;

#if defined(ASYNC)
/* Callback for end of RX transfer. */
void __end_of_rx(void *arg)
{
    struct args_s *args = (struct args_s *) arg;
    printf("Task_%d: read async done.\n", args->id);
    done |= (1 << args->id);
}

/* Callback for end of TX transfer. */
void __end_of_tx(void *arg)
{
    pi_task_t *cb = (pi_task_t *) arg;
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
    pi_task_t cb_tx, cb_rx;
    pi_task_callback(&cb_rx, __end_of_rx, args);
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_ram_write_async(args->ram, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    #else
    pi_ram_write(args->ram, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: write sync done.\n", args->id);
    pi_ram_read(args->ram, args->hyper_buff, args->rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: read sync done.\n", args->id);
    done |= (1 << args->id);
    #endif  /* ASYNC */
    /* Suspend current task. */
    vTaskSuspend(NULL);
}

void test_hyper_ram(void)
{
    printf("Entering main controller\n");

    uint32_t errors = 0;

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
        xTask = xTaskCreate(transfer_test, taskname, configMINIMAL_STACK_SIZE * 2,
                            (void *) &args[i], tskIDLE_PRIORITY + 1, &xHandler[i]);
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

    do
    {
        pi_yield();
    } while (((1 << NB_TASKS) - 1) != done);

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
