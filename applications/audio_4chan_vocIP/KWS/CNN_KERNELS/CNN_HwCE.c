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
#include "CNN_HwCE.h"

#define HWCE_Y_OUT_BASE_ADDR 	HWCE_Y_OUT_3_BASE_ADDR
#define HWCE_Y_IN_BASE_ADDR 	HWCE_Y_IN_3_BASE_ADDR
#define HWCE_OUT0		3
#define HWCE_OUT1		2
#define HWCE_OUT2		1
#define HWCE_OUT3		0

#define HWCE_LBSIZE		32
#define HWCE_LBSIZE3x3		30

#define PIX_S 			sizeof(short int)


static inline void HwCE_Trigger_Job()

{
	__builtin_pulp_OffsetedWrite(1, (int *) HWCE_ADDR_BASE, HWCE_TRIGGER);
}


static inline void HwCE_SetOutTransfer_Size(unsigned int ColHeight, unsigned int LineProduced)

{
	hwce_y_trans_size_set((ColHeight*LineProduced)/PIX_S);	/* Writes ColHeight * LineProduced items, in words */
}

static inline void HwCE_SetInTransfer_Size(unsigned int ColHeight, unsigned int LineConsummed)

{
	hwce_x_trans_size_set((ColHeight*LineConsummed)/PIX_S);	/* Writes ColHeight * LineConsummed items, in words */
}

static inline void HwCE_SetOutLine_StrideAndLength(unsigned int LineSize, unsigned int LineProduced)

{
	hwce_y_line_stride_length_set(
			((LineSize*PIX_S)<<16) |	/* Output line stride in bytes */
			(LineProduced/PIX_S)		/* Processed output line length in words */
	);
}

static inline void HwCE_SetOutLine_Length(unsigned int LineProduced)

{
#ifdef NOBTIN
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_Y_LINE_STRIDE_LENGTH)) = LineProduced/PIX_S;
#else
	__builtin_pulp_OffsetedWriteHalf(LineProduced/PIX_S, (int *)HWCE_ADDR_BASE, HWCE_Y_LINE_STRIDE_LENGTH);
#endif
}

static inline void HwCE_SetInLine_StrideAndLength(unsigned int LineSize, unsigned int LineConsummed)

{
	hwce_x_line_stride_length_set(
			((LineSize*PIX_S)<<16) |	/* Input line stride in bytes */
			(LineConsummed/PIX_S)		/* Consummed input line length in words */
	);
}

static inline void HwCE_SetInLine_Length(unsigned int LineConsummed)

{
#ifdef NOBTIN
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_X_LINE_STRIDE_LENGTH)) = LineConsummed/PIX_S;
#else
	__builtin_pulp_OffsetedWriteHalf(LineConsummed/PIX_S, (int *)HWCE_ADDR_BASE, HWCE_X_LINE_STRIDE_LENGTH);
#endif
}


static inline void HwCE_SetOutFeature_StrideAndLength(unsigned int FeatureWidth, unsigned int FeatureHeight)

{
	hwce_y_feat_stride_length_set(
			(((FeatureWidth*FeatureHeight)*PIX_S)<<16) |	/* Next ouput plane stride, in bytes */
                        FeatureHeight);					/* Number of rows for an output plane */
}

static inline void HwCE_SetInFeature_StrideAndLength(unsigned int FeatureWidth, unsigned int FeatureHeight)

{
	hwce_x_feat_stride_length_set(
			(((FeatureWidth*FeatureHeight)*PIX_S)<<16) |	/* Next input plane stride, in bytes */
                        FeatureHeight);					/* Number of rows for an input plane */
}

static inline void HwCE_SetOut_Address(unsigned int OutIndex, unsigned int Out)

{
	__builtin_pulp_OffsetedWrite(Out, (int *)HWCE_ADDR_BASE, HWCE_Y_OUT_BASE_ADDR + 4*OutIndex);
	__builtin_pulp_OffsetedWrite(Out, (int *)HWCE_ADDR_BASE, HWCE_Y_IN_BASE_ADDR + 4*OutIndex);
}

