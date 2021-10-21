#include "Gap.h"
#include "DSP_Lib.h"

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

void get_max(PreEmphasis_T *Arg)
{
        short int * __restrict__  Frame =   Arg->Frame;
        int FrameSize = Arg->FrameSize;
        int QIn_FFT = Arg->QIn_FFT;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);
        int *maxin = Arg->maxin;
        int shift;

        maxin[CoreId]=0;
        if (CoreId==0) maxin[0] = Abs(Arg->Prev);

        for (int j=First;j<Last;j++) {
                if(Abs((int)Frame[j])>maxin[CoreId]) maxin[CoreId]=Abs((int)Frame[j]);
        }
        gap_waitbarrier(0);

        if (CoreId==0) {
                int max=0;
                for(int j=0; j<gap_ncore();j++) {
                        if (maxin[j]>max) max=maxin[j];
                }

                shift = max?gap_fl1(max):0;
                // set to Q13 for FFT RAD2 and Q12 for FFT RAD4
                //if (shift<=12) shift = (12-shift); else shift=0;
                shift = (QIn_FFT-shift);
                *(Arg->Shift)=shift;
        }
        gap_waitbarrier(0);
}

void PreEmphasis(PreEmphasis_T *Arg)
{
        short int * __restrict__ Frame = (short int *__restrict__) Arg->Frame;
        short int * __restrict__ Out =   (short int *__restrict__) Arg->Out;
        int FrameSize = Arg->FrameSize;
        short int PreempFactor = Arg->PreempFactor;
        short int S;
        short int Sprev;

        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        // Y[n]=X[n]−0.97⋅X[n−1]
        // To gain in precision:
        //    Y[n] = X[n]<<shift - Q15(0.97)*X[n-1]<<shift
        // X[n] -> Q15
        // Y[n] -> Q15+shift
        get_max(Arg);
        int shift = *(Arg->Shift);
        if (shift>0) {
                S = CoreId? Frame[First-1]<<shift : Arg->Prev<<shift;
                for (i=First; i<Last; i++) {
                        Sprev = Frame[i]<<shift;
                        Out[i] = Sprev - gap_mulsRN(PreempFactor, S, 15);
                        S = Sprev;
                }
                gap_waitbarrier(0);
        } else {
                shift = -shift;
                S = CoreId? Frame[First-1]>>shift : Arg->Prev>>shift;
                for (i=First; i<Last; i++) {
                        Sprev = Frame[i]>>shift;
                        Out[i] = Sprev - gap_mulsRN(PreempFactor, S, 15);
                        S = Sprev;
                }
                gap_waitbarrier(0);                
        }

        if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
        if (CoreId==0) {
        printf("\nShift = %d\n",*Arg->Shift); printf("out_preemph_c = np.array([\n\t\t"); for(int i=0; i<FrameSize ; i++) printf("%d, ", Out[i]); printf("])\n");
        printf("\ninput_frame = np.array([\n\t\t"); for(int i=0; i<FrameSize ; i++) printf("%d, ", Frame[i]); printf("])\n");
        }
        gap_waitbarrier(0);
#endif
}

void PreEmphasis_f16(PreEmphasis_f16_T *Arg)
{
        F16_DSP * __restrict__ Frame = (F16_DSP *__restrict__) Arg->Frame;
        F16_DSP * __restrict__ Out =   (F16_DSP *__restrict__) Arg->Out;
        int FrameSize = Arg->FrameSize;
        F16_DSP PreempFactor = (F16_DSP) Arg->PreempFactor;
        F16_DSP S;
        F16_DSP Sprev;

        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        // Y[n]=X[n]−0.97⋅X[n−1]
        S = CoreId? Frame[First-1] : Arg->Prev;
        for (i=First; i<Last; i++) {
                Sprev = Frame[i];
                Out[i] = Sprev - PreempFactor * S;
                S = Sprev;
        }
        gap_waitbarrier(0);

        if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_preemph_c = np.array([\n\t\t"); for(int i=0; i<FrameSize ; i++) printf("%f, ", Out[i]); printf("])\n");
        }
        gap_waitbarrier(0);
#endif
}

void PreEmphasis_f32(PreEmphasis_f32_T *Arg)
{
        float * __restrict__ Frame = (float *__restrict__) Arg->Frame;
        float * __restrict__ Out =   (float *__restrict__) Arg->Out;
        int FrameSize = Arg->FrameSize;
        float PreempFactor = (float) Arg->PreempFactor;
        float S;
        float Sprev;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        // Y[n]=X[n]−0.97⋅X[n−1]
        S = CoreId? Frame[First-1] : Arg->Prev;

        for (i=First; i<Last; i++) {
                Sprev = Frame[i];
                Out[i] = Sprev - PreempFactor * S;
                S = Sprev;
        }

        gap_waitbarrier(0);

        if (CoreId==0) Arg->Prev = Frame[FrameSize-1];

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_preemph_c = np.array([\n\t\t"); for(int i=0; i<FrameSize ; i++) printf("%f, ", Out[i]); printf("])\n");
        }
        gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        v2s *__restrict__       OutFrame  = (v2s*) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (v2s) {gap_mulsRN(Frame[i], Window[i], 15), 0};
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = (v2s) {0,0};

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i][0]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[2*i]   = (int) gap_mulsRN(Frame[i], Window[i], 15);
                OutFrame[2*i+1] = (int) 0;
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[2*i] = 0;
                OutFrame[2*i+1] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[2*i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16V_DSP *__restrict__ OutFrame  = (F16V_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                OutFrame[i] = (F16V_DSP) {Frame[i] * Window[i], 0};
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = (F16V_DSP) {0,0};

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i][0]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[2*i] = (float) (Frame[i] * Window[i]);
                OutFrame[2*i+1] = 0;
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[2*i] = 0;
                OutFrame[2*i+1] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[2*i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_PadCenter_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        v2s *__restrict__       OutFrame  = (v2s*) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = (v2s) {0,0};

        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[(PadSize>>1)+i] = (v2s) {gap_mulsRN(Frame[i], Window[i], 15), 0};
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = (v2s) {0,0};

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i][0]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_PadCenter_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) {
                OutFrame[2*i]   = 0;
                OutFrame[2*i+1] = 0;
        }

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[2*i]   = (int) gap_mulsRN(Frame[i], Window[i-(PadSize>>1)], 15);
                OutFrame[2*i+1] = (int) 0;
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[2*i]   = 0;
                OutFrame[2*i+1] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[2*i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}


