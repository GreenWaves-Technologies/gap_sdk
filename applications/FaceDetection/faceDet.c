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

#include "faceDet.h"
#include "face_cascade.h"
#include "ImageDraw.h"

#undef DEBUG_PRINTF
#define DEBUG_PRINTF(...) ((void ) 0)

uint8_t * face_det_l1_memory;
uint32_t  l1_offset = 0;

static uint8_t* __l1_malloc_private(uint32_t size)
{
    uint32_t _offset = l1_offset;
    l1_offset += size;
    if (l1_offset > FACE_DETECT_L1_MEMORY_POOL_SIZE)
    {
        DEBUG_PRINTF("Problem of allocate L1, overflowed\n");
        return 0;
    }
    return (face_det_l1_memory + _offset);
}

static cascade_t *getFaceCascade(){

	cascade_t *face_cascade;

	face_cascade = (cascade_t*) __l1_malloc_private(sizeof(cascade_t));
	if(face_cascade==0){
		DEBUG_PRINTF("Error allocatin model thresholds...");
		return 0;
	}
	single_cascade_t  **model_stages = (single_cascade_t**) __l1_malloc_private(sizeof(single_cascade_t*)*CASCADE_TOTAL_STAGES);

	face_cascade->stages_num = CASCADE_TOTAL_STAGES;
	face_cascade->thresholds = (signed short *) __l1_malloc_private( sizeof(signed short )*face_cascade->stages_num);
	if(face_cascade->thresholds==0){
		DEBUG_PRINTF("Error allocatin model thresholds...");
		return 0;
	}

	for(int a=0;a<face_cascade->stages_num;a++){
		face_cascade->thresholds[a] = model_thresholds[a];
	}
		switch(CASCADE_TOTAL_STAGES){
		case 25:
			model_stages[24] = &stage_24;
			/* fall through */
		case 24:
			model_stages[23] = &stage_23;
			/* fall through */
		case 23:
			model_stages[22] = &stage_22;
			/* fall through */
		case 22:
			model_stages[21] = &stage_21;
			/* fall through */
		case 21:
			model_stages[20] = &stage_20;
			/* fall through */
		case 20:
			model_stages[19] = &stage_19;
			/* fall through */
		case 19:
			model_stages[18] = &stage_18;
			/* fall through */
		case 18:
			model_stages[17] = &stage_17;
			/* fall through */
		case 17:
			model_stages[16] = &stage_16;
			/* fall through */
		case 16:
			model_stages[15] = &stage_15;
			/* fall through */
		case 15:
			model_stages[14] = &stage_14;
			/* fall through */
		case 14:
			model_stages[13] = &stage_13;
			/* fall through */
		case 13:
			model_stages[12] = &stage_12;
			/* fall through */
		case 12:
			model_stages[11] = &stage_11;
			/* fall through */
		case 11:
			model_stages[10] = &stage_10;
			/* fall through */
		case 10:
			model_stages[9] = &stage_9;
			/* fall through */
		case 9:
			model_stages[8] = &stage_8;
			/* fall through */
		case 8:
			model_stages[7] = &stage_7;
			/* fall through */
		case 7:
			model_stages[6] = &stage_6;
			/* fall through */
		case 6:
			model_stages[5] = &stage_5;
			/* fall through */
		case 5:
			model_stages[4] = &stage_4;
			/* fall through */
		case 4:
			model_stages[3] = &stage_3;
			/* fall through */
		case 3:
			model_stages[2] = &stage_2;
			/* fall through */
		case 2:
			model_stages[1] = &stage_1;
			/* fall through */
		case 1:
			model_stages[0] = &stage_0;
			/* fall through */
		case 0:
			break;
	}
	face_cascade->stages = model_stages;
	return face_cascade;

}


static int biggest_cascade_stage(cascade_t *cascade){

	//Calculate cascade bigger layer
	int biggest_stage_size=0;
	int cur_layer;

	for (int i=0; i<cascade->stages_num; i++) {

		cur_layer = sizeof(cascade->stages[i]->stage_size) +
					sizeof(cascade->stages[i]->rectangles_size) +
					(cascade->stages[i]->stage_size*
						(sizeof(cascade->stages[i]->thresholds) +
						 sizeof(cascade->stages[i]->alpha1) +
						 sizeof(cascade->stages[i]->alpha2) +
						 sizeof(cascade->stages[i]->rect_num)
						 )
					) +
					(cascade->stages[i]->rectangles_size*sizeof(cascade->stages[i]->rectangles)) +
					((cascade->stages[i]->rectangles_size/4)*sizeof(cascade->stages[i]->weights));

		if(cur_layer>biggest_stage_size)
			biggest_stage_size=cur_layer;
		//DEBUG_PRINTF ("Stage size: %d\n",cur_layer);
	}

	return biggest_stage_size;
}



