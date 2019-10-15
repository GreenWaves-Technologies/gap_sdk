
#ifndef CASCADE_H
#define CASCADE_H

typedef struct single_cascade{
	//unsigned short num_stages;
	unsigned short stage_size;

	unsigned short rectangles_size;
	short* thresholds;
	short* alpha1;
	short* alpha2;
	unsigned short*  rect_num;
	signed char*  weights;
	char*  rectangles;
} single_cascade_t;

typedef struct cascade{
	int stages_num;              //number of cascades
	signed short *thresholds;    //cascades thresholds
	single_cascade_t ** stages ;  //pointer to single cascade stages
	single_cascade_t* buffers_l1[2];
} cascade_t;


#endif 