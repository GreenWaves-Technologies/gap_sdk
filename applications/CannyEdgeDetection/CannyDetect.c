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

/* PMSIS includes. */
#include "pmsis.h"

/* Autotiler includes. */
#include "Gap.h"

/* Gap_lib includes. */
#include "gaplib/ImgIO.h"

#define STACK_SIZE      2048
#define MOUNT           1
#define UNMOUNT         0
#define CID             0

#define EDGE_LOW_THRESHOLD 1
#define EDGE_HIGH_THRESHOLD 10

#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))

#ifdef FROM_FILE
// If you want to use a local image, please specify the imgae size below:
#define ISRGB 0
#define LINE 240
#define COL 320

#if ( (COL%4!=0) || (LINE%4!=0))
    #error LINE and COL of input image must be multiple of four!
#endif


PI_L2 unsigned char *ImageIn_L2;
#else
#include "Mills.h"
#endif
unsigned char PI_L2 *ImageOut_L2;


typedef struct ArgImage {
    unsigned char *In;
    unsigned char *Out;
    unsigned int W;
    unsigned int H;
} ArgImageT;

static struct ArgImage ArgC;


#define	ALLOCATED_MEM	40*1024

typedef unsigned char PixelT;
typedef unsigned char MagnitudeT;
typedef unsigned char OrientationT;

// A big buffer for the cluster usage
static unsigned char  * PI_L1 WorkingArea = NULL;

// Some pointers used for "WorkingArea" buffer.
static unsigned char  * PI_L1 InImage;
static unsigned char  * PI_L1 OutImage;
static MagnitudeT     * PI_L1 GradientBuffer;
static unsigned char  * PI_L1 OrientBuffer;

// Pointer for storing the address of input and output images
static unsigned char  * PI_L1 ImgInL2;
static unsigned char  * PI_L1 ImgOutL2;


int  PI_L1 OrientOffset[10];
int  PI_L1 BlobOffset[8];

unsigned int PI_L2 Performance[4];
unsigned char index_perf = 0;

unsigned int PI_L1 Elapsed[10];

// This global variable controls the number of cores used:
// 1 - execute with 1, 2, 4, 8 cores
// 0 - execute only with 8 cores
static int CoreCountDynamic = 0;
static int ActiveCore;
int finished = 0;

static inline unsigned int ChunkSize(unsigned int X)

{
    unsigned int NCore;
    unsigned int Log2Core;
    unsigned int Chunk;

    if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
    Log2Core = gap_fl1(NCore);
    Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
    return Chunk;
}

void __attribute__ ((noinline)) RGBConvert(struct ArgImage *Arg)

{
#define cR_7 FP2FIXR(0.299, 7)
#define cG_7 FP2FIXR(0.587, 7)
#define cB_7 ((1 << 7) - cR_7 - cG_7)
    unsigned char *__restrict__ In = Arg->In;
    unsigned char *__restrict__ Out = Arg->Out;
    unsigned int Line = Arg->H;
    unsigned int Col = Arg->W;
    v4u rgb;
    v4u Coeff = (v4u) {cR_7, cG_7, cB_7, 0};
    int Scale = 7;
    unsigned int i;
    int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap_coreid();

    Chunk = ChunkSize(Line*Col);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, Line*Col);

    for (i=First; i<Last; i+=2) {
        rgb = *((v4u *) &In[3*i]);
        Out[i] = gap_roundnormu(gap_dotpu4(rgb, Coeff), Scale);
        rgb = *((v4u *) &In[3*(i+1)]);
        Out[i+1] = gap_roundnormu(gap_dotpu4(rgb, Coeff), Scale);
    }
    pi_cl_team_barrier();
#undef cR_7
#undef cG_7
#undef cB_7
}

void MasterRGBConvert(unsigned int W, unsigned int H)

