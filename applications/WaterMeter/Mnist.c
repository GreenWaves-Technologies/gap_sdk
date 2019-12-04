/*
 * Copyright 2019 GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "Gap8.h"
#include "MnistKernels.h"
#include "Mnist_trained_coef.def"
#include "ImgIO.h"

#define MERGED_DMA_EVENT 1
#define SINGLE_DMA_EVENT 0
#define NORM_IN 4

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#define IMG_W 324
#define IMG_H 244

#define COUNTER_DIGITS 8

//Points in the image
RT_L2_DATA unsigned int pointsY[]= { 113 };
RT_L2_DATA unsigned int pointsX[]= { 52,80,105,128,155,181,202,230 };

L2_MEM signed short int *Out_Layer0;
L2_MEM signed short int *Out_Layer1;
L2_MEM signed short int *Out_Layer2;
L2_MEM rt_perf_t *cluster_perf;

RT_L2_DATA signed short *ImageIn;
RT_L2_DATA signed short *ImageIn_16;
RT_L2_DATA unsigned char  *ImageIn_real;
RT_L2_DATA unsigned char  *ImageOut;


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
    return (gap8_clip(gap8_roundnorm_reg(Acc, Norm), 15));
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

RT_L2_DATA int result[COUNTER_DIGITS];
RT_L2_DATA int confidence[COUNTER_DIGITS];

static void RunMnist()

{

    rt_dma_copy_t DmaR_Evt1;
    unsigned int ElapsedTime[1];
    //int result[6];
    int CheckResults = 1;
    rt_perf_t *perf = cluster_perf;

    // initialize the performance clock
    rt_perf_init(perf);
    // Configure performance counters for counting the cycles
    rt_perf_conf(perf, (1<<RT_PERF_CYCLES));

    gap8_resethwtimer();

    rt_perf_reset(perf);
    rt_perf_start(perf);

    //Here add a stage to get the starting point of the input


    for(int digit=0;digit<COUNTER_DIGITS;digit++){

        int x=0;
        for(unsigned int j=pointsY[0]; j<pointsY[0]+28; j++)
            for(unsigned int i=pointsX[digit]; i<pointsX[digit]+28; i++)
                ImageIn[x++] = (ImageIn_16[j*IMG_W+i]) << NORM_IN;

        Conv5x5ReLUMaxPool2x2_0((short int*)ImageIn   , L2_W_0, L2_B_0, Out_Layer0, 12, 0);
        //DumpPaddedCoeff("OUTPUT Layer 1",Out_Layer0,12*12,32);
        Conv5x5ReLUMaxPool2x2_1(            Out_Layer0, L2_W_1, L2_B_1, Out_Layer1, 12, 0);
        //DumpPaddedCoeff("OUTPUT Layer 1",Out_Layer1,4*4,32);
        LinearLayerReLU_2      (            Out_Layer1, L2_W_2, L2_B_2, Out_Layer2, 14, 10, 0);
        //DumpPaddedCoeff("OUTPUT Layer 1",Out_Layer2,1,10);

        //Get Best Result
        int rec_digit=0;
        int highest=Out_Layer2[0];
        for(int i=1;i<10;i++){
            if(highest<Out_Layer2[i]){
                highest=Out_Layer2[i];
                rec_digit=i;
            }
        }
        confidence[digit] = highest;
        result    [digit] = rec_digit;
    }

    rt_perf_stop(perf);
    rt_perf_save(perf);
    ElapsedTime[0] = rt_perf_get(perf, RT_PERF_CYCLES);

    printf("Total Elapse Cycles: %7d Cycles\n", ElapsedTime[0]);
    printf("Recognized Number: ");

    for(int digit=0;digit<COUNTER_DIGITS;digit++){
        printf("%d ", result[digit]);
    }
    printf("\n");


}


RT_L2_DATA char *ImageList[]={
    "../../../dataset/img_OUT1.ppm",
    "../../../dataset/img_OUT2.ppm",
    "../../../dataset/img_OUT3.ppm",
    "../../../dataset/img_OUT4.ppm",
    "../../../dataset/img_OUT5.ppm",
    "../../../dataset/img_OUT6.ppm",
    "../../../dataset/img_OUT7.ppm",
    "../../../dataset/img_OUT8.ppm",
    "../../../dataset/img_OUT9.ppm",
    "../../../dataset/img_OUT10.ppm",
    "../../../dataset/img_OUT11.ppm",
    "../../../dataset/img_OUT12.ppm",
    "../../../dataset/img_OUT13.ppm",
    "../../../dataset/img_OUT14.ppm",
    "../../../dataset/img_OUT15.ppm",
    "../../../dataset/img_OUT16.ppm",
    "../../../dataset/img_OUT17.ppm",
    "../../../dataset/img_OUT18.ppm",
    "../../../dataset/img_OUT19.ppm",
    "../../../dataset/img_OUT20.ppm",
    "../../../dataset/img_OUT21.ppm",
    "../../../dataset/img_OUT22.ppm",
    "../../../dataset/img_OUT23.ppm",
    "../../../dataset/img_OUT24.ppm",
    "../../../dataset/img_OUT25.ppm",
    "../../../dataset/img_OUT26.ppm",
    "../../../dataset/img_OUT27.ppm",
    "../../../dataset/img_OUT28.ppm",
    "../../../dataset/img_OUT29.ppm",
    "../../../dataset/img_OUT30.ppm",
    "../../../dataset/img_OUT31.ppm",
    "../../../dataset/img_OUT32.ppm",
    "../../../dataset/img_OUT33.ppm",
    "../../../dataset/img_OUT34.ppm",
};


RT_L2_DATA  int GT[][8]={

    {0,0,0,0,3,3,8,2},
    {0,0,0,0,3,3,9,1},
    {0,0,0,0,3,3,9,7},
    {0,0,0,0,3,4,0,5},
    {0,0,0,0,3,4,1,2},
    {0,0,0,0,3,4,1,9},
    {0,0,0,0,3,4,2,6},
    {0,0,0,0,3,4,3,3},
    {0,0,0,0,3,4,4,0},
    {0,0,0,0,3,4,4,7},
    {0,0,0,0,3,4,5,5},
    {0,0,0,0,3,4,6,2},
    {0,0,0,0,3,4,6,9},
    {0,0,0,0,3,4,7,6},
    {0,0,0,0,3,4,8,3},
    {0,0,0,0,3,4,9,0},
    {0,0,0,0,3,4,9,8},
    {0,0,0,0,3,5,0,5},
    {0,0,0,0,3,5,1,3},
    {0,0,0,0,3,5,2,1},
    {0,0,0,0,3,5,2,9},
    {0,0,0,0,3,5,3,6},
    {0,0,0,0,3,5,4,3},
    {0,0,0,0,3,5,5,0},
    {0,0,0,0,3,5,5,8},
    {0,0,0,0,3,5,6,5},
    {0,0,0,0,3,5,7,2},
    {0,0,0,0,3,5,7,9},
    {0,0,0,0,3,5,8,6},
    {0,0,0,0,3,5,9,4},
    {0,0,0,0,3,6,0,0},
    {0,0,0,0,3,6,0,7},
    {0,0,0,0,3,6,1,4},
    {0,0,0,0,3,6,2,1},
    {0,0,0,0,3,6,2,8},
    {0,0,0,0,3,6,3,5},
    {0,0,0,0,3,6,4,2},

};

int main()

{
    char * ImageName;
    unsigned int Wi, Hi;

    //Input image size
    unsigned int Win = 324, Hin = 244;
    unsigned int W   = 28,  H   = 28;

    rt_freq_set(RT_FREQ_DOMAIN_FC, 250000000);

    printf("Entering main controller\n");

    if (rt_event_alloc(NULL, 8)) return -1;

    //Allocating input and output image buffers in L2 memory
    ImageIn_real  = (unsigned char *)  rt_alloc( RT_ALLOC_L2_CL_DATA, Win*Hin*sizeof(unsigned char));
    ImageIn_16    = (short *)          rt_alloc( RT_ALLOC_L2_CL_DATA, Win*Win*sizeof(unsigned short ));
    ImageIn       = (short *)          rt_alloc( RT_ALLOC_L2_CL_DATA, W*H*sizeof(unsigned short));
    ImageOut      = (unsigned char *)  rt_alloc( RT_ALLOC_L2_CL_DATA, W*H*sizeof(unsigned char));

    if (ImageIn_real==0) {
        printf("Failed to allocate Memory for Image (%d bytes)\n", W*H*sizeof(unsigned char));
        return 1;
    }
    if (ImageIn_16==0) {
        printf("Failed to allocate Memory for Image (%d bytes)\n", W*H*sizeof(unsigned short));
        return 1;
    }


    //TODO Move this to Cluster
    Out_Layer0 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 12*12*sizeof(short int)*128);
    //printf("Allocating %d: %s\n", 24*24*sizeof(short int)*32, Out_Layer0?"Ok":"Failed");
    Out_Layer1 = (short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 4*4*sizeof(short int)*64);
    //printf("Allocating %d: %s\n", 4*4*sizeof(short int)*64, Out_Layer1?"Ok":"Failed");
    Out_Layer2 = ( short int *) rt_alloc(RT_ALLOC_L2_CL_DATA, 10*sizeof( short int));
    //printf("Allocating %d: %s\n", 10*sizeof(short int), Out_Layer2?"Ok":"Failed");

    if (!(Out_Layer0 && Out_Layer1 && Out_Layer2)) {
        printf("Failed to allocated memory, giving up.\n");
        return 0;
    }

    rt_cluster_mount(MOUNT, CID, 0, NULL);
    rt_freq_set(RT_FREQ_DOMAIN_CL, 175000000);

    // Allocate the memory of L2 for the performance structure
    cluster_perf = rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(rt_perf_t));
    if (cluster_perf == NULL) return -1;

    // Allocate some stacks for cluster in L1, rt_nb_pe returns how many cores exist.
    void *stacks = rt_alloc(RT_ALLOC_CL_DATA, STACK_SIZE*rt_nb_pe());
    if (stacks == NULL) return -1;

    Mnist_L1_Memory = rt_alloc(RT_ALLOC_CL_DATA, _Mnist_L1_Memory_SIZE);
    if(Mnist_L1_Memory == NULL) {
        printf("Mnist_L1_Memory alloc failed\n");
        return -1;
    }
    int correct=0;
    int wrong=0;
    int skipped=0;
    for (unsigned int k=0; k<(sizeof(ImageList)/sizeof(char *)) && (k == -1 || k < NB_FRAMES); k++){
        ImageName = ImageList[k];

        //Reading Image from Hyperflash
        if ((ReadImageFromFile(ImageName, &Wi, &Hi, ImageIn_real, Win*Hin*sizeof(unsigned char))==0) || (Wi!=Win) || (Hi!=Hin)) {
            printf("Failed to load image %s or dimension mismatch Expects [%dx%d], Got [%dx%d]\n", ImageName, Win, Hin, Wi, Hi);
            return 1;
        }

        //Convert Image to 16 bit
        for(unsigned int i=0;i<Win*Hin;i++){
            ImageIn_16[i] = ImageIn_real[i];
        }

        // Execute the function "RunCifar10" on the cluster.
        rt_cluster_call(NULL, CID, (void *) RunMnist, NULL, stacks, STACK_SIZE, STACK_SIZE, rt_nb_pe(), NULL);

        int * GT_in = GT[k];
        for(int num=0;num<7;num++){
            if(GT_in[num]==-1){
                skipped++;
                continue;
            }
            if(GT_in[num]==result[num])
                correct++;
            else
                wrong++;
        }

    }

    printf("Total: %d\n"  ,skipped+correct+wrong);
    printf("Skiped: %d\n" ,skipped);
    printf("Correct: %d\n",correct);
    printf("Wrong: %d\n"  ,wrong);

    // Close the cluster
    rt_cluster_mount(UNMOUNT, CID, 0, NULL);


    /*	int digit = 5;
        int x=0;
        for(unsigned int j=pointsY[0]; j<pointsY[0]+28; j++){
        for(unsigned int i=pointsX[digit]; i<pointsX[digit]+28; i++){
        ImageOut[x++] = ImageIn_real[j*IMG_W+i];
        }
        }

        WriteImageToFile("../../../output.pgm",W,H,ImageOut);

*/

    /*for(int i=0;i<Win*Hin;i++){
      ImageOut[i] = ImageIn[i] >> NORM_IN ;
      }

      WriteImageToFile("../../../output.pgm",W,H,ImageOut);*/

    //Add Buffer Free
    printf("Test success\n");
    return 0;
}