//Permanently Store a scascade stage to L1
static single_cascade_t* sync_copy_cascade_stage_to_l1(single_cascade_t* cascade_l2){

	pi_cl_dma_copy_t DmaR_Evt1;

	single_cascade_t* cascade_l1;
	cascade_l1 = (single_cascade_t* )__l1_malloc_private( sizeof(single_cascade_t));

	cascade_l1->stage_size = cascade_l2->stage_size;
	cascade_l1->rectangles_size = cascade_l2->rectangles_size;


	cascade_l1->thresholds     = (short*)__l1_malloc_private(sizeof(short)*cascade_l2->stage_size);
	__cl_dma_memcpy((unsigned int) cascade_l2->thresholds, (unsigned int) cascade_l1->thresholds, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);


	cascade_l1->alpha1         = (short*)__l1_malloc_private( sizeof(short)*cascade_l2->stage_size);
	__cl_dma_memcpy((unsigned int) cascade_l2->alpha1, (unsigned int) cascade_l1->alpha1, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);

	cascade_l1->alpha2         = (short*)__l1_malloc_private( sizeof(short)*cascade_l2->stage_size);
	__cl_dma_memcpy((unsigned int) cascade_l2->alpha2, (unsigned int) cascade_l1->alpha2, sizeof(short)*cascade_l1->stage_size, PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);

	cascade_l1->rect_num       = (unsigned  short*)__l1_malloc_private( sizeof(unsigned short)*((cascade_l2->stage_size)+1));
	__cl_dma_memcpy((unsigned int) cascade_l2->rect_num, (unsigned int) cascade_l1->rect_num, sizeof(unsigned short)*(cascade_l1->stage_size+1), PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);


	cascade_l1->weights    = (signed char*)__l1_malloc_private( sizeof(signed char)*(cascade_l2->rectangles_size/4));
	__cl_dma_memcpy((unsigned int) cascade_l2->weights, (unsigned int) cascade_l1->weights, sizeof(signed char)*(cascade_l2->rectangles_size/4), PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);


	cascade_l1->rectangles = (char*)__l1_malloc_private( sizeof(char)*cascade_l2->rectangles_size);
	__cl_dma_memcpy((unsigned int) cascade_l2->rectangles, (unsigned int) cascade_l1->rectangles, sizeof(char)*cascade_l2->rectangles_size, PI_CL_DMA_DIR_EXT2LOC, 0, &DmaR_Evt1);
	pi_cl_dma_wait(&DmaR_Evt1);

	if(cascade_l1->rectangles==0)
		DEBUG_PRINTF("Allocation Error...\n");

	return cascade_l1;
}



static int rect_intersect_area( unsigned short a_x, unsigned short a_y, unsigned short a_w, unsigned short a_h,
						 unsigned short b_x, unsigned short b_y, unsigned short b_w, unsigned short b_h ){

	#define MIN(a,b) ((a) < (b) ? (a) : (b))
    #define MAX(a,b) ((a) > (b) ? (a) : (b))

    int x = MAX(a_x,b_x);
    int y = MAX(a_y,b_y);

    int size_x = MIN(a_x+a_w,b_x+b_w) - x;
    int size_y = MIN(a_y+a_h,b_y+b_h) - y;

    if(size_x <=0 || size_x <=0)
        return 0;
    else
        return size_x*size_y;

    #undef MAX
    #undef MIN
}


static void non_max_suppress(cascade_reponse_t* reponses, int reponse_idx){

	int idx,idx_int;

    //Non-max supression
     for(idx=0;idx<reponse_idx;idx++){
        //check if rect has been removed (-1)
        if(reponses[idx].x==-1)
            continue;

        for(idx_int=0;idx_int<reponse_idx;idx_int++){

            if(reponses[idx_int].x==-1 || idx_int==idx)
                continue;

            //check the intersection between rects
            int intersection = rect_intersect_area(reponses[idx].x,reponses[idx].y,reponses[idx].w,reponses[idx].h,
               									   reponses[idx_int].x,reponses[idx_int].y,reponses[idx_int].w,reponses[idx_int].h);

            if(intersection >= NON_MAX_THRES){ //is non-max
                //supress the one that has lower score
                if(reponses[idx_int].score > reponses[idx].score){
                    reponses[idx].x = -1;
                    reponses[idx].y = -1;
                }
                else{
                    reponses[idx_int].x = -1;
                    reponses[idx_int].y = -1;
                }
            }
        }
    }
}

