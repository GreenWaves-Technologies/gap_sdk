/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#include <stdio.h>
#include "Gap.h"
#include "CNN_BasicKernels.h"
#include "SSD_BasicKernels.h"

// optimize the division to find the chunk size
// equivalent to ceil(KerArg0->W/rt_nb_pe())
inline static unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
    unsigned int NCore = gap_ncore();
    unsigned int Log2Core = gap_fl1(NCore);
    unsigned int Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
    return Chunk;
}

#define Abs(a)      (((int)(a)<0)?(-(a)):(a))

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


void Ker_SSD_Init(Ker_SSD_Init_ArgT  *KerArg0 )
{    
    *(KerArg0->bbox_idx) = 0;
}


// Q and info parameters    0            1         2        3           4         5        6          7
//uint8_t SSD_Inscales = [scale_x, scale_x_anc, scale_y, scale_y_anc, scale_h, scale_w, scale_ao, scale_score] uint8
//uint8_t norms        = [norm_x,  norm_x_anc,  norm_y,  norm_y_anc,  norm_h,  norm_w,  norm_ao,  norm_score] uint8
//infos  = [IOU threshold (Q7), score threshold (Q of output), max detection (Q0), max detection per class (Q0)]

// The actual code that does the tile addition
void Ker_SSD_Decoder(Ker_SSD_Decoder_ArgT  *KerArg0 )
{
    unsigned int CoreId = gap_coreid();
    unsigned int Chunk  = ChunkSize(KerArg0->H);
    unsigned int First  = CoreId*Chunk;
    unsigned int Last   = (First+Chunk > KerArg0->H) ? (KerArg0->H) : (First+Chunk);
    bbox_t * bbox       = KerArg0->bbox_out;
    int8_t * scores     = KerArg0->classes_in;
    int bbn             = *(KerArg0->bbox_idx);
    int num_classes     = KerArg0->Class_W;
    int16_t n_max_bb    = KerArg0->n_max_bb;
    int8_t score_th     = KerArg0->infos[1];
    uint8_t* scales     = KerArg0->in_scales;
    uint8_t* norms      = KerArg0->in_norms;
    int num_coords      = 4;
    int boxes_idx, Ax, Aw, Ay, Ah, O;

	for (unsigned int i = First; i<Last; ++i)
       {
        boxes_idx = i*num_coords;
        for (unsigned int j=1; j<num_classes; j++){
            if((scores[(i*num_classes)+j]) > score_th){
                #ifndef __EMUL__
                pi_cl_team_critical_enter();
                #endif
                bbn++;
                if((bbn - *(KerArg0->bbox_idx)) >= n_max_bb){
                    #ifndef __EMUL__
                    pi_cl_team_critical_exit(); 
                    #endif
                    *(KerArg0->bbox_idx) = bbn;
                    return;
                }
                #ifndef __EMUL__
                pi_cl_team_critical_exit();
                #endif
                // Valid BBOX --> alive
                bbox[bbn].alive = 1;
                //Save score always as a Q15
                bbox[bbn].score = scores[(i*num_classes)+j]*scales[7] << (15 - norms[7]);
                bbox[bbn].class = j;
                // xcnt, ycnt --> Q14
                // xcnt = (So*O * Sa*Aw)/params.x_scale + Sa*Ax = So*Sa/params.x_scale (O*Aw + x_scale/So * Ax) =
                //        (scale_x * (O*Aw + (scale_x_anc*Ax)>>scale_x_ancNorm))>>scale_xNorm =
                //        at_norm(scale_x*(O*Aw + at_norm(scale_x_anc*Ax, scale_x_ancNorm)), scale_xNorm)
                O = KerArg0->boxes_in[boxes_idx+1];
                Ax = KerArg0->anchors_in[boxes_idx+1];
                Aw = KerArg0->anchors_in[boxes_idx+3];
                bbox[bbn].x = AT_NORM(scales[0]*(O*Aw + AT_NORM(scales[1]*Ax, norms[1])), norms[0]);
                O = KerArg0->boxes_in[boxes_idx];
                Ay = KerArg0->anchors_in[boxes_idx];
                Ah = KerArg0->anchors_in[boxes_idx+2];
                bbox[bbn].y = AT_NORM(scales[2]*(O*Ah + AT_NORM(scales[3]*Ay, norms[3])), norms[2]);
                // half_h, half_w --> Q14
                // half_h = exp(So*Off / params.h_scale) * Sa*A = Sa/So * exp(So/params.h_scale *O) * A =
                //          (scale_ao * (Aw * exp17.15(scale_h*O<<15-scale_hNorm))>>scale_aoNorm) =
                //          at_norm(scale_ao*(A*exp17.15(scale_h*O << 15-scale_hNorm)), scale_aoNorm)
                O = KerArg0->boxes_in[boxes_idx+3];
                bbox[bbn].w = AT_NORM(scales[6] * (Aw * Exp_fp_17_15((O * scales[5]) << 15-norms[5])), norms[6]);
                O = KerArg0->boxes_in[boxes_idx+2];
                bbox[bbn].h = AT_NORM(scales[6] * (Ah * Exp_fp_17_15((O * scales[4]) << 15-norms[4])), norms[6]);

                bbox[bbn].x = bbox[bbn].x - (bbox[bbn].w >> 1);
                bbox[bbn].y = bbox[bbn].y - (bbox[bbn].h >> 1);
            }
    	}
    }
    *(KerArg0->bbox_idx) = bbn;
}

