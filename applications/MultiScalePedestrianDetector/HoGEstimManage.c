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

#include <stdio.h>
#include "Gap8.h"
#include "HoGEstimManage.h"
#include "HoGEstimParameters.h"
#include "ImageDraw.h"

#define Max(a, b) (((a)>(b))?(a):(b))
#define Min(a, b) (((a)<(b))?(a):(b))

#ifdef __EMUL__
#define plp_alloc_l2(N) calloc(N, 1)
#endif
L2_MEM static BoundingBox *BBList;
L2_MEM static unsigned int HeadBBList;
L2_MEM static unsigned int MaxBBList;
L2_MEM static unsigned int BBDimRatio;

typedef struct {
        int X0;
        int X1;
        int Y0;
        int Y1;
} Rect;

int AllocateBBList(
	unsigned int N)
{
	BBList = (BoundingBox *) rt_alloc(RT_ALLOC_L2_CL_DATA, sizeof(BoundingBox)*N);
	HeadBBList = 0;
	MaxBBList = N;
	return (BBList==0);
}

void ResetBBList()

{
	HeadBBList = 0;
}


void DrawBBList(unsigned char* image,unsigned int W, unsigned int H)

{
	unsigned int i;
	for (i=0; i<HeadBBList; i++) {
		if (BBList[i].Merit==0) continue;
		DrawRectangle(image, H, W, BBList[i].X, BBList[i].Y, BBList[i].W, BBList[i].H, 255);
		DrawRectangle(image, H, W, (BBList[i].X)-1, (BBList[i].Y)-1, BBList[i].W+2, BBList[i].H+2, 255);
		DrawRectangle(image, H, W, (BBList[i].X)-2, (BBList[i].Y)-2, BBList[i].W+4, BBList[i].H+4, 0);
		DrawRectangle(image, H, W, (BBList[i].X)-3, (BBList[i].Y)-3, BBList[i].W+6, BBList[i].H+6, 255);
	}
}


void DumpBBList()

{
	unsigned int i;
	printf("\nMatching List after non max suppression [x, y, w, h, merit]:\n");
        for (i=0; i<HeadBBList; i++) {
		if (BBList[i].Merit==0) continue;
		printf("\t{%d, %d, %d, %d, %d},\n", BBList[i].X, BBList[i].Y, BBList[i].W, BBList[i].H, BBList[i].Merit);
	}
}


void SetBBAspectRatio(
	unsigned int DimRef,
	unsigned int Dim)

{
	BBDimRatio = (DimRef<<12)/Dim;
}


static inline unsigned int BBInteresectArea(Rect *R0, Rect *R1)

{
	int W = Max(0, Min(R0->X1, R1->X1)-Max(R0->X0, R1->X0));
	int H = Max(0, Min(R0->Y1, R1->Y1)-Max(R0->Y0, R1->Y0));
	return (W*H);
}

static inline unsigned int RefArea(int A0, int A1, int AreaInter)

{
        int UseMin = 1;

        if (UseMin) return Min(A0, A1); else return (A0+A1-AreaInter);
}


void RemoveNonMaxBB()

{
	BoundingBox *BB = BBList;
	unsigned int HeadBB;
	unsigned int i, j;
	unsigned int BBn = HeadBBList;

	printf("Remove NonMax: %d\n", BBn);
		for (i=0; i<BBn; i++) {
		BoundingBox *RefB = BB+i;
                if (RefB->Merit <= HOG_ESTIM_BB_THRES) continue;
                unsigned int AreaRef = RefB->W*RefB->H;
                Rect R0 = (Rect){RefB->X, RefB->X+RefB->W, RefB->Y, RefB->Y+RefB->H};

                for (j=i+1; j<BBn;  j++) {
			BoundingBox *CurB = BB+j;
                        if (CurB->Merit <= HOG_ESTIM_BB_THRES) continue;
                        Rect R1 = (Rect){CurB->X, CurB->X+CurB->W, CurB->Y, CurB->Y+CurB->H};
                        unsigned int AreaInter = BBInteresectArea(&R0, &R1);
                        if (AreaInter == 0) continue;
                        unsigned int Area = RefArea(AreaRef, CurB->W*CurB->H, AreaInter);
                        if (AreaInter>((Area*HOG_ESTIM_BB_OVERLAP)>>15)) {
                                if (RefB->Merit<CurB->Merit) {
                                        RefB->Merit = 0; break;
                                } else CurB->Merit = 0;
                        }
                }
        }
        HeadBB = 0;
        for (i=0; i<BBn; i++) if (BB[i].Merit > 0) BB[HeadBB++] = BB[i];
        HeadBBList = HeadBB;
	printf("Done Remove NonMax: %d\n", HeadBBList);
}

