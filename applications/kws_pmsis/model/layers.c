#include "kwsKernels.h"
extern L2_MEM AT_L2_POINTER kws_L2_Memory;
extern AT_HYPERRAM_POINTER kws_L3_Memory;

void DumpInt(char *Mess, int *Vect, int N, int F, int Precision);
void DumpShortInt(char *Mess, short int *Vect, int N1,int N2, int F, int Precision);

int kwsCNN_layers(
		short int *__restrict__ Input_1,
		short int *__restrict__ Output_1)

{


    printf("=====>kwsL2mem %p\n",kws_L2_Memory);


    //for (int i=0;i<40*98;i++) Input_1[i] = Input_1[i]<<6;
#if 0
        DumpShortInt("input image ", (short int*)(Input_1), 40*98, 40, 0, 0 );
        DumpShortInt("weights 1st stage ", (short int*)(kws_L2_Memory+146816), 32*8*20, 20,0, 0 );
        DumpShortInt("bias 1st stage ", (short int*)(kws_L2_Memory+157056), 32, 1, 0, 0 );
#endif
	kwsPerf[0] = gap_cl_readhwtimer();
	S2_Conv2d_32x1x20x8(
		(short int *__restrict__) Input_1, /* In */
		(short int *__restrict__) (kws_L2_Memory + 146816), /* Filter */
		(short int *__restrict__) (kws_L2_Memory + 157056), /* Bias */
		(short int *__restrict__) (kws_L2_Memory + 24960) /* Out */
	);
	kwsPerf[0] = gap_cl_readhwtimer() - kwsPerf[0];
	kwsPerf[1] = gap_cl_readhwtimer();
#if 0
        DumpShortInt("input image(++) ", (short int*)(Input_1), 40*98, 40, 0, 0 );
	DumpShortInt("first conv layer", (short int*)(kws_L2_Memory + 24960), 32*16*39, 16, 0, 0 );

        DumpShortInt("weights 2nd stage ", (short int*)(kws_L2_Memory + 64896), 32*32*4*10, 10, 0, 0 );
        DumpShortInt("bias 2nd stage ", (short int*)(kws_L2_Memory + 157120), 32, 1,0, 0 );
#endif
	S3_Conv2d_32x32x10x4_Relu(
		(short int *__restrict__) (kws_L2_Memory + 24960), /* In */
		(short int *__restrict__) (kws_L2_Memory + 64896), /* Filter */
		(short int *__restrict__) (kws_L2_Memory + 157120), /* Bias */
		(short int *__restrict__) (kws_L2_Memory + 0) /* Out */
	);
	kwsPerf[1] = gap_cl_readhwtimer() - kwsPerf[1];
	kwsPerf[2] = gap_cl_readhwtimer();
#if 0
	DumpShortInt("second conv layer", (short int*)(kws_L2_Memory), 32*13*30, 13, 0, 0 );
        DumpShortInt("bias 3rd stage ", (short int*)(kws_L2_Memory + 157184), 12, 1,0, 0 );
#endif
	S4_Linear_12x32x30x13(
		(short int *__restrict__) (kws_L2_Memory+0), /* In */
		(short int *__restrict__) (kws_L3_Memory+0), /* Filter */
		(short int *__restrict__) (kws_L2_Memory+157184), /* Bias */
		(short int *__restrict__) Output_1 /* Out */
	);
	kwsPerf[2] = gap_cl_readhwtimer() - kwsPerf[2];
	DumpShortInt("output layer", (short int*)(Output_1), 12, 1, 0, 0 );

	return 0;
}
