/*
 * Copyright 2020 GreenWaves Technologies, SAS
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
#include "SSDBasicKernels.h"
#include "SSDParams.h"
#include "setup.h"

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

void KerSDD3Dto2DShort(KerSDD3Dto2DShort_ArgT *Arg)
{

    short * __restrict__ In   = Arg->In;
    unsigned int Win          = Arg->Win;
    unsigned int Hin          = Arg->Hin;
    short * __restrict__ Out  = Arg->Out;
    unsigned int Wout         = Arg->Wout;
    unsigned int Hout         = Arg->Hout;
    unsigned char Q           = Arg->Q;
    unsigned short n_classes  = Arg->n_classes;

    unsigned int CoreId = gap_coreid();
    unsigned int ChunkCell = ChunkSize(Win);
    unsigned int First = CoreId*ChunkCell, Last  = Min(Win, First+ChunkCell);

    
    for(int j=First;j<Last;j++){
        for(int i=0;i<Hin;i++){
            Out[j*Hin+i] = In[i*Win+j];
        }
    }

    //TODO: This we need to be shure that classes are not split between PE
    if(Q){
        ChunkCell = ChunkSize(Win*Hin);
        First = CoreId*ChunkCell, Last  = Min(Win*Hin, First+ChunkCell); 
        for(int i=First;i<Last;i+=n_classes) SoftMax_fp16(&Out[i],n_classes,&Out[i],Q);
    }
    
}


#define Abs(a)      (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)   (((a)<(b))?(a):(b))
#define Max(a, b)   (((a)>(b))?(a):(b))

static unsigned short int IntegerExpLUT[] =
{
    0x0001, 0x0002, 0x0007, 0x0014, 0x0036, 0x0094, 0x0193, 0x0448, 0x0BA4, 0x1FA7, 0x560A, 0xE9E2
};

static unsigned short int FractionExpLUT[] =
{
    0x0000, 0x5BF1, 0x31CD, 0x0AF3, 0x4C90, 0x34E2, 0x36E3, 0x510B, 0x7A9F, 0x0ABE, 0x3B9F, 0x1224
};

/* 17.15 fixed point format */
static unsigned short int ExpCoeffLUT[] =
{
    0x7FFF, 0x7FFF, 0x4000, 0x1555, 0x0555, 0x0111, 0x002E, 0x0007, 0x0001
};


#define ARRAYSIZE(x)    (sizeof(x) / sizeof(x[ 0 ]))

/* X : fixed point, format Q17.15, returns in Q17.15 */
static unsigned int Exp_fp_17_15(unsigned int X)

{
    int  Y, Result, IntX, FractX, ScaledInt;
    short int Z_s, FractX_s;
    unsigned short int  ScaledFract;

    if (!X) return 0x8000;
    Y = Abs(X);
    IntX = (Y >> 15);
    if (IntX >= (int) ARRAYSIZE (IntegerExpLUT))
    {
        if (Y == X) return 0x7FFFFFFF;
        else return 0;
    }
    FractX = (Y & 0x7FFF);
    if (gap_bitextractu(FractX, 1, 14))
    {
        /* Taylor series converges quickly only when | FractX | < 0.5 */
        FractX -= 0x8000;
        IntX++;
    }
    ScaledInt = IntegerExpLUT[IntX];
    ScaledFract = FractionExpLUT[IntX];
    /* Taylor's series: exp(x) = 1 + x + x ^ 2 / 2 + x ^ 3 / 3! + x ^ 4 / 4! + x ^ 5 / 5! + x ^ 6 / 6! + x ^ 7 / 7! + x ^ 8 / 8!  */
    FractX_s = FractX;
    Z_s = FractX;
    Result = 0;
    for (int i = 1; i < ARRAYSIZE (ExpCoeffLUT); i++)
    {
        Result += Z_s * ExpCoeffLUT[i]; // gap8_macs(Result, Z, ExpCoeffLUT[ i ]);
        Z_s = gap_mulsRN(Z_s, FractX_s, 15);
    }
    Result = gap_roundnorm(Result, 15) + ExpCoeffLUT[0];
    unsigned short int U_Res = Result;
    Result = gap_muluRN(U_Res, ScaledFract, 15) + U_Res * ScaledInt;
    if (Result && (X > 0x7FFFFFFF))
        Result = ((0x7FFFFFFF / Result) >> 1);      /* negative value */
    return (unsigned int) Result;
}

