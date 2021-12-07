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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wint-conversion"
#include "Gap.h"
#include "CNN_BasicKernels_NE16.h"

#define RESET_QUANTOUT 	(~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT)) & (~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT)) & (~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS))
#define SET_QUANTOUT 	(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT) | (NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT) | (NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS)
#define SET_STREAMIN 	(NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN)

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

static int FirstDefinedOutput(int F, int Pad, int Stride)

{
        // k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

        return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(int DimIn, int F, int PadL, int Stride, int D)

{
        // k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

        return ((DimIn - ((F-1)/2*D - PadL + (F/2)*D) + Stride-1)/Stride);
}

#define B_CLR(x, bits) ((x) & (~((1 << (bits)) - 1)))
static inline void Copy(void *__restrict__ To, void *__restrict__ From, unsigned int Size, unsigned int CoreId)

{
        unsigned int Chunk = ChunkSize(Size), First = Chunk * CoreId, Last = Min(First + Chunk, Size);
        unsigned int Iter = Max(0, Last - First);

        int *pFrom = (int *)(From + First), *pTo = (int *)(To + First);
        for (int i = 0; i < Iter / 8; i++)
        {
                int V0 = pFrom[2 * i], V1 = pFrom[2 * i + 1];
                pTo[2 * i] = V0;
                pTo[2 * i + 1] = V1;
        }
        if (Iter & 0x4)
                *((int *)(To + First + B_CLR(Iter, 3))) = *((int *)(From + First + B_CLR(Iter, 3)));
        if (Iter & 0x2)
                *((short int *)(To + First + B_CLR(Iter, 2))) = *((short int *)(From + First + B_CLR(Iter, 2)));
        if (Iter & 0x1)
                *((signed char *)(To + First + Iter - 1)) = *((signed char *)(From + First + Iter - 1));
}

static inline void SetNE16_InPointer(void *InPointer){
	NE16_WRITE_REG(NE16_REG_INFEAT_PTR, (int) InPointer);
        #if defined( DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
                printf("InPointer:\t%x\n", InPointer);
        #endif
}
static inline void SetNE16_OutPointer(void *OutPointer){
	NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR, (int) OutPointer);
        #if defined( DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
                printf("OutPointer:\t%x\n", OutPointer);
        #endif
}
static inline void SetNE16_WeightsPointer(void *WeightsPointer){
	NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR, (int) WeightsPointer);
        #ifdef DEBUG_NE16
                printf("WeightsPointer:\t%x\n", WeightsPointer);
        #endif
}
static inline void SetNE16_BiasPointer(void *BiasPointer){
	NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR, (int) BiasPointer);
        #ifdef DEBUG_NE16
                printf("BiasPointer:\t%x\n", BiasPointer);
        #endif
}
static inline void SetNE16_ScalePointer(void *ScalePointer){
	NE16_WRITE_REG(NE16_REG_SCALE_PTR, (int) ScalePointer);
        #ifdef DEBUG_NE16
                printf("ScalePointer:\t%x\n", ScalePointer);
        #endif
}
static inline void SetNE16_ScaleNPointer(void *ScaleNPointer){
	NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, (int) ScaleNPointer);
        #ifdef DEBUG_NE16
                printf("ScaleNPointer:\t%x\n", ScaleNPointer);
        #endif
}

static inline void SetNE16_Strides(unsigned short In_D0,
		     unsigned short In_D1,
		     unsigned short In_D2,

		     unsigned short Out_D0,
		     unsigned short Out_D1,
		     unsigned short Out_D2,

		     unsigned short Weights_D0,
		     unsigned short Weights_D1,
		     unsigned short Weights_D2)
{
	NE16_WRITE_REG(NE16_REG_INFEAT_D0_STRIDE, In_D0);
	NE16_WRITE_REG(NE16_REG_INFEAT_D1_STRIDE, In_D1);
	NE16_WRITE_REG(NE16_REG_INFEAT_D2_STRIDE, In_D2);
	NE16_WRITE_REG(NE16_REG_OUTFEAT_D0_STRIDE, Out_D0);
	NE16_WRITE_REG(NE16_REG_OUTFEAT_D1_STRIDE, Out_D1);
	NE16_WRITE_REG(NE16_REG_OUTFEAT_D2_STRIDE, Out_D2);
	NE16_WRITE_REG(NE16_REG_WEIGHTS_D0_STRIDE, Weights_D0);
	NE16_WRITE_REG(NE16_REG_WEIGHTS_D1_STRIDE, Weights_D1);
	NE16_WRITE_REG(NE16_REG_WEIGHTS_D2_STRIDE, Weights_D2);
        #ifdef DEBUG_NE16
                printf("InStrides: %d - %d - %d OutStrides: %d - %d - %d WeightsStrides: %d - %d - %d\n", In_D0, In_D1, In_D2, Out_D0, Out_D1, Out_D2, Weights_D0, Weights_D1, Weights_D2);
        #endif
}

static inline void SetNE16_Dim(unsigned short Nb_KI,
		 unsigned short Nb_KO,
		 unsigned short Nb_WO,
		 unsigned short Nb_HO)
{
	NE16_WRITE_REG(NE16_REG_NB_KO_KI,  ((Nb_KI  & NE16_MASK_NB_KI)  << NE16_SHIFT_NB_KI) |  /**< The Ki remainder must be the remainder of Ki / 16 if such remainder is not zero, otherwise must be 16 */
					   ((Nb_KO  & NE16_MASK_NB_KO)  << NE16_SHIFT_NB_KO));  /**< The number of Ko subtiles is Ko / 32, plus 1 if Ko / 32 has non-zero remainder */
	NE16_WRITE_REG(NE16_REG_NB_HO_WO,  ((Nb_WO  & NE16_MASK_NB_WO)  << NE16_SHIFT_NB_WO) |  /**< The number of Wo subtitles is Wo / 3, plus 1 if Wo / 3 has non-zero remainder */
					   ((Nb_HO  & NE16_MASK_NB_HO)  << NE16_SHIFT_NB_HO));  /**< The number of Ho subtitles is Ho / 3, plus 1 if Ho / 3 has non-zero remainder */
        #ifdef DEBUG_NE16
                printf("Nb_KI:\t%d\tNb_KO:\t%d\tNb_HO:\t%d\tNb_WO:\t%d\n", Nb_KI, Nb_KO, Nb_WO, Nb_HO);
        #endif
}

static inline void SetNE16_Reminders(unsigned short Rem_WI,
		       unsigned short Rem_HI,
		       unsigned short Rem_KI,
		       unsigned short Rem_KO,
		       unsigned short Rem_WO,
		       unsigned short Rem_HO)
{
	NE16_WRITE_REG(NE16_REG_REM_KO_KI, ((Rem_KI & NE16_MASK_REM_KI) << NE16_SHIFT_REM_KI) | /**< The Ki remainder must be the remainder of Ki / 16 if such remainder is not zero, otherwise must be 16 */
					   ((Rem_KO & NE16_MASK_REM_KO) << NE16_SHIFT_REM_KO)); /**< The Ko remainder must be the remainder of Ko / 32 if such remainder is not zero, otherwise must be 32 */
	NE16_WRITE_REG(NE16_REG_REM_HO_WO, ((Rem_WO & NE16_MASK_REM_WO) << NE16_SHIFT_REM_WO) | /**< The Wo remainder must be the remainder of Wo / 3 */
					   ((Rem_HO & NE16_MASK_REM_HO) << NE16_SHIFT_REM_HO)); /**< The Ho remainder must be the remainder of Ho / 3 */
	NE16_WRITE_REG(NE16_REG_REM_HI_WI, ((Rem_WI & NE16_MASK_REM_WI) << NE16_SHIFT_REM_WI) | /**< */
					   ((Rem_HI & NE16_MASK_REM_HI) << NE16_SHIFT_REM_HI)); /**< */
        #ifdef DEBUG_NE16
                printf("Rem_KI:\t%d\tRem_KO:\t%d\tRem_HI:\t%d\tRem_WI:\t%d\tRem_HO:\t%d\tRem_WO:\t%d\n", Rem_KI, Rem_KO, Rem_HI, Rem_WI, Rem_HO, Rem_WO);
        #endif
}

static inline void SetNE16_ConfigPad(v4s Pad, short int PadVal)
{
	NE16_WRITE_REG(NE16_REG_PADDING,     ((PadVal & NE16_MASK_PADDING_VALUE)    << NE16_SHIFT_PADDING_VALUE) | /**< Only if the NE16 is set to 3x3 mode. Explicit padding forces the value of part of the input set.  */
					     ((Pad[0]  & NE16_MASK_PADDING_LEFT)    << NE16_SHIFT_PADDING_LEFT)  |     /**< It can be from 0 to 2 in each direction (left/right/top/bottom) */
					     ((Pad[1]  & NE16_MASK_PADDING_RIGHT)   << NE16_SHIFT_PADDING_RIGHT) |     /**< The padding value can be from 0 to 255 in basic mode and from 0 to 65535 in mode16 */
					     ((Pad[2]  & NE16_MASK_PADDING_TOP)     << NE16_SHIFT_PADDING_TOP)   |
					     ((Pad[3]  & NE16_MASK_PADDING_BOTTOM)  << NE16_SHIFT_PADDING_BOTTOM));
	#ifdef DEBUG_NE16
		printf("Padding: {%d, %d, %d, %d}, Val: %d\n", Pad[0], Pad[1], Pad[2], Pad[3], PadVal);
	#endif
}

static inline void SetNE16_ConfigFMask(v4s FilterMask)
{
	NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((FilterMask[0] & NE16_MASK_FILTER_MASK_LEFT)   << NE16_SHIFT_FILTER_MASK_LEFT)   | /**< Only if the NE16 is set to 3x3 mode. */
					     ((FilterMask[1] & NE16_MASK_FILTER_MASK_RIGHT)  << NE16_SHIFT_FILTER_MASK_RIGHT)  | /**< Filter masking forces the value of part of the weights in the spatial direction */
					     ((FilterMask[2] & NE16_MASK_FILTER_MASK_TOP)    << NE16_SHIFT_FILTER_MASK_TOP)    | /**< It can be from 0 to 1 in each direction (left/right/top/bottom) */
					     ((FilterMask[3] & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM));
	#ifdef DEBUG_NE16
		printf("FMask: {%d, %d, %d, %d}\n", FilterMask[0], FilterMask[1], FilterMask[2], FilterMask[3]);
	#endif
}

static inline void SetNE16_WOffset(int W_Offset){
	NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, W_Offset);
	#ifdef DEBUG_NE16
		printf("W_Offset: %d\n", W_Offset);
	#endif
}

