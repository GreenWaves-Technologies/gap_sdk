#include <stdio.h>
#include "Gap8.h"
#include "AutoTilerLibTypes.h"
#include "CNN_BasicKernels.h"
#include "CNN_KernelRef.h"
#include "CNN_Test.h"

char *_APP_DATA_ALLOC_L2_MEM;
char *IN;
char *FILTER;
char *BIAS;
char *OUT;
char *BUFFER;
extern char **_APP_L1_Memory;
extern char **_APP_L2_Memory;
extern int _APP_L1_MemorySize;
extern int _APP_L2_MemorySize;


#if !defined(__riscv__)
#include <stdlib.h>
#define Alloc_l1(x, y) malloc(x)
#define Alloc_l2(x, y) malloc(x)
#define Free_l1(x) free((x))
#define Free_l2(x) free((x))
#define PT_TYPE void *
#define UINT unsigned long int
#elif defined(OLD_RUNTIME)
#define Alloc_l1(x)     plp_alloc_l1((x))
#define Alloc_l2(x)     plp_alloc_l2((x))
#define Free_l1(x, y)   plp_free_l1((x))
#define Free_l2(x, y)   plp_free_l2((x))
#define PT_TYPE unsigned int
#define UINT unsigned int
#else
#define Alloc_l1(x)     rt_alloc( RT_ALLOC_CL_DATA, (x) )
#define Alloc_l2(x)     rt_alloc( RT_ALLOC_L2_CL_DATA, (x) )
#define Free_l1(x, y)   rt_free(RT_ALLOC_CL_DATA, (x), (y))
#define Free_l2(x, y)   rt_free(RT_ALLOC_L2_CL_DATA, (x), (y))
#define PT_TYPE unsigned int
#define UINT unsigned int

#endif

#define MALIGN(Value, Mask)	(((Value)+(Mask))&(~(Mask)))

static char *DoAlloc(v4s DataSize, v4s L3, unsigned int InFeat, unsigned int OutFeat,
	             unsigned int W, unsigned int H, unsigned int F, unsigned int Wc, unsigned int Hc, unsigned int Wo, unsigned int Ho, unsigned int DoConv, unsigned int DoPool)

{
	char *Base = _APP_DATA_ALLOC_L2_MEM;
#ifndef __riscv__
	L3 = (v4s){0,0,0,0};
#endif

	if (DataSize[0] && (L3[0]==0)) {
		Base = (char *) MALIGN((UINT) Base, DataSize[0]-1);
		IN = (char *) Base; Base += DataSize[0]*W*H*InFeat;
	} else IN = (char *) Base;
	if (DataSize[1] && (L3[1]==0)) {
		Base = (char *) MALIGN((UINT) Base, DataSize[1]-1);
		FILTER = (char *) Base; Base += DataSize[1]*F*F*InFeat*OutFeat;
	} else FILTER = (char *) Base;
	if (DataSize[2] && (L3[2]==0)) {
		Base = (char *) MALIGN((UINT) Base, DataSize[2]-1);
		BIAS = (char *) Base; Base += DataSize[2]*OutFeat;
	} else BIAS = (char *) Base;
	if (DataSize[3] && (L3[3]==0)) {
		Base = (char *) MALIGN((UINT) Base, DataSize[3]-1);
		OUT = (char *) Base; Base += DataSize[3]*Wo*Ho*OutFeat;
	} else OUT = (char *) Base;
	if (DoConv && DoPool && (((int) L3)==0)) {
		Base = (char *) MALIGN((UINT) Base, DataSize[3]-1);
		BUFFER = (char *) Base; Base += DataSize[3]*Wc*Hc;
	} else BUFFER = 0;

	return Base;
}

#ifdef __EMUL__
extern unsigned int __L3_Read, __L3_Write, __L2_Read, __L2_Write;
#endif

static unsigned int __attribute__ ((noinline)) RunTensorTest(
	RunConf_T *Conf,
	int Trace,
	int Log,
	int ReportRefElapsed,
	int CheckResults,
	int *Passed
)

