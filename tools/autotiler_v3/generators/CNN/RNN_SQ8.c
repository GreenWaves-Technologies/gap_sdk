#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels_SQ8.h"

#define Minu(a, b)            (( ((unsigned int)a)<((unsigned int)b) )?((unsigned int)a):((unsigned int)b) )

#ifdef __pulp__
#define Abs(a)                  __builtin_pulp_abs((a))
#define Min(a, b)              __builtin_pulp_minsi((a), (b))
#define Max(a, b)              __builtin_pulp_maxsi((a), (b))
#else
#define Abs(a)                  (((int)(a)<0)?(-(a)):(a))
#define Min(a, b)              (((a)<(b))?(a):(b))
#define Max(a, b)              (((a)>(b))?(a):(b))
#endif

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

void RNN_ParKerB32_SQ8(KerRNN_SQ8_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos: Forget: HSigmoid:       Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN          7

		if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
	*/
	int PerChannelQuant = 0;
        signed char *__restrict__ StateInOut = Arg->StateInOut;
        signed char *__restrict__ Xin= Arg->Xin;
        signed char *__restrict__ State = Arg->State;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        signed char *__restrict__ Wf = Arg->Wf;
        int * __restrict__ Bf = Arg->Bf;
        signed char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, DimState, CoreId);
		else Copy(State, StateInOut, DimState, CoreId);
		gap_waitbarrier(0);
	}
	if (Xin) {
		Copy(State+DimState, Xin, DimIn, CoreId);
		gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/4;
	v4s *Vin = (v4s *) State;
	v4s *Vf  = (v4s *) (Wf+Nin*First);
	for (int o=First; o<Last; o++) {
		int Of = Bf[o];
		for (int i=0; i<Nin/8; i++) {
			v4s X0 = Vin[2*i], X1 = Vin[2*i+1];
			v4s Y0 = Vf[2*i], Y1 = Vf[2*i+1];
			Of = gap_sumdotp4(X0, Y0, Of);
			Of = gap_sumdotp4(X1, Y1, Of);
		}
		if (Nin & 0x4) Of = gap_sumdotp4(Vin[Nin/4-1], Vf[Nin/4-1], Of);
		for (int i=(Nin/4)*4; i<Nin; i++) {
			int X = ((char *)Vin)[Off+i];
			Of += X * ((char *)Vf)[Off+i];
		}

		/* Of = HTanh(Scaled(Of)) */
		Of = gap_clip(AT_SCALE(Of, ((unsigned char *)Infos)[RNN_F_SCALE], ((unsigned char *)Infos)[RNN_F_SCALEN]), 7);
		Of = Max(Infos[RNN_F_A0], Min(Infos[RNN_F_B0], Of));

		if (StateInOut) StateInOut[o] = Of;
		if (Hout) Hout[o] = Of;

		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
		Vf = (v4s *) ((char *)Vf + NS);
	}
	gap_waitbarrier(0);
}

