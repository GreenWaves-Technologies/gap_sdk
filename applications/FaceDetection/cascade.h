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