void WindowingReal2Cmplx_PadCenter_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16V_DSP *__restrict__ OutFrame  = (F16V_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = (F16V_DSP) {0,0};

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                OutFrame[i] = (F16V_DSP) {Frame[i] * Window[i-(PadSize>>1)], 0};
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = (F16V_DSP) {0,0};

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i][0]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Cmplx_PadCenter_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) {
                OutFrame[2*i] = 0;
                OutFrame[2*i+1] = 0;
        }

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[2*i] = (float) (Frame[i] * Window[i-(PadSize>>1)]);
                OutFrame[2*i+1] = 0;
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[2*i] = 0;
                OutFrame[2*i+1] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[2*i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        short int *__restrict__ OutFrame  = (short int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = gap_mulsRN(Frame[i], Window[i], 15);
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (int) gap_mulsRN(Frame[i], Window[i], 15);
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16_DSP *__restrict__ OutFrame  = (F16_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                OutFrame[i] = Frame[i] * Window[i];
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (float) (Frame[i] * Window[i]);
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_PadCenter_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        short int *__restrict__ OutFrame  = (short int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (int) gap_mulsRN(Frame[i], Window[i-(PadSize>>1)], 15);
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_PadCenter_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (int) gap_mulsRN(Frame[i], Window[i-(PadSize>>1)], 15);
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}


void WindowingReal2Real_PadCenter_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16_DSP *__restrict__ OutFrame  = (F16_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = Frame[i] * Window[i-(PadSize>>1)];
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void WindowingReal2Real_PadCenter_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (float) (Frame[i] * Window[i-(PadSize>>1)]);
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPad_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        short int *__restrict__ OutFrame  = (short int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = Frame[i];
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPad_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (int) Frame[i];
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPad_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16_DSP *__restrict__ OutFrame  = (F16_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                OutFrame[i] = Frame[i];
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPad_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = Frame[i];
        }
        // 0 padding
        Chunk = ChunkSize(FFT_Dim-FrameSize);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPadCenter_Fix16(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        short int *__restrict__ OutFrame  = (short int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                     FrameSize = Arg->FrameSize;
        int                     FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[(PadSize>>1)+i] = Frame[i];
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPadCenter_Fix32(Windowing_T *Arg)
{
        short int *__restrict__ Frame     = (short int *) Arg->Frame;
        int       *__restrict__ OutFrame  = (int *) Arg->OutFrame;
        short int *__restrict__ Window    = (short int *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (int) Frame[i];
        }

        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%d, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}


void ZeroPadCenter_f16(Windowing_T *Arg)
{
        F16_DSP *__restrict__ Frame     = (F16_DSP*) Arg->Frame;
        F16_DSP *__restrict__ OutFrame  = (F16_DSP*) Arg->OutFrame;
        F16_DSP *__restrict__ Window    = (F16_DSP*) Arg->Window;
        int               FrameSize = Arg->FrameSize;
        int               FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                OutFrame[i] = Frame[i];
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) OutFrame[i] = 0;

        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void ZeroPadCenter_f32(Windowing_T *Arg)
{
        float *__restrict__ Frame     = (float *) Arg->Frame;
        float *__restrict__ OutFrame  = (float *) Arg->OutFrame;
        float *__restrict__ Window    = (float *) Arg->Window;
        int                 FrameSize = Arg->FrameSize;
        int                 FFT_Dim   = Arg->FFT_Dim;
        int PadSize = FFT_Dim-FrameSize;
        unsigned int i, Chunk, First, Last, CoreId=gap_coreid();

        // 0 padding
        Chunk = ChunkSize(PadSize>>1);
        First = FrameSize + CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1));
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }

        Chunk = ChunkSize(FrameSize);
        First = (PadSize>>1)+CoreId*Chunk; Last = Min(First + Chunk, (PadSize>>1)+FrameSize);
        for (i=First; i<Last; i++) {
                /* Frame is Q15+shift and Window is Q15 */
                OutFrame[i] = (float) (Frame[i] * Window[i-(PadSize>>1)]);
        }
        // 0 padding
        Chunk = ChunkSize(PadSize-(PadSize>>1));
        First = (PadSize>>1)+FrameSize + CoreId*Chunk; Last = Min(First + Chunk, FFT_Dim);
        for (i=First; i<Last;i++) {
                OutFrame[i] = 0;
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("out_window_c = np.array([\n\t\t"); for(int i=0; i<FFT_Dim ; i++) printf("%f, ", OutFrame[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}