static inline void HwCE_SetIn_Address(unsigned int In)

{
	__builtin_pulp_OffsetedWrite(In, (int *)HWCE_ADDR_BASE, HWCE_X_IN_BASE_ADDR);
}

static inline void HwCE_SetCoeff_Address(unsigned int Coeff)

{
	__builtin_pulp_OffsetedWrite(Coeff, (int *)HWCE_ADDR_BASE, HWCE_W_BASE_ADDR);
}


static void HwCE_Set_JobConfig0(unsigned int ProcessedInputWidth, int Bias)

{
#ifdef NOBTIN
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_JOB_CONFIG0)) = ProcessedInputWidth/PIX_S;
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_JOB_CONFIG0 + 2)) = Bias;
#else
	__builtin_pulp_OffsetedWriteHalf(ProcessedInputWidth/PIX_S, (int *)HWCE_ADDR_BASE, HWCE_JOB_CONFIG0);
	__builtin_pulp_OffsetedWriteHalf(Bias, (int *)HWCE_ADDR_BASE, HWCE_JOB_CONFIG0 + 2);
#endif
}

static void HwCE_Set_JobConfig0_BufLen(unsigned int ProcessedInputWidth)

{
	/* We don't use bias automatic setting, NOYCONST is set to 0 */
	/* Set BufLen only, NOYCONST (bias value) is set independantly in case we want to use it and is activated by the NY bit of GEN_CONFIG0 */
	// __builtin_pulp_OffsetedWrite(ProcessedInputWidth/PIX_S, (int *) HWCE_ADDR_BASE, HWCE_JOB_CONFIG0);
#ifdef NOBTIN
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_JOB_CONFIG0)) = ProcessedInputWidth/PIX_S;
#else
	__builtin_pulp_OffsetedWriteHalf(ProcessedInputWidth/PIX_S, (int *)HWCE_ADDR_BASE, HWCE_JOB_CONFIG0);
#endif
}



static void HwCE_Set_JobConfig1(unsigned int OutputEnableMask)

{
	/* We don't use 3D or 4D feature, just output enable mask */
	__builtin_pulp_OffsetedWrite(OutputEnableMask, (int *) HWCE_ADDR_BASE, HWCE_JOB_CONFIG1);
}

static inline void HwCE_Set_GenericConfig1()

{
	/* Pixel: 16 bits, no Output right shift, no Input left shift */
	__builtin_pulp_OffsetedWrite(0, (int *) HWCE_ADDR_BASE, HWCE_GEN_CONFIG1);

}

static inline int HwCE_Acquire_Job()

{
	int JobId;

	while ((JobId = hwce_acquire_job()) < 0);
	while (hwce_acquire_job() == HWCE_ACQUIRE_CONTEXT_COPY);
	return JobId;
}

static inline void HwCE_Wait_JobDone()

{
	hwce_wait_event();
	// eu_evt_maskWaitAndClr(1<<PLP_EVT_ACC0);
}

void HwCE_SetYinMode(unsigned int Disable)

{
	unsigned int Value = HWCE_READ(HWCE_GEN_CONFIG0);
	Value = gap8_bitinsert(Value, Disable, 1, 7);
	HWCE_WRITE(Value, HWCE_GEN_CONFIG0);
}

void HwCE_SetInputBias(int Bias)

{
#ifdef NOBTIN
	*((volatile short int *)(HWCE_ADDR_BASE + HWCE_JOB_CONFIG0 + 2)) = Bias;
#else
	__builtin_pulp_OffsetedWriteHalf(Bias, (int *)HWCE_ADDR_BASE, HWCE_JOB_CONFIG0 + 2);
#endif
}

void HWCE_GenericInit(unsigned int ConvType, unsigned int WStride, unsigned int Norm)

