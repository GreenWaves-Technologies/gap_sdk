/**
 * Decompressor unit test: decompress data using T1 mode
 **/

#include <stdio.h>

#include "pmsis.h"
#include "pmsis/cluster/dma/cl_dma_decompressor.h"

//#define PRINTF(...)
#define PRINTF(...) printf(__VA_ARGS__)

static struct pi_device decomp;
static volatile int errors = 0;

#define L2_BUFFER_SIZE (10)
#define L1_BUFFER_SIZE (20)

/* L2 compressed data buffer */
//static PI_L2 uint8_t l2_buffer[L2_BUFFER_SIZE] = {0};
PI_L2 unsigned int DataOut[313] = { 0 };

/* L1 decompressed data buffer */
PI_L1 signed char DataIn[500] = { 0 };

#define TRANSFER_NUMBER (10)
static PI_L1 int8_t l1_buffer[TRANSFER_NUMBER][500];

static PI_L1 pi_cl_dma_decompressor_cmd_t cmd[TRANSFER_NUMBER] = {0};
/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
void cluster_test_main(void* arg)
{
    (void) arg;

    PRINTF("Executing T1 decompression from cluster\n");

    uint32_t nb_items = sizeof(DataIn)/sizeof(DataIn[0]);
    for(size_t i = 0; i < TRANSFER_NUMBER; i++)
    {
        cmd[i].ext = (uint32_t) DataOut;
        cmd[i].loc = (uint32_t) l1_buffer[i];

        cmd[i].conf.decompr_mode        = PI_CL_DMA_DECOMPRESSOR_MODE_T1;
        cmd[i].conf.extension_type      = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_8;
        cmd[i].conf.item_bit_width      = 1;
        cmd[i].conf.sign_extension      = PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_SIGNED;
        cmd[i].conf.start_bit           = 0;
        cmd[i].conf.decompr_direction   = PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC;
        cmd[i].conf.start_byte          = 0;
        cmd[i].conf.items_to_transfer   = 1;

        cmd[i].lut_size                 = 0;
        cmd[i].lut                      = NULL;
        cmd[i].special_symbol           = 0xabbaabba;

        cmd[i].transf_type              = PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN;
        cmd[i].l2_2d.count              = 0;
        cmd[i].l2_2d.stride             = 0;
        cmd[i].tcdm_2d.count            = 0;
        cmd[i].tcdm_2d.stride           = 0;

        cmd[i].done                     = 0;
        cmd[i].flag.all                 = 0;

    }

    pi_cl_dma_decompressor_init();

    /* launch the decompression */
    for(size_t i = 0; i < TRANSFER_NUMBER; i++)
    {
        pi_cl_dma_decompressor_cmd(&cmd[i], cmd[i].ext, cmd[i].loc, cmd[i].conf, cmd[i].transf_type, cmd[i].lut_size);
    }

    for(size_t i = 0; i < TRANSFER_NUMBER; i++)
    {
        printf("Waiting for transfer %ld\n", i);
        pi_cl_dma_decompressor_wait(&cmd[i]);
    }

    /* verify decompressed data */
    //TODO useless ?
    printf("Verifying decompressed data\n");

    for(size_t j = 0; j < TRANSFER_NUMBER; j++)
    {
        for(uint32_t i = 0; i < nb_items; i++)
        {
            if(l1_buffer[j][i] != DataIn[i])
            {
                errors++;
                printf("Elt %d incorrect, got %lx, expected %lx\n", i, l1_buffer[j][i], DataIn[i]);
            }
        }
    }

    if(errors)
    {
        printf("test returned with %d errors\n", errors);
    }
    else
    {
        printf("Test succeeded\n");
    }
}

int test_main(void)
{
    uint32_t core_id = pi_core_id(), cluster_id = pi_cluster_id();

    struct pi_device cluster_dev = {0};
    struct pi_cluster_conf cl_conf = {0};

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&cl_conf);
    cl_conf.id = 0;
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task cl_task = {0};
    cl_task.entry = cluster_test_main;
    cl_task.arg = NULL;

    pi_cluster_send_task_to_cl(&cluster_dev, &cl_task);

    pi_cluster_close(&cluster_dev);

    pmsis_exit(errors);
    return 0;
}

int main(void)
{
    return pmsis_kickoff((void*) test_main);
}

