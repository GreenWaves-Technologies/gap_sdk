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

#include "Gap.h"
#include "CNN_BasicKernels_SQ8.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"


static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)

{
	unsigned int NCore;
	unsigned int Log2Core;
	unsigned int Chunk;

	if (CoreCountDynamic) NCore = ActiveCore; else NCore = gap_ncore();
	Log2Core = gap_fl1(NCore);
	Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
	return Chunk;
}

static int FirstDefinedOutput(int F, int Pad, int Stride)

{
	// k*S - (F-1)/2 >=0 => k >= (((F-1)/2) + S-1)/S

	return ((Pad+Stride-1)/Stride);
}

static int LastDefinedOutput(int DimIn, int F, int PadL, int Stride)

{
	// k*S + ((F-1)/2 - PadL + F/2) < Dim  => k < (Dim-((F-1)/2 - PadL + (F/2)) + S-1)/S

	return ((DimIn - ((F-1)/2 - PadL + (F/2)) + Stride-1)/Stride);
}

static inline void __attribute__((always_inline)) KerConv2x1from3x1StrideNx1_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	int V0,V1;
	int C0,C1;
	signed char *PtIn;
	signed char *PtOut;

	if (FilterConf) { /* Right Side */
		C0 = Filter[0]; C1 = Filter[1];
	} else { /* Left Side */
		C0 = Filter[1]; C1 = Filter[2];
	}
	PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V0 = *PtIn; V1 = *(PtIn+1); PtIn += W;
		Acc += V0*C0; Acc += V1*C1;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
	}
}

static inline void __attribute__((always_inline)) KerConv1x2from1x3Stride1xN_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	int V0,V1;
	int C0,C1;
	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	if (FilterConf) { /* Bottom Side */
		C0 = Filter[0]; C1 = Filter[1];
	} else { /* Top Side */
		C0 = Filter[1]; C1 = Filter[2];
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = Bias;
		V0 = *(PtIn+0*W+0); V1 = *(PtIn+1*W+0); PtIn++;
		Acc += V0*C0; Acc += V1*C1;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv2x3from3x3Stride1_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	signed char *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
		V1 = *((v4s *) PtIn); PtIn += W;
	} else  { // == 0
		PtIn = In + (Ho_F*1-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = V1; V1 = V2;
	}
	if (Bottom) {
		int Acc = Bias;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8);
	}
}

static inline void __attribute__((always_inline)) KerConv2x3from3x3Stride2_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	signed char *PtOut = Out;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
	} else  { // == 0
		PtIn = In + (Ho_F*2-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = V2;
	}
	if (Bottom) {
		int Acc = Bias;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); 
	}
}

static inline void __attribute__((always_inline)) KerConv2x3from3x3StrideS_V_SQ8(
	signed char * __restrict__ In,
	int W, int PadTOrg,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s V0, V1, V2;
	v4s C0, C1, C2;
	signed char *PtIn;
	signed char *PtOut;
	int Bottom = Ho-Ho_L;

	if (FilterConf) {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+0)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+0)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+0));
	} else {
		C0 = (v4s) (int) *((unsigned short *) (Filter + 0*3+1)); C1 = (v4s) (int) *((unsigned short *) (Filter + 1*3+1)); C2 = (v4s) (int) *((unsigned short *) (Filter + 2*3+1));
	}
	if (Ho_F==1) {
		PtIn = In; PtOut = Out; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
	} else  { // == 0
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += (Stride-2)*W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
	}
	if (Bottom) {
		int Acc = Bias;
		PtIn -= W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn);;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); 
	}
}


static inline void __attribute__((always_inline)) KerConv3x2from3x3Stride1_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = Bias;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn++;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x2from3x3Stride2_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*2-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = Bias;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=2;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x2from3x3StrideS_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Stride,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1;
	v4s C0, C1;
	signed char *PtIn = In+Wo_F*Stride-PadL;
	signed char *PtOut = Out;

	if (FilterConf) {
		C0 = *((v4s *) &Filter[0*3+0]); C1 = *((v4s *) &Filter[1*3+0]); C0[3] = 0; C1[3] = 0;
	} else {
		C0 = *((v4s *) &Filter[1*3+0]); C1 = *((v4s *) &Filter[2*3+0]); C0[3] = 0; C1[3] = 0;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = Bias;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); PtIn+=Stride;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv4x1from5x1StrideNx1_V_SQ8(
	signed char * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0;
	v4s C0;
	signed char *PtIn;
	signed char *PtOut;

	switch (FilterConf) {
		case 2: // [0..4 x 0] => [2..4 x 0]  PadL==2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			break;
		case 1: // [0..4 x 0] => [1..4 x 0] PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			break;
		case 3: // [0..4 x 0] => [0..3 x 0] PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			break;
		case 4: // [0..4 x 0] => [0..2 x 0] PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0 = (v4s)(((int)C0)<<8);
			break;
		case 5: // [0..4 x 0] => [0..2 x 0] PadR==2, Wo==1
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			break;
	}
	PtIn = In + (Ho_F*1-PadOrg[2])*W; PtOut = Out+Ho_F*Wo;
	V0 = * (v4s *) PtIn; PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		Acc = gap_sumdotp4(V0, C0, Acc);
		V0 = * (v4s *) PtIn; PtIn += W;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
	}
}

static inline void __attribute__((always_inline)) KerConv1x4from1x5Stride1xN_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0;
	v4s C0;
	int x0,x1,x2,x3;
	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT == 2
			C0 = *((v4s *) &Filter[2]);  C0[3] = 0;
			break;
		case 1: // PadT == 1
			C0 = *((v4s *) &Filter[1]);
			break;
		case 3: // PadB == 1
			C0 = *((v4s *) &Filter[0]);
			break;
		case 4: // PadB == 2
			C0 = *((v4s *) &Filter[0]); C0 = (v4s)((int)C0<<8);
			break;
		case 5: // PadB == 2, Ho == 1
			C0 = *((v4s *) &Filter[0]);  C0[3] = 0;
			break;
	}
	x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn+=1;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int Acc = Bias;
		Acc = gap_sumdotp4(V0, C0, Acc);
		x0 = *(PtIn+0*W+0); x1 = *(PtIn+1*W+0); x2 = *(PtIn+2*W+0); x3 = *(PtIn+3*W+0); V0 = gap_pack4(x0,x1,x2,x3); PtIn+=1;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv4x5from5x5Stride1_V_SQ8(
	signed char * __restrict__ In,
	int W, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	signed char *PtOut;
	int Bottom, PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL == 2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL == 1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR == 1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [1..3 x 0..4] PadR == 2
			C0 = *((v4s*) (Filter + 0*5+0)); C0 = (v4s)(((int)C0)<<8);
			C1 = *((v4s*) (Filter + 1*5+0)); C1 = (v4s)(((int)C1)<<8);
			C2 = *((v4s*) (Filter + 2*5+0)); C2 = (v4s)(((int)C2)<<8);
			C3 = *((v4s*) (Filter + 3*5+0)); C3 = (v4s)(((int)C3)<<8);
			C4 = *((v4s*) (Filter + 4*5+0)); C4 = (v4s)(((int)C4)<<8);
			break;
		case 5: // [0..4 x 0..4] => [0..2 x 0..4] PadR == 2, Wo==1
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0};
		V1 = *((v4s *) PtIn); PtIn += W;
	} else { // Ho_F==0
		PtIn = In + (Ho_F*1-PadTOrg)*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
	V2 = *((v4s *) PtIn); PtIn += W;
	V3 = *((v4s *) PtIn); PtIn += W;
 	PtOut = Out+Ho_F*Wo;
	if (Ho==1) {
		int Acc = Bias; Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); *PtOut = Acc;
		return;
	}
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = V1; V1 = V2; V2 = V3; V3 = V4;
	}
	if (PadB) {
		int Acc = Bias;
		PtIn -= 4*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc);
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		if (PadB==2) {
			Acc = Bias;
			V0 = V1; V1 = V2; V2 = V3;
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtOut = OUT_CLIP(Acc, 0, 8); 
		}
	}
}

