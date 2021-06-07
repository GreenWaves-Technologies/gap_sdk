#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "Gap.h"
#include "CNN_BasicKernels_NE16.h"

unsigned int NE16_GenDefaultConfig(unsigned char Qw,
                        unsigned char Mode16,
                        unsigned char StreamoutMode,
                        unsigned char FilterMode,
                        unsigned char LinearMode,
                        unsigned char StridedMode,
                        unsigned char NormBits,
                        unsigned char Streamin,
                        unsigned char WOffsetCfg,
                        unsigned char QuantRightShift,
                        unsigned char QuantBits,
                        unsigned char QuantNoRect,
                        unsigned char NormShift,
                        unsigned char NormBias
                       )
{
        unsigned int ret = 0;
        ret = ret | ((Qw-1          & NE16_MASK_WBITS_M1)          << NE16_SHIFT_WBITS_M1);            /**< weight bits minus 1. */
        ret = ret | ((Mode16        & NE16_MASK_MODE16)            << NE16_SHIFT_MODE16);              /**< mode16 (0=8-bits mode; 1=16-bits mode) */
        ret = ret | ((StreamoutMode & NE16_MASK_OUTQUANT)          << NE16_SHIFT_OUTQUANT);            /**< quantization rect(0=rectify + consider as unsigned; 1=do not rectify, keep sign) */
        ret = ret | ((FilterMode    & NE16_MASK_FILTER_MODE)       << NE16_SHIFT_FILTER_MODE);         /**< filter mode (11=reserved, 10=1x1, 01=3x3 depthwise, 00=3x3) */
        ret = ret | ((LinearMode    & NE16_MASK_LINEAR_MODE)       << NE16_SHIFT_LINEAR_MODE);         /**< linear mode (0=normal operation, 1=linear mode) */
        ret = ret | ((StridedMode   & NE16_MASK_STRIDED_MODE)      << NE16_SHIFT_STRIDED_MODE);        /**< strided 2x2 mode (0=normal operation, 1=strided mode) */
        ret = ret | ((NormBits      & NE16_MASK_NORM_BITS)         << NE16_SHIFT_NORM_BITS);           /**< normalization bits (00=8-bits, 01=16-bits, 10=32-bits) */
        ret = ret | ((Streamin      & NE16_MASK_STREAMIN)          << NE16_SHIFT_STREAMIN);            /**< streamin mode (0=normal operation, 1=perform streamin) */
        ret = ret | ((WOffsetCfg    & NE16_MASK_WEIGHT_OFFSET_CFG) << NE16_SHIFT_WEIGHT_OFFSET_CFG);   /**< weight offset cfg (0=symmetric weights, 1=use layer-wise weight_offset) */
        ret = ret | ((QuantRightShift & NE16_MASK_QUANT_RIGHT_SHIFT) << NE16_SHIFT_QUANT_RIGHT_SHIFT); /**< quantization right shift */
        ret = ret | ((QuantBits     & NE16_MASK_QUANT_BITS)        << NE16_SHIFT_QUANT_BITS);          /**< quantization bits (00=8-bits, 01=16-bits, 10=32-bits) */
        ret = ret | ((QuantNoRect   & NE16_MASK_QUANT_NORECT)      << NE16_SHIFT_QUANT_NORECT);        /**< streamout / quantization (1=quantization+streamout, 0=streamout only) */
        ret = ret | ((NormShift     & NE16_MASK_NORM_SHIFT)        << NE16_SHIFT_NORM_SHIFT);          /**< norm option shift (0=use quantization right shift; 1=load with norm) */
        ret = ret | ((NormBias      & NE16_MASK_NORM_BIAS)         << NE16_SHIFT_NORM_BIAS);           /**< norm option bias (0=do not load bias; 1=load bias) */
        return ret;
}

