/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* Matrix add example main file. This code actually runs on GAP8 */

#include <stdio.h>
#include "pmsis.h"
#include "Gap.h"
#include "MatAddKernels.h"

#define STACK_SIZE ( 1024 )
#define CID        ( 0 )

#define MAT_W      ( 100 )
#define MAT_H      ( 100 )
#define MAT_SIZE   ( MAT_W * MAT_H )

extern char *L1_Memory;

PI_L2 int Mat1[MAT_SIZE];
PI_L2 int Mat2[MAT_SIZE];
PI_L2 int MatOut[MAT_SIZE];

static void cluster_main()
{
    printf ("cluster master start\n");

    MatADD(Mat1, Mat2, MatOut);
}

static void init_test()
{
    for(int i = 0; i < MAT_SIZE; i++)
    {
        Mat1[i] = 1;
        Mat2[i] = 2;
        MatOut[i] = 0;
    }
}

void run_MatAdd(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;

    /* Load matrices to add. */
    init_test();

    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = (uint32_t) CID;   /* Cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, (void *) &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    /* Allocate L1 memory used by AutoTiler in L1. */
    L1_Memory = (char *) pi_l1_malloc(&cluster_dev, _L1_Memory_SIZE);
    if (L1_Memory == 0 )
    {
        printf("Memory Allocation Error! Quit...");
        pmsis_exit(-2);
    }
    printf("Allocated: %p, for %d bytes\n", L1_Memory, _L1_Memory_SIZE);

    /* Prepare task to be offload to Cluster. */
    struct pi_cluster_task task = {0};
    task.entry = cluster_main;
    task.arg = NULL;
    task.stack_size = (uint32_t) STACK_SIZE;

    /* Offloading Task to cluster. */
    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    pi_l1_free(&cluster_dev, L1_Memory, _L1_Memory_SIZE);

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    /* Checking result. */
    for (int i = 0; i < MAT_H; i++)
    {
        for (int j = 0; j < MAT_W; j++)
        {
            if (MatOut[(i * MAT_W) + j] != 3)
            {
                errors++;
                printf("Error: MatOut[%d][%d]=%d != 3\n", i, j, MatOut[(i * MAT_W) + j]);
            }
        }
    }

    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}


int main(void)
{
    printf("\n\n\t *** MatAdd ***\n\n");
    return pmsis_kickoff((void *) run_MatAdd);
}