static inline void __attribute__((always_inline)) KerConv4x5from5x5Stride2_V_SQ8(
	signed char * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	signed char *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL == 2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = *((v4s *) PtIn); PtIn += W;
	} else {
		PtIn = In + (Ho_F*2-PadTOrg)*W;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
 	PtOut = Out+Ho_F*Wo;
	V2 = *((v4s *) PtIn); PtIn += W;
	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = V2; V1 = V3; V2 = V4;
	}
	if (PadB) {
		int Acc = Bias;
		PtIn -= 3*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		if (PadB==1) {
			V3 = *((v4s *) PtIn); Acc = gap_sumdotp4(V3, C3, Acc);
		}
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); 
	}
}

static inline void __attribute__((always_inline)) KerConv4x5from5x5StrideS_V_SQ8(
	signed char * __restrict__ In,
	int W, int H, v4s PadOrg, v4s Pad,
	int Wo, int Ho, int Ho_F, int Ho_L,
	int Stride, 
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	/* Here stride should always be > 2 since we have specialized form for smaller strides */
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;
	signed char *PtIn;
	signed char *PtOut;
	int PadL = PadOrg[0], PadT = Pad[2], PadTOrg = PadOrg[2], PadB = Pad[3];

	switch (FilterConf) {
		case 2: // [0..4 x 0..4] => [2..4 x 0..4] PadL==2
			C0 = *((v4s*) (Filter + 0*5+2)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+2)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+2)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+2)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+2)); C4[3] = 0;
			break;
		case 1: // [0..4 x 0..4] => [1..4 x 0..4] PadL==1
			C0 = *((v4s*) (Filter + 0*5+1));
			C1 = *((v4s*) (Filter + 1*5+1));
			C2 = *((v4s*) (Filter + 2*5+1));
			C3 = *((v4s*) (Filter + 3*5+1));
			C4 = *((v4s*) (Filter + 4*5+1));
			break;
		case 3: // [0..4 x 0..4] => [0..3 x 0..4] PadR==1
			C0 = *((v4s*) (Filter + 0*5+0));
			C1 = *((v4s*) (Filter + 1*5+0));
			C2 = *((v4s*) (Filter + 2*5+0));
			C3 = *((v4s*) (Filter + 3*5+0));
			C4 = *((v4s*) (Filter + 4*5+0));
			break;
		case 4: // [0..4 x 0..4] => [0..2 x 0..4] PadR==2
			C0 = *((v4s*) (Filter + 0*5+0)); C0[3] = 0;
			C1 = *((v4s*) (Filter + 1*5+0)); C1[3] = 0;
			C2 = *((v4s*) (Filter + 2*5+0)); C2[3] = 0;
			C3 = *((v4s*) (Filter + 3*5+0)); C3[3] = 0;
			C4 = *((v4s*) (Filter + 4*5+0)); C4[3] = 0;
			break;
	}
	if (PadT==2) {
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = (v4s){0,0,0,0};
	} else if (PadT) { // == 1
		PtIn = In; Ho_F = 0;
		V0 = (v4s){0,0,0,0}; V1 = *((v4s *) PtIn); PtIn += W;
	} else {
		PtIn = In + (Ho_F*Stride-PadTOrg)*W; PtOut = Out+Ho_F*Wo;
		V0 = *((v4s *) PtIn); PtIn += W;
		V1 = *((v4s *) PtIn); PtIn += W;
	}
 	PtOut = Out+Ho_F*Wo;


	for (unsigned int i=Ho_F; i<Ho_L; i++) {
		int Acc = Bias;
		V2 = *((v4s *) PtIn); PtIn += W; V3 = *((v4s *) PtIn); PtIn += W; V4 = *((v4s *) PtIn); PtIn += (Stride-4)*W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut+=Wo;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W;
	}
	if (PadB) {
		int Acc = Bias;
		PtIn -= 2*W;
		V0 = *((v4s *) PtIn); PtIn += W; V1 = *((v4s *) PtIn); PtIn += W; V2 = *((v4s *) PtIn); PtIn += W;
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
		if (PadB==1) {
			V3 = *((v4s *) PtIn); Acc = gap_sumdotp4(V3, C3, Acc);
		}
		Acc = Acc;
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); 
	}
}

static inline void __attribute__((always_inline)) KerConv5x4from5x5Stride1_H_SQ8(
	signed char * __restrict__ In,
	int W, int PadL,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*1-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 2: // PadT == 2
			C0 = *((v4s *) &Filter[2*5+0]); 
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1: // PadT == 1
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 3: // PadB == 1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4: // PadB == 2
			C0 = (v4s){0,0,0,0};
			C1 = *((v4s *) &Filter[0*5+0]);
			C2 = *((v4s *) &Filter[1*5+0]);
			C3 = *((v4s *) &Filter[2*5+0]); C4 = (v4s){0, Filter[0*5+4], Filter[1*5+4], Filter[2*5+4]};
			break;
		case 5: // PadB == 2, Ho == 1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
			break;
	}
	V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = Bias;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn++;
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
		V0 = __builtin_shuffle(V0, (v4s)(int)V4, (v4s){1,2,3,4});
		V1 = __builtin_shuffle(V1, (v4s)(int)V4, (v4s){1,2,3,5});
		V2 = __builtin_shuffle(V2, (v4s)(int)V4, (v4s){1,2,3,6});
		V3 = __builtin_shuffle(V3, (v4s)(int)V4, (v4s){1,2,3,7});
	}
}

static inline void __attribute__((always_inline)) KerConv5x4from5x5Stride2_H_SQ8(
	signed char * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*2-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 2:	// PadT==2
			C0 = *((v4s *) &Filter[2*5+0]); 
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1:	// PadT==1
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 3: // PadB==1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4: // PadB==2
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
			break;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = Bias;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=(2-4);
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConv5x4from5x5StrideS_H_SQ8(
	signed char * __restrict__ In,
	int W, int H, int PadL, int PadT,
	int Wo, int Wo_F, int Wo_L,
	unsigned int Stride,
	int Bias,
	signed char * __restrict__ Out,
	signed char * __restrict__ Filter,
	int FilterConf,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s V0, V1, V2, V3, V4;
	v4s C0, C1, C2, C3, C4;

	signed char *PtIn = In+Wo_F*Stride-PadL;
	signed char *PtOut = Out;

	switch (FilterConf) {
		case 2:	// PadT==2
			C0 = *((v4s *) &Filter[2*5+0]); 
			C1 = *((v4s *) &Filter[3*5+0]);
			C2 = *((v4s *) &Filter[4*5+0]);
			C3 = (v4s){0,0,0,0}; C4 = (v4s){Filter[2*5+4], Filter[3*5+4], Filter[4*5+4], 0};
			break;
		case 1:	// PadT==1
			C0 = *((v4s *) &Filter[1*5+0]);
			C1 = *((v4s *) &Filter[2*5+0]);
			C2 = *((v4s *) &Filter[3*5+0]);
			C3 = *((v4s *) &Filter[4*5+0]); C4 = (v4s){Filter[1*5+4], Filter[2*5+4], Filter[3*5+4], Filter[4*5+4]};
			break;
		case 3: // PadB==1
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = *((v4s *) &Filter[3*5+0]); C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], Filter[3*5+4]};
			break;
		case 4: // PadB==2
			C0 = *((v4s *) &Filter[0*5+0]);
			C1 = *((v4s *) &Filter[1*5+0]);
			C2 = *((v4s *) &Filter[2*5+0]);
			C3 = (v4s){0,0,0,0};
			C4 = (v4s){Filter[0*5+4], Filter[1*5+4], Filter[2*5+4], 0};
			break;
	}
	for (unsigned int i=Wo_F; i<Wo_L; i++) {
		int x0, x1, x2, x3;
		int Acc = Bias;
		V0 = *((v4s *) (PtIn+0*W+0)); V1 = *((v4s *) (PtIn+1*W+0)); V2 = *((v4s *) (PtIn+2*W+0)); V3 = *((v4s *) (PtIn+3*W+0)); PtIn += 4;
		x0 = PtIn[0*W]; x1 = PtIn[1*W]; x2 = PtIn[2*W]; x3 = PtIn[3*W]; PtIn+=((int)Stride-4);
		V4 = gap_pack4(x0,x1,x2,x3);
		Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc); Acc = gap_sumdotp4(V3, C3, Acc); Acc = gap_sumdotp4(V4, C4, Acc);
		Acc = AT_SCALE(Acc, Scale, ScaleN);
		ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
		*PtOut = OUT_CLIP(Acc, 0, 8); PtOut++;
	}
}