{
    static int Debug = 1;
    static int FullReport = 0;
    pi_cl_dma_cmd_t dmaCpIn, dmaCpOut;
    unsigned int First, Last, IterCount, Toggle, Time;
    unsigned int h, i;

    InImage 	  = WorkingArea;
    First = 0;
    h = ALLOCATED_MEM/(8*W);
    h = Min(h, H);
    OutImage 	  = WorkingArea+6*h*W;
    Last = h;
    IterCount = H/h;

    pi_perf_reset();
    pi_perf_start();
    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    pi_cl_dma_cmd((unsigned int) ImgInL2, (unsigned int) InImage, 3*W*h, PI_CL_DMA_DIR_EXT2LOC, &dmaCpIn);
    ArgC.W = W;
    Toggle = 0;
    pi_cl_dma_cmd_wait(&dmaCpIn);

    for (i=0; i<=IterCount; i++) {
        unsigned int NextFirst = Last, NextLast = Min(Last+h, H);

        // If not the last iteration move to next strip of input image
        if (i!=IterCount) {
            pi_cl_dma_cmd((unsigned int) (ImgInL2 + NextFirst*3*W), (unsigned int) (InImage+((~Toggle)&h)*3*W),
                    3*W*(NextLast-NextFirst), PI_CL_DMA_DIR_EXT2LOC, &dmaCpIn);
        }
        ArgC.In = InImage + (Toggle&h)*3*W; ArgC.Out = OutImage + (Toggle&h)*W; ArgC.H = Last-First;
        pi_cl_team_fork(ActiveCore, (void *) RGBConvert, (void *) &ArgC);

        // If not the first iteration wait for previous copy to L2 to finish
        if (i!=0) pi_cl_dma_cmd_wait(&dmaCpOut);
        // Move produced output to current strip of output image
        pi_cl_dma_cmd((unsigned int) (ImgOutL2 + First*W), (unsigned int) (OutImage+((Toggle)&h)*W),
                W*(Last-First), PI_CL_DMA_DIR_LOC2EXT, &dmaCpOut);

        First = NextFirst; Last = NextLast;
        Toggle = ~Toggle;

        // If not last iteration wait for copy from L2 to finish
        if (i!=IterCount) pi_cl_dma_cmd_wait(&dmaCpIn);
    }
    // Wait for last copy to L2 to finish, if we iterated at least one time
    pi_cl_dma_cmd_wait(&dmaCpOut);
    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;
    pi_perf_stop();
    printf("RGB to BW Total with Master           : %10d Cycles\n", Time);
}

void  __attribute__ ((noinline)) EdgeIntensityAndOrientation_Vectorial(
        unsigned char * __restrict__ In, MagnitudeT * __restrict__ Mag,
        unsigned char * __restrict__ Orient, v4s * __restrict__ Kernels, unsigned int W, unsigned int H)

