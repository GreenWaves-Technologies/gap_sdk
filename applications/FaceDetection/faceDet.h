#ifndef __FACE_DET_H__
#define __FACE_DET_H__

#include "pmsis.h"
#include "FaceDetKernels.h"
#include "stdio.h"
#include "setup.h"

#define FACE_DETECT_L1_MEMORY_POOL_SIZE  (53*1024)


typedef struct cascade_answers{
	int x;
	int y;
	int w;
	int h;
	int score;
}cascade_reponse_t;

typedef struct ArgCluster {
	unsigned char*   	ImageIn;
	unsigned char*   	OutCamera;
	unsigned int 		Win;
	unsigned int 		Hin;
	unsigned char* 		ImageOut;
	unsigned int 		Wout;
	unsigned int 		Hout;
	unsigned int* 		ImageIntegral;
	unsigned int* 		SquaredImageIntegral;
	unsigned short *		image16;
	cascade_reponse_t* reponses;
	unsigned char num_reponse;
	int* output_map;
	cascade_t* model;
	unsigned int cycles;
} ArgCluster_T;

void faceDet_cluster_init(ArgCluster_T *ArgC);
void faceDet_cluster_main(ArgCluster_T *ArgC);

#endif