void SetNE16_InPointer(void *InPointer){
	NE16_WRITE_REG(NE16_REG_INFEAT_PTR, (int) InPointer);
        #if defined( DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
                printf("InPointer:\t%x\n", InPointer);
        #endif
}
void SetNE16_OutPointer(void *OutPointer){
	NE16_WRITE_REG(NE16_REG_OUTFEAT_PTR, (int) OutPointer);
        #if defined( DEBUG_NE16) || defined(DEBUG_NE16_PNTR)
                printf("OutPointer:\t%x\n", OutPointer);
        #endif
}
void SetNE16_WeightsPointer(void *WeightsPointer){
	NE16_WRITE_REG(NE16_REG_WEIGHTS_PTR, (int) WeightsPointer);
        #ifdef DEBUG_NE16
                printf("WeightsPointer:\t%x\n", WeightsPointer);
        #endif
}
void SetNE16_BiasPointer(void *BiasPointer){
	NE16_WRITE_REG(NE16_REG_SCALE_BIAS_PTR, (int) BiasPointer);
        #ifdef DEBUG_NE16
                printf("BiasPointer:\t%x\n", BiasPointer);
        #endif
}
void SetNE16_ScalePointer(void *ScalePointer){
	NE16_WRITE_REG(NE16_REG_SCALE_PTR, (int) ScalePointer);
        #ifdef DEBUG_NE16
                printf("ScalePointer:\t%x\n", ScalePointer);
        #endif
}
void SetNE16_ScaleNPointer(void *ScaleNPointer){
	NE16_WRITE_REG(NE16_REG_SCALE_SHIFT_PTR, (int) ScaleNPointer);
        #ifdef DEBUG_NE16
                printf("ScaleNPointer:\t%x\n", ScaleNPointer);
        #endif
}

