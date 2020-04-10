/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

/* PMSIS includes. */
#include "pmsis.h"

/* Autotiler includes. */
#include "Gap.h"
#include "MnistKernels.h"
#include "MnistCoeffs.h"

#if defined(ENABLE_BRIDGE)
#include "ImgIO.h"
#else
#include "golden.h"
#endif  /* USE_BRIDGE */

/* Variables used. */
#define IMG_DIR test_img
#define NUM_DIR 6
#define NUM_PIC 1578

#define __IMG_NAME(x)    #x
#define _IMG_NAME(x,y,z) __IMG_NAME(../../../x/y/z.pgm)
#define IMG_NAME(x,y,z)  _IMG_NAME(x,y,z)
#define NAME             IMG_NAME(IMG_DIR, NUM_DIR, NUM_PIC)

#define STACK_SIZE      (2048)

int16_t *Filter_Layer[3] = {0};
int16_t *Bias_Layer[3] = {0};
int16_t *Out_Layer[3];
uint32_t Out_Layer_Size[3] = {0};
uint16_t *image_in = NULL;
uint8_t *image_in_real = NULL;
uint8_t rec_digit = 0x2A;

int ConvAt(short *In, short int *Filter, unsigned int X, unsigned int Y, unsigned int W, unsigned int H, unsigned int Norm)
{
    unsigned int i, j;
    int Acc = 0;
    unsigned int K = 5;

    for (i=0; i<K; i++) {
        for (j=0; j<K; j++) {
            Acc += In[(X+i)*W+Y+j]*Filter[K*i+j];
        }
    }
    return (gap_clip(gap_roundnorm_reg(Acc, Norm), 15));
}


void DumpPlane(char *Mess, short int *Plane, unsigned int W, unsigned int H)
{
    unsigned int i, j;

    printf("----------------- %s ------------------------\n", Mess);
    for (i=0; i<H; i++) {
        printf("%2d: ", i);
        for (j=0; j<W; j++) {
            printf("%4x ", (unsigned short) Plane[i*W+j]);
        }
        printf("\n");
    }
    printf("-----------------------------------------\n");
}

void DumpPaddedCoeff(char *Name, short int *C, unsigned int NTap, unsigned int NFilter)
{
    unsigned int i, j;
    printf("L2_MEM short int %s[] = {\n", Name);
    for (i=0; i<NFilter; i++) {
        for (j=0; j<NTap; j++) {
            printf("%d, ", C[i*NTap+j]);
        }
        printf("0,\n");
    }
    printf("};\n");
}

int CheckSum(short int *In, int Size)
{
    int i;
    int S=0;

    for (i=0; i<Size; i++) S += In[i];
    return S;
}

void Check(char *Mess, short int *Planes, int NPlane, int W, int H)
{
    int i;
    printf("Check sum for %s\n", Mess);

    for (i=0; i<NPlane; i++) {
        printf("\t%2d: %d\n", i, CheckSum(Planes + i*(W*H), W*H));
    }
}

static void RunMnist(void *arg)
{
    Conv5x5ReLUMaxPool2x2_0((int16_t *) image_in,
                            Filter_Layer[0],
                            Bias_Layer[0],
                            Out_Layer[0]);

    Conv5x5ReLUMaxPool2x2_1(Out_Layer[0],
                            Filter_Layer[1],
                            Bias_Layer[1],
                            Out_Layer[1]);

    LinearLayerReLU_1(Out_Layer[1],
                      Filter_Layer[2],
                      Bias_Layer[2],
                      Out_Layer[2]);

    uint8_t * digit = (uint8_t *) arg;
    int16_t highest = Out_Layer[2][0];
    *digit = 0;
    for (uint8_t i = 1; i < 10; i++)
    {
        if (highest < Out_Layer[2][i])
        {
            highest = Out_Layer[2][i];
            *digit = i;
        }
    }
}

