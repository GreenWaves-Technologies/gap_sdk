/*
 * Copyright 2021 GreenWaves Technologies, SAS
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

#include <stdlib.h>
#include <math.h>
#include "face_detection_front.h"
#include "post_process.h"

/* Defines */
#define NUM_CLASSES 	2
#define AT_INPUT_SIZE 	(AT_INPUT_WIDTH*AT_INPUT_HEIGHT*AT_INPUT_COLORS)

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s 

typedef signed char NETWORK_OUT_TYPE;

signed char* scores_out;
signed char* boxes_out;

AT_HYPERFLASH_FS_EXT_ADDR_TYPE face_detection_front_L3_Flash = 0;
signed char Input_1[AT_INPUT_SIZE];
char *ImageName = NULL;

static void RunNetwork()
{
#ifdef PERF
	//printf("Start timer\n");
	gap_cl_starttimer();
	gap_cl_resethwtimer();
#endif
  printf("Running on cluster\n");
  face_detection_frontCNN(Input_1,scores_out,&(scores_out[512]),boxes_out,&(boxes_out[512*16]));
  printf("Runner completed\n");

}

void printBboxes_forPython(bbox_t *boundbxs){
	printf("\n\n======================================================");
	printf("\nThis can be copy-pasted to python to draw BoudingBoxs   ");
	printf("\n\n");

	for (int counter=0;counter< MAX_BB_OUT;counter++){
		if(boundbxs[counter].alive){
			printf("rect = patches.Rectangle((%d,%d),%d,%d,linewidth=1,edgecolor='r',facecolor='none')\nax.add_patch(rect)\n",
				boundbxs[counter].xmin,
				boundbxs[counter].ymin,
				boundbxs[counter].w,
				boundbxs[counter].h
				);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k1_x,
				boundbxs[counter].k1_y);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k2_x,
				boundbxs[counter].k2_y);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k3_x,
				boundbxs[counter].k3_y);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k4_x,
				boundbxs[counter].k4_y);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k5_x,
				boundbxs[counter].k5_y);
			printf("kp = patches.Circle((%d,%d),radius=1,color='green')\nax.add_patch(kp)\n",
				boundbxs[counter].k6_x,
				boundbxs[counter].k6_y);
		}
	}//
}

