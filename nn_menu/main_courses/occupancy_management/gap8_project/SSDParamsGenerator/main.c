/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> //memcpy

#include "layers.h"

#define FIX2FP(Val, Precision)						((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)						((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)						((int)((Val)*((1 << (Precision))-1)))


char SourceOut[]="../SSDParams.c";
char HeaderOut[]="../SSDParams.h";

#define CONFIDENCE_THR 0.45

int Q_calc(float min,float max){

	int integ = 1;
	int fract = 15;
	float min_fp = -pow(2,integ-1); 
	float max_fp =  pow(2,integ-1)-pow(2,-fract);		
	while (min < min_fp || max > max_fp){
		integ++;
		fract--;
		min_fp = -pow(2,integ-1); 
		max_fp =  pow(2,integ-1)-pow(2,-fract);
	}
	//printf("Layer %15s : min %10f max %10f -> Suggest Q%d\n",  layerName,min,max,fract);
	return fract;
}


void Generate_common_declarations(FILE * sf, FILE * hf){

	fprintf(hf,"#include <stdbool.h>\n");
	fprintf(hf,"#include <inttypes.h>\n");
	fprintf(hf,"#include <math.h>\n");
	fprintf(hf,"#include <Gap.h>\n");


	// a structure including bboxes info
	fprintf(hf,"typedef struct{\n");
	fprintf(hf,"\tuint32_t x;\n");
	fprintf(hf,"\tuint32_t y;\n");
	fprintf(hf,"\tuint32_t w;\n");
	fprintf(hf,"\tuint32_t h;\n");
	fprintf(hf,"\tint16_t score;\n");
	fprintf(hf,"\tuint16_t class;\n");
	fprintf(hf,"\tuint8_t alive;\n");
	fprintf(hf,"}bbox_t;\n");

	fprintf(hf,"typedef struct{\n");
	fprintf(hf,"\tfloat x;\n");
	fprintf(hf,"\tfloat y;\n");
	fprintf(hf,"\tfloat w;\n");
	fprintf(hf,"\tfloat h;\n");
	fprintf(hf,"\tint16_t score;\n");
	fprintf(hf,"\tuint16_t class;\n");
	fprintf(hf,"\tuint8_t alive;\n");
	fprintf(hf,"}bbox_fp_t;\n");

	fprintf(hf,"typedef struct{\n");
	fprintf(hf,"\tbbox_t * bbs;\n");
	fprintf(hf,"\tint16_t num_bb;\n");
	fprintf(hf,"}bboxs_t;\n");

	fprintf(hf,"typedef struct{\n");
	fprintf(hf,"\tbbox_fp_t * bbs;\n");
	fprintf(hf,"\tint16_t num_bb;\n");
	fprintf(hf,"}bboxs_fp_t;\n");

	// with and height of default anchors
    fprintf(hf,"typedef struct {\n");
    fprintf(hf,"\tfloat w;\n");
    fprintf(hf,"\tfloat h;\n");
    fprintf(hf,"}anchorWH_t;\n");

	fprintf(hf,"\n\n");    
    
    fprintf(hf,"typedef struct {\n");
    fprintf(hf,"\tuint16_t n_classes;\n");
    fprintf(hf,"\tuint8_t  feature_map_width;\n");
    fprintf(hf,"\tuint8_t  feature_map_height;\n");
    fprintf(hf,"\tuint16_t img_height;\n");
    fprintf(hf,"\tuint16_t img_width;\n");
    fprintf(hf,"\tfloat this_scale;\n");
    fprintf(hf,"\tfloat next_scale;\n");
    fprintf(hf,"\tfloat step_height;\n");
    fprintf(hf,"\tfloat step_width;\n");
    fprintf(hf,"\tfloat offset_height;\n");
    fprintf(hf,"\tfloat offset_width;\n");
    fprintf(hf,"\tfloat *aspect_ratios;\n");
    fprintf(hf,"\tuint8_t n_aspect_ratios;\n");

	fprintf(hf,"\tuint8_t anchor_params; \n");
	fprintf(hf,"\tfloat * variances; \n");
	fprintf(hf,"\tuint8_t n_variances; \n");
	fprintf(hf,"\tbool two_boxes_for_ar1; \n");
	fprintf(hf,"\tuint8_t n_anchors; \n");
	fprintf(hf,"\tanchorWH_t* anchorsWH; \n");
	fprintf(hf,"\tint* samples_per_class; \n");
	fprintf(hf,"\tshort int confidence_thr; \n");
	fprintf(hf,"\tfloat iou_thr; \n");

	fprintf(hf,"}Alps;\n");

	fprintf(sf,"#include \"SSDParams.h\"\n");

}

