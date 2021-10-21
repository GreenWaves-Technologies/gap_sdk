/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */




/* Operations for simplifications */

#include "layers.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */
//#include <stdio.h>


// TODO:these sections should be moved to common header
#define malloc2(size) malloc(size)
#define malloc2_nofree(size) malloc(size)
#define malloc2_noccm(size) malloc(size)
#define free2(ptr) free(ptr)


void Anchor_layer_init(Alps *anchors){

	// If 'step' and 'offset' of width and height are not assigned, automatically calculte them for each layer.
	if (isnan(anchors->step_height)){anchors->step_height = (float)anchors->img_height / (float)anchors->feature_map_height;};
	if (isnan(anchors->step_width)){anchors->step_width  = (float)anchors->img_width  / (float)anchors->feature_map_width;};
	if (isnan(anchors->offset_height)){anchors->offset_height = 0.5;}
	if (isnan(anchors->offset_width)){anchors->offset_width = 0.5;};
	anchors->n_decoded_acnhors = 0;

	float size = (float)inMIN(anchors->img_height, anchors->img_width);

	// Number of default anchors for the current layer
	anchors->n_anchors=anchors->n_aspect_ratios;
	for (int i =0;i< anchors->n_aspect_ratios; i++){
		if ((anchors->aspect_ratios[i]==1.) & (anchors->two_boxes_for_ar1==true)){
			anchors->n_anchors+=1;}}

	anchors->anchorsWH = (anchorWH_t*)malloc(sizeof(anchorWH_t)*anchors->n_anchors);
	anchors->bboxes    = (bbox_t*)malloc(sizeof(bbox_t)*anchors->max_bboxes);
	anchors->samples_per_class = (int*)malloc(sizeof(int)*anchors->n_classes);
	// TODO: add memset for initialization

	 //calculate width and height of anchors according to the parameters of current layer
	int counter = 0;
	float w,h;
	anchorWH_t temp_anchorWH;
	for (int i =0;i< anchors->n_aspect_ratios; i++){
		if (anchors->aspect_ratios[i]==1.){
			w = size * anchors->this_scale;
			h = size * anchors->this_scale;
//			printf("\nWH: %f  %f\n", w,h);
			temp_anchorWH.h = h;
			temp_anchorWH.w = w;
			anchors->anchorsWH[counter] = temp_anchorWH;
			counter++;
			if (anchors->two_boxes_for_ar1==true){
				w = size  * sqrt(anchors->this_scale * anchors->next_scale);
				h = size * sqrt(anchors->this_scale * anchors->next_scale);
//				printf("\nWH: %f  %f\n", w,h);
				temp_anchorWH.h = h;
				temp_anchorWH.w = w;
				anchors->anchorsWH[counter] = temp_anchorWH;
				counter++;
			}//end if

		}else{
			w = anchors->this_scale * size  * sqrt(anchors->aspect_ratios[i]);
			h = anchors->this_scale * size  / sqrt(anchors->aspect_ratios[i]);
//			printf("\nWH: %f  %f\n", w,h);
			temp_anchorWH.h = h;
			temp_anchorWH.w = w;
			anchors->anchorsWH[counter] = temp_anchorWH;
			counter++;
		}//end_if
	}// end for
};