static inline void __attribute__((always_inline)) KerConvNxNStrideS_BorderRedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*Stride;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*Stride;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
	       	for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			ht -= Stride; hb -= Stride;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
	       	for (unsigned int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			hb -= Stride;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw2;
	       	for (unsigned int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wl -= Stride; wr -= Stride;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
	       	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
		       		int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wr -= Stride;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= Stride; wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= Stride;
				}
				ht -= Stride; hb -= Stride;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= Stride; wr -= Stride;
				}
				hb -= Stride;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+Stride) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+Stride) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*Stride-PadTOrg+i)*W + (w*Stride-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= Stride;
				}
				hb -= Stride;
			}
		}
	}
}

static inline void __attribute__((always_inline)) KerConvNxMStrideSxSy_BorderRedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int Fw2 = (Fw-1)/2, Fh2 = (Fh-1)/2;
	int Hi_F = Fh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;	// iff Hi_L>Hi_F
	int Wi_F = Fw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;	// iff Wi_L>Wi_F

	if (PadT) { /* Top */
		int ht = PadTOrg, hb = H - Hi_F + Fh2;
	       	for (unsigned int h=0; h<Ho_F; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			ht -= StrideY; hb -= StrideY;
		}
	}
	if (PadB) { /* Bottom */
		int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
	       	for (unsigned int h=Ho_L; h<Ho; h++) {
			int Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			hb -= StrideY;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg, wr = W - Wi_F + Fw2;
	       	for (unsigned int w=0; w<Wo_F; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // wh Can't be < 0 by definition of Wo_F so we can remove and use wl only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wl -= StrideX; wr -= StrideX;
		}
	}
	if (PadR) { /* Right */
		int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
	       	for (unsigned int w=Wo_L; w<Wo; w++) {
			int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
		       		int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wr -= StrideX;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only. ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= StrideX; wr -= StrideX;
				}
				ht -= StrideY; hb -= StrideY;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg, hb = H - Hi_F + Fh2;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
					// ht Can't be > F by definition of Ho_L so we can remove and use ht only. ht Can't be > F by definition of Ho_L so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= StrideX;
				}
				ht -= StrideY; hb -= StrideY;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg, wr = W - Wi_F + Fw2;
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= StrideX; wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int ht = 0, hb = H - (Hi_L+StrideY) + Fh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = 0, wr = W - (Wi_L+StrideX) + Fw2;
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
 					// wh Can't be < 0 by definition of Wo_F so we can remove and use wl only.  ht Can't be < 0 by definition of Ho_F so we can remove and use ht only
					int Wh_min = wl, Wh_max = AT_CLIP_POS(wr, Fw), Fh_min = ht, Fh_max = AT_CLIP_POS(hb, Fh);
					for (unsigned int i=Fh_min; i<Fh_max; i++) 
						for (unsigned int j=Wh_min; j<Wh_max; j++) Acc += In[(h*StrideY-PadTOrg+i)*W + (w*StrideX-PadLOrg+j)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static inline void __attribute__((always_inline)) KerConvNxMDxDyStrideSxSy_BorderRedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int Dw,
	int Dh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadROrg = PadOrg[1], PadBOrg = PadOrg[3];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];
	int TFw = Dw*(Fw-1)+1, TFh = Dh*(Fh-1)+1;
	int TFw2 = (TFw-1)/2, TFh2 = (TFh-1)/2;
	int Hi_F = TFh2 - PadTOrg;
	int Hi_L = Hi_F + (Ho_L-1)*StrideY;	// iff Hi_L>Hi_F
	int Wi_F = TFw2 - PadLOrg;
	int Wi_L = Wi_F + (Wo_L-1)*StrideX;	// iff Wi_L>Wi_F
	#ifndef Prec
	int Prec=10;
	#endif
	int InvDh = ((1<<Prec)+Dh-1)/Dh;
	int InvDw = ((1<<Prec)+Dw-1)/Dw;

	/*
	Here we assume that for a given filter output we don't have padding on both side of the input.
	Thanks to this assumption we can simplify a bit where filter starts and stops in the input.
	Either starts at 0 if (right or bottom) and stops at a place function of the padding or
	stops at Fw/Fh if (left or bottom) and starts  a place function of the padding
	*/
	if (PadT) { /* Top */
		int ht = PadTOrg;
	       	for (unsigned int h=0; h<Ho_F; h++) {
			int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
			int Fh_min = hta;
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=Fh_min; i<Fh; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			ht -= StrideY;
		}
	}
	if (PadB) { /* Bottom */
		int hb = H - (Hi_L+StrideY) + TFh2;
	       	for (unsigned int h=Ho_L; h<Ho; h++) {
			int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
			int Fh_max = AT_CLIP_POS(hba, Fh);
			for (unsigned int w=Wo_F; w<Wo_L; w++) {
				int Acc = Bias;
				for (unsigned int i=0; i<Fh_max; i++) 
					for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			hb -= StrideY;
		}
	}
	if (PadL) { /* Left */
		int wl = PadLOrg;
	       	for (unsigned int w=0; w<Wo_F; w++) {
			int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
			int Wl_min = wla;
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
				int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wl -= StrideX;
		}
	}
	if (PadR) { /* Right */
		int wr = W - (Wi_L+StrideX) + TFw2;
	       	for (unsigned int w=Wo_L; w<Wo; w++) {
			int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
			int Wr_max = AT_CLIP_POS(wra, Fw); // ht Can't be > F by definition of Ho_L so we can remove and use ht only
			for (unsigned int h=Ho_F; h<Ho_L; h++) {
		       		int Acc = Bias;
				for (unsigned int i=0; i<Fh; i++) 
					for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
				Acc = AT_SCALE(Acc, Scale, ScaleN);
				ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
				Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
			}
			wr -= StrideX;
		}
	}
	if (PadT) {
		if (PadL) { /* Upper left corner */
			int ht = PadTOrg;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wl = PadLOrg;
				int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
					int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
					int Wl_min = wla, Fh_min = hta;
					for (unsigned int i=Fh_min; i<Fh; i++) 
						for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= StrideX;
				}
				ht -= StrideY;
			}
		}
		if (PadR) { /* Upper right corner */
			int ht = PadTOrg;
			for (unsigned int h=0; h<Ho_F; h++) {
				int wr = W - (Wi_L+StrideX) + TFw2;
				int hta = gap_mulsN(ht-1, InvDh, Prec) + 1; // hta = (ht-1)/Dh+1
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = AT_CLIP_POS(wra, Fw), Fh_min = hta;
					for (unsigned int i=Fh_min; i<Fh; i++) 
						for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= StrideX;
				}
				ht -= StrideY;
			}
		}
	}
	if (PadB) {
		if (PadL) { /* Bottom Left corner */
			int hb = H - (Hi_L+StrideY) + TFh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wl = PadLOrg;
				int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
				for (unsigned int w=0; w<Wo_F; w++) {
					int Acc = Bias;
					int wla = gap_mulsN(wl-1, InvDw, Prec) + 1; // wla = (wl-1)/Dw+1
					int Wl_min = wla, Fh_max = AT_CLIP_POS(hba, Fh);
					for (unsigned int i=0; i<Fh_max; i++) 
						for (unsigned int j=Wl_min; j<Fw; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wl -= StrideX;
				}
				hb -= StrideY;
			}
		}
		if (PadR) { /* Bottom Right corner */
			int hb = H - (Hi_L+StrideY) + TFh2;
			for (unsigned int h=Ho_L; h<Ho; h++) {
				int wr = W - (Wi_L+StrideX) + TFw2;
				int hba = gap_mulsN(hb-1, InvDh, Prec) + 1; // hba = (hb-1)/Dh+1
				for (unsigned int w=Wo_L; w<Wo; w++) {
					int Acc = Bias;
					int wra = gap_mulsN(wr-1, InvDw, Prec) + 1; // wra = (wr-1)/Dw+1
					int Wr_max = AT_CLIP_POS(wra, Fw), Fh_max = AT_CLIP_POS(hba, Fh);
					for (unsigned int i=0; i<Fh_max; i++) 
						for (unsigned int j=0; j<Wr_max; j++) Acc += In[(h*StrideY-PadTOrg+i*Dh)*W + (w*StrideX-PadLOrg+j*Dw)]*Filter[Fw*i+j];
					Acc = AT_SCALE(Acc, Scale, ScaleN);
					ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
					Out[Wo*h+w] = OUT_CLIP(Acc, 0, 8);
					wr -= StrideX;
				}
				hb -= StrideY;
			}
		}
	}
}

