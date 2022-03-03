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
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#include <stdio.h>
#include "CNN_BasicKernels_fp16.h"

static int CoreCountDynamic = 1;
static int ActiveCore = gap_ncore();
#include "FastFloatApprox16.h"

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

#define B_CLR(x, bits)	((x)&(~((1<<(bits))-1)))

static inline void Copy(char *__restrict__ To, char *__restrict__ From, unsigned int Size, unsigned int CoreId)

{
        unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
        unsigned int Iter = Max(0, Last-First);

	int *pFrom = (int *) (From+First), *pTo = (int *) (To+First);
        for (int i=0; i<Iter/8; i++) {
                int V0 = pFrom[2*i], V1 = pFrom[2*i+1];
		pTo[2*i] = V0; pTo[2*i+1] = V1;
	}
	if (Iter & 0x4) *((int *) (To + First + B_CLR(Iter, 3))) = *((int *) (From + First + B_CLR(Iter, 3)));
	if (Iter & 0x2) *((short int *) (To + First + B_CLR(Iter, 2))) = *((short int *) (From + First + B_CLR(Iter, 2)));
	if (Iter & 0x1) *((signed char *) (To + First + Iter - 1)) = *((signed char *) (From + First + Iter - 1));
}

static inline void Zero(char *__restrict__ To, unsigned int Size, unsigned int CoreId)

{
        unsigned int Chunk = ChunkSize(Size), First = Chunk*CoreId, Last = Min(First+Chunk, Size);
        int Iter = Max(0, Last-First);

	int *pTo = (int *) (To+First);
        for (int i=0; i<Iter/8; i++) {
		pTo[2*i] = 0; pTo[2*i+1] = 0;
	}
	if (Iter & 0x4) *((int *) (To + First + B_CLR(Iter, 3))) = 0;
	if (Iter & 0x2) *((short int *) (To + First + B_CLR(Iter, 2))) = 0;
	if (Iter & 0x1) *((signed char *) (To + First + Iter - 1)) = 0;
}

void RNN_ParKer_fp16(KerRNN_fp16_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0
	*/
	int PerChannelQuant = 0;
        F16 *__restrict__ StateInOut = Arg->StateInOut;
        F16 *__restrict__ Xin= Arg->Xin;
        F16 *__restrict__ State = Arg->State;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        F16 *__restrict__ Wf = Arg->Wf;
        F16 * __restrict__ Bf = Arg->Bf;
        F16 *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);
	unsigned int OutBase = Arg->OutBase;
	// First += Arg->OutBase; Last += Arg->OutBase;

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero((char *__restrict__) State, 2*DimState, CoreId);
		else Copy((char *__restrict__) State, (char *__restrict__) StateInOut, 2*DimState, CoreId);
		gap_waitbarrier(0);
	}
	if (Xin) {
		Copy((char *__restrict__) (State+DimState), (char *__restrict__) Xin, 2*DimIn, CoreId);
		gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/2;
	F16V *Vin = (F16V *) State;
	// F16V *Vf  = (F16V *) (Wf+Nin*First);
	F16V *Vf  = (F16V *) (Wf+Nin*First);
	for (int o=First; o<Last; o++) {
		F16V Ofv = (F16V) {Bf[o], 0.0};
		for (int i=0; i<Nin/4; i++) {
			F16V X0 = Vin[2*i], X1 = Vin[2*i+1];
			F16V Y0 = Vf[2*i], Y1 = Vf[2*i+1];
			Ofv += X0 * Y0;
			Ofv += X1 * Y1;
		}
		if (Nin & 0x2) Ofv += Vin[Nin/2-1] * Vf[Nin/2-1];
		F16 Of = Ofv[0]+Ofv[1];
		if (Nin&0x1) Of += ((F16 *)Vin)[Nin-1] * ((F16 *)Vf)[Nin-1];

		/* Of = HTanh(Scaled(Of)) */
		Of = FastTanhF16(Of);

		if (StateInOut) StateInOut[o+OutBase] = Of;
		if (Hout) Hout[o] = Of;

		Vf = (F16V *) ((F16 *)Vf + NS);
	}
	gap_waitbarrier(0);
}

