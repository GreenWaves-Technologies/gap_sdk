/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )
#define NB_BUFFERS       (10)

static uint8_t *buff, *rcv_buff[NB_BUFFERS];
pi_task_t block[NB_BUFFERS];
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

void test_dmacpy(void)
{
    printf("Entering main controller\n");

    uint32_t cpy_size = 264;

    buff = (uint8_t *) pi_l2_malloc(cpy_size);
    if (buff == NULL)
    {
        printf("Buff alloc failed !\n");
        pmsis_exit(-1);
    }

    for (size_t i = 0; i < NB_BUFFERS; i++)
    {
        rcv_buff[i] = (uint8_t *) pi_l2_malloc(cpy_size);
        if (rcv_buff[i] == NULL)
        {
            printf("Rcv_buff #%d alloc failed !\n", i);
            pmsis_exit(-2);
        }
        pi_task_block(&block[i]);
    }

    for (uint32_t i=0; i<cpy_size; i++)
    {
        buff[i] = i & 0xFF;
        for (size_t j = 0; j < NB_BUFFERS; j++)
        {
            rcv_buff[j][i] = 0;
        }
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

    printf("Copy async from L2->L2, "
           "size = %d Bytes.\n",
           cpy_size);

    /* Copy buffer from L2 to L2. */
    for (size_t j = 0; j < NB_BUFFERS; j++)
    {
        errors = pi_dmacpy_copy_async(&dmacpy, (void *) buff, (void *) rcv_buff[j],
                                      cpy_size, PI_DMACPY_L2_L2, &block[j]);
    }

    if (errors)
    {
        printf("Copy from L2 to L2 failed : %ld\n", errors);
        pmsis_exit(-5);
    }
    printf("Copy done\n");

    /* Check buffers data. */
    for (size_t j = 0; j < NB_BUFFERS; j++)
    {
        check_buffer(buff, rcv_buff[j], cpy_size, &errors);
        pi_l2_free(rcv_buff[j], cpy_size);
    }

    pi_l2_free(buff, cpy_size);
    pi_dmacpy_close(&dmacpy);

    printf("\nDmacpy transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Dmacpy L2/L2 async test ***\n\n");
    return pmsis_kickoff((void *) test_dmacpy);
}
