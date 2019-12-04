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