void LSTM_ParKer_fp16(KerLSTM_fp16_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

	*/
	int PerChannelQuant = 0;
        F16 *__restrict__ StateInOut = Arg->StateInOut;
        F16 *__restrict__ State = Arg->State;
        F16 *__restrict__ Xin= Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        F16 *__restrict__ Wf = Arg->Wf;
        F16 * __restrict__ Bf = Arg->Bf;
        F16 *__restrict__ Wi = Arg->Wi;
        F16 * __restrict__ Bi = Arg->Bi;
        F16 *__restrict__ Wg = Arg->Wg;
        F16 * __restrict__ Bg = Arg->Bg;
        F16 *__restrict__ Wo = Arg->Wo;
        F16 * __restrict__ Bo = Arg->Bo;
        F16 *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);
	unsigned int OutBase = Arg->OutBase;
	// First += Arg->OutBase; Last += Arg->OutBase;

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero((char *__restrict__) State, 2*2*DimState, CoreId);
		else Copy((char *__restrict__) State, (char *__restrict__) StateInOut, 2*2*DimState, CoreId);
		gap_waitbarrier(0);
	}
        if (Xin) {
                Copy((char *__restrict__) (State+2*DimState), (char *__restrict__) Xin, 2*DimIn, CoreId);
                gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/2;

	F16V *Vin = (F16V *) (State+DimState);
	F16V *Vf  = (F16V *) (Wf+Nin*First);
	F16V *Vi  = (F16V *) (Wi+Nin*First);
	F16V *Vg  = (F16V *) (Wg+Nin*First);
	F16V *Vo  = (F16V *) (Wo+Nin*First);
	for (int o=First; o<Last; o++) {
		F16V Ofv = (F16V){Bf[o], 0.0}, Oiv = (F16V){Bi[o], 0.0}, Ogv = (F16V){Bg[o], 0.0}, Oov = (F16V){Bo[o], 0.0};
		for (int i=0; i<Nin/2; i++) {
			F16V X = Vin[i];
			Ofv += X * Vf[i];
			Oiv += X * Vi[i];
			Ogv += X * Vg[i];
			Oov += X * Vo[i];
		}
		F16 Of = Ofv[0] + Ofv[1], Oi = Oiv[0] + Oiv[1], Og = Ogv[0] + Ogv[1], Oo = Oov[0] + Oov[1];
		if (Nin&0x1) {
			F16 X = ((F16 *)Vin)[Nin-1];
			Of += X * ((F16 *)Vf)[Nin-1];
			Oi += X * ((F16 *)Vi)[Nin-1];
			Og += X * ((F16 *)Vg)[Nin-1];
			Oo += X * ((F16 *)Vo)[Nin-1];
		}

		/* Of = HSigmoid(Scaled(Of)) */
		Of = FastSigmoidF16(Of);

		/* Oi = HSigmoid(Scaled(Oi)) */
		Oi = FastSigmoidF16(Oi);

		/* Og = HTanh(Scaled(Og)) */
		Og = FastTanhF16(Og);

        	/* Oo = HSigmoid(Scaled(Oo)) */
		Oo = FastSigmoidF16(Oo);
        
        	F16 X1 = Of * State[o+OutBase] + Oi * Og;
        	if (StateInOut) StateInOut[o+OutBase] = X1;

		/* X1 = HTanh(X1) */
		X1 = FastTanhF16(X1);

        	F16 X2 = Oo * X1;
        	if (StateInOut) StateInOut[DimState+o+OutBase] = X2;
		if (Hout) Hout[o] = X2;

		Vf = (F16V *) ((F16 *)Vf + NS);
		Vi = (F16V *) ((F16 *)Vi + NS);
		Vg = (F16V *) ((F16 *)Vg + NS);
		Vo = (F16V *) ((F16 *)Vo + NS);
	}
	gap_waitbarrier(0);
}

