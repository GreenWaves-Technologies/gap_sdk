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

#ifndef __HOGESTIMBASICKERNELS_H__
#define __HOGESTIMBASICKERNELS_H__
#include "Gap8.h"
#include "HoGEstimParameters.h"

typedef struct {
	uint16_t * __restrict__ In;
	uint32_t H;
	uint32_t FeatureSize;
	uint32_t EstimWidth;
	uint16_t ** __restrict__ HoGFeatCols;
	uint32_t HoGFeatColIndex;
} KerReadHoGFeatCol_ArgT;
void KerReadHoGFeatCol(KerReadHoGFeatCol_ArgT *Arg);

typedef struct {
	uint16_t ** __restrict__ HoGFeatCols;
	uint32_t FeatureSize;
	uint32_t WEstimator;
	uint32_t HEstimator;
	uint32_t HFeatCols;
	uint32_t * __restrict__ Out;
} KerEstimate_ArgT;
void KerEstimate(KerEstimate_ArgT *Arg);

void KerEstimateWin(
        uint16_t ** __restrict__ HoGFeatCols,
        uint32_t FeatureSize,
        uint32_t WEstimator,
        uint32_t HEstimator,
        uint32_t HFeatCols,
        uint32_t * __restrict__ Out,
        uint16_t * __restrict__ Buffer);

void KerWeakEstimateWin(
        uint16_t ** __restrict__ HoGFeatCols,
        uint32_t FeatureSize,
        uint32_t WEstimator,
        uint32_t HEstimator,
        uint32_t HFeatCols,
        uint8_t * __restrict__ Out,
        uint16_t * __restrict__ Buffer,
        HoGWeakPredictor_T * __restrict__ Model,
        uint32_t ModelSize);

typedef struct {
        uint16_t ** __restrict__ HoGFeatCols;
	uint32_t ColumnIndexM1;
        uint32_t HEstimator;
        uint32_t HFeatCols;
        uint32_t FeatureSize;
        HoGWeakPredictorBis_T * __restrict__ Model;
        uint32_t ModelSize;
} KerWeakEstimate_ArgT;
void KerWeakEstimateAllWindows(KerWeakEstimate_ArgT *Arg);

void InstallModel(
	HoGWeakPredictor_T *From,
	HoGWeakPredictor_T *To,
	unsigned int N);

void InstallModelBis(
	HoGWeakPredictorBis_T *From,
	HoGWeakPredictorBis_T *To,
	unsigned int N);
#endif