{
#define Abs(x) ((x<0)?(-x):x)

    unsigned int i, j;
    /* Buffered Data under the kernels */
    v4u V0, V1, V2;
    /* Gy Kernel
       C0 = {-1, -2, -1, 0};
       C1 = { 1,  2,  1, 0};
       Gx Kernel
       C2 = {-1,  0,  1, 0};
       C3 = {-2,  0,  2, 0};
       C4 = {-1,  0,  1, 0};
       */

    v4s C0 = Kernels[0], C1 = Kernels[1], C2 = Kernels[2], C3 = Kernels[3], C4 = Kernels[4];

    v4u *VectIn;
    MagnitudeT *OMag;
    unsigned char  *OOrient;

    int S, Vx, Vy;

    int CoreId;
    unsigned int First, Last, Chunk;

    if (H<=0 || W<=0) return;

    CoreId = gap_coreid();
    Chunk = ChunkSize(W-2);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, (W-2));

    /* Initialize data cache, all in registers */
    VectIn = (v4u *) (In);
    V1 = VectIn[0]; V2 = VectIn[W>>2];

    /* Half kernel size has to be removed left, right, top and bottom for proper and safe operations */
    // The following loop corresponds to for (i=0; i<(W-2); i++)
    for (i=First; i<Last; i++) {
        /* VectIn points, as a vector to the top left corner of the current vertical strip */
        VectIn = (v4u *) (In + 2*W + i);
        /* O points, to the center of the first kernel to be evaluated at the top of current vertical strip */
        OMag = Mag + W + i + 1; OOrient = Orient + W + i + 1;
        /* Process one full vertical strip, width = kernel width */
        for (j=0; j<(H-2); j++) {
            int Vx, Vy, AbsVx, AbsVy;
            MagnitudeT M;
            int O=0;

            V0 = V1; V1 = V2;
            V2 = *VectIn; VectIn += (W>>2);
            S  = gap_dotpus4    (V0, C0);
            S  = gap_sumdotpus4 (V2, C1, S);
            Vy = S;
            S  = gap_dotpus4    (V0, C2);
            S  = gap_sumdotpus4 (V1, C3, S);
            S  = gap_sumdotpus4 (V2, C4, S);
            Vx = S;
            AbsVx = Abs(Vx); AbsVy = Abs(Vy);
            if (sizeof(MagnitudeT)==1) M =  (AbsVx+AbsVy)>>3;
            else M =  (AbsVx+AbsVy);
            if (M >= EDGE_LOW_THRESHOLD) {
                if      ((AbsVy<<1) <= AbsVx) O = 1;
                else if ((AbsVx<<1) <= AbsVy) O = 3;
                else if ((Vx*Vy) > 0) O = 2;
                else O = 4;
            } else M = 0;
            *OMag = M; OMag += W;
            *OOrient = O; OOrient += W;
        }
    }
#undef Abs
}

void  __attribute__ ((noinline)) Conv5x5_Byte_Vectorial(unsigned char * __restrict__ In,
        unsigned char * __restrict__ Out,
        unsigned int W, unsigned int H,
        v4s * __restrict__ Coeff, int NormFactor)

{
    unsigned int i, j;
    v4u V0, V1, V2, V3, V4, V5, V6;
    /* Move all Kernel coefficients into registers */
    v4s C0 = Coeff[0], C1 = Coeff[1], C2 = Coeff[2], C3 = Coeff[3], C4 = Coeff[4], C5 = Coeff[5], C6 = Coeff[6];
    /* Shuffling mask for the delay line: Shuffle((1 2 3 4) (5 6 7 8), Mask)  = (2 3 4 5) */
    v4s Mask  = {1,2,3,4};
    v4u *VectIn;
    unsigned char *O;
    int S;
    int CoreId;
    unsigned int First, Last, Chunk;

    if (H<=4 || W<=4) return;
    /* Assumption: W%4==0 for proper vectorial behavior, if not image should be padded */

    CoreId = gap_coreid();
    Chunk = ChunkSize(W-4);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, W-4);

    for (i=First; i<Last; i++) {
        V5 = (v4u) {0, In[i+4],In[i+W+4], In[i+2*W+4]};
        VectIn = (v4u *) (In + i);
        // Copy the image to vectors, let VectIn point to next row
        V1 = VectIn[0];                 VectIn += (W>>2);
        V2 = VectIn[0];                 VectIn += (W>>2);
        V3 = VectIn[0];                 VectIn += (W>>2);
        V4 = VectIn[0]; V6 = VectIn[1]; VectIn += (W>>2);
        // Set the output position
        O = Out + 2*W + i + 2;
        for (j=0; j<((unsigned)H-4); j++) {
            V0 = V1; V1 = V2; V2 = V3; V3 = V4;
            // Shuffle((1 2 3 4) (5 6 7 8), Mask)  = (2 3 4 5)
            V5 = __builtin_shuffle(V5, V6, Mask);
            V4 = *VectIn; VectIn++; V6 = *VectIn;
            VectIn += ((W>>2)-1);
            /* This zone of image would be like this:
             *    V0 V0 V0 V0 V5
             *    V1 V1 V1 V1 V5
             *    V2 V2 V2 V2 V5
             *    V3 V3 V3 V3 V5
             *    V4 V4 V4 V4 V6 V6 V6 V6
             * Then we do the convolution with the Gaussian Filter.
             * Because the last filter C6 = {2, 0, 0, 0}, the 3 last elements
             * of V6 would not be counted into the result.
             */
            S = gap_dotpus4   (V0, C0);
            S = gap_sumdotpus4(V1, C1, S); S = gap_sumdotpus4(V2, C2, S);
            S = gap_sumdotpus4(V3, C3, S); S = gap_sumdotpus4(V4, C4, S);
            S = gap_sumdotpus4(V5, C5, S); S = gap_sumdotpus4(V6, C6, S);
            *O = (S*NormFactor)>>15;
            O+=W;
        }
    }
}

