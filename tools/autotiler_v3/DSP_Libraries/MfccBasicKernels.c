#include "Gap.h"
#include "DSP_Lib.h"
#include "FastFloatApprox.h"

// used in computelog -> assume output of log in Q16
//#define LN_2   ((int) floor(log(2)*(float)(1<<QN)+0.5))
//#define c1_3      ((int) floor((1.0/3.0) * (float)(1<<QN) +0.5))
#define QNN                 15
#define LN_2_1F16           (0xb172)
#define LN_2_1F15           0x000058B9
#define c1_3                (0x5555)
#define LN_10_INV_Q10       (0x1bd)
#define LOG10_2             (0x2688)
//#define LOG_OFFSET    (0x20) // 1e-3 * 2^15


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

void MelFilterBank_Fix32(MelFilterBank_T *Arg)
{
        unsigned int i, j, k;

        unsigned int *__restrict__ FramePower     = (unsigned int *__restrict__) Arg->FramePower;
        unsigned int *__restrict__ Mel_Spectr     = (unsigned int *__restrict__) Arg->MelSpectr;
        unsigned short int    *__restrict__ Mel_Coeffs     = (unsigned short int *__restrict__)    Arg->Mel_Coeffs;
        signed char  *__restrict__ shift_buff     = (signed char *__restrict__)  Arg->shift_buff;
        fbank_type_t *__restrict__ Mel_FilterBank = (fbank_type_t *__restrict__) Arg->Mel_FilterBank;
        unsigned int               Mel_NBanks     = (unsigned int) Arg->Mel_NBanks;
        short int                  Mel_Coeff_Dyn  = Arg->Mel_Coeff_dyn;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();

        /* MFCC_Coeffs is in MFCC_Coeff_Dyn */
        First = CoreId; int nb_cores = gap_ncore();
        for (i=First; i< Mel_NBanks; i+=nb_cores) {
                unsigned int Coeff = 0;
                int Base = Mel_FilterBank[i].Base;
                int NonZeroItems = Mel_FilterBank[i].Items;
                int LogNItems = gap_fl1(NonZeroItems);

                // Before computing the coeff multiplication check that max(FramePower) + 10 (QCoeff)
                // do not exceed 31, if so right shift (lose precision) the coeff
                int maxin=0;
                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        if (FramePower[j]>(unsigned int)maxin) maxin =FramePower[j];
                }
                int shift;
                short int shift0 = maxin?gap_fl1(maxin):0;
                // check multiply overflow condition FramePower * Q6.10 <= Q31
                if ((shift0+Mel_Coeff_Dyn+LogNItems)>31) shift = shift0+Mel_Coeff_Dyn+LogNItems-31; else shift = 0;
                shift_buff[i] = shift;

                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        // very easy to overflow here - maybe decrease the coeff dyn
                        Coeff = Coeff + Mel_Coeffs[Base+k]*(FramePower[j]>>shift);
                }
                Mel_Spectr[i] = Coeff?Coeff:1; // Avoid 0s for log
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_melfilterbank = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%u, ", Mel_Spectr[i]); printf("])\n"); 
                printf("\nshift_BF = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%d, ", shift_buff[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

/* compatible with block floating point fft */
void MelFilterBank_Fix32_Scal(MelFilterBank_T *Arg)
{
        unsigned int i, j, k;

        unsigned int *__restrict__ FramePower     = (unsigned int *__restrict__) Arg->FramePower;
        unsigned int *__restrict__ Mel_Spectr     = (unsigned int *__restrict__) Arg->MelSpectr;
        signed char  *__restrict__ shift_buff     = (signed char *__restrict__)  Arg->shift_buff;
        signed char  *__restrict__ shift_fft      = (signed char *__restrict__)  Arg->shift_fft;
        short int    *__restrict__ Mel_Coeffs     = (short int *__restrict__)    Arg->Mel_Coeffs;
        fbank_type_t *__restrict__ Mel_FilterBank = (fbank_type_t *__restrict__) Arg->Mel_FilterBank;
        unsigned int               Mel_NBanks     = (unsigned int) Arg->Mel_NBanks;
        short int                  Mel_Coeff_Dyn  = Arg->Mel_Coeff_dyn;
        signed char IsMagSquared = Arg->IsMagSquared;
        int MUL_EXP = IsMagSquared?2:1;
  
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        //Chunk = ChunkSize(Mel_NBanks);
        //First = CoreId*Chunk; Last = Min(First + Chunk, Mel_NBanks);
        //for (i=First; i<(unsigned int)Last; i++) {

        int min_shift;
        short int shift;
        First = CoreId; int nb_cores = gap_ncore();
        for (i=First; i< Mel_NBanks; i+=nb_cores) {
                unsigned int Coeff = 0;
                int Base = Mel_FilterBank[i].Base;
                int NonZeroItems = Mel_FilterBank[i].Items;
                int LogNItems = gap_fl1(NonZeroItems);

                int maxin=0;
                min_shift = 0x7fff;
                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        if (shift_fft[j] < min_shift) min_shift = shift_fft[j];
                }
                // align the block scaling on the min , compute the max value in the block
                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++) {
                        unsigned int TMP = FramePower[j] >> (MUL_EXP * (shift_fft[j] - min_shift));
                        if (TMP > (unsigned int) maxin) maxin = TMP;
                }

                // compute shift right to apply on the block to prevent overflow
                shift  = 0 ;
                short int shift0 = maxin?gap_fl1(maxin):0;

                // check multiply overflow conditionQ18.14 * Q6.10 => Q8.24
                if ((shift0+Mel_Coeff_Dyn+LogNItems) > 31) shift = shift0+Mel_Coeff_Dyn+LogNItems-31; else shift = 0;

                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        if ((shift + MUL_EXP * (shift_fft[j] - min_shift)) < 32)
                                Coeff = Coeff + Mel_Coeffs[Base+k]*(FramePower[j]>>(shift + MUL_EXP * (shift_fft[j] - min_shift)));
                }
                // update the shift left  value on the block (to compensate)
                shift_buff[i] = shift - MUL_EXP * min_shift;
                Mel_Spectr[i] = Coeff?Coeff:1; // Avoid 0s for log
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_melfilterbank = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%u, ", Mel_Spectr[i]);
                printf("])\nshift_BF = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%d, ", shift_buff[i]); printf("])\n");
        }
        gap_waitbarrier(0);