void LSTM_ParKerB32_SQ8(KerLSTM_SQ8_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
			Forget:	HSigmoid:	Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN		7
			Input:	HSigmoid:	Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN		7
			Gate:	HTanh:		Scale, ScaleN, A0, B0 					4
			Output:	HSigmoid:	Scale, ScaleN, A0, B0, C0, ActScale, ActScaleN		7
			Cout:	HTanh:		A0, B0, Scale, ScaleN					4

		In total: 29	LSTM_CELL_INFOS
		if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
	*/
	int PerChannelQuant = 0;
        signed char *__restrict__ StateInOut = Arg->StateInOut;
        signed char *__restrict__ State = Arg->State;
        signed char *__restrict__ Xin= Arg->Xin;
        unsigned short int DimState = Arg->DimState;
        unsigned short int DimIn = Arg->DimIn;
        signed char *__restrict__ Wf = Arg->Wf;
        int * __restrict__ Bf = Arg->Bf;
        signed char *__restrict__ Wi = Arg->Wi;
        int * __restrict__ Bi = Arg->Bi;
        signed char *__restrict__ Wg = Arg->Wg;
        int * __restrict__ Bg = Arg->Bg;
        signed char *__restrict__ Wo = Arg->Wo;
        int * __restrict__ Bo = Arg->Bo;
        signed char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, 2*DimState, CoreId);
		else Copy(State, StateInOut, 2*DimState, CoreId);
		gap_waitbarrier(0);
	}
        if (Xin) {
                Copy(State+2*DimState, Xin, DimIn, CoreId);
                gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/4;

	v4s *Vin = (v4s *) (State+DimState);
	v4s *Vf  = (v4s *) (Wf+Nin*First);
	v4s *Vi  = (v4s *) (Wi+Nin*First);
	v4s *Vg  = (v4s *) (Wg+Nin*First);
	v4s *Vo  = (v4s *) (Wo+Nin*First);
	for (int o=First; o<Last; o++) {
		int Of = Bf[o], Oi = Bi[o], Og = Bg[o], Oo = Bo[o];
		for (int i=0; i<Nin/4; i++) {
			v4s X = Vin[i];
			Of = gap_sumdotp4(X, Vf[i], Of);
			Oi = gap_sumdotp4(X, Vi[i], Oi);
			Og = gap_sumdotp4(X, Vg[i], Og);
			Oo = gap_sumdotp4(X, Vo[i], Oo);
		}
		for (int i=(Nin/4)*4; i<Nin; i++) {
			int X = ((char *)Vin)[Off+i];
			Of += X * ((char *)Vf)[Off+i];
			Oi += X * ((char *)Vi)[Off+i];
			Og += X * ((char *)Vg)[Off+i];
			Oo += X * ((char *)Vo)[Off+i];
		}

		/* Of = HSigmoid(Scaled(Of)) */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[LSTM_F_SCALE], ((unsigned char *)Infos)[LSTM_F_SCALEN]);
		Of = AT_NORM(Max(0, Min(*((short *)&Infos[LSTM_INT_A0]), Of + *((short *)&Infos[LSTM_INT_B0]))) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Oi = HSigmoid(Scaled(Oi)) */
		Oi = AT_SCALE(Oi, ((unsigned char *)Infos)[LSTM_I_SCALE], ((unsigned char *)Infos)[LSTM_I_SCALEN]);
		Oi = AT_NORM(Max(0, Min(*((short *)&Infos[LSTM_INT_A0]), Oi + *((short *)&Infos[LSTM_INT_B0]))) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Og = HTanh(Scaled(Og)) */
		Og = AT_SCALE(Og, ((unsigned char *)Infos)[LSTM_G_SCALE], ((unsigned char *)Infos)[LSTM_G_SCALEN]);

		int one = 1 << ((unsigned char *)Infos)[LSTM_INT_Q];
		Og = Max(-one, Min(one, Og));

		/* Oo = HSigmoid(Scaled(Oo)) */
		Oo = AT_SCALE(Oo, ((unsigned char *)Infos)[LSTM_O_SCALE], ((unsigned char *)Infos)[LSTM_O_SCALEN]);
		Oo = AT_NORM(Max(0, Min(*((short *)&Infos[LSTM_INT_A0]), Oo + *((short *)&Infos[LSTM_INT_B0]))) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);
		
		int X1 = AT_SCALE(State[o], ((unsigned char *)Infos)[LSTM_CIN_SCALE], ((unsigned char *)Infos)[LSTM_CIN_SCALEN]);
		X1 = Of * X1 + Oi * Og;
		StateInOut[o] = gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]), 7);

		/* X1 = HTanh(X1) */
		one = one << Infos[LSTM_INT_Q];
		X1 = Max(-one, Min(one, X1));

		int X2 = gap_clip(AT_SCALE(Oo * X1, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]), 7);
		StateInOut[DimState+o] = X2;
		if (Hout) Hout[o] = X2;

		Vf = (v4s *) ((char *)Vf + NS);
		Vi = (v4s *) ((char *)Vi + NS);
		Vg = (v4s *) ((char *)Vg + NS);
		Vo = (v4s *) ((char *)Vo + NS);
		if (PerChannelQuant) Infos += LSTM_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void GRU_ParKerB32_SQ8(KerGRU_SQ8_T *Arg)