void __attribute__ ((noinline)) CannyRemoveNonMax(MagnitudeT * __restrict__ In, unsigned char * __restrict__ Orient, unsigned char * __restrict__ Out, const unsigned int W, const unsigned int H, const int * __restrict__ Offset)

{
    unsigned int i, j;
    unsigned int *OutW = (unsigned int *) Out;
    int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap_coreid();
    Chunk = ChunkSize(W-2);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, (W-2));
    for (i=First+1; i<(Last+1); i++) {
        for (j=1; j<(H-1); j++) {
            unsigned int V;
            if (In[W*j+i]) {
                int Off = Offset[Orient[j*W+i]];
                MagnitudeT Val = Max(Max(In[W*j+i], In[W*j+i+Off]), In[W*j+i-Off]);
                int T = (Val == In[W*j+i]);
                V = Min(255, Val*T);
            } else V = 0;
            Out[W*j+i] = V;

        }
    }
}

void __attribute__ ((noinline)) CannyRemoveWeakEdges(unsigned char *In, unsigned int W, unsigned int H)

{
    unsigned int i, j;
    int CoreId;
    unsigned int First, Last, Chunk, VectSize;

    if (H<=0 || W<=0) return;

    CoreId = gap_coreid();
    VectSize = (H*W)>>2;
    Chunk = ChunkSize(VectSize);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, VectSize);

    for (i=First; i<Last; i+=2) {
        v4u V;
        V = (((v4u *)In)[i] == (v4u) {255,255,255,255});
        ((v4u *)In)[i] = V;
        V = (((v4u *)In)[i+1] == (v4u) {255,255,255,255});
        ((v4u *)In)[i+1] = V;
    }
}


void __attribute__ ((noinline)) CannyBlobAnalysis(unsigned char * __restrict__ In, unsigned char * __restrict__ Orient,
        const unsigned int W, const unsigned int H, int * __restrict__ BlobOffset)

{
    unsigned int i, j, k;
    int CoreId;
    unsigned int First, Last, Chunk;

    CoreId = gap_coreid();
    Chunk = ChunkSize(W-2);
    First =  CoreId*Chunk; Last = First+Chunk; Last = Min(Last, (W-2));

    for (i=First+1; i<(Last+1); i++) {
        for (j=1; j<H-1; j++) {
            unsigned int V = In[W*j+i];
            if (V >= EDGE_HIGH_THRESHOLD) {
                int Off = BlobOffset[Orient[W*j+i]];
                V = 255;
                if (In[ Off] >= EDGE_LOW_THRESHOLD && In[ Off] < EDGE_HIGH_THRESHOLD) In[ Off] = 255;
                if (In[-Off] >= EDGE_LOW_THRESHOLD && In[-Off] < EDGE_HIGH_THRESHOLD) In[-Off] = 255;
            }
            In[W*j+i] = V;
        }
    }
}

void __attribute__ ((noinline)) CannyDetector(struct ArgImage *Arg)