static inline void __attribute__((always_inline)) KerConv3x1BorderRedActStrideNx1_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x1from3x1StrideNx1_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv2x1from3x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv1x3BorderRedActStride1xN_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) KerConv1x2from1x3Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv1x2from1x3Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv3x3BorderRedActStride1_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int Fh=3, Fw=3, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride1_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv2x3from3x3Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadT) KerConv3x2from3x3Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv3x2from3x3Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv3x3BorderRedActStride2_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int Fh=3, Fw=3, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3Stride2_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv2x3from3x3Stride2_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadT) KerConv3x2from3x3Stride2_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv3x2from3x3Stride2_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv3x3BorderRedActStrideS_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	/* Stride is > 2 */
	int Fh=3, Fw=3;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv2x3from3x3StrideS_V_SQ8(In, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv2x3from3x3StrideS_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadTOrg, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out+Wo-1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadT) KerConv3x2from3x3StrideS_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Wo_F, Filter, 0, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv3x2from3x3StrideS_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Ho_L*Wo+Wo_F, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv5x1BorderRedActStrideNx1_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) {
		if (Wo_F==2) {
			KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		} else KerConv4x1from5x1StrideNx1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, PadL, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	}
	if (PadR) {
		if ((Wo-Wo_L)==2) {
			KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-2, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 4, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		} else if (Wo==1) KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 5, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		else KerConv4x1from5x1StrideNx1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, PadR+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	}
}

static inline void __attribute__((always_inline)) KerConv1x5BorderRedActStride1xN_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadT) {
		if (Ho_F==2) { // Happens only if stride = 1
			KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F+Wo, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		} else KerConv1x4from1x5Stride1xN_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, PadT, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	}
	if (PadB) {
		if ((Ho-Ho_L)==2) { // Happens only if stride == 1
			KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		} else if (Ho==1) KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 5, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		else KerConv1x4from1x5Stride1xN_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	}
}

static inline void __attribute__((always_inline)) KerConv5x5BorderRedActStride1_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	/* With stride=1 we are sure that padding will be 2, 2 for a given dim */
	int Fh=5, Fw=5, Stride=1;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL==2) {
		KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+1, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	} else if (PadL==1) KerConv4x5from5x5Stride1_V_SQ8(In, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR==2) {
		if (Wo==1) KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 5, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		else {
			KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-2, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 4, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		}
	} else if (PadR==1) KerConv4x5from5x5Stride1_V_SQ8(In+Wo_L*Stride-PadLOrg, W, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadT==2) {
		KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F+Wo, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	} else if (PadT==1) KerConv5x4from5x5Stride1_H_SQ8(In, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, 1, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB==2) {
		if (Ho==1) KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L)*Wo+Wo_F, Filter, 5, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		else {
			KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
			KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+(Ho_L+1)*Wo+Wo_F, Filter, 4, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
		}
	} else if (PadB==1) KerConv5x4from5x5Stride1_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, PadLOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, 3, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv5x5BorderRedActStride2_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	/* Max padding is 4, distributed Pad/2, Pad-Pad/2 or Pad-Pad/2, Pad, with stride 2 each padded area cannot produce more than 1 outputput  */
	int Fh=5, Fw=5, Stride=2;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5Stride2_V_SQ8(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out, Filter, PadL, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv4x5from5x5Stride2_V_SQ8(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Bias, Out+Wo-1, Filter, PadR+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);

	if (PadT) KerConv5x4from5x5Stride2_H_SQ8(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Bias, Out+Wo_F, Filter, PadT, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv5x4from5x5Stride2_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

static inline void __attribute__((always_inline)) KerConv5x5BorderRedActStrideS_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	v4s PadOrg,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	/* Stride is assumed to be >2 since we have specialized variants therefore no more than 1 output can be created in each padded area */
	int Fh=5, Fw=5;
	int PadLOrg = PadOrg[0], PadTOrg = PadOrg[2];
	int PadL = Pad[0], PadR = Pad[1], PadT = Pad[2], PadB = Pad[3];

	if (PadL) KerConv4x5from5x5StrideS_V_SQ8(In, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out, Filter, PadL, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadR) KerConv4x5from5x5StrideS_V_SQ8(In+Wo_L*Stride-PadLOrg, W, H, PadOrg, Pad, Wo, Ho, Ho_F, Ho_L, Stride, Bias, Out+Wo-1, Filter, PadR+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);

	if (PadT) KerConv5x4from5x5StrideS_H_SQ8(In, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Wo_F, Filter, PadT, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
	if (PadB) KerConv5x4from5x5StrideS_H_SQ8(In+(Ho_L*Stride-PadTOrg)*W, W, H, PadLOrg, PadTOrg, Wo, Wo_F, Wo_L, Stride, Bias, Out+Ho_L*Wo+Wo_F, Filter, PadB+2, Scale, ScaleN, Activation, ActScale, ActScaleN, A, B, C);
}

/* Convolution, body processing (covers both padded and non padded variants)

	Input feature maps, Output feature maps and filter on bytes

		KerConv1x1Stride1_Body_RedAct_SQ8		1x1 convolution, stride 1
		KerConv1x1Stride2_Body_RedAct_SQ8		1x1 convolution, stride 2
		KerConv1x1StrideS_Body_RedAct_SQ8		1x1 convolution, stride S

		KerConv3x1Stride1x1_Body_RedAct_SQ8		3x1 convolution, stride 1x1
		KerConv3x1Stride2x1_Body_RedAct_SQ8		3x1 convolution, stride 2x1
		KerConv1x3Stride1x1_Body_RedAct_SQ8		1x3 convolution, stride 1x1
		KerConv1x3Stride1x2_Body_RedAct_SQ8		1x3 convolution, stride 1x2
		KerConv3x3Stride1_Body_RedAct_SQ8		3x3 convolution, stride 1
		KerConv3x3Stride2_Body_RedAct_SQ8		3x3 convolution, stride 2
		KerConv3x3StrideS_Body_RedAct_SQ8		3x3 convolution, stride S

		KerConv5x1Stride1x1_Body_RedAct_SQ8		5x1 convolution, stride 1x1
		KerConv5x1Stride2x1_Body_RedAct_SQ8		5x1 convolution, stride 2x1
		KerConv1x5Stride1x1_Body_RedAct_SQ8		1x5 convolution, stride 1x1
		KerConv1x5Stride1x2_Body_RedAct_SQ8		1x5 convolution, stride 1x2
		KerConv5x5Stride1_Body_RedAct_SQ8		5x5 convolution, stride 1
		KerConv5x5Stride2_Body_RedAct_SQ8		5x5 convolution, stride 2
		KerConv5x5StrideS_Body_RedAct_SQ8		5x5 convolution, stride S
		KerConv7x7StrideS_Body_RedAct_SQ8		7x7 convolution, stride S

		KerConvNxNStrideS_Body_RedAct_SQ8		NxN convolution, stride S
		KerConvNxMStrideSxSy_Body_RedAct_SQ8		NxM convolution, stride Sx, Sy
		KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8	NxM convolution, dilation Dx,Dy, stride Sx, Sy

*/

static inline void __attribute__((always_inline)) KerConv1x1Stride1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	int Stride = 1;
        int PadL = Pad[0], PadT = Pad[2];

        int C0 = Filter[0];
        int IterW = Wo_L-Wo_F;
        for (unsigned int h=Ho_F; h<Ho_L; h++) {
                int *LineOut = (int *) (&Out[Wo*h+Wo_F]);
                signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
                for (unsigned int w=0; w<(IterW/2); w++) {
                        LineOut[2*w] = PtI[2*w]*C0+Bias; LineOut[2*w+1] = PtI[2*w+1]*C0+Bias;
                }
                if (IterW&0x1) Out[Wo*h+Wo_L-1] = Bias + PtI[IterW-1]*C0;
        }
}

static inline void __attribute__((always_inline)) KerConv1x1Stride2_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	int Stride = 2;
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = Bias;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static inline void __attribute__((always_inline)) KerConv1x1StrideS_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	int C0 = Filter[0];
	int Fw = 1, Fh = 1;
	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = In + (h*Stride-PadT)*W + (Wo_F*Stride-PadL);
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = Bias;
			int I = *PtI; PtI+=Stride;
			Acc += I*C0;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static inline void __attribute__((always_inline)) KerConv3x1Stride1x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = Acc;
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x1Stride2x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	unsigned short int StrideX = 2;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = Acc;
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv1x3Stride1x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	v4s Mask = (v4s) {1,2,4,0};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		V0[1] = *PtI; PtI = ((signed char *)PtI+W);
		V0[2] = *PtI; PtI = ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			int X0 = *PtI; PtI = ((signed char *)PtI+W);
			V0 = __builtin_shuffle(V0, (v4s) X0, Mask);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = Acc;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv1x3Stride1x2_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]);
	v4s V0;
	v4s Mask = (v4s) {2,4,5,0};
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		V0[2] = *PtI; PtI = ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			unsigned int X0 = *(unsigned char *) PtI; PtI = ((signed char *)PtI+W);
			unsigned int X1 = *(unsigned char *) PtI; PtI = ((signed char *)PtI+W);
			X0 = X0 | (X1<<8);
			V0 = __builtin_shuffle(V0, (v4s) X0, Mask);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = Acc;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x3Stride1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			Acc = Acc;
			V0 = V1; V1 = V2;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x3Stride2_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			Acc = Acc;
			V0 = V2;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv3x3StrideS_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0 = *((v4s *) &Filter[0]), C1 = *((v4s *) &Filter[3]), C2 = *((v4s *) &Filter[6]);
	v4s V0, V1, V2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	C0[3]=0; C1[3]=0; C2[3]=0;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int Acc = Bias;
			V0 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W);
			V2 = *PtI; PtI = (v4s*) ((signed char *)PtI+(Stride-2)*W);
			Acc = gap_sumdotp4(V0, C0, Acc); Acc = gap_sumdotp4(V1, C1, Acc); Acc = gap_sumdotp4(V2, C2, Acc);
			Acc = Acc;
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv5x1Stride1x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s V0;
	int StrideX = 1;
	int StrideY = 1;
	int PadL = Pad[0], PadT = 0;
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	v4s Mask = (v4s) {1,2,3,4};

       	for (int h=Ho_F; h<Ho_L; h++) {
		signed char *PtI = (In + (h*StrideY-PadT)*W + (Wo_F*StrideX-PadL));
		signed char *PtO = PtO1;
		v4s V0 = ((v4s *)PtI)[0];
		int x0 = PtI[4];
		PtI += 5;
       		for (int w=Wo_F; w<Wo_L; w++) {
			int S = Bias;
			S = gap_sumdotp4(V0,  C0,  S); S += x0*C1;
			V0 = __builtin_shuffle(V0, (v4s) x0, Mask); x0 = *PtI; PtI++;
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO++;
		}
		PtO1+=Wo;
	}
}

