#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "CNN_BasicKernels.h"


float Fp2Fl(int X, int Norm)

{
	return FIX2FP(X, Norm);
}

int Fl2Fp(float X, int Norm)

{
	return FP2FIX(X, Norm);
}

void InitShort(float *InF, short int *In_fp, int Norm, int N)

{
	float M=-1.0;
    	srand((unsigned int)time(NULL));

    	float a = (float) (1<<(16-Norm));
    	for (int i=0;i<N;i++) {
		InF[i] = ((float)rand()/(float)(RAND_MAX)) * a;
		if (InF[i]>M) M = InF[i];
		
	}
	M = M/2.0;
    	for (int i=0;i<N;i++) {
		InF[i] -= M;
		In_fp[i] = FP2FIX(InF[i], Norm);
	}
}

void InitByte(float *InF, signed char *In_fp, int Norm, int N)

{
	float M=-1.0;
    	srand((unsigned int)time(NULL));

    	float a = (float) (1<<(8-Norm));
    	for (int i=0;i<N;i++) {
		InF[i] = ((float)rand()/(float)(RAND_MAX)) * a;
		if (InF[i]>M) M = InF[i];
		
	}
	M = M/2.0;
    	for (int i=0;i<N;i++) {
		InF[i] -= M;
		In_fp[i] = FP2FIX(InF[i], Norm);
	}
}

void DumpShort(float *InF, short int *In_fp, float *OutF, short int *Out_fp, int Norm, int N)

{
	for (int i=0; i<N; i++) {
		printf("%3d: In: %f %f, Out: %f %f -> Error: %f\n", i, InF[i], FIX2FP(In_fp[i], Norm), OutF[i], FIX2FP(Out_fp[i], 15), OutF[i]-FIX2FP(Out_fp[i], 15));
	}
}

void DumpByte(float *InF, signed char *In_fp, float *OutF, short int *Out_fp, int Norm, int N)

{
	for (int i=0; i<N; i++) {
		printf("%3d: In: %f %f, Out: %f %f -> Error: %f\n", i, InF[i], FIX2FP(In_fp[i], Norm), OutF[i], FIX2FP(Out_fp[i], 15), OutF[i]-FIX2FP(Out_fp[i], 15));
	}
}

void SM(float *input_ptr, float *output_ptr, int dim)

{
	int d;
	int stride = 1;
	float sum;
	float inputMax = input_ptr[0];

 	for (d = 0; d < dim; d++) if (input_ptr[d*stride] >= inputMax) inputMax = input_ptr[d*stride];

	sum = 0;
    	for (d = 0; d < dim; d++) {
      		float z = exp(input_ptr[d*stride] - inputMax);
      		output_ptr[d*stride] = z;
      		sum += z;
    	}

	printf("True Sum: %f\n", sum);

    	for (d = 0; d < dim; d++) output_ptr[d*stride] *= 1/sum;
}

#define DIM 1000
short int INs[DIM];
short int OUTs[DIM];

signed char INb[DIM];
signed char OUTb[DIM];

float IN_F[DIM];
float OUT_F[DIM];

void TestShort()

{
	int Dim = sizeof(INs)/sizeof(short int);
	int Norm = 12;
	KerSoftMax_fp_T Arg = {INs, Dim, Norm, OUTs};

	InitShort(IN_F, INs, Norm, Dim);
	SM(IN_F, OUT_F, Dim);
	KerParSoftMax_fp(&Arg);
	DumpShort(IN_F, INs, OUT_F, OUTs, Norm, Dim);

	float SumFl = 0.0;
	int SumFp = 0;
	for (int i=0; i<Dim; i++) {
		SumFl += OUT_F[i];
		SumFp += OUTs[i];
	}
	printf("[Shorts] Sum -> True: %f, Estim: %f\n", SumFl, FIX2FP(SumFp, 15));
}

void TestByte()

{
	int Dim = sizeof(INb)/sizeof(signed char);
	int Norm = 6;
	KerSoftMax_fps_T Arg = {INb, Dim, Norm, OUTs};

	InitByte(IN_F, INb, Norm, Dim);
	SM(IN_F, OUT_F, Dim);
	KerParSoftMax_fps(&Arg);
	DumpByte(IN_F, INb, OUT_F, OUTs, Norm, Dim);

	float SumFl = 0.0;
	int SumFp = 0;
	for (int i=0; i<Dim; i++) {
		SumFl += OUT_F[i];
		SumFp += OUTs[i];
	}
	printf("[Bytes] Sum -> True: %f, Estim: %f\n", SumFl, FIX2FP(SumFp, 15));
}


int main()

{
	TestShort();
	TestByte();
}

