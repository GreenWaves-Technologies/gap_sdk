/*
 * Copyright (C) 2021 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <math.h>
#include "DeMosaicBasicKernels.h"

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

static inline unsigned int __attribute__((always_inline)) Mod2ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	if(Chunk%2!=0) Chunk++;
	return Chunk;
}

void KerDeMosaicAndResizeGBRG(KerDeMosaicAndResize_ArgT *Arg)

{
	unsigned char * __restrict__ In  = Arg->In;
	unsigned int Wout                = Arg->W;
	unsigned int Hout                = Arg->H;
	unsigned char * __restrict__ Out = Arg->Out;

	unsigned int Win                 = Wout*2;
	unsigned int Hin                 = Hout*2;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = Mod2ChunkSize(Wout);
	unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);


	for(int j=0;j<Hout;j++){
		for(int i=First;i<Last;i++){
			//R
			Out[(j*Wout+i)*3]  = In[j*2*Win+i*2+Win];
			//G
			Out[(j*Wout+i)*3+1]= (In[j*2*Win+i*2]>>1)+(In[j*2*Win+i*2+Win+1]>>1);
 			//B
 			Out[(j*Wout+i)*3+2]= In[(j*2*Win)+i*2+1];
		}
	}
}

void KerDeMosaicAndResizeGRBG(KerDeMosaicAndResize_ArgT *Arg)

{
	unsigned char * __restrict__ In  = Arg->In;
	unsigned int Wout                = Arg->W;
	unsigned int Hout                = Arg->H;
	unsigned char * __restrict__ Out = Arg->Out;

	unsigned int Win                 = Wout*2;
	unsigned int Hin                 = Hout*2;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = Mod2ChunkSize(Wout);
	unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);


	for(int j=0;j<Hout;j++){
		for(int i=First;i<Last;i++){
			//R
			Out[(j*Wout+i)*3]  = In[(j*2*Win)+i*2+1];
			//G
			Out[(j*Wout+i)*3+1]= (In[j*2*Win+i*2]>>1)+(In[j*2*Win+i*2+Win+1]>>1);
 			//B
 			Out[(j*Wout+i)*3+2]= In[j*2*Win+i*2+Win];
		}
	}
}


void KerDeMosaicAndResizeBGGR(KerDeMosaicAndResize_ArgT *Arg)

{
	unsigned char * __restrict__ In  = Arg->In;
	unsigned int Wout                = Arg->W;
	unsigned int Hout                = Arg->H;
	unsigned char * __restrict__ Out = Arg->Out;

	unsigned int Win                 = Wout*2;
	unsigned int Hin                 = Hout*2;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = Mod2ChunkSize(Wout);
	unsigned int First = CoreId*ChunkCell, Last  = Min(Wout, First+ChunkCell);


	for(int j=0;j<Hout;j++){
		for(int i=First;i<Last;i++){
			//R
			Out[(j*Wout+i)*3]  = In[j*2*Win+i*2+Win+1];
			//G
			Out[(j*Wout+i)*3+1]= (In[j*2*Win+i*2+1]>>1)+(In[j*2*Win+i*2+Win]>>1);
 			//B
 			Out[(j*Wout+i)*3+2]= In[j*2*Win+i*2];
		}
	}
}

