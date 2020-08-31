/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff_0, *rcv_buff_1, *rcv_buff_2;
static struct pi_device dmacpy;
static int32_t errors = 0;

static void check_buffer(uint8_t *buff_1, uint8_t *buff_2, uint32_t size, int32_t *error)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (buff_1[i] != buff_2[i])
        {
            (*error)++;
            printf("%d) %2x - %2x\n", i, buff_1[i], buff_2[i]);
        }
    }
}
static void print_data_buffer(uint8_t *buff, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        printf(" %2x ", buff[i]);
    }
    printf("\n");
}

#if defined(ASYNC)
volatile uint8_t done = 0;

typedef struct dma_memcpy_s
{
    uint8_t *src;
    uint8_t *dst;
    uint32_t size;
    pi_dmacpy_dir_e dir;
    pi_task_t *task;
    struct dma_memcpy_s *next;
} dma_memcpy_t;

static void __end_of_copy(void *arg)
{
    dma_memcpy_t *copy = (dma_memcpy_t *) arg;
    dma_memcpy_t *next = copy->next;
    printf("Copy from %p to %p done.\n", copy->src, copy->dst);
    done++;
    if (next != NULL)
    {
        pi_task_callback(next->task, __end_of_copy, copy->next);
        errors = pi_dmacpy_copy_async(&dmacpy, (void *) next->src, (void *) next->dst,
                                      next->size, next->dir, next->task);
        if (errors)
        {
            printf("Copy from %p to %p failed : %ld\n", next->src, next->dst, errors);
            done = 4;
        }
    }
}
#endif  /* ASYNC */

