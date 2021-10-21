#include "SSDParams.h"



PI_L2 Alps * anchor_layer_2;
PI_L2 float n_aspect_ratios_2[]= {0.250000,0.333333,0.500000,1.000000,2.000000,3.000000,4.000000,};
PI_L2 float n_variances_2[]={0.100000,0.100000,0.100000,0.100000,};
PI_L2 anchorWH_t anchorWH_2[]={
{2.000000,8.000000},
{2.309401,6.928203},
{2.828427,5.656854},
{4.000000,4.000000},
{6.928204,6.928204},
{5.656854,2.828427},
{6.928203,2.309401},
{8.000000,2.000000},
};
int initAnchorLayer_2(){
	anchor_layer_2 = (Alps*) pmsis_l2_malloc(sizeof(Alps));
	if(anchor_layer_2==NULL) return 1;
	anchor_layer_2->feature_map_width  = 40;
	anchor_layer_2->feature_map_height = 40;
	anchor_layer_2->n_classes  = 2;
	anchor_layer_2->img_width  = 80;
	anchor_layer_2->img_height = 80;
	anchor_layer_2->this_scale = 0.050000;
	anchor_layer_2->next_scale = 0.150000;
	anchor_layer_2->anchor_params = 4;
	anchor_layer_2->iou_thr = 0.100000;
	anchor_layer_2->step_height= 2.000000;
	anchor_layer_2->step_width = 2.000000;
	anchor_layer_2->offset_height = 0.500000;
	anchor_layer_2->offset_width  = 0.500000;
	anchor_layer_2->aspect_ratios = n_aspect_ratios_2;
	anchor_layer_2->variances = n_variances_2;
	anchor_layer_2->two_boxes_for_ar1 = true;
	anchor_layer_2->confidence_thr = 14745;
	anchor_layer_2->n_aspect_ratios =  7;
	anchor_layer_2->n_anchors   =  8;
	anchor_layer_2->n_variances =  4;
	anchor_layer_2->anchorsWH = anchorWH_2;
}




PI_L2 Alps * anchor_layer_3;
PI_L2 float n_aspect_ratios_3[]= {0.250000,0.333333,0.500000,1.000000,2.000000,3.000000,4.000000,};
PI_L2 float n_variances_3[]={0.100000,0.100000,0.100000,0.100000,};
PI_L2 anchorWH_t anchorWH_3[]={
{6.000000,24.000000},
{6.928203,20.784609},
{8.485281,16.970562},
{12.000000,12.000000},
{16.970564,16.970564},
{16.970562,8.485281},
{20.784609,6.928203},
{24.000000,6.000000},
};
int initAnchorLayer_3(){
	anchor_layer_3 = (Alps*) pmsis_l2_malloc(sizeof(Alps));
	if(anchor_layer_3==NULL) return 1;
	anchor_layer_3->feature_map_width  = 20;
	anchor_layer_3->feature_map_height = 20;
	anchor_layer_3->n_classes  = 2;
	anchor_layer_3->img_width  = 80;
	anchor_layer_3->img_height = 80;
	anchor_layer_3->this_scale = 0.150000;
	anchor_layer_3->next_scale = 0.300000;
	anchor_layer_3->anchor_params = 4;
	anchor_layer_3->iou_thr = 0.100000;
	anchor_layer_3->step_height= 4.000000;
	anchor_layer_3->step_width = 4.000000;
	anchor_layer_3->offset_height = 0.500000;
	anchor_layer_3->offset_width  = 0.500000;
	anchor_layer_3->aspect_ratios = n_aspect_ratios_3;
	anchor_layer_3->variances = n_variances_3;
	anchor_layer_3->two_boxes_for_ar1 = true;
	anchor_layer_3->confidence_thr = 14745;
	anchor_layer_3->n_aspect_ratios =  7;
	anchor_layer_3->n_anchors   =  8;
	anchor_layer_3->n_variances =  4;
	anchor_layer_3->anchorsWH = anchorWH_3;
}




