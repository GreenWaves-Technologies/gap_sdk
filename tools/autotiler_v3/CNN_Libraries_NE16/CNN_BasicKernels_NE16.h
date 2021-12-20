/*
 * Copyright (C) 2018 GreenWaves Technologies
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

#ifndef __CNN_BASICKERNELS_NE16__
#define __CNN_BASICKERNELS_NE16__
#include "Gap.h"
#include "hal_ne16.h"
#include "../CNN_Libraries/CNN_Defines.h"

typedef struct {
	void * __restrict__ In;	/**< Pointer to input tile  */
	unsigned short * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	int * __restrict__ Bias;		/**< Pointer to bias tile, used when convolution is depth wise */
	void * __restrict__ Out;			/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	unsigned char * __restrict__ Scale;	/**< Pointer to Scale tensor applied channel-wise on the output */
	unsigned char * __restrict__ ScaleN;	/**< Pointer to Scale Shift tensor applied channel-wise on the output */
	unsigned short int Tile_InFeat;		/**< Number of features in In tile */
	unsigned short int TotalInFeatures; 	/**< Number of loaded In features tile */
	unsigned short int Tile_InH;		/**< Number of rows in In tile */
	unsigned short int Tile_InW;		/**< Number of columns in In tile */
	unsigned short int Tile_OutFeat;	/**< Number of features in Out tile */
	unsigned short int Tile_OutH;		/**< Number of rows in Out tile */
	unsigned short int Tile_OutW;		/**< Number of columns in Out tile */
	unsigned short int FilterSize;		/**< Fcx * Fcy */
	unsigned short int Pad_Val;		/**< Only if the NE16 is set to 3x3 mode. Explicit padding forces the value of part of the input set. The padding value can be from 0 to 255 in basic mode and from 0 to 65535 in mode16 */
	v4s 		   Pad;			/**< Only if the NE16 is set to 3x3 mode. Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom. It can be from 0 to 2 in each direction. */
	int 		   W_Offset;		/**< All weight tensors used by NE16 have to be stored as unsigned numbers (from 2 to 8 bits of range). This scalar signed 32-bit integer is used to set offset the stored tensor */
	unsigned char 	   Qw;			/**< Number of bits for filter */
	unsigned char 	   Mode16;
	unsigned char 	   FirstD0;
	unsigned char	   LastD0;
	unsigned int 	   Default_NE16_Job_Cfg;
	unsigned char      Fx;
	unsigned char      Fy;
	unsigned char	   Sx;
	unsigned char	   Sy;
	unsigned char	   Dx;
	unsigned char	   Dy;
} KerConv_NE16_T;

typedef struct {
	void * __restrict__ In;			/**< Pointer to input tile  */
	void * __restrict__ ColBuff;		/**< Pointer to Buffer for Im2Col  */
	unsigned short * __restrict__ Filter;	/**< Pointer to convolution coefficients. (Nx x Ny) coeffs in Q15 */
	int * __restrict__ Bias;		/**< Pointer to bias tile, used when convolution is depth wise */
	void * __restrict__ Out;		/**< Pointer to output tile, this tile can have up to N-1 lines and N-1 column than In depending on Pad */
	unsigned char * __restrict__ Scale;	/**< Pointer to Scale tensor applied channel-wise on the output */
	unsigned char * __restrict__ ScaleN;	/**< Pointer to Scale Shift tensor applied channel-wise on the output */
	unsigned short int Tile_InFeat;		/**< Number of features in In tile */
	unsigned short int TotalInFeatures; 	/**< Number of loaded In features tile */
	unsigned short int Tile_InH;		/**< Number of rows in In tile */
	unsigned short int Tile_InW;		/**< Number of columns in In tile */
	unsigned short int Tile_OutFeat;	/**< Number of features in Out tile */
	unsigned short int Tile_OutH;		/**< Number of rows in Out tile */
	unsigned short int Tile_OutW;		/**< Number of columns in Out tile */
	unsigned short int FilterSize;		/**< Fcx * Fcy */
	unsigned short int Pad_Val;		/**< Only if the NE16 is set to 3x3 mode. Explicit padding forces the value of part of the input set. The padding value can be from 0 to 255 in basic mode and from 0 to 65535 in mode16 */
	v4s 		   Pad;			/**< Only if the NE16 is set to 3x3 mode. Paddding, 0: Left, 1: Right, 2: Top, 3: Bottom. It can be from 0 to 2 in each direction. */
	int 		   W_Offset;		/**< All weight tensors used by NE16 have to be stored as unsigned numbers (from 2 to 8 bits of range). This scalar signed 32-bit integer is used to set offset the stored tensor */
	unsigned char 	   Qw;			/**< Number of bits for filter */
	unsigned char 	   Mode16;
	unsigned char 	   FirstD0;
	unsigned char	   LastD0;
	unsigned char 	   FirstTile;
	unsigned int 	   Default_NE16_Job_Cfg;
	unsigned char      Fx;
	unsigned char      Fy;
	unsigned char	   Sx;
	unsigned char	   Sy;
	unsigned char	   Dx;
	unsigned char	   Dy;
	unsigned int       Semaphores[2];
} KerConv_MM_NE16_T;

typedef struct {
	void * __restrict__  In;
	unsigned short * __restrict__ Filter;
	int * __restrict__            Bias;
	void * __restrict__           Out;
	unsigned char * __restrict__  Scale;
	unsigned char * __restrict__  ScaleN;
	unsigned short int            Tile_InFeat;
	unsigned short int            Tile_OutFeat;
	unsigned short int            H;
	unsigned short int            W;
	unsigned char                 LastD0;
	unsigned char                 FirstD0;
	int                           W_Offset;
	unsigned char                 Qw;
	unsigned char 	   	      	  Mode16;
	unsigned int                  Default_NE16_Job_Cfg;
} KerLinear_NE16_T;