int16_t ioveru( short a_x, short a_y, short a_w, short a_h,
                short b_x, short b_y, short b_w, short b_h, int Thr ){

    int intersect, runion,iou;
    int x = Max(a_x,b_x);
    int y = Max(a_y,b_y); 

    int size_x = Min(a_x+a_w,b_x+b_w) - x;
    int size_y = Min(a_y+a_h,b_y+b_h) - y;

    if(size_x <=0 || size_y <=0){
        intersect=0;
        return (Thr?0:1);
    }
    else
        intersect=(size_x*size_y); 

    runion = (((a_w)*(a_h))) + (((b_w)*(b_h))) - intersect ;

    iou =  (intersect * (1 << 14)) / runion;
    return intersect >= ((runion * Thr)>>7);
}


void non_max_suppress(bbox_t * boundbxs, int iouThres, int nnbb){
    //BBOX value are in Q14 and non_max_threshold in Q14
    int idx, idx_int;
    //Non-max supression
    for(idx=0; idx<nnbb; idx++){
        //check if rect has been removed (-1)
        if(boundbxs[idx].alive == 0)
            continue;

        for(idx_int=idx+1; idx_int<nnbb; idx_int++){
            if((boundbxs[idx_int].alive==0 || idx_int==idx) || (boundbxs[idx_int].class != boundbxs[idx].class))
                continue;
            //check the intersection between rects
            int iou = ioveru(boundbxs[idx].x,boundbxs[idx].y,boundbxs[idx].w,boundbxs[idx].h,
                             boundbxs[idx_int].x,boundbxs[idx_int].y,boundbxs[idx_int].w,boundbxs[idx_int].h, iouThres);
            if(iou){ //is non-max
                //supress the one that has lower score that is alway the internal index, since the input is sorted
                boundbxs[idx_int].alive = 0;
            }
        }
    }
}
#if 0
void init_test(bbox_t * boundbxs){
    boundbxs[0].x = FP2FIX(0.418335,14);
    boundbxs[0].y = FP2FIX(0.227356,14);
    boundbxs[0].w = FP2FIX(0.347717,14);
    boundbxs[0].h = FP2FIX(0.349854,14);
    boundbxs[0].score = FP2FIX(0.757812,15);
    boundbxs[0].class = 3;
    boundbxs[0].alive = 1;

    boundbxs[1].x = FP2FIX(0.480591,14);
    boundbxs[1].y = FP2FIX(0.148743,14);
    boundbxs[1].w = FP2FIX(0.250061,14);
    boundbxs[1].h = FP2FIX(0.481140,14);
    boundbxs[1].score = FP2FIX(0.7,15);
    boundbxs[1].class = 3;
    boundbxs[1].alive = 1;

    boundbxs[2].x = FP2FIX(0.1,14);
    boundbxs[2].y = FP2FIX(0.1,14);
    boundbxs[2].w = FP2FIX(0.3,14);
    boundbxs[2].h = FP2FIX(0.3,14);
    boundbxs[2].score = FP2FIX(0.7,15);
    boundbxs[2].class = 2;
    boundbxs[2].alive = 1;

    boundbxs[3].x = FP2FIX(0.2,14);
    boundbxs[3].y = FP2FIX(0.2,14);
    boundbxs[3].w = FP2FIX(0.2,14);
    boundbxs[3].h = FP2FIX(0.2,14);
    boundbxs[3].score = FP2FIX(0.6,15);
    boundbxs[3].class = 2;
    boundbxs[3].alive = 1;


}
#endif