{
	/* Bit 14 controls the rounding mode */
	hwce_gen_config0_set(
		hwce_gen_config0_value(
			WStride,			/* WSTRIDE, Bytes but 4 byte aligned */
			0,				/* Performs job copy */
			ConvType,			/* Convolution type: 3x3, 5x5, 4x7 */
			0,				/* Vect mode = 0 => Scalar */
			0,				/* Signed arithmetic */
			1,				/* Uses NOYCONST from job reg 2 instead of loading y_in */
			1,				/* Linear mode */
			Norm,				/* Fixed point precision */
			1				/* Rounding enabled */
		)
	);
	HwCE_Set_GenericConfig1();
}

/*
	In:	Vertical stripe [StripeW x H] in input data plane [W x H]
	Out0:	Output data plane [(StripeW-FDimM1) x (H-FDimM1)] in output data plane [(W-FDimM1) x (H-FDimM1)]
	Out1:	Output data plane [(StripeW-FDimM1) x (H-FDimM1)] in output data plane [(W-FDimM1) x (H-FDimM1)]
	Out2:	Output data plane [(StripeW-FDimM1) x (H-FDimM1)] in output data plane [(W-FDimM1) x (H-FDimM1)]
	Filter:	Convolution coefficients [FDim x FDim]
	FDimM1:	Filter dimension - 1
	Fix3x3:	Hack for the HWCE in 3x3 mode since internally line buffer is managed as if conv=5x5 so need to add a fixup
*/
static void HWCE_TileInit_FirstStripe_MultiOut(short int *In, short int *Out0, short int *Out1, short int *Out2,
					       unsigned int W, unsigned int H, short int *Filter, short int Bias,
			                       unsigned int StripeW, unsigned int FDimM1, unsigned int Fix3x3,
					       unsigned int OutMask)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input, One vertical stripe, width=HwCE_W, Height=H */
	HwCE_SetInTransfer_Size((H), (StripeW+Fix3x3));		/* Fix3x3 adds 2 to W since LB manages only 5x5 convol, +2 colums are needed, not true for H */
	HwCE_SetInLine_StrideAndLength(W, StripeW+Fix3x3);
	HwCE_SetInFeature_StrideAndLength(W, H);
	HwCE_SetIn_Address((unsigned int) In);

	/* Filter coefficients */
	HwCE_SetCoeff_Address((unsigned int) Filter);

	/* Configure Output, One vertical stripe, width=StripeW-2*K, Height=H-2*K */
	HwCE_SetOutTransfer_Size((H-FDimM1), (StripeW-FDimM1));
	HwCE_SetOutLine_StrideAndLength(W-FDimM1, (StripeW-FDimM1));
	HwCE_SetOutFeature_StrideAndLength((W-FDimM1), (H-FDimM1));
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out0);
	HwCE_SetOut_Address(HWCE_OUT1, (unsigned int) Out1);
	HwCE_SetOut_Address(HWCE_OUT2, (unsigned int) Out2);

	HwCE_Set_JobConfig0(StripeW+Fix3x3, Bias);		/* Do we need a fixup here in 3x3 ? */
	HwCE_Set_JobConfig1(OutMask);
}

/*
	In:	Vertical stripe [StripeW x H] in input data plane [W x H]
	Out:	Output data plane [(StripeW-FDimHM1) x (H-FDimVM1)] in output data plane [(W-FDimM1) x (H - FDimM1)]
	Filter:	Convolution coefficients [FHDim x FVDim]
	StripeW:Width of the vertical stripe
	FDimM1: Filter horizontal/vertical dimension - 1
*/

