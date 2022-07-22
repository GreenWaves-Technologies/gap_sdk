/*
 * Tests transaction of 2D type with characteristics:
 *    - 2D L2 memory
 *    - 1D (linear) L1 memory
 */

#include <stdint.h>
#include "pmsis.h"
#include "pmsis/cluster/dma/cl_dma_decompressor.h"

static volatile int errors = 0;

// include generated file
#include "compressed_data.c"

static PI_L1 typeof(tcdm_expect_data[0]) l1_buffer[sizeof(tcdm_expect_data)/sizeof(tcdm_expect_data[0])] = {0};

static void cluster_test_main(void* arg)
{
    (void) arg;
    printf("Cluster test main\n");

    printf("Executing T1 decompression from cluster\n");

    cmd.loc = (uint32_t) l1_buffer;

    pi_cl_dma_decompressor_init();

    /* launch the decompression */
    printf("Enqueueing decompression\n");
    pi_cl_dma_decompressor_cmd(&cmd, cmd.ext, cmd.loc, cmd.conf, cmd.transf_type, cmd.lut_size);

    /* wait for the decompression to end */
    printf("Waiting for decompression\n");
    pi_cl_dma_decompressor_wait(&cmd);

    /* verify decompressed data */
    printf("Verifying decompressed data\n");

    uint32_t nb_items = sizeof(tcdm_expect_data)/sizeof(tcdm_expect_data[0]);
    for(uint32_t i = 0; i < nb_items; i++)
    {
        //printf("Elt %d - got: 0x%x, exp:0x%x\n", i, l1_buffer[i], tcdm_expect_data[i]);
        if(l1_buffer[i] != tcdm_expect_data[i])
        {
            errors++;
            printf("Elt %d incorrect, got 0x%x, expected 0x%x\n", i, l1_buffer[i], tcdm_expect_data[i]);
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
    //TODO pass result (errors) to FC
}

static int test_main(void)
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

/* Program Entry. */
int main(void)
{
    printf("\n\n\t *** PMSIS Decomp test - l2 1D => l1 2D ***\n\n");
    return pmsis_kickoff((void *) test_main);
}