void faceDet_cluster_init(ArgCluster_T *ArgC){

	//DEBUG_PRINTF ("Cluster Init start\n");

    face_det_l1_memory = (uint8_t *) pmsis_l1_malloc(FACE_DETECT_L1_MEMORY_POOL_SIZE);
    if (!face_det_l1_memory) {
		DEBUG_PRINTF("Failed to allocate L1 memory pool\n");
        return ;
	}

	FaceDet_L1_Memory = (char *) __l1_malloc_private(_FaceDet_L1_Memory_SIZE);
	if (FaceDet_L1_Memory == 0) {
		DEBUG_PRINTF("Failed to allocate %d bytes for L1_memory\n", _FaceDet_L1_Memory_SIZE); return ;
	}

	ArgC->output_map = pmsis_l2_malloc(sizeof(unsigned int)*(ArgC->Hout-24+1)*(ArgC->Wout-24+1));

	if(ArgC->output_map==0){
		DEBUG_PRINTF("Error Allocating output buffer...");
		return;
	}
	//Get Cascade Model
	ArgC->model = getFaceCascade();

	int max_cascade_size = biggest_cascade_stage(ArgC->model);
	//DEBUG_PRINTF("Max cascade size:%d\n",max_cascade_size);

	for(int i=0;i<CASCADE_STAGES_L1;i++)
		ArgC->model->stages[i] = sync_copy_cascade_stage_to_l1((ArgC->model->stages[i]));

	//Assigning space to cascade buffers

	ArgC->model->buffers_l1[0] = (single_cascade_t *)__l1_malloc_private(max_cascade_size);
	ArgC->model->buffers_l1[1] = (single_cascade_t *)__l1_malloc_private(max_cascade_size);



	if(ArgC->model->buffers_l1[0]==0 ){
		DEBUG_PRINTF("Error allocating cascade buffer 0...\n");
	}
	if(
	ArgC->model->buffers_l1[1] == 0){
		DEBUG_PRINTF("Error allocating cascade buffer 1...\n");
	}

	ArgC->reponses = (cascade_reponse_t*) pmsis_l2_malloc(sizeof(cascade_reponse_t)*MAX_NUM_OUT_WINS);

}


void faceDet_cluster_deinit(ArgCluster_T *ArgC)
{
    pmsis_l2_malloc_free(ArgC->reponses, sizeof(cascade_reponse_t)*MAX_NUM_OUT_WINS);
    pmsis_l2_malloc_free(ArgC->output_map, sizeof(unsigned int)*(ArgC->Hout-24+1)*(ArgC->Wout-24+1));
    pmsis_l1_malloc_free(face_det_l1_memory, FACE_DETECT_L1_MEMORY_POOL_SIZE);
}