static void HWCE_TileInit_FirstStripe_SingleOut(short int *In, short int *Out,
					       unsigned int W, unsigned int H, short int *Filter, short int Bias,
			                       unsigned int StripeW, unsigned int FDimM1)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input, One vertical stripe, width=HwCE_W, Height=H */
	HwCE_SetInTransfer_Size((H), (StripeW));
	HwCE_SetInLine_StrideAndLength(W, StripeW);
	HwCE_SetInFeature_StrideAndLength(W, H);
	HwCE_SetIn_Address((unsigned int) In);

	/* Filter coefficients */
	HwCE_SetCoeff_Address((unsigned int) Filter);

	/* Configure Output, One vertical stripe, width=StripeW-2*K, Height=H-2*K */
	HwCE_SetOutTransfer_Size((H-FDimM1), (StripeW-FDimM1));
	HwCE_SetOutLine_StrideAndLength(W-FDimM1, (StripeW-FDimM1));
	HwCE_SetOutFeature_StrideAndLength((W-FDimM1), (H-FDimM1));
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	HwCE_Set_JobConfig0(StripeW, Bias);
	HwCE_Set_JobConfig1(HWCE_ONE_OUT);
}

static void HWCE_TileInit_FirstStripe_8x20(short int *In, short int *Out,
					       unsigned int W, unsigned int H, short int *Filter, short int Bias,
			                       unsigned int StripeW, unsigned int FDimM1)

{
	unsigned int JobId = HwCE_Acquire_Job();
	/* Configure Input, One vertical stripe, width=HwCE_W, Height=H */
	HwCE_SetInTransfer_Size((98-15), 32);
	HwCE_SetInLine_StrideAndLength(40, 32);
	HwCE_SetInFeature_StrideAndLength(40, 98-15);
	HwCE_SetIn_Address((unsigned int) In);

	/* Filter coefficients */
	HwCE_SetCoeff_Address((unsigned int) Filter);

	/* Configure Output, One vertical stripe, width=StripeW-2*K, Height=H-2*K */
	HwCE_SetOutTransfer_Size((98-20+1), 32-4);//<=========
	HwCE_SetOutLine_StrideAndLength(32, 32-4);//<==========
	HwCE_SetOutFeature_StrideAndLength(32, 98-20+1);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	HwCE_Set_JobConfig0(StripeW, Bias);
	HwCE_Set_JobConfig1(HWCE_ONE_OUT);
}

static void HWCE_TileInit_FirstStripe_4x10(short int *In, short int *Out,
					       unsigned int W, unsigned int H, short int *Filter, short int Bias,
			                       unsigned int StripeW, unsigned int FDimM1)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input, One vertical stripe, width=HwCE_W, Height=H */
	HwCE_SetInTransfer_Size((39-5), (StripeW));
	HwCE_SetInLine_StrideAndLength(16, StripeW);
	HwCE_SetInFeature_StrideAndLength(16, 39-5);
	HwCE_SetIn_Address((unsigned int) In);

	/* Filter coefficients */
	HwCE_SetCoeff_Address((unsigned int) Filter);

	/* Configure Output, One vertical stripe, width=StripeW-2*K, Height=H-2*K */
	HwCE_SetOutTransfer_Size((39-10+1), (StripeW-FDimM1));//<=========
	HwCE_SetOutLine_StrideAndLength(14, (StripeW-FDimM1));//<==========
	HwCE_SetOutFeature_StrideAndLength(14, 39-10+1);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	HwCE_Set_JobConfig0(StripeW, Bias);
	HwCE_Set_JobConfig1(HWCE_ONE_OUT);
}

static void HWCE_TileInit_MultiOut(short int *In, short int *Out0, short int *Out1, short int *Out2, unsigned int W, unsigned int H,
			           unsigned int StripeW, unsigned int FDimM1, unsigned int Fix3x3, unsigned ReconfigureStripe)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input and Output buffers */
	HwCE_SetIn_Address((unsigned int) In);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out0);
	HwCE_SetOut_Address(HWCE_OUT1, (unsigned int) Out1);
	HwCE_SetOut_Address(HWCE_OUT2, (unsigned int) Out2);

	if (ReconfigureStripe) {
		/* Reconfigure Tile related parameters */
		HwCE_SetInTransfer_Size((H), (StripeW+Fix3x3));
		HwCE_SetInLine_Length(StripeW+Fix3x3);

		HwCE_SetOutTransfer_Size((H-FDimM1), (StripeW-FDimM1));
		HwCE_SetOutLine_Length((StripeW-FDimM1));
		HwCE_Set_JobConfig0_BufLen(StripeW+Fix3x3);
	}
}