void Ker_SSD_NMS(Ker_SSD_NMS_ArgT  *KerArg0 )
{
    
    int16_t bbox_idx_max = *(KerArg0->bbox_idx);
    int16_t bbox_max     = KerArg0->n_max_bb;
    int max_detections   = KerArg0->infos[2];
    bbox_t * bbox        = KerArg0->bbox_out;
    int non_max_thres    = ((int) KerArg0->infos[0]) << 7;
    bbox_t temp;

    //Sort results from the most confident
    int changed;
    do{
        changed=0;
        //TODO change to MAX BB
        for(int i=0; i<bbox_idx_max;i++){
            if(bbox[i].score < bbox[i+1].score){
                temp      = bbox[i];
                bbox[i]   = bbox[i+1];
                bbox[i+1] = temp;
                changed   = 1;
            }
        }
    }while(changed);
    
    #if 0
    //To test algo with sample boxes
    bbox_t test_bbx[4];
    init_test(test_bbx);
    non_max_suppress(test_bbx,(int16_t)KerArg0->infos[0]<<7,4);
    int bbn=0;
    for(int bbn=0; bbn<4;bbn++){
        if(test_bbx[bbn].alive==1){
            //printf("ADD BB %d, class: %d score %f - x: %d, y:%d, w: %d, h:%d\n",bbn,bbox[bbn].class,FIX2FP(bbox[bbn].score,15),bbox[bbn].x,bbox[bbn].y,bbox[bbn].w,bbox[bbn].h);
            printf("ADD BB %d, class: %d score %f - x: %f, y:%f, w: %f, h:%f\n",bbn,test_bbx[bbn].class,FIX2FP(test_bbx[bbn].score,15),FIX2FP(test_bbx[bbn].y,14),FIX2FP(test_bbx[bbn].x,14),FIX2FP(test_bbx[bbn].h,14),FIX2FP(test_bbx[bbn].w,14));
        }
    }
    #endif

    //Suppressing double detections
    non_max_suppress(bbox, non_max_thres, bbox_idx_max);

    //Applying max output from TFLITE
    for(int i=0; i<bbox_max; i++){
        if  (bbox[i].alive==1 && max_detections) max_detections--;
        else bbox[i].alive=0;
    }

    #if 0
    //To print them out
    for(int bbn=0; bbn<bbox_max;bbn++){
    if(bbox[bbn].alive==1){
        //printf("ADD BB %d, class: %d score %f - x: %d, y:%d, w: %d, h:%d\n",bbn,bbox[bbn].class,FIX2FP(bbox[bbn].score,15),bbox[bbn].x,bbox[bbn].y,bbox[bbn].w,bbox[bbn].h);
        printf("ADD BB %d, alive %d, class: %d score %f - x: %f, y:%f, w: %f, h:%f\n",bbn,bbox[bbn].alive,bbox[bbn].class,FIX2FP(bbox[bbn].score,15),FIX2FP(bbox[bbn].y,14),FIX2FP(bbox[bbn].x,14),FIX2FP(bbox[bbn].h,14),FIX2FP(bbox[bbn].w,14));
     }
    }
    #endif
}