{

	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
		if (PerChannelQuant) Infos group for each output elemt (Nout) else one group for all out
	*/
	int PerChannelQuant = 0;
	signed char *__restrict__ StateInOut = Arg->StateInOut;
	signed char *__restrict__ State = Arg->State;
        signed char *__restrict__ Xin= Arg->Xin;
        unsigned int DimState = Arg->DimState;
        unsigned int DimIn = Arg->DimIn;
        signed char *__restrict__ Wr = Arg->Wr;
        int * __restrict__ Br = Arg->Br;
        signed char *__restrict__ Wz = Arg->Wz;
        int * __restrict__ Bz = Arg->Bz;
        signed char *__restrict__ Wh = Arg->Wh;
        int * __restrict__ Bh = Arg->Bh;
        signed char *__restrict__ Sbuff = Arg->Sbuff;
        signed char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, DimState, CoreId);
		else Copy(State, StateInOut, DimState, CoreId);
		gap_waitbarrier(0);
	}
        if (Xin) {
                Copy(State+DimState, Xin, DimIn, CoreId);
                gap_waitbarrier(0);
	} else Nin -= DimIn;
	int Off = Nin/4;
	/* Fisrt eval Sbuff<DimState> = HSigmoid(NLP(StateInOut<DimState+DimIn>, Wr<DimState+DimIn>, Br<DimState>)) !*! StateInOut<DimState>
	   !*! is Hadammard product (elemt wise product)
	   Actual dimension of Sbuff is <DimState+DimIn>
	   Previous step has copied StateInOut into the DimIn last items of Sbuff
	*/
	v4s *Vin = (v4s *) State;
	v4s *Vr  = (v4s *) (Wr+Nin*First);
	for (int o=First; o<Last; o++) {
		int Or = Br[o];
		for (int i=0; i<Nin/8; i++) {
			v4s X0 = Vin[2*i], X1 = Vin[2*i+1];
			v4s Y0 = Vr[2*i], Y1 = Vr[2*i+1];
			Or = gap_sumdotp4(X0, Y0, Or);
			Or = gap_sumdotp4(X1, Y1, Or);
		}
		if (Nin & 0x4) Or= gap_sumdotp4(Vin[Nin/4], Vr[Nin/4], Or);
		for (int i=(Nin/4)*4; i<Nin; i++) {
			int X = ((char *)Vin)[Off+i];
			Or += X * ((char *)Vr)[Off+i];
		}
		/* Or = HSigmoid(Scaled(Or)) !*! StateInOut[o] */
		Or = gap_clip(AT_SCALE(Or, ((unsigned char *)Infos)[GRU_R_SCALE], ((unsigned char *)Infos)[GRU_R_SCALEN]), 7);
		Or = AT_SCALE(Max(0, Min(Infos[GRU_R_A0], Or + Infos[GRU_R_B0])) * Infos[GRU_R_C0] * StateInOut[o], ((unsigned char *)Infos)[GRU_R_ASCALE], ((unsigned char *)Infos)[GRU_R_ASCALEN]);
		Sbuff[o] = Or;
		Vr = (v4s *) ((char *)Vr + NS);
		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
	}
	gap_waitbarrier(0);

	/* Now eval the rest:
	 	Zt  = HSigmoid(NLP(StateInOut<DimState+DimIn>, Wz<DimState+DimIn>, Bz<DimState>))
		Ht' = HTanh(NLP(Sbuff<DimState+DimIn>, Wh<DimState+DimIn>, Bh<DimState>))
		Ht  = (1-Zt) !*! Ht'  +  Zt !*! Ht'
	*/
	if (PerChannelQuant) Infos = Arg->Infos;
	Vr = (v4s *) Sbuff;
	v4s *Vz = (v4s *) (Wz+Nin*First);
	v4s *Vh = (v4s *) (Wh+Nin*First);
	for (int o=First; o<Last; o++) {
		int Oz = Bz[o], Oh = Bh[o];
		for (int i=0; i<Nin/4; i++) {
			v4s X0 = Vin[i], Z0 = Vz[i], V0 = Vh[i], R0 = Vr[i];
			Oz = gap_sumdotp4(X0, Z0, Oz);
			Oh = gap_sumdotp4(R0, V0, Oh);
		}
		for (int i=(Nin/4)*4; i<Nin; i++) {
			int X = ((char *)Vin)[Off+i], R = ((char *)Vr)[Off+i];
			Oz += X * ((char *)Vz)[Off+i];
			Oh += R * ((char *)Vh)[Off+i];
		}
		/* Oz = HSigmoid(Scaled(Oz)) */
		Oz = gap_clip(AT_SCALE(Oz, ((unsigned char *)Infos)[GRU_Z_SCALE], ((unsigned char *)Infos)[GRU_Z_SCALEN]), 7);
		Oz = AT_SCALE(Max(0, Min(Infos[GRU_Z_A0], Oz + Infos[GRU_Z_B0])) * Infos[GRU_Z_C0], ((unsigned char *)Infos)[GRU_Z_ASCALE], ((unsigned char *)Infos)[GRU_Z_ASCALEN]);

		/* Oht = HTanh(Scaled(Oh)) */
		int Oht = gap_clip(AT_SCALE(Oh, ((unsigned char *)Infos)[GRU_HT_SCALE], ((unsigned char *)Infos)[GRU_HT_SCALEN]), 7);
		Oht = Max(Infos[GRU_HT_A0], Min(Infos[GRU_HT_B0], Oht));

		/* Oh = Scaled((1-Oz)*StateInOut[o] + Oz*Oht) */
		Oh = AT_SCALE((Infos[GRU_H_A0] - Oz)*StateInOut[o] + Oz*Oht, ((unsigned char *)Infos)[GRU_H_SCALE], ((unsigned char *)Infos)[GRU_H_SCALEN]);

		StateInOut[o] = Oh;
		if (Hout) Hout[o] = Oh;

		Vz = (v4s *) ((char *)Vz + NS);
		Vh = (v4s *) ((char *)Vh + NS);
		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

