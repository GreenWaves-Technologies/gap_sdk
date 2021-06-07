/*
 * Copyright (C) 2017 GreenWaves Technologies
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#include <stdio.h>
#include "Gap.h"
#include "AutoTilerLib.h"
#include "Kernels.h"

L2_MEM char *M1;
L2_MEM char *M2, *M2_T;
L2_MEM char *Out;
L2_MEM char *Out2;

int Line_M1 = 200;
int Col_M1 = 100;
int Line_M2 = 100;
int Col_M2 = 150;

void Report(char *Mess, unsigned int T, int SoP)

{
	printf("%30s -> %10d. fmac/cycle: %f\n", Mess, T, (float)SoP/T);
}

void Check(signed char *In1, signed char *In2, int Size){
	for (int i=0; i<Size; i++) if(In1[i]!=In2[i]) printf("Not equal: In1[%d] = %d In2[%d] = %d\n", i, In1[i], i, In2[i]);
}

void InitMatrixfixed8(signed char *M1, signed char *M2, int Line_M1, int Col_M1, int Line_M2, int Col_M2){
	for (int line=0; line<Line_M2; line++) {
		for (int col=0; col<Col_M2; col++){
			M2[line*Col_M2+col] = line;
		}
	}
	for (int line=0; line<Line_M1; line++) {
		for (int col=0; col<Col_M1; col++){
			M1[line*Col_M1+col] = line;
		}
	}
}
void InitMatrixfixed16(short int *M1, short int *M2, int Line_M1, int Col_M1, int Line_M2, int Col_M2){
	for (int line=0; line<Line_M2; line++) {
		for (int col=0; col<Col_M2; col++){
			M2[line*Col_M2+col] = line;
		}
	}
	for (int line=0; line<Line_M1; line++) {
		for (int col=0; col<Col_M1; col++){
			M1[line*Col_M1+col] = line;
		}
	}
}
void TransposeMatrixfixed8(signed char *M, signed char *MT, int Line_M, int Col_M){
	for (int line=0; line<Line_M; line++) {
		for (int col=0; col<Col_M; col++){
			MT[col*Line_M+line] = M[line*Col_M+col];
		}
	}
}

void Process()

{
	unsigned int Ti;
	int SoP = Line_M1*Line_M2*Col_M1;

	L1_Memory = (char *) AT_L1_ALLOC(0, _L1_Memory_SIZE);
	if (L1_Memory==0) {
		printf("Failed to allocate shared L1 memory.\n"); 
	}

	printf("\nTesting [%dx%d]*[%dx%d] -> [%dx%d] SumOfProducts: %d. Running on %d cores. Shared L1: %d bytes\n\n",
		Line_M1, Col_M1, Line_M2, Col_M2, Line_M1, Col_M2, Col_M1*Col_M2*Line_M1, gap_ncore(), _L1_Memory_SIZE);
	gap_cl_starttimer();  gap_cl_resethwtimer();

	InitMatrixfixed8((signed char*)M1, (signed char*)M2, Line_M1, Col_M1, Line_M2, Col_M2);
	TransposeMatrixfixed8((signed char *)M2, (signed char *)M2_T, Line_M2, Col_M2);
	Ti = gap_cl_readhwtimer(); ParMultfixedp8((signed char *)M1, (signed char *)M2, (signed char *)Out, 10); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultfixedp8", Ti, SoP);
	Ti = gap_cl_readhwtimer(); ParMultVectfixedp8((signed char *)M1, (signed char *)M2, (signed char *)Out2, 10); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultVectfixedp8", Ti, SoP);
	Check((signed char *)Out, (signed char *)Out2, Line_M1*Col_M2);
	/* Experimental: // Ti = gap_cl_readhwtimer(); ParMultVect_Transfixedp8((signed char *)M1, (signed char *)M2_T, (signed char *)Out2, 10); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultVect_Transfixedp8", Ti, SoP); Check((signed char *)Out, (signed char *)Out2, Line_M1*Col_M2); */
	InitMatrixfixed16((short int *)M1, (short int *)M2, Line_M1, Col_M1, Line_M2, Col_M2);
	Ti = gap_cl_readhwtimer(); ParMultfixedp16((short int *)M1, (short int *)M2, (short int *)Out, 7); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultfixedp16", Ti, SoP);
	Ti = gap_cl_readhwtimer(); ParMultVectfixedp16((short int *)M1, (short int *)M2, (short int *)Out2, 7); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultVectfixedp16", Ti, SoP);
	Check((signed char *)Out, (signed char *)Out2, Line_M1*Col_M2);
	Ti = gap_cl_readhwtimer(); ParMultfixedp32((int *) M1, (int *) M2, (int *) Out, 15); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultfixedp32", Ti, SoP);

#ifdef WITH_FLOAT16
	Ti = gap_cl_readhwtimer(); ParMultfloat16((f16 *) M1, (f16 *) M2, (f16 *) Out); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultfloat16", Ti, SoP);
	Ti = gap_cl_readhwtimer(); ParMultVectfloat16((f16 *) M1, (f16 *) M2, (f16 *) Out); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultVectfloat16", Ti, SoP);
#endif
	Ti = gap_cl_readhwtimer(); ParMultfloat32((float *) M1, (float *) M2, (float *) Out); Ti = gap_cl_readhwtimer() - Ti; Report("ParMultfloat32", Ti, SoP);
}

int main()

{
	int Line_Out = Line_M1;
	int Col_Out = Col_M2;

	M1  = (char *) AT_L2_ALLOC(0, Line_M1*Col_M1*sizeof(int));
	M2  = (char *) AT_L2_ALLOC(0, Line_M2*Col_M2*sizeof(int));
	M2_T= (char *) AT_L2_ALLOC(0, Line_M2*Col_M2*sizeof(int));
	Out = (char *) AT_L2_ALLOC(0, Line_Out*Col_Out*sizeof(int));
	Out2 = (char *) AT_L2_ALLOC(0, Line_Out*Col_Out*sizeof(int));
	if ((M1==0)||(M2==0)||(Out==0)) {
		printf("Failed to allocate mem.\n"); return 0;
	}
#ifdef __riscv__
    struct pi_device cluster_dev;
    struct pi_cluster_conf cl_conf;
    cl_conf.id = 0;
    pi_open_from_conf(&cluster_dev, (void *) &cl_conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-4);
    }

	gap_fc_starttimer(); gap_fc_resethwtimer();
    struct pi_cluster_task task = {0};
	task.entry = Process;
    task.arg = NULL;
    task.stack_size = (unsigned int) 2024;
    task.slave_stack_size = (unsigned int) 1024;

    pi_cluster_send_task_to_cl(&cluster_dev, &task);

    pi_cluster_close(&cluster_dev);
#else
	Process();
#endif

	return 0;
}

