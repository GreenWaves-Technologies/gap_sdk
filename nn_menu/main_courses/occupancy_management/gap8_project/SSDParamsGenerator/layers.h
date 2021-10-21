/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#ifndef LAYERS_H_
#define LAYERS_H_

#include "layers.h"
#include <stdbool.h>
#include <inttypes.h>

/* min, max and abs functions */
#define inMIN(a, b) ( ((a) < (b)) ? (a) :  (b) )
#define inMAX(a, b) ( ((a) > (b)) ? (a) :  (b) )
#define inABS(a)    ( ((a) >  0 ) ? (a) : -(a) )

// a structure including parameters settings for final predictions
typedef struct {
	int n_bboxes;
	int top_k;
	float iou_thr;
	int n_classes;
	int background_class;
	int num_kepts;
} dectinfo_t;

// a structure including bboxes info
typedef struct{
	float x;
	float y;
	float w;
	float h;
	float score;
	int class;
}bbox_t;

//With and height of default anchors
typedef struct {
	float w;
	float h;
}anchorWH_t;


// anchors layer parameters
typedef struct {
	int n_classes;
	uint8_t feature_map_width;
	uint8_t feature_map_height;
	uint8_t img_height;
	uint8_t img_width;
	float this_scale;
	float next_scale;
	float step_height;
	float step_width;
	float offset_height;
	float offset_width;
	float *aspect_ratios;
	uint8_t n_aspect_ratios;
	uint8_t anchor_params;
	float *variances;
	uint8_t n_variances;
	bool two_boxes_for_ar1;
	uint8_t n_anchors;
	anchorWH_t* anchorsWH;
	int max_bboxes;
	bbox_t* bboxes;
	int* samples_per_class;
 	int n_confidence;
 	float confidence_thr;
 	float iou_thr;
 	int max_candidates_per_class;
 	int n_decoded_acnhors;
}Alps;


// initialize the anchor layer
void Anchor_layer_init(Alps * anchors);

#endif /* LAYERS_H_ */