int rect_intersect_area( short a_x, short a_y, short a_w, short a_h,
                         short b_x, short b_y, short b_w, short b_h ){

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

void non_max_suppress(bbox_t * boundbxs){

    int idx,idx_int;

    //Non-max supression
     for(idx=0;idx<MAX_BB_OUT;idx++){
        //check if rect has been removed (-1)
        if(boundbxs[idx].alive==0)
            continue;

        for(idx_int=0;idx_int<MAX_BB_OUT;idx_int++){

            if(boundbxs[idx_int].alive==0 || idx_int==idx)
                continue;

            //check the intersection between rects
            int intersection = rect_intersect_area(boundbxs[idx].xmin,boundbxs[idx].ymin,boundbxs[idx].w,boundbxs[idx].h,
                                                   boundbxs[idx_int].xmin,boundbxs[idx_int].ymin,boundbxs[idx_int].w,boundbxs[idx_int].h);

            if(intersection >= NON_MAX_THRES){ //is non-max
                //supress the one that has lower score that is alway the internal index, since the input is sorted
                boundbxs[idx_int].alive =0;
            }
        }
    }
}


int checkResults(bbox_t *boundbxs){
    int totAliveBB=0;
    int x,y,w,h;

    for(int idx=0;idx<MAX_BB_OUT;idx++){
        if(boundbxs[idx].alive){
			totAliveBB++;
        	x = boundbxs[idx].xmin;
            y = boundbxs[idx].ymin;
            w = boundbxs[idx].w;
            h = boundbxs[idx].h;
        }
    }

    //Cabled check of result (not nice but effective) with +/- 3 px tollerance
    if(totAliveBB!=1) return -1;
    if( x > 12 + 1 || x < 12 - 1 )         return -1;
    if( y > 30 + 1 || y < 30 - 1 )         return -1;
    if( w > 46 + 1 || w < 46 - 1 )         return -1;
    if( h > 46 + 1 || h < 46 - 1 )         return -1;

    return 0;

}



int start()
{

	#ifndef __EMUL__
		boxes_out=pmsis_l2_malloc(sizeof(char)*(16*896));
		scores_out=pmsis_l2_malloc(sizeof(char)*(1*896));
	
		/*-----------------------OPEN THE CLUSTER--------------------------*/
		struct pi_device cluster_dev;
		struct pi_cluster_conf conf;
		pi_cluster_conf_init(&conf);
		pi_open_from_conf(&cluster_dev, (void *)&conf);
		pi_cluster_open(&cluster_dev);
	#else
		Output_1=malloc(sizeof(char)*(16*16*32+96*8*8));
		Output_2=malloc(sizeof(char)*(16*16*2+8*8*6));
	#endif

	printf("Reading image %s\n", ImageName);
	//Reading Image from Bridge
	if (ReadImageFromFile(ImageName, AT_INPUT_WIDTH, AT_INPUT_HEIGHT, AT_INPUT_COLORS, Input_1, AT_INPUT_SIZE*sizeof(char), IMGIO_OUTPUT_CHAR, 0)) {
	printf("Failed to load image %s\n", ImageName);
	return 1;
	}
	printf("Constructing Network\n");
	int err_construct = face_detection_frontCNN_Construct();
	if (err_construct){
		printf("Graph Constructor exited with error: %d\n", err_construct);
		return -1;
	}
	printf("Graph constructor was OK\n");

	#ifndef __EMUL__
		/*--------------------------TASK SETUP------------------------------*/
		struct pi_cluster_task *task_encoder = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
		if(task_encoder==NULL) {
			printf("pi_cluster_task alloc Error!\n");
			pmsis_exit(-1);
		}
		printf("Stack size is %d and %d\n",STACK_SIZE,SLAVE_STACK_SIZE );
		memset(task_encoder, 0, sizeof(struct pi_cluster_task));
		task_encoder->entry = &RunNetwork;
		task_encoder->stack_size = STACK_SIZE;
		task_encoder->slave_stack_size = SLAVE_STACK_SIZE;
		task_encoder->arg = NULL;
		pi_cluster_send_task_to_cl(&cluster_dev, task_encoder);
	#else
		RunNetwork();
	#endif

	#ifdef PERF
	{
		unsigned int TotalCycles = 0, TotalOper = 0;
		printf("\n");
		for (int i=0; i<(sizeof(AT_GraphPerf)/sizeof(unsigned int)); i++) {
			printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", AT_GraphNodeNames[i],
			       AT_GraphPerf[i], AT_GraphOperInfosNames[i], ((float) AT_GraphOperInfosNames[i])/ AT_GraphPerf[i]);
			TotalCycles += AT_GraphPerf[i]; TotalOper += AT_GraphOperInfosNames[i];
		}
		printf("\n");
		printf("%45s: Cycles: %10d, Operations: %10d, Operations/Cycle: %f\n", "Total", TotalCycles, TotalOper, ((float) TotalOper)/ TotalCycles);
		printf("\n");
	}
	#endif

    printf("Destructing Network\n");
	face_detection_frontCNN_Destruct();
	printf("Encoder destructed\n\n");
	
/* ------------------------------------------------------------------------- */

	#ifdef FLOAT_POST_PROCESS
	float *scores = pmsis_l2_malloc(896*sizeof(float));
	float *boxes  = pmsis_l2_malloc(16*896*sizeof(float));
	bbox_t* bboxes = pmsis_l2_malloc(MAX_BB_OUT*sizeof(bbox_t));

	if(scores==NULL || boxes==NULL || bboxes==NULL){
		printf("Alloc error\n");
		pmsis_exit(-1);
	}
	printf("\n");
	for(int i=0;i<896;i++){
		if(i<512)
			scores[i] = 1/(1+exp(-(((float)scores_out[i])*face_detection_front_Output_3_OUT_SCALE)));
		else
			scores[i] = 1/(1+exp(-(((float)scores_out[i])*face_detection_front_Output_4_OUT_SCALE)));

		for(int j=0;j<16;j++){
			if(i<512)
				boxes[(i*16)+j] = ((float)boxes_out[(i*16)+j])*(float)face_detection_front_Output_5_OUT_SCALE;
			else
				boxes[(i*16)+j] = ((float)boxes_out[(i*16)+j])*(float)face_detection_front_Output_6_OUT_SCALE;
		}	
	}

  	post_process(scores,boxes,bboxes,128,128, 0.5f);
  	
  	#else

	int16_t *scores = pmsis_l2_malloc(896*sizeof(int16_t));
	int16_t *boxes  = pmsis_l2_malloc(16*896*sizeof(int16_t));
	bbox_t* bboxes = pmsis_l2_malloc(MAX_BB_OUT*sizeof(bbox_t));

	if(scores==NULL || boxes==NULL || bboxes==NULL){
		printf("Alloc error\n");
		pmsis_exit(-1);
	}
	printf("\n");
	for(int i=0;i<896;i++){
		//Sigmoid input is Q12 and output Q15
		if(i<512)
			scores[i] = Sigmoid((((int)scores_out[i])*face_detection_front_Output_3_OUT_QSCALE)<< (12-face_detection_front_Output_3_OUT_QNORM));
		else
			scores[i] = Sigmoid((((int)scores_out[i])*face_detection_front_Output_4_OUT_QSCALE)<< (12-face_detection_front_Output_4_OUT_QNORM));

		for(int j=0;j<16;j++){
			if(i<512)
				boxes[(i*16)+j] = (((int)boxes_out[(i*16)+j])*face_detection_front_Output_5_OUT_QSCALE) << (8 - face_detection_front_Output_5_OUT_QNORM);
			else
				boxes[(i*16)+j] = (((int)boxes_out[(i*16)+j])*face_detection_front_Output_6_OUT_QSCALE) << (8 - face_detection_front_Output_6_OUT_QNORM);
		}
	}
  	//Now scores are Q15 and boxes Q8 
  	//In fixed point there is no need to pass image coords since the NN has been trained for 128x128
  	//[TODO] add scaling for different image sizes
	post_process_fix(scores,boxes,bboxes,128,128, FP2FIX(0.5,15));
	
  	#endif

  	non_max_suppress(bboxes);
  	printBboxes_forPython(bboxes);

  	//for(int i=0;i<MAX_BB_OUT;i++){
  	//	if (bboxes[i].alive)
  	//		printf("%f %d %d %d %d\n",bboxes[i].score, bboxes[i].xmin,bboxes[i].ymin,bboxes[i].w,bboxes[i].h);
  	//}

  	#ifdef FLOAT_POST_PROCESS
  	pmsis_l2_malloc_free(scores,896*sizeof(float));
  	pmsis_l2_malloc_free(boxes,16*896*sizeof(float));
	#else
  	pmsis_l2_malloc_free(scores,896*sizeof(int16_t));
  	pmsis_l2_malloc_free(boxes,16*896*sizeof(int16_t));
	#endif

	if(checkResults(bboxes)){
		printf("Output is not correct...\n");
		pmsis_exit(-1);
	}else{
		printf("Output correct!\n");
	}

	pmsis_l2_malloc_free(bboxes,MAX_BB_OUT*sizeof(bbox_t));

	#ifndef __EMUL__
		pmsis_l2_malloc_free(scores_out,sizeof(char)*(1*896));
		pmsis_l2_malloc_free(boxes_out,sizeof(char)*(16*896));

		pmsis_exit(0);
	#else
		free(Output_2);
		free(Output_1);
	#endif

	printf("Ended\n");
	return 0;
}

#ifdef __EMUL__
void main(int argc, char *argv[])
{
	if (argc < 2) {
	printf("Usage: %s [image_file]\n", argv[0]);
	exit(1);
	}
	ImageName = argv[1];
	start(NULL);
}
#else
void main()
{

#define __XSTR(__s) __STR(__s)
#define __STR(__s) #__s
    printf("\n\n\t *** NNTOOL MAIN APPL ***\n\n");
    ImageName=__XSTR(AT_IMAGE);
   	pmsis_kickoff((void *)start);
}
#endif
