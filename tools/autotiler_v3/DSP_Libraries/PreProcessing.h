#include "Gap.h"

typedef struct {
	void * __restrict__ Frame;
	void * __restrict__ Out;
	short int Prev;
	short int PreempFactor; 
	short int *Shift;
        short int QIn_FFT;
	int FrameSize;
	int maxin[8];
} PreEmphasis_T;

#ifdef __gap9__
typedef struct {
        void * __restrict__ Frame;
        void * __restrict__ Out;
        f16 Prev;
        f16 PreempFactor;
        int FrameSize;
} PreEmphasis_f16_T;
#endif

typedef struct {
        void * __restrict__ Frame;
        void * __restrict__ Out;
        float Prev;
        float PreempFactor;
        int FrameSize;
} PreEmphasis_f32_T;

typedef struct {
	void *__restrict__ Frame;
	void *__restrict__ OutFrame;
	void *__restrict__ Window;
	int FrameSize;
	int FFT_Dim;
} Windowing_T;


void PreEmphasis(PreEmphasis_T *Arg);
void PreEmphasis_f32(PreEmphasis_f32_T *Arg);

void WindowingReal2Cmplx_Fix16(Windowing_T *Arg);
void WindowingReal2Cmplx_Fix32(Windowing_T *Arg);
void WindowingReal2Cmplx_f32(Windowing_T *Arg);
void WindowingReal2Cmplx_PadCenter_Fix16(Windowing_T *Arg);
void WindowingReal2Cmplx_PadCenter_Fix32(Windowing_T *Arg);
void WindowingReal2Cmplx_PadCenter_f32(Windowing_T *Arg);
void WindowingReal2Real_Fix16(Windowing_T *Arg);
void WindowingReal2Real_Fix32(Windowing_T *Arg);
void WindowingReal2Real_f32(Windowing_T *Arg);
void WindowingReal2Real_PadCenter_Fix16(Windowing_T *Arg);
void WindowingReal2Real_PadCenter_Fix32(Windowing_T *Arg);
void WindowingReal2Real_PadCenter_f32(Windowing_T *Arg);
#ifdef __gap9__
void PreEmphasis_f16(PreEmphasis_f16_T *Arg);
void WindowingReal2Cmplx_f16(Windowing_T *Arg);
void WindowingReal2Cmplx_PadCenter_f16(Windowing_T *Arg);
void WindowingReal2Real_f16(Windowing_T *Arg);
void WindowingReal2Real_PadCenter_f16(Windowing_T *Arg);
#endif