static void HWCE_TileInit_8x20(short int *In, short int *Out, unsigned int W, unsigned int H,
			            unsigned int StripeW, unsigned int FDimM1,
				    unsigned ReconfigureStripe)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input and Output buffers */
	HwCE_SetIn_Address((unsigned int) In);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	if (ReconfigureStripe) {
	  /* Reconfigure Tile related parameters logh*/
		HwCE_SetInTransfer_Size((H), (StripeW));
		HwCE_SetInLine_Length(StripeW);

		HwCE_SetOutTransfer_Size((98-20+1), (StripeW-5+1));
		HwCE_SetOutLine_Length((StripeW-5+1));
		HwCE_Set_JobConfig0_BufLen(StripeW);
	}
}
static void HWCE_TileInit_4x10(short int *In, short int *Out, unsigned int W, unsigned int H,
			            unsigned int StripeW, unsigned int FDimM1,
				    unsigned ReconfigureStripe)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input and Output buffers */
	HwCE_SetIn_Address((unsigned int) In);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	if (ReconfigureStripe) {
		/* Reconfigure Tile related parameters */
		HwCE_SetInTransfer_Size((H), (StripeW));
		HwCE_SetInLine_Length(StripeW);

		HwCE_SetOutTransfer_Size((39-10+1), (StripeW-4));
		HwCE_SetOutLine_Length((StripeW-4));
		HwCE_Set_JobConfig0_BufLen(StripeW);
	}
}

static void HWCE_TileInit_SingleOut(short int *In, short int *Out, unsigned int W, unsigned int H,
			            unsigned int StripeW, unsigned int FDimM1,
				    unsigned ReconfigureStripe)

{
	unsigned int JobId = HwCE_Acquire_Job();

	/* Configure Input and Output buffers */
	HwCE_SetIn_Address((unsigned int) In);
	HwCE_SetOut_Address(HWCE_OUT0, (unsigned int) Out);

	if (ReconfigureStripe) {
		/* Reconfigure Tile related parameters */
		HwCE_SetInTransfer_Size((H), (StripeW));
		HwCE_SetInLine_Length(StripeW);

		HwCE_SetOutTransfer_Size((H-FDimM1), (StripeW-FDimM1));
		HwCE_SetOutLine_Length((StripeW-FDimM1));
		HwCE_Set_JobConfig0_BufLen(StripeW);
	}
}

#ifdef DEBUG
static void DumpStrip(char *Name, short int *Data, unsigned int W, unsigned int H)

{
        int i, j;
        int LineSize = 17;
        printf("%s\n", Name);

        for (i=0; i<H; i++) {
                printf("Line: %3d ->\n", i);
                for (j=0; j<W; j++) {
                        if (((j+1)%LineSize)==0) printf("\n");
                        printf("%6d ", Data[W*i+j]);
                }
                printf("\n");
        }
}

void CheckIf0(short int *X, unsigned int Size)

{
	int i;
	for (i=0; i<Size; i++) {
		if (X[i] != 0) printf("At %d found %d\n", i, X[i]);
	}
}
#endif

void HWCE_ProcessOneTile3x3_MultiOut(short int *In, short int *Out0, short int *Out1, short int *Out2, short int *Filter, short int Bias,
				     unsigned int W, unsigned int H, unsigned int OutMask)

