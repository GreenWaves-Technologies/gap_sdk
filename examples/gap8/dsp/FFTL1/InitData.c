
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Gap.h"

void InitData(short int *Data, int Size, int Pad, int Freq)
{
	int i;
	float Theta = (2*M_PI)/Freq;

	for (i=0; i<Size-Pad; i++) {
		if ((i%128)==0) printf("**** %d\n", i);
		// Data[i] = FP2FIXR(sinf(Theta*i), 15);
		Data[2*i] = FP2FIXR(cosf(Theta*i), 15);
		Data[2*i+1] = 0;
	}
	for (int i=Size-Pad; i<Size; i++) {
		Data[2*i] = 0;
		Data[2*i+1] = 0;
	}
}

void InitData4_float(float *Data, int Size, int Pad, int F1, int F2, int F3, int F4, float p1, float p2, float p3, float p4)
{
	int i;
	float Theta1 = (2*M_PI)/F1;
	float Theta2 = (2*M_PI)/F2;
	float Theta3 = (2*M_PI)/F3;
	float Theta4 = (2*M_PI)/F4;

	for (i=0; i<Size-Pad; i++) {
		// Data[i] = FP2FIXR(sinf(Theta*i), 15);
		float V = (cosf(Theta1*i+p1)+cosf(Theta2*i+p2)+cosf(Theta3*i+p3)+cosf(Theta4*i+p4))/4;
		Data[2*i] = V; // FP2FIXR(cosf(Theta1*i), 15);
		Data[2*i+1] = 0;
	}
	for (int i=Size-Pad; i<Size; i++) {
		Data[2*i] = 0;
		Data[2*i+1] = 0;
	}

	#ifdef DEBUG
	printf("Input_F32 = np.array(["); for (i=0; i<Size; i++) printf("%e%+ej, ", Data[2*i], Data[2*i+1]); printf("])\n");
	#endif
}

void InitData4(short int *Data, int Size, int Pad, int F1, int F2, int F3, int F4, float p1, float p2, float p3, float p4)
{
	int i;
	float Theta1 = (2*M_PI)/F1;
	float Theta2 = (2*M_PI)/F2;
	float Theta3 = (2*M_PI)/F3;
	float Theta4 = (2*M_PI)/F4;

	for (i=0; i<Size-Pad; i++) {
		// Data[i] = FP2FIXR(sinf(Theta*i), 15);
		float V = (cosf(Theta1*i+p1)+cosf(Theta2*i+p2)+cosf(Theta3*i+p3)+cosf(Theta4*i+p4))/4;
		Data[2*i] = FP2FIXR(V, 12); // FP2FIXR(cosf(Theta1*i), 15);
		Data[2*i+1] = 0;
	}
	for (int i=Size-Pad; i<Size; i++) {
		Data[2*i] = 0;
		Data[2*i+1] = 0;
	}

	#ifdef DEBUG
	printf("Input_Q12 = np.array(["); for (i=0; i<Size; i++) printf("%d%+dj, ", Data[2*i], Data[2*i+1]); printf("])\n");
	#endif
}

#ifdef GENERATE_FILES
int main(int argc, char *argv[])
{
	FILE *Fi;
	Fi = fopen("In_Data.h", "w");

	short int *InDataQ16  = (short int *)  malloc(2*MAXDIM*sizeof(short int));
	float *InDataf32  = (float *)      malloc(2*MAXDIM*sizeof(float));
	InitData4      (InDataQ16, MAXDIM, PAD, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	InitData4_float(InDataf32, MAXDIM, PAD, 37, 15, 23, 73, 0.1, 0.5, 0.6, 0.8);
	fprintf(Fi, "signed short InDataQ16[] = {\n\t");
	for (int i=0; i<2*MAXDIM; i++) fprintf(Fi, "%d, ", InDataQ16[i]);
	fprintf(Fi, "\n};\n");
	fprintf(Fi, "float InDataf32[] = {\n\t");
	for (int i=0; i<2*MAXDIM; i++) fprintf(Fi, "%f, ", InDataf32[i]);
	fprintf(Fi, "\n};\n");
	fclose(Fi);
    return 0;
}
#endif