void GRU_ParKer_fp16(KerGRU_fp16_T *Arg)

{

	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

	*/
	F16 *__restrict__ StateInOut = Arg->StateInOut;
	F16 *__restrict__ State = Arg->State;
        F16 *__restrict__ Xin= Arg->Xin;
        unsigned int DimState = Arg->DimState;
        unsigned int DimIn = Arg->DimIn;
        F16 *__restrict__ Wr = Arg->Wr;
        F16 * __restrict__ Br = Arg->Br;
        F16 *__restrict__ Wz = Arg->Wz;
        F16 * __restrict__ Bz = Arg->Bz;
        F16 *__restrict__ Wh = Arg->Wh;
        F16 * __restrict__ Bwh = Arg->Bwh;
        F16 * __restrict__ Brh = Arg->Brh;
        F16 *__restrict__ Sbuff = Arg->Sbuff;
        F16 *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);
	unsigned int OutBase = Arg->OutBase;
	// First += Arg->OutBase; Last += Arg->OutBase;

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero((char *__restrict__) State, 2*DimState, CoreId);
		else Copy((char *__restrict__) State, (char *__restrict__) StateInOut, 2*DimState, CoreId);
		gap_waitbarrier(0);
	}
        if (Xin) {
                Copy((char *__restrict__) (State+DimState), (char *__restrict__) Xin, 2*DimIn, CoreId);
                gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/2;
	/* Fisrt eval Sbuff<DimState> = HSigmoid(NLP(StateInOut<DimState+DimIn>, Wr<DimState+DimIn>, Br<DimState>)) !*! StateInOut<DimState>
	   !*! is Hadammard product (elemt wise product)
	   Actual dimension of Sbuff is <DimState+DimIn>
	   Previous step has copied StateInOut into the DimIn last items of Sbuff
	*/
	F16V *VState = (F16V *) State;
	F16V *Vin    = (F16V *) Xin;
	F16V *Vr     = (F16V *) (Wr+Nin*First);
	F16V *Vz     = (F16V *) (Wz+Nin*First);
	F16V *Vh     = (F16V *) (Wh+Nin*First);
	F16 Or, Oh, Oz;
	F16 Or_in = 0.0, Oh_in = 0.0, Oz_in = 0.0;
	for (int o=First; o<Last; o++) {
		/* MatMul(State, Wr) */
		F16V Orv = (F16V) {Br[o], 0.0};
		F16V Ozv = (F16V) {Bz[o], 0.0};
		F16V Ohv = (F16V) {Brh[o], 0.0};
		for (int i=0; i<DimState/4; i++) {
			F16V X0 = VState[2*i], X1 = VState[2*i+1];
			F16V R0 = Vr[2*i], R1 = Vr[2*i+1];
			F16V Z0 = Vz[2*i], Z1 = Vz[2*i+1];
			F16V H0 = Vh[2*i], H1 = Vh[2*i+1];
			Orv += X0 * R0; Orv += X1 * R1;
			Ozv += X0 * Z0; Ozv += X1 * Z1;
			Ohv += X0 * H0; Ohv += X1 * H1;
		}
		if (DimState & 0x2) {
			Orv += VState[DimState/2] * Vr[DimState/2];
			Ozv += VState[DimState/2] * Vz[DimState/2];
			Ohv += VState[DimState/2] * Vh[DimState/2];
		}
		Or = Orv[0]+Orv[1];
		Oz = Ozv[0]+Ozv[1];
		Oh = Ohv[0]+Ohv[1];
		if (DimState&0x1) {
			Or += ((F16 *)VState)[DimState-1] * ((F16 *)Vr)[DimState-1];
			Oz += ((F16 *)VState)[DimState-1] * ((F16 *)Vz)[DimState-1];
			Oh += ((F16 *)VState)[DimState-1] * ((F16 *)Vh)[DimState-1];
		}
		/* MatMul(In, Wi) */
		F16V Orv_in = (F16V) {0.0, 0.0};
		F16V Ozv_in = (F16V) {0.0, 0.0};
		F16V Ohv_in = (F16V) {Bwh[o], 0.0};
		Vr = (F16V *) ((F16 *)Vr + DimState);
		Vz = (F16V *) ((F16 *)Vz + DimState);
		Vh = (F16V *) ((F16 *)Vh + DimState);
		if (Xin) {
			for (int i=0; i<DimIn/2; i++) {
				F16V X0 = Vin[i];
				F16V R0 = Vr[i];
				F16V Z0 = Vz[i];
				F16V H0 = Vh[i];
				Orv_in += X0 * R0;
				Ozv_in += X0 * Z0;
				Ohv_in += X0 * H0;
			}
			Or_in = Orv_in[0]+Orv_in[1];
			Oz_in = Ozv_in[0]+Ozv_in[1];
			Oh_in = Ohv_in[0]+Ohv_in[1];
			if (DimIn&0x1) {
				Or_in += ((F16 *)Vin)[DimIn-1] * ((F16 *)Vr)[DimIn-1];
				Oz_in += ((F16 *)Vin)[DimIn-1] * ((F16 *)Vz)[DimIn-1];
				Oh_in += ((F16 *)Vin)[DimIn-1] * ((F16 *)Vh)[DimIn-1];
			}
			Or += Or_in;
			Oz += Oz_in;
			Vr = (F16V *) ((F16 *)Vr + DimIn);
			Vz = (F16V *) ((F16 *)Vz + DimIn);
			Vh = (F16V *) ((F16 *)Vh + DimIn);
		}

		/* Or = HSigmoid(Scaled(Or)) !*! StateInOut[o] */
		Or = FastSigmoidF16(Or);

		/* Or = HSigmoid(Scaled(Or)) !*! StateInOut[o] */
		Oz = FastSigmoidF16(Oz);

		Oh = FastTanhF16(Oh*Or + Oh_in);

		/* Oht = HTanh(Oh) */
		//F16 Oht = Fast_Tanh_fp16(Oh);

		/* Oh = (1-Oz)*StateInOut[o] + Oz*Oht */
		Oh = ((F16) 1.0 - Oz) * Oh + Oz*StateInOut[o+OutBase];

		if (StateInOut) StateInOut[OutBase+o] = Oh;
		if (Hout)	Hout[o] = Oh;
	}
	gap_waitbarrier(0);
}