void faceDet_cluster_main(ArgCluster_T *ArgC)
{
	DEBUG_PRINTF ("Cluster master start\n");

	int Win=ArgC->Win, Hin=ArgC->Hin;
	int Wout, Hout;

	unsigned int i, MaxCore = gap_ncore(),Ti;

	//create structure for output
	cascade_reponse_t* reponses = ArgC->reponses;
	int reponse_idx = 0;

	Wout=64;
	Hout=48;

	int result;

    // TODO: Perf api haven't been implemented yet.
    //gap8_resethwtimer();
    Ti = 0; //gap8_readhwtimer();


	//Init image windows
	for(int i=0;i<MAX_NUM_OUT_WINS;i++)
		reponses[i].x=-1;

#ifdef ENABLE_LAYER_1
	ResizeImage_1(ArgC->ImageIn,ArgC->ImageOut);
	ProcessIntegralImage_1(ArgC->ImageOut,ArgC->ImageIntegral);
	ProcessSquaredIntegralImage_1(ArgC->ImageOut,ArgC->SquaredImageIntegral);
	ProcessCascade_1(ArgC->ImageIntegral,ArgC->SquaredImageIntegral,ArgC->model, ArgC->output_map);

	for(int i=0;i<Hout-24+1;i+=DETECT_STRIDE)
		for(int j=0;j<Wout-24+1;j+=DETECT_STRIDE){

			result = ArgC->output_map[i*(Wout-24+1)+j];

			if(result!=0){
				reponses[reponse_idx].x       = (j*Win)/Wout;
				reponses[reponse_idx].y       = (i*Hin)/Hout;
				reponses[reponse_idx].w = (24*Win)/Wout;
				reponses[reponse_idx].h = (24*Hin)/Hout;
				reponses[reponse_idx].score   = result;
				reponse_idx++;
					//DEBUG_PRINTF("Face Found in %dx%d at X: %d, Y: %d - value: %d\n",Wout,Hout,j,i,result);
			}
	}
#endif

	Wout /= 1.25, Hout /= 1.25;

#ifdef ENABLE_LAYER_2
	ResizeImage_2(ArgC->ImageIn,ArgC->ImageOut);
	ProcessIntegralImage_2(ArgC->ImageOut,ArgC->ImageIntegral);
	ProcessSquaredIntegralImage_2(ArgC->ImageOut,ArgC->SquaredImageIntegral);
	ProcessCascade_2(ArgC->ImageIntegral,ArgC->SquaredImageIntegral,ArgC->model, ArgC->output_map);

	for(int i=0;i<Hout-24+1;i+=DETECT_STRIDE)
		for(int j=0;j<Wout-24+1;j+=DETECT_STRIDE){

			result = ArgC->output_map[i*(Wout-24+1)+j];

			if(result!=0){
				reponses[reponse_idx].x     = (j*Win)/Wout;
				reponses[reponse_idx].y     = (i*Hin)/Hout;
				reponses[reponse_idx].w 	= (24*Win)/Wout;
				reponses[reponse_idx].h 	= (24*Hin)/Hout;
				reponses[reponse_idx].score = result;
				reponse_idx++;
					//DEBUG_PRINTF("Face Found in %dx%d at X: %d, Y: %d - value: %d\n",Wout,Hout,j,i,result);
			}
	}
#endif

	Wout /= 1.25, Hout /= 1.25;

#ifdef ENABLE_LAYER_3
	ResizeImage_3(ArgC->ImageIn,ArgC->ImageOut);
	ProcessIntegralImage_3(ArgC->ImageOut,ArgC->ImageIntegral);
	ProcessSquaredIntegralImage_3(ArgC->ImageOut,ArgC->SquaredImageIntegral);
	ProcessCascade_3(ArgC->ImageIntegral,ArgC->SquaredImageIntegral,ArgC->model, ArgC->output_map);
	for(int i=0;i<Hout-24+1;i+=DETECT_STRIDE)
		for(int j=0;j<Wout-24+1;j+=DETECT_STRIDE){

			result = ArgC->output_map[i*(Wout-24+1)+j];

			if(result!=0){
				reponses[reponse_idx].x     = (j*Win)/Wout;
				reponses[reponse_idx].y     = (i*Hin)/Hout;
				reponses[reponse_idx].w 	= (24*Win)/Wout;
				reponses[reponse_idx].h 	= (24*Hin)/Hout;
				reponses[reponse_idx].score = result;
				reponse_idx++;
					//DEBUG_PRINTF("Face Found in %dx%d at X: %d, Y: %d - value: %d\n",Wout,Hout,j,i,result);
			}
	}

#endif

	non_max_suppress(reponses,reponse_idx);

    // TODO: reset when perf api done
    ArgC->cycles = 0; //gap8_readhwtimer() - Ti;
    //DEBUG_PRINTF("Test Face Detection CNN Cluster cycles: %d\n", Ti);


	ArgC->num_reponse=reponse_idx;
	int detected_faces = 0;
	for (int i=0; i<reponse_idx; i++)
            if (reponses[i].x!=-1)
            {
            	detected_faces++;
                //printf("Found a face: ");
                //printf("X: %d Y: %d W: %d H: %d\n",reponses[i].x,reponses[i].y,reponses[i].w,reponses[i].h);
                DrawRectangle(ArgC->ImageIn, Hin, Win,  reponses[i].x, reponses[i].y, reponses[i].w, reponses[i].h, 0);
                DrawRectangle(ArgC->ImageIn, Hin, Win,  reponses[i].x-1, reponses[i].y-1, reponses[i].w+2, reponses[i].h+2, 0);
                DrawRectangle(ArgC->ImageIn, Hin, Win,  reponses[i].x-2, reponses[i].y-2, reponses[i].w+4, reponses[i].h+4, 0);
                DrawRectangle(ArgC->ImageIn, Hin, Win,  reponses[i].x-3, reponses[i].y-3, reponses[i].w+6, reponses[i].h+6, 0);
                DrawRectangle(ArgC->ImageIn, Hin, Win,  reponses[i].x-4, reponses[i].y-4, reponses[i].w+8, reponses[i].h+8, 0);
                //real_detections++;
            }
    ArgC->num_reponse=detected_faces;
	final_resize(ArgC->ImageIn,ArgC->ImageOut);
}


