#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
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
#include "CNN_BasicKernels_fp16.h"
#include "SSD_BasicKernels_fp16.h"

#ifndef __EMUL__
    #define CL_CRITICAL_ENTER() pi_cl_team_critical_enter()
    #define CL_CRITICAL_EXIT()  pi_cl_team_critical_exit()
#else
    #define CL_CRITICAL_ENTER()
    #define CL_CRITICAL_EXIT()
#endif
// optimize the division to find the chunk size
// equivalent to ceil(KerArg0->W/rt_nb_pe())
inline static unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
    unsigned int NCore = gap_ncore();
    unsigned int Log2Core = gap_fl1(NCore);
    unsigned int Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
    return Chunk;
}

/*
    Uses exp(x) = lim (1 + x/n)^n   when n approaches infinite
    here we use n = 1024, x^1024 = (x^2)^512 = ... = ((((((((((x^2)^2)^2)^2)^2)^2)^2)^2)^2)^2)

    When x is small accuracy is pretty good, order of 10-3
*/
static F16 Fast_Exp_fp16(F16 x)

{
    x = (F16) 1.0 + x / (F16) 1024.0;
    x *= x; x *= x; x *= x; x *= x; x *= x; x *= x; x *= x; x *= x; x *= x; x *= x;
    return x;
}


void Ker_SSD_Init_f16(Ker_SSD_Init_Arg_f16_T  *KerArg0)
{    
    // Initialize the output bounding boxes
    *(KerArg0->bbox_idx) = 0;
    bbox_f16_t * bbox = KerArg0->bbox_buf;
    int n_max_bb  = KerArg0->n_max_bb;
    for (int i=0; i<n_max_bb; i++){
        bbox[i].score = 0;
        bbox[i].alive = 0;
    }
}

// The actual code that does the tile addition
void Ker_SSD_Decoder_fp16(Ker_SSD_Decoder_Arg_fp16_T  *KerArg0 )
{
    unsigned int CoreId = gap_coreid();
    unsigned int Chunk  = ChunkSize(KerArg0->H);
    unsigned int First  = CoreId*Chunk;
    unsigned int Last   = (First+Chunk > KerArg0->H) ? (KerArg0->H) : (First+Chunk);
    bbox_f16_t * bbox   = KerArg0->bbox_buf;
    F16 * scores        = KerArg0->classes_in;
    int num_classes     = KerArg0->N_Classes;
    short int n_max_bb  = KerArg0->n_max_bb;
    F16 score_th        = KerArg0->ScoreThr;
    int num_coords      = 4;
    int boxes_idx;
    F16 Ax, Aw, Ay, Ah, O;
    int bbn;

	for (unsigned int i = First; i<Last; i++)
    {
        boxes_idx = i*num_coords;
        for (unsigned int j=1; j<num_classes; j++){
            if((scores[(i*num_classes)+j]) > score_th){
                CL_CRITICAL_ENTER();
                bbn = KerArg0->bbox_idx[0]++;
                // printf("Core: %d\tbbox_idx:%d\n", CoreId, KerArg0->bbox_idx[0]);
                if(bbn > n_max_bb){ // check if we reched n_max_bb
                    KerArg0->bbox_idx[0]--;
                    CL_CRITICAL_EXIT();
                    goto exit_double_for;
                }
                CL_CRITICAL_EXIT();
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
                bbox[bbn].x = O*Aw + Ax;
                O = KerArg0->boxes_in[boxes_idx];
                Ay = KerArg0->anchors_in[boxes_idx];
                Ah = KerArg0->anchors_in[boxes_idx+2];
                bbox[bbn].y = O*Ah + Ay;
                // half_h, half_w --> Q14
                // half_h = exp(So*Off / params.h_scale) * Sa*A = Sa/So * exp(So/params.h_scale *O) * A =
                //          (scale_ao * (Aw * exp17.15(scale_h*O<<15-scale_hNorm))>>scale_aoNorm) =
                //          at_norm(scale_ao*(A*exp17.15(scale_h*O << 15-scale_hNorm)), scale_aoNorm)
                O = KerArg0->boxes_in[boxes_idx+3];
                bbox[bbn].w = Aw * Fast_Exp_fp16(O);
                O = KerArg0->boxes_in[boxes_idx+2];
                bbox[bbn].h = Ah * Fast_Exp_fp16(O);

                bbox[bbn].x = bbox[bbn].x - (bbox[bbn].w / 2);
                bbox[bbn].y = bbox[bbn].y - (bbox[bbn].h / 2);
            }
    	}
    }
    exit_double_for:
    gap_waitbarrier(0);
    if (CoreId == 0) KerArg0->bbox_idx[0]--;
}

static int16_t KerIoverU(F16 a_x, F16 a_y, F16 a_w, F16 a_h,
                         F16 b_x, F16 b_y, F16 b_w, F16 b_h,
                         float Thr)
{
    int intersect, runion;
    F16 x = MaxF(a_x,b_x);
    F16 y = MaxF(a_y,b_y); 

    F16 size_x = MinF(a_x+a_w,b_x+b_w) - x;
    F16 size_y = MinF(a_y+a_h,b_y+b_h) - y;

    if(size_x <= 0 || size_y <= 0){
        // no intersection
        return (Thr?0:1);
    }
    else
        intersect=(size_x*size_y); 

    runion = (a_w*a_h + b_w*b_h) - intersect;

    F16 iou = (F16) intersect / (F16) runion;
    return iou >= Thr;
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
            int iou = KerIoverU(boundbxs[idx].x,boundbxs[idx].y,boundbxs[idx].w,boundbxs[idx].h,
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
    
    short int bbox_idx_max = *(KerArg0->bbox_idx);
    short int bbox_max     = KerArg0->n_max_bb;
    int max_detections     = KerArg0->n_out_box;
    bbox_f16_t * bbox      = KerArg0->bbox_buf;
    F16 * out_bbox         = KerArg0->bbox_out;
    F16 * scores_out       = KerArg0->scores_out;
    F16 * class_out        = KerArg0->class_out;
    F16 non_max_thres      = (F16) KerArg0->NMSThr;
    bbox_f16_t temp;

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

    //Suppressing double detections
    KerNonMaxSuppress(bbox, non_max_thres, bbox_idx_max);

    //Applying max output from TFLITE
    int out_idx = 0;
    for(int i=0; i<bbox_idx_max, out_idx<max_detections; i++){
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
