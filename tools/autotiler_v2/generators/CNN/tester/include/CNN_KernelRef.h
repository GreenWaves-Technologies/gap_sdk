#ifndef __REF_KERNEL_H__
#define __REF_KERNEL_H__

extern int InitTensor(v4s DataSize, unsigned int InP, unsigned int OutP, unsigned int W, unsigned int H, unsigned int Wo, unsigned int Ho,
		unsigned int FSw, unsigned int FSh,
		void *Gen_In, void *Gen_Filter, void *Gen_Bias, void *Gen_Out, void *Gen_BufferOut,
		int IStep, int OStep, int FStep, int Fval, int VStep, int BVal, int BStep);

extern int EvaluateLayer(
		v4s DataSize,
		unsigned int InFeat, unsigned int OutFeat, unsigned int W, unsigned int H,
		void *Gen_In, void *Gen_Filter, void *Gen_Bias, void *Gen_Out, void *Gen_Buffer,
		int ConvStride, int Fwc, int Fhc, v4s DoPadc, int ReLUc, int DoConv,
		int PoolStride, int Fwp, int Fhp, v4s DoPadp, int ReLUp, int DoPool,
		int DoReLU, int DoLinear, int DoSoftMax,
		unsigned int Norm, unsigned int NormBias);

extern unsigned int CheckSum(int DataSize, void *Gen_In, int Size);

extern void DumpPlane(char *Mess, int DataSize, void *Plane, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax);

extern void DumpFeaturePlanes(char *Mess, int DataSize, void *Plane, unsigned int NPlanes, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax);

extern void Dump2DFeaturePlanes(char *Mess, int DataSize, void *Plane, unsigned int InP, unsigned int OutP, unsigned int W, unsigned int Wmax, unsigned int H, unsigned int Hmax);

#endif
