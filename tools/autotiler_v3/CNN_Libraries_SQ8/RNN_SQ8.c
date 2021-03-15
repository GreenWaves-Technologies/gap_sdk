#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <stdio.h>
#include <math.h>
#include "CNN_BasicKernels_SQ8.h"

#define NEAREST //Use nearest LUT element instead of linearly interpolate

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

unsigned short int SIGMOID_LUT_uint16[256] = {
    32768, 33451, 34133, 34813, 35493, 36169, 36843, 37513, 38180, 38841, 39498,
    40149, 40794, 41432, 42064, 42688, 43304, 43912, 44511, 45102, 45683, 46255,
    46817, 47369, 47911, 48443, 48964, 49475, 49975, 50464, 50942, 51409, 51865,
    52311, 52745, 53169, 53581, 53983, 54374, 54755, 55125, 55485, 55834, 56174,
    56503, 56823, 57133, 57433, 57724, 58007, 58280, 58544, 58800, 59048, 59288,
    59519, 59743, 59959, 60168, 60370, 60565, 60753, 60935, 61110, 61279, 61441,
    61599, 61750, 61896, 62036, 62172, 62302, 62428, 62549, 62666, 62778, 62886,
    62990, 63090, 63186, 63279, 63368, 63454, 63536, 63615, 63691, 63765, 63835,
    63903, 63968, 64030, 64090, 64148, 64204, 64257, 64308, 64357, 64405, 64450,
    64494, 64536, 64576, 64614, 64652, 64687, 64721, 64754, 64786, 64816, 64845,
    64873, 64900, 64926, 64950, 64974, 64997, 65019, 65039, 65060, 65079, 65097,
    65115, 65132, 65149, 65164, 65179, 65194, 65208, 65221, 65234, 65246, 65258,
    65269, 65280, 65291, 65301, 65310, 65319, 65328, 65337, 65345, 65352, 65360,
    65367, 65374, 65381, 65387, 65393, 65399, 65404, 65410, 65415, 65420, 65425,
    65429, 65433, 65438, 65442, 65445, 65449, 65453, 65456, 65459, 65462, 65465,
    65468, 65471, 65474, 65476, 65479, 65481, 65483, 65485, 65488, 65489, 65491,
    65493, 65495, 65497, 65498, 65500, 65501, 65503, 65504, 65505, 65507, 65508,
    65509, 65510, 65511, 65512, 65513, 65514, 65515, 65516, 65517, 65517, 65518,
    65519, 65520, 65520, 65521, 65522, 65522, 65523, 65523, 65524, 65524, 65525,
    65525, 65526, 65526, 65526, 65527, 65527, 65528, 65528, 65528, 65529, 65529,
    65529, 65529, 65530, 65530, 65530, 65530, 65531, 65531, 65531, 65531, 65531,
    65532, 65532, 65532, 65532, 65532, 65532, 65533, 65533, 65533, 65533, 65533,
    65533, 65533, 65533, 65534, 65534, 65534, 65534, 65534, 65534, 65534, 65534,
    65534, 65534, 65535};


int Sigmoid(int x){
	/* Input x: Q12 [-8:8] range

	   Output y = sig(x) -> Q15
	*/
#ifndef NEAREST
	int result, ua, ub, ut;
	int abs_x = (Abs(x) * 3) >> 9; // * 3/4 (*3 >>2) and clip it to [0:255] (>>7) ot be an index of the LUT
	if (abs_x > 255) {
		result = 0x1FFFC00; // result = 1 in Q25
	} else {
		ua = SIGMOID_LUT_uint16[abs_x];
		ub = SIGMOID_LUT_uint16[abs_x+1];
		ut = abs_x & 0xFF;
		result = (ua << 9) + ut * (ub-ua); // LUT in Q16 * ut in Q9 = Q25
	}
	if (x>0) result = result;
	else     result = (1<<25) - result;
	return result >> 10;
#else
	int result;
	int abs_x = (Abs(x) * 3) >> 9; // * 3/4 (*3 >>2) and clip it to [0:255] (>>7) ot be an index of the LUT
	if (abs_x > 255) {
		result = 0xFFFF; // result = 1 in Q16
	} else {
		result = SIGMOID_LUT_uint16[abs_x]; // LUT in Q16
	}
	if (x>0) result = result;
	else     result = (1<<16) - result;
	return result >> 1;
#endif
}

int Tanh(int x){
#ifndef NEAREST
	int result, ua, ub, ut;
	int abs_x = (Abs(x) * 3) >> 8; // 2*x
	if (abs_x > 255) {
	  result = 0xFFFF00;
	} else {
	  ua = SIGMOID_LUT_uint16[abs_x];
	  ub = SIGMOID_LUT_uint16[abs_x+1];
	  ut = abs_x & 0xFF;
	  result = (ua << 8) + ut * (ub-ua);
	}
	if (x>0) result =  result - (1 << 23);
	else     result = -result + (1 << 23);
	return result >> 8; // back to 16 bits
#else
	int result;
	int abs_x = (Abs(x) * 3) >> 8; // 2*x
	if (abs_x > 255) {
	  result = 0xFFFF;
	} else {
	  result = SIGMOID_LUT_uint16[abs_x];
	}
	if (x>0) result =  result - (1 << 15);
	else     result = -result + (1 << 15);
	return result; // back to 16 bits
#endif
}