static inline void __attribute__((always_inline)) KerConv5x1Stride2x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s V0;

	unsigned short int StrideX = 2;
	unsigned short int StrideY = 1;
	unsigned short int PadL = Pad[0], PadT = 0;
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		int x0;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S += x0*C1;
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv1x5Stride1x1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s Mask = (v4s) {1,2,3,4};
	v4s V0;
	signed char V1;

	unsigned short int StrideX = 1;
	unsigned short int StrideY = 1;
	unsigned short int PadL = 0, PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		int x0,x1,x2;
		x0 = *PtI; PtI = PtI+W;
		x1 = *PtI; PtI = PtI+W;
		x2 = *PtI; PtI = PtI+W;
		V0 = gap_pack4(0,x0,x1,x2);
		V1 = *PtI; PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V0 = __builtin_shuffle(V0, (v4s)((int) V1), Mask);
			V1 = (signed char)(*PtI); PtI = PtI+W;
			S = gap_sumdotp4(V0,  C0,  S); S += V1*C1;
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv1x5Stride1x2_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	v4s C0  = *((v4s *) &Filter[0]);
	signed char C1 = Filter[4];
	v4s Mask = (v4s) {2,3,4,5};
	v4s V0, V1;

	unsigned short int StrideX = 1;
	unsigned short int StrideY = 2;
	unsigned short int PadL = 0, PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		signed char *PtI = (In + (Ho_F*StrideY-PadT)*W + (w*StrideX-PadL));
		signed char *PtO = PtO1;
		int x0,x1;
		x0 = *PtI; PtI = PtI+W;
		x1 = *PtI; PtI = PtI+W;
		V0 = gap_pack4(0,0,x0,x1);
		V1 = (v4s) ((int) (*PtI)); PtI = PtI+W;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			x0 = *PtI; PtI = PtI+W;
			V1[1] = x0; V0 = __builtin_shuffle(V0, (v4s)((int) V1), Mask);
			x1 = (*PtI); PtI = PtI+W; V1 = (v4s)((int)x1);
			S = gap_sumdotp4(V0,  C0,  S); S += x1*C1;
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv5x5Stride1_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {1,2,3,4};

	unsigned short int Stride = 1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		int x0,x1,x2,x3;
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V5 = gap_pack4(x0,x1,x2,x3);

		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			V0 = V1; V1 = V2; V2 = V3; V3 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv5x5Stride2_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {2,3,4,4};

	unsigned short int Stride = 2;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		int x0,x1,x2,x3;
		V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
		V5 = gap_pack4(x0,x1,x2,0);
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V3 = *PtI++; V5[3] = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			V0 = V2; V1 = V3; V2 = V4;
			V5 = __builtin_shuffle(V5, V6, Mask);
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv5x5StrideS_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0  = *((v4s *) &Filter[0]),
	    C1  = *((v4s *) &Filter[5]),
	    C2  = *((v4s *) &Filter[10]),
	    C3  = *((v4s *) &Filter[15]),
	    C4  = *((v4s *) &Filter[20]),
	    C5 = gap_pack4(Filter[4], Filter[9], Filter[14], Filter[19]),
	    C6 = (v4s)(int)(*((unsigned char *) &Filter[24]));

	v4s V0, V1, V2, V3, V4, V5, V6;
	v4s Mask  = {2,3,4,4};

	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;

       	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *PtO = PtO1;
		int x0,x1,x2,x3;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V0 = *PtI++; x0 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V1 = *PtI++; x1 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V2 = *PtI++; x2 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4);
			V3 = *PtI++; x3 = *((signed char *) PtI); PtI = (v4s*) ((signed char *)PtI+W-4); V5 = gap_pack4(x0,x1,x2,x3);
			V4 = *PtI++; V6 = (v4s) (int) (*((signed char *) PtI)); PtI = (v4s*) ((signed char *)PtI+(Stride-4)*W-4);
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = gap_sumdotp4(V2,  C2,  S); S = gap_sumdotp4(V3,  C3,  S);
			S = gap_sumdotp4(V4,  C4,  S); S = gap_sumdotp4(V5,  C5,  S); S = gap_sumdotp4(V6,  C6,  S);
			// V5 = __builtin_shuffle(V5, V6, Mask);
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConv7x7StrideS_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)