{
#define Min(x, y) (((x)<(y))?(x):(y))
	int Fix3x3 = 2;
	int FDimM1 = 2;
	int StripeW, RemW = W;;

	StripeW = Min(W, HWCE_LBSIZE3x3);
	HWCE_TileInit_FirstStripe_MultiOut(In, Out0, Out1, Out2, W, H, Filter, Bias, StripeW, FDimM1, Fix3x3, OutMask);
	while (StripeW) {
		int Offset;
		/* Process current stripe */
		HwCE_Trigger_Job();
		/* Prepare next stripe if needed */
		Offset = StripeW - FDimM1;
		RemW -= Offset;
		if (RemW > FDimM1) {
			StripeW = Min(RemW, HWCE_LBSIZE3x3);
			In += Offset; Out0 += Offset;
			if (Out1) Out1 += Offset;
			if (Out2) Out2 += Offset;
			HWCE_TileInit_MultiOut(In, Out0, Out1, Out2, W, H, StripeW, FDimM1, Fix3x3, (StripeW != HWCE_LBSIZE3x3));
		} else StripeW = 0;
		/* Wait for termination of current stripe */
		HwCE_Wait_JobDone();
	}
#undef Min
}

void HWCE_ProcessOneTile5x5(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H)

{
#define Min(x, y) (((x)<(y))?(x):(y))
	int FDimM1 = 4;
	int StripeW, RemW = W;

	StripeW = Min(W, HWCE_LBSIZE);
	HWCE_TileInit_FirstStripe_SingleOut(In, Out, W, H, Filter, Bias, StripeW, FDimM1);
	while (StripeW) {
		int Offset;
		/* Process current stripe */
		HwCE_Trigger_Job();
		/* Prepare next stripe if needed */
		Offset = StripeW - FDimM1;
		RemW -= Offset;
		if (RemW > FDimM1) {
			StripeW = Min(RemW, HWCE_LBSIZE);
			In += Offset; Out += Offset;
			HWCE_TileInit_SingleOut(In, Out, W, H, StripeW, FDimM1, (StripeW != HWCE_LBSIZE));
		} else StripeW = 0;
		/* Wait for termination of current stripe */
		HwCE_Wait_JobDone();
	}
#undef Min
}

// 8x20 filter ==> 2 stripes
void HWCE_2stripes_5x5(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H, unsigned char pass_nb)

{
#define Min(x, y) (((x)<(y))?(x):(y))
	int FDimM1 = 4;
	int StripeW, RemW = W;
	int i,j;
	short int *outb;

	
	// first stripe
	StripeW = HWCE_LBSIZE;
	HWCE_TileInit_FirstStripe_8x20(In, Out, W, H-15, Filter, Bias, StripeW, FDimM1);

	/* Process current stripe */
	HwCE_Trigger_Job();	
	
	/* Wait for termination of current stripe */
	HwCE_Wait_JobDone();

#ifdef PRINTDEB_TILE0
	if (pass_nb==2) {
	  printf("8x20: first stripe, pass %d (stripeW %d bias %d %x)\n",pass_nb,StripeW,Bias,Bias);
	  //for(i=0;i<H-15;i++)
	  //  for (j=0;j<28;j++) printf("hwce1 ==> %d %d out %x\n",i,j,Out[j+i*33]);
	  for(i=0;i<26;i++) printf("hwce ==> %d coef %x\n",i,Filter[i]);
	  //for(i=0;i<(H-15)*(40);i++) printf("hwce ==> %d %d in %x\n",i/40,i%40,In[i]);
	  for(i=0;i<H-19;i++)
	    for(j=0;j<32;j++) printf("hwce1 ==> %d %d o %x\n",i,j,Out[i*32+j]-Bias);
	}
#endif
	
	/* Prepare next stripe if needed */
	outb=Out;
	In += StripeW - FDimM1; Out += StripeW - FDimM1;
	// !! stripew must be even
	if (pass_nb==1) 
	  StripeW= 7+1 ;
	else
	  StripeW= 7+1 ;
	  
	HWCE_TileInit_8x20(In, Out, W, H-15, StripeW, FDimM1, (StripeW != HWCE_LBSIZE));
	// second stripe
	HwCE_Trigger_Job();
	HwCE_Wait_JobDone();

#ifdef PRINTDEB_TILE0	
	printf("8x20: second stripe, pass %d (stripeW %d)\n",pass_nb,StripeW);
	for(i=0;i<H-19;i++)
	  for(j=0;j<32;j++) printf("hwce2 ==> %d %d o %x\n",i,j,outb[j+i*32]);
#endif
	
#undef Mins
}
// 6x10 filter => 1 stripe

