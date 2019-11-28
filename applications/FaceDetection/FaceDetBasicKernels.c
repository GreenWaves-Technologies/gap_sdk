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
#include <math.h>
#include "FaceDetBasicKernels.h"
#include "setup.h"

#include "pmsis.h"
#include "Gap.h"

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)
{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

void KerResizeBilinear(KerResizeBilinear_ArgT *Arg)

{
        unsigned char * __restrict__ In  = Arg->In;
        unsigned int Win                 = Arg->Win;
        unsigned int Hin                 = Arg->Hin;
        unsigned char * __restrict__ Out = Arg->Out;
        unsigned int Wout                = Arg->Wout;
        unsigned int Hout                = Arg->Hout;
        unsigned int HTileOut            = Arg->HTileOut;
        unsigned int FirstLineIndex      = Arg->FirstLineIndex;

        unsigned int CoreId = gap_coreid();
        unsigned int ChunkCell = ChunkSize(Wout);
        unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);

        unsigned int WStep = ((Win-1)<<16)/Wout;
        unsigned int HStep = ((Hin-1)<<16)/Hout;

        unsigned int x, y;
        unsigned int hCoeff = HStep*FirstLineIndex;
        unsigned int BaseY = (hCoeff>>16);
        for (y = 0 ; y < HTileOut ; y++) {
                unsigned int offsetY = (hCoeff >> 16) - BaseY;
                unsigned int hc2 = (hCoeff >> 9) & 127;
                unsigned int hc1 = 128 - hc2;
                // unsigned int wCoeff = 0;
                unsigned int wCoeff = First*WStep;

                // for (x = 0 ; x < Wout ; x++) {
                for (x = First ; x < Last ; x++) {
                        unsigned int offsetX = (wCoeff >> 16);
                        unsigned int wc2 = (wCoeff >> 9) & 127;
                        unsigned int wc1 = 128 - wc2;
                        unsigned int P1 = In[offsetY*Win       + offsetX    ];
                        unsigned int P2 = In[(offsetY + 1)*Win + offsetX    ];
                        unsigned int P3 = In[offsetY*Win       + offsetX + 1];
                        unsigned int P4 = In[(offsetY + 1)*Win + offsetX + 1];
                        Out[y*Wout + x] = ((P1*hc1 + P2*hc2)*wc1 + (P3*hc1 + P4*hc2)*wc2) >> 14;
                        wCoeff += WStep;
                }
                hCoeff += HStep;
        }
        gap_waitbarrier(0);
}


void KerIntegralImagePrime(KerPrimeImage_ArgT *KerArg)
{
        unsigned int W = KerArg->W;
        unsigned int *Buffer = KerArg->KerBuffer;
        unsigned int Col,CoreId = gap_coreid();

        unsigned int ChunkBlock = ChunkSize(W);
        unsigned int First = CoreId*ChunkBlock;
        unsigned int Last  = (First+ChunkBlock > W) ? (W) : (First+ChunkBlock);

        for (Col=First; Col<Last; Col++){
                Buffer[Col] = 0;
        }

}

void KerIntegralImageProcess(KerProcessImage_ArgT *KerArg)

{
        unsigned char* inImg = KerArg->In;
        unsigned int W = KerArg->W;
        unsigned int H = KerArg->H;
        unsigned int* outIntImg = KerArg->IntegralImage;
        unsigned int* buff = KerArg->KerBuffer;
        unsigned int Col,Line;
        unsigned int CoreId = gap_coreid();

        unsigned int ChunkBlock = ChunkSize(W);
        unsigned int FirstCol = CoreId*ChunkBlock;
        unsigned int LastCol  = (FirstCol+ChunkBlock > W) ? (W) : (FirstCol+ChunkBlock);

        ChunkBlock = ChunkSize(H);
        unsigned int FirstLine = CoreId*ChunkBlock;
        unsigned int LastLine  = (FirstLine+ChunkBlock > H) ? (H) : (FirstLine+ChunkBlock);

        for (Col=FirstCol; Col<LastCol; Col++){
                outIntImg[Col] = inImg[Col] + buff[Col];
                for (Line=0; Line<H-1; Line++){
                        outIntImg[Col + ((Line+1)*W)] = outIntImg[Col+(Line)*W] + inImg[Col+(Line+1)*W];
                }

        }
        gap_waitbarrier(0);
        //Saving to Buff intermediate results
        for (Col=FirstCol; Col<LastCol; Col++){
                buff[Col] = outIntImg[Col+((H-1)*W)];
        }

        gap_waitbarrier(0);
        for (Line=FirstLine; Line<LastLine; Line++){
                for (Col=0; Col<W-1; Col++){
                        outIntImg[Col+1 +(Line*W)] = outIntImg[Col+(Line)*W] + outIntImg[Col+1+(Line)*W];
                }
        }
}