{
	unsigned int InFeat = Conf->InFeat;
	unsigned int OutFeat = Conf->OutFeat;
	unsigned int W = Conf->W;
	unsigned int H = Conf->H;
	v4s L3 = Conf->L3;
	unsigned FConv = 1, StrideConv = 1;
	unsigned FPool = 1, StridePool = 1;
	unsigned int PadLc = 0, PadRc = 0, PadTc = 0, PadBc = 0;
	unsigned int PadLp = 0, PadRp = 0, PadTp = 0, PadBp = 0;

	if (Conf->DoConv) {
		FConv = Conf->Fc; StrideConv = Conf->Stridec;
	}
	if (Conf->DoPool) {
		FPool = Conf->Fp; StridePool = Conf->Stridep;
	}
	if ((int)(Conf->Padc)) {
		PadLc = (FConv-1)/2; PadRc = (FConv)/2; PadTc = (FConv-1)/2; PadBc = (FConv)/2;
	}
	if ((int)(Conf->Padp)) {
		PadLp = (FPool-1)/2; PadRp = (FPool)/2; PadTp = (FPool-1)/2; PadBp = (FPool)/2;
	}

	unsigned int Woc = (W-FConv+PadLc+PadRc)/StrideConv + 1;
	unsigned int Hoc = (H-FConv+PadTc+PadBc)/StrideConv + 1;
	unsigned int Wo = Conf->DoPool?((Woc-FPool+PadLp+PadRp)/StridePool + 1):Woc;
	unsigned int Ho = Conf->DoPool?((Hoc-FPool+PadTp+PadBp)/StridePool + 1):Hoc;

	v4s PadInc = (v4s){PadLc,PadRc,PadTc, PadBc};
	v4s PadInp = (v4s){PadLp,PadRp,PadTp, PadBp};

	void *In, *Filter, *Bias, *Out, *Buffer;

	unsigned int RefCheck, ActualCheck, ElapsedTime, ElapsedRefTime;
	unsigned int CyclePerConv, RemC;
	unsigned int RefCyclePerConv, RefRemC;

#ifndef __riscv__
	L3 = (v4s){0,0,0,0};
#endif
	if ((int) L3) CheckResults = 0;

	DoAlloc(Conf->DataSize, L3, InFeat, OutFeat, W, H, FConv, Woc, Hoc, Wo, Ho, Conf->DoConv, Conf->DoPool);
	In = (void *) IN; Filter = (void *) FILTER; Bias = (void *) BIAS; Out = (void *) OUT; Buffer = (void *) BUFFER;
	if (Log) {
		printf("%50s: I:%3d,O:%3d,W:%3d,H:%3d Size:[%1d,%1d,%1d,%1d] L3:[%1d%1d%1d%1d]", Conf->TestName, InFeat, OutFeat, W, H,
			Conf->DataSize[0], Conf->DataSize[1], Conf->DataSize[2], Conf->DataSize[3],
			L3[0],L3[1],L3[2],L3[3]);
		if (Conf->DoConv)
			printf(" Conv:[D:%1d,S:%1d,P:%c,R:%c]:%3dx%3d", FConv, StrideConv, (((int)PadInc)!=0)?'Y':'N', Conf->ReLUc?'Y':'N', Woc, Hoc);
		if (Conf->DoPool)
			printf(" Pool:[D:%1d,S:%1d,P:%c,R:%c]:%3dx%3d", FPool, StridePool,  (((int)PadInp)!=0)?'Y':'N', Conf->ReLUp?'Y':'N', Wo, Ho);
		if (Conf->DoLinear) {
			printf(" Linear:%3d", OutFeat);
			if (Conf->DoReLU) printf(" ReLU:%3d", OutFeat);
		} else if (Conf->DoReLU) printf(" ReLU:%3dx%3d", Wo, Ho);
		if (Conf->DoSoftMax) printf(" SoftMax:%3d", Ho);
		printf("%s", Trace?"\n":" => ");
	}
	if (CheckResults && (Conf->OutCheckSum == 0)) {
		InitTensor(Conf->DataSize, InFeat, OutFeat, W, H, Wo, Ho,
			   FConv, FConv,
			   In, Filter, Bias, Out, Buffer,
			   1, 0, 10, FP2FIX(0.1, 12), FP2FIX(0.01, 12), 0, 11);
		if (Trace) {
			if (Conf->DoLinear) DumpFeaturePlanes("RefIn", Conf->DataSize[0], In, 1, InFeat, -1, 1, -1);
			else DumpFeaturePlanes("RefIn", Conf->DataSize[0], In, InFeat, W, -1, H, -1);
			if (Conf->DoConv || Conf->DoLinear) {
				if (Conf->DoConv) Dump2DFeaturePlanes("Filter", Conf->DataSize[1], Filter, InFeat, OutFeat, FConv, 10, FConv, 10);
				else DumpFeaturePlanes("Filter", Conf->DataSize[1], Filter, OutFeat, InFeat, -1, 1, -1);
   				DumpFeaturePlanes("Bias", Conf->DataSize[2], Bias, 1, 1, -1, OutFeat, -1);
			}
		}
		gap8_resethwtimer(); ElapsedRefTime = gap8_readhwtimer();
		EvaluateLayer(
			Conf->DataSize,
			InFeat, OutFeat, W, H,
			In, Filter, Bias, Out, Buffer,
			StrideConv, FConv, FConv, PadInc, Conf->ReLUc, Conf->DoConv,
			StridePool, FPool, FPool, PadInp, Conf->ReLUp, Conf->DoPool,
			Conf->DoReLU, Conf->DoLinear, Conf->DoSoftMax, Conf->Norm, Conf->NormBias);
		ElapsedRefTime = gap8_readhwtimer() - ElapsedRefTime;
		RefCheck = CheckSum(Conf->DataSize[3], Out, OutFeat*Wo*Ho);
   		if (Trace) {
			if (Conf->DoLinear) DumpFeaturePlanes("RefOut", Conf->DataSize[3], Out, 1, OutFeat, -1, 1, -1);
			else DumpFeaturePlanes("RefOut", Conf->DataSize[3], Out, OutFeat, Wo, -1, Ho, -1);
		}
	} else if (CheckResults) RefCheck = Conf->OutCheckSum;
	if (((int) L3)==0) {
		InitTensor(Conf->DataSize, InFeat, OutFeat, W, H, Wo, Ho,
			   FConv, FConv,
			   In, Filter, Bias, Out, Buffer,
			   1, 0, 10, FP2FIX(0.1, 12), FP2FIX(0.01, 12), 0, 11);
	}
#ifdef __EMUL__
	__L3_Read = 0; __L3_Write = 0; __L2_Read = 0; __L2_Write = 0;
#endif
	gap8_resethwtimer(); ElapsedTime = gap8_readhwtimer();
	switch (Conf->TensorFun.Type) {
		case Tf6_fp:
			Conf->TensorFun.Fun.TensorFun6_fp((short int *) In, (short int *) Filter, (short int *) Bias, (short int *) Out, Conf->Norm, 0); break;
		case Tf6_fps:
			Conf->TensorFun.Fun.TensorFun6_fps((signed char *) In, (signed char *) Filter, (signed char *) Bias, (signed char *) Out, Conf->Norm, 0); break;
		case Tf7_fp:
			Conf->TensorFun.Fun.TensorFun7_fp((short int *) In, (short int *) Filter, (short int *) Bias, (short int *) Out, Conf->Norm, Conf->NormBias, 0); break;
		case Tf7_fps:
			Conf->TensorFun.Fun.TensorFun7_fps((signed char *) In, (signed char *) Filter, (signed char *) Bias, (signed char *) Out, Conf->Norm, Conf->NormBias, 0); break;
		case Tf7_fp_fps_fp:
			Conf->TensorFun.Fun.TensorFun7_fp_fps_fp((short int *) In, (signed char *) Filter, (short int *) Bias, (short int *) Out, Conf->Norm, Conf->NormBias, 0); break;
		case Tf7_fp_fp_fpd:
			Conf->TensorFun.Fun.TensorFun7_fp_fp_fpd((short int *) In, (short int *) Filter, (short int *) Bias, (int *) Out, Conf->Norm, Conf->NormBias, 0); break;
		case Tf4_fp:
			Conf->TensorFun.Fun.TensorFun4_fp((short int *) In, (short int *) Out, Conf->Norm, 0); break;
		case Tf4_fps:
			Conf->TensorFun.Fun.TensorFun4_fps((signed char *) In, (signed char *) Out, Conf->Norm, 0); break;
		case Tf3_fp:
			Conf->TensorFun.Fun.TensorFun3_fp((short int *) In, (short int *) Out, 0); break;
		case Tf3_fps:
			Conf->TensorFun.Fun.TensorFun3_fps((signed char *) In, (signed char *) Out, 0); break;
	}
	ElapsedTime = gap8_readhwtimer() - ElapsedTime;

	if (Trace) {
		if (Conf->DoLinear) DumpFeaturePlanes("Out", Conf->DataSize[3], Out, 1, OutFeat, -1, 1, -1);
		else DumpFeaturePlanes("Out", Conf->DataSize[3], Out, OutFeat, Wo, -1, Ho, -1);
	}
	if (CheckResults) ActualCheck = CheckSum(Conf->DataSize[3], Out, OutFeat*Wo*Ho);
	CyclePerConv = ElapsedTime/(InFeat*OutFeat*Woc*Hoc);
	RemC = (10*(ElapsedTime%(InFeat*OutFeat*Woc*Hoc)))/(InFeat*OutFeat*Woc*Hoc);

	if (Log) printf(" Elapsed: %10d Cycles (%2d.%1d).", ElapsedTime, CyclePerConv, RemC);
	if (CheckResults && ReportRefElapsed) {
		RefCyclePerConv = ElapsedRefTime/(InFeat*OutFeat*Woc*Hoc);
		RefRemC = (10*(ElapsedRefTime%(InFeat*OutFeat*Woc*Hoc)))/(InFeat*OutFeat*Woc*Hoc);
		printf(" Cycles (%3d.%1d).", RefCyclePerConv, RefRemC);
	}
	if (CheckResults && (ActualCheck != RefCheck)) {
		*Passed = 0;
		printf(" Check: Ref: %8X, Act: %8X", RefCheck, ActualCheck);
	} else {
		*Passed = 1;
		if (Log) {
			if (CheckResults) printf(" Check: %s", "OK"); else printf(" No Check");
		}
	}
#ifdef __EMUL__
	// printf(" L3[%d,%d], L2[%d,%d]", __L3_Read,__L3_Write, __L2_Read, __L2_Write);
	printf(" L3[%d,%d]", __L3_Read,__L3_Write);
#endif
	printf("\n");
	return ElapsedTime;
}