void HWCE_2stripes_5x5_4x10(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H)
{
#define Min(x, y) (((x)<(y))?(x):(y))
	int FDimM1 = 4;
	int StripeW;
	int i,j;
	short int *Outb;
	// first stripe
	StripeW = 10 ;

	//printf("W %d H %d StripeW %d \n",W,H,StripeW);

	if (0) {
	  printf("input 4x10\n");
	  for (i=0;(unsigned int)i<H;i++)
	    for (j=0;j<16;j++) printf("hwce in %d %d %x\n",j,i,In[j+16*i]);
	  for (i=0;i<2;i++)
	    for (j=0;j<26;j++) printf("hwce coef %d %d %x\n",i,j,Filter[j+26*i]); 
	}
	
	HWCE_TileInit_FirstStripe_4x10(In, Out, W, H-5, Filter, Bias, StripeW, FDimM1 );
	//printf("conf done\n");

	/* Process current stripe */
	HwCE_Trigger_Job();
	HwCE_Wait_JobDone();

#ifdef PRINTDEB_TILE2	
	printf("4x10: first stripe (stripeW %d)\n",StripeW);
	for(i=0;i<H-9;i++)
	  for(j=0;j<13;j++) printf("hwce2 ==> %d %d o %x\n",i,j,Out[j+i*13]);
#endif

	Outb=Out;
	In += StripeW - FDimM1; Out +=  StripeW - FDimM1;
	StripeW = (17-10+4+1);
	
	HWCE_TileInit_4x10(In, Out, W, H-5, StripeW, FDimM1, (StripeW != HWCE_LBSIZE));

	/* process 2nd stripe */
	HwCE_Trigger_Job();
	HwCE_Wait_JobDone();

#ifdef PRINTDEB_TILE2	
	printf("4x10: second stripe (stripeW %d)\n",StripeW);
	for(i=0;i<H-9;i++)
	  for(j=0;j<14;j++) printf("hwce2 ==> %d %d o %x\n",i,j,Outb[j+i*14]);
#endif

	

#undef Min
}


void __attribute__ ((noinline)) HWCE_ProcessOneTile8x20(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H)

{
  /*
	Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	Word16 *Filter = Arg->Filter;
	unsigned int Norm = Arg->Norm;
	int * __restrict__ Out = (int*) Arg->Out;
  */
  
  int i;


	unsigned int j;
	//if (CoreId==0) printf("W=%d H=%d N=%d M=%d\n",W,H,N,M);

	// first pass of 4 5x5 kernels
	int tile,tile5;
	for (tile=0;tile<4;tile++){

	  // compute partial pixel with filter number "tile" in pass 1 
#ifdef PRINTDEB_TILE
	  printf("1st pass tile %d\n",tile);
#endif
	  HWCE_2stripes_5x5(In + 5*tile*W , Out , Filter + tile*26 , Bias, W, H , 1);
	  if (tile==0) HwCE_SetYinMode(
			  (unsigned int) (0)
			  );
	}
	
	// second pass of 4 x (3+2) x5 kernels
	for (tile=0;tile<4;tile++){
#ifdef PRINTDEB_TILE
	  printf("2nd pass tile %d\n",tile);
#endif
	  tile5 = 5 + 40*5*tile;

	  // stripe loop (Bias ?)
	  HWCE_2stripes_5x5(In + tile5 , Out , Filter + 4*26 + tile*26 , Bias, W, H , 2);

	      
	}

#ifdef PRINTDEB_TILE	
	printf("8x20: final\n");
	for(i=0;i<H-19;i++)
	  for(j=0;j<32;j++) printf("hwce2 ==> %d %d o %x\n",i,j,Out[j+i*32]);
#endif
	
}