void RNN_ParKerB32_Hard_SameInStateScale_SQ8(KerRNN_SQ8_T *Arg)

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
        int TileOff = Arg->TileOffset;

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
			Of += X * ((signed char *)Vf)[Off+i];
		}

		/* Of = HTanh(Scaled(Of)) */
		Of = gap_clip(AT_SCALE(Of, ((unsigned char *)Infos)[RNN_F_SCALE], ((unsigned char *)Infos)[RNN_F_SCALEN]), 7);
		Of = Max(Infos[RNN_F_A0], Min(Infos[RNN_F_B0], Of));

		if (StateInOut) StateInOut[TileOff+o] = Of;
		if (Hout) Hout[o] = Of;

		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
		Vf = (v4s *) ((char *)Vf + NS);
	}
	gap_waitbarrier(0);
}


void RNN_ParKerB32_Hard_SQ8(KerRNN_SQ8_T *Arg)

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
        int TileOff = Arg->TileOffset;

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
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;
	v4s *Vin    = (v4s *) Xin;
	v4s *Vstate = (v4s *) State;
	v4s *Vf     = (v4s *) (Wf+Nin*First);
	for (int o=First; o<Last; o++) {
		int Of = Bf[o];
		for (int i=0; i<DimState/8; i++) {
			v4s X0 = Vstate[2*i], X1 = Vstate[2*i+1];
			v4s Y0 = Vf[2*i], Y1 = Vf[2*i+1];
			Of = gap_sumdotp4(X0, Y0, Of);
			Of = gap_sumdotp4(X1, Y1, Of);
		}
		if (DimState & 0x4) Of = gap_sumdotp4(Vin[DimState/4-1], Vf[DimState/4-1], Of);
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((char *)Vin)[Off+i];
			Of += X * ((signed char *)Vf)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + DimState);
		if (Xin) {
			int Of_in = 0;
			for (int i=0; i<DimIn/8; i++) {
				v4s X0 = Vin[2*i], X1 = Vin[2*i+1];
				v4s Y0 = Vf[2*i], Y1 = Vf[2*i+1];
				Of_in = gap_sumdotp4(X0, Y0, Of);
				Of_in = gap_sumdotp4(X1, Y1, Of);
			}
			if (DimIn & 0x4) Of = gap_sumdotp4(Vin[DimIn/4-1], Vf[DimIn/4-1], Of);
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((char *)Vin)[Off+i];
				Of_in += X * ((signed char *)Vf)[Off+i];
			}
			Of += AT_SCALE(Of_in, ((unsigned char *)Infos)[RNN_F_IN_SCALE], ((unsigned char *)Infos)[RNN_F_IN_SCALEN]);
			Vf = (v4s *) ((char *)Vf + DimIn);
		}

		/* Of = HTanh(Scaled(Of)) */
		Of = gap_clip(AT_SCALE(Of, ((unsigned char *)Infos)[RNN_F_SCALE], ((unsigned char *)Infos)[RNN_F_SCALEN]), 7);
		Of = Max(Infos[RNN_F_A0], Min(Infos[RNN_F_B0], Of));

		if (StateInOut) StateInOut[TileOff+o] = Of;
		if (Hout) Hout[o] = Of;

		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void RNN_ParKerB32_SameInStateScale_SQ8(KerRNN_SQ8_T *Arg)

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
        int TileOff = Arg->TileOffset;

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
			Of += X * ((signed char *)Vf)[Off+i];
		}

		/* Of = Tanh(Scaled(Of)) */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[RNN_F_SCALE], ((unsigned char *)Infos)[RNN_F_SCALEN]);
		Of = Tanh(Of);
		/* Scale to Output scale*/
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[RNN_OUT_SCALE], ((unsigned char *)Infos)[RNN_OUT_SCALEN]);

		if (StateInOut) StateInOut[TileOff+o] = Of;
		if (Hout) Hout[o] = Of;

		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
		Vf = (v4s *) ((char *)Vf + NS);
	}
	gap_waitbarrier(0);
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
        int TileOff = Arg->TileOffset;

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
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;
	v4s *Vin    = (v4s *) Xin;
	v4s *Vstate = (v4s *) State;
	v4s *Vf     = (v4s *) (Wf+Nin*First);
	for (int o=First; o<Last; o++) {
		int Of = Bf[o];
		for (int i=0; i<DimState/8; i++) {
			v4s X0 = Vstate[2*i], X1 = Vstate[2*i+1];
			v4s Y0 = Vf[2*i], Y1 = Vf[2*i+1];
			Of = gap_sumdotp4(X0, Y0, Of);
			Of = gap_sumdotp4(X1, Y1, Of);
		}
		if (DimState & 0x4) Of = gap_sumdotp4(Vin[DimState/4-1], Vf[DimState/4-1], Of);
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((char *)Vin)[Off+i];
			Of += X * ((signed char *)Vf)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + DimState);
		if (Xin) {
			int Of_in = 0;
			for (int i=0; i<DimIn/8; i++) {
				v4s X0 = Vin[2*i], X1 = Vin[2*i+1];
				v4s Y0 = Vf[2*i], Y1 = Vf[2*i+1];
				Of_in = gap_sumdotp4(X0, Y0, Of);
				Of_in = gap_sumdotp4(X1, Y1, Of);
			}
			if (DimIn & 0x4) Of = gap_sumdotp4(Vin[DimIn/4-1], Vf[DimIn/4-1], Of);
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((char *)Vin)[Off+i];
				Of_in += X * ((signed char *)Vf)[Off+i];
			}
			Of += AT_SCALE(Of_in, ((unsigned char *)Infos)[RNN_F_IN_SCALE], ((unsigned char *)Infos)[RNN_F_IN_SCALEN]);
			Vf = (v4s *) ((char *)Vf + DimIn);
		}

		/* Of = Tanh(Scaled(Of)) */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[RNN_F_SCALE], ((unsigned char *)Infos)[RNN_F_SCALEN]);
		Of = Tanh(Of);
		/* Scale to Output scale*/
		Of = gap_clip(AT_SCALE(Of, ((unsigned char *)Infos)[RNN_OUT_SCALE], ((unsigned char *)Infos)[RNN_OUT_SCALEN]), 7);

		if (StateInOut) StateInOut[TileOff+o] = Of;
		if (Hout) Hout[o] = Of;
		if (PerChannelQuant) Infos += RNN_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void LSTM_ParKerB32_Hard_SQ8(KerLSTM_SQ8_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
			2 + 2 + 2 + 2 + 6 + 7  LSTM_INT group contains 3 shorts and 1 byte

		In total: 21	LSTM_CELL_INFOS
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
        int TileOff = Arg->TileOffset;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, 2*DimState, CoreId); // Reset c_state and i_state -> 0s
		else Copy(State, StateInOut, 2*DimState, CoreId);                  // Copy c_state and i_state into State
		gap_waitbarrier(0);
	}

	// Offsets for leftovers
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;

	v4s *Vstate = (v4s *) (State+DimState);
	v4s *Vin    = (v4s *) (Xin);
	v4s *Vf     = (v4s *) (Wf+Nin*First);
	v4s *Vi     = (v4s *) (Wi+Nin*First);
	v4s *Vg     = (v4s *) (Wg+Nin*First);
	v4s *Vo     = (v4s *) (Wo+Nin*First);
	for (int o=First; o<Last; o++) {
		// Calculate: <in_state, r_2_xx_w> + xx_b (xx = ['forget', 'input', 'gate/cell', 'output'])
		// Scale: 	  Sin_state * Sxx_w
		int Of = Bf[o], Oi = Bi[o], Og = Bg[o], Oo = Bo[o];
		for (int i=0; i<DimState/4; i++) {
			v4s X = Vstate[i];
			Of = gap_sumdotp4(X, Vf[i], Of);
			Oi = gap_sumdotp4(X, Vi[i], Oi);
			Og = gap_sumdotp4(X, Vg[i], Og);
			Oo = gap_sumdotp4(X, Vo[i], Oo);
		}
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((char *)Vstate)[Off+i];
			Of += X * ((signed char *)Vf)[Off+i];
			Oi += X * ((signed char *)Vi)[Off+i];
			Og += X * ((signed char *)Vg)[Off+i];
			Oo += X * ((signed char *)Vo)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + DimState);
		Vi = (v4s *) ((char *)Vi + DimState);
		Vg = (v4s *) ((char *)Vg + DimState);
		Vo = (v4s *) ((char *)Vo + DimState);
		if (Xin){
			// Calculate: <input, i_2_xx_w> (xx = ['forget', 'input', 'gate/cell', 'output'])
			// Scale: 	  Sin * Sxx_w
			int Of_in = 0, Oi_in = 0, Og_in = 0, Oo_in = 0;
			for (int i=0; i<DimIn/4; i++) {
				v4s X = Vin[i];
				Of_in = gap_sumdotp4(X, Vf[i], Of_in);
				Oi_in = gap_sumdotp4(X, Vi[i], Oi_in);
				Og_in = gap_sumdotp4(X, Vg[i], Og_in);
				Oo_in = gap_sumdotp4(X, Vo[i], Oo_in);
			}
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((char *)Vin)[Off_in+i];
				Of_in += X * ((signed char *)Vf)[Off_in+i];
				Oi_in += X * ((signed char *)Vi)[Off_in+i];
				Og_in += X * ((signed char *)Vg)[Off_in+i];
				Oo_in += X * ((signed char *)Vo)[Off_in+i];
			}
			// Rescale Ox_in into Ox Scale and sum --> Infos[LSTM_X_IN_SCALE/N] = Sin / Sin_state
			// Ox = Ox + Sin/Sin_state * Of_in
			Of += AT_SCALE(Of_in, ((unsigned char *)Infos)[LSTM_F_IN_SCALE], ((unsigned char *)Infos)[LSTM_F_IN_SCALEN]);
			Oi += AT_SCALE(Oi_in, ((unsigned char *)Infos)[LSTM_I_IN_SCALE], ((unsigned char *)Infos)[LSTM_I_IN_SCALEN]);
			Og += AT_SCALE(Og_in, ((unsigned char *)Infos)[LSTM_G_IN_SCALE], ((unsigned char *)Infos)[LSTM_G_IN_SCALEN]);
			Oo += AT_SCALE(Oo_in, ((unsigned char *)Infos)[LSTM_O_IN_SCALE], ((unsigned char *)Infos)[LSTM_O_IN_SCALEN]);
			Vf = (v4s *) ((char *)Vf + DimIn);
			Vi = (v4s *) ((char *)Vi + DimIn);
			Vg = (v4s *) ((char *)Vg + DimIn);
			Vo = (v4s *) ((char *)Vo + DimIn);
		}

		/* Of = HSigmoid(Scaled(Of)) */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[LSTM_F_SCALE], ((unsigned char *)Infos)[LSTM_F_SCALEN]);
		Of = AT_NORM(AT_CLIP_POS(Of + *(short *)&Infos[LSTM_INT_B0], *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Oi = HSigmoid(Scaled(Oi)) */
		Oi = AT_SCALE(Oi, ((unsigned char *)Infos)[LSTM_I_SCALE], ((unsigned char *)Infos)[LSTM_I_SCALEN]);
		Oi = AT_NORM(AT_CLIP_POS(Oi + *(short *)&Infos[LSTM_INT_B0], *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Og = HTanh(Scaled(Og)) */
		Og = AT_SCALE(Og, ((unsigned char *)Infos)[LSTM_G_SCALE], ((unsigned char *)Infos)[LSTM_G_SCALEN]);

		int one = 1 << ((unsigned char *)Infos)[LSTM_INT_Q];
		Og = Max(-one, Min(one, Og));
		// Og = AT_CLIP(Og, One);

        	/* Oo = HSigmoid(Scaled(Oo)) */
        	Oo = AT_SCALE(Oo, ((unsigned char *)Infos)[LSTM_O_SCALE], ((unsigned char *)Infos)[LSTM_O_SCALEN]);
        	// Oo = AT_NORM(Max(0, Min(*((short *)&Infos[LSTM_INT_A0]), Oo + *((short *)&Infos[LSTM_INT_B0]))) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);
        	Oo = AT_NORM(AT_CLIP_POS(Oo + *((short *)&Infos[LSTM_INT_B0]), *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);
        
        	int X1 = AT_SCALE(State[TileOff+o], ((unsigned char *)Infos)[LSTM_CIN_SCALE], ((unsigned char *)Infos)[LSTM_CIN_SCALEN]);
        	X1 = Of * X1 + Oi * Og;
        	if (StateInOut) StateInOut[TileOff+o] = gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]), 7);
		// int X1 = AT_SCALE(Of * State[o] + Oi * Og, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]);
		// if (StateInOut) StateInOut[o] = X1;

		/* X1 = HTanh(X1) */
		one = one << Infos[LSTM_INT_Q];
		X1 = Max(-one, Min(one, X1));
		// X1 = AT_CLIP(X1, One);

        	int X2 = gap_clip(AT_SCALE(Oo * X1, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]), 7);
        	if (StateInOut) StateInOut[TileOff+DimState+o] = X2;
		// int X2 = AT_SCALE(Oo * X1, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]);
		// if (StateInOut) StateInOut[DimState+o] = X2;
		if (Hout) Hout[o] = X2;
		if (PerChannelQuant) Infos += LSTM_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void LSTM_ParKerB32_Hard_SameInStateScale_SQ8(KerLSTM_SQ8_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
			2 + 2 + 2 + 2 + 6 + 7  LSTM_INT group contains 3 shorts and 1 byte

		In total: 21	LSTM_CELL_INFOS
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
        int TileOff = Arg->TileOffset;

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
			Of += X * ((signed char *)Vf)[Off+i];
			Oi += X * ((signed char *)Vi)[Off+i];
			Og += X * ((signed char *)Vg)[Off+i];
			Oo += X * ((signed char *)Vo)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + NS);
		Vi = (v4s *) ((char *)Vi + NS);
		Vg = (v4s *) ((char *)Vg + NS);
		Vo = (v4s *) ((char *)Vo + NS);

		/* Of = HSigmoid(Scaled(Of)) */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[LSTM_F_SCALE], ((unsigned char *)Infos)[LSTM_F_SCALEN]);
		Of = AT_NORM(AT_CLIP_POS(Of + *(short *)&Infos[LSTM_INT_B0], *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Oi = HSigmoid(Scaled(Oi)) */
		Oi = AT_SCALE(Oi, ((unsigned char *)Infos)[LSTM_I_SCALE], ((unsigned char *)Infos)[LSTM_I_SCALEN]);
		Oi = AT_NORM(AT_CLIP_POS(Oi + *(short *)&Infos[LSTM_INT_B0], *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);

		/* Og = HTanh(Scaled(Og)) */
		Og = AT_SCALE(Og, ((unsigned char *)Infos)[LSTM_G_SCALE], ((unsigned char *)Infos)[LSTM_G_SCALEN]);

		int one = 1 << ((unsigned char *)Infos)[LSTM_INT_Q];
		Og = Max(-one, Min(one, Og));
		// Og = AT_CLIP(Og, One);

        	/* Oo = HSigmoid(Scaled(Oo)) */
        	Oo = AT_SCALE(Oo, ((unsigned char *)Infos)[LSTM_O_SCALE], ((unsigned char *)Infos)[LSTM_O_SCALEN]);
        	// Oo = AT_NORM(Max(0, Min(*((short *)&Infos[LSTM_INT_A0]), Oo + *((short *)&Infos[LSTM_INT_B0]))) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);
        	Oo = AT_NORM(AT_CLIP_POS(Oo + *((short *)&Infos[LSTM_INT_B0]), *((short *)&Infos[LSTM_INT_A0])) * *((short *)&Infos[LSTM_INT_C0]), ((unsigned char *)Infos)[LSTM_INT_Q]);
        
        	int X1 = AT_SCALE(State[TileOff+o], ((unsigned char *)Infos)[LSTM_CIN_SCALE], ((unsigned char *)Infos)[LSTM_CIN_SCALEN]);
        	X1 = Of * X1 + Oi * Og;
        	if (StateInOut) StateInOut[TileOff+o] = gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]), 7);
		// int X1 = AT_SCALE(Of * State[o] + Oi * Og, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]);
		// if (StateInOut) StateInOut[o] = X1;

		/* X1 = HTanh(X1) */
		one = one << Infos[LSTM_INT_Q];
		X1 = Max(-one, Min(one, X1));
		// X1 = AT_CLIP(X1, One);

        	int X2 = gap_clip(AT_SCALE(Oo * X1, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]), 7);
        	if (StateInOut) StateInOut[TileOff+DimState+o] = X2;
		// int X2 = AT_SCALE(Oo * X1, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]);
		// if (StateInOut) StateInOut[DimState+o] = X2;
		if (Hout) Hout[o] = X2;

		if (PerChannelQuant) Infos += LSTM_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void LSTM_ParKerB32_SameInStateScale_SQ8(KerLSTM_SQ8_T *Arg)