void Generate_anchor_layer_output(FILE * sf, FILE * hf, Alps * anchor_layer,int postFIX){

	fprintf(sf,"\n\n\n");

	fprintf(sf,"PI_L2 Alps * anchor_layer_%d;\n",postFIX);

	//print Aspect Ratios
	fprintf(sf,"PI_L2 float n_aspect_ratios_%d[]= {",postFIX);
	for(int i=0;i<anchor_layer->n_aspect_ratios;i++){
		fprintf(sf,"%f,",anchor_layer->aspect_ratios[i]);
	}
	fprintf(sf,"};\n");

	//Print variances
	fprintf(sf,"PI_L2 float n_variances_%d[]={",postFIX);

	for(int i=0;i<anchor_layer->n_variances;i++){
		fprintf(sf,"%f,",anchor_layer->variances[i]);
	}
	fprintf(sf,"};\n");

	//Print anchorWH_t
	fprintf(sf,"PI_L2 anchorWH_t anchorWH_%d[]={\n",postFIX);
	//TODO CHeck weather this <= is formally correct
	for(int i=0;i<anchor_layer->n_anchors;i++){
		fprintf(sf,"{%f,%f},\n",anchor_layer->anchorsWH[i].w,anchor_layer->anchorsWH[i].h);
	}
	fprintf(sf,"};\n");


	//Generate INIT SOURCE CODE

	fprintf(sf,"int initAnchorLayer_%d(){\n",postFIX);


	fprintf(sf,"\tanchor_layer_%d = (Alps*) pmsis_l2_malloc(sizeof(Alps));\n",postFIX);
    fprintf(sf,"\tif(anchor_layer_%d==NULL) return 1;\n",postFIX);
    fprintf(sf,"\tanchor_layer_%d->feature_map_width  = %d;\n",postFIX,anchor_layer->feature_map_width);
    fprintf(sf,"\tanchor_layer_%d->feature_map_height = %d;\n",postFIX,anchor_layer->feature_map_height);
    fprintf(sf,"\tanchor_layer_%d->n_classes  = %d;\n",postFIX,anchor_layer->n_classes);
    fprintf(sf,"\tanchor_layer_%d->img_width  = %d;\n",postFIX,anchor_layer->img_width);
    fprintf(sf,"\tanchor_layer_%d->img_height = %d;\n",postFIX,anchor_layer->img_height);
    fprintf(sf,"\tanchor_layer_%d->this_scale = %f;\n",postFIX,anchor_layer->this_scale);
    fprintf(sf,"\tanchor_layer_%d->next_scale = %f;\n",postFIX,anchor_layer->next_scale);
    fprintf(sf,"\tanchor_layer_%d->anchor_params = %d;\n",postFIX,anchor_layer->anchor_params);
    fprintf(sf,"\tanchor_layer_%d->iou_thr = %f;\n",postFIX,anchor_layer->iou_thr);
    fprintf(sf,"\tanchor_layer_%d->step_height= %f;\n",postFIX,anchor_layer->step_height);
    fprintf(sf,"\tanchor_layer_%d->step_width = %f;\n",postFIX,anchor_layer->step_width);
    fprintf(sf,"\tanchor_layer_%d->offset_height = %f;\n",postFIX,anchor_layer->offset_height);
    fprintf(sf,"\tanchor_layer_%d->offset_width  = %f;\n",postFIX,anchor_layer->offset_height);
    fprintf(sf,"\tanchor_layer_%d->aspect_ratios = n_aspect_ratios_%d;\n",postFIX,postFIX);
    fprintf(sf,"\tanchor_layer_%d->variances = n_variances_%d;\n",postFIX,postFIX);
    fprintf(sf,"\tanchor_layer_%d->two_boxes_for_ar1 = true;\n",postFIX);
    //This is always convertend into Q15 since it uses the output of softmax which is always Q15
    fprintf(sf,"\tanchor_layer_%d->confidence_thr = %d;\n",postFIX,FP2FIX(anchor_layer->confidence_thr,15));
    fprintf(sf,"\tanchor_layer_%d->n_aspect_ratios =  %d;\n",postFIX,anchor_layer->n_aspect_ratios);
    fprintf(sf,"\tanchor_layer_%d->n_anchors   =  %d;\n",postFIX,anchor_layer->n_anchors);    
    fprintf(sf,"\tanchor_layer_%d->n_variances =  %d;\n",postFIX,anchor_layer->n_variances);    
    fprintf(sf,"\tanchor_layer_%d->anchorsWH = anchorWH_%d;\n",postFIX,postFIX);
    fprintf(sf,"}\n");
    fprintf(sf,"\n");


}