void __attribute__ ((noinline)) HWCE_ProcessOneTile4x10(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H )
{

  /*
  Word16 * __restrict__ In = Arg->In;
	int W = Arg->W;
	int H = Arg->H;
	int N = Arg->N;
	int M = Arg->M;
	char *Filter = (char *) Arg->Filter;
	unsigned int Norm = Arg->Norm;
	int * __restrict__ Out = (int *) Arg->Out;
	v2s *Line;
  */
 
       
	//  pass of 2 x (5x(4+1)) kernels
	int tile;
	int i,j;
	for (tile=0;tile<2;tile++){
	  // compute partial pixel with filter number "tile" in pass 1 
	  HWCE_2stripes_5x5_4x10(In + 5*tile*W , Out , Filter + tile*26 , Bias, W, H);
	  // set HWCE in accumulation mode after first tile
	  if (tile==0) HwCE_SetYinMode(	(unsigned int) (0));
	}

}

void HWCE_ProcessOneTile7x7(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H)

{
#define Min(x, y) (((x)<(y))?(x):(y))
	int i;
	int FDimM1 = 6;

	for (i=0; i<2; i++) {
		short int *In0 = In + i*4*W;
		short int *Out0 = Out;
		int StripeW = Min(W, HWCE_LBSIZE);
		int RemW = W;
		/* First iteration copy in constant is disabled, second we copy in Out */
		HwCE_SetYinMode((i==0));
		/* First iter: 7 x 4 convolutions */
		/* Second group: 7 x 4 convolutions, last line of coeff is all 0 */
		HWCE_TileInit_FirstStripe_SingleOut(In0, Out0, W, H, Filter+i*7*4, Bias, StripeW, FDimM1);
		while (StripeW) {
			int Offset;
			/* Process current stripe */
			HwCE_Trigger_Job();
			/* Prepare next stripe if needed */
			Offset = StripeW - FDimM1;
			RemW -= Offset;
			if (RemW > FDimM1) {
				StripeW = Min(RemW, HWCE_LBSIZE);
				In0 += Offset; Out0 += Offset;
				HWCE_TileInit_SingleOut(In0, Out0, W, H, StripeW, FDimM1, (StripeW != HWCE_LBSIZE));
			} else StripeW = 0;
			/* Wait for termination of current stripe */
			HwCE_Wait_JobDone();
		}
	}
#undef Min
}

void HWCE_ProcessOneTile7x4(short int *In, short int *Out, short int *Filter, short int Bias, unsigned int W, unsigned int H)

{
#define Min(x, y) (((x)<(y))?(x):(y))
	int FDimM1 = 6;
	int StripeW, RemW = W;

	StripeW = Min(W, HWCE_LBSIZE);
	HWCE_TileInit_FirstStripe_SingleOut(In, Out, W, H, Filter, Bias, StripeW, FDimM1);
	while (StripeW) {
		int Offset;
		/* Process current stripe */
		HwCE_Trigger_Job();
		/* Prepare next stripe if needed */
		Offset = StripeW - FDimM1;
		RemW -= Offset;
		if (RemW > FDimM1) {
			StripeW = Min(RemW, HWCE_LBSIZE);
			In += Offset; Out += Offset;
			HWCE_TileInit_SingleOut(In, Out, W, H, StripeW, FDimM1, (StripeW != HWCE_LBSIZE));
		} else StripeW = 0;
		/* Wait for termination of current stripe */
		HwCE_Wait_JobDone();
	}
#undef Min
}