void SetNE16_Strides(unsigned short In_D0,
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

void SetNE16_Dim(unsigned short Nb_KI,
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

void SetNE16_Reminders(unsigned short Rem_WI,
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

void SetNE16_ConfigPad(v4s Pad, short int PadVal)
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

void SetNE16_ConfigFMask(v4s FilterMask)
{
	NE16_WRITE_REG(NE16_REG_FILTER_MASK, ((FilterMask[0] & NE16_MASK_FILTER_MASK_LEFT)   << NE16_SHIFT_FILTER_MASK_LEFT)   | /**< Only if the NE16 is set to 3x3 mode. */
					     ((FilterMask[1] & NE16_MASK_FILTER_MASK_RIGHT)  << NE16_SHIFT_FILTER_MASK_RIGHT)  | /**< Filter masking forces the value of part of the weights in the spatial direction */
					     ((FilterMask[2] & NE16_MASK_FILTER_MASK_TOP)    << NE16_SHIFT_FILTER_MASK_TOP)    | /**< It can be from 0 to 1 in each direction (left/right/top/bottom) */
					     ((FilterMask[3] & NE16_MASK_FILTER_MASK_BOTTOM) << NE16_SHIFT_FILTER_MASK_BOTTOM));
	#ifdef DEBUG_NE16
		printf("FMask: {%d, %d, %d, %d}\n", FilterMask[0], FilterMask[1], FilterMask[2], FilterMask[3]);
	#endif
}

void SetNE16_WOffset(int W_Offset){
	NE16_WRITE_REG(NE16_REG_WEIGHT_OFFSET, W_Offset);
	#ifdef DEBUG_NE16
		printf("W_Offset: %d\n", W_Offset);
	#endif
}

void SetNE16_GenConfig(unsigned int Cfg){
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

void WeightsExpandLastTile(WeightsExpandLastTile_T *Arg){
	if (Arg->IsLastTile){
		unsigned short int *Filter = (unsigned short int *) Arg->Filter;
		unsigned short int Chin = Arg->Chin;
		unsigned int DimNoKi = Arg->OutFeat * Arg->Fcx * Arg->Fcy * Arg->Nbits;
		unsigned int BitCurDim = Chin * Arg->Nbits * Arg->Fcx * Arg->Fcy * Arg->OutFeat;
		for(int i=DimNoKi; i>0; i--){
			BitCurDim -= Chin;
			unsigned int bitindex = BitCurDim & 0xF;
			unsigned BitPos = BitCurDim >> 4; /// 16;
			if (bitindex == 0){
				continue;
			} else if ((bitindex >= Chin) && (bitindex <= (16-Chin))){ // All the current Chin contributions of bit q are in this word
				Filter[i] = Filter[BitPos] >> (bitindex);
			} else if (bitindex < Chin) { // Splitted into two consecutive words
				Filter[i] = (Filter[BitPos] << (Chin - bitindex)) | (Filter[BitPos-1] >> (16 - (Chin-bitindex)));
			} else {
				Filter[i] = (Filter[BitPos] >> (bitindex)) | (Filter[BitPos+1] << (16 - bitindex));
			}
		}
	}
}

void KerConv3x3Stride1_NE16(KerConv_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

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
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2], PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1], PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
	       	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
	}

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
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 5*5*Tile_InFeat,			// In_D0, In_D1, In_D2 - unused
			       32, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2
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

void KerConvDW3x3Stride1_NE16(KerConv_NE16_T *Arg)
{	
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

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
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
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
			       32, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2
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

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

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
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
	       	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
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
							       2*3*3, W_Stride1, 0);
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

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

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
	int PadR = 0, PadB = 0;
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
	       	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
	}

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA

	// define strided conv iteration indeces
	int subtile_i_major=0;
	int subtile_j_major=0;
	IsLastSubtileH = subtile_i_major==(H_subtiles-1);
	IsLastSubtileW = subtile_j_major==(W_subtiles-1);

	// acquire first job
	NE16_BARRIER_ACQUIRE(job_id);

	// preload all common configuration for the layer in the first job
       	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 	        			// In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, W_Stride1, 0);                                				// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
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

	// acquire second job
	NE16_BARRIER_ACQUIRE(job_id);

	// preload all common configuration for the layer in the second job
	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				        // In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, W_Stride1, 0);				                                // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Moving to next spatial subtile means consider less padding (2 because of the stride)
	PadL = Max(0, PadL-2*subtile_j_major);
	PadT = Max(0, PadT-2*subtile_i_major);
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

	// main iteration strided conv iteration loop (does not need ne16_c0_config any more)
	do {
		// acquire job
		NE16_BARRIER_ACQUIRE(job_id);

		// update input / output pointers
		if(subtile_i_major==(H_subtiles-1) && subtile_j_major==(W_subtiles-1)) {
			Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(subtile_i_major==(H_subtiles-1)) {
			Rem_HO = Last_Rem_HO;  Rem_WO = 0;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(subtile_j_major==(W_subtiles-1)) {
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
		PadL = Max(0, PadL-2*subtile_j_major);
		PadT = Max(0, PadT-2*subtile_i_major);
		SetNE16_ConfigPad ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);

		SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
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
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
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

	// preload all common configuration for the layer in the first job
       	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 	        			// In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, W_Stride1, 0);                                				// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
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

	// acquire second job
	NE16_BARRIER_ACQUIRE(job_id);

	// preload all common configuration for the layer in the second job
	// update input / output pointers
	SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major  - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
	SetNE16_OutPointer(Out + OutBytes*2*Tile_OutW * Tile_OutFeat * subtile_i_major   + OutBytes*2*Tile_OutFeat * subtile_j_major);
	SetNE16_WeightsPointer(Filter);
	SetNE16_BiasPointer   (Bias);
	SetNE16_ScalePointer  (Scale);
	SetNE16_ScaleNPointer (ScaleN);
	SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 				        // In_D0, In_D1, In_D2 - unused
		      	       Out_Stride0, OutBytes * Tile_OutFeat / 2, OutBytes * Tile_OutFeat * Tile_OutW / 2,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
		      	       2*3*3, W_Stride1, 0);				                                // Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
	// Moving to next spatial subtile means consider less padding (2 because of the stride)
	PadL = Max(0, PadL-2*subtile_j_major);
	PadT = Max(0, PadT-2*subtile_i_major);
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

	// main iteration strided conv iteration loop (does not need ne16_c0_config any more)
	do {
		// acquire job
		NE16_BARRIER_ACQUIRE(job_id);

		// update input / output pointers
		if(subtile_i_major==(H_subtiles-1) && subtile_j_major==(W_subtiles-1)) {
			Rem_HO = Last_Rem_HO;  Rem_WO = Last_Rem_WO;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = Rem_WO?(Rem_WO+2):0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1];
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(subtile_i_major==(H_subtiles-1)) {
			Rem_HO = Last_Rem_HO;  Rem_WO = 0;
			Rem_HI = Rem_HO?(Rem_HO+2):0; Rem_WI = 0;
			SetNE16_Reminders(Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			PadR = 0;
			PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];
		}
		else if(subtile_j_major==(W_subtiles-1)) {
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
		PadL = Max(0, PadL-2*subtile_j_major);
		PadT = Max(0, PadT-2*subtile_i_major);
		SetNE16_ConfigPad ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);

		SetNE16_InPointer (In  +          4*Tile_InW  * Tile_InFeat  * subtile_i_major   +          4*Tile_InFeat  * subtile_j_major - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
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

void KerConvNxMStride1_NE16(KerConv_NE16_T *Arg)
{
	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;
	int Nb_KI	= Arg->Tile_InFeat/16 + (Arg->Tile_InFeat%16?1:0);
	int Rem_KI	= Arg->Tile_InFeat%16?Arg->Tile_InFeat%16:16;
	int Nb_KO	= Arg->Tile_OutFeat/32 + (Arg->Tile_OutFeat%32?1:0);
	int Rem_KO	= Arg->Tile_OutFeat%32?Arg->Tile_OutFeat%32:32;
	int Rem_WO	= Tile_OutW % 3;
	int Nb_WO	= Tile_OutW / 3 + (Rem_WO?1:0);
	int Rem_HO	= Tile_OutH % 3;
	int Nb_HO	= Tile_OutH / 3 + (Rem_HO?1:0);
	int Rem_WI	= Rem_WO?(Rem_WO+2):0;
	int Rem_HI	= Rem_HO?(Rem_HO+2):0;
        int W_Stride1   = 2*Arg->Fx*Arg->Fy*Arg->Qw*Nb_KI;

        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	int PadL = Arg->Pad[0], PadT = Arg->Pad[2], PadR = (Rem_WI?(5 - Rem_WI):0) + Arg->Pad[1], PadB = (Rem_HI?(5 - Rem_HI):0) + Arg->Pad[3];

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
		res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
		res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
		res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
	}

	volatile int job_id;
        NE16_SETPRIORITY_NE16(); // priority to NE16 w.r.t. cores, DMA
	// Divide the NxM filter into subfilters:
	//   - 3x3 FMask={0,0,0,0}
	//   - 3x3 FMask={0,1,0,0}
	//   - 3x3 FMask={0,0,1,0}
	//   - 3x3 FMask={0,0,1,0}
	int NSubfilters_i = Arg->Fy / 3 + (Arg->Fy%3?1:0);
	int NSubfilters_j = Arg->Fx / 3 + (Arg->Fx%3?1:0);
	int RemSubfilters_i = 3 - (Arg->Fy % 3);
	int RemSubfilters_j = 3 - (Arg->Fx % 3);

	for (int subfilter_i=0; subfilter_i<NSubfilters_i; subfilter_i++){
		int LastSubfilter_i = subfilter_i == (NSubfilters_i-1);
		int DimSubFilter_i = LastSubfilter_i?RemSubfilters_i:3;
		for (int subfilter_j=0; subfilter_j<NSubfilters_j; subfilter_j++){
			// acquire first job
			NE16_BARRIER_ACQUIRE(job_id);

			int LastSubfilter_j = subfilter_j == (NSubfilters_j-1);
			int DimSubFilter_j = LastSubfilter_j?RemSubfilters_j:3;

			// TODO - LOGIC FOR PADDING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			// preload all common configuration for the layer in the first job
			// update input / output pointers
			SetNE16_InPointer (Arg->In  + 3*Tile_InW*Tile_InFeat*subfilter_i + 3*Tile_InFeat*subfilter_j - Tile_InFeat*PadL - Tile_InFeat*Tile_InW*PadT);
			SetNE16_OutPointer(Arg->Out);
			SetNE16_WeightsPointer(Arg->Filter + Tile_OutFeat*DimSubFilter_i*Tile_InFeat*subfilter_i + Tile_OutFeat*DimSubFilter_j*Tile_InFeat*subfilter_j);
			SetNE16_BiasPointer   (Arg->Bias);
			SetNE16_ScalePointer  (Arg->Scale);
			SetNE16_ScaleNPointer (Arg->ScaleN);
			SetNE16_Strides       (Tile_InFeat, Tile_InFeat * Tile_InW, 0, 	        		// In_D0, In_D1, In_D2 - unused
				      	       32, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW,	// Out_D0, Out_D1, Out_D2 div 2 to take into account strideness
				      	       2*DimSubFilter_j*DimSubFilter_i, 2*DimSubFilter_j*DimSubFilter_i*Arg->Qw, 0);	// Weights_D0, Weights_D1, Weights_D2
			SetNE16_Reminders     (Rem_WI, Rem_HI, Rem_KI, Rem_KO, Rem_WO, Rem_HO);
			SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
			SetNE16_ConfigPad     ((v4s) {PadL, PadR, PadT, PadB}, Arg->Pad_Val);
			SetNE16_ConfigFMask   ((v4s) {0, LastSubfilter_j?(3-RemSubfilters_j):0, 0, LastSubfilter_i?(3-RemSubfilters_i):0});
			SetNE16_WOffset       (Arg->W_Offset);
			SetNE16_GenConfig     (Gen_Cfg);

			// Apply streamin after first subfilter anyway
			streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
			Gen_Cfg = Default_cfg | streamin_flag;

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
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat,  Tile_InW  = Arg->Tile_InW,  Tile_InH  = Arg->Tile_InH;
	int Tile_OutFeat = Arg->Tile_OutFeat, Tile_OutW = Arg->Tile_OutW, Tile_OutH = Arg->Tile_OutH;

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
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
	       	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
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
			       32, OutBytes * Tile_OutFeat, OutBytes * Tile_OutFeat * Tile_OutW, // Out_D0, Out_D1, Out_D2
                               2*Arg->Qw, Nb_KI*16*Arg->Qw/8, 0);
			       //2*1*1, 2*1*1*Arg->Qw, 0);			                 // Weights_D0, Weights_D1, Weights_D2
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

void KerLinear_NE16(KerLinear_NE16_T *Arg)
{
	unsigned char * __restrict__ In = (unsigned char *) Arg->In;
	void * __restrict__ Out = (void *) Arg->Out;
	int * __restrict__ Bias = (int *) Arg->Bias;
	unsigned short int * __restrict__ Filter = (unsigned short int *) Arg->Filter;
	unsigned char * __restrict__ Scale = (unsigned char *__restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char *__restrict__) Arg->ScaleN;

	int Tile_InFeat  = Arg->Tile_InFeat;
	int Tile_OutFeat = Arg->Tile_OutFeat;

	int Nb_KI, Rem_KI;
	int Nb_KO	= Tile_OutFeat/32 + (Tile_OutFeat%32?1:0);
	int Rem_KO	= Tile_OutFeat%32?Tile_OutFeat%32:32; // Check different wrt simulator
        int Nb_HO = 1, Nb_WO = 1;
        unsigned int res_quant_out_flag, streamin_flag, Gen_Cfg;
        unsigned int Default_cfg = Arg->Default_NE16_Job_Cfg;
	int QuantBitsFlag = (Default_cfg >> NE16_SHIFT_QUANT_BITS) & NE16_MASK_QUANT_BITS;
        int OutBytes	= (QuantBitsFlag==2)?4: ((QuantBitsFlag==1)?2:1);
	int Out_Stride0 = (QuantBitsFlag==2)?32:((QuantBitsFlag==1)?16:0);
	// int Weights_KI = Tile_InFeat / 16 + (Tile_InFeat%16?1:0);

        if(Arg->Mode16) {
                Rem_KI = ((Tile_InFeat % 512) / 16) == 0 ? 16 : (Tile_InFeat % 512) / 16;
                Nb_KI  = Tile_InFeat / 512 + (Tile_InFeat % 512 ? 1 : 0);
        } else {
                Rem_KI = ((Tile_InFeat % 256) / 16) == 0 ? 16 : (Tile_InFeat % 256) / 16;
                Nb_KI  = Tile_InFeat / 256 + (Tile_InFeat % 256 ? 1 : 0);
        }

	Gen_Cfg = Default_cfg;
	if (!Arg->LastD0){
		// Do not apply reduction if not last
	       	res_quant_out_flag  = ~(NE16_MASK_OUTQUANT << NE16_SHIFT_OUTQUANT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_SHIFT << NE16_SHIFT_NORM_SHIFT);
	       	res_quant_out_flag &= ~(NE16_MASK_NORM_BIAS << NE16_SHIFT_NORM_BIAS);
		Gen_Cfg = (Default_cfg & res_quant_out_flag) | (NE16_MASK_QUANT_NORECT << NE16_SHIFT_QUANT_NORECT);
	}
	if (!Arg->FirstD0){
		// Apply streamin if not first
		streamin_flag = (NE16_MASK_STREAMIN << NE16_SHIFT_STREAMIN);
		Gen_Cfg = Default_cfg | streamin_flag;
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
	SetNE16_Strides       (16, 0, 0, 	                                // In_D0, In_D1 - unused, In_D2 - unused
			       32, 0, 0,		                        // Out_D0, Out_D1 - unused, Out_D2 - unused
			       Tile_InFeat/Arg->Qw, Tile_InFeat, Tile_InFeat);	// Weights_D0, Weights_D1, Weights_D2
	SetNE16_Reminders     (0, 0, Rem_KI, Rem_KO, 0, 0);
	SetNE16_Dim           (Nb_KI, Nb_KO, Nb_WO, Nb_HO);
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

#pragma GCC diagnostic pop