//Floating Point Version
void GenerateAnchorsLayer(){

	FILE * source_file = fopen(SourceOut,"w");
	FILE * header_file = fopen(HeaderOut,"w");

	Generate_common_declarations(source_file,header_file);

	//Output Files Open
 	Alps * anchors_layer_2 = NULL;
 	anchors_layer_2 = malloc(sizeof(Alps));
 	float n_aspect_ratios_2[]= {1./4., 1./3., 1./2., 1., 2., 3., 4.};
 	float n_variances_2[]={0.1, 0.1, 0.1, 0.1};
 	anchors_layer_2->feature_map_width  = 40;
 	anchors_layer_2->feature_map_height = 40;
 	anchors_layer_2->n_classes  = 2;
 	anchors_layer_2->img_width  = 80;
 	anchors_layer_2->img_height = 80;
 	anchors_layer_2->this_scale = 0.05;
 	anchors_layer_2->next_scale = 0.15;
 	anchors_layer_2->anchor_params = 4;
 	anchors_layer_2->iou_thr = 0.1;
 	anchors_layer_2->step_height= NAN;        // Automatic assignment
 	anchors_layer_2->step_width = NAN;        // Automatic assignment
 	anchors_layer_2->offset_height = NAN;     // Automatic assignment
 	anchors_layer_2->offset_width  = NAN;     // Automatic assignment
 	anchors_layer_2->aspect_ratios = n_aspect_ratios_2;
 	anchors_layer_2->variances = n_variances_2;
 	anchors_layer_2->two_boxes_for_ar1 = true;
 	anchors_layer_2->max_bboxes = 20;
 	anchors_layer_2->n_confidence = 0;
 	anchors_layer_2->confidence_thr = CONFIDENCE_THR;           // it can be changed, more specifically after performance analysis.
 	anchors_layer_2->max_candidates_per_class = 20;  // it can be changed, more specifically after performance analysis and also for power consumption.
 	anchors_layer_2->n_aspect_ratios =	sizeof(n_aspect_ratios_2)/sizeof(n_aspect_ratios_2[0]);
 	anchors_layer_2->n_variances     =	sizeof(n_variances_2)/sizeof(n_variances_2[0]);
 	Anchor_layer_init(anchors_layer_2);             // initialize the layer with the information provided


	Generate_anchor_layer_output(source_file,header_file, anchors_layer_2, 2);
 	

	////////////////////////
	// Second anchor layer //
	////////////////////////
	// parameters of the first layer of anchors
 	// Alps: a structure containing all anchors layer info
	Alps * anchors_layer_3 = NULL;
	anchors_layer_3 = malloc(sizeof(Alps));
	float n_aspect_ratios_3[]= {1./4., 1./3., 1./2., 1., 2., 3., 4.};
	float n_variances_3[]={0.1, 0.1, 0.1, 0.1};
	anchors_layer_3->feature_map_width  = 20;
	anchors_layer_3->feature_map_height = 20;
	anchors_layer_3->n_classes  = 2;
	anchors_layer_3->img_width  = 80;
	anchors_layer_3->img_height = 80;
	anchors_layer_3->this_scale = 0.15;
	anchors_layer_3->next_scale = 0.3;
	anchors_layer_3->anchor_params = 4;
	anchors_layer_3->iou_thr = 0.1;
	anchors_layer_3->step_height= NAN;        // Automatic assignment
	anchors_layer_3->step_width = NAN;        // Automatic assignment
	anchors_layer_3->offset_height = NAN;     // Automatic assignment
	anchors_layer_3->offset_width  = NAN;     // Automatic assignment
	anchors_layer_3->aspect_ratios = n_aspect_ratios_3;
	anchors_layer_3->variances = n_variances_3;
	anchors_layer_3->two_boxes_for_ar1 = true;
	anchors_layer_3->max_bboxes = 20;
	anchors_layer_3->n_confidence = 0;
	anchors_layer_3->confidence_thr = CONFIDENCE_THR;           // it can be changed, more specifically after performance analysis.
	anchors_layer_3->max_candidates_per_class = 20;  // it can be changed, more specifically after performance analysis and also for power consumption.
	anchors_layer_3->n_aspect_ratios =	sizeof(n_aspect_ratios_3)/sizeof(n_aspect_ratios_3[0]);
	anchors_layer_3->n_variances     =	sizeof(n_variances_3)/sizeof(n_variances_3[0]);
	Anchor_layer_init(anchors_layer_3);             // initialize the layer with the information provided

	Generate_anchor_layer_output(source_file,header_file, anchors_layer_3, 3);

	////////////////////////
	// Third anchor layer //
	////////////////////////
	// parameters of the first layer of anchors
	// Alps: a structure containing all anchors layer info
	Alps * anchors_layer_4 = NULL;
	anchors_layer_4 = malloc(sizeof(Alps));
	float n_aspect_ratios_4[]= {1./4., 1./3., 1./2., 1., 2., 3., 4.};
	float n_variances_4[]={0.1, 0.1, 0.1, 0.1};
	anchors_layer_4->feature_map_width  = 10;
	anchors_layer_4->feature_map_height = 10;
	anchors_layer_4->n_classes  = 2;
	anchors_layer_4->img_width  = 80;
	anchors_layer_4->img_height = 80;
	anchors_layer_4->this_scale = 0.3;
	anchors_layer_4->next_scale = 0.4;
	anchors_layer_4->anchor_params = 4;
	anchors_layer_4->iou_thr = 0.1;
	anchors_layer_4->step_height= NAN;        // Automatic assignment
	anchors_layer_4->step_width = NAN;        // Automatic assignment
	anchors_layer_4->offset_height = NAN;     // Automatic assignment
	anchors_layer_4->offset_width  = NAN;     // Automatic assignment
	anchors_layer_4->aspect_ratios = n_aspect_ratios_4;
	anchors_layer_4->variances = n_variances_4;
	anchors_layer_4->two_boxes_for_ar1 = true;
	anchors_layer_4->max_bboxes = 20;
	anchors_layer_4->n_confidence = 0;
	anchors_layer_4->confidence_thr = CONFIDENCE_THR;           // it can be changed, more specifically after performance analysis.
	anchors_layer_4->max_candidates_per_class = 20;  // it can be changed, more specifically after performance analysis and also for power consumption.
	anchors_layer_4->n_aspect_ratios =	sizeof(n_aspect_ratios_4)/sizeof(n_aspect_ratios_4[0]);
	anchors_layer_4->n_variances     =	sizeof(n_variances_4)/sizeof(n_variances_4[0]);
	Anchor_layer_init(anchors_layer_4);             // initialize the layer with the information provided

	Generate_anchor_layer_output(source_file,header_file, anchors_layer_4, 4);

	////////////////////////
	// Forth anchor layer //
	////////////////////////
	// parameters of the first layer of anchors
	// Alps: a structure containing all anchors layer info
	Alps * anchors_layer_5 = NULL;
	anchors_layer_5 = malloc(sizeof(Alps));
	float n_aspect_ratios_5[]= {1./4., 1./3., 1./2., 1., 2., 3., 4.};
	float n_variances_5[]={0.1, 0.1, 0.1, 0.1};
	anchors_layer_5->feature_map_width  = 5;
	anchors_layer_5->feature_map_height = 5;
	anchors_layer_5->n_classes  = 2;
	anchors_layer_5->img_width  = 80;
	anchors_layer_5->img_height = 80;
	anchors_layer_5->this_scale = 0.4;
	anchors_layer_5->next_scale = 0.6;
	anchors_layer_5->anchor_params = 4;
	anchors_layer_5->iou_thr = 0.1;
	anchors_layer_5->step_height= NAN;        // Automatic assignment
	anchors_layer_5->step_width = NAN;        // Automatic assignment
	anchors_layer_5->offset_height = NAN;     // Automatic assignment
	anchors_layer_5->offset_width  = NAN;     // Automatic assignment
	anchors_layer_5->aspect_ratios = n_aspect_ratios_5;
	anchors_layer_5->variances = n_variances_5;
	anchors_layer_5->two_boxes_for_ar1 = true;
	anchors_layer_5->max_bboxes = 20;
	anchors_layer_5->n_confidence = 0;
	anchors_layer_5->confidence_thr = CONFIDENCE_THR;           // it can be changed, more specifically after performance analysis.
	anchors_layer_5->max_candidates_per_class = 20;  // it can be changed, more specifically after performance analysis and also for power consumption.
	anchors_layer_5->n_aspect_ratios =	sizeof(n_aspect_ratios_5)/sizeof(n_aspect_ratios_5[0]);
	anchors_layer_5->n_variances     =	sizeof(n_variances_5)/sizeof(n_variances_5[0]);
	Anchor_layer_init(anchors_layer_5);             // initialize the layer with the information provided

	Generate_anchor_layer_output(source_file,header_file, anchors_layer_5, 5);

	fclose(source_file);
	fclose(header_file);


}

int main()
{
	printf("\n########################################################################\n");
	printf("               Generating SSD Anchor Layers Parameters for GAP\n");
	printf("########################################################################\n");

	printf("\n...");
	GenerateAnchorsLayer();

	printf("Done\n");

	return 0;
}

