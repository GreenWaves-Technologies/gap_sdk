#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Gap8.h"
#include "MFCC.h"

#define MAX_FB 	128
#define MAX_FFT 512

float Mel(int k)

{
  	return(1125 * log(1 + k/700.0));
}

float InvMel(float k)

{
	return (700.0*(exp(k/1125.0) - 1.0));
}

typedef struct {
	int Start;
	int Stop;
	int Base;
	float Norm;
} FbankType;

FbankType Fbank[MAX_FB];
float MFCC_Coeffs[((MAX_FFT/2)+1)*MAX_FB];
int HeadCoeff;

void BuildFB(int SampleRate, int N_fft, int Nbanks, int Fmin, int Fmax)

{
	float SamplePeriod = 1.0/SampleRate;
	float FreqRes = (SamplePeriod*N_fft*700.0);
	float Step = ((float) (Mel(Fmax)-Mel(Fmin)))/(Nbanks+1);
	float Fmel0 = Mel(Fmin);
	short int f[MAX_FB];
	int i, j, k;
	FILE *fi;
	char Name[100];
	int Ntaps;

	printf("Sampling Rate = %d Hz, Period= %f\n", SampleRate, SamplePeriod);
	printf("FFT           = %d\n", N_fft);
	printf("Nb Banks      = %d\n", Nbanks);
	printf("F min         = %d Hz\n", Fmin);
	printf("F max         = %d Hz\n", Fmax);
	printf("Mel Step Freq = %f\n", Step);

	for (i=0; i<(Nbanks+2); i++) f[i] = ((N_fft+1)*InvMel(Fmel0+i*Step))/SampleRate;
	for (i=0; i<(Nbanks+2); i++) printf("f[%d] = %d\n", i, f[i]);

	HeadCoeff = 0;
	for (i=1; i<(Nbanks+1); i++) {
		Fbank[i-1].Start = f[i-1];
		Fbank[i-1].Stop  = f[i+1];
		Fbank[i-1].Base  = HeadCoeff;
		for (k=f[i-1]; k<f[i]; k++) {
			MFCC_Coeffs[HeadCoeff++] = ((float)(k-f[i-1]))/(f[i]-f[i-1]);
		}
		for (k=f[i]; k<f[i+1]; k++) {
			MFCC_Coeffs[HeadCoeff++] = ((float)(f[i+1]-k))/(f[i+1]-f[i]);
		}
	}
	Ntaps = 0;
	for (i=0; i<Nbanks; i++) {
		printf("Filter %3d: Start: %3d, Stop: %3d, Base: %d, Items:%d\n\t", i, Fbank[i].Start, Fbank[i].Stop, Fbank[i].Base,
			(Fbank[i].Stop-Fbank[i].Start+1));
		Ntaps += (Fbank[i].Stop-Fbank[i].Start+1);
		for (k=0, j=Fbank[i].Base; j<(Fbank[i].Base + (Fbank[i].Stop-Fbank[i].Start+1)); j++, k++) {
			printf("%f, ", MFCC_Coeffs[j]);
			if (((k+1)%10)==0) printf("\n\t");
		}
		printf("\n");
	}
	printf("Ntaps: %d, Avg = %f\n", Ntaps, (float) Ntaps/Nbanks);
// return;
	sprintf(Name, "MFCC_FB%d.def", Nbanks);
	fi = fopen(Name, "w");
	// fprintf(fi, "#define MFCC_BANK_COUNT\t\t%d\n", Nbanks);
	fprintf(fi, "#define MFCC_COEFF_CNT\t%d\n\n", HeadCoeff);
	fprintf(fi, "typedef struct {\n");
	fprintf(fi, "\tint Start;\n");
	fprintf(fi, "\tint Stop;\n");
	fprintf(fi, "\tint Base;\n");
	fprintf(fi, "\tshort int Norm;\n");
	fprintf(fi, "} FbankType;\n\n");
	fprintf(fi, "/* Filter Bank bands:\n\n");
	fprintf(fi, "\tMinimum Frequency: %d Hz\n", Fmin);
	fprintf(fi, "\tMaximum Frequency: %d Hz\n\n", Fmax);
	for (i=0; i<Nbanks; i++) {
		float Sum = 0.0;
		for (j=Fbank[i].Base; j<(Fbank[i].Base + (Fbank[i].Stop-Fbank[i].Start+1)); j++) {
			Sum += MFCC_Coeffs[j];
		}
		Fbank[i].Norm = 1.0/Sum;
		fprintf(fi, "\tBank%d\t: %8.2f Hz to %8.2f Hz, %2d Taps, SumOfTaps: %f\n",
			i,
			(Fbank[i].Start*SampleRate)/(float)N_fft, (Fbank[i].Stop*SampleRate)/(float)N_fft,
			Fbank[i].Stop-Fbank[i].Start+1,
			Sum);
	}
	fprintf(fi, "*/\n");
	fprintf(fi, "FbankType MFCC_FilterBank[MFCC_BANK_CNT] = {\n");
	for (i=0; i<Nbanks; i++) {
		fprintf(fi, "\t{%3d, %3d, %3d, %d},\n", Fbank[i].Start, Fbank[i].Stop, Fbank[i].Base, FP2FIX(Fbank[i].Norm, MFCC_COEFF_DYN));
	}
	fprintf(fi, "};\n\n");
	fprintf(fi, "short int MFCC_Coeffs[MFCC_COEFF_CNT] = {\n\t");
	for (i=0; i<HeadCoeff; i++) {
		fprintf(fi, "%5d, ", FP2FIX(MFCC_Coeffs[i], MFCC_COEFF_DYN));
		if (((i+1)%15)==0) fprintf(fi, "\n\t");
	}
	fprintf(fi, "\n};\n");
	fclose(fi);
}

void main()

{
	// BuildFB(IN_FREQ, N_FFT, MFCC_BANK_CNT, MIN_FREQ, MAX_FREQ);
	BuildFB(IN_FREQ, N_FFT, 64, 300, 8000);
}
	
	
