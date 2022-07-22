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

#include "data.cgen"

static PI_L1 typeof(DataIn[0]) l1_buffer[sizeof(DataIn)/sizeof(DataIn[0])] = {0};

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
void cluster_test_main(void* arg)
{
    (void) arg;

    PRINTF("Executing decompression from cluster\n");

    cmd.loc = (uint32_t) l1_buffer;

    pi_cl_dma_decompressor_init();

    /* launch the decompression */
    PRINTF("Enqueueing decompression\n");
    pi_cl_dma_decompressor_cmd(&cmd, cmd.ext, cmd.loc, cmd.conf, cmd.transf_type, cmd.lut_size);

    /* wait for the decompression to end */
    PRINTF("Waiting for decompression\n");
    pi_cl_dma_decompressor_wait(&cmd);

    /* verify decompressed data */
    printf("Verifying decompressed data\n");

    uint32_t nb_items = sizeof(DataIn)/sizeof(DataIn[0]);
    for(uint32_t i = 0; i < nb_items; i++)
    {
        if(l1_buffer[i] != DataIn[i])
        {
            errors++;
            printf("Elt %d incorrect, got %lx, expected %lx\n", i, l1_buffer[i], DataIn[i]);
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

int test_main(void)
{
    printf("TEST\n");
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

