/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#define FFT_TWIDDLE_DYN 15

#define gap8_fl1(x)                     (31 - __builtin_clz((x)))
#define FIX2FP(Val, Precision)          ((float) (Val) / (float) (1<<(Precision)))
#define FP2FIXR(Val, Precision)         ((int)((Val)*((1 << (Precision))-1) + 0.5))
#define FP2FIX(Val, Precision)          ((int)((Val)*((1 << (Precision))-1)))


void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int N, int Inverse)

{
        int i;

        /* Radix 4: 3/4 of the twiddles
           Radix 2: 1/2 of the twiddles
        */

        if (Inverse) {
                float Theta = (2*M_PI)/Nfft;
                for (i=0; i<N; i++) {
                        float Phi = Theta*i;
                        // P_Twid[i] = (v2s) {(short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
                        //                    (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
                        Twiddles[2*i  ] = FP2FIXR(cosf(Phi), FFT_TWIDDLE_DYN); // (short int) (cos(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                        Twiddles[2*i+1] = FP2FIXR(sinf(Phi), FFT_TWIDDLE_DYN); // (short int) (sin(Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                }
        } else {
                float Theta = (2*M_PI)/Nfft;
                for (i=0; i<N; i++) {
                        float Phi = Theta*i;
                        // P_Twid[i] = (v2s) {(short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1)),
                        //                    (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1))};
                        Twiddles[2*i  ] = FP2FIXR(cosf(-Phi), FFT_TWIDDLE_DYN); // (short int) (cos(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                        Twiddles[2*i+1] = FP2FIXR(sinf(-Phi), FFT_TWIDDLE_DYN); // (short int) (sin(-Phi)*((1<<FFT_TWIDDLE_DYN)-1));
                }
        }
}

/* Setup a LUT for digitally reversed indexed, base is 4 */

void SetupR4SwapTable (short int *SwapTable, int Ni)

{
        int iL, iM, i, j;
        int Log4N  = gap8_fl1(Ni)>>1;

        iL = Ni / 4; iM = 1;
        SwapTable[0] = 0;

        for (i = 0; i < Log4N; ++i) {
                for (j = 0; j < iM; ++j) {
                        SwapTable[    iM + j] = SwapTable[j] +     iL;
                        SwapTable[2 * iM + j] = SwapTable[j] + 2 * iL;
                        SwapTable[3 * iM + j] = SwapTable[j] + 3 * iL;
                }
                iL >>= 2; iM <<= 2;
        }
}

void SetupR2SwapTable (short int *SwapTable, int Ni)

{
        int i, j, iL, iM;
        int Log2N  = gap8_fl1(Ni);

        iL = Ni / 2;
        iM = 1;
        SwapTable[0] = 0;

        for (i = 0; i < Log2N; ++i) {
                for (j = 0; j < iM; ++j) SwapTable[j + iM] = SwapTable[j] + iL;
                iL >>= 1; iM <<= 1;
        }
}

void PrintTwiddles(FILE *FiDef, FILE *FiInc, short int *Twid, int Dim)

{
	int i;

	fprintf(FiDef, "#ifdef FFTR2_%d\n", Dim);
	fprintf(FiDef, "L2_MEM short int R2_Twiddles_%d[%d] = {\n", Dim, Dim);
	for (i=0; i<(Dim/2); i++) fprintf(FiDef, "\t%d, %d,\n",  Twid[2*i], Twid[2*i+1]);
	fprintf(FiDef, "};\n");
	fprintf(FiDef, "#endif\n");

	fprintf(FiInc, "extern short int R2_Twiddles_%d[%d];\n", Dim, Dim);

	if (__builtin_ffs(Dim)&0x1) {	// Radix 4
		fprintf(FiDef, "#ifdef FFTR4_%d\n", Dim);
		fprintf(FiDef, "L2_MEM short int R4_Twiddles_%d[%d] = {\n", Dim, (3*Dim)/2);
		for (i=0; i<((3*Dim)/4); i++) fprintf(FiDef, "\t%d, %d,\n",  Twid[2*i], Twid[2*i+1]);
		fprintf(FiDef, "};\n");
		fprintf(FiDef, "#endif\n");

		fprintf(FiInc, "extern short int R4_Twiddles_%d[%d];\n", Dim, (3*Dim)/2);
	}
}

void PrintSwapTables(FILE *FiDef, FILE *FiInc, short int *R4SwapTable, short int *R2SwapTable, int Dim)

{
	int i;

	fprintf(FiDef, "#ifdef FFTR2_%d\n", Dim);
	fprintf(FiDef, "L2_MEM short int R2_SwapTable_%d[%d] = {\n", Dim, Dim);
	for (i=0; i<(Dim/2); i++) fprintf(FiDef, "\t%d, %d,\n",  R2SwapTable[2*i], R2SwapTable[2*i+1]);
	fprintf(FiDef, "};\n");
	fprintf(FiDef, "#endif\n");

	fprintf(FiInc, "extern short int R2_SwapTable_%d[%d];\n", Dim, Dim);

	if (__builtin_ffs(Dim)&0x1) {	// Radix 4
		fprintf(FiDef, "#ifdef FFTR4_%d\n", Dim);
		fprintf(FiDef, "L2_MEM short int R4_SwapTable_%d[%d] = {\n", Dim, Dim);
		for (i=0; i<(Dim/2); i++) fprintf(FiDef, "\t%d, %d,\n",  R4SwapTable[2*i], R4SwapTable[2*i+1]);
		fprintf(FiDef, "};\n");
		fprintf(FiDef, "#endif\n");

		fprintf(FiInc, "extern short int R4_SwapTable_%d[%d];\n", Dim, Dim);
	}
}

void GenerateTwiddles(FILE *FiDef, FILE *FiInc, int DimMin, int DimMax)

{
	int Dim;

	fprintf(FiDef, "#include \"Gap8.h\"\n");

	for (Dim=(1<<DimMin); Dim<(1<<DimMax); Dim = Dim<<1) {
		short int *Twid = (signed short *) malloc(sizeof(short int)*2*Dim);
		SetupTwiddlesLUT(Twid, Dim, Dim, 0);
		PrintTwiddles(FiDef, FiInc, Twid, Dim);
		free(Twid);
	}
}


void GenerateSwapTables(FILE *FiDef, FILE *FiInc, int DimMin, int DimMax)

{
	int Dim;

	fprintf(FiDef, "#include \"Gap8.h\"\n");

	for (Dim=(1<<DimMin); Dim<(1<<DimMax); Dim = Dim<<1) {
		short int *R4SwapTable = (signed short *) malloc(sizeof(short int)*Dim);
		short int *R2SwapTable = (signed short *) malloc(sizeof(short int)*Dim);
		SetupR4SwapTable(R4SwapTable, Dim);
		SetupR2SwapTable(R2SwapTable, Dim);
		PrintSwapTables(FiDef, FiInc, R4SwapTable, R2SwapTable, Dim);
		free(R4SwapTable); free(R2SwapTable);
	}
}

static char *StringAppend(char *Str1, char *Str2, char *Str3)

{
    char *Str = (char *) malloc(strlen(Str1)+strlen(Str2)+strlen(Str3)+1);
    if (Str==0) printf("StringAppend, memory allocation failed");
    sprintf(Str, "%s%s%s", Str1, Str2, Str3);
    return Str;
}

void main(int argc, char **argv)

{
	FILE *FiDef, *FiInc;
    char *OutputDir = ".";
    int c;

    while ((c = getopt (argc, argv, "rOKo:T:")) != -1) {
		switch (c) {
			case 'o':
				OutputDir = optarg;
				break;
			default:
                OutputDir = ".";
		}
	}

	FiDef = fopen(StringAppend(OutputDir, "/", "TwiddlesDef.c"), "w+"); FiInc = fopen(StringAppend(OutputDir, "/", "TwiddlesDef.h"), "w+");
	GenerateTwiddles(FiDef, FiInc, 4, 15);
	fclose(FiDef); fclose(FiInc);


	FiDef = fopen(StringAppend(OutputDir, "/", "SwapTablesDef.c"), "w+"); FiInc = fopen(StringAppend(OutputDir, "/", "SwapTablesDef.h"), "w+");
	GenerateSwapTables(FiDef, FiInc, 4, 15);
	fclose(FiDef); fclose(FiInc);

}