{
    static int Debug = 0;
    unsigned char * __restrict__ In = Arg->In;
    unsigned char * __restrict__ ImageOut = Arg->Out;
    unsigned int W = Arg->W;
    unsigned int H = Arg->H;
    MagnitudeT *Gradient = GradientBuffer;
    unsigned char  *Orient   = OrientBuffer;
    unsigned int Time;

    if (Debug) printf("Starting worker Canny workers on core %d\n", gap_coreid());

    /* Gaussian fiter with Sigma = approx 1.4 */
    v4s KerGaussOpt[] =
    {
        {2,  4,  5,  4},
        {4,  9, 12,  9},
        {5, 12, 15, 12},
        {4,  9, 12,  9},
        {2,  4,  5,  4},
        {2,  4,  5,  4},
        {2,  0,  0,  0}
    }; // Sum = 159
    int GaussNormFactor = (32767/159);

    v4s Kernels_Sobel[] =
    {
        /* Gy Kernel */
        {-1, -2, -1, 0},
        { 1,  2,  1, 0},
        /* Gx Kernel */
        {-1,  0,  1, 0},
        {-2,  0,  2, 0},
        {-1,  0,  1, 0},
    };

    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    // Filter the slice of image with a Gaussian 5*5 filter.
    // result = (Filter * Image) / 159
    Conv5x5_Byte_Vectorial((unsigned char *) In, (unsigned char *) ImageOut, W, H, (v4s *) KerGaussOpt, GaussNormFactor);
    pi_cl_team_barrier();
    Elapsed[0] += pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    // Find the intensity gradient of the image
    // Here we use a Sobel operator for finding the Gx and Gy
    EdgeIntensityAndOrientation_Vectorial((unsigned char *) ImageOut, (MagnitudeT *) Gradient,
            (unsigned char *) Orient, (v4s * __restrict__) Kernels_Sobel, W, H);
    pi_cl_team_barrier();
    Elapsed[1] += pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    // Apply non-maximum suppression to get rid of spurious response to edge detection
    CannyRemoveNonMax((MagnitudeT *) Gradient, (unsigned char *) Orient, (unsigned char *) ImageOut, W, H, OrientOffset);
    pi_cl_team_barrier();
    Elapsed[2] += pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    // Apply Blob analysis
    CannyBlobAnalysis((unsigned char *) ImageOut, (unsigned char *) Orient, W, H, OrientOffset+5);
    pi_cl_team_barrier();
    Elapsed[3] += pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    // Finalize the detection of edges by suppressing all the other edges that are weak and not connected to strong edges.
    CannyRemoveWeakEdges((unsigned char *) ImageOut, W, H);
    pi_cl_team_barrier();
    Elapsed[4] += pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

}

void InitBlobOffset(int W, int BlobOffset[])

{
    BlobOffset[0] = 1;
    BlobOffset[1] = -1;
    BlobOffset[2] = W-1;
    BlobOffset[3] = W;
    BlobOffset[4] = W+1;
    BlobOffset[5] = -W-1;
    BlobOffset[6] = -W;
    BlobOffset[7] = -W+1;
}

void InitNonMaxOffset(int Offset[], int W)

{
    Offset[0] = Offset[5]  = 0;

    Offset[1] = Offset[8]  = 1;
    Offset[2] = Offset[9]  = W+1;
    Offset[3] = Offset[6]  = W;
    Offset[4] = Offset[7]  = W-1;
}

static void InitCannyMaster(unsigned int W)

{
    unsigned int i;

    InitNonMaxOffset(OrientOffset, W); InitBlobOffset(W, BlobOffset);
    for (i=0; i<(sizeof(Elapsed)/sizeof(unsigned int)); i++) Elapsed[i] = 0;
}

void MasterCannyDetector(unsigned int W, unsigned int H)