{
	v4s C0, C1;
	v4s V0, V1;
	unsigned short int PadL = Pad[0], PadT = Pad[2];
	signed char *PtO1 = Out+Wo*Ho_F+Wo_F;
	int Off = (6 - Stride)*W;

	for (unsigned int w=Wo_F; w<Wo_L; w++) {
		v4s *__restrict__ PtI = (v4s *) (In + (Ho_F*Stride-PadT)*W + (w*Stride-PadL));
		signed char *__restrict__ PtO = PtO1;
		v4s *__restrict__ PtC = (v4s *) Filter;
		for (unsigned int h=Ho_F; h<Ho_L; h++) {
			int S = Bias;
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI+W-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			V0 = *PtI++; V1 = *PtI; PtI = (v4s*) ((signed char *)PtI-Off-4); C0 = *PtC++; C1 = *PtC; PtC = (v4s *) ((signed char *)PtC+3-49); C1[3] = 0;
			S = gap_sumdotp4(V0,  C0,  S); S = gap_sumdotp4(V1,  C1,  S);
			S = AT_SCALE(S, Scale, ScaleN);
			ACT_SWITCH(S, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(S, 0, 8); PtO+=Wo;
		}
		PtO1++;
	}
}

static inline void __attribute__((always_inline)) KerConvNxNStrideS_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int Stride,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*Stride-PadT+i)*W + (w*Stride-PadL+j)]*Filter[Fw*i+j];
			}
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static inline void __attribute__((always_inline)) KerConvNxMStrideSxSy_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadT+i)*W + (w*StrideX-PadL+j)]*Filter[Fw*i+j];
			}
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

