/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "pmsis.h"
#include "Gap.h"
#include "MatMultKernels.h"

#define STACK_SIZE ( 1024 * 2 )
#define CID        ( 0 )

PI_L2 short int *M1;
PI_L2 short int *M2;
PI_L2 short int *Out1;
PI_L2 short int *Out2;

extern char *L1_Memory;

static void cluster_main()
{
    printf ("cluster master start\n");

    ParMatMult(M1, M2, Out1, 0);

    ParVectMatMult(M1, M2, Out2, 0);
}

void run_MatMult(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;

    printf("Matrix Mult start\n");
    uint32_t num_op = 0;
    uint32_t H_M1 = 200;
    uint32_t W_M1 = 70;
    uint32_t H_M2 = 70;
    uint32_t W_M2 = 150;

    num_op = H_M1 * W_M2 * (W_M1 + H_M2 - 1);
    //num_op = H_M1 * W_M2 * ( 70 + 70 - 1);

    printf("==================================\n");
    printf("Matrix 1: %ld x %ld\n", W_M1, H_M1);
    printf("Matrix 2: %ld x %ld\n", W_M2, H_M2);
    printf("Matrix Multiplication number of operations: %ld\n", num_op);
    printf("==================================\n");
    uint32_t W_Out = H_M1;
    uint32_t H_Out = W_M2;

    M1 = (short int *) pi_l2_malloc(W_M1 * H_M1 * sizeof(short int));
    M2 = (short int *) pi_l2_malloc(W_M2 * H_M2 * sizeof(short int));
    Out1 = (short int *) pi_l2_malloc(W_Out * H_Out * sizeof(short int));
    Out2 = (short int *) pi_l2_malloc(W_Out * H_Out * sizeof(short int));
    if ((M1 == NULL) || (M2 == NULL) || (Out1 == NULL) || (Out2 == NULL))
    {
        printf("Failed to allocate memory for matrix.\n");
        pmsis_exit(-1);
    }
    else
    {
        printf("Allocated matrix :\nM1 : %p, %ld  M2 : %p, %ld\n"
               "Out1 : %p, %ld  Out2 : %p, %ld\n",
               M1, W_M1 * H_M1, M2, W_M2 * H_M2,
               Out1, W_Out * H_Out, Out2, W_Out * H_Out);
    }

    for (uint32_t i = 0; i < (W_M1 * H_M1); i++)
    {
        M1[i] = i;
    }
    for (uint32_t i = 0; i < (W_M2 * H_M2); i++)
    {
        M2[i] = i + 1;
    }
    for (uint32_t i = 0; i < (W_Out * H_Out); i++)
    {
        Out1[i] = 0;
        Out2[i] = 0;
    }

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
        pmsis_exit(-2);
    }

    /* Allocate the predetermined memory size in the shared L1 memory that the cluster can act on. */
    L1_Memory = (char *) pi_l1_malloc(&cluster_dev, _L1_Memory_SIZE);
    if (L1_Memory == NULL )
    {
        printf("Memory Allocation Error! Quit...\n");
        pmsis_exit(-3);
    }

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task task = {0};
    task.entry = cluster_main;
    task.arg = NULL;
    task.stack_size = (uint32_t) STACK_SIZE;

    /* Offloading Task to cluster. */
    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    pi_l1_free(&cluster_dev, L1_Memory, _L1_Memory_SIZE);

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    for (uint32_t i = 0; i < H_Out; i++)
    {
        for (uint32_t j = 0; j < W_Out; j++)
        {
            if (Out1[i * W_Out + j] != Out2[i * W_Out + j])
            {
                printf("Error: Out1[%ld][%ld]=%x != Out2[%ld][%ld]=%x\n",
                       i, j, Out1[i * W_Out + j], i, j, Out2[i * W_Out + j]);
                errors++;
            }
        }
    }

    pi_l2_free(M1, W_M1 * H_M1 * sizeof(short int));
    pi_l2_free(M2, W_M2 * H_M2 * sizeof(short int));
    pi_l2_free(Out1, W_Out * H_Out * sizeof(short int));
    pi_l2_free(Out2, W_Out * H_Out * sizeof(short int));

    if (errors)
    {
        printf("Error, result of different methods does not correspond!\n");
    }

    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

int main(void)
{
    printf("\n\n\t *** MatMult ***\n\n");
    return pmsis_kickoff((void *) run_MatMult);
}
