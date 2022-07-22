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
PI_L2 unsigned int DataOut[5] = {
	0xCBC1EE7E,
	0x60DCA4D6,
	0xF1FADE23,
	0x8B8E3966,
	0x10C460,
};
/* L1 decompressed data buffer */

PI_L1 unsigned short DataIn2[300] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};
PI_L1 unsigned short DataIn[30] = {
	30 , 19 , 27 , 3  , 28 ,
	5  , 27 , 26 , 4  , 5  ,
	23 , 1  , 22 , 17 , 24 ,
	27 , 26 , 15 , 28 , 13 ,
	22 , 28 , 24 , 17 , 11 ,
	4  , 24 , 8  , 12 , 8  ,
};

static PI_L1 uint16_t l1_buffer[30] = {0};

/**
 * \brief launch the main test
 *
 * \warning hangs indefinitely
 */
void cluster_test_main(void* arg)
{
    (void) arg;

    PRINTF("Executing T1 decompression from cluster\n");

    pi_cl_dma_decompressor_cmd_t cmd = {0};

    uint32_t nb_items = sizeof(DataIn)/sizeof(DataIn[0]);

    printf("DataOut: %lx, l1_buffer: %lx, DataIn2: %lx, DataIn: %lx\n",
            DataOut,      l1_buffer,      DataIn2,        DataIn);

    cmd.ext = (uint32_t) DataOut;
    cmd.loc = (uint32_t) l1_buffer;

    cmd.conf.decompr_mode      = PI_CL_DMA_DECOMPRESSOR_MODE_T1;
    cmd.conf.extension_type    = PI_CL_DMA_DECOMPRESSOR_EXT_TYPE_16;
    cmd.conf.item_bit_width    = 5;
    cmd.conf.sign_extension    = PI_CL_DMA_DECOMPRESSOR_EXT_SIGN_UNSIGNED;
    cmd.conf.start_bit         = 0;
    cmd.conf.decompr_direction = PI_CL_DMA_DECOMPRESSOR_DIR_EXT2LOC;
    cmd.conf.start_byte        = 0;
    cmd.conf.items_to_transfer = nb_items; 

    cmd.lut             = NULL;
    cmd.lut_size        = 0;
    cmd.special_symbol  = 0;

    cmd.transf_type     = PI_CL_DMA_DECOMPRESSOR_TRANSF_EXT_LIN_LOC_LIN;
    cmd.l2_2d.count     = 0;
    cmd.l2_2d.stride    = 0;
    cmd.tcdm_2d.count   = 0;
    cmd.tcdm_2d.stride  = 0;
    cmd.done            = 0;
    cmd.flag.all        = 0;

    pi_cl_dma_decompressor_init();

    /* launch the decompression */
    PRINTF("Enqueueing decompression\n");
    pi_cl_dma_decompressor_cmd(&cmd, cmd.ext, cmd.loc, cmd.conf, cmd.transf_type, cmd.lut_size);

    /* wait for the decompression to end */
    PRINTF("Waiting for decompression\n");
    pi_cl_dma_decompressor_wait(&cmd);

    /* verify decompressed data */
    printf("Verifying decompressed data\n");

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

