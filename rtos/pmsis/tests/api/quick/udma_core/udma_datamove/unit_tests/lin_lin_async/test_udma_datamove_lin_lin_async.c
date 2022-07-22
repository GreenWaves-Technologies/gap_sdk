/* PMSIS includes */
#include "pmsis.h"

/* --------- */
/* constants */
/* --------- */
#define NB_DEVICES (20)
#define DATA_SIZE (100)

/* --------- */
/* variables */
/* --------- */

static pi_device_t udma_datamove_device[NB_DEVICES] = {0};
static pi_udma_datamove_conf_t conf[NB_DEVICES] = {0};

static pi_task_t block[NB_DEVICES];
static uint8_t src_data[NB_DEVICES][DATA_SIZE];
static uint8_t dst_data[NB_DEVICES][DATA_SIZE];

/* ---------------- */
/* static functions */
/* ---------------- */

/* main test */
static void test_lin_lin(void)
{
    /* open a bunch of datamove devices */
    printf("Opening devices\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        pi_udma_datamove_conf_init(&conf[j]);
        pi_open_from_conf(&udma_datamove_device[j], &conf[j]);
        int ret = pi_udma_datamove_open(&udma_datamove_device[j]);

        if (0 != ret)
        {
            pmsis_exit(-1);
        }

        pi_task_block(&block[j]);
    }

    /* initializing data */
    printf("Initializing data\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        for (int32_t k = 0; k < DATA_SIZE; k++)
        {
            src_data[j][k] = k + j;
            dst_data[j][k] = j;
        }
    }

    /* copy data */
    printf("Copying data (async)\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        pi_udma_datamove_copy_async(&udma_datamove_device[j], src_data[j], dst_data[j], DATA_SIZE, &block[j]);
    }
    /* waiting */
    printf("Waiting\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        pi_task_wait_on(&block[j]);
    }
    /* verify data */
    printf("Verifying data\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        for (int32_t k = 0; k < DATA_SIZE; k++)
        {
            if (src_data[j][k] != dst_data[j][k])
            {
                printf("ERROR(#%d): expected %x, got %x\n", j, src_data[j][k], dst_data[j][k]);
                pmsis_exit(-1);
            }
        }
    }

    /* close devices */
    printf("Closing devices\n");
    for (int32_t j = 0; j < NB_DEVICES; j++)
    {
        pi_udma_datamove_close(&udma_datamove_device[j]);
    }

    pmsis_exit(0);
}

/* -------------- */
/* Program Entry. */
/* -------------- */
int main(void)
{
    printf("\n\n\t *** PMSIS UDMA DATAMOVE LIN/LIN Tests ***\n\n");
    return pmsis_kickoff((void *) test_lin_lin);
}

