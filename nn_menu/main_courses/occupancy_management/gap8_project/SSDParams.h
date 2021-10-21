#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include <Gap.h>
typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
	int16_t score;
	uint16_t class;
	uint8_t alive;
}bbox_t;
typedef struct{
	float x;
	float y;
	float w;
	float h;
	int16_t score;
	uint16_t class;
	uint8_t alive;
}bbox_fp_t;
typedef struct{
	bbox_t * bbs;
	int16_t num_bb;
}bboxs_t;
typedef struct{
	bbox_fp_t * bbs;
	int16_t num_bb;
}bboxs_fp_t;
typedef struct {
	float w;
	float h;
}anchorWH_t;


typedef struct {
	uint16_t n_classes;
	uint8_t  feature_map_width;
	uint8_t  feature_map_height;
	uint16_t img_height;
	uint16_t img_width;
	float this_scale;
	float next_scale;
	float step_height;
	float step_width;
	float offset_height;
	float offset_width;
	float *aspect_ratios;
	uint8_t n_aspect_ratios;
	uint8_t anchor_params; 
	float * variances; 
	uint8_t n_variances; 
	bool two_boxes_for_ar1; 
	uint8_t n_anchors; 
	anchorWH_t* anchorsWH; 
	int* samples_per_class; 
	short int confidence_thr; 
	float iou_thr; 
}Alps;
