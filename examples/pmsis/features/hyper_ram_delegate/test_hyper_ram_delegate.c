/* PMSIS includes */
#include "pmsis.h"
#include "bsp/ram.h"
#include "bsp/ram/hyperram.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff1, *rcv_buff2, *rcv_buff3;
static uint32_t hyper_buff;
static volatile uint32_t done = 0;
static struct pi_device ram;
static struct pi_hyperram_conf conf;

#ifdef ASYNC
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
    pi_ram_read_async(&ram, hyper_buff, rcv_buff1, (uint32_t) BUFFER_SIZE, cb);
}
#endif  /* ASYNC */

/* Task executed by cluster cores. */
/* Cluster main entry, executed by core 0. */
void cluster_delegate(void *arg)
{
    printf("Cluster master entry.\n");
    /* Send a request to FC to allocate memory in ram and wait for termination of req. */
    pi_cl_ram_alloc_req_t alloc_req;
    pi_cl_ram_alloc(&ram, (uint32_t) BUFFER_SIZE, &alloc_req);
    if (pi_cl_ram_alloc_wait(&alloc_req, NULL))
    {
        printf("Cl ram malloc failed !\n");
        return;
    }
    printf("Allocated %ld Bytes in Hyperram %lx.\n", alloc_req.size, alloc_req.result);

    pi_cl_ram_req_t buff_req, buff_req1, buff_req2;
    /* Send a request to FC to write in ram and wait for termination of req. */
    pi_cl_ram_write(&ram, alloc_req.result, buff, alloc_req.size, &buff_req);
    pi_cl_ram_write_wait(&buff_req);
    printf("Cluster write done.\n");

    /* Send requests to FC to read fron ram and wait for termination of reqs. */
    pi_cl_ram_read(&ram, alloc_req.result, rcv_buff2, alloc_req.size, &buff_req1);
    pi_cl_ram_read(&ram, alloc_req.result, rcv_buff3, alloc_req.size, &buff_req2);
    pi_cl_ram_read_wait(&buff_req1);
    printf("Cluster read 1 done.\n");
    pi_cl_ram_read_wait(&buff_req2);
    printf("Cluster read 2 done.\n");

    /* Send a request to FC to free allocated memory and wait for termination of req. */
    pi_cl_ram_free_req_t free_req;
    pi_cl_ram_free(&ram, alloc_req.result, alloc_req.size, &free_req);
    pi_cl_ram_free_wait(&free_req);
    printf("Freed %ld Bytes in Hyperram %lx.\n", free_req.size, free_req.chunk);
}

void test_hyper_ram_delegate(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;

    buff = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (buff == NULL)
    {
        printf("buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff1 = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff1 == NULL)
    {
        printf("rcv_buff1 alloc failed !\n");
        pmsis_exit(-2);
    }

    rcv_buff2 = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff2 == NULL)
    {
        printf("rcv_buff2 alloc failed !\n");
        pmsis_exit(-3);
    }

    rcv_buff3 = (uint8_t *) pmsis_l2_malloc((uint32_t) BUFFER_SIZE);
    if (rcv_buff3 == NULL)
    {
        printf("rcv_buff3 alloc failed !\n");
        pmsis_exit(-4);
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff1[i] = 0;
        rcv_buff2[i] = 0;
        rcv_buff3[i] = 0;
    }

    /* Init & open ram. */
    pi_hyperram_conf_init(&conf);
    pi_open_from_conf(&ram, &conf);
    if (pi_ram_open(&ram))
    {
        printf("Error ram open !\n");
        pmsis_exit(-5);
    }

    if (pi_ram_alloc(&ram, &hyper_buff, (uint32_t) BUFFER_SIZE))
    {
        printf("Ram malloc failed !\n");
        pmsis_exit(-6);
    }
    else
    {
        printf("Ram allocated : %lx %ld.\n", hyper_buff, (uint32_t) BUFFER_SIZE);
    }

    /* Write a buffer in ram, then read back from ram. */
    #ifdef ASYNC
    pi_task_t cb_rx, cb_tx;
    pi_task_callback(&cb_rx, __end_of_rx, NULL);
    pi_task_callback(&cb_tx, __end_of_tx, &cb_rx);
    pi_ram_write_async(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE, &cb_tx);
    /* Yield while task is not completed. */
    while (!done)
    {
        pi_yield();
    }
    #else
    pi_ram_write(&ram, hyper_buff, buff, (uint32_t) BUFFER_SIZE);
    pi_ram_read(&ram, hyper_buff, rcv_buff1, (uint32_t) BUFFER_SIZE);
    #endif  /* ASYNC */

    /* Verification. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff1[i])
        {
            errors++;
            printf("%x-%x ", buff[i], rcv_buff1[i]);
        }
    }

    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-7);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task = {0};
    cl_task.entry = cluster_delegate;
    cl_task.arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    /* Verification. */
    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff2[i])
        {
            errors++;
            printf("%x-%x ", buff[i], rcv_buff2[i]);
        }
    }

    for (uint32_t i=0; i<(uint32_t) BUFFER_SIZE; i++)
    {
        if (buff[i] != rcv_buff3[i])
        {
            errors++;
            printf("%x-%x ", buff[i], rcv_buff3[i]);
        }
    }

    pi_ram_free(&ram, hyper_buff, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff3, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff2, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(rcv_buff1, (uint32_t) BUFFER_SIZE);
    pmsis_l2_malloc_free(buff, (uint32_t) BUFFER_SIZE);

    pi_ram_close(&ram);

    printf("\nHyperram delegate transfer done with %ld error(s) !\n", errors);
    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Hyperram delegate Test ***\n\n");
    return pmsis_kickoff((void *) test_hyper_ram_delegate);
}
