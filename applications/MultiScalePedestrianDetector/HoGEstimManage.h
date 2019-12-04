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

#ifndef __HOG_ESTIMMANAGE_H__
#define __HOG_ESTIMMANAGE_H__
typedef struct {
	unsigned short int X;
	unsigned short int Y;
	unsigned short int W;
	unsigned short int H;
	unsigned short int Merit;
} BoundingBox;

int AllocateBBList(
        unsigned int N
	);

void ResetBBList();
void DumpBBList();

void DrawBBList(
	unsigned char* image,
	unsigned int W,
	unsigned int H);

int PushBB(
	unsigned int X,
	unsigned int Y,
	unsigned int W,
	unsigned int H,
	unsigned int Merit,
	unsigned int NonMax);

int PushBBAt(
        unsigned int XFeat,
        unsigned int YFeat,
        unsigned int Merit,
	unsigned int NonMax);

void SetBBAspectRatio(
        unsigned int DimRef,
        unsigned int Dim);

void RemoveNonMaxBB();

#endif
