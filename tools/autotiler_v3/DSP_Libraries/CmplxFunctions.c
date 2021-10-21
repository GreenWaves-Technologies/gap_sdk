#include "Gap.h"
#include "DSP_Lib.h"

#define SQRT_2_Q15          (0xb504)

static inline unsigned int __attribute__((always_inline)) ChunkSize(unsigned int X)
{
        unsigned int NCore;
        unsigned int Log2Core;
        unsigned int Chunk;

        NCore = gap_ncore();
        Log2Core = gap_fl1(NCore);
        Chunk = (X>>Log2Core) + ((X&(NCore-1))!=0);
        return Chunk;
}

void CmplxMagSquared_Fix16(CmplxMag_T *Arg)
{
        int          *__restrict__ FrameIn    = (int *)          Arg->FrameIn;
        unsigned int *__restrict__ MagSquared = (unsigned int *) Arg->FrameOut;
        short int                  ExtraQ  = Arg->ExtraQ;
        int                        In_Q    = Arg->Input_QFormat;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        unsigned int P;
        for (i=First; i<(unsigned int)Last; i++) {
                P = (unsigned int) gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i]);
                MagSquared[i] = P;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_power = np.array([\n\t"); for (i=0; i<N; i++) printf("%d, ", MagSquared[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMagSquared_Fix32_scal(CmplxMag_T *Arg)
{
        int          *__restrict__ FrameIn    = (int *)          Arg->FrameIn;
        unsigned int *__restrict__ MagSquared = (unsigned int *) Arg->FrameOut;
        short int                  ExtraQ  = Arg->ExtraQ;
        int                        In_Q    = Arg->Input_QFormat;
        signed char  *__restrict__ shift_fft  = (signed char *)  Arg->shift_fft;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        unsigned int P;
        for (i=First; i<(unsigned int)Last; i++) {
                P = (unsigned int) ((FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16));
                MagSquared[i] = P;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_power = np.array([\n\t"); for (i=0; i<N; i++) printf("%d, ", MagSquared[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMagSquared_f16(CmplxMag_T *Arg)
{
        F16V_DSP *__restrict__ FrameIn    = (F16V_DSP *) Arg->FrameIn;
        F16_DSP *__restrict__ MagSquared = (F16_DSP *) Arg->FrameOut;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        for (i=First; i<(unsigned int)Last; i++) {
                F16_DSP P = (FrameIn[i][0] * FrameIn[i][0]) + (FrameIn[i][1] * FrameIn[i][1]);
                MagSquared[i] = P;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_power = np.array([\n\t"); for (i=0; i<N; i++) printf("%f, ", MagSquared[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMagSquared_f32(CmplxMag_T *Arg)
{
        float *__restrict__ FrameIn    = (float *) Arg->FrameIn;
        float *__restrict__ MagSquared = (float *) Arg->FrameOut;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        for (i=First; i<(unsigned int)Last; i++) {
                float P = (FrameIn[2*i] * FrameIn[2*i]) + (FrameIn[2*i+1] * FrameIn[2*i+1]);
                MagSquared[i] = P;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_power = np.array([\n\t"); for (i=0; i<N; i++) printf("%f, ", MagSquared[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMag_Fix32(CmplxMag_T *Arg)
{
        int          *__restrict__ FrameIn = (int *)          Arg->FrameIn;
        unsigned int *__restrict__ Mag     = (unsigned int *) Arg->FrameOut;
        short int                  ExtraQ  = Arg->ExtraQ;
        int                        In_Q    = Arg->Input_QFormat;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        unsigned int SqrtP;
        int QP = 2*(In_Q + ExtraQ) - 15;
        int Mul = QP%2?(SQRT_2_Q15>>7):(1<<8);
        int Shift = QP>0?9:8;
        if (QP > 0){
                for (i=First; i<(unsigned int)Last; i++) {
                        SqrtP = usqrt_17_15((unsigned int) gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i])) >> (QP/2);
                        Mag[i] = gap_roundnorm_reg(SqrtP*Mul, Shift);
                }
        } else {
                for (i=First; i<(unsigned int)Last; i++) {
                        SqrtP = usqrt_17_15((unsigned int) gap_dotp2(((v2s*)FrameIn)[i], ((v2s*)FrameIn)[i])) << (-QP/2);
                        Mag[i] = gap_roundnorm_reg(SqrtP*Mul, Shift);
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_mag = np.array([\n\t"); for (i=0; i<N; i++) printf("%u, ", Mag[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMag_Fix32_scal(CmplxMag_T *Arg)
{
        int          *__restrict__ FrameIn = (int *)          Arg->FrameIn;
        unsigned int *__restrict__ Mag     = (unsigned int *) Arg->FrameOut;
        short int                  ExtraQ  = Arg->ExtraQ;
        signed char *__restrict__  shift_fft = Arg->shift_fft;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        unsigned int SqrtP;
        int QP, Mul;
        for (i=First; i<(unsigned int)Last; i++) {
                QP = 2*(ExtraQ + shift_fft[i] + 15 - 16) - 15;
                Mul = QP%2?(SQRT_2_Q15>>5):(1<<10);
                if (QP > 0){
                        SqrtP = usqrt_17_15((unsigned int) ((FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16))) >> (QP/2);
                        Mag[i] = (SqrtP * Mul) >> 11;
                } else {
                        SqrtP = usqrt_17_15((unsigned int) ((FrameIn[2*i]>>16) * (FrameIn[2*i]>>16) + (FrameIn[2*i+1]>>16) * (FrameIn[2*i+1]>>16))) << (-QP/2);
                        Mag[i] = (SqrtP * Mul) >> 10;
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_mag = np.array([\n\t"); for (i=0; i<N; i++) printf("%d, ", Mag[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMag_f16(CmplxMag_T *Arg)
{
        F16_DSP *__restrict__ FrameIn = (F16_DSP *) Arg->FrameIn;
        F16_DSP *__restrict__ Mag     = (F16_DSP *) Arg->FrameOut;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        for (i=First; i<(unsigned int)Last; i++) {
                #ifdef F16_DSP_BFLOAT
                Mag[i] = (F16_DSP) (Sqrtf16a(FrameIn[2*i]*FrameIn[2*i] + FrameIn[2*i+1]*FrameIn[2*i+1]));
                #else
                Mag[i] = (F16_DSP) (Sqrtf16(FrameIn[2*i]*FrameIn[2*i] + FrameIn[2*i+1]*FrameIn[2*i+1]));
                #endif
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_mag = np.array([\n\t"); for (i=0; i<N; i++) printf("%f, ", Mag[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void CmplxMag_f32(CmplxMag_T *Arg)
{
        float *__restrict__ FrameIn = (float *) Arg->FrameIn;
        float *__restrict__ Mag     = (float *) Arg->FrameOut;
        int N = (Arg->Nfft)/2 + 1;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        for (i=First; i<(unsigned int)Last; i++) {
                Mag[i] = (float) Sqrtf32(FrameIn[2*i]*FrameIn[2*i] + FrameIn[2*i+1]*FrameIn[2*i+1]);
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_mag = np.array([\n\t"); for (i=0; i<N; i++) printf("%f, ", Mag[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}