PI_L2 Alps * anchor_layer_4;
PI_L2 float n_aspect_ratios_4[]= {0.250000,0.333333,0.500000,1.000000,2.000000,3.000000,4.000000,};
PI_L2 float n_variances_4[]={0.100000,0.100000,0.100000,0.100000,};
PI_L2 anchorWH_t anchorWH_4[]={
{12.000000,48.000000},
{13.856406,41.569218},
{16.970562,33.941124},
{24.000000,24.000000},
{27.712814,27.712814},
{33.941124,16.970562},
{41.569218,13.856406},
{48.000000,12.000000},
};
int initAnchorLayer_4(){
	anchor_layer_4 = (Alps*) pmsis_l2_malloc(sizeof(Alps));
	if(anchor_layer_4==NULL) return 1;
	anchor_layer_4->feature_map_width  = 10;
	anchor_layer_4->feature_map_height = 10;
	anchor_layer_4->n_classes  = 2;
	anchor_layer_4->img_width  = 80;
	anchor_layer_4->img_height = 80;
	anchor_layer_4->this_scale = 0.300000;
	anchor_layer_4->next_scale = 0.400000;
	anchor_layer_4->anchor_params = 4;
	anchor_layer_4->iou_thr = 0.100000;
	anchor_layer_4->step_height= 8.000000;
	anchor_layer_4->step_width = 8.000000;
	anchor_layer_4->offset_height = 0.500000;
	anchor_layer_4->offset_width  = 0.500000;
	anchor_layer_4->aspect_ratios = n_aspect_ratios_4;
	anchor_layer_4->variances = n_variances_4;
	anchor_layer_4->two_boxes_for_ar1 = true;
	anchor_layer_4->confidence_thr = 14745;
	anchor_layer_4->n_aspect_ratios =  7;
	anchor_layer_4->n_anchors   =  8;
	anchor_layer_4->n_variances =  4;
	anchor_layer_4->anchorsWH = anchorWH_4;
}




PI_L2 Alps * anchor_layer_5;
PI_L2 float n_aspect_ratios_5[]= {0.250000,0.333333,0.500000,1.000000,2.000000,3.000000,4.000000,};
PI_L2 float n_variances_5[]={0.100000,0.100000,0.100000,0.100000,};
PI_L2 anchorWH_t anchorWH_5[]={
{16.000000,64.000000},
{18.475208,55.425625},
{22.627417,45.254833},
{32.000000,32.000000},
{39.191837,39.191837},
{45.254833,22.627417},
{55.425625,18.475208},
{64.000000,16.000000},
};
int initAnchorLayer_5(){
	anchor_layer_5 = (Alps*) pmsis_l2_malloc(sizeof(Alps));
	if(anchor_layer_5==NULL) return 1;
	anchor_layer_5->feature_map_width  = 5;
	anchor_layer_5->feature_map_height = 5;
	anchor_layer_5->n_classes  = 2;
	anchor_layer_5->img_width  = 80;
	anchor_layer_5->img_height = 80;
	anchor_layer_5->this_scale = 0.400000;
	anchor_layer_5->next_scale = 0.600000;
	anchor_layer_5->anchor_params = 4;
	anchor_layer_5->iou_thr = 0.100000;
	anchor_layer_5->step_height= 16.000000;
	anchor_layer_5->step_width = 16.000000;
	anchor_layer_5->offset_height = 0.500000;
	anchor_layer_5->offset_width  = 0.500000;
	anchor_layer_5->aspect_ratios = n_aspect_ratios_5;
	anchor_layer_5->variances = n_variances_5;
	anchor_layer_5->two_boxes_for_ar1 = true;
	anchor_layer_5->confidence_thr = 14745;
	anchor_layer_5->n_aspect_ratios =  7;
	anchor_layer_5->n_anchors   =  8;
	anchor_layer_5->n_variances =  4;
	anchor_layer_5->anchorsWH = anchorWH_5;
}