int PushBB(
	unsigned int X,
	unsigned int Y,
	unsigned int W,
	unsigned int H,
	unsigned int Merit,
	unsigned int NonMax)

{
	if (HeadBBList>=MaxBBList) return 1;
	BoundingBox BB = (BoundingBox) {X, Y, W, H, Merit};
	if (NonMax) {
        	int j;
		BoundingBox *RefB = &BB;
		unsigned int AreaRef = W*H;
		Rect R0 = (Rect){X, X+W, Y, Y+H};

		for (j=HeadBBList-1; j>=0;  j--) {
			BoundingBox *CurB = BBList+j;
			if (CurB->Merit <= HOG_ESTIM_BB_THRES) continue;
			Rect R1 = (Rect){CurB->X, CurB->X+CurB->W, CurB->Y, CurB->Y+CurB->H};
			unsigned int AreaInter = BBInteresectArea(&R0, &R1);
			if (AreaInter == 0) continue;
			unsigned int Area = RefArea(AreaRef, CurB->W*CurB->H, AreaInter);
			if (AreaInter>((Area*HOG_ESTIM_BB_OVERLAP)>>15)) {
				if (RefB->Merit<CurB->Merit) {
					RefB->Merit = 0; return 0;
				}
				CurB->Merit = 0;
			}
		}
	}
	BBList[HeadBBList++] = BB;
	return 0;
}

int PushBBAt(
	unsigned int XFeat,
	unsigned int YFeat,
	unsigned int Merit,
	unsigned int NonMax)

{
	if (Merit<=HOG_ESTIM_BB_THRES) return 0;
	unsigned int FeatPix = (HOG_BLOCK_SIZE-HOG_BLOCK_OVERLAP)*HOG_CELL_SIZE;

	return PushBB(gap8_muluRN(XFeat*FeatPix, BBDimRatio, 12), gap8_muluRN(YFeat*FeatPix, BBDimRatio, 12),
		      gap8_muluRN(HOG_ESTIM_WIDTH, BBDimRatio, 12), gap8_muluRN(HOG_ESTIM_HEIGHT, BBDimRatio, 12),
		      Merit, NonMax);
}

#ifdef OLD
void RemoveNonMaxBB()

{

	BoundingBox *BB = BBList;
        unsigned int HeadBB;
        unsigned int i, j;
	unsigned int BBn = HeadBBList;

printf("Remove NonMax: %d\n", BBn);
        for (i=0; i<BBn; i++) {
		BoundingBox *RefB = BB+i;
                unsigned int RefArea = RefB->W*RefB->H;
                unsigned int Xi1 = RefB->X+RefB->W;
                unsigned int Yi1 = RefB->Y+RefB->H;
                if (RefB->Merit <= HOG_ESTIM_BB_THRES) continue;
                for (j=i+1; j<BBn;  j++) {
			BoundingBox *CurB = BB+j;
                        unsigned int w, h, OverlapArea, CurArea;

                        if (CurB->Merit <= HOG_ESTIM_BB_THRES) continue;

                        w = Min(Xi1, CurB->X+CurB->W) - Max(RefB->X, CurB->X);
                        if (w<=0) continue;
                        h = Min(Yi1, CurB->Y+CurB->H) - Max(RefB->Y, CurB->Y);
                        if (h<=0) continue;
                        OverlapArea = w*h;
                        CurArea = Min(RefArea, CurB->W*CurB->H);
                        if (OverlapArea > ((HOG_ESTIM_BB_OVERLAP*CurArea)>>15)) {
				if (RefB->Merit>=CurB->Merit) {
					printf("Removing %d cause it overlaps %d\n", j, i);
					CurB->Merit = 0;
				} else {
					printf("Removing %d cause it overlaps %d\n", i, j);
					RefB->Merit = 0;
					break;
				}
			}
                }
        }
        HeadBB = 0;
        for (i=0; i<BBn; i++) if (BB[i].Merit > 0) BB[HeadBB++] = BB[i];
	
        HeadBBList = HeadBB;
printf("Done Remove NonMax: %d\n", HeadBBList);
}
#endif