int RunTest(
	RunConf_T *List,
	int N,
	int Trace,
	int Log,
	int ReportRefElapsed,
	int CheckResults,
	int *NPassed)

{
#ifdef __EMUL__
	unsigned int Total_L3_Read = 0;
	unsigned int Total_L3_Write = 0;
#endif
	unsigned int TotalTime = 0;
	unsigned int TotalPassed = 0;

	*_APP_L1_Memory = (char *) Alloc_l1(_APP_L1_MemorySize);
	if (*_APP_L1_Memory==0) {
		printf("L1 Mem alloc failed, %d bytes\n", _APP_L1_MemorySize); return 0;
	}
	for (unsigned int i=0; i<N; i++) {
		int Passed;
		TotalTime += RunTensorTest(&List[i], Trace, Log, ReportRefElapsed, CheckResults, &Passed);
		TotalPassed += Passed;
#ifdef __EMUL__
	Total_L3_Read += __L3_Read;
	Total_L3_Write += __L3_Write;
#endif
	}
	*NPassed = TotalPassed;

	Free_l1((PT_TYPE) *_APP_L1_Memory, _APP_L1_MemorySize);
#ifdef __EMUL__
	if (Log) printf("L3 traffic: Read: %d bytes, Write: %d bytes. Total: %d bytes\n", Total_L3_Read, Total_L3_Write, Total_L3_Read + Total_L3_Write);
#endif
	return TotalTime;
}