{
    static int Debug = 1;
    static int FullReport = 1;
    pi_cl_dma_cmd_t dmaCpIn, dmaCpOut;
    unsigned int First, Last, IterCount, Toggle, Time;
    unsigned int h, i;
    /*
       Tile overlapping: 4

Filtering:	5x5 Convolution ->	2
Gradient:	Sobel		    ->	1
NonMax:				        ->	1
*/
    unsigned int K = 4;

    //InitCannyMaster(W);

    /*
       Maximum height of the tile we can process allocating memory from WorkingArea for:
       in and out as double buffer
       gradient and gradient orientation as simple buffers
    */
    h = ALLOCATED_MEM/((2*sizeof(PixelT) + 2*sizeof(PixelT) + sizeof(MagnitudeT) + sizeof(OrientationT))*W);
    h = Min(h, H);

    InImage        = WorkingArea;
    OutImage       = WorkingArea+((2*sizeof(PixelT))*h*W);
    GradientBuffer = (MagnitudeT *) (WorkingArea+((2*sizeof(PixelT) + 2*sizeof(PixelT))*h*W));
    OrientBuffer   = (WorkingArea+ ((2*sizeof(PixelT) + 2*sizeof(PixelT) + sizeof(MagnitudeT)) *h*W));


    First = 0; Last = h;

    if (FullReport) {
        IterCount = H/(h-2*K)-1;
        printf("Entering Master, W=%d, H=%d, %d iterations, read stripes: %d lines, wrote stripe: %d lines\n", W, H, IterCount, h, h-2*K);
    }

    pi_perf_reset();
    pi_perf_start();
    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES);
    /* Feed in fresh in data from L2, we have to wait for completion of the copy before we can start processing */
    pi_cl_dma_cmd((unsigned int) ImgInL2, (unsigned int) InImage, W*h, PI_CL_DMA_DIR_EXT2LOC, &dmaCpIn);
    ArgC.W = W;
    Toggle = 0;
    IterCount = H/(h-2*K)-1;
    pi_cl_dma_cmd_wait(&dmaCpIn);

    for (i=0; i<=IterCount; i++) {
        unsigned int NextFirst = Last - 2*K, NextLast = Min(Last+(h-2*K), H);

        /* If not last iteration move in next strip of in image, Toggle control in which part of the double buffer we are */
        if (i!=IterCount) {
            pi_cl_dma_cmd((unsigned int) (ImgInL2 + NextFirst*W),
                    (unsigned int) (InImage+((~Toggle)&h)*W),
                    W*(NextLast-NextFirst), PI_CL_DMA_DIR_EXT2LOC, &dmaCpIn);
        }

        ArgC.In = InImage + (Toggle&h)*W; ArgC.Out = OutImage + (Toggle&h)*W; ArgC.H = Last-First;
        /* Enqueue CannyDetector in the HW dispatcher, core 1 to 7 are idle on wait for dispatch */
        pi_cl_team_fork(ActiveCore, (void *) CannyDetector, (void *) &ArgC);

        /* If not first iteration */
        if (i!=0) pi_cl_dma_cmd_wait(&dmaCpOut);
        /* Move produced output to current strip of out image, Toogle controls which part of the double buffer */
        pi_cl_dma_cmd((unsigned int) (ImgOutL2 + (First+K)*W),
                (unsigned int) (OutImage+(((Toggle)&h)+K)*W),
                W*(Last-First-2*K), PI_CL_DMA_DIR_LOC2EXT, &dmaCpOut);

        First = NextFirst; Last = NextLast;
        Toggle = ~Toggle;

        /* If not last iteration wait for copy in, initiated at the beginning of this iter, of the other part of input double buffer */
        if (i!=IterCount) pi_cl_dma_cmd_wait(&dmaCpIn);
    }
    /* Wait for last copy out */
    pi_cl_dma_cmd_wait(&dmaCpOut);
    pi_perf_stop();
    Time = pi_perf_read(PI_PERF_ACTIVE_CYCLES)-Time;

    if (FullReport) {
        printf("Conv5x5_Byte_Vectorial                : %10d Cycles\n", Elapsed[0]);
        printf("EdgeIntensityAndOrientation_Vectorial : %10d Cycles\n", Elapsed[1]);
        printf("CannyRemoveNonMax                     : %10d Cycles\n", Elapsed[2]);
        printf("CannyBlobAnalysis                     : %10d Cycles\n", Elapsed[3]);
        printf("CannyRemoveWeakEdges                  : %10d Cycles\n", Elapsed[4]);
        printf("Total                                 : %10d Cycles\n", Elapsed[0]+ Elapsed[1]+ Elapsed[2]+ Elapsed[3]+ Elapsed[4]);
    }
    printf("Total with Master                     : %10d Cycles\n", Time);
    Performance[index_perf++] = Time;

}