static inline void SetNE16_GenConfig(unsigned int Cfg){
	NE16_WRITE_REG(NE16_REG_CONFIG, 0);
	NE16_WRITE_REG(NE16_REG_CONFIG, Cfg);
	#ifdef DEBUG_NE16
		int Qw 			= ((Cfg >> NE16_SHIFT_WBITS_M1) 	 & NE16_MASK_WBITS_M1) + 1;
		int Mode16 		= ((Cfg >> NE16_SHIFT_MODE16) 		 & NE16_MASK_MODE16);
		int StreamoutMode 	= ((Cfg >> NE16_SHIFT_OUTQUANT) 	 & NE16_MASK_OUTQUANT);
		int FilterMode 		= ((Cfg >> NE16_SHIFT_FILTER_MODE) 	 & NE16_MASK_FILTER_MODE);
		int LinearMode 		= ((Cfg >> NE16_SHIFT_LINEAR_MODE) 	 & NE16_MASK_LINEAR_MODE);
		int StridedMode 	= ((Cfg >> NE16_SHIFT_STRIDED_MODE) 	 & NE16_MASK_STRIDED_MODE);
		int NormBits 		= ((Cfg >> NE16_SHIFT_NORM_BITS) 	 & NE16_MASK_NORM_BITS);
		int Streamin 		= ((Cfg >> NE16_SHIFT_STREAMIN) 	 & NE16_MASK_STREAMIN);
		int WOffsetCfg 		= ((Cfg >> NE16_SHIFT_WEIGHT_OFFSET_CFG) & NE16_MASK_WEIGHT_OFFSET_CFG);
		int QuantRightShift 	= ((Cfg >> NE16_SHIFT_QUANT_RIGHT_SHIFT) & NE16_MASK_QUANT_RIGHT_SHIFT);
		int QuantBits 		= ((Cfg >> NE16_SHIFT_QUANT_BITS) 	 & NE16_MASK_QUANT_BITS);
		int QuantNoRect 	= ((Cfg >> NE16_SHIFT_QUANT_NORECT) 	 & NE16_MASK_QUANT_NORECT);
		int NormShift 		= ((Cfg >> NE16_SHIFT_NORM_SHIFT) 	 & NE16_MASK_NORM_SHIFT);
		int NormBias 		= ((Cfg >> NE16_SHIFT_NORM_BIAS) 	 & NE16_MASK_NORM_BIAS);
		printf("General config: %d\n\tQw: %d Mode16: %d StreamoutMode: %d FilterMode: %d LinearMode: %d StridedMode: %d NormBits: %d Streamin: %d WOffsetCfg: %d QuantRightShift: %d QuantBits: %d QuantNoRect: %d NormShift: %d NormBias %d\n", Cfg, Qw, Mode16, StreamoutMode, FilterMode, LinearMode, StridedMode, NormBits, Streamin, WOffsetCfg, QuantRightShift, QuantBits, QuantNoRect, NormShift, NormBias);
	#endif
}

void NE16_ComputeBorders(KerConv_NE16_T *Arg, int Wo_F, int Wo_L, int Wo, int Ho_F, int Ho_L, int Ho) {
	unsigned char * __restrict__ In      = (unsigned char *__restrict__) Arg->In;
	int 	      * __restrict__ Out     = (int           *__restrict__) Arg->Out;
	int 	      * __restrict__ Bias    = (int           *__restrict__) Arg->Bias;
	unsigned char * __restrict__ Filter  = (unsigned char *__restrict__) Arg->Filter;
	unsigned char * __restrict__ Scale   = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN  = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned char * InPointer;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}

	int PadL = Arg->Pad[0], PadR = Arg->Pad[1], PadT = Arg->Pad[2], PadB = Arg->Pad[3];
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Nb_LoadedKI = Arg->TotalInFeatures/16 + (Arg->TotalInFeatures%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Sx = Arg->Sx, Fx = Arg->Fx, Dx = Arg->Dx;
	int Sy = Arg->Sy, Fy = Arg->Fy, Dy = Arg->Dy;
	int Rem_H_Subtiles = (Ho_L - Ho_F) % 3, H_SubTiles = (Ho_L - Ho_F) / 3 + (Rem_H_Subtiles?1:0);
	int Rem_W_Subtiles = (Wo_L - Wo_F) % 3, W_SubTiles = (Wo_L - Wo_F) / 3 + (Rem_W_Subtiles?1:0);
        unsigned int res_quant_out_flag, set_quant_out_flag, streamin_flag, Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes = (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
	streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
	set_quant_out_flag  = Default_cfg & (NE16_MASK_OUTQUANT     << NE16_SHIFT_OUTQUANT);
	set_quant_out_flag |= Default_cfg & (NE16_MASK_NORM_SHIFT   << NE16_SHIFT_NORM_SHIFT);
	set_quant_out_flag |= Default_cfg & (NE16_MASK_NORM_BIAS    << NE16_SHIFT_NORM_BIAS);
	int ApplyRect = (Default_cfg & (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT)) == 0;

        volatile int job_id;
	if (PadL && H_SubTiles) {
		InPointer = In;
		for (int w=0; w<Wo_F; w++) {
			Gen_Cfg = Default_cfg;
			for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
				int LastSubfilterH = (subfilter_j+1) >= Fy;
				for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
					int DoPad = ((subfilter_i*Dx) < (PadL-w*Sx))?1:0;
				        int LastSubfilterW = (subfilter_i+1) >= Fx;
					if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
						// Do not apply reduction if not last
						Gen_Cfg |= set_quant_out_flag;
						if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
					} else {
						Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
					}
				        // acquire job
				        NE16_BARRIER_ACQUIRE(job_id);
					SetNE16_InPointer     ((void *) Max((unsigned int) In, (unsigned int) InPointer + Tile_InFeat*(subfilter_i*Dx + w*Sx-PadL + Tile_InW*(Ho_F*Sy-PadT + subfilter_j*Dy))));
					SetNE16_OutPointer    (Out + Tile_OutFeat*(w + Ho_F*Tile_OutW));
					// TODO - checkme I think here you need the total number of loaded chin
					SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
					SetNE16_Reminders     (1, Rem_H_Subtiles, Rem_KI, Rem_KO, 1, Rem_H_Subtiles);
					SetNE16_Dim           (Nb_KI, Nb_KO, 1, H_SubTiles);
					SetNE16_GenConfig     (Gen_Cfg);
					SetNE16_BiasPointer   (Bias);
					SetNE16_ScalePointer  (Scale);
					SetNE16_ScaleNPointer (ScaleN);
					SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
							       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
							       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
					SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
					SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
					SetNE16_WOffset       (Arg->W_Offset);
					// Always Apply streamin because you preset the bias on the borders
					Gen_Cfg |= streamin_flag;
					// already commit and trigger NE16 computation, while programming the next one
					NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
				}
			}
		}	
	}
	if (PadR && H_SubTiles) {
		InPointer = In; // + Tile_InFeat*(Wo_L*Sx-PadL + Tile_InW*(Ho_F*Sy-PadT));
		for (int w=Wo_L; w<Wo; w++) {
			Gen_Cfg = Default_cfg;
			for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
				int LastSubfilterH = (subfilter_j+1) >= Fy;
				for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
					int DoPad = ((subfilter_i*Dx + w*Sx-PadL) >= (Tile_InW))?1:0;
				        int LastSubfilterW = (subfilter_i+1) >= Fx;
					if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
						// Do not apply reduction if not last
						Gen_Cfg |= set_quant_out_flag;
						if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
					} else {
						Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
					}
				        // acquire job
				        NE16_BARRIER_ACQUIRE(job_id);
					SetNE16_InPointer     (InPointer + Tile_InFeat*(w*Sx-PadL + subfilter_i*Dx + Tile_InW*(Ho_F*Sy-PadT + subfilter_j*Dy)));
					SetNE16_OutPointer    (Out + Tile_OutFeat*(w + Ho_F*Tile_OutW));
					// TODO - checkme I think here you need the total number of loaded chin
					SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
					SetNE16_Reminders     (1, Rem_H_Subtiles, Rem_KI, Rem_KO, 1, Rem_H_Subtiles);
					SetNE16_Dim           (Nb_KI, Nb_KO, 1, H_SubTiles);
					SetNE16_GenConfig     (Gen_Cfg);
					SetNE16_BiasPointer   (Bias);
					SetNE16_ScalePointer  (Scale);
					SetNE16_ScaleNPointer (ScaleN);
					SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
							       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
							       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
					SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
					SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
					SetNE16_WOffset       (Arg->W_Offset);
					// Always Apply streamin because you preset the bias on the borders
					Gen_Cfg |= streamin_flag;
					// already commit and trigger NE16 computation, while programming the next one
					NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
				}
			}
		}	
	}
	if (PadT && W_SubTiles) {
		InPointer = In;
		for (int h=0; h<Ho_F; h++) {
			Gen_Cfg = Default_cfg;
			for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
				int DoPad = ((subfilter_j*Dy) < (PadT-h*Sy))?1:0;
				int LastSubfilterH = (subfilter_j+1) >= Fy;
				for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
				        int LastSubfilterW = (subfilter_i+1) >= Fx;
					if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
						// Do not apply reduction if not last
						Gen_Cfg |= set_quant_out_flag;
						if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
					} else {
						Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
					}
				        // acquire job
				        NE16_BARRIER_ACQUIRE(job_id);
					SetNE16_InPointer     ((void *) Max((unsigned int) InPointer, (unsigned int) InPointer + Tile_InFeat*(Wo_F*Sx-PadL + subfilter_i*Dx + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW)));
					SetNE16_OutPointer    (Out + Tile_OutFeat*(Wo_F + h*Tile_OutW));
					// TODO - checkme I think here you need the total number of loaded chin
					SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
					SetNE16_Reminders     (Rem_W_Subtiles, 1, Rem_KI, Rem_KO, Rem_W_Subtiles, 1);
					SetNE16_Dim           (Nb_KI, Nb_KO, W_SubTiles, 1);
					SetNE16_GenConfig     (Gen_Cfg);
					SetNE16_BiasPointer   (Bias);
					SetNE16_ScalePointer  (Scale);
					SetNE16_ScaleNPointer (ScaleN);
					SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    	    // In_D0, In_D1, In_D2 - unused
							       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
							       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   		    // Weights_D0, Weights_D1, Weights_D2
					SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
					SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
					SetNE16_WOffset       (Arg->W_Offset);
					// Always Apply streamin because you preset the bias on the borders
					Gen_Cfg |= streamin_flag;
					// already commit and trigger NE16 computation, while programming the next one
					NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
				}
			}
		}	
	}
	if (PadB && W_SubTiles) {
		InPointer = In; // + Tile_InFeat*Tile_InW*(-PadT+(Tile_OutH-PadB)*Sy);
		for (unsigned int h=Ho_L; h<Ho; h++) {
			Gen_Cfg = Default_cfg;
			for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
				int DoPad = ((subfilter_j*Dy + h*Sy-PadT) >= Tile_InH)?1:0;
				int LastSubfilterH = (subfilter_j+1) >= Fy;
				for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
				        int LastSubfilterW = (subfilter_i+1) >= Fx;
					if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
						// Do not apply reduction if not last
						Gen_Cfg |= set_quant_out_flag;
						if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
					} else {
						Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
					}
				        // acquire job
				        NE16_BARRIER_ACQUIRE(job_id);
					SetNE16_InPointer     (InPointer + Tile_InFeat*(Wo_F*Sx-PadL + subfilter_i*Dx + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW));
					SetNE16_OutPointer    (Out + Tile_OutFeat*(Wo_F + h*Tile_OutW));
					// TODO - checkme I think here you need the total number of loaded chin
					SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
					SetNE16_Reminders     (Rem_W_Subtiles, 1, Rem_KI, Rem_KO, Rem_W_Subtiles, 1);
					SetNE16_Dim           (Nb_KI, Nb_KO, W_SubTiles, 1);
					SetNE16_GenConfig     (Gen_Cfg);
					SetNE16_BiasPointer   (Bias);
					SetNE16_ScalePointer  (Scale);
					SetNE16_ScaleNPointer (ScaleN);
					SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    	    // In_D0, In_D1, In_D2 - unused
							       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
							       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   		    // Weights_D0, Weights_D1, Weights_D2
					SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
					SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
					SetNE16_WOffset       (Arg->W_Offset);
					// Always Apply streamin because you preset the bias on the borders
					Gen_Cfg |= streamin_flag;
					// already commit and trigger NE16 computation, while programming the next one
					NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
				}
			}
		}	
	}
	/* Corners */
	if (PadL && PadT) {
		InPointer = In;
		for (int h=0; h<Ho_F; h++) {
			for (int w=0; w<Wo_F; w++) {
				Gen_Cfg = Default_cfg;
				for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
					int LastSubfilterH = (subfilter_j+1) >= Fy;
					for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
						int DoPad = (((subfilter_j*Dy) < (PadT-h*Sy)) || ((subfilter_i*Dx) < (PadL-w*Sx)))?1:0;
					        // acquire job
					        NE16_BARRIER_ACQUIRE(job_id);
					        int LastSubfilterW = (subfilter_i+1) >= Fx;
						if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
							// Do not apply reduction if not last
							Gen_Cfg |= set_quant_out_flag;
							if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						} else {
							Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
							if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
						}
						SetNE16_InPointer     ((void *) Max((unsigned int) In, (unsigned int) InPointer + Tile_InFeat*(subfilter_i*Dx + w*Sx - PadL + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW)));
						SetNE16_OutPointer    (Out + Tile_OutFeat*(w + h*Tile_OutW));
						// TODO - checkme I think here you need the total number of loaded chin
						SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
						SetNE16_Reminders     (1, 1, Rem_KI, Rem_KO, 1, 1);
						SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
						SetNE16_GenConfig     (Gen_Cfg);
						SetNE16_BiasPointer   (Bias);
						SetNE16_ScalePointer  (Scale);
						SetNE16_ScaleNPointer (ScaleN);
						SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
								       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
								       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
						SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
						SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
						SetNE16_WOffset       (Arg->W_Offset);
						// Always Apply streamin because you preset the bias on the borders
						Gen_Cfg |= streamin_flag;
						// already commit and trigger NE16 computation, while programming the next one
						NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
					}
				}
			}
		}
	}
	if (PadR && PadT) {
		Gen_Cfg = Default_cfg;
		InPointer = In; // + Tile_InFeat*(-PadL+(Tile_OutW-PadR)*Sx-PadT*Tile_InW);
		for (int h=0; h<Ho_F; h++) {
			for (int w=Wo_L; w<Wo; w++) {
				Gen_Cfg = Default_cfg;
				for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
					int LastSubfilterH = (subfilter_j+1) >= Fy;
					for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
						int DoPad = (((subfilter_i*Dx + w*Sx-PadL) >= (Tile_InW)) || ((subfilter_j*Dy) < (PadT-h*Sy)))?1:0;
					        // acquire job
					        NE16_BARRIER_ACQUIRE(job_id);
					        int LastSubfilterW = (subfilter_i+1) >= Fx;
						if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
							// Do not apply reduction if not last
							Gen_Cfg |= set_quant_out_flag;
							if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						} else {
							Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
							if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
						}
						SetNE16_InPointer     ((void *) Max((unsigned int) In, (unsigned int) InPointer + Tile_InFeat*(subfilter_i*Dx + (w*Sx-PadL) + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW)));
						SetNE16_OutPointer    (Out + Tile_OutFeat*(w + h*Tile_OutW));
						// TODO - checkme I think here you need the total number of loaded chin
						SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
						SetNE16_Reminders     (1, 1, Rem_KI, Rem_KO, 1, 1);
						SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
						SetNE16_GenConfig     (Gen_Cfg);
						SetNE16_BiasPointer   (Bias);
						SetNE16_ScalePointer  (Scale);
						SetNE16_ScaleNPointer (ScaleN);
						SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
								       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
								       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
						SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
						SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
						SetNE16_WOffset       (Arg->W_Offset);
						// Always Apply streamin because you preset the bias on the borders
						Gen_Cfg |= streamin_flag;
						// already commit and trigger NE16 computation, while programming the next one
						NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
					}
				}
			}
		}	
	}
	if (PadB && PadL) {
		Gen_Cfg = Default_cfg;
		InPointer = In;
		for (unsigned int h=Ho_L; h<Ho; h++) {
			for (int w=0; w<Wo_F; w++) {
				Gen_Cfg = Default_cfg;
				for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
					int LastSubfilterH = (subfilter_j+1) >= Fy;
					for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
						int DoPad = ((((subfilter_j*Dy + h*Sy-PadT) >= Tile_InH) || ((subfilter_i*Dx) < (PadL-w*Sx))))?1:0;
					        // acquire job
					        NE16_BARRIER_ACQUIRE(job_id);
					        int LastSubfilterW = (subfilter_i+1) >= Fx;
						if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
							// Do not apply reduction if not last
							Gen_Cfg |= set_quant_out_flag;
							if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						} else {
							Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
							if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
						}
						SetNE16_InPointer     ((void *) Max((unsigned int) In, (unsigned int) InPointer + Tile_InFeat*(subfilter_i*Dx + w*Sx - PadL + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW)));
						SetNE16_OutPointer    (Out + Tile_OutFeat*(h*Tile_OutW + w));
						// TODO - checkme I think here you need the total number of loaded chin
						SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
						SetNE16_Reminders     (1, 1, Rem_KI, Rem_KO, 1, 1);
						SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
						SetNE16_GenConfig     (Gen_Cfg);
						SetNE16_BiasPointer   (Bias);
						SetNE16_ScalePointer  (Scale);
						SetNE16_ScaleNPointer (ScaleN);
						SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
								       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
								       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
						SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
						SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
						SetNE16_WOffset       (Arg->W_Offset);
						// Always Apply streamin because you preset the bias on the borders
						Gen_Cfg |= streamin_flag;
						// already commit and trigger NE16 computation, while programming the next one
						NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
					}
				}
			}
		}
	}
	if (PadB && PadR) {
		Gen_Cfg = Default_cfg;
		InPointer = In; // + Tile_InFeat*(-PadL);
		for (unsigned int h=Ho_L; h<Ho; h++) {
			for (int w=Wo_L; w<Wo; w++) {
				Gen_Cfg = Default_cfg;
				for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
					int LastSubfilterH = (subfilter_j+1) >= Fy;
					for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
						int DoPad = (((subfilter_j*Dy + h*Sy-PadT) >= Tile_InH) || (subfilter_i*Dx + w*Sx-PadL) >= (Tile_InW))?1:0;
					        // acquire job
					        NE16_BARRIER_ACQUIRE(job_id);
					        int LastSubfilterW = (subfilter_i+1) >= Fx;
						if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
							// Do not apply reduction if not last
							Gen_Cfg |= set_quant_out_flag;
							if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
						} else {
							Gen_Cfg = (Gen_Cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
							if (!Arg->FirstD0) Gen_Cfg |= streamin_flag;
						}
						SetNE16_InPointer     (InPointer + Tile_InFeat*(subfilter_i*Dx + w*Sx-PadL + (subfilter_j*Dy+h*Sy-PadT)*Tile_InW));
						SetNE16_OutPointer    (Out + Tile_OutFeat*(h*Tile_OutW + w));
						// TODO - checkme I think here you need the total number of loaded chin
						SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*Fx*subfilter_j);
						SetNE16_Reminders     (1, 1, Rem_KI, Rem_KO, 1, 1);
						SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
						SetNE16_GenConfig     (Gen_Cfg);
						SetNE16_BiasPointer   (Bias);
						SetNE16_ScalePointer  (Scale);
						SetNE16_ScaleNPointer (ScaleN);
						SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*Tile_InW, 0,	    // In_D0, In_D1, In_D2 - unused
								       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
								       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
						SetNE16_ConfigPad     ((v4s) {DoPad, DoPad, DoPad, DoPad}, Arg->Pad_Val);
						SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
						SetNE16_WOffset       (Arg->W_Offset);
						// Always Apply streamin because you preset the bias on the borders
						Gen_Cfg |= streamin_flag;
						// already commit and trigger NE16 computation, while programming the next one
						NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
					}
				}
			}
		}
	}
}