int InitTest(RunConf_T *List, int N)

{
	UINT MaxSize = 0;
	char *L2_Mem = 0;

	for (unsigned int i=0; i<N; i++) {
		RunConf_T *Conf = &List[i];
		UINT Size;
		unsigned int W = Conf->W, H = Conf->H, Woc, Hoc, Wo, Ho;
		unsigned FConv = 1, StrideConv = 1;
		unsigned FPool = 1, StridePool = 1;
		unsigned int PadLc = 0, PadRc = 0, PadTc = 0, PadBc = 0;
		unsigned int PadLp = 0, PadRp = 0, PadTp = 0, PadBp = 0;

		if (Conf->DoConv) {
			FConv = Conf->Fc; StrideConv = Conf->Stridec;
		}
		if (Conf->DoPool) {
			FPool = Conf->Fp; StridePool = Conf->Stridep;
		}
		if ((int)(Conf->Padc)) {
			PadLc = (FConv-1)/2; PadRc = (FConv)/2; PadTc = (FConv-1)/2; PadBc = (FConv)/2;
		}
		if ((int)(Conf->Padp)) {
			PadLp = (FPool-1)/2; PadRp = (FPool)/2; PadTp = (FPool-1)/2; PadBp = (FPool)/2;
		}

		Woc = (W-FConv+PadLc+PadRc)/StrideConv + 1;
		Hoc = (H-FConv+PadTc+PadBc)/StrideConv + 1;
		Wo = Conf->DoPool?((Woc-FPool+PadLp+PadRp)/StridePool + 1):Woc;
		Ho = Conf->DoPool?((Hoc-FPool+PadTp+PadBp)/StridePool + 1):Hoc;

		_APP_DATA_ALLOC_L2_MEM = (char *) (UINT) MALIGN(_APP_L2_MemorySize, 4-1);
		Size = (UINT) DoAlloc(Conf->DataSize, Conf->L3, Conf->InFeat, Conf->OutFeat, W, H, FConv, Woc, Hoc, Wo, Ho, Conf->DoConv, Conf->DoPool);
		if (Size > MaxSize) MaxSize = Size;
	}

	L2_Mem = (char *) Alloc_l2(MaxSize);
	if (L2_Mem==0) {
		printf("L2 Mem alloc failed, %d bytes (Buffer L2: %d)\n", (unsigned int) MaxSize, _APP_L2_MemorySize); return 1;
	} else {
		printf("L2 Mem alloc OK, %d bytes (Buffer L2: %d)\n", (unsigned int) MaxSize, _APP_L2_MemorySize);
	}
	*_APP_L2_Memory = L2_Mem;
	_APP_DATA_ALLOC_L2_MEM = L2_Mem + MALIGN(_APP_L2_MemorySize, 4);

	return 0;

}