{
	/*	Sequences
	 	In:	DimIn!=0, Hout==0
		InOut:	DimIn!=0, Hout!=0
		None:	DimIn==0, Hout==0
		Out:	DimIn==0, Hout!=0

		Infos:
			2 + 2 + 2 + 2 + 6 + 7  LSTM_INT group contains 3 shorts and 1 byte

		In total: 21	LSTM_CELL_INFOS
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
        int TileOff = Arg->TileOffset;

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
			int X = ((signed char *)Vin)[Off+i];
			Of += X * ((signed char *)Vf)[Off+i];
			Oi += X * ((signed char *)Vi)[Off+i];
			Og += X * ((signed char *)Vg)[Off+i];
			Oo += X * ((signed char *)Vo)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + NS);
		Vi = (v4s *) ((char *)Vi + NS);
		Vg = (v4s *) ((char *)Vg + NS);
		Vo = (v4s *) ((char *)Vo + NS);

		/* Scale to internal_qtype --> 16bits QY */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[LSTM_F_SCALE], ((unsigned char *)Infos)[LSTM_F_SCALEN]);
		/* Of = Sigmoid(internal_scaled(Of)) */
		Of = Sigmoid(Of);

		/* Scale to internal_qtype --> 16bits QY */
		Oi = AT_SCALE(Oi, ((unsigned char *)Infos)[LSTM_I_SCALE], ((unsigned char *)Infos)[LSTM_I_SCALEN]); //--> clipped to int16
		/* Oi = Sigmoid(internal_scaled(Oi)) */
		Oi = Sigmoid(Oi);

		/* Scale to internal_qtype --> 16bits QY */
		Og = AT_SCALE(Og, ((unsigned char *)Infos)[LSTM_G_SCALE], ((unsigned char *)Infos)[LSTM_G_SCALEN]); //--> clipped to int16
		Og = Tanh(Og);

		/* Scale to internal_qtype --> 16bits QY */
		Oo = AT_SCALE(Oo, ((unsigned char *)Infos)[LSTM_O_SCALE], ((unsigned char *)Infos)[LSTM_O_SCALEN]); //--> clipped to int16
		/* Oi = Sigmoid(internal_scaled(Oi)) */
		Oo = Sigmoid(Oo);

		int X1 = AT_SCALE(State[TileOff+o] * Of, ((unsigned char *)Infos)[LSTM_CIN_SCALE], ((unsigned char *)Infos)[LSTM_CIN_SCALEN]);
		/* X1 = c_state = c_state*Of + Oi*Og */
		X1 = (int) X1 + ((Oi * Og) >> 15+3);
		
		/* Write c_state */
		if (StateInOut) StateInOut[TileOff+o] = gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]), 7);

		/* X1 = QuantTanh(X1) */
		X1 = Tanh(X1); //X1 in Q15 -> scale to Q12 to feed the Tanh function

		/* X2 = i_state = tanh(c_state) * Oo*/
		int X2 = ((X1 * Oo) >> 15);
		X2 = gap_clip(AT_SCALE(X2, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]), 7);
		if (StateInOut) StateInOut[TileOff+DimState+o] = X2;
		if (Hout) Hout[o] = X2;
		if (PerChannelQuant) Infos += LSTM_CELL_INFOS;
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
			2 + 2 + 2 + 2 + 6 + 7 + 7 LSTM_INT group contains 3 shorts and 1 byte

		In total: 29	LSTM_CELL_NOHARD_INFOS
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
        int TileOff = Arg->TileOffset;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;
	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, 2*DimState, CoreId); // Reset c_state and i_state -> 0s
		else Copy(State, StateInOut, 2*DimState, CoreId);                  // Copy c_state and i_state into State
		gap_waitbarrier(0);
	}

	// Offsets for leftovers
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;

	v4s *Vstate = (v4s *) (State+DimState);
	v4s *Vin    = (v4s *) (Xin);
	v4s *Vf     = (v4s *) (Wf+Nin*First);
	v4s *Vi     = (v4s *) (Wi+Nin*First);
	v4s *Vg     = (v4s *) (Wg+Nin*First);
	v4s *Vo     = (v4s *) (Wo+Nin*First);
	for (int o=First; o<Last; o++) {
		// Calculate: <in_state, r_2_xx_w> + xx_b (xx = ['forget', 'input', 'gate/cell', 'output'])
		// Scale: 	  Sin_state * Sxx_w
		int Of = Bf[o], Oi = Bi[o], Og = Bg[o], Oo = Bo[o];
		for (int i=0; i<DimState/4; i++) {
			v4s X = Vstate[i];
			Of = gap_sumdotp4(X, Vf[i], Of);
			Oi = gap_sumdotp4(X, Vi[i], Oi);
			Og = gap_sumdotp4(X, Vg[i], Og);
			Oo = gap_sumdotp4(X, Vo[i], Oo);
		}
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((char *)Vstate)[Off+i];
			Of += X * ((signed char *)Vf)[Off+i];
			Oi += X * ((signed char *)Vi)[Off+i];
			Og += X * ((signed char *)Vg)[Off+i];
			Oo += X * ((signed char *)Vo)[Off+i];
		}
		Vf = (v4s *) ((char *)Vf + DimState);
		Vi = (v4s *) ((char *)Vi + DimState);
		Vg = (v4s *) ((char *)Vg + DimState);
		Vo = (v4s *) ((char *)Vo + DimState);
		if (Xin){
			// Calculate: <input, i_2_xx_w> (xx = ['forget', 'input', 'gate/cell', 'output'])
			// Scale: 	  Sin * Sxx_w
			int Of_in = 0, Oi_in = 0, Og_in = 0, Oo_in = 0;
			for (int i=0; i<DimIn/4; i++) {
				v4s X = Vin[i];
				Of_in = gap_sumdotp4(X, Vf[i], Of_in);
				Oi_in = gap_sumdotp4(X, Vi[i], Oi_in);
				Og_in = gap_sumdotp4(X, Vg[i], Og_in);
				Oo_in = gap_sumdotp4(X, Vo[i], Oo_in);
			}
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((char *)Vin)[Off_in+i];
				Of_in += X * ((signed char *)Vf)[Off_in+i];
				Oi_in += X * ((signed char *)Vi)[Off_in+i];
				Og_in += X * ((signed char *)Vg)[Off_in+i];
				Oo_in += X * ((signed char *)Vo)[Off_in+i];
			}
			// Rescale Ox_in into Ox Scale and sum --> Infos[LSTM_X_IN_SCALE/N] = Sin / Sin_state
			// Ox = Ox + Sin/Sin_state * Of_in
			Of += AT_SCALE(Of_in, ((unsigned char *)Infos)[LSTM_F_IN_SCALE], ((unsigned char *)Infos)[LSTM_F_IN_SCALEN]);
			Oi += AT_SCALE(Oi_in, ((unsigned char *)Infos)[LSTM_I_IN_SCALE], ((unsigned char *)Infos)[LSTM_I_IN_SCALEN]);
			Og += AT_SCALE(Og_in, ((unsigned char *)Infos)[LSTM_G_IN_SCALE], ((unsigned char *)Infos)[LSTM_G_IN_SCALEN]);
			Oo += AT_SCALE(Oo_in, ((unsigned char *)Infos)[LSTM_O_IN_SCALE], ((unsigned char *)Infos)[LSTM_O_IN_SCALEN]);
			Vf = (v4s *) ((char *)Vf + DimIn);
			Vi = (v4s *) ((char *)Vi + DimIn);
			Vg = (v4s *) ((char *)Vg + DimIn);
			Vo = (v4s *) ((char *)Vo + DimIn);
		}

		/* Scale to internal_qtype --> 16bits QY */
		Of = AT_SCALE(Of, ((unsigned char *)Infos)[LSTM_F_SCALE], ((unsigned char *)Infos)[LSTM_F_SCALEN]);
		/* Of = Sigmoid(internal_scaled(Of)) */
		Of = Sigmoid(Of);

		/* Scale to internal_qtype --> 16bits QY */
		Oi = AT_SCALE(Oi, ((unsigned char *)Infos)[LSTM_I_SCALE], ((unsigned char *)Infos)[LSTM_I_SCALEN]); //--> clipped to int16
		/* Oi = Sigmoid(internal_scaled(Oi)) */
		Oi = Sigmoid(Oi);

		/* Scale to internal_qtype --> 16bits QY */
		Og = AT_SCALE(Og, ((unsigned char *)Infos)[LSTM_G_SCALE], ((unsigned char *)Infos)[LSTM_G_SCALEN]); //--> clipped to int16
		Og = Tanh(Og);

		/* Scale to internal_qtype --> 16bits QY */
		Oo = AT_SCALE(Oo, ((unsigned char *)Infos)[LSTM_O_SCALE], ((unsigned char *)Infos)[LSTM_O_SCALEN]); //--> clipped to int16
		/* Oi = Sigmoid(internal_scaled(Oi)) */
		Oo = Sigmoid(Oo);

		int X1 = AT_SCALE(State[TileOff+o] * Of, ((unsigned char *)Infos)[LSTM_CIN_SCALE], ((unsigned char *)Infos)[LSTM_CIN_SCALEN]);
		/* X1 = c_state = c_state*Of + Oi*Og */
		X1 = (int) X1 + ((Oi * Og) >> 15+3);
		
		/* Write c_state */
		if (StateInOut) StateInOut[TileOff+o] = gap_clip(AT_SCALE(X1, ((unsigned char *)Infos)[LSTM_COUT_SCALE], ((unsigned char *)Infos)[LSTM_COUT_SCALEN]), 7);

		/* X1 = QuantTanh(X1) */
		X1 = Tanh(X1); //X1 in Q15 -> scale to Q12 to feed the Tanh function

		/* X2 = i_state = tanh(c_state) * Oo*/
		int X2 = ((X1 * Oo) >> 15);
		X2 = gap_clip(AT_SCALE(X2, ((unsigned char *)Infos)[LSTM_OUT_SCALE], ((unsigned char *)Infos)[LSTM_OUT_SCALEN]), 7);
		if (StateInOut) StateInOut[TileOff+DimState+o] = X2;
		if (Hout) Hout[o] = X2;
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
        int * __restrict__ Bwh = Arg->Bwh;
        int * __restrict__ Brh = Arg->Brh;
        signed char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, DimState, CoreId); // Reset h_state -> 0s
		else Copy(State, StateInOut, DimState, CoreId);                  // Copy h_state into State
		gap_waitbarrier(0);
	}

	// Offsets for leftovers
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;

	v4s *Vstate = (v4s *) State;
	v4s *Vin    = (v4s *) Xin;
	v4s *Vr     = (v4s *) (Wr+Nin*First);
	v4s *Vz     = (v4s *) (Wz+Nin*First);
	v4s *Vh     = (v4s *) (Wh+Nin*First);
	for (int o=First; o<Last; o++) {
		int Or = Br[o], Oz = Bz[o], Oh = Brh[o];
		for (int i=0; i<DimState/4; i++) {
			v4s X0 = Vstate[i];
			v4s R0 = Vr[i];
			v4s Z0 = Vz[i];
			v4s H0 = Vh[i];
			Or = gap_sumdotp4(X0, R0, Or);
			Oz = gap_sumdotp4(X0, Z0, Oz);
			Oh = gap_sumdotp4(X0, H0, Oh);
		}
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((signed char *)Vstate)[Off+i];
			Or += X * ((signed char *)Vr)[Off+i];
			Oz += X * ((signed char *)Vz)[Off+i];
			Oh += X * ((signed char *)Vh)[Off+i];
		}
		Vr = (v4s *) ((signed char *)Vr + DimState);
		Vz = (v4s *) ((signed char *)Vz + DimState);
		Vh = (v4s *) ((signed char *)Vh + DimState);
		int Oh_in = Bwh[o];
		if (Xin) {
			int Or_in = 0, Oz_in = 0;
			for (int i=0; i<DimIn/4; i++) {
				v4s X0 = Vin[i];
				v4s R0 = Vr[i];
				v4s Z0 = Vz[i];
				v4s H0 = Vh[i];
				Or_in = gap_sumdotp4(X0, R0, Or_in);
				Oz_in = gap_sumdotp4(X0, Z0, Oz_in);
				Oh_in = gap_sumdotp4(X0, H0, Oh_in);
			}
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((signed char *)Vin)[Off_in+i];
				Or_in += X * ((signed char *)Vr)[Off_in+i];
				Oz_in += X * ((signed char *)Vz)[Off_in+i];
				Oh_in += X * ((signed char *)Vh)[Off_in+i];
			}
			Or += AT_SCALE(Or_in, ((unsigned char *)Infos)[GRU_R_IN_SCALE], ((unsigned char *)Infos)[GRU_R_IN_SCALEN]);
			Oz += AT_SCALE(Oz_in, ((unsigned char *)Infos)[GRU_Z_IN_SCALE], ((unsigned char *)Infos)[GRU_Z_IN_SCALEN]);
			Vr = (v4s *) ((signed char *)Vr + DimIn);
			Vz = (v4s *) ((signed char *)Vz + DimIn);
			Vh = (v4s *) ((signed char *)Vh + DimIn);
		}

		/* Scale to internal_qtype --> 16bits QY */
		Or = AT_SCALE(Or, ((unsigned char *)Infos)[GRU_R_INT_SCALE], ((unsigned char *)Infos)[GRU_R_INT_SCALEN]);
		/* Or = Sigmoid(internal_scaled(Or)) */
		Or = Sigmoid(Or);
		/* Q15 -> Q12 */
		Or = AT_NORM(Or, 3);

		/* Scale to internal_qtype --> 16bits QY */
		Oz = AT_SCALE(Oz, ((unsigned char *)Infos)[GRU_Z_INT_SCALE], ((unsigned char *)Infos)[GRU_Z_INT_SCALEN]);
		/* Oz = Sigmoid(internal_scaled(Oz)) */
		Oz = Sigmoid(Oz);
		/* Q15 -> Q12 */
		Oz = AT_NORM(Oz, 3);

		Oh = AT_NORM(Oh*Or, 12) + AT_SCALE(Oh_in, ((unsigned char *)Infos)[GRU_HT_IN_SCALE], ((unsigned char *)Infos)[GRU_HT_IN_SCALEN]);

		Oh = AT_SCALE(Oh, ((unsigned char *)Infos)[GRU_H_INT_SCALE], ((unsigned char *)Infos)[GRU_H_INT_SCALEN]);
		Oh = Tanh(Oh);
		Oh = AT_NORM(Oh, 3);

		int CurrStateQ12 = State[TileOff+o] << 5;
		/* h_t = (1 - Oz) * Oh + Oz * h_t-1 */
		/* Oz->Q12 * Oh->Q12 --> Q24 */
		/* h_t -> Q7 --> >> (24 - 7) */
		Oh = gap_clip(AT_NORM(((1 << 12) - Oz) * Oh + Oz * CurrStateQ12, 17), 7);
		if (StateInOut) StateInOut[TileOff+o] = Oh;
		if (Hout) Hout[o] = Oh;
		if (PerChannelQuant) Infos += GRU_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