void NE16_PrepareJob(Ker_NE16_Job_T *Arg)
{
	if (Arg->LastT0 && Arg->LastD0) return;
	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	int FilterMode = (Arg->Default_NE16_Job_Cfg >> NE16_SHIFT_FILTER_MODE) & NE16_MASK_FILTER_MODE;
	int Nb_KI = Tile_InFeat/16  + (Tile_InFeat%16?1:0),  Rem_KI = Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO, Rem_KO;
	if (FilterMode == 1) Nb_KO = Nb_KI, Rem_KO = Rem_KI;
	else 		     Nb_KO = Tile_OutFeat/32 + (Tile_OutFeat%32?1:0), Rem_KO = Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Rem_WO = Tile_OutW % 3, Nb_WO = Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO = Tile_OutH % 3, Nb_HO = Tile_OutH / 3 + (Rem_HO?1:0);
	int QuantBitsFlag = (Arg->Default_NE16_Job_Cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes = (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int PadL = 0, PadR = 0, PadB = 0, PadT = 0;
	int Rem_WI, Rem_HI, W_Stride0, W_Stride1;

	unsigned int Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	Gen_Cfg = Default_cfg;
	if (FilterMode == 2) {
		Rem_WI = Rem_WO, Rem_HI = Rem_HO;
		W_Stride0 = 2*Arg->Qw;
		W_Stride1 = Nb_KI*16*Arg->Qw/8;
	} else {
		Rem_WI = Rem_WO?(Rem_WO+2):0, Rem_HI = Rem_HO?(Rem_HO+2):0;
		W_Stride0 = 2*3*3;
		W_Stride1 = (FilterMode==1)?0:(2*3*3*Arg->Qw*Nb_KI);
		PadL = Arg->Pad[0], PadT = Arg->Pad[2];
		PadR = Arg->Pad[1]?((Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1]):0, PadB = Arg->Pad[3]?((Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3]):0;
	}
	if (FilterMode != 1){
		Gen_Cfg = Default_cfg;
		if (!Arg->LastD0){
			// Do not apply reduction if not last
			Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
		}
		if (!Arg->FirstD0){
			Gen_Cfg |= SET_STREAMIN;
		}
	}

        volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        // acquire job
        NE16_BARRIER_ACQUIRE(job_id);

	// load configuration for the layer
	SetNE16_InPointer     (Arg->In - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer    (Arg->Out);
	SetNE16_WeightsPointer(Arg->Filter);
	SetNE16_BiasPointer   (Arg->Bias);
	SetNE16_ScalePointer  (Arg->Scale);
	SetNE16_ScaleNPointer (Arg->ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0,					  // In_D0, In_D1, In_D2 - unused
			       Out_Stride0, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW, // Out_D0, Out_D1, Out_D2
			       W_Stride0, W_Stride1, 0);				                  // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	SetNE16_ConfigPad     ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);
}

void KerConv3x3Stride1_NE16(KerConv_NE16_T *Arg) {
	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}

	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32; // TO CHECK !!!!!! may be only Tile_OutFeat%32
	int Rem_WO	= Tile_OutW % 3;
	int Nb_WO	= Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO	= Tile_OutH % 3;
	int Nb_HO	= Tile_OutH / 3 + (Rem_HO?1:0);
	int Rem_WI	= Rem_WO?(Rem_WO+2):0;
	int Rem_HI	= Rem_HO?(Rem_HO+2):0;
        int W_Stride1   = 2*3*3*Arg->Qw*Nb_KI;
        unsigned int Gen_Cfg = Default_cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
        int PadL = Arg->Pad[0], PadT = Arg->Pad[2], PadR = Arg->Pad[1]?((Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1]):0, PadB = Arg->Pad[3]?((Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3]):0;

	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}

        volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        // acquire job
        NE16_BARRIER_ACQUIRE(job_id);

	SetNE16_InPointer     (Arg->In - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer    (Arg->Out);
	SetNE16_WeightsPointer(Arg->Filter);
	SetNE16_BiasPointer   (Arg->Bias);
	SetNE16_ScalePointer  (Arg->Scale);
	SetNE16_ScaleNPointer (Arg->ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 5*5*Tile_InFeat,			// In_D0, In_D1, In_D2 - unused
			       Out_Stride0, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2
			       (Mode16?1:2)*3*3, (Mode16?1:2)*3*3*Arg->Qw*Nb_KI, 0);	
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	SetNE16_ConfigPad     ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConvDW3x3Stride1_NE16(KerConv_NE16_T *Arg)
{	
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Nb_KI;
	int Rem_KO	= Rem_KI;
	int Rem_WO	= Tile_OutW % 3;
	int Nb_WO	= Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO	= Tile_OutH % 3;
	int Nb_HO	= Tile_OutH / 3 + (Rem_HO?1:0);
	int Rem_WI	= Rem_WO?(Rem_WO+2):0;
	int Rem_HI	= Rem_HO?(Rem_HO+2):0;
        int W_Stride1   = 0;
        unsigned int quant_out_flag, streamin_flag, Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2], PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1], PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	Gen_Cfg = Default_cfg;

        volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        // acquire job
        NE16_BARRIER_ACQUIRE(job_id);

	// load configuration for the layer
	SetNE16_InPointer     (In - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer    (Out);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				// In_D0, In_D1, In_D2 - unused
			       Out_Stride0, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2
			       2*3*3, W_Stride1, 0);				                        // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	SetNE16_ConfigPad     ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv3x3Stride1_DxDy_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}
	int Dx 		= Arg->Dx;
	int Dy 		= Arg->Dy;
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int Nb_WO = 1;
	int Nb_HO = 1;
	int Last_Rem_WO, Last_Rem_HO, Last_Rem_HI, Last_Rem_WI;

        int W_Stride1   = 2*3*3*Arg->Qw*Nb_KI;
        unsigned int Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}

        volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        for (int j=0; j<Dy; j++) {
        	int HO = (((Tile_OutH - j) / Dy) + (((Tile_OutH - j) % Dy)?1:0));
		int Nb_HO_Subtiles =  HO/3 + (HO%3?1:0);
		Last_Rem_HO = HO % 3;
		Last_Rem_HI = Last_Rem_HO?(Last_Rem_HO+2):0;
        	for (int i=0; i<Dx; i++) {
        		int WO = (((Tile_OutW - i) / Dx) + (((Tile_OutW - i) % Dx)?1:0));
			int Nb_WO_Subtiles = WO/3 + (WO%3?1:0);
			Last_Rem_WO = WO % 3;
			Last_Rem_WI = Last_Rem_WO?(Last_Rem_WO+2):0;
			int PadR = (Last_Rem_WI?(5 - Last_Rem_WI):0) + Arg->Pad[1], PadB = (Last_Rem_HI?(5 - Last_Rem_HI):0) + Arg->Pad[3];
        		for (int ho=0; ho<Nb_HO_Subtiles; ho++){
		        	for (int wo=0; wo<Nb_WO_Subtiles; wo++){
					NE16_BARRIER_ACQUIRE(job_id);
					SetNE16_WeightsPointer(Filter);
					SetNE16_BiasPointer   (Bias);
					SetNE16_ScalePointer  (Scale);
					SetNE16_ScaleNPointer (ScaleN);
					/* First JOB */
					// TODO - Check spatial counters
					SetNE16_InPointer (In  + Tile_InFeat*(Dx*3*wo+i) + Tile_InFeat*Tile_InW*(Dy*3*ho+j) - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
					SetNE16_OutPointer(Out + Tile_OutFeat*OutBytes*(Dx*3*wo+i) + Tile_OutFeat*Tile_OutW*OutBytes*(Dy*3*ho+j));
					// Strides consider dilation, i.e. 2 consecutive pixels in the W dimension will be at Dx*Chin stride
					SetNE16_Strides       (Dx * Tile_InFeat, Dy * Tile_InFeat * Tile_InW, 0, 				// In_D0, In_D1, In_D2 - unused
							       Out_Stride0, Dx * OutBytes * Tile_OutFeat, Dy * OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2
							       (Mode16?1:2)*3*3, (Mode16?1:2)*3*3*Arg->Qw*Nb_KI, 0);
					// load configuration for the layer
					SetNE16_Reminders (wo==(Nb_WO_Subtiles-1)?Last_Rem_WI:0, ho==(Nb_HO_Subtiles-1)?Last_Rem_HI:0, Rem_KI, Rem_KO, 
							   wo==(Nb_WO_Subtiles-1)?Last_Rem_WO:0, ho==(Nb_HO_Subtiles-1)?Last_Rem_HO:0);
					SetNE16_Dim       (Nb_KI, Nb_KO, 1, 1);
					SetNE16_ConfigPad  ((v4s) {wo==0?PadL:0, wo==(Nb_WO_Subtiles-1)?PadR:0, ho==0?PadT:0, ho==(Nb_HO_Subtiles-1)?PadB:0}, Arg->Pad_Val);
					SetNE16_ConfigFMask((v4s) {0, 0, 0, 0});
					SetNE16_WOffset    (Arg->W_Offset);
					SetNE16_GenConfig  (Gen_Cfg);

					// commit and trigger NE16 computation
					NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
		        	}
		        }
		}
	}

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv3x3Stride2_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}
	// In each subtile we compute 2 output pixel in both spatial dimensions
        int H_subtiles  = Tile_OutH / 2 + (Tile_OutH % 2);
        int W_subtiles  = Tile_OutW / 2 + (Tile_OutW % 2);
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Rem_WO	= 0;
	int Nb_WO	= 1;
	int Rem_HO	= 0;
	int Nb_HO	= 1;
	int Rem_WI	= 0;
	int Rem_HI	= 0;
	int Last_Rem_HO = Tile_OutH % 2;
	int Last_Rem_WO = Tile_OutW % 2;
	int W_Stride1   = 2*3*3*Arg->Qw*Nb_KI;
	int IsLastSubtileW = 0;
	int IsLastSubtileH = 0;

	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int TilePadL = PadL, TilePadT = PadT;
	int PadR = 0, PadB = 0;
        unsigned int Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int ApplyRect = (Default_cfg & (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT)) == 0;

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	// acquire first job
	NE16_BARRIER_ACQUIRE(job_id);

	// define strided conv iteration indeces
	int subtile_i_major=0;
	int subtile_j_major=0;
	IsLastSubtileH = subtile_i_major==(H_subtiles-1);
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);
	// update input / output pointers
	if(IsLastSubtileH && IsLastSubtileW) {
		Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileH) {
		Rem_HO = Last_Rem_HO;  Rem_WO = 0;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileW) {
		Rem_HO = 0; Rem_WO = Last_Rem_WO;
		Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = 0;
	}
	else {
		Rem_HO = 0; Rem_WO = 0;
		Rem_HI = 0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = 0;
	}
	// preload all common configuration for the layer in the first job
       	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 	        			// In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       (Mode16?1:2)*3*3, (Mode16?1:2)*3*3*Arg->Qw*Nb_KI, 0);                                				// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Assume first subtile no need for right/bottom pad
	SetNE16_ConfigPad     ((v4s) {PadL, IsLastSubtileW?PadR:0, PadT, IsLastSubtileH?PadB:0}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	subtile_j_major ++;
	if(subtile_j_major==W_subtiles) {
		subtile_j_major = 0;
		subtile_i_major ++;
		IsLastSubtileH = subtile_i_major>=(H_subtiles-1);
	}
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// already commit and trigger NE16 computation, while programming the next one
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	if (IsLastSubtileH && IsLastSubtileW) {
		// wait for end of computation
		NE16_BARRIER();
		// set priority to core side
		NE16_SETPRIORITY_CORE();
		return;
	}

	// acquire second job
	NE16_BARRIER_ACQUIRE(job_id);

	// update input / output pointers
	if(IsLastSubtileH && IsLastSubtileW) {
		Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileH) {
		Rem_HO = Last_Rem_HO;  Rem_WO = 0;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileW) {
		Rem_HO = 0; Rem_WO = Last_Rem_WO;
		Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = 0;
	}
	else {
		Rem_HO = 0; Rem_WO = 0;
		Rem_HI = 0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = 0;
	}
	// preload all common configuration for the layer in the second job
	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				        // In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       (Mode16?1:2)*3*3, (Mode16?1:2)*3*3*Arg->Qw*Nb_KI, 0);				                                // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Moving to next spatial subtile means consider less padding (2 because of the stride)
	PadL = Max(0, TilePadL-2*subtile_j_major);
	PadT = Max(0, TilePadT-2*subtile_i_major);
	SetNE16_ConfigPad     ((v4s) {PadL, IsLastSubtileW?PadR:0, PadT, IsLastSubtileH?PadB:0}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	subtile_j_major ++;
	if(subtile_j_major==W_subtiles) {
		subtile_j_major = 0;
		subtile_i_major ++;
		IsLastSubtileH = subtile_i_major>=(H_subtiles-1);
	}
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// already commit and trigger NE16 computation, while programming the next one
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	if (IsLastSubtileH && IsLastSubtileW) {
		// wait for end of computation
		NE16_BARRIER();
		// set priority to core side
		NE16_SETPRIORITY_CORE();
		return;
	}

	// main iteration strided conv iteration loop (does not need ne16_c0_config any more)
	do {
                // acquire job
                NE16_BARRIER_ACQUIRE(job_id);

		IsLastSubtileH = subtile_i_major>=(H_subtiles-1);
		IsLastSubtileW = subtile_j_major>=(W_subtiles-1);
		// update input / output pointers
		if(IsLastSubtileH && IsLastSubtileW) {
			Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(IsLastSubtileH) {
			Rem_HO = Last_Rem_HO;  Rem_WO = 0;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(IsLastSubtileW) {
			Rem_HO = 0; Rem_WO = Last_Rem_WO;
			Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = 0;
		}
		else {
			Rem_HO = 0; Rem_WO = 0;
			Rem_HI = 0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = 0;
		}
		// Moving to next spatial subtile means consider less padding (2 because of the stride)
		PadL = Max(0, TilePadL-2*subtile_j_major);
		PadT = Max(0, TilePadT-2*subtile_i_major);
		SetNE16_ConfigPad ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);

		SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
		SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);

		// commit and trigger NE16 computation
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

		// update indeces
		subtile_j_major ++;
		if(subtile_j_major==W_subtiles) {
			subtile_j_major = 0;
			subtile_i_major ++;
		}

	} while((subtile_i_major<H_subtiles) && (subtile_j_major<W_subtiles));

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConvDW3x3Stride2_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	// In each subtile we compute 2 output pixel in both spatial dimensions
        int H_subtiles  = Tile_OutH / 2 + (Tile_OutH % 2);
        int W_subtiles  = Tile_OutW / 2 + (Tile_OutW % 2);
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Nb_KI;
	int Rem_KO	= Rem_KI;
	int Rem_WO	= 0;
	int Nb_WO	= 1;
	int Rem_HO	= 0;
	int Nb_HO	= 1;
	int Rem_WI	= 0;
	int Rem_HI	= 0;
	int Last_Rem_HO = Tile_OutH % 2;
	int Last_Rem_WO = Tile_OutW % 2;
	int W_Stride1   = 0;
	int IsLastSubtileW = 0;
	int IsLastSubtileH = 0;

	int PadL = Arg->Pad[0], PadR = Arg->Pad[1], PadT = Arg->Pad[2], PadB = Arg->Pad[3];
	int TilePadL = PadL, TilePadT = PadT;
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	Gen_Cfg = Default_cfg;

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	// define strided conv iteration indeces
	int subtile_i_major=0;
	int subtile_j_major=0;
	IsLastSubtileH = subtile_i_major==(H_subtiles-1);
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// acquire first job
	NE16_BARRIER_ACQUIRE(job_id);
	// update input / output pointers
	if(IsLastSubtileH && IsLastSubtileW) {
		Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileH) {
		Rem_HO = Last_Rem_HO;  Rem_WO = 0;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileW) {
		Rem_HO = 0; Rem_WO = Last_Rem_WO;
		Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = 0;
	}
	else {
		Rem_HO = 0; Rem_WO = 0;
		Rem_HI = 0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = 0;
	}
	// preload all common configuration for the layer in the first job
       	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 	        			// In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, 2*3*3*Arg->Qw*Nb_KI, 0);                                				// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Assume first subtile no need for right/bottom pad
	SetNE16_ConfigPad     ((v4s) {PadL, IsLastSubtileW?PadR:0, PadT, IsLastSubtileH?PadB:0}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	subtile_j_major ++;
	if(subtile_j_major==W_subtiles) {
		subtile_j_major = 0;
		subtile_i_major ++;
		IsLastSubtileH = subtile_i_major==(H_subtiles-1);
	}
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// already commit and trigger NE16 computation, while programming the next one
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	if (IsLastSubtileH && IsLastSubtileW) {
		// wait for end of computation
		NE16_BARRIER();
		// set priority to core side
		NE16_SETPRIORITY_CORE();
		return;
	}

	// acquire second job
	NE16_BARRIER_ACQUIRE(job_id);

	// update input / output pointers
	if(IsLastSubtileH && IsLastSubtileW) {
		Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileH) {
		Rem_HO = Last_Rem_HO;  Rem_WO = 0;
		Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
	}
	else if(IsLastSubtileW) {
		Rem_HO = 0; Rem_WO = Last_Rem_WO;
		Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
		PadB = 0;
	}
	else {
		Rem_HO = 0; Rem_WO = 0;
		Rem_HI = 0; Rem_WI = 0;
		SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
		PadR = 0;
		PadB = 0;
	}
	// preload all common configuration for the layer in the second job
	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major  - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				        // In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, 2*3*3*Arg->Qw*Nb_KI, 0);				                                // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Moving to next spatial subtile means consider less padding (2 because of the stride)
	PadL = Max(0, TilePadL-2*subtile_j_major);
	PadT = Max(0, TilePadT-2*subtile_i_major);
	SetNE16_ConfigPad     ((v4s) {PadL, IsLastSubtileW?PadR:0, PadT, IsLastSubtileH?PadB:0}, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_GenConfig     (Gen_Cfg);

	subtile_j_major ++;
	if(subtile_j_major==W_subtiles) {
		subtile_j_major = 0;
		subtile_i_major ++;
		IsLastSubtileH = subtile_i_major==(H_subtiles-1);
	}
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// already commit and trigger NE16 computation, while programming the next one
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	if (IsLastSubtileH && IsLastSubtileW) {
		// wait for end of computation
		NE16_BARRIER();
		// set priority to core side
		NE16_SETPRIORITY_CORE();
		return;
	}

	// main iteration strided conv iteration loop (does not need ne16_c0_config any more)
	do {
	        // acquire second job
	        NE16_BARRIER_ACQUIRE(job_id);

		IsLastSubtileH = subtile_i_major==(H_subtiles-1);
		IsLastSubtileW = subtile_j_major==(W_subtiles-1);
		// update input / output pointers
		if(IsLastSubtileH && IsLastSubtileW) {
			Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(IsLastSubtileH) {
			Rem_HO = Last_Rem_HO;  Rem_WO = 0;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(IsLastSubtileW) {
			Rem_HO = 0; Rem_WO = Last_Rem_WO;
			Rem_HI = 0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = 0;
		}
		else {
			Rem_HO = 0; Rem_WO = 0;
			Rem_HI = 0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = 0;
		}
		// Moving to next spatial subtile means consider less padding (2 because of the stride)
		PadL = Max(0, TilePadL-2*subtile_j_major);
		PadT = Max(0, TilePadT-2*subtile_i_major);
		SetNE16_ConfigPad ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);

		SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*TilePadL - Tile_InFeat*Tile_InW*TilePadT);
		SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);

		// commit and trigger NE16 computation
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

		// update indeces
		subtile_j_major ++;
		if(subtile_j_major==W_subtiles) {
			subtile_j_major = 0;
			subtile_i_major ++;
		}

	} while((subtile_i_major<H_subtiles) && (subtile_j_major<W_subtiles));

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv1D_StrideS_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In     = (unsigned char *__restrict__) Arg->In;
	unsigned char * __restrict__ Out    = (unsigned char *__restrict__) Arg->Out;
	int 	      * __restrict__ Bias   = (int           *__restrict__) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *__restrict__) Arg->Filter;
	unsigned char * __restrict__ Scale  = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int Tile_InFeat  = Arg->Tile_InFeat, Tile_InW = Arg->Tile_InW;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = 3;
	int Sx = Arg->Sx, Fx = Arg->Fx, Dx = Arg->Dx;

	int Wo = (Tile_InW-Fx+Arg->Pad[0]+Arg->Pad[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(Fx, Arg->Pad[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Tile_InW, Fx, Arg->Pad[0], Sx, Dx));

	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Nb_LoadedKI = Arg->TotalInFeatures/(Mode16?8:16) + (Arg->TotalInFeatures%(Mode16?8:16)?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;

	int NoPadWo = (Wo_L - Wo_F);
	int SubtileRem = (NoPadWo) % 9;
	int LastSubtileHeight = SubtileRem / 3;
	int LastRemW = SubtileRem % 3;
	int LastSubtileSize = LastSubtileHeight?(LastSubtileHeight*3):9;

	int Rem_WI = 0, Rem_HI = 0;
	unsigned int Gen_Cfg = Default_cfg;
	int ApplyRect = (Default_cfg & (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT)) == 0;

	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		Gen_Cfg |= SET_STREAMIN;
	}
	int QuantBitsFlag = (Gen_Cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        if ((int) Arg->Pad) {
        	Arg->Fy=1; Arg->Sy=1; Arg->Dy=1;
        	NE16_ComputeBorders(Arg, Wo_F, Wo_L, Wo, 0, 1, 0);
        }
        int subfilter_i = 0, w = Wo_F, LastSubtile = 0, SubTileCount = 0;
        do {
	        // acquire second job
	        NE16_BARRIER_ACQUIRE(job_id);

        	int LastSubtileW   = w >= (Wo_L - LastSubtileSize - LastRemW);
		int LastSubfilterW = subfilter_i >= (Fx-1);
		if (Arg->LastD0 && LastSubfilterW){
			// Do not apply reduction if not last
			if (!Mode16) Gen_Cfg |= SET_QUANTOUT;
			if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
		} else {
			Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
		}
		// Apply streamin after finished the first subfilter
		if (subfilter_i > 0) Gen_Cfg = Gen_Cfg | SET_STREAMIN;

		unsigned char *pIn   = In + Tile_InFeat*(w*Sx - Arg->Pad[0] + subfilter_i*Dx);
		unsigned char *pOut  = Out    + OutBytes*Tile_OutFeat*w;
		unsigned char *pFilt = Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*subfilter_i;
		if (LastSubtileW) {
			Rem_HI = LastSubtileHeight;
			w += LastSubtileSize;
			if (w >= Wo_L){
				if (LastRemW) {Rem_WI = LastRemW, Rem_HI = 1;}
				w = Wo_F;
				subfilter_i++;
				LastSubtile = LastSubfilterW;
			}
		} else {
			Rem_WI = 0, Rem_HI = 0;
			w += 9;
		}

		// preload all common configuration for the layer in the first job
		// update input / output pointers
		SetNE16_InPointer     (pIn);
		SetNE16_OutPointer    (pOut);
		SetNE16_WeightsPointer(pFilt);
		SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WI, Rem_HI);
		SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
		SetNE16_GenConfig     (Gen_Cfg);
		if (SubTileCount < 2) {
			SetNE16_BiasPointer   (Bias);
			SetNE16_ScalePointer  (Scale);
			SetNE16_ScaleNPointer (ScaleN);
			SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sx*3, 0,		    // In_D0, In_D1, In_D2 - unused
					       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*3, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
					       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);			  	    // Weights_D0, Weights_D1, Weights_D2
			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
			SetNE16_WOffset       (Arg->W_Offset);
			SubTileCount++;
		}
		// already commit and trigger NE16 computation, while programming the next one
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        } while (!LastSubtile);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConvNxMDxDy_StrideSxSy_NE16(KerConv_NE16_T *Arg) 
{
	unsigned char * __restrict__ In     = (unsigned char *__restrict__) Arg->In;
	unsigned char * __restrict__ Out    = (unsigned char *__restrict__) Arg->Out;
	int 	      * __restrict__ Bias   = (int           *__restrict__) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *__restrict__) Arg->Filter;
	unsigned char * __restrict__ Scale  = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat, Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

	unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	unsigned int Gen_Cfg = Default_cfg;
	int ApplyRect = (Default_cfg & (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT)) == 0;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}
	int OutBytes	= 4;
	int Out_Stride0 = 32;
	int SubTileCount = 0;

	int Sx = Arg->Sx, Fx = Arg->Fx, Dx = Arg->Dx;
	int Sy = Arg->Sy, Fy = Arg->Fy, Dy = Arg->Dy;
	int Ho = (Tile_InH-Dy*(Fy-1)-1+Arg->Pad[2]+Arg->Pad[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(Fy, Arg->Pad[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Tile_InH, Fy, Arg->Pad[2], Sy, Dy));
	int Wo = (Tile_InW-Dx*(Fx-1)-1+Arg->Pad[0]+Arg->Pad[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(Fx, Arg->Pad[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Tile_InW, Fx, Arg->Pad[0], Sx, Dx));
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Nb_LoadedKI = Arg->TotalInFeatures/(Mode16?8:16) + (Arg->TotalInFeatures%(Mode16?8:16)?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Rem_WO	= (Wo_L - Wo_F) % 3;
	int Nb_WO	= (Wo_L - Wo_F) / 3 + (Rem_WO?1:0);
	int Rem_HO	= (Ho_L - Ho_F) % 3;
	int Nb_HO	= (Ho_L - Ho_F) / 3 + (Rem_HO?1:0);

	volatile int job_id;
	NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	if ((int) Arg->Pad) NE16_ComputeBorders(Arg, Wo_F, Wo_L, Wo, Ho_F, Ho_L, Ho);
	/* You can have cases where Ho_L-Ho_F=0, in such cases you have to do nothing */
	if (Nb_WO && Nb_HO)
	for (int subfilter_j=0; subfilter_j<Fy; subfilter_j++) {
		int LastSubfilterH = subfilter_j >= (Fy-1);
		for (int subfilter_i=0; subfilter_i<Fx; subfilter_i++) {
			int LastSubfilterW = subfilter_i >= (Fx-1);
			if (Arg->LastD0 && LastSubfilterW && LastSubfilterH){
				// Do not apply reduction if not last
				if (!Mode16) Gen_Cfg |= SET_QUANTOUT;
				if (ApplyRect) Gen_Cfg &= ~(NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
			} else {
				Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
			}
			// Apply streamin after finished the first subfilter
			if ((subfilter_i > 0) || (subfilter_j > 0)) Gen_Cfg = Gen_Cfg | SET_STREAMIN;

			// acquire second job
			NE16_BARRIER_ACQUIRE(job_id);
			// preload all common configuration for the layer in the first job
			// update input / output pointers
			SetNE16_InPointer     (In + Tile_InFeat*(Wo_F*Sx - Arg->Pad[0] + subfilter_i*Dx + Tile_InW*(Ho_F*Sy - Arg->Pad[2] + subfilter_j*Dy)));
			SetNE16_OutPointer    (Out + OutBytes*Tile_OutFeat*(Wo_F + Tile_OutW*Ho_F));
			SetNE16_WeightsPointer(Filter + Tile_OutFeat*(Mode16?8:16)*Nb_LoadedKI*(subfilter_i + Fx*subfilter_j));
			SetNE16_Reminders     (Rem_WO, Rem_HO, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
			SetNE16_GenConfig     (Gen_Cfg);
			if (SubTileCount < 2) {
				SetNE16_BiasPointer   (Bias);
				SetNE16_ScalePointer  (Scale);
				SetNE16_ScaleNPointer (ScaleN);
				SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Tile_InW*Sy, 0,	    // In_D0, In_D1, In_D2 - unused
						       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*Tile_OutW, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
						       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);			  	    // Weights_D0, Weights_D1, Weights_D2
				SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
				SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
				SetNE16_WOffset       (Arg->W_Offset);
				SubTileCount++;
			}
			// already commit and trigger NE16 computation, while programming the next one
			NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
		}
	}
	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv1x1Stride1_NE16(KerConv_NE16_T *Arg)
{
	//if (Arg->Tile_OutH == 1) {KerConv1x1_SmallHW_Stride1_NE16(Arg); return;}
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned int Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat, Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}

	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Rem_WO	= Tile_OutW % 3;
	int Nb_WO	= Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO	= Tile_OutH % 3;
	int Nb_HO	= Tile_OutH / 3 + (Rem_HO?1:0);
	int Rem_WI	= Tile_InW % 3;
	int Rem_HI	= Tile_InH % 3;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}

 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        // acquire job
        NE16_BARRIER_ACQUIRE(job_id);

	// load configuration for the layer
	SetNE16_InPointer     (In);
	SetNE16_OutPointer    (Out);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				 // In_D0, In_D1, In_D2 - unused
			       Out_Stride0, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW, // Out_D0, Out_D1, Out_D2
                               (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);				 // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_ConfigPad     (Arg->Pad, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_GenConfig     (Gen_Cfg);

	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv1x1StrideS_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
	int Sx = Arg->Sx;
	int Sy = Arg->Sy;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}

	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Rem_WO	= Tile_OutW % 3;
	int Nb_WO	= Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO	= Tile_OutH % 3;
	int Nb_HO	= Tile_OutH / 3 + (Rem_HO?1:0);
        unsigned int Gen_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);


	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}

 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	// acquire job
	NE16_BARRIER_ACQUIRE(job_id);

	// load configuration for the layer
	SetNE16_InPointer     (In);
	SetNE16_OutPointer    (Out);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Tile_InW*Sy, 0,			 // In_D0, In_D1, In_D2 - unused
			       Out_Stride0, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW, // Out_D0, Out_D1, Out_D2
                               (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		                 // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WO, Rem_HO, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_ConfigPad     (Arg->Pad, Arg->Pad_Val);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_GenConfig     (Gen_Cfg);

	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}


void KerConv1x1_SmallHW_Stride1_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In     = (unsigned char *__restrict__) Arg->In;
	unsigned char * __restrict__ Out    = (unsigned char *__restrict__) Arg->Out;
	int 	      * __restrict__ Bias   = (int           *__restrict__) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *__restrict__) Arg->Filter;
	unsigned char * __restrict__ Scale  = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned char * __restrict__ InPointer;
	unsigned char * __restrict__ OutPointer;
	unsigned char * __restrict__ FilterPointer;
	unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat; //Tile_InW  = Arg->Tile_InW  / 3, Tile_InH  = 3;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}

	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;

	int SubtileRem = (Tile_OutW*Tile_OutH) % 9;
	int NSubtiles = (Tile_OutW*Tile_OutH/9) + ((Tile_OutW*Tile_OutH)%9?1:0);
	int LastRemH = SubtileRem / 3 + (SubtileRem%3?1:0);
	int LastRemW = SubtileRem % 3;

	int Rem_WI = 0;
	int Rem_HI = 0;
        int W_Stride1   = 2*Arg->Fx*1*Arg->Qw*Nb_KI;

        unsigned int Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}
	int QuantBitsFlag = (Gen_Cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        int subtile_i = 0;
        int subtile_j = 0;
        int LastSubtile = 0;
        int SubTileCount = 0;
        do {
        	int LastSubtileW = subtile_i == (NSubtiles-1);
        	int LastSubtileH = subtile_j == (LastRemH-1);

		InPointer     = In     + 9*Tile_InFeat*subtile_i + 3*Tile_InFeat*subtile_j;
		OutPointer    = Out    + OutBytes*Tile_OutFeat*9*subtile_i + 3*OutBytes*Tile_OutFeat*subtile_j;
		FilterPointer = Filter;
		if (LastSubtileW) {
			Rem_HI = LastRemH;
			subtile_j++;
			if (LastSubtileH){
				Rem_WI = LastRemW;
				Rem_HI = 1;
				subtile_i = 0;
				subtile_j = 0;
				LastSubtile = 1;
			}
		} else {
			subtile_i++;
		}

		// preload all common configuration for the layer in the first job
		// update input / output pointers
		SetNE16_InPointer     (InPointer);
		SetNE16_OutPointer    (OutPointer);
		SetNE16_WeightsPointer(FilterPointer);
		SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WI, Rem_HI);
		SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
		SetNE16_GenConfig     (Gen_Cfg);
		if (SubTileCount < 2) {
			SetNE16_BiasPointer   (Bias);
			SetNE16_ScalePointer  (Scale);
			SetNE16_ScaleNPointer (ScaleN);
			SetNE16_Strides       (Tile_InFeat, Tile_InFeat*3, 0,			   // In_D0, In_D1, In_D2 - unused
					       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*3, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
					       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
			SetNE16_WOffset       (Arg->W_Offset);
			SubTileCount++;
		}
		// acquire first job
		NE16_BARRIER_ACQUIRE(job_id);
		// already commit and trigger NE16 computation, while programming the next one
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        } while (!LastSubtile);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerConv1x1_SmallHW_StrideS_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In     = (unsigned char *__restrict__) Arg->In;
	unsigned char * __restrict__ Out    = (unsigned char *__restrict__) Arg->Out;
	int 	      * __restrict__ Bias   = (int           *__restrict__) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *__restrict__) Arg->Filter;
	unsigned char * __restrict__ Scale  = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;
        unsigned char * __restrict__ InPointer;
	unsigned char * __restrict__ OutPointer;
	unsigned char * __restrict__ FilterPointer;
	unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;

	int Tile_InFeat  = Arg->Tile_InFeat; //Tile_InW  = Arg->Tile_InW  / 3, Tile_InH  = 3;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Mode16 = 0;
        if ((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
        	Tile_InFeat *= 2;
        	Mode16 = 1;
	}
	int Nb_KI	= Tile_InFeat/16 + (Tile_InFeat%16?1:0);
	int Rem_KI	= Tile_InFeat%16?Tile_InFeat%16:16;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32;
	int Sx = Arg->Sx;
	int Sy = Arg->Sy;

	int SubtileRem = (Tile_OutW*Tile_OutH) % 9;
	int NSubtiles = (Tile_OutW*Tile_OutH/9) + ((Tile_OutW*Tile_OutH)%9?1:0);
	int LastRemH = SubtileRem / 3 + (SubtileRem%3?1:0);
	int LastRemW = SubtileRem % 3;

	int Rem_WI = 0;
	int Rem_HI = 0;
        int W_Stride1   = 2*Arg->Fx*1*Arg->Qw*Nb_KI;

        unsigned int Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}
	int QuantBitsFlag = (Gen_Cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
        int subtile_i = 0;
        int subtile_j = 0;
        int LastSubtile = 0;
        int SubTileCount = 0;
        do {
        	int LastSubtileW = subtile_i == (NSubtiles-1);
        	int LastSubtileH = subtile_j == (LastRemH-1);

		InPointer     = In     + 9*Tile_InFeat*subtile_i*Sx + 3*Tile_InFeat*subtile_j;
		OutPointer    = Out    + OutBytes*Tile_OutFeat*9*subtile_i + 3*OutBytes*Tile_OutFeat*subtile_j;
		FilterPointer = Filter;
		if (LastSubtileW) {
			Rem_HI = LastRemH;
			subtile_j++;
			if (LastSubtileH){
				Rem_WI = LastRemW;
				Rem_HI = 1;
				subtile_i = 0;
				subtile_j = 0;
				LastSubtile = 1;
			}
		} else {
			subtile_i++;
		}

		// preload all common configuration for the layer in the first job
		// update input / output pointers
		SetNE16_InPointer     (InPointer);
		SetNE16_OutPointer    (OutPointer);
		SetNE16_WeightsPointer(FilterPointer);
		SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WI, Rem_HI);
		SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
		SetNE16_GenConfig     (Gen_Cfg);
		if (SubTileCount < 2) {
			SetNE16_BiasPointer   (Bias);
			SetNE16_ScalePointer  (Scale);
			SetNE16_ScaleNPointer (ScaleN);
			SetNE16_Strides       (Tile_InFeat*Sx, Tile_InFeat*Sy*3, 0,			   // In_D0, In_D1, In_D2 - unused
					       Out_Stride0, OutBytes*Tile_OutFeat, OutBytes*Tile_OutFeat*3, // Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
					       (Mode16?1:2)*Arg->Qw, Nb_KI*(Mode16?1:2)*Arg->Qw, 0);		   // Weights_D0, Weights_D1, Weights_D2
			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
			SetNE16_WOffset       (Arg->W_Offset);
			SubTileCount++;
		}
		// acquire first job
		NE16_BARRIER_ACQUIRE(job_id);
		// already commit and trigger NE16 computation, while programming the next one
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

        } while (!LastSubtile);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerLinear_8a_NE16(KerLinear_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	unsigned char * __restrict__ Out = (unsigned char *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat;
	int UsedKI 	 = Tile_InFeat;
	int Tile_OutFeat = Arg->Tile_OutFeat;

	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32; // Check different wrt simulator
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	// int Weights_KI = Tile_InFeat / 16 + (Tile_InFeat%16?1:0);

	if (Tile_InFeat % 16) {
		UsedKI = (Tile_InFeat/16+1)*16;
		for (int i=Tile_InFeat; i<UsedKI; i++) In[i] = 0;
	}
	int Nb_KI  = UsedKI / 256 + (UsedKI % 256 ? 1 : 0);
	int Rem_KI = ((UsedKI % 256) / 16) == 0 ? 16 : (UsedKI % 256) / 16;

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}
 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	// acquire job
	NE16_BARRIER_ACQUIRE(job_id);
	// load configuration for the layer
	SetNE16_OutPointer    (Out);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Reminders     (0, 0, Rem_KI, Rem_KO, 0, 0);
	SetNE16_InPointer     (In);
	SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
			       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
			       UsedKI*2/16, Arg->Qw*UsedKI*2/16, Arg->Qw*UsedKI*2);	// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
	SetNE16_WOffset       (Arg->W_Offset);
	SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
	SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	SetNE16_GenConfig     (Gen_Cfg);

	// commit and trigger NE16 computation
	NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerLinear_16a_NE16(KerLinear_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	unsigned char * __restrict__ Out = (unsigned char *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned char * __restrict__ Filter = (unsigned char *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat;
	int UsedKI 	 = Tile_InFeat;
	int Tile_OutFeat = Arg->Tile_OutFeat;


	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32; // Check different wrt simulator
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	// int Weights_KI = Tile_InFeat / 16 + (Tile_InFeat%16?1:0);

	if (Tile_InFeat % 32) {
		UsedKI = (Tile_InFeat/32+1)*32;
	 	for (int i=Tile_InFeat; i<UsedKI; i++) ((unsigned short int *) In)[i] = 0;
	}
        int Nb_KI  = UsedKI / 256 + (UsedKI % 256 ? 1 : 0);
        int Rem_KI = ((UsedKI % 256) / 16) == 0 ? 16 : (UsedKI % 256) / 16;

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		Gen_Cfg |= SET_STREAMIN;
	}
 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

 //        int Offset = 0;
 //        for (int subtile_ki=0; subtile_ki<Nb_KI; subtile_ki++) {
 //        	int IsLastKI = subtile_ki == (Nb_KI-1);
 //        	int InFeatSubTile = IsLastKI?(UsedKI%256):256;
	//         for (int subtile_ko=0; subtile_ko<Nb_KO; subtile_ko++) {
	// 		// acquire job
	// 		NE16_BARRIER_ACQUIRE(job_id);
	// 		int IsLastKO = subtile_ko == (Nb_KO-1);
	// 		// load configuration for the layer
	// 		SetNE16_OutPointer    (Out+subtile_ko*32*OutBytes);
	// 		SetNE16_WeightsPointer(Filter+subtile_ko*32*UsedKI+subtile_ki*256*Tile_OutFeat);
	// 		SetNE16_BiasPointer   (Bias+subtile_ko*32);
	// 		SetNE16_ScalePointer  (Scale+subtile_ko*32);
	// 		SetNE16_ScaleNPointer (ScaleN+subtile_ko*32);
	// 		SetNE16_Reminders     (0, 0, IsLastKI?Rem_KI:0, IsLastKO?Rem_KO:32, 0, 0);
	// 		if (subtile_ko<2){
	// 			SetNE16_InPointer     (In+256*subtile_ki*2);
	// 			SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
	// 					       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
	// 					       InFeatSubTile*2/16, Arg->Qw*InFeatSubTile*2/16, Arg->Qw*InFeatSubTile*2);	// Weights_D0, Weights_D1, Weights_D2
	// 					       //Tile_InFeat/Arg->Qw, Tile_InFeat, Tile_InFeat);	// Weights_D0, Weights_D1, Weights_D2
	// 			SetNE16_Dim           (1, 1, 1, 1);
	// 			SetNE16_WOffset       (Arg->W_Offset);
	// 			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
	// 			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
	// 			SetNE16_GenConfig     (Gen_Cfg);
	// 		}

	// 		// commit and trigger NE16 computation
	// 		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	// 	}
	// }

        int Offset = 0;
        for (int subtile_ki=0; subtile_ki<Nb_KI; subtile_ki++) {
        	int IsLastKI = subtile_ki == (Nb_KI-1);
        	int InFeatSubTile = IsLastKI?(UsedKI%256):256;
		// acquire job
		NE16_BARRIER_ACQUIRE(job_id);
		// load configuration for the layer
		SetNE16_OutPointer    (Out);
		SetNE16_WeightsPointer(Filter+subtile_ki*256*Tile_OutFeat);
		SetNE16_BiasPointer   (Bias);
		SetNE16_ScalePointer  (Scale);
		SetNE16_ScaleNPointer (ScaleN);
		SetNE16_Reminders     (0, 0, IsLastKI?Rem_KI:0, Rem_KO, 0, 0);
		SetNE16_InPointer     (In+256*subtile_ki*2);
		SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
				       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
				       InFeatSubTile*2/16, Arg->Qw*InFeatSubTile*2/16, Arg->Qw*InFeatSubTile*2);	// Weights_D0, Weights_D1, Weights_D2
				       //Tile_InFeat/Arg->Qw, Tile_InFeat, Tile_InFeat);	// Weights_D0, Weights_D1, Weights_D2
		SetNE16_Dim           (1, Nb_KO, 1, 1);
		SetNE16_WOffset       (Arg->W_Offset);
		SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
		SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
		SetNE16_GenConfig     (Gen_Cfg);

		// commit and trigger NE16 computation
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	}

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerMatMul_8aFast_NE16(KerMatMul_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In1;
	unsigned char * __restrict__ Out = (unsigned char *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned char * __restrict__ In2 = (unsigned char *) Arg->In2;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int W_In1  = Arg->W_In1;
	int W_In2  = Arg->W_In2;

	int Nb_KO	= W_In2/32 + (W_In2%32?1:0);
	int Rem_KO	= W_In2%32?W_In2%32:32; // Check different wrt simulator
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int Qw = ((Default_cfg >> NE16_SHIFT_WBITS_M1) & NE16_MASK_WBITS_M1) + 1;

	int Nb_KI  = W_In1 / 256 + (W_In1 % 256 ? 1 : 0);
	int Rem_KI = ((W_In1 % 256) / 16) == 0 ? 16 : (W_In1 % 256) / 16;

	Gen_Cfg = Default_cfg;
 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        for (int Line=0; Line<Arg->H_In1; Line++) {
		// acquire job
		NE16_BARRIER_ACQUIRE(job_id);
		// load configuration for the layer
		SetNE16_InPointer     (In + W_In1*Line);
		//SetNE16_OutPointer    (Out + W_In2*(i + OutFirstCol));
		SetNE16_OutPointer    (Out + W_In2*Line);
		SetNE16_WeightsPointer(In2);
		SetNE16_BiasPointer   (Bias);
		SetNE16_ScalePointer  (Scale);
		SetNE16_ScaleNPointer (ScaleN);
		SetNE16_Reminders     (0, 0, Rem_KI, Rem_KO, 0, 0);
		SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
				       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
				       W_In1*2/16, Qw*W_In1*2/16, Qw*W_In1*2);	// Weights_D0, Weights_D1, Weights_D2
		SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
		SetNE16_WOffset       (Arg->W_Offset);
		SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
		SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
		SetNE16_GenConfig     (Gen_Cfg);

		// commit and trigger NE16 computation
		NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
	}

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}


void KerMatMul_16aFast_NE16(KerMatMul_NE16_T *Arg)
{
	short int * __restrict__ In = (short int *) Arg->In1;
	int * __restrict__ Out = (int *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned char * __restrict__ In2 = (unsigned char *) Arg->In2;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int W_In1  = Arg->W_In1;
	int W_In2  = Arg->W_In2;

	int Nb_KO	= W_In2/32 + (W_In2%32?1:0);
	int Rem_KO	= W_In2%32?W_In2%32:32; // Check different wrt simulator
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int Qw = ((Default_cfg >> NE16_SHIFT_WBITS_M1) & NE16_MASK_WBITS_M1) + 1;

        int Nb_KI  = W_In1 / 256 + (W_In1 % 256 ? 1 : 0);
        int Rem_KI = ((W_In1 % 256) / 16) == 0 ? 16 : (W_In1 % 256) / 16;

	Gen_Cfg = Default_cfg;
 	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

        for (int Line=0; Line<Arg->H_In1; Line++) {
	        for (int subtile_ki=0; subtile_ki<Nb_KI; subtile_ki++) {

	        	int IsLastKI = subtile_ki == (Nb_KI-1);
	        	int InFeatSubTile = IsLastKI?(W_In1%256):256;
			// acquire job
			NE16_BARRIER_ACQUIRE(job_id);
			// load configuration for the layer
			SetNE16_InPointer     (In + W_In1*Line +256*subtile_ki*2);
			SetNE16_OutPointer    (Out + W_In2*Line);
			SetNE16_WeightsPointer(In2+subtile_ki*256*W_In2);
			SetNE16_BiasPointer   (Bias);
			SetNE16_ScalePointer  (Scale);
			SetNE16_ScaleNPointer (ScaleN);
			SetNE16_Reminders     (0, 0, IsLastKI?Rem_KI:0, Rem_KO, 0, 0);
			SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
					       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
					       InFeatSubTile*2/16, Qw*InFeatSubTile*2/16, Qw*InFeatSubTile*2);	// Weights_D0, Weights_D1, Weights_D2
					       //Tile_InFeat/Qw, Tile_InFeat, Tile_InFeat);	// Weights_D0, Weights_D1, Weights_D2
			SetNE16_Dim           (1, Nb_KO, 1, 1);
			SetNE16_WOffset       (Arg->W_Offset);
			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
			SetNE16_GenConfig     (Gen_Cfg);

			// commit and trigger NE16 computation
			NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);
		}
	}

	// wait for end of computation
	NE16_BARRIER();

	// set priority to core side
	NE16_SETPRIORITY_CORE();
}

void KerMatMul_8a_NE16(KerMatMul_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In1;
	int W_In1  = Arg->W_In1;
	int UsedKI = (W_In1%16)?((W_In1/16+1)*16):W_In1;
	unsigned char * __restrict__ Buff1 = (unsigned char *__restrict__) Arg->Buff;
	unsigned char * __restrict__ Buff2 = Buff1 + UsedKI;
	unsigned char * __restrict__ CurBuff = Buff1;
        unsigned int CoreId = gap_coreid();
        unsigned int *Semaphores = Arg->Semaphores;

	if (CoreId == 0) {
		Semaphores[0] = pi_cl_sem_alloc();
		Semaphores[1] = pi_cl_sem_alloc();
		pi_cl_sem_set(Semaphores[0], 0);
		pi_cl_sem_set(Semaphores[1], 2);
	 	for (int i=W_In1; i<UsedKI; i++) {
	 		((unsigned char *) Buff1)[i] = 0;
	 		((unsigned char *) Buff2)[i] = 0;
	 	}
	}
	gap_waitbarrier_cc();

	for (int Line=0; Line<Arg->H_In1; Line++) {
		if (CoreId != 8) {
			/* Producer */
			if (CoreId == 0)
				pi_cl_sem_dec(Semaphores[1]);
			gap_waitbarrier(0);

			/* --- */
			Copy(CurBuff, In+W_In1*Line, W_In1, CoreId);
			CurBuff = (CurBuff==Buff1)?Buff2:Buff1;
			gap_waitbarrier(0);

			if (CoreId == 0)
				pi_cl_sem_inc(Semaphores[0], 1);
			gap_waitbarrier(0);

		} else {
			/* Consumer */
			unsigned char * __restrict__ Out = (unsigned char *) Arg->Out;
			int * __restrict__ Bias = (int *) Arg->Bias;
			unsigned char * __restrict__ In2 = (unsigned char *) Arg->In2;
			unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
			unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

			int W_In2  = Arg->W_In2;
			int Nb_KO  = W_In2/32 + (W_In2%32?1:0);
			int Rem_KO = W_In2%32?W_In2%32:32;

		        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
		        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
			int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
		        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
			int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
			int Qw = ((Default_cfg >> NE16_SHIFT_WBITS_M1) & NE16_MASK_WBITS_M1) + 1;

			int Nb_KI  = UsedKI / 256 + (UsedKI % 256 ? 1 : 0);
			int Rem_KI = ((UsedKI % 256) / 16) == 0 ? 16 : (UsedKI % 256) / 16;

			Gen_Cfg = Default_cfg;
		 	volatile int job_id;
		        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

			// acquire job
			NE16_BARRIER_ACQUIRE(job_id);

			pi_cl_sem_dec(Semaphores[0]);

			// load configuration for the layer
			SetNE16_InPointer     (CurBuff);
			//SetNE16_OutPointer    (Out + W_In2*(i + OutFirstCol));
			SetNE16_OutPointer    (Out + W_In2*Line);
			SetNE16_WeightsPointer(In2);
			SetNE16_BiasPointer   (Bias);
			SetNE16_ScalePointer  (Scale);
			SetNE16_ScaleNPointer (ScaleN);
			SetNE16_Reminders     (0, 0, Rem_KI, Rem_KO, 0, 0);
			SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
					       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
					       UsedKI*2/16, Qw*UsedKI*2/16, Qw*UsedKI*2);	// Weights_D0, Weights_D1, Weights_D2
			SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
			SetNE16_WOffset       (Arg->W_Offset);
			SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
			SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
			SetNE16_GenConfig     (Gen_Cfg);

			// commit and trigger NE16 computation
			NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

			// wait for end of computation
			NE16_BARRIER();

			// set priority to core side
			NE16_SETPRIORITY_CORE();
			CurBuff = (CurBuff==Buff1)?Buff2:Buff1;

			pi_cl_sem_inc(Semaphores[1], 1);
		}
	}
}

void Ker_MM_Conv2D_NE16(
	Ker_MM_Conv_NE16_T *Arg
	)

{
	unsigned char *__restrict__ In = (unsigned char *__restrict__) Arg->In;
	int W = Arg->Tile_InW, H = Arg->Tile_InH;
	unsigned char *__restrict__ Filter = (unsigned char *__restrict__) Arg->Filter;
	int Fx = Arg->Fx, Sx = Arg->Sx;
	int Fy = Arg->Fy, Sy = Arg->Sy;
	int FS = Fx*Fy;
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2];
	int InFeat = Arg->Tile_InFeat, OutFeat = Arg->Tile_OutFeat;
        int * __restrict__ Bias = Arg->Bias;
        signed char * __restrict__ Out = Arg->Out;
        unsigned char * __restrict__ Scale = Arg->Scale;
        unsigned char * __restrict__ ScaleN = Arg->ScaleN;
        unsigned char * __restrict__ ColBuff1 = Arg->ColBuff;
        unsigned char * __restrict__ ColBuff2 = Arg->ColBuff + InFeat*FS;
	int Wo = Arg->Tile_OutW, Ho = Arg->Tile_OutH;
	unsigned int * Semaphores = Arg->Semaphores;

	/* ColBuff must be large enough to accomodate Align(Fx*InFeat, 8) elements */
	unsigned int W_In1 = InFeat*Fx*Fy;
	unsigned int CoreId = gap_coreid(), C = ChunkSize(InFeat), F = Min(CoreId*C, InFeat), L = Min(InFeat, F+C);
	unsigned int ChunkCell = ChunkSize(OutFeat), First = CoreId*ChunkCell, Last  = Min(OutFeat, First+ChunkCell);

	if (CoreId == 0) {
		Semaphores[0] = pi_cl_sem_alloc();
		Semaphores[1] = pi_cl_sem_alloc();
		pi_cl_sem_set(Semaphores[0], 0);
		pi_cl_sem_set(Semaphores[1], 2);
	}
	gap_waitbarrier_cc();

	int ColBuffSize = ((W_In1+15)/16)*16;
	int Tail = ColBuffSize / 4;
	((int *)ColBuff1)[Tail-1] = 0; ((int *)ColBuff1)[Tail-2] = 0; ((int *)ColBuff1)[Tail-3] = 0; ((int *)ColBuff1)[Tail-4] = 0;
	((int *)ColBuff2)[Tail-1] = 0; ((int *)ColBuff2)[Tail-2] = 0; ((int *)ColBuff2)[Tail-3] = 0; ((int *)ColBuff2)[Tail-4] = 0;
	int PosL = Arg->FirstTile?(-PadT):0;
	int Iter = L-F;
	int Iter1 = Iter*FS;

	unsigned int Gen_Cfg, Default_cfg = Arg->Default_NE16_Job_Cfg;
	int Nb_KI=0, Rem_KI=0, Nb_KO=0, Rem_KO=0, OutBytes=0, Out_Stride0=0;
	if (CoreId == 8) {
		Nb_KO	= OutFeat/32 + (OutFeat%32?1:0);
		Rem_KO	= OutFeat%32?OutFeat%32:32; // Check different wrt simulator
		int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
		OutBytes    = (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
		Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

		if((Default_cfg >> NE16_SHIFT_MODE16) & NE16_MASK_MODE16) {
			Rem_KI = ((ColBuffSize % 512) / 16) == 0 ? 16 : (ColBuffSize % 512) / 16;
			Nb_KI  = ColBuffSize / 512 + (ColBuffSize % 512 ? 1 : 0);
		} else {
			Rem_KI = ((ColBuffSize % 256) / 16) == 0 ? 16 : (ColBuffSize % 256) / 16;
			Nb_KI  = ColBuffSize / 256 + (ColBuffSize % 256 ? 1 : 0);
		}

		Gen_Cfg = Default_cfg;
		if (!Arg->LastD0){
			// Do not apply reduction if not last
			Gen_Cfg = (Gen_Cfg & RESET_QUANTOUT) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
		}
		if (!Arg->FirstD0){
			Gen_Cfg |= SET_STREAMIN;
		}
		NE16_SETPRIORITY_NE16();
	}
	volatile int job_id;

	unsigned char * __restrict__ ColBuff = ColBuff1;
	for (int l=0; l<Ho; l++) {
		int PosC = -PadL;
		int Tb = Max(PosL, 0), Db = Min(PosL+Fy, H);
		int OffL = -Tb - Min(PosL, 0);
		for (int c=0; c<Wo; c++) {
			if (CoreId != 8) {
				/* Producer: Im2Col in 8 cores onto 2 pingpong buffers */

				if (CoreId == 0)
					pi_cl_sem_dec(Semaphores[1]);
				gap_waitbarrier(0);

				/* Im2Col */
				for (int i=0; i<(Iter1/4); i++) ((int *)(ColBuff+F*FS))[i]=0;
				if (Iter1&0x2) ((short int *)(ColBuff+F*FS))[Iter1/2-1]=0;
				if (Iter1&0x1) ((signed char *)(ColBuff+F*FS))[Iter1-1]=0;
				int Lb = Max(PosC, 0), Rb = Min(PosC+Fx, W);
				int OffC = -Lb - Min(PosC, 0);
				int Size = Rb-Lb;
				if (Iter>=4) {
					for (int f=0; f<(Iter/4); f++)
						for (int j=Tb; j<Db; j++)
							for (int i=Lb; i<Rb; i++)
								((int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[f] = ((int *)(In+j*W*InFeat + i*InFeat+F))[f];
					if (Iter&0x2)
						for (int j=Tb; j<Db; j++)
							for (int i=Lb; i<Rb; i++)
								((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1)
						for (int j=Tb; j<Db; j++)
							for (int i=Lb; i<Rb; i++)
								((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
				} else if (Iter>=2) {
					if (Iter&0x2)
						for (int j=Tb; j<Db; j++)
							for (int i=Lb; i<Rb; i++)
								((short int *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter/2-1] = ((short int *)(In+j*W*InFeat + i*InFeat+F))[Iter/2-1];
					if (Iter&0x1)
						for (int j=Tb; j<Db; j++)
							for (int i=Lb; i<Rb; i++)
								((signed char *)(ColBuff+(j+OffL)*InFeat*Fx+(i+OffC)*InFeat+F))[Iter-1] = ((signed char *)(In+j*W*InFeat + i*InFeat+F))[Iter-1];
				} else if (Iter>0)
					for (int j=Tb; j<Db; j++)
						for (int i=Lb; i<Rb; i++)
							ColBuff[(j+OffL)*InFeat*Fx+(i+OffC)*InFeat + F] = In[j*W*InFeat + i*InFeat + F];
				PosC += Sx;

				if (CoreId == 0)
					pi_cl_sem_inc(Semaphores[0], 1);

				ColBuff = (ColBuff==ColBuff1)?ColBuff2:ColBuff1;
				gap_waitbarrier(0);
			} else {
				/* Consumer: Master core set the job and trigger NE16 on the prepared buffers */
				pi_cl_sem_dec(Semaphores[0]);

				// load configuration for the layer
				SetNE16_InPointer     (ColBuff);
				//SetNE16_OutPointer    (Out + W_In2*(i + OutFirstCol));
				SetNE16_OutPointer    (Out + ((l*Wo+c)*OutFeat)*OutBytes);
				SetNE16_WeightsPointer(Filter);
				SetNE16_BiasPointer   (Bias);
				SetNE16_ScalePointer  (Scale);
				SetNE16_ScaleNPointer (ScaleN);
				SetNE16_Reminders     (0, 0, Rem_KI, Rem_KO, 0, 0);
				SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
						       Out_Stride0, 0, 0,				// Out_D0, Out_D1 - unused, Out_D2 - unused
						       FS*InFeat*2/16, Arg->Qw*FS*InFeat*2/16, Arg->Qw*FS*InFeat*4);	// Weights_D0, Weights_D1, Weights_D2
				SetNE16_Dim           (Nb_KI, Nb_KO, 1, 1);
				SetNE16_WOffset       (Arg->W_Offset);
				SetNE16_ConfigPad     ((v4s) {0, 0, 0, 0}, 0);
				SetNE16_ConfigFMask   ((v4s) {0, 0, 0, 0});
				SetNE16_GenConfig     (Gen_Cfg);

				// commit and trigger NE16 computation
				NE16_WRITE_CMD(NE16_COMMIT_AND_TRIGGER, NE16_TRIGGER_CMD);

				NE16_BARRIER();
				ColBuff = (ColBuff==ColBuff1)?ColBuff2:ColBuff1;

				pi_cl_sem_inc(Semaphores[1], 1);
		        }
		}
		PosL += Sy;
	}
	if (CoreId == 8) {
		// set priority to core side
		NE16_SETPRIORITY_CORE();
	}
	gap_waitbarrier_cc();
}

#pragma GCC diagnostic pop