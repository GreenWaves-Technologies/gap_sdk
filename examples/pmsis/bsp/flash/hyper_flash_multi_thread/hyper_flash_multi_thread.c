/* PMSIS includes */
#include "pmsis.h"
#include "bsp/flash.h"
#include "bsp/flash/hyperflash.h"

/* Variables used. */
#define NB_TASKS         ( 4 )
#define BUFFER_SIZE      ( 2048 )

struct args_s
{
    pi_device_t *flash;
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
    pi_flash_read_async(args->flash, args->hyper_buff, args->rcv_buff, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

void transfer_test(void *parameters)
{
    struct args_s *args = (struct args_s *) parameters;
    //printf("Task_%d exec now\n", args->id);
    /* Write a buffer then read back from flash. */
    #if defined(ASYNC)
    struct pi_task cb_tx, cb_rx;
    pi_task_block(&cb_rx);
    cb_rx.arg[1] = (uint32_t) args;
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_flash_program_async(args->flash, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    pi_task_wait_on(&cb_rx);
    printf("Task_%d: read async done.\n", args->id);
    #else
    pi_flash_program(args->flash, args->hyper_buff, args->buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: write sync done.\n", args->id);
    pi_flash_read(args->flash, args->hyper_buff, args->rcv_buff, (uint32_t) BUFFER_SIZE);
    printf("Task_%d: read sync done.\n", args->id);
    #endif  /* ASYNC */

    /* Suspend current task. */
    vTaskSuspend(NULL);
}

void hyper_flash(void)
{
    printf("Entering main controller\n\n");

    uint32_t errors = 0;
    uint8_t *buff[NB_TASKS], *rcv_buff[NB_TASKS];
    uint32_t hyper_buff;

    struct pi_flash_info flash_info;

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

    struct pi_device flash;
    struct pi_hyperflash_conf conf;
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
    pi_flash_erase(&flash, hyper_buff, (uint32_t) BUFFER_SIZE);

    static struct args_s args[NB_TASKS];
    TaskHandle_t xHandler[NB_TASKS] = {NULL};
    BaseType_t xTask;
    char taskname[8];
    for (uint32_t i=0; i<(uint32_t) NB_TASKS; i++)
    {
        sprintf(taskname, "Task_%d", i);
        args[i].flash = &flash;
        args[i].buff = buff[i];
        args[i].rcv_buff = rcv_buff[i];
        args[i].hyper_buff = hyper_buff + (((uint32_t) BUFFER_SIZE) * i);
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
    }
    pi_flash_close(&flash);

    printf("\nHyperflash transfer done with %ld error(s) !\n", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperflash w/ multiple threads ***\n\n");
    return pmsis_kickoff((void *) hyper_flash);
}