static void cluster_main()
{
    printf ("cluster master start\n");
    int W=COL, H=LINE;
    
    // Configure performance counters for counting the cycles
    pi_perf_conf(1 << PI_PERF_ACTIVE_CYCLES);
    
    if (CoreCountDynamic) {
        /* Here the canny edge would be executed on 1, 2, 4 and 8 cores*/
        int i;
        for (i=0; i<4; i++) {
            // In these 4 loops, we will active 1, 2, 4 and 8 cores for the next processing steps.
            ActiveCore = (1<<i);
            printf("Canny Edge Detector running on %d cores, Source %s image[W=%d, H=%d]\n", ActiveCore, ISRGB?"RGB":"Mono", W, H);
            // Because the algorithm runs with a mono color image (gray scale), if the image format is RGB, we need to convert it to gray scale.
            if (ISRGB) {
                MasterRGBConvert(W, H);
            }
            ImgOutL2 = ImageOut_L2; ImgInL2 = ImageIn_L2;
            // Start Canny Edge Detector on core 0, that will dispatch the algorithm to the other cores.
            MasterCannyDetector(W, H);
        }
    } else {
        /* Here the canny edge would be executed only on all the cores of cluster*/
        printf("Canny Edge Detector running on %d cores, Source %s image[W=%d, H=%d]\n", gap_ncore(), ISRGB?"RGB":"Mono", W, H);
        ActiveCore = gap_ncore();
        if (ISRGB) {
            MasterRGBConvert(W, H);
        }
        ImgOutL2 = ImageOut_L2; ImgInL2 = ImageIn_L2;
        // Start Canny Edge Detector on core 0, that will dispatch the algorithm to the other cores.
        MasterCannyDetector(W, H);
    }
    // Stop the clock for performance usage.
    pi_perf_stop();
}

void canny_edge_detector()
{
    printf ("Start of application\n");

#if FROM_FILE

	char *Imagefile = "Pedestrian.pgm";
	char imageName[64];
	sprintf(imageName, "../../../%s", Imagefile);
	ImageIn_L2 = (unsigned char *) pi_l2_malloc( COL*LINE*sizeof(unsigned char));

    if (ReadImageFromFile(imageName, COL,LINE, 1, ImageIn_L2, LINE*COL*sizeof(unsigned char), 0, 0))
    {
        printf("Failed to load image %s\n", imageName);
        pmsis_exit(-1);
    }

#endif

    // Activate the Cluster
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-1);
    }

    // Allocate a big buffer in L1 for the algorithm usage.
    WorkingArea 	  = pi_l1_malloc(0, ALLOCATED_MEM) ;
    if(WorkingArea == NULL) {
        printf("WorkingArea alloc error\n");
        pmsis_exit(-1);
    }

    // Allocate the buffer for the output image.
    ImageOut_L2 = pi_l2_malloc((LINE*COL));

    printf ("Call cluster\n");
    struct pi_cluster_task *task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
    memset(task, 0, sizeof(struct pi_cluster_task));
    task->entry = cluster_main;
    task->arg = (void *) NULL;
    task->stack_size = (uint32_t) STACK_SIZE;

    //Synchronous call to Cluster, Execution will start only on PE0
    pi_cluster_send_task_to_cl(&cluster_dev, task);

    char imgName[50];
    sprintf(imgName, "../../../img_OUT.ppm");
    printf("imgName: %s\n", imgName);
    WriteImageToFile(imgName, COL, LINE, 1, (ImageOut_L2), sizeof(unsigned char));

    pi_cluster_close(&cluster_dev);
    
    pmsis_exit(0);
 
 }


int main(int argc, char *argv[])
{
    printf("\n\n\t *** Canny Edge Detector ***\n\n");
    return pmsis_kickoff((void *) canny_edge_detector);
}