#endif
}

void MelFilterBank_f16(MelFilterBank_T *Arg)
{
        unsigned int i, j, k;
        F16_DSP *__restrict__ FramePower = (F16_DSP *__restrict__) Arg->FramePower;
        F16_DSP *__restrict__ Mel_Spectr = (F16_DSP *__restrict__) Arg->MelSpectr;
        F16_DSP *__restrict__ Mel_Coeffs = (F16_DSP *__restrict__) Arg->Mel_Coeffs;
        fbank_type_t *__restrict__ Mel_FilterBank = (fbank_type_t *__restrict__) Arg->Mel_FilterBank;
        unsigned int               Mel_NBanks     = (unsigned int) Arg->Mel_NBanks;

        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        //Chunk = ChunkSize(Mel_NBanks);
        //First = CoreId*Chunk; Last = Min(First + Chunk, Mel_NBanks);
        //for (i=First; i<(unsigned int)Last; i++) {

        First = CoreId; int nb_cores = gap_ncore();
        for (i=First; i< Mel_NBanks; i+=nb_cores) {
                F16_DSP Coeff = 0;
                int Base = Mel_FilterBank[i].Base;
                int NonZeroItems = Mel_FilterBank[i].Items;
                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        Coeff = Coeff + Mel_Coeffs[Base+k]*FramePower[j];
                }
                Mel_Spectr[i] = Coeff; // Avoid 0s for log
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_melfilterbank = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%e, ", Mel_Spectr[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MelFilterBank_f32(MelFilterBank_T *Arg)
{
        unsigned int i, j, k;

        float *__restrict__ FramePower = (float *__restrict__) Arg->FramePower;
        float *__restrict__ Mel_Spectr = (float *__restrict__) Arg->MelSpectr;
        float *__restrict__ Mel_Coeffs = (float *__restrict__) Arg->Mel_Coeffs;
        fbank_type_t *__restrict__ Mel_FilterBank = (fbank_type_t *__restrict__) Arg->Mel_FilterBank;
        unsigned int               Mel_NBanks     = (unsigned int) Arg->Mel_NBanks;

        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        //Chunk = ChunkSize(Mel_NBanks);
        //First = CoreId*Chunk; Last = Min(First + Chunk, Mel_NBanks);
        //for (i=First; i<(unsigned int)Last; i++) {

        First = CoreId; int nb_cores = gap_ncore();
        for (i=First; i< Mel_NBanks; i+=nb_cores) {
                float Coeff = 0;
                int Base = Mel_FilterBank[i].Base;
                int NonZeroItems = Mel_FilterBank[i].Items;
                for (k=0, j=Mel_FilterBank[i].Start; k<(unsigned int) NonZeroItems; j++, k++){
                        Coeff = Coeff + Mel_Coeffs[Base+k]*FramePower[j];
                }
                Mel_Spectr[i] = Coeff; // Avoid 0s for log
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_melfilterbank = np.array([\n\t"); for (i=0; i<Mel_NBanks; i++) printf("%e, ", Mel_Spectr[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeLog_Fix32(MFCC_Log_T *Arg)
{
        unsigned int i;
        int         size      = Arg->FrameSize;
        unsigned int *frameIn = (unsigned int *) Arg->FrameIn;
        short int *frameOut = (short int *) Arg->FrameOut;
        short int Norm = Arg->Norm;
        signed char *shift_buff = Arg->shift_buff;
        int         ExtraQ      = Arg->ExtraQ;
        int         FFT_QFormat = Arg->Q_FFT_Out;
        int         Mel_Coeff_Dyn = Arg->Mel_Coeff_Dyn;
        int         LogOffset = Arg->LogOffset; // Q30

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (Arg->IsMagSquared) {
                if (LogOffset){
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-shift_buff[i]+2*FFT_QFormat+2*ExtraQ); //POWER HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                } else  {
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-shift_buff[i]+2*FFT_QFormat+2*ExtraQ); //POWER HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                }
        } else {
                if (LogOffset) {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                } else {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                }
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nNorm = %d\nout_log = np.array([\n", Norm); for(i=0;i<size;i++) printf("%d, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeLog_Fix32_Scal(MFCC_Log_T *Arg)
{
        unsigned int i;
        int         size      = Arg->FrameSize;
        unsigned int *frameIn = (unsigned int *) Arg->FrameIn;
        short int   *frameOut = (short int *) Arg->FrameOut;
        short int   Norm = Arg->Norm;
        signed char *shift_buff = Arg->shift_buff;
        int         ExtraQ      = Arg->ExtraQ;
        int         FFT_QFormat = Arg->Q_FFT_Out;
        int         Mel_Coeff_Dyn = Arg->Mel_Coeff_Dyn;
        int         LogOffset = Arg->LogOffset; // Q30

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (Arg->IsMagSquared) {
                if (LogOffset){
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                } else  {
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                }
        } else {
                if (LogOffset){
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                } else  {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(TMP - (Qformat-QNN) * LN_2_1F15, Norm), 15);
                        }
                }
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nNorm = %d\nout_log = np.array([\n", Norm); for(i=0;i<size;i++) printf("%d, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeLog_f16( MFCC_LogF_T *Arg)
{        
        unsigned int i;
        int size      = Arg->FrameSize;
        F16_DSP *frameIn  = (F16_DSP *) Arg->FrameIn;
        F16_DSP *frameOut = (F16_DSP *) Arg->FrameOut;
        float LogOffset = Arg->LogOffset;

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);


        if (LogOffset) {
                for (i=First;i<Last;i++){
                        frameOut[i] = (F16_DSP) fastlog((float) frameIn[i] + LogOffset);
                }
        } else {
                for (i=First;i<Last;i++){
                        frameOut[i] = (F16_DSP) fastlog((float) (frameIn[i]>1e-6?frameIn[i]:1e-6));
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_log = np.array([\n"); for(i=0;i<size;i++) printf("%f, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeLog_f32(MFCC_LogF_T *Arg) 
{
        unsigned int i;
        int   size      = Arg->FrameSize;
        float *frameIn  = (float *) Arg->FrameIn;
        float *frameOut = (float *) Arg->FrameOut;
        float LogOffset = Arg->LogOffset;

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (LogOffset) {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) fastlog((float) frameIn[i] + LogOffset);
                }
        } else {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) fastlog((float) (frameIn[i]>1e-6?frameIn[i]:1e-6));
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_log = np.array([\n"); for(i=0;i<size;i++) printf("%f, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDB_Fix32(MFCC_Log_T *Arg)
{
        unsigned int i;
        int         size      = Arg->FrameSize;
        unsigned int *frameIn = (unsigned int *) Arg->FrameIn;
        short int   *frameOut = (short int *) Arg->FrameOut;
        short int Norm = Arg->Norm;
        signed char *shift_buff = Arg->shift_buff;
        int         ExtraQ      = Arg->ExtraQ;
        int         FFT_QFormat = Arg->Q_FFT_Out;
        int         Mel_Coeff_Dyn = Arg->Mel_Coeff_Dyn;
        int         LogOffset = Arg->LogOffset; // Q30

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (Arg->IsMagSquared) {
                if (LogOffset){
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                } else  {
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                }
        } else {
                if (LogOffset) {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                } else {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                }
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nNorm = %d\nout_log = np.array([\n", Norm); for(i=0;i<size;i++) printf("%d, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDB_Fix32_Scal(MFCC_Log_T *Arg)
{
        unsigned int i;
        int         size      = Arg->FrameSize;
        unsigned int *frameIn = (unsigned int *) Arg->FrameIn;
        short int   *frameOut = (short int *) Arg->FrameOut;
        short int Norm = Arg->Norm;
        signed char *shift_buff = Arg->shift_buff;
        int         ExtraQ      = Arg->ExtraQ;
        int         FFT_QFormat = Arg->Q_FFT_Out;
        int         Mel_Coeff_Dyn = Arg->Mel_Coeff_Dyn;
        int         LogOffset = Arg->LogOffset; // Q30

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (Arg->IsMagSquared) {
                if (LogOffset){
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                } else  {
                        for (i=First;i<Last;i++){
                                int Qformat =  (Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ); //POWER HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                }
        } else {
                if (LogOffset) {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                unsigned int Log_offset = ((30-Qformat)<=0)?LogOffset << (-30+Qformat):LogOffset >> (30-Qformat);
                                TMP = ulogn_17_15(frameIn[i] + Log_offset);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                } else {
                        for (i=First;i<Last;i++){
                                int Qformat = (30 - shift_buff[i]); //Abs HIGH_PREC
                                TMP = ulogn_17_15(frameIn[i]);
                                frameOut[i] = (short int) gap_clip(AT_NORM(10 * ((TMP * LN_10_INV_Q10 >> 10) - (Qformat-QNN) * LOG10_2), Norm), 15);
                        }
                }
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nNorm = %d\nout_log = np.array([\n", Norm); for(i=0;i<size;i++) printf("%d, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDB_f16( MFCC_LogF_T *Arg)
{        
        unsigned int i;
        int size      = Arg->FrameSize;
        F16_DSP *frameIn  = (F16_DSP *) Arg->FrameIn;
        F16_DSP *frameOut = (F16_DSP *) Arg->FrameOut;
        float LogOffset = Arg->LogOffset;

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (LogOffset) {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) 10*log10f((float) frameIn[i] + LogOffset);
                }
        } else {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) 10*log10f((float) (frameIn[i]>1e-6?frameIn[i]:1e-6));
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_log = np.array([\n"); for(i=0;i<size;i++) printf("%f, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDB_f32(MFCC_LogF_T *Arg) 
{
        unsigned int i;
        int    size     = Arg->FrameSize;
        float *frameIn  = (float *) Arg->FrameIn;
        float *frameOut = (float *) Arg->FrameOut;
        float LogOffset = Arg->LogOffset;

        // log is natural log in C
        int TMP;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(size);
        First = CoreId*Chunk; Last = Min(First + Chunk, size);

        if (LogOffset) {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) 10*log10f((float) frameIn[i] + LogOffset);
                }
        } else {
                for (i=First;i<Last;i++){
                        frameOut[i] = (float) 10*log10f((float) (frameIn[i]>1e-6?frameIn[i]:1e-6));
                }
        }
        gap_waitbarrier(0);

#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_log = np.array([\n"); for(i=0;i<size;i++) printf("%f, ", frameOut[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void norm_clip_16(Norm_Clip_args_T *Args)
{
        int       *__restrict__ In  = (int *__restrict__) Args->In;
        short int *__restrict__ Out = (short int *__restrict__) Args->Out;
        short int Norm = Args->Norm;
        int N = Args->N;
  
        unsigned int i;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();

        if (CoreId==0) {
                for (i=0; i<(unsigned int)N; i++) {
                        Out[i] = (short int) gap_clip(AT_NORM(In[i], Norm), 15);
                }
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_norm_clip = np.array([\n\t"); for (i=0; i<N; i++) printf("%d, ", Out[i]); printf("])\nNorm = %d\n", Norm);
        } gap_waitbarrier(0);
#endif
}

void norm_clip_32_melspect(MFCC_Clip_32_T *Args) {
        // Output is 32 bit integer in Q(QNN) fixed point
        unsigned int *In  = (unsigned int *__restrict__) Args->In;
        unsigned int *Out = (unsigned int *__restrict__) Args->Out;
        signed char *shift_buff = Args->shift_buff;
        int N = Args->N;

        unsigned int i, Norm;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        for (i=First; i<Last; i++) {
                int Qformat = (30 - shift_buff[i]);
                Norm = Qformat - 16; //POWER
                Out[i] = (Norm<32)?(In[i] >> Norm):0;
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_norm_clip = np.array([\n\t"); for (i=0; i<N; i++) printf("%d, ", Out[i]); printf("])\n");;
        }
#endif
}

void norm_clip_32_melspect_scal(MFCC_Clip_32_T *Args)
{
        // Output is 32 bit integer in Q(QNN) fixed point
        unsigned int *In  = (unsigned int *__restrict__) Args->In;
        unsigned int *Out = (unsigned int *__restrict__) Args->Out;
        int          ExtraQ     = Args->ExtraQ;
        int          Q_FFT_Out  = Args->Q_FFT_Out;
        int          Mel_Coeff_Dyn = Args->Mel_Coeff_Dyn;
        signed char *shift_buff = Args->shift_buff;
        int N = Args->N;
        signed char IsMagSquared = Args->IsMagSquared;

        int Norm;
        unsigned int Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(N);
        First = CoreId*Chunk; Last = Min(First + Chunk, N);

        if (IsMagSquared){
                for (unsigned int i=First; i<Last; i++) {
                        Norm = Mel_Coeff_Dyn-2-shift_buff[i]+2*ExtraQ - 16; //POWER HIGH_PREC
                        Out[i] = (Norm<32)?(In[i] >> Norm):0;
                }
        } else {
                for (unsigned int i=First; i<Last; i++) {
                        Norm = Mel_Coeff_Dyn-1-shift_buff[i]+ExtraQ - 16; //Abs HIGH_PREC
                        Out[i] = (Norm<32)?(In[i] >> Norm):0;
                }                        
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_norm_clip = np.array([\n\t"); for (unsigned int i=0; i<N; i++) printf("%d, ", Out[i]); printf("])\n");
        }
#endif
}

void MFCC_ComputeDCT_II_Fix16(DCT_II_Arg_T *Args)
{
        unsigned int i,k;
        v2s * in_dct   = (v2s * __restrict__ ) Args->Data;
        short int * DCTCoeff = (short int * __restrict__) Args->DCTCoeff;
        short int * FeatList = (short int * __restrict__ ) Args->FeatList;
        unsigned int NDCT  = (unsigned int) Args->n_dct;
        unsigned int NInputs = (unsigned int) Args->n_input;

        unsigned int Chunk, First, Last, CoreId=gap_coreid();

        // last dct stage
        Chunk = ChunkSize(NInputs);
        First = CoreId*Chunk; Last = Min(First + Chunk, NInputs);
        for (k=First; k<Last; k++) {
                int Acc = 0;
                v2s *DCT_Table = (v2s *) &DCTCoeff[k*NDCT];
                for (i=0; i<NDCT/4; i++){
                        Acc = gap_sumdotp2(in_dct[2*i],   DCT_Table[2*i],   Acc);
                        Acc = gap_sumdotp2(in_dct[2*i+1], DCT_Table[2*i+1], Acc);
                }
                for (i=NDCT/4*4; i<NDCT; i++) Acc += ((short int *) in_dct)[i] * ((short int *) DCTCoeff)[k*NDCT+i];
                FeatList[k] = (short int) gap_clip(AT_NORM(Acc, 14), 15);
        }
        gap_waitbarrier(0);
#ifdef PRINTDEB
        if (CoreId==0) {
                printf("\nout_dct_ii = np.array([\n"); for (int i=0; i<NDCT; i++) printf("%d, ", FeatList[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDCT_II_f16(DCT_II_Arg_T *Args)
{
        unsigned int i,k;
        F16V_DSP * in_dct  = (F16V_DSP * __restrict__ ) Args->Data;
        F16_DSP * FeatList = (F16_DSP * __restrict__ ) Args->FeatList;
        F16_DSP * DCTCoeff = (F16_DSP * __restrict__) Args->DCTCoeff;
        unsigned int NDCT  = (unsigned int) Args->n_dct;
        unsigned int NInputs = (unsigned int) Args->n_input;

        unsigned int Chunk, First, Last, CoreId=gap_coreid();

        // last dct stage
        Chunk = ChunkSize(NInputs);
        First = CoreId*Chunk; Last = Min(First + Chunk, NInputs);
        for (k=First; k<Last; k++) {
                F16_DSP Acc = 0;
                F16V_DSP *DCT_Table = (F16V_DSP *) &DCTCoeff[k*NDCT];
                for (i=0; i<NDCT/4; i++){
                        F16V_DSP A = in_dct[2*i]   * DCT_Table[2*i]; 
                        F16V_DSP B = in_dct[2*i+1] * DCT_Table[2*i+1]; 
                        Acc += A[0] + A[1] + B[0] + B[1];
                }
                for (i=NDCT/4*4; i<NDCT; i++) Acc += ((F16_DSP *) in_dct)[i] * ((F16_DSP *) DCTCoeff)[k*NDCT+i];
                FeatList[k] = Acc;
        }
        gap_waitbarrier(0);
#if PRINTDEB
        if (CoreId==0) {
                printf("\nout_dct_ii = np.array([\n"); for (int i=0; i<NDCT; i++) printf("%f, ", FeatList[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_ComputeDCT_II_f32(DCT_II_Arg_T *Args)
{
        unsigned int i,k;
        float * in_dct   = (float * __restrict__ ) Args->Data;
        float * FeatList = (float * __restrict__ ) Args->FeatList;
        float * DCTCoeff = (float * __restrict__) Args->DCTCoeff;
        unsigned int NDCT  = (unsigned int) Args->n_dct;
        unsigned int NInputs = (unsigned int) Args->n_input;

        unsigned int Chunk, First, Last, CoreId=gap_coreid();

        // last dct stage
        Chunk = ChunkSize(NInputs);
        First = CoreId*Chunk; Last = Min(First + Chunk, NInputs);
        for (k=First; k<Last; k++) {
                float Acc = 0;
                for (i=0; i<NDCT/2; i++){
                        Acc += in_dct[2*i]   * DCTCoeff[2*i  +k*NDCT];
                        Acc += in_dct[2*i+1] * DCTCoeff[2*i+1+k*NDCT];
                }
                if (NDCT&0x1) Acc += in_dct[NDCT-1] * DCTCoeff[NDCT-1];
                FeatList[k] = Acc;
        }
        gap_waitbarrier(0);
#if PRINTDEB
        if (CoreId==0) {
          printf("\nout_dct_ii = np.array([\n"); for (int i=0; i<NDCT; i++) printf("%f, ", FeatList[i]); printf("])\n");
        } gap_waitbarrier(0);
#endif
}

void MFCC_Lifter_Fix16(Lifter_Arg_T *Args)
{
        short int * FeatList = (short int * __restrict__ ) Args->FeatList;
        short int * LiftCoeff = (short int * __restrict__ ) Args->LiftCoeff;
        int FrameSize        = Args->FrameSize;
        // LIFTER LiftCoeff in Q11
        unsigned int j, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (j=First; j<Last; j++) FeatList[j] = gap_mulsRN(FeatList[j], LiftCoeff[j], 11);
        gap_waitbarrier(0);
}

void MFCC_Lifter_f16(Lifter_Arg_T *Args)
{
        F16_DSP * FeatList  = (F16_DSP * __restrict__ ) Args->FeatList;
        F16_DSP * LiftCoeff = (F16_DSP * __restrict__ ) Args->LiftCoeff;
        int FrameSize   = Args->FrameSize;
        unsigned int j, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (j=First; j<Last; j++) FeatList[j] = FeatList[j] * LiftCoeff[j];
        gap_waitbarrier(0);
}

void MFCC_Lifter_f32(Lifter_Arg_T *Args)
{
        float * FeatList  = (float * __restrict__ ) Args->FeatList;
        float * LiftCoeff = (float * __restrict__ ) Args->LiftCoeff;
        int FrameSize   = Args->FrameSize;
        unsigned int j, Chunk, First, Last, CoreId=gap_coreid();
        Chunk = ChunkSize(FrameSize);
        First = CoreId*Chunk; Last = Min(First + Chunk, FrameSize);

        for (j=First; j<Last; j++) FeatList[j] = FeatList[j] * LiftCoeff[j];
        gap_waitbarrier(0);
}