void test_mnist(void)
{
    printf("Entering main controller\n");

    uint8_t CheckResults = 0;
    uint32_t errors = 0;
    char *image_name = NAME;

    /* Bias & Filters. */
    Bias_Layer[0] = Bias_Layer0;
    Bias_Layer[1] = Bias_Layer1;
    Bias_Layer[2] = Bias_Layer2;
    Filter_Layer[0] = Filter_Layer0;
    Filter_Layer[1] = Filter_Layer1;
    Filter_Layer[2] = Filter_Layer2;

    /* Output result size. */
    Out_Layer_Size[0] = (24 * 24 * sizeof(int16_t) * 32);
    Out_Layer_Size[1] = (4 * 4 * sizeof(int16_t) * 64);
    Out_Layer_Size[2] = (1 * 1 * sizeof(int16_t) * 10);

    uint32_t Wi, Hi;
    //Input image size
    uint32_t img_w = 28, img_h = 28;
    uint32_t size_img_in = 0, size_img_in_real = 0;
    size_img_in = img_w * img_h * sizeof(uint16_t);
    size_img_in_real = img_w * img_h * sizeof(uint8_t);

    //Allocating input and output image buffers in L2 memory
    image_in  = (uint16_t *) pmsis_l2_malloc(size_img_in);
    if (image_in == NULL)
    {
        printf("Failed to allocate memory for image (%d bytes)\n", size_img_in);
        pmsis_exit(-1);
    }

    #if defined(ENABLE_BRIDGE)
    image_in_real = (uint8_t *) pmsis_l2_malloc(size_img_in_real);
    if (image_in_real == NULL)
    {
        printf("Failed to allocate memory for image (%d bytes)\n", size_img_in);
        pmsis_exit(-2);
    }

    //Reading Image from host
    uint8_t *read_status = ReadImageFromFile(image_name, &Wi, &Hi, image_in_real, size_img_in_real);
    if ((read_status == 0) || (Wi != img_w) || ( Hi!= img_h))
    {
        printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n",
               image_name, img_w, img_h, Wi, Hi);
        pmsis_exit(-3);
    }

    #else
    image_in_real = ImageIn;
    #endif  /* ENABLE_BRIDGE */

    //Convert in Mnist dataset format
    for (uint32_t i = 0; i < (img_w * img_h); i++)
    {
        image_in[i] = image_in_real[i] << 4; //Q8+Q4 = 12
    }

    //TODO Move this to Cluster
    for (uint32_t i = 0; i < 3; i++)
    {
        Out_Layer[i] = (int16_t *) pmsis_l2_malloc(Out_Layer_Size[i]);
        if (Out_Layer[i] == NULL)
        {
            printf("Failed to allocate memory for Out_layer_%d (%d Bytes).\n",
                   i, Out_Layer_Size[i]);
            pmsis_exit(-4 - i);
        }
        else
        {
            printf("Allocating %d: OK -> %p\n", Out_Layer_Size[i], Out_Layer[i]);
        }
    }

    /* Configure And open cluster. */
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-7);
    }

    Mnist_L1_Memory = pmsis_l1_malloc(_Mnist_L1_Memory_SIZE);
    if (Mnist_L1_Memory == NULL)
    {
        printf("Mnist_L1_Memory alloc failed\n");
        pmsis_exit(-8);
    }

    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = RunMnist;
    task->arg = (void *) &rec_digit;
    task->stack_size = (uint32_t) STACK_SIZE;

    pi_cluster_send_task_to_cl(&cluster_dev, task);

    pmsis_l1_malloc_free(Mnist_L1_Memory, _Mnist_L1_Memory_SIZE);

    pi_cluster_close(&cluster_dev);

    if (CheckResults)
    {
        Check("SW   Layer0", Out_Layer[0], 32, 24, 24);
        Check("SW   Layer1", Out_Layer[1], 64, 4, 4);
        Check("SW   Layer2", Out_Layer[2], 10, 1, 1);
    }

    printf("Recognized number : %d\n", rec_digit);
    
    #if defined(ENABLE_BRIDGE)
    errors = (rec_digit != (uint8_t) NUM_DIR);
    #else
    errors = (rec_digit != (uint8_t) GoldenOutput);
    #endif /* defined(ENABLE_BRIDGE) */

    printf("\nTest %s with %d error(s) !\n", (errors) ? "failed" : "success", errors);

    if(errors) pmsis_exit(-9);
    else pmsis_exit(0);
}

int main()
{
    printf("\n\n\t *** PMSIS Mnist Test ***\n\n");
    return pmsis_kickoff((void *) test_mnist);
}
