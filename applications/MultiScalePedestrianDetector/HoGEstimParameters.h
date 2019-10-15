#ifndef __HOG_ESTIM_PARAMETERS_H__
#define __HOG_ESTIM_PARAMETERS_H__

#include "HoGParameters.h"

#define HOG_ESTIM_WIDTH  64
#define HOG_ESTIM_HEIGHT 128

#define HOG_ESTIM_SCALE_FACTOR 8


#define HOG_ESTIM_WIN_FEAT_W	(1+((HOG_ESTIM_WIDTH/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP)))
#define HOG_ESTIM_WIN_FEAT_H	(1+((HOG_ESTIM_HEIGHT/HOG_CELL_SIZE - HOG_BLOCK_SIZE)/(HOG_BLOCK_SIZE - HOG_BLOCK_OVERLAP)))

#define HOG_ESTIM_WIN_NFEAT	HOG_ESTIM_WIN_FEAT_W*HOG_ESTIM_WIN_FEAT_H*HOG_BLOCK_SIZE*HOG_BLOCK_SIZE*HOG_NBINS

// #define	HOG_WEAK_ESTIM	1024
#define	HOG_WEAK_ESTIM	1000
// #define	HOG_WEAK_ESTIM	(1024*3)/4
#define HOG_ESTIM_TREE_DEPTH 1
// #define HOG_ESTIM_TREE_DEPTH 2

#define HOG_ESTIM_TD1

#define HOG_ESTIM_MAX_BB 	100
#define HOG_ESTIM_BB_THRES	15
#define HOG_ESTIM_BB_OVERLAP	((int)((0.5)*((1 << (15))-1)))

typedef struct {
        unsigned short int FeatureId;
        unsigned short int ThresHold[(1<<HOG_ESTIM_TREE_DEPTH)-1];
#ifdef HOG_ESTIM_TD1
        int Value[1];
#else
        short int Value[1<<HOG_ESTIM_TREE_DEPTH];
#endif
} HoGWeakPredictor_T;

typedef struct {
	// unsigned short int Col;
	// unsigned short int Off;
	unsigned short int Pos; /* b15..b12: Col, b11..b0: Offset in column */
        unsigned short int ThresHold;
        int Value;
} HoGWeakPredictorBis_T;

#endif