void KerSquaredIntegralImageProcess(KerProcessImage_ArgT *KerArg)

{
        unsigned char* inImg = KerArg->In;
        unsigned int W = KerArg->W;
        unsigned int H = KerArg->H;
        unsigned int* outIntImg = KerArg->IntegralImage;
        unsigned int* buff = KerArg->KerBuffer;
        unsigned int Col,Line;
        unsigned int CoreId = gap_coreid();

        unsigned int ChunkBlock = ChunkSize(W);
        unsigned int FirstCol = CoreId*ChunkBlock;
        unsigned int LastCol  = (FirstCol+ChunkBlock > W) ? (W) : (FirstCol+ChunkBlock);

        ChunkBlock = ChunkSize(H);
        unsigned int FirstLine = CoreId*ChunkBlock;
        unsigned int LastLine  = (FirstLine+ChunkBlock > H) ? (H) : (FirstLine+ChunkBlock);

        for (Col=FirstCol; Col<LastCol; Col++){
                outIntImg[Col] = (inImg[Col]*inImg[Col]) + buff[Col];
                for (Line=0; Line<H-1; Line++){
                        outIntImg[Col + ((Line+1)*W)] = outIntImg[Col+(Line)*W] + (inImg[Col+(Line+1)*W]*inImg[Col+(Line+1)*W]);
                }

        }
        gap_waitbarrier(0);
        //Saving to Buff intermediate results
        for (Col=FirstCol; Col<LastCol; Col++){
                buff[Col] = outIntImg[Col+((H-1)*W)];
        }

        gap_waitbarrier(0);
        for (Line=FirstLine; Line<LastLine; Line++){
                for (Col=0; Col<W-1; Col++){
                        outIntImg[Col+1 +(Line*W)] = outIntImg[Col+(Line)*W] + outIntImg[Col+1+(Line)*W];
                }
        }
}



#define DETECT_THRESHOLD 1
#define SKIP_HOMOGENEOUS 1

static unsigned int SquareRootRounded(unsigned int a_nInput)
{
    unsigned int op  = a_nInput;
    unsigned int res = 0;
    unsigned int one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type

    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }

    /* Do arithmetic rounding to nearest integer */
    if (op > res)
    {
        res++;
    }

    return res;
}


static int integral_image_lookup(unsigned int* __restrict__ integralImage, int x, int y, int w, int h, int win_w)
{
        unsigned int res = (integralImage[(h+y)*win_w + w+x] + integralImage[(y*win_w) + x] - integralImage[(y)*win_w + w+x] -integralImage[(h+y)*win_w + x]);

        //#define DEBUG 1
        //PRINTF("II values:%d %d %d %d: %d\n", integralImage[(y)*win_w + x],integralImage[(h+y)*win_w + w+x], integralImage[(y)*win_w + w+x],integralImage[(h+y)*win_w + x], res);

        return res;
}

static int eval_weak_classifier(unsigned int* __restrict__ integralImage,int img_w, single_cascade_t* __restrict__ weaks, int std,int t_idx, int w_idx, int r_idx,int off_x, int off_y)
{

        int sumw=0;

        /* The node threshold is multiplied by the standard deviation of the sub window */
        int t = (int)weaks->thresholds[t_idx] * std;

        for (int i=0; i<(weaks->rect_num[t_idx+1] - weaks->rect_num[t_idx]); i++) {
                int x = weaks->rectangles[r_idx + (i<<2) + 0];
                int y = weaks->rectangles[r_idx + (i<<2) + 1];
                int w = weaks->rectangles[r_idx + (i<<2) + 2];
                int h = weaks->rectangles[r_idx + (i<<2) + 3];
                // Lookup the feature
                sumw += ((int)integral_image_lookup(integralImage, off_x + x, off_y+y, w, h,img_w))* (((int) weaks->weights[w_idx + i])<<12);
        }

        if (sumw >= t) {
                return weaks->alpha2[t_idx];
        }

        return weaks->alpha1[t_idx];
}