void test_dmacpy(void)
{
    printf("Entering main controller\n");

    uint32_t cpy_size = 264;//(uint32_t) BUFFER_SIZE;
    uint32_t print_buffer = 0;

    buff = (uint8_t *) pi_fc_l1_malloc(cpy_size);
    if (buff == NULL)
    {
        printf("Buff alloc failed !\n");
        pmsis_exit(-1);
    }

    rcv_buff_0 = (uint8_t *) pi_l2_malloc(cpy_size);
    if (rcv_buff_0 == NULL)
    {
        printf("Rcv_buff_0 alloc failed !\n");
        pmsis_exit(-2);
    }

    rcv_buff_1 = (uint8_t *) pi_l2_malloc(cpy_size);
    if (rcv_buff_1 == NULL)
    {
        printf("Rcv_buff_1 alloc failed !\n");
        pmsis_exit(-2);
    }

    rcv_buff_2 = (uint8_t *) pi_fc_l1_malloc(cpy_size);
    if (rcv_buff_2 == NULL)
    {
        printf("Rcv_buff_2 alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<cpy_size; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff_0[i] = 0;
        rcv_buff_1[i] = 0;
        rcv_buff_2[i] = 0;
    }

    /* Init & open dmacpy. */
    struct pi_dmacpy_conf dmacpy_conf = {0};
    pi_dmacpy_conf_init(&dmacpy_conf);
    pi_open_from_conf(&dmacpy, &dmacpy_conf);
    errors = pi_dmacpy_open(&dmacpy);
    if (errors)
    {
        printf("Error dmacpy open : %ld !\n", errors);
        pmsis_exit(-3);
    }

    #if defined(ASYNC)
    printf("Copy async from FC ( %p ) -> L2 ( %p ) -> L2 ( %p ) -> FC ( %p ), "
           "size = %d Bytes.\n",
           buff, rcv_buff_0, rcv_buff_1, rcv_buff_2, cpy_size);

    pi_task_t task_fcl2 = {0}, task_l2l2 = {0}, task_l2fc = {0}, task_copy = {0};
    dma_memcpy_t copy_fcl2 = {0}, copy_l2l2 = {0}, copy_l2fc = {0};
    copy_fcl2.src = buff;
    copy_fcl2.dst = rcv_buff_0;
    copy_fcl2.size = cpy_size;
    copy_fcl2.dir = PI_DMACPY_FC_L1_L2;
    copy_fcl2.task = &task_copy;
    copy_fcl2.next = &copy_l2l2;

    copy_l2l2.src = rcv_buff_0;
    copy_l2l2.dst = rcv_buff_1;
    copy_l2l2.size = cpy_size;
    copy_l2l2.dir = PI_DMACPY_L2_L2;
    copy_l2l2.task = &task_copy;
    copy_l2l2.next = &copy_l2fc;

    copy_l2fc.src = rcv_buff_1;
    copy_l2fc.dst = rcv_buff_2;
    copy_l2fc.size = cpy_size;
    copy_l2fc.dir = PI_DMACPY_L2_FC_L1;
    copy_l2fc.task = &task_copy;
    copy_l2fc.next = NULL;

    pi_task_callback(&task_copy, __end_of_copy, &copy_fcl2);
    errors = pi_dmacpy_copy_async(&dmacpy, (void *) buff, (void *) rcv_buff_0,
                                  cpy_size, PI_DMACPY_FC_L1_L2,
                                  &task_copy);
    if (errors)
    {
        printf("Copy from FC to L2 failed : %ld\n", errors);
        pmsis_exit(-4);
    }
    while (done < 3)
    {
        pi_yield();
    }
    if (done == 4)
    {
        printf("Error while copying\n");
        pmsis_exit(-5);
    }
    #else
    printf("Copy sync from FC ( %p ) -> L2 ( %p ) -> L2 ( %p ) -> FC ( %p ), "
           "size = %d Bytes.\n",
           buff, rcv_buff_0, rcv_buff_1, rcv_buff_2, cpy_size);

    /* Copy buffer from FC to L2. */
    errors = pi_dmacpy_copy(&dmacpy, (void *) buff, (void *) rcv_buff_0,
                            cpy_size, PI_DMACPY_FC_L1_L2);
    if (errors)
    {
        printf("Copy from FC to L2 failed : %ld\n", errors);
        pmsis_exit(-4);
    }
    printf("Copy from %p to %p done.\n", buff, rcv_buff_0);

    /* Copy buffer from L2 to L2. */
    errors = pi_dmacpy_copy(&dmacpy, (void *) rcv_buff_0, (void *) rcv_buff_1,
                            cpy_size, PI_DMACPY_L2_L2);
    if (errors)
    {
        printf("Copy from L2 to L2 failed : %ld\n", errors);
        pmsis_exit(-5);
    }
    printf("Copy from %p to %p done.\n", rcv_buff_0, rcv_buff_1);

    /* Copy buffer from L2 to FC. */
    errors = pi_dmacpy_copy(&dmacpy, (void *) rcv_buff_1, (void *) rcv_buff_2,
                            cpy_size, PI_DMACPY_L2_FC_L1);
    if (errors)
    {
        printf("Copy from L2 to FC failed : %ld\n", errors);
        pmsis_exit(-6);
    }
    printf("Copy from %p to %p done.\n", rcv_buff_1, rcv_buff_2);
    #endif  /* ASYNC */

    /* Check buffers data. */
    check_buffer(buff, rcv_buff_0, cpy_size, &errors);
    check_buffer(buff, rcv_buff_1, cpy_size, &errors);
    check_buffer(buff, rcv_buff_2, cpy_size, &errors);

    if (print_buffer)
    {
        printf(" FC SRC \n");
        print_data_buffer(buff, cpy_size);
        printf(" L2 DST \n");
        print_data_buffer(rcv_buff_0, cpy_size);
        printf(" L2 SRC \n");
        print_data_buffer(rcv_buff_1, cpy_size);
        printf(" FC DST \n");
        print_data_buffer(rcv_buff_2, cpy_size);
    }

    pi_fc_l1_free(rcv_buff_2, cpy_size);
    pi_l2_free(rcv_buff_1, cpy_size);
    pi_l2_free(rcv_buff_0, cpy_size);
    pi_fc_l1_free(buff, cpy_size);
    pi_dmacpy_close(&dmacpy);

    printf("\nDmacpy transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Dmacpy Example ***\n\n");
    return pmsis_kickoff((void *) test_dmacpy);
}
