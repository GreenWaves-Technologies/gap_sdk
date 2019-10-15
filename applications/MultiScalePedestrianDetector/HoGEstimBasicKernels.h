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
