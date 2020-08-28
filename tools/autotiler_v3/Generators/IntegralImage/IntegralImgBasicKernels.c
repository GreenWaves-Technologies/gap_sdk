/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <math.h>
#include "Gap.h"
#include "IntegralImgBasicKernels.h"

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

void KerPrime(KerPrimeImage_ArgT *KerArg)
{
	unsigned int W = KerArg->W;
	unsigned int *Buffer = KerArg->KerBuffer;
	unsigned int Col,CoreId = gap_coreid();

	unsigned int ChunkBlock = ChunkSize(W);
	unsigned int First = CoreId*ChunkBlock;
	unsigned int Last  = (First+ChunkBlock > W) ? (W) : (First+ChunkBlock);

	for (Col=First; Col<Last; Col++){
		Buffer[Col] = 0;
	}

}

void KerProcess(KerProcessImage_ArgT *KerArg)

{
	unsigned char* inImg = KerArg->In;
	unsigned int W = KerArg->W;
	unsigned int H = KerArg->H;
	unsigned int* outIntImg = KerArg->IntegralImage;
	unsigned int* buff = KerArg->KerBuffer;
	unsigned int Col,Line;
	unsigned int CoreId = gap_coreid();

	unsigned int ChunkBlock = ChunkSize(W);
	unsigned int FirstCol = CoreId*ChunkBlock;
	unsigned int LastCol  = (FirstCol+ChunkBlock > W) ? (W) : (FirstCol+ChunkBlock);

	ChunkBlock = ChunkSize(H);
	unsigned int FirstLine = CoreId*ChunkBlock;
	unsigned int LastLine  = (FirstLine+ChunkBlock > H) ? (H) : (FirstLine+ChunkBlock);

	for (Col=FirstCol; Col<LastCol; Col++){
		outIntImg[Col] = inImg[Col] + buff[Col];
		for (Line=0; Line<H-1; Line++){
			outIntImg[Col + ((Line+1)*W)] = outIntImg[Col+(Line)*W] + inImg[Col+(Line+1)*W];
		}

	}
	gap_waitbarrier(0);
	//Saving to Buff intermediate results
	for (Col=FirstCol; Col<LastCol; Col++){
		buff[Col] = outIntImg[Col+((H-1)*W)];
	}

	gap_waitbarrier(0);
	for (Line=FirstLine; Line<LastLine; Line++){
		for (Col=0; Col<W-1; Col++){
			outIntImg[Col+1 +(Line*W)] = outIntImg[Col+(Line)*W] + outIntImg[Col+1+(Line)*W];
		}
	}
}