#define NE16_FC_PRENORM 	0
#define NE16_FC_ZP 			1
#define NE16_FC_WOFF		3
#define NE16_FC_INF			4

typedef struct {
	unsigned short * __restrict__  	In;
	unsigned char * __restrict__ 	Filter;
	int * __restrict__            	Bias;
	void * __restrict__  	 		Out;
	int * __restrict__  	 		Buf;
	unsigned char * __restrict__  	Scale;
	unsigned char * __restrict__  	ScaleN;
	unsigned short int            	Tile_InFeat;
	unsigned short int            	Tile_OutFeat;
	unsigned short int            	H;
	unsigned short int            	W;
	unsigned char                 	LastD0;
	unsigned char                 	FirstD0;
	unsigned char                 	LastD1;
	unsigned char                 	FirstD1;
	unsigned char                 	Qw;
	unsigned int                  	Default_NE16_Job_Cfg;
	unsigned int					Sem;
	signed char * 					Infos;
} KerLinear_NE16fp_T;

typedef struct {
	void * __restrict__ In1;
	unsigned short int W_In1;
	unsigned short int H_In1;
	void * __restrict__ In2;
	unsigned short int W_In2;
	void * __restrict__ Bias;
	unsigned char * __restrict__ Scale;
	unsigned char * __restrict__ ScaleN;
	void * __restrict__ Out;
	void * __restrict__ Buff;
	signed char * __restrict__ Infos;
	int W_Offset;
	unsigned int Semaphores[2];
	unsigned int Default_NE16_Job_Cfg;
} KerMatMul_NE16_T;

typedef struct 
{
	unsigned char * __restrict__ In;
	unsigned short int           Tile_InW;
	unsigned short int           Tile_InH;
	v4s                          Pad;
	unsigned short * __restrict__ Filter;
	int * __restrict__           Bias;
	unsigned char * __restrict__ Out;
	unsigned char * __restrict__ Scale;
	unsigned char * __restrict__ ScaleN;
	unsigned short int           Tile_OutW;
	unsigned short int           Tile_OutH;
	unsigned short int           Tile_InFeat;
	unsigned short int           Tile_OutFeat;
	int                          W_Offset;
	unsigned short int 	     Pad_Val;
	unsigned char                Qw;
	unsigned int 		     Default_NE16_Job_Cfg;
	unsigned char                FirstD0;
	unsigned char                LastD0;
	unsigned char                LastT0;
	void *                       NE16JobEvt;
} Ker_NE16_Job_T;

/** @brief Enable the NE16

Enable the NE16

Clock is released
*/
static inline void NE16_Enable() {
        // enable clock
        NE16_CG_ENABLE();

        // setup HCI
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        NE16_RESET_MAXSTALL();   // reset maximum stall
        NE16_SET_MAXSTALL(8);    // set maximum consecutive stall to 8 cycles for cores, DMA side
}

/** @brief Disable the NE16

Disable the NE16
*/
static inline void NE16_Disable() {
	// disable clock
	NE16_CG_DISABLE();
	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

/** @brief Software reset the NE16

Software reset the NE16
*/
static inline void NE16_SoftReset() {
        // soft-clear NE16
        NE16_WRITE_CMD(NE16_SOFT_CLEAR, NE16_SOFT_CLEAR_ALL);
        for(volatile int kk=0; kk<10; kk++);
}

static inline void NE16_GvsocTrace(int TraceLevel) {
	NE16_WRITE_REG(NE16_SPECIAL_TRACE_REG, (int) TraceLevel);
}
static inline void NE16_GvsocPrintOpt(int PrintOpt) { /* 0: dec, 1: hex */
	NE16_WRITE_REG(NE16_SPECIAL_FORMAT_TRACE_REG, (int) PrintOpt);
}


static inline void NE16_FireJob() {
	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	// wait for end of computation
	NE16_BARRIER();
	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

/* Basic Kernels functions */
void NE16_PrepareJob(Ker_NE16_Job_T *Arg);
void KerConv3x3Stride1_NE16(KerConv_NE16_T *Arg);
void KerConv3x3Stride2_NE16(KerConv_NE16_T *Arg);
void KerConvDW3x3Stride1_NE16(KerConv_NE16_T *Arg);
void KerConvDW3x3Stride2_NE16(KerConv_NE16_T *Arg);
void KerConv3x3StrideSxxSy_NE16(KerConv_NE16_T *Arg);
void KerConv1x1Stride1_NE16(KerConv_NE16_T *Arg);
void KerConv1x1StrideS_NE16(KerConv_NE16_T *Arg);
void KerConv1x1_SmallHW_StrideS_NE16(KerConv_NE16_T *Arg);
void KerConv1x1_SmallHW_Stride1_NE16(KerConv_NE16_T *Arg);
void KerConv1D_StrideS_NE16(KerConv_NE16_T *Arg);
void KerConvNxMDxDy_StrideSxSy_NE16(KerConv_NE16_T *Arg);
void KerConv3x3Stride1_DxDy_NE16(KerConv_NE16_T *Arg);
void KerLinear_8a_NE16(KerLinear_NE16_T *Arg);
void KerLinear_16a_NE16(KerLinear_NE16_T *Arg);
void Ker_MM_Conv2D_NE16(KerConv_MM_NE16_T *Arg);
void KerMatMul_8a_NE16(KerMatMul_NE16_T *Arg);
void KerMatMul_8aFast_NE16(KerMatMul_NE16_T *Arg);
void KerMatMul_16a_NE16(KerMatMul_NE16_T *Arg);
void KerMatMul_16aFast_NE16(KerMatMul_NE16_T *Arg);

#endif