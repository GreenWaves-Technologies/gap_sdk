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
#include "ImgIO.h"
#include "Gap.h"
#include "ResizeKernels.h"

#define STACK_SIZE ( 1024 * 2 )
#define CID        ( 0 )

#define IMAGE_W    ( 322 )
#define IMAGE_H    ( 242 )

#if defined(NO_BRIDGE)
#include "golden.h"
#else
uint8_t *ImageIn;
#endif  /* NO_BRIDGE */

uint8_t *ImageOut;

typedef struct ArgCluster
{
    uint32_t Win;
    uint32_t Hin;
    uint32_t Wout;
    uint32_t Hout;
    uint8_t *ImageIn;
    uint8_t *ImageOut;
} ArgCluster_T;

static void cluster_main(ArgCluster_T *ArgC)
{
    printf ("cluster master start\n");

    /* Launching resize on cluster. */
    ResizeImage(ArgC->ImageIn, ArgC->ImageOut);
}

void run_Bilinear_Resize(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;

    /* Input image size. */
    uint32_t w_in = (uint32_t) IMAGE_W, h_in = (uint32_t) IMAGE_H;
    /* Output image size, after resizing. */
    uint32_t w_out = w_in/2, h_out = h_in/2;

    ImageOut = (uint8_t *) pi_l2_malloc(w_out * h_out * sizeof(uint8_t));
    if (ImageOut == NULL)
    {
        printf("Failed to allocate memory for output image(%ld bytes).\n",
               w_out * h_out * sizeof(uint8_t));
        pmsis_exit(-1);
    }
    #if !defined(NO_BRIDGE)
    char *image_name = IN_FILE_PATH;
    uint32_t w_image = 0, h_image = 0;
    ImageIn = (uint8_t *) pi_l2_malloc(w_in * h_in * sizeof(uint8_t));
    if (ImageIn == NULL)
    {
        printf("Failed to allocate memory for input image(%ld bytes).\n",
               w_in * h_in * sizeof(uint8_t));
        pmsis_exit(-2);
    }
    /* Read image from bridge. */
    if ((ReadImageFromFile(image_name, (unsigned int *) &w_image, (unsigned int *) &h_image,
                           ImageIn, w_in * h_in * sizeof(unsigned char)) == 0) ||
        (w_image != w_in) || (h_image != h_in))
    {
        printf("Failed to load image %s or dimension mismatch Expects [%ldx%ld], Got [%ldx%ld]\n",
               image_name, w_in, h_in, w_image, h_image);
        pmsis_exit(-3);
    }
    #endif  /* NO_BRIDGE */

    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = (uint32_t) CID;   /* Cluster ID. */
    /* Configure And open cluster. */
    pi_open_from_conf(&cluster_dev, (void *) &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }

    /* Allocating L1 memory for cluster */
    Resize_L1_Memory = (char *) pi_l1_malloc(&cluster_dev, _Resize_L1_Memory_SIZE);
    if (Resize_L1_Memory == 0)
    {
        printf("Failed to allocate %d bytes for L1_memory\n", _Resize_L1_Memory_SIZE);
        pmsis_exit(-5);
    }

    ArgCluster_T cluster_call;

    //Assinging all input variables to Cluster structure
    cluster_call.ImageIn     = ImageIn;
    cluster_call.Win         = w_in;
    cluster_call.Hin         = h_in;
    cluster_call.Wout        = w_out;
    cluster_call.Hout        = h_out;
    cluster_call.ImageOut    = ImageOut;

    /* Prepare task to be offload to Cluster. */
    struct pi_cluster_task task = {0};
    task.entry = (void *) cluster_main;
    task.arg = &cluster_call;
    task.stack_size = (uint32_t) STACK_SIZE;

    /* Execute the function "cluster_main" on the Core 0 of cluster. */
    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    pi_l1_free(&cluster_dev, Resize_L1_Memory, _Resize_L1_Memory_SIZE);

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    #if defined(NO_BRIDGE)
    /* check output with golden for regressions. */
    for (uint32_t i = 0; i < (w_out * h_out); i++)
    {
        if (ImageOut[i] != ImageOut_golden[i])
        {
            errors++;
        }
    }
    #else
    /* write result image to host. */
    WriteImageToFile(OUT_FILE_PATH, cluster_call.Wout, cluster_call.Hout, ImageOut);
    #endif  /* NO_BRIDGE */

    printf("Test %s with %ld error(s) !\n", (errors) ? "failed" : "success", errors);

    pmsis_exit(errors);
}

int main()
{
    printf("\n\n\t *** Bilinear Resize ***\n\n");
    return pmsis_kickoff((void *) run_Bilinear_Resize);
}
