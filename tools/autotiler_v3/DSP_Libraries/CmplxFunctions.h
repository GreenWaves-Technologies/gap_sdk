#include "Gap.h"

typedef struct {
        void *__restrict__ FrameIn;
        void *__restrict__ FrameOut;
        int Nfft;
        short int ExtraQ;
        short int Input_QFormat;
        signed char *__restrict__ shift_fft;
} CmplxMag_T;

void CmplxMagSquared_Fix16(CmplxMag_T *Arg);
void CmplxMagSquared_Fix32_scal(CmplxMag_T *Arg);
void CmplxMagSquared_f32(CmplxMag_T *Arg);
void CmplxMag_Fix32(CmplxMag_T *Arg);
void CmplxMag_Fix32_scal(CmplxMag_T *Arg);
void CmplxMag_f32(CmplxMag_T *Arg);
#ifdef __gap9__
void CmplxMagSquared_f16(CmplxMag_T *Arg);
void CmplxMag_f16(CmplxMag_T *Arg);
#endif
