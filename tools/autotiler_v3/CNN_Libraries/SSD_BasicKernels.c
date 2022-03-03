/*
 * Copyright (C) 2020 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

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
    // Initialize the output bounding boxes
    *(KerArg0->bbox_idx) = 0;
    bbox_t * bbox = KerArg0->bbox_buf;
    int n_max_bb  = KerArg0->n_max_bb;
    for (int i=0; i<n_max_bb; i++){
        bbox[i].score = 0;
        bbox[i].alive = 0;
    }
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
    bbox_t * bbox       = KerArg0->bbox_buf;
    int8_t * scores     = KerArg0->classes_in;
    int initial_idx     = KerArg0->bbox_idx[0];
    int num_classes     = KerArg0->Class_W;
    int16_t n_max_bb    = KerArg0->n_max_bb;
    int8_t score_th     = KerArg0->ScoreThr;
    uint8_t* scales     = KerArg0->in_scales;
    uint8_t* norms      = KerArg0->in_norms;
    int num_coords      = 4;
    int boxes_idx, Ax, Aw, Ay, Ah, O;
    int bbn;

	for (unsigned int i = First; i<Last; i++)
    {
        boxes_idx = i*num_coords;
        for (unsigned int j=1; j<num_classes; j++){
            if((scores[(i*num_classes)+j]) > score_th){
                gap_cl_critical_enter();
                bbn = KerArg0->bbox_idx[0]++;
                // printf("Core: %d\tbbox_idx:%d\n", CoreId, KerArg0->bbox_idx[0]);
                if(bbn > n_max_bb){ // check if we reched n_max_bb
                    gap_cl_critical_exit();
                    goto exit_double_for;
                }
                gap_cl_critical_exit();
                // Valid BBOX --> alive
                bbox[bbn].alive = 1;
                //Save score always as a Q7
                bbox[bbn].score = scores[(i*num_classes)+j];
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
                bbox[bbn].w = AT_NORM(scales[6] * (Aw * Exp_fp_17_15((O * scales[5]) << (15-norms[5]))), norms[6]);
                O = KerArg0->boxes_in[boxes_idx+2];
                bbox[bbn].h = AT_NORM(scales[6] * (Ah * Exp_fp_17_15((O * scales[4]) << (15-norms[4]))), norms[6]);

                bbox[bbn].x = bbox[bbn].x - (bbox[bbn].w >> 1);
                bbox[bbn].y = bbox[bbn].y - (bbox[bbn].h >> 1);
            }
    	}
    }
    exit_double_for:
    gap_waitbarrier(0);
}

static int8_t KerIoverU( short a_x, short a_y, short a_w, short a_h,
                short b_x, short b_y, short b_w, short b_h, int Thr ){

    int intersect, runion;
    int x = Max(a_x,b_x);
    int y = Max(a_y,b_y); 

    int size_x = Min(a_x+a_w,b_x+b_w) - x;
    int size_y = Min(a_y+a_h,b_y+b_h) - y;

    if(size_x <= 0 || size_y <= 0){
        // no intersection
        return (Thr?0:1);
    }
    else
        intersect=(size_x*size_y); 

    runion = (a_w*a_h + b_w*b_h) - intersect;

    return intersect >= AT_NORM(Thr * runion, 7);
}


static void KerNonMaxSuppress(bbox_t * boundbxs, float iouThres, int nnbb){
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
            int8_t iou = KerIoverU(boundbxs[idx].x,boundbxs[idx].y,boundbxs[idx].w,boundbxs[idx].h,
                             boundbxs[idx_int].x,boundbxs[idx_int].y,boundbxs[idx_int].w,boundbxs[idx_int].h, iouThres);
            if(iou){ //is non-max
                //supress the one that has lower score that is alway the internal index, since the input is sorted
                boundbxs[idx_int].alive = 0;
            }
        }
    }
}

void Ker_SSD_NMS(Ker_SSD_NMS_ArgT  *KerArg0 )
{
    
    int16_t bbox_idx_max = *(KerArg0->bbox_idx);
    int16_t bbox_max     = KerArg0->n_max_bb;
    int max_detections   = KerArg0->n_out_box;
    bbox_t * bbox        = KerArg0->bbox_buf;
    int16_t * out_bbox   = KerArg0->bbox_out;
    int8_t * scores_out  = KerArg0->scores_out;
    int8_t * class_out   = KerArg0->class_out;
    int non_max_thres  = (int) KerArg0->NMSThr;
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

    // for (int i=0; i<bbox_idx_max; i++) printf("box: %d %d %d %d - %d@%d \n", bbox[i].x,bbox[i].y,bbox[i].w,bbox[i].h, bbox[i].class, bbox[i].score);
    //Suppressing double detections
    // KerNonMaxSuppress(bbox, non_max_thres, bbox_idx_max);
    //BBOX value are in Q14 and non_max_threshold in Q14
    int idx, idx_int;
    //Non-max supression
    for(idx=0; idx<bbox_idx_max; idx++){
        //check if rect has been removed (-1)
        if(bbox[idx].alive == 0)
            continue;

        for(idx_int=idx+1; idx_int<bbox_idx_max; idx_int++){
            if((bbox[idx_int].alive==0 || idx_int==idx) || (bbox[idx_int].class != bbox[idx].class))
                continue;
            //check the intersection between rects
            int8_t iou = KerIoverU(bbox[idx].x,bbox[idx].y,bbox[idx].w,bbox[idx].h,
                                bbox[idx_int].x,bbox[idx_int].y,bbox[idx_int].w,bbox[idx_int].h, non_max_thres);
            if(iou){ //is non-max
                //supress the one that has lower score that is always the internal index, since the input is sorted
                bbox[idx_int].alive = 0;
            }
        }
    }

    //Applying max output from TFLITE
    int out_idx = 0;
    for(int i=0; i<bbox_idx_max&&out_idx<max_detections; i++){
        if (!bbox[i].alive) continue;
        out_bbox[4*out_idx]   = bbox[i].y;
        out_bbox[4*out_idx+1] = bbox[i].x;
        out_bbox[4*out_idx+2] = bbox[i].y + bbox[i].h;
        out_bbox[4*out_idx+3] = bbox[i].x + bbox[i].w;
        scores_out[out_idx] = bbox[i].score;
        class_out[out_idx] = bbox[i].class;
        out_idx++;
    }
    while (out_idx<max_detections) scores_out[out_idx++] = 0;

}
#pragma GCC diagnostic pop