static inline void __attribute__((always_inline)) KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8(
	signed char *__restrict__ In,
	signed char * __restrict__ Out,
	signed char *__restrict__ Filter,
	int Fw,
	int Fh,
	int Dw,
	int Dh,
	int W,
	int H,
	int Wo,
	int Wo_F,
	int Wo_L,
	int Ho,
	int Ho_F,
	int Ho_L,
	int StrideX,
	int StrideY,
	v4s Pad,
	int Bias,
	unsigned char Scale,
	unsigned char ScaleN,
	CNN_ActivationOper_T Activation,
	unsigned char ActScale,
	unsigned char ActScaleN,
	signed char A,
	signed char B,
	signed char C
	)
{
	unsigned short int PadL = Pad[0], PadT = Pad[2];

	signed char *PtO = Out+Wo*Ho_F+Wo_F;
	signed char *PtC = Filter;
	for (unsigned int h=Ho_F; h<Ho_L; h++) {
		for (unsigned int w=Wo_F; w<Wo_L; w++) {
			int Acc = Bias;
			for (unsigned int i=0; i<Fh; i++) {
				for (unsigned int j=0; j<Fw; j++) Acc += In[(h*StrideY-PadT+i*Dh)*W + (w*StrideX-PadL+j*Dw)]*Filter[Fw*i+j];
			}
			Acc = AT_SCALE(Acc, Scale, ScaleN);
			ACT_SWITCH(Acc, Activation, ActScale, ActScaleN, A, B, C, 32, 0);
			*PtO = OUT_CLIP(Acc, 0, 8); PtO++;
		}
		PtO = PtO + (Wo-Wo_L)+Wo_F;
	}
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are bytes (_SQ8) Dim=1,3,5,N, Stride=1,2,S

	Output feature maps are evaluated in parallel, one per core

	Argument data type: KerConvRedAct_SQ8_T
	Bias 8b: B8, 16b: B16, 32b: B32

	KerParConvDWRedAct1x1Stride1_SQ8
	KerParConvDWRedAct1x1Stride2_SQ8
	KerParConvDWRedAct1x1StrideS_SQ8

	KerParConvDWRedAct3x1Stride1x1_SQ8
		|------	KerConv3x1Stride1x1_Body_RedAct_SQ8
		|------	KerConv3x1StrideNx1_BorderRedAct_SQ8
	KerParConvDWRedAct3x1Stride2x1_SQ8
		|------	KerConv3x1Stride2x1_Body_RedAct_SQ8
		|------	KerConv3x1StrideNx1_BorderRedAct_SQ8
	KerParConvDWRedAct1x3Stride1x1_SQ8
		|------	KerConv1x3Stride1x1_Body_RedAct_SQ8
		|------	KerConv1x3Stride1xN_BorderRedAct_SQ8
	KerParConvDWRedAct1x3Stride1x2_SQ8
		|------	KerConv1x3Stride1x2_Body_RedAct_SQ8
		|------	KerConv1x3Stride1xN_BorderRedAct_SQ8
	KerParConvDWRedAct3x3Stride1_SQ8
		|------	KerConv3x3Stride1_Body_RedAct_SQ8
		|------	KerConv3x3Stride1_BorderRedAct_SQ8
	KerParConvDWRedAct3x3Stride2_SQ8
		|------	KerConv3x3Stride2_Body_RedAct_SQ8
		|------	KerConv3x3Stride2_BorderRedAct_SQ8
	KerParConvDWRedAct3x3StrideS_SQ8
		|------	KerConv3x3StrideS_Body_RedAct_SQ8
		|------	KerConv3x3StrideS_BorderRedAct_SQ8

	KerParConvDWRedAct5x1Stride1x1_SQ8
		|------	KerConv5x1Stride1x1_Body_RedAct_SQ8
		|------	KerConv5x1StrideNx1_BorderRedAct_SQ8
	KerParConvDWRedAct5x1Stride2x1_SQ8
		|------	KerConv5x1Stride2x1_Body_RedAct_SQ8
		|------	KerConv5x1StrideNx1_BorderRedAct_SQ8
	KerParConvDWRedAct1x5Stride1x1_SQ8
		|------	KerConv1x5Stride1x1_Body_RedAct_SQ8
		|------	KerConv1x5Stride1xN_BorderRedAct_SQ8
	KerParConvDWRedAct1x5Stride1x2_SQ8
		|------	KerConv1x5Stride1x2_Body_RedAct_SQ8
		|------	KerConv1x5Stride1xN_BorderRedAct_SQ8
	KerParConvDWRedAct5x5Stride1_SQ8
		|------	KerConv5x5Stride1_Body_RedAct_SQ8
		|------	KerConv5x5Stride1_BorderRedAct_SQ8
	KerParConvDWRedAct5x5Stride2_SQ8
		|------	KerConv5x5Stride2_Body_RedAct_SQ8
		|------	KerConv5x5Stride2_BorderRedAct_SQ8
	KerParConvDWRedAct5x5StrideS_SQ8
		|------	KerConv5x5StrideS_Body_RedAct_SQ8
		|------	KerConvNxNStrideS_BorderRedAct_SQ8
	KerParConvDWRedAct7x7StrideS_SQ8
		|------	KerConv7x7StrideS_Body_RedAct_SQ8
		|------	KerConvNxNStrideS_BorderRedAct_SQ8

	KerParConvDWRedActNxNStrideS_SQ8
		|------	KerConvNxNStrideS_Body_RedAct_SQ8
		|------	KerConvNxNStrideS_BorderRedAct_SQ8

	KerParConvDWRedActNxMStrideSxSy_SQ8
		|------	KerConvNxMStrideSxSy_Body_RedAct_SQ8
		|------	KerConvNxMStrideSxSy_BorderRedAct_SQ8

	KerParConvDWRedActNxMDxDyStrideSxSy_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8
		|------	KerConvNxMDxDyStrideSxSy_BorderRedAct_SQ8
*/

/* 32 bits Bias */

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x1Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x1Stride1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x1Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x1Stride2_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x1StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x1StrideS_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct3x1Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=3, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv3x1Stride1x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x1BorderRedActStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct3x1Stride2x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=3, FSy=1, Sx=2, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv3x1Stride2x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x1BorderRedActStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x3Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x3Stride1x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x3BorderRedActStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x3Stride1x2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, FSy=3, Sx=1, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x3Stride1x2_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x3BorderRedActStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct3x3Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv3x3Stride1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStride1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct3x3Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv3x3Stride2_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStride2_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct3x3StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv3x3StrideS_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStrideS_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct5x1Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=5, FSy=1, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv5x1Stride1x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x1BorderRedActStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct5x1Stride2x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=5, FSy=1, Sx=2, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv5x1Stride2x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x1BorderRedActStrideNx1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x5Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x5Stride1x1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x5BorderRedActStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 1, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct1x5Stride1x2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, FSy=5, Sx=1, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;


	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv1x5Stride1x2_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x5BorderRedActStride1xN_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, 2, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct5x5Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv5x5Stride1_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStride1_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct5x5Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv5x5Stride2_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStride2_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct5x5StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv5x5StrideS_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStrideS_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedAct7x7StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=7, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConv7x7StrideS_Body_RedAct_SQ8(in, out, filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxNStrideS_BorderRedAct_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedActNxNStrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=Arg->N, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FS*FS*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConvNxNStrideS_Body_RedAct_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxNStrideS_BorderRedAct_SQ8(in, out, filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedActNxMStrideSxSyB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConvNxMStrideSxSy_Body_RedAct_SQ8(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxMStrideSxSy_BorderRedAct_SQ8(in, out, filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	unsigned int NormBias = Arg->NormBias;
	unsigned int OutFeatures = Arg->OutFeatures;
	signed char * __restrict__ Filter = Arg->Filter;
	int * __restrict__ Bias = (int * __restrict__) Arg->Bias;
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char * __restrict__ Scale  = (unsigned char * __restrict__) Arg->Scale;
	unsigned char * __restrict__ ScaleN = (unsigned char * __restrict__) Arg->ScaleN;
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];

	unsigned int CoreId = gap_coreid();
	unsigned int Chunk = ChunkSize(OutFeatures);
	unsigned int First = Chunk*CoreId;
	unsigned int Last = Min(First+Chunk, OutFeatures);
	v4s PadIn = Arg->Pad;

	int Wo = (Arg->UsedW-(Dx*(FSx-1)+1)+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput((Dx*(FSx-1)+1), PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, (Dx*(FSx-1)+1), PadIn[0], Sx));
	int Ho = (Arg->UsedH-(Dy*(FSy-1)+1)+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput((Dy*(FSy-1)+1), PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, (Dy*(FSy-1)+1), PadIn[2], Sy));

       	for (unsigned int of=First; of<Last; of++) {
		signed char *in = In+W*H*of, *filter = Filter+FSx*FSy*of; signed char *out = Out+Wo*Ho*of;
		int B = AT_LSHIFT(Bias[of], NormBias);
		KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxMDxDyStrideSxSy_BorderRedAct_SQ8(in, out, filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadIn, B, Scale[of], ScaleN[of], Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

/*
	Optionally 0 padded convolutions.

	Input, output features and filters are bytes (_SQ8) Dim=1,3,5,N, Stride=1,2,S

	A single feature map is evaluated in parallel on all cores

	Argument data type: KerConvRedAct_SQ8_T

	KerConvDWRedAct1x1Stride1_SQ8
	KerConvDWRedAct1x1Stride2_SQ8
	KerConvDWRedAct1x1StrideS_SQ8

	KerConvDWRedAct3x1Stride1x1_SQ8
		|------	KerConv3x1Stride1x1_Body_RedAct_SQ8
		|------	KerConv3x1StrideNx1_BorderRedAct_SQ8
	KerConvDWRedAct3x1Stride2x1_SQ8
		|------	KerConv3x1Stride2x1_Body_RedAct_SQ8
		|------	KerConv3x1StrideNx1_BorderRedAct_SQ8
	KerConvDWRedAct1x3Stride1x1_SQ8
		|------	KerConv1x3Stride1x1_Body_RedAct_SQ8
		|------	KerConv1x3Stride1xN_BorderRedAct_SQ8
	KerConvDWRedAct1x3Stride1x2_SQ8
		|------	KerConv1x3Stride1x2_Body_RedAct_SQ8
		|------	KerConv1x3Stride1xN_BorderRedAct_SQ8
	KerConvDWRedAct3x3Stride1_SQ8
		|------	KerConv3x3Stride1_Body_RedAct_SQ8
		|------	KerConv3x3Stride1_BorderRedAct_SQ8
	KerConvDWRedAct3x3Stride2_SQ8
		|------	KerConv3x3Stride2_Body_RedAct_SQ8
		|------	KerConv3x3Stride2_BorderRedAct_SQ8
	KerConvDWRedAct3x3StrideS_SQ8
		|------	KerConv3x3StrideS_Body_RedAct_SQ8
		|------	KerConv3x3StrideS_BorderRedAct_SQ8

	KerConvDWRedAct5x1Stride1x1_SQ8
		|------	KerConv5x1Stride1x1_Body_RedAct_SQ8
		|------	KerConv5x1StrideNx1_BorderRedAct_SQ8
	KerConvDWRedAct5x1Stride2x1_SQ8
		|------	KerConv5x1Stride2x1_Body_RedAct_SQ8
		|------	KerConv5x1StrideNx1_BorderRedAct_SQ8
	KerConvDWRedAct1x5Stride1x1_SQ8
		|------	KerConv1x5Stride1x1_Body_RedAct_SQ8
		|------	KerConv1x5Stride1xN_BorderRedAct_SQ8
	KerConvDWRedAct1x5Stride1x2_SQ8
		|------	KerConv1x5Stride1x2_Body_RedAct_SQ8
		|------	KerConv1x5Stride1xN_BorderRedAct_SQ8
	KerConvDWRedAct5x5Stride1_SQ8
		|------	KerConv5x5Stride1_Body_RedAct_SQ8
		|------	KerConv5x5Stride1_BorderRedAct_SQ8
	KerConvDWRedAct5x5Stride2_SQ8
		|------	KerConv5x5Stride2_Body_RedAct_SQ8
		|------	KerConv5x5Stride2_BorderRedAct_SQ8
	KerConvDWRedAct5x5StrideS_SQ8
		|------	KerConv5x5StrideS_Body_RedAct_SQ8
		|------	KerConv5x5StrideS_BorderRedAct_SQ8
	KerConvDWRedAct7x7StrideS_SQ8
		|------	KerConv7x7StrideS_Body_RedAct_SQ8
		|------	KerConv7x7StrideS_BorderRedAct_SQ8

	KerConvDWRedActNxNStrideS_SQ8
		|------	KerConvNxNStrideS_Body_RedAct_SQ8
		|------	KerConvNxNStrideS_BorderRedAct_SQ8

	KerConvDWRedActNxMStrideSxSy_SQ8
		|------	KerConvNxMStrideSxSy_Body_RedAct_SQ8
		|------	KerConvNxMStrideSxSy_BorderRedAct_SQ8

	KerConvDWRedActNxMDxDyStrideSxSy_SQ8
		|------	KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8
		|------	KerConvNxMDxDyStrideSxSy_BorderRedAct_SQ8
*/

/* 32 Bits Bias */
static inline void __attribute__((always_inline)) KerConvDWRedAct1x1Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1Stride1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x1Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1Stride2_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x1StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=1, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) KerConv1x1StrideS_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct3x1Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=3, Sx=1;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x1Stride1x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x1BorderRedActStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct3x1Stride2x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=3, Sx=2;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x1Stride2x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x1BorderRedActStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x3Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x3Stride1x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x3BorderRedActStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x3Stride1x2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=3, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x3Stride1x2_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x3BorderRedActStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct3x3Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3Stride1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStride1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct3x3Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3Stride2_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStride2_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct3x3StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=3, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv3x3StrideS_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv3x3BorderRedActStrideS_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct5x1Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=5, Sx=1;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x1Stride1x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x1BorderRedActStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct5x1Stride2x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=5, Sx=2;
	unsigned int FSy=1, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x1Stride2x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x1BorderRedActStrideNx1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x5Stride1x1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x5Stride1x1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x5BorderRedActStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct1x5Stride1x2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=1, Sx=1;
	unsigned int FSy=5, Sy=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv1x5Stride1x2_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv1x5BorderRedActStride1xN_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct5x5Stride1B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=1;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5Stride1_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStride1_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct5x5Stride2B32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=2;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5Stride2_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStride2_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct5x5StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=5, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv5x5StrideS_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConv5x5BorderRedActStrideS_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedAct7x7StrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=7, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConv7x7StrideS_Body_RedAct_SQ8(In, Out, Filter, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxNStrideS_BorderRedAct_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedActNxNStrideSB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FS=Arg->N, S=Arg->S;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FS+PadIn[0]+PadIn[1])/S + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FS, PadIn[0], S)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FS, PadIn[0], S));
	int Ho = (Arg->UsedH-FS+PadIn[2]+PadIn[3])/S + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FS, PadIn[2], S)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FS, PadIn[2], S));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxNStrideS_Body_RedAct_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxNStrideS_BorderRedAct_SQ8(In, Out, Filter, FS, FS, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, S, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedActNxMStrideSxSyB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-FSx+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput(FSx, PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, FSx, PadIn[0], Sx));
	int Ho = (Arg->UsedH-FSy+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput(FSy, PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, FSy, PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxMStrideSxSy_Body_RedAct_SQ8(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxMStrideSxSy_BorderRedAct_SQ8(In, Out, Filter, FSx, FSy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

static inline void __attribute__((always_inline)) KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(
	KerConvRedAct_SQ8_T *Arg,
	CNN_ActivationOper_T Activation
	)

{
	unsigned int FSx=Arg->N, Sx=Arg->S;
	unsigned int FSy=Arg->Ny, Sy=Arg->Sy;
	int Dx=Arg->D, Dy=Arg->Dy;
	signed char * __restrict__ In = Arg->In;
	unsigned int W = Arg->W;
	unsigned int H = Arg->H;
	signed char * __restrict__ Filter = Arg->Filter;
	unsigned int NormBias = Arg->NormBias;
	int B = AT_LSHIFT(((int *__restrict__) Arg->Bias)[0], NormBias);
	signed char * __restrict__ Out = (signed char * __restrict__) Arg->Out;
	unsigned char Scale  = ((unsigned char * __restrict__) Arg->Scale)[0];
	unsigned char ScaleN = ((unsigned char * __restrict__) Arg->ScaleN)[0];
	unsigned char * __restrict__ Infos  = (unsigned char * __restrict__) Arg->Infos;
	
	unsigned int ActScale = ((unsigned char *)Infos)[AT_INF_ACTSCALE], ActScaleN = ((unsigned char *)Infos)[AT_INF_ACTSCALEN];
	int A0 = ((unsigned char *)Infos)[AT_INF_A0]; int B0 = ((unsigned char *)Infos)[AT_INF_B0]; int C0 = ((unsigned char *)Infos)[AT_INF_C0];
	v4s PadIn = Arg->Pad;
	int Wo = (Arg->UsedW-(Dx*(FSx-1)+1)+PadIn[0]+PadIn[1])/Sx + 1;
	int Wo_F = Min(Wo, FirstDefinedOutput((Dx*(FSx-1)+1), PadIn[0], Sx)), Wo_L = Max(Wo_F, LastDefinedOutput(Arg->UsedW, (Dx*(FSx-1)+1), PadIn[0], Sx));
	int Ho = (Arg->UsedH-(Dy*(FSy-1)+1)+PadIn[2]+PadIn[3])/Sy + 1;
	int Ho_F = Min(Ho, FirstDefinedOutput((Dy*(FSy-1)+1), PadIn[2], Sy)), Ho_L = Max(Ho_F, LastDefinedOutput(Arg->UsedH, (Dy*(FSy-1)+1), PadIn[2], Sy));
	unsigned int CoreId = gap_coreid();
	v4s PadOrg = PadIn;
	unsigned int Chunk, First, Last;

	if (Arg->Orientation) { // Horizontal
		Chunk = ChunkSize(Wo); First = Chunk*CoreId; Last = Min(First+Chunk, Wo);
		PadIn[0] *= (First==0); PadIn[1] *= (Last==Wo);
		Wo_F = Max(First, Wo_F); Wo_L = Min(Last, Wo_L);
	} else {
		Chunk = ChunkSize(Ho); First = Chunk*CoreId; Last = Min(First+Chunk, Ho);
		PadIn[2] *= (First==0); PadIn[3] *= (Last==Ho);
		Ho_F = Max(First, Ho_F); Ho_L = Min(Last, Ho_L);
	}
	if (First<Last) {
		KerConvNxMDxDyStrideSxSy_Body_RedAct_SQ8(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
		if ((int)PadIn) KerConvNxMDxDyStrideSxSy_BorderRedAct_SQ8(In, Out, Filter, FSx, FSy, Dx, Dy, W, H, Wo, Wo_F, Wo_L, Ho, Ho_F, Ho_L, Sx, Sy, PadIn, PadOrg, B, Scale, ScaleN, Activation, ActScale, ActScaleN, A0, B0, C0);
	}
	gap_waitbarrier(0);
}

/* */
void KerParConvDWRedAct1x1Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x1Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x1Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x1Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x1Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x1Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x1Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x1Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x1StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x1StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x1StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x1StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct3x1Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct3x1Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct3x1Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct3x1Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct3x1Stride2x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct3x1Stride2x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct3x1Stride2x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct3x1Stride2x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x3Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x3Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x3Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x3Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x3Stride1x2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x3Stride1x2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x3Stride1x2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x3Stride1x2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct3x3Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct3x3Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct3x3Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct3x3Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct3x3Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct3x3Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct3x3Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct3x3Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct3x3StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct3x3StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct3x3StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct3x3StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct5x1Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct5x1Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct5x1Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct5x1Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct5x1Stride2x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct5x1Stride2x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct5x1Stride2x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct5x1Stride2x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x5Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x5Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x5Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x5Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct1x5Stride1x2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct1x5Stride1x2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct1x5Stride1x2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct1x5Stride1x2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct5x5Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct5x5Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct5x5Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct5x5Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct5x5Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct5x5Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct5x5Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct5x5Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct5x5StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct5x5StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct5x5StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct5x5StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedAct7x7StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedAct7x7StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedAct7x7StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedAct7x7StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedActNxNStrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedActNxNStrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedActNxNStrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedActNxNStrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedActNxMStrideSxSyB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedActNxMStrideSxSyB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedActNxMStrideSxSyB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedActNxMStrideSxSyB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_NONE);
}

void KerParConvDWRedActNxMDxDyStrideSxSyB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELU);
}

void KerParConvDWRedActNxMDxDyStrideSxSyB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELUN);
}