//Output and input can be the same buffer
void SoftMax_fp16(
    short int *in_buffer,
    int i_size,
    short int *out_buffer,
    int norm //This is Qx of the input
    )

{
    short int * In = in_buffer;
    short int * Out = out_buffer;
    int N = i_size;
    unsigned Norm = norm;

    int M, Sum, InvSum;

    /* Turns In into distribution
    /* Find max */
    M = 0x80000000;
    for (int i = 0; i < i_size; i++) M = Max(M, In[i]);
    
    Sum = 0;
    for (int i = 0; i < i_size; i++)
    {
        unsigned int Exp = Exp_fp_17_15((In[i]-M) << (15 - Norm));
        Out[i] = Exp;
        Sum += Exp;
    }
    
    
    InvSum = (FP2FIX(1.0, 15) << 15) / Sum;
    for (int i = 0; i < i_size; i++) Out[i] = Abs(gap_roundnorm_reg(Out[i] * InvSum, 15));

}



void KerEstimate_bbox(int index, int global_index,int confidence, short int *Boxes,int Boxes_Q, Alps *Anchors, bboxs_t *bbxs, int class){

    int anchor_location;
    int i_loc;
    int j_loc;
    int anchor_id;
    int16_t cx_offset, cy_offset, w_offset, h_offset;
    int32_t cx, cy;
    float w, h;

    //This can be done offline
    float zero_height = Anchors->offset_height * Anchors->step_height;
    float zero_width  = Anchors->offset_width  * Anchors->step_width;

    // step 1: decode the anchor location and its id
    anchor_location  = global_index/Anchors->n_anchors;
    anchor_id        = global_index%Anchors->n_anchors;
    i_loc    = anchor_location/Anchors->feature_map_width;
    j_loc    = anchor_location%Anchors->feature_map_width;

    // step 2: Identify the offsets
    cx_offset = Boxes[index*Anchors->anchor_params+0];//Q11
    cy_offset = Boxes[index*Anchors->anchor_params+1];//Q11
    w_offset  = Boxes[index*Anchors->anchor_params+2];//Q11
    h_offset  = Boxes[index*Anchors->anchor_params+3];//Q11
    //printf("%f\n",FIX2FP(w_offset,11));
    //printf("%f\n",FIX2FP(h_offset,11));

    // step 3: calculate default anchor parameters for this location
    cx = FP2FIX(((zero_width  + (float)j_loc * Anchors->step_width)/(float)Anchors->img_width)  , 20);
    cy = FP2FIX(((zero_height + (float)i_loc * Anchors->step_height)/(float)Anchors->img_height), 20);


    //[TODO] This can be done offline
    anchorWH_t anchorWH = Anchors->anchorsWH[anchor_id];
    w = anchorWH.w / (float)Anchors->img_width;
    h = anchorWH.h / (float)Anchors->img_height;
    
    //printf("\nAnchor info: %f, %f, %f, %f\n", cx, cy, w, h);
    int16_t var_0_f = FP2FIX(Anchors->variances[0]  * w,10);
    int16_t var_1_f = FP2FIX(Anchors->variances[1]  * h,10);
    int16_t var_2_f = FP2FIX(Anchors->variances[2]     ,10);
    int16_t var_3_f = FP2FIX(Anchors->variances[3]     ,10);

    //printf("w: %f\n",w);
    //printf("h: %f\n",h);

    uint32_t w_fix = FP2FIX(w,11);
    uint32_t h_fix = FP2FIX(h,11);
    //printf("w_fix: %d\n",w_fix);
    //printf("h_fix: %d\n",h_fix);


    // step 4: apply offsets to the default anchor box 
    int shift = 20-10-Boxes_Q;
    // step 4: apply offsets to the default anchor box
    if(shift<0){
        bbxs->bbs[bbxs->num_bb].x  = ((cx_offset * var_0_f) >> -shift) + cx;
        bbxs->bbs[bbxs->num_bb].y  = ((cy_offset * var_1_f) >> -shift) + cy;
    }else{
        bbxs->bbs[bbxs->num_bb].x  = ((cx_offset * var_0_f) << shift) + cx;
        bbxs->bbs[bbxs->num_bb].y  = ((cy_offset * var_1_f) << shift) + cy;
    }    
    //printf("%f\n",FIX2FP(var_2_f,11));
    //printf("%f\n",FIX2FP(var_3_f,11));
    
    //printf("%f\n",FIX2FP((w_offset * var_2_f),22));
    //printf("%f\n",FIX2FP((h_offset * var_3_f),22));
    
    //unsigned int Exp = Exp_fp_17_15((In[i]-M) << (15 - Norm));
    unsigned int Exp;
    Exp = Exp_fp_17_15( (w_offset * var_2_f) >> (Boxes_Q-5));
    bbxs->bbs[bbxs->num_bb].w       = (Exp * w_fix); //Output Q26
    //printf("exp: %f\n",FIX2FP(Exp,15));
    //printf("w_fix: %f\n",FIX2FP(w_fix,15));
    //printf("exp mult: %f\n",FIX2FP(Exp * w_fix,15));
    //printf("exp mult: %f\n",FIX2FP(bbxs->bbs[bbxs->num_bb].w,26));

    Exp = Exp_fp_17_15( (h_offset * var_3_f) >> (Boxes_Q-5));
    bbxs->bbs[bbxs->num_bb].h       = (Exp * h_fix); //Output Q26
    
    //printf("exp: %f\n",FIX2FP(Exp,15));
    //printf("h_fix: %f\n",FIX2FP(h_fix,15));
    //printf("exp mult: %f\n",FIX2FP(bbxs->bbs[bbxs->num_bb].h,26));
    
    bbxs->bbs[bbxs->num_bb].class   = class;
    bbxs->bbs[bbxs->num_bb++].score = confidence;

    //printf("\n");
}