#if 0
void GRU_ParKer_fp16(KerGRU_fp16_T *Arg)

{

	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

	*/
	int PerChannelQuant = 0;
	F16 *__restrict__ StateInOut = Arg->StateInOut;
	F16 *__restrict__ State = Arg->State;
        F16 *__restrict__ Xin= Arg->Xin;
        unsigned int DimState = Arg->DimState;
        unsigned int DimIn = Arg->DimIn;
        F16 *__restrict__ Wr = Arg->Wr;
        F16 * __restrict__ Br = Arg->Br;
        F16 *__restrict__ Wz = Arg->Wz;
        F16 * __restrict__ Bz = Arg->Bz;
        F16 *__restrict__ Wh = Arg->Wh;
        F16 * __restrict__ Bh = Arg->Bh;
        F16 *__restrict__ Sbuff = Arg->Sbuff;
        F16 *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);
	unsigned int OutBase = Arg->OutBase;
	// First += Arg->OutBase; Last += Arg->OutBase;

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero((char *__restrict__) State, 2*DimState, CoreId);
		else Copy((char *__restrict__) State, (char *__restrict__) StateInOut, 2*DimState, CoreId);
		gap_waitbarrier(0);
	}
        if (Xin) {
                Copy((char *__restrict__) (State+DimState), (char *__restrict__) Xin, 2*DimIn, CoreId);
                gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/2;
	/* Fisrt eval Sbuff<DimState> = HSigmoid(NLP(StateInOut<DimState+DimIn>, Wr<DimState+DimIn>, Br<DimState>)) !*! StateInOut<DimState>
	   !*! is Hadammard product (elemt wise product)
	   Actual dimension of Sbuff is <DimState+DimIn>
	   Previous step has copied StateInOut into the DimIn last items of Sbuff
	*/
	F16V *Vin = (F16V *) State;
	F16V *Vr  = (F16V *) (Wr+Nin*First);
	for (int o=First; o<Last; o++) {
		F16V Orv = (F16V) {Br[o], 0.0};
		for (int i=0; i<Nin/4; i++) {
			F16V X0 = Vin[2*i], X1 = Vin[2*i+1];
			F16V Y0 = Vr[2*i], Y1 = Vr[2*i+1];
			Orv += X0 * Y0;
			Orv += X1 * Y1;
		}
		if (Nin & 0x4) Orv += Vin[Nin/2] * Vr[Nin/2];
		F16 Or = Orv[0]+Orv[1];
		if (Nin&0x1) Or += ((F16 *)Vin)[Nin-1] * ((F16 *)Vr)[Nin-1];

		/* Or = HSigmoid(Scaled(Or)) !*! StateInOut[o] */
		Or = Fast_Sigmoid_fp16(Or) * StateInOut[o+OutBase];

		Sbuff[o+OutBase] = Or;
		Vr = (F16V *) ((F16 *)Vr + NS);
	}
	gap_waitbarrier(0);

	/* Now eval the rest:
	 	Zt  = HSigmoid(NLP(StateInOut<DimState+DimIn>, Wz<DimState+DimIn>, Bz<DimState>))
		Ht' = HTanh(NLP(Sbuff<DimState+DimIn>, Wh<DimState+DimIn>, Bh<DimState>))
		Ht  = (1-Zt) !*! Ht'  +  Zt !*! Ht'
	*/
	Vr = (F16V *) (Sbuff+OutBase);
	F16V *Vz = (F16V *) (Wz+Nin*First);
	F16V *Vh = (F16V *) (Wh+Nin*First);
	for (int o=First; o<Last; o++) {
		F16V Ozv = (F16V) {Bz[o], 0.0}, Ohv = (F16V) {Bh[o], 0.0};
		for (int i=0; i<Nin/2; i++) {
			F16V X0 = Vin[i], Z0 = Vz[i], V0 = Vh[i], R0 = Vr[i];
			Ozv += X0 * Z0;
			Ohv += R0 * V0;
		}
		F16 Oz = Ozv[0]+Ozv[1];
		F16 Oh = Ohv[0]+Ohv[1];
		if (Nin&0x1) {
			F16 X = ((F16 *)Vin)[Nin-1], R = ((F16 *)Vr)[Nin-1];
			Oz += X * ((F16 *)Vz)[Nin-1];
			Oh += R * ((F16 *)Vh)[Nin-1];
		}
		/* Oz = HSigmoid(Oz) */
		Oz = Fast_Sigmoid_fp16(Oz);

		/* Oht = HTanh(Oh) */
		F16 Oht = Fast_Tanh_fp16(Oh);

		/* Oh = (1-Oz)*StateInOut[o] + Oz*Oht */
		Oh = ((F16) 1.0 - Oz) * StateInOut[o+OutBase] + Oz*Oht;

		if (StateInOut) StateInOut[o+OutBase] = Oh;
		if (Hout) Hout[o] = Oh;

		Vz = (F16V *) ((F16 *)Vz + NS);
		Vh = (F16V *) ((F16 *)Vh + NS);
	}
	gap_waitbarrier(0);
}
#endif

#pragma GCC diagnostic pop