static void spawn_eval_weak_classifier(eval_weak_classifier_Arg_T* Arg)
{

    single_cascade_t *single_cascade=Arg->cascade_stage;

    unsigned int CoreId = gap_coreid();
    unsigned int ChunkBlock = ChunkSize(single_cascade->stage_size);
    unsigned int FirstCol = CoreId*ChunkBlock;
    unsigned int LastCol  = (FirstCol+ChunkBlock > (single_cascade->stage_size)) ? (single_cascade->stage_size) : (FirstCol+ChunkBlock);

    int w_idx, r_idx;
    Arg->stage_sum[CoreId] = 0;

    for (unsigned int t_idx=FirstCol; t_idx<LastCol; t_idx++) {
        w_idx = single_cascade->rect_num[t_idx];
        r_idx = single_cascade->rect_num[t_idx] * 4;
        // Send the shifted window to a haar filter
        Arg->stage_sum[CoreId] += eval_weak_classifier(Arg->integralImage,Arg->img_w,single_cascade, Arg->std, t_idx, w_idx, r_idx, Arg->off_x, Arg->off_y);
    }
}

//COPY a cascade stage to L1
void async_cascade_stage_to_l1(single_cascade_t* cascade_l2, single_cascade_t* cascade_l1, pi_cl_dma_copy_t* Dma_Evt){

    unsigned int addr = (unsigned int) cascade_l1;
    //cascade_l1 = cascade_l1(single_cascade_t*) addr;

    addr+= sizeof(single_cascade_t);

    cascade_l1->stage_size = cascade_l2->stage_size;
    cascade_l1->rectangles_size = cascade_l2->rectangles_size;

    cascade_l1->thresholds     = (short*)addr;//rt_alloc( RT_ALLOC_CL_DATA, sizeof(short)*cascade_l2->stage_size);
    addr+=sizeof(short)*cascade_l1->stage_size;
    __cl_dma_memcpy((unsigned int) cascade_l2->thresholds, (unsigned int) cascade_l1->thresholds, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

    cascade_l1->alpha1       = (short*) addr;//(short*)rt_alloc( RT_ALLOC_CL_DATA, sizeof(short)*cascade_l2->stage_size);
    addr+=sizeof(short)*cascade_l1->stage_size;
    __cl_dma_memcpy((unsigned int) cascade_l2->alpha1, (unsigned int) cascade_l1->alpha1, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

    cascade_l1->alpha2         = (short*) addr;//rt_alloc( RT_ALLOC_CL_DATA, sizeof(short)*cascade_l2->stage_size);
    addr+=sizeof(short)*cascade_l1->stage_size;
    __cl_dma_memcpy((unsigned int) cascade_l2->alpha2, (unsigned int) cascade_l1->alpha2, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

    cascade_l1->rect_num       = (unsigned  short*) addr;//rt_alloc( RT_ALLOC_CL_DATA, sizeof(unsigned short)*((cascade_l2->stage_size)+1));
    addr+= sizeof(unsigned short)*((cascade_l1->stage_size)+1);
    __cl_dma_memcpy((unsigned int) cascade_l2->rect_num, (unsigned int) cascade_l1->rect_num, sizeof(unsigned short)*(cascade_l1->stage_size+1), PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

    cascade_l1->weights    = (signed char*) addr;//rt_alloc( RT_ALLOC_CL_DATA, sizeof(signed char)*(cascade_l2->rectangles_size/4));
    addr+=sizeof(signed char)*(cascade_l1->rectangles_size << 2);
    __cl_dma_memcpy((unsigned int) cascade_l2->weights, (unsigned int) cascade_l1->weights, sizeof(signed char)*(cascade_l2->rectangles_size/4), PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

    cascade_l1->rectangles = (char*) addr;//rt_alloc( RT_ALLOC_CL_DATA, sizeof(char)*cascade_l2->rectangles_size);
    addr+=sizeof(signed char)*(cascade_l2->rectangles_size<<2);
    __cl_dma_memcpy((unsigned int) cascade_l2->rectangles, (unsigned int) cascade_l1->rectangles, sizeof(char)*cascade_l2->rectangles_size, PI_CL_DMA_DIR_EXT2LOC, 0, Dma_Evt);
    pi_cl_dma_wait(Dma_Evt);

}

static int windows_cascade_classifier(unsigned int* __restrict__ integralImage, unsigned int* __restrict__ sqaredIntegralImage, cascade_t * __restrict__ cascade, int win_w, int win_h, int img_w,int off_x, int off_y){

        pi_cl_dma_copy_t Dma_Evt;
        int buffer=0;
        int n = (win_w * win_h);
        int i_s = integral_image_lookup (integralImage,      off_x,off_y,win_w,win_h,img_w);
        int i_sq = integral_image_lookup(sqaredIntegralImage,off_x,off_y,win_w,win_h,img_w);
        int m = i_s/n;
        int v = i_sq/n-(m*m);

        #if SKIP_HOMOGENEOUS
        // Skip homogeneous regions.
        if (v<(50*50)) {
                return 0;
        }
        #endif

        int std = i_sq*n-(i_s*i_s);

        std = SquareRootRounded(std);

        int stage_sum[8];
        int stages_score=0;
        eval_weak_classifier_Arg_T Arg;

        Arg.integralImage = integralImage;

        Arg.img_w = img_w;
        Arg.stage_sum = stage_sum;
        Arg.std   = std;
        Arg.off_x = off_x;
        Arg.off_y = off_y;
        int i;

        async_cascade_stage_to_l1((cascade->stages[CASCADE_STAGES_L1]), (cascade->buffers_l1[buffer%2]), &Dma_Evt);

        for (i=0; i<cascade->stages_num; i++) {
                //PRINTF("Here %d\n",i);
                if(i<CASCADE_STAGES_L1)
                    Arg.cascade_stage=(cascade->stages[i]);

                if(i>=CASCADE_STAGES_L1){
                    //pi_cl_dma_wait(&Dma_Evt);
                    //PRINTF("Here 1 %d\n",i);
                    Arg.cascade_stage = (cascade->buffers_l1[buffer%2]);
                    //If it is not last
                if( i<cascade->stages_num-1)
                        async_cascade_stage_to_l1((cascade->stages[i+1]), (cascade->buffers_l1[(++buffer)%2]), &Dma_Evt);
                    //PRINTF("Here 2 %d\n",i);
                }
                pi_cl_team_fork(gap_ncore(), (void *) spawn_eval_weak_classifier, (void *) &Arg);
                //Here we suppose always using 8 cores!
                stage_sum[0]+= stage_sum[1] + stage_sum[2] + stage_sum[3] + stage_sum[4] + stage_sum[5] + stage_sum[6] + stage_sum[7];
                //Move this operation offline
                stages_score+=stage_sum[0]-cascade->thresholds[i];
                //This Operation can be moved offline!
                if (stage_sum[0] < (cascade->thresholds[i])) {
                //if (stage_sum[0] < ((DETECT_THRESHOLD) * cascade->thresholds[i])) {
                        return 0;
                }
        }

        return stages_score;
}


void KerEvaluateCascade(
    unsigned int * __restrict__ IntegralImage,
    unsigned int * __restrict__ SquaredIntegralImage,
    unsigned int W,
    unsigned int H,
    void * cascade_model,
    unsigned char WinW,
    unsigned char WinH,
    int * __restrict__ CascadeReponse){

    cascade_t *model = (cascade_t*) cascade_model;

    for(unsigned int Line=0;Line<H-WinW+1;Line++){
        for (unsigned int Col=0; Col<W-WinH+1; Col++){
            if((Line%DETECT_STRIDE) == 0 && (Col%DETECT_STRIDE) == 0)
                CascadeReponse[Line*(W-WinW+1) + (Col)] = windows_cascade_classifier(IntegralImage, SquaredIntegralImage, model, WinW, WinH, W, Col, Line);
            else
            CascadeReponse[Line*(W-WinW+1) + (Col)]=0;
        }
    }

    return;
}