void KerPredecoderShort(KerPredecoderShort_ArgT *Arg){

    short * __restrict__ Classes = Arg->Classes;
    short * __restrict__ Boxes   = Arg->Boxes;
    unsigned int W          = Arg->Classes_W;  //Same info as n_classes
    unsigned int H          = Arg->Classes_H;
    unsigned int TileIndex  = Arg->Classes_TileIndex;
    unsigned int Std_H      = Arg->Classes_Std_H;
    unsigned int n_classes  = Arg->n_classes;
    int Boxes_Q  = Arg->Boxes_Q;
    
    Alps * anch             = (Alps*) Arg->Ancor_layer;
    bboxs_fp_t* bbxs        = (bboxs_fp_t*) Arg->BoundingBoxes;
    
    unsigned int CoreId = gap_coreid();


    if(!CoreId){
        for(int i=0;i<H;i++){
            int global_index = TileIndex*Std_H;
        //we start from 1 since we skip the Background
            for(uint8_t n=1;n<W;n++){

            //Here it would be nice to add a different confidence for each class
                if(Classes[i*n_classes+n] > anch->confidence_thr){
                //printf("Confidence > %f:  %f\n", FIX2FP(anch->confidence_thr,15),FIX2FP(classes[i*n_classes+n],15));
                //Here we pass the row index to find the correct row in the boxes
                    if(bbxs->num_bb>=MAX_BB){
                        PRINTF("Reached Max BB number...\n");
                        return;
                    }
                    KerEstimate_bbox(i, TileIndex*Std_H+i,Classes[i*n_classes+n], Boxes, Boxes_Q,anch, bbxs, n);
                     
                }
            }
        }
    }
}
