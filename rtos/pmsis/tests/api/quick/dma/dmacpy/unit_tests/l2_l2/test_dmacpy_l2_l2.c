/* PMSIS includes */
#include "pmsis.h"

/* Variables used. */
#define BUFFER_SIZE      ( 2048 )

static uint8_t *buff, *rcv_buff_0;
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

    rcv_buff_0 = (uint8_t *) pi_l2_malloc(cpy_size);
    if (rcv_buff_0 == NULL)
    {
        printf("Rcv_buff_0 alloc failed !\n");
        pmsis_exit(-2);
    }

    for (uint32_t i=0; i<cpy_size; i++)
    {
        buff[i] = i & 0xFF;
        rcv_buff_0[i] = 0;
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

    printf("Copy sync from L2 ( %p ) -> L2 ( %p ), "
           "size = %d Bytes.\n",
           buff, rcv_buff_0,
           cpy_size);

    /* Copy buffer from L2 to L2. */
    errors = pi_dmacpy_copy(&dmacpy, (void *) buff, (void *) rcv_buff_0,
                            cpy_size, PI_DMACPY_L2_L2);
    if (errors)
    {
        printf("Copy from L2 to L2 failed : %ld\n", errors);
        pmsis_exit(-5);
    }
    printf("Copy done\n");

    /* Check buffers data. */
    check_buffer(buff, rcv_buff_0, cpy_size, &errors);

    pi_l2_free(rcv_buff_0, cpy_size);
    pi_l2_free(buff, cpy_size);
    pi_dmacpy_close(&dmacpy);

    printf("\nDmacpy transfer done with %ld error(s) !\n", errors);
    printf("\nTest %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Dmacpy L2/L2 test ***\n\n");
    return pmsis_kickoff((void *) test_dmacpy);
}
