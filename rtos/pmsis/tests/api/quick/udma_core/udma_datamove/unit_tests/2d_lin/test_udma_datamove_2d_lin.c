/* PMSIS includes */
#include "pmsis.h"

/* --------- */
/* constants */
/* --------- */
#define NB_DEVICES (4)
#define DATA_SIZE (100)

/* --------- */
/* variables */
/* --------- */

static pi_device_t udma_datamove_device[NB_DEVICES] = {0};
static pi_udma_datamove_conf_t conf[NB_DEVICES] = {0};

static uint8_t dst_data[NB_DEVICES][DATA_SIZE];
static uint8_t src_data[NB_DEVICES][DATA_SIZE * (2 * NB_DEVICES)];

/* ---------------- */
/* static functions */
/* ---------------- */

/* main test */
static void test_2d_lin(void)
{
    /* open a bunch of datamove devices */
    printf("Opening devices\n");
    for (size_t j = 0; j < NB_DEVICES; j++)
    {
        pi_udma_datamove_conf_init(&conf[j]);
        /* configure */
        conf[j].device_id = j % 2;
        conf[j].dst_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_LINEAR;
        conf[j].src_trf_cfg.type = PI_UDMA_DATAMOVE_TRF_2D;
        conf[j].src_trf_cfg.row_len = (j+1);
        conf[j].src_trf_cfg.stride = ((2*j)+1);

        /* open */
        pi_open_from_conf(&udma_datamove_device[j], &conf[j]);
        int ret = pi_udma_datamove_open(&udma_datamove_device[j]);

        if (0 != ret)
        {
            pmsis_exit(-1);
        }
    }

    /* initializing data */
    size_t src_data_size = sizeof(src_data[0])/sizeof(src_data[0][0]);
    printf("Initializing data[size: %d]\n", src_data_size);
    for (size_t j = 0; j < NB_DEVICES; j++)
    {
        for (size_t k = 0; k < src_data_size; k++)
        {
            src_data[j][k] = (k + j) % 100;
        }
        for (size_t k = 0; k < DATA_SIZE; k++)
        {
            dst_data[j][k] = j % 100;
        }
    }

    /* copy data */
    printf("Copying data\n");
    for (size_t j = 0; j < NB_DEVICES; j++)
    {
        pi_udma_datamove_copy(&udma_datamove_device[j], src_data[j], dst_data[j], DATA_SIZE);
    }
    /* verify data */
    printf("Verifying data\n");
    for (size_t j = 0; j < NB_DEVICES; j++)
    {
        const int32_t row_len = (j+1);
        const int32_t stride = (2*j) + 1;
        int32_t row_counter = 0;
        int32_t stride_counter = 0;
        for (size_t k = 0; k < DATA_SIZE; k++)
        {
            if (row_counter >= row_len)
            {
                row_counter = 0;
                stride_counter++;
            }
            int index_2d = row_counter + stride * stride_counter;
            if (src_data[j][index_2d] != dst_data[j][k])
            {
                printf("ERROR(p%d-#%d): expected %x, got %x\n", j, k, src_data[j][index_2d], dst_data[j][k]);
                pmsis_exit(-1);
            }
            row_counter++;
        }
    }

    /* close devices */
    printf("Closing devices\n");
    for (size_t j = 0; j < NB_DEVICES; j++)
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
    printf("\n\n\t *** PMSIS UDMA DATAMOVE 2D/LIN Tests ***\n\n");
    return pmsis_kickoff((void *) test_2d_lin);
}

