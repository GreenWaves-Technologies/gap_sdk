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

#include "Gap8.h"
#include <stdlib.h>
#include <stdio.h>
#include "MFCC.h"

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
		printf("%d:%E, ", i, FIX2FP(Vect[i], Precision));
		if ((i+1)%4==0) printf("\n\t");
	}
	printf("\n");
#undef Abs
}

void DumpInt_norm(char *Mess, int *Vect, int N, int F, int Precision, FbankType  *norm)

{
#define Abs(x)	(((x)<0)?-(x):(x))
	unsigned int i;
	int Max = 0;
	int shift2,shift1;

	for (i=0; i<N; i++) {
		if (Abs(Vect[i])>Max) Max = Abs(Vect[i]);
	}
	printf("%s (%d). Dim= %d, Q%d, Max=%f\n", Mess, F, N, Precision, FIX2FP(Max, Precision));
	if (Precision>=31) {shift1=30;shift2=Precision-30;} else {shift1=Precision;shift2=0;}
	for (i=0; i<N; i++) {
	  printf("%d:%E, ", i, (Vect[i]/(float) (1<<shift1))/(float)(1<<shift2));
		if ((i+1)%4==0) printf("\n\t");
	}
	printf("\n");
#undef Abs
}

void DumpInt(char *Mess, int *Vect, int N, int F, int Precision)

{
#define Abs(x)	(((x)<0)?-(x):(x))
	unsigned int i;
	int Max = 0;
	int shift2,shift1;

	for (i=0; i<N; i++) {
		if (Abs(Vect[i])>Max) Max = Abs(Vect[i]);
	}
	printf("%s (%d). Dim= %d, Q%d, Max=%f\n", Mess, F, N, Precision, FIX2FP(Max, Precision));
	if (Precision>=31) {shift1=30;shift2=Precision-30;} else {shift1=Precision;shift2=0;}
	for (i=0; i<N; i++) {
	  printf("%d:%E, ", i, (Vect[i]/(float) (1<<shift1))/(float)(1<<shift2));
		if ((i+1)%4==0) printf("\n\t");
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
	  printf("%d %E, %E (%x %x)\n", i, FIX2FP(Vect[i][0], Precision), FIX2FP(Vect[i][1], Precision),Vect[i][0],Vect[i][1]);
		//if ((i+1)%6==0) printf("\n\t");
	}
	printf("\n");
#undef Abs
}