void GRU_ParKerB32_Hard_SQ8(KerGRU_SQ8_T *Arg)

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
        int * __restrict__ Bwh = Arg->Bwh;
        int * __restrict__ Brh = Arg->Brh;
        signed char *__restrict__ Hout = Arg->Hout;
        unsigned short int Nout = Arg->Nout;
        signed char *__restrict__ Infos = Arg->Infos;
        int TileOff = Arg->TileOffset;

	unsigned int Nin = DimState+DimIn;
	unsigned int NS = Nin;

	unsigned int CoreId = gap_coreid();
	unsigned int ChunkCell = ChunkSize(Nout);
	unsigned int First = CoreId*ChunkCell;
	unsigned int Last  = Min(First+ChunkCell, Nout);

	if (Arg->FirstOut) {
		if (Arg->FirstCell && Arg->Reset) Zero(State, DimState, CoreId); // Reset h_state -> 0s
		else Copy(State, StateInOut, DimState, CoreId);                  // Copy h_state into State
		gap_waitbarrier(0);
	}

	// Offsets for leftovers
	int Off_in = 0;
	if (Xin) Off_in = DimIn/4; else Nin -= DimIn;
	int Off = DimState/4;

	v4s *Vstate = (v4s *) State;
	v4s *Vin    = (v4s *) Xin;
	v4s *Vr     = (v4s *) (Wr+Nin*First);
	v4s *Vz     = (v4s *) (Wz+Nin*First);
	v4s *Vh     = (v4s *) (Wh+Nin*First);
	for (int o=First; o<Last; o++) {
		int Or = Br[o], Oz = Bz[o], Oh = Brh[o];
		for (int i=0; i<DimState/4; i++) {
			v4s X0 = Vstate[i];
			v4s R0 = Vr[i];
			v4s Z0 = Vz[i];
			v4s H0 = Vh[i];
			Or = gap_sumdotp4(X0, R0, Or);
			Oz = gap_sumdotp4(X0, Z0, Oz);
			Oh = gap_sumdotp4(X0, H0, Oh);
		}
		for (int i=(DimState/4)*4; i<DimState; i++) {
			int X = ((signed char *)Vstate)[Off+i];
			Or += X * ((signed char *)Vr)[Off+i];
			Oz += X * ((signed char *)Vz)[Off+i];
			Oh += X * ((signed char *)Vh)[Off+i];
		}
		Vr = (v4s *) ((signed char *)Vr + DimState);
		Vz = (v4s *) ((signed char *)Vz + DimState);
		Vh = (v4s *) ((char *)Vh + DimState);

		int Oh_in = Bwh[o];
		if (Xin) {
			int Or_in = 0, Oz_in = 0;
			for (int i=0; i<DimIn/4; i++) {
				v4s X0 = Vin[i];
				v4s R0 = Vr[i];
				v4s Z0 = Vz[i];
				v4s H0 = Vh[i];
				Or_in = gap_sumdotp4(X0, R0, Or_in);
				Oz_in = gap_sumdotp4(X0, Z0, Oz_in);
				Oh_in = gap_sumdotp4(X0, H0, Oh_in);
			}
			for (int i=(DimIn/4)*4; i<DimIn; i++) {
				int X = ((signed char *)Vin)[Off_in+i];
				Or_in += X * ((signed char *)Vr)[Off_in+i];
				Oz_in += X * ((signed char *)Vz)[Off_in+i];
				Oh_in += X * ((signed char *)Vh)[Off_in+i];
			}
			Or += AT_SCALE(Or_in, ((unsigned char *)Infos)[GRU_R_IN_SCALE], ((unsigned char *)Infos)[GRU_R_IN_SCALEN]);
			Oz += AT_SCALE(Oz_in, ((unsigned char *)Infos)[GRU_Z_IN_SCALE], ((unsigned char *)Infos)[GRU_Z_IN_SCALEN]);
			Vr = (v4s *) ((signed char *)Vr + DimIn);
			Vz = (v4s *) ((signed char *)Vz + DimIn);
			Vh = (v4s *) ((signed char *)Vh + DimIn);
		}

		/* Scale to internal_qtype --> 16bits QY */
		Or = AT_SCALE(Or, ((unsigned char *)Infos)[GRU_R_INT_SCALE], ((unsigned char *)Infos)[GRU_R_INT_SCALEN]);
		/* Or = HSigmoid(internal_scaled(Or)) */
		Or = AT_NORM(AT_CLIP_POS(Or + *(short *)&Infos[GRU_INT_B0], *((short *)&Infos[GRU_INT_A0])) * *((short *)&Infos[GRU_INT_C0]), ((unsigned char *)Infos)[GRU_INT_Q]);

		/* Scale to internal_qtype --> 16bits QY */
		Oz = AT_SCALE(Oz, ((unsigned char *)Infos)[GRU_Z_INT_SCALE], ((unsigned char *)Infos)[GRU_Z_INT_SCALEN]);
		/* Oz = HSigmoid(internal_scaled(Oz)) */
		Oz = AT_NORM(AT_CLIP_POS(Oz + *(short *)&Infos[GRU_INT_B0], *((short *)&Infos[GRU_INT_A0])) * *((short *)&Infos[GRU_INT_C0]), ((unsigned char *)Infos)[GRU_INT_Q]);

		Oh = AT_NORM(Oh*Or, 12) + AT_SCALE(Oh_in, ((unsigned char *)Infos)[GRU_HT_IN_SCALE], ((unsigned char *)Infos)[GRU_HT_IN_SCALEN]);
		Oh = AT_SCALE(Oh, ((unsigned char *)Infos)[GRU_H_INT_SCALE], ((unsigned char *)Infos)[GRU_H_INT_SCALEN]);
		/* X1 = HTanh(X1) */
		int one = 1 << ((unsigned char *)Infos)[GRU_INT_Q];
		Oh = Max(-one, Min(one, Oh));

		int CurrStateQ12 = State[TileOff+o] << 5;
		/* h_t = (1 - Oz) * Oh + Oz * h_t-1 */
		/* Oz->Q12 * Oh->Q12 --> Q24 */
		/* h_t -> Q7 --> >> (24 - 7) */
		Oh = gap_clip(AT_NORM(((1 << 12) - Oz) * Oh + Oz * CurrStateQ12, 17), 7);
		if (StateInOut) StateInOut[TileOff+o] = Oh;
		if (Hout) Hout[o] = Oh;
		if (PerChannelQuant) Infos += GRU_CELL_INFOS;
	}
	gap_waitbarrier(0);
}

#pragma GCC diagnostic pop
