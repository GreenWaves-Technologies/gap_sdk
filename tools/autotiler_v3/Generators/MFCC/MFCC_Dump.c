#include "Gap8.h"
#include <stdlib.h>
#include <stdio.h>

void DumpShortInt(char *Mess, short int *Vect, int N, int F, int Precision)

{
#define Abs(x)	(((x)<0)?-(x):(x))
	unsigned int i;
	int Max = 0;

	for (i=0; i<N; i++) {
		if (Abs(Vect[i])>Max) Max = Abs(Vect[i]);
	}
	printf("%s (%d). Dim= %d, Q%d, Max=%f\n\t", Mess, F, N, Precision, FIX2FP(Max, Precision));
	for (i=0; i<N; i++) {
		printf("%d:%f, ", i, FIX2FP(Vect[i], Precision));
		if ((i+1)%10==0) printf("\n\t");
	}
	printf("\n");
#undef Abs
}

void DumpComplex(char *Mess, v2s *Vect, int N, int F, int Precision)

{
#define Abs(x)	(((x)<0)?-(x):(x))
	unsigned int i;
	int MaxI = 0, MaxR = 0;

	for (i=0; i<N; i++) {
		if (Abs(Vect[i][0])>MaxR) MaxR = Abs(Vect[i][0]);
		if (Abs(Vect[i][1])>MaxI) MaxI = Abs(Vect[i][1]);
	}

	printf("%s (%d) Dim: %d, Q%d, Max=[%f, %f]\n\t", Mess, F, N, Precision, FIX2FP(MaxR, Precision), FIX2FP(MaxI, Precision));
	for (i=0; i<N; i++) {
		printf("( %f, %f)", FIX2FP(Vect[i][0], Precision), FIX2FP(Vect[i][1], Precision));
		if ((i+1)%6==0) printf("\n\t");
	}
	printf("\n");
#undef Abs
}