void KerParConvDWRedActNxMDxDyStrideSxSyB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg){
	KerParConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELUM);
}


/********************************************/
/**************** ParFEAT = 0 ***************/


/* */
void KerConvDWRedAct1x1Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x1Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x1Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x1Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x1Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x1Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x1Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x1Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x1StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x1StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x1StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x1StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x1StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct3x1Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct3x1Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct3x1Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct3x1Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct3x1Stride2x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct3x1Stride2x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct3x1Stride2x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct3x1Stride2x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x1Stride2x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x3Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x3Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x3Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x3Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x3Stride1x2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x3Stride1x2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x3Stride1x2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x3Stride1x2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x3Stride1x2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct3x3Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct3x3Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct3x3Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct3x3Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct3x3Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct3x3Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct3x3Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct3x3Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct3x3StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct3x3StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct3x3StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct3x3StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct3x3StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct5x1Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct5x1Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct5x1Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct5x1Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct5x1Stride2x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct5x1Stride2x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct5x1Stride2x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct5x1Stride2x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x1Stride2x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x5Stride1x1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x5Stride1x1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x5Stride1x1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x5Stride1x1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct1x5Stride1x2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct1x5Stride1x2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct1x5Stride1x2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct1x5Stride1x2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct1x5Stride1x2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct5x5Stride1B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct5x5Stride1B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct5x5Stride1B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct5x5Stride1B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride1B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct5x5Stride2B32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct5x5Stride2B32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct5x5Stride2B32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct5x5Stride2B32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5Stride2B32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct5x5StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct5x5StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct5x5StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct5x5StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct5x5StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedAct7x7StrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedAct7x7StrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedAct7x7StrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedAct7x7StrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedAct7x7StrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedActNxNStrideSB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedActNxNStrideSB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedActNxNStrideSB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedActNxNStrideSB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxNStrideSB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedActNxMStrideSxSyB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedActNxMStrideSxSyB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedActNxMStrideSxSyB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedActNxMStrideSxSyB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMStrideSxSyB32_SQ8_act(Arg, ACT_RELUM);
}

/* */
void KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_NONE);
}

void KerConvDWRedActNxMDxDyStrideSxSyB32_ReLU_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELU);
}

void KerConvDWRedActNxMDxDyStrideSxSyB32_ReLUN_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELUN);
}

void KerConvDWRedActNxMDxDyStrideSxSyB32_ReLUM_SQ8(KerConvRedAct_SQ8_T *Arg) {
	KerConvDWRedActNxMDxDyStrideSxSyB32_SQ8_act(Arg, ACT_RELUM);
}


#pragma GCC diagnostic pop
