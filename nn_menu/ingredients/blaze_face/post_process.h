
#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__

#include "pmsis.h"
#include "Gap.h"
#define MAX_BB_OUT 50
//#define FLOAT_POST_PROCESS

#define POST_PROCESS_OUTPUT_REVERSED

#ifdef POST_PROCESS_OUTPUT_REVERSED

#define box_offset_y 		1
#define box_offset_x 		0
#define box_offset_height 	3
#define box_offset_width 	2
#define keypoint_offset_y 	1
#define keypoint_offset_x 	0

#else

#define box_offset_y 		0
#define box_offset_x 		1
#define box_offset_height 	2
#define box_offset_width 	3
#define keypoint_offset_y 	0
#define keypoint_offset_x 	1

#endif

#define keypoints_coord_offset 4

#define NON_MAX_THRES 0.5

#define Y_SCALE 128
#define X_SCALE 128
#define W_SCALE 128
#define H_SCALE 128

#define INV_X_SCALE_Q7 FP2FIX(0.0078125,7)
#define INV_Y_SCALE_Q7 FP2FIX(0.0078125,7)
#define INV_W_SCALE_Q7 FP2FIX(0.0078125,7)
#define INV_H_SCALE_Q7 FP2FIX(0.0078125,7)


typedef struct 
{
	int    xmin;
	int    ymin;
	int    w;
	int    h;
	float score;
	uint8_t alive;
	int k1_x; //Left eye
	int k1_y;
	int k2_x; //Right Eye
	int k2_y;
	int k3_x; // Nose
	int k3_y;
	int k4_x; // Mouth
	int k4_y;
	int k5_x; // Left Ear
	int k5_y;
	int k6_x; // Right Ear
	int k6_y;
}bbox_t;

void post_process(float* scores,float * boxes,bbox_t* bboxes,int img_w,int img_h, float thres);
void post_process_fix(int16_t* scores,int16_t * boxes,bbox_t* bboxes,int img_w,int img_h, int16_t thres);
#endif //__POST_PROCESS_H__