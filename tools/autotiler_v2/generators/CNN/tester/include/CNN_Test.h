#ifndef _CNN_TEST_H_
#define _CNN_TEST_H_

#include "Gap8.h"
#include "AutoTilerLibTypes.h"

typedef void (*TestTensorFun6_fp)(
                short int * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                unsigned int Norm,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun6_fps)(
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                unsigned int Norm,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun7_fp)(
                short int * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                unsigned int Norm,
                unsigned int NormBias,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun7_fps)(
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                unsigned int Norm,
                unsigned int NormBias,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun7_fp_fps_fp)(
                short int * __restrict__ ,
                signed char * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                unsigned int Norm,
                unsigned int NormBias,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun7_fp_fp_fpd)(
                short int * __restrict__ ,
                short int * __restrict__ ,
                short int * __restrict__ ,
                int * __restrict__ ,
                unsigned int Norm,
                unsigned int NormBias,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun4_fp)(
                short int * __restrict__ ,
                short int * __restrict__ ,
                unsigned int Norm,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun4_fps)(
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                unsigned int Norm,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun3_fp)(
                short int * __restrict__ ,
                short int * __restrict__ ,
                Kernel_Exec_T *Ker);

typedef void (*TestTensorFun3_fps)(
                signed char * __restrict__ ,
                signed char * __restrict__ ,
                Kernel_Exec_T *Ker);

typedef enum {
	Tf6_fp		= 0,
	Tf6_fps		= 1,
	Tf7_fp		= 2,
	Tf7_fps		= 3,
	Tf7_fp_fps_fp	= 4,
	Tf7_fp_fp_fpd	= 5,
	Tf4_fp		= 6,
	Tf4_fps		= 7,
	Tf3_fp		= 8,
	Tf3_fps		= 9,
} FunType_T;

typedef struct {
        FunType_T Type;
        union {
                TestTensorFun6_fp TensorFun6_fp;
                TestTensorFun6_fps TensorFun6_fps;
                TestTensorFun7_fp TensorFun7_fp;
                TestTensorFun7_fps TensorFun7_fps;
                TestTensorFun7_fp_fps_fp TensorFun7_fp_fps_fp;
                TestTensorFun7_fp_fp_fpd TensorFun7_fp_fp_fpd;
                TestTensorFun4_fp TensorFun4_fp;
                TestTensorFun4_fps TensorFun4_fps;
                TestTensorFun3_fp TensorFun3_fp;
                TestTensorFun3_fps TensorFun3_fps;
        } Fun;
} TestTensorFun_T;


typedef struct {
	char *TestName;

	TestTensorFun_T TensorFun;

	v4s DataSize;	/* In, Filter, Bias, Out */

	v4s L3;	/* 0: In, 1: Filter, 2: Bias, 3: Out */

	unsigned short InFeat;
	unsigned short OutFeat;
	unsigned short W;
	unsigned short H;

	unsigned char Fc;
	unsigned char Stridec;
	v4s Padc;
	unsigned char ReLUc;
	unsigned char DoConv;

	unsigned char Fp;
	unsigned char Stridep;
	v4s Padp;
	unsigned char ReLUp;
	unsigned char DoPool;

	unsigned char DoLinear;
	unsigned char DoReLU;
	unsigned char DoSoftMax;

	unsigned char Norm;
	unsigned char NormBias;
	unsigned int OutCheckSum;
} RunConf_T;

extern int InitTest(RunConf_T *List, int N);
extern int RunTest(
        RunConf_T *List,
        int N,
        int Trace,
        int Log,
        int ReportRefElapsed,
        int CheckResults,
        int *NPassed);

